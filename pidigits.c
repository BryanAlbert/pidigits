/*
 * Computation of the n'th decimal digit of pi with very little memory.
 * Written by Fabrice Bellard on February 26, 1997.
 *
 * We use a slightly modified version of the method described by Simon
 * Plouffe in "On the Computation of the n'th decimal digit of various
 * transcendental numbers" (November 1996). We have modified the algorithm
 * to get a running time of O(n^2) instead of O(n^3log(n)^3).
 *
 * This program uses a variation of the formula found by Gosper in 1974 :
 *
 * pi = sum((25*n-3)/(binomial(3*n,n)*2^(n-1)), n=0..infinity);
 *
 * This program uses mostly integer arithmetic. It may be slow on some
 * hardware where integer multiplication and divison must be done by
 * software. We have supposed that 'int' has a size of at least 32 bits. If
 * your compiler supports 'long long' integers of 64 bits, you may use the
 * integer version of 'mul_mod' (see HAS_LONG_LONG).
 * 
 * From https://bellard.org/pi/pi1.c, see https://bellard.org/pi/
 * Refactored and modified to display 10 digits instead of 9 (which seems
 * accurate?), to time the computation and to show a rough progress bar.
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys\timeb.h>


/* uncomment the following line to use 'long long' integers (which we do) */
#define HAS_LONG_LONG

#ifdef HAS_LONG_LONG
#define mul_mod(a, b, m) (((long long)(a) * (long long)(b)) % (m))
#else
#define mul_mod(a, b, m) fmod((double)a *(double)b, m)
#endif

#define DIVN(t, a, v, vinc, kq, kqinc) \
{													\
	kq += kqinc;								\
	if (kq >= a)								\
	{												\
		while (kq >= a)						\
			kq -= a;								\
		if (kq == 0)							\
		{											\
			do										\
			{										\
				t /= a;							\
				v += vinc;						\
			}										\
			while (t % a == 0);				\
		}											\
	}												\
}


/* return the inverse of x mod y */
int inv_mod(int x, int y)
{
	int q, t;

	int u = x;
	int v = y;
	int c = 1;
	int a = 0;

	do
	{
		q = v / u;

		t = c;
		c = a - q * c;
		a = t;

		t = u;
		u = v - q * u;
		v = t;
	}
	while (u != 0);

	a %= y;
	if (a < 0)
		a = y + a;

	return a;
}

/* return the inverse of u mod v, if v is odd */
int inv_mod2(int u, int v)
{
	int t1, t3;

	int u1 = 1;
	int u3 = u;
	int v1 = v;
	int v3 = v;

	if ((u & 1) != 0)
	{
		t1 = 0;
		t3 = -v;
		goto Y4;
	}
	else
	{
		t1 = 1;
		t3 = u;
	}

	do
	{
		do
		{
			if ((t1 & 1) == 0)
			{
				t1 = t1 >> 1;
				t3 = t3 >> 1;
			}
			else
			{
				t1 = (t1 + v) >> 1;
				t3 = t3 >> 1;
			}

		Y4:;
		}
		while ((t3 & 1) == 0);

		if (t3 >= 0)
		{
			u1 = t1;
			u3 = t3;
		}
		else
		{
			v1 = v - t1;
			v3 = -t3;
		}

		t1 = u1 - v1;
		t3 = u3 - v3;
		if (t1 < 0)
			t1 = t1 + v;
	}
	while (t3 != 0);

	return u1;
}

/* return (a^b) mod m */
int pow_mod(int a, int b, int m)
{
	int r = 1;
	int aa = a;

	while (1)
	{
		if (b & 1)
			r = mul_mod(r, aa, m);
	
		b >>= 1;
		if (b == 0)
			break;
	
		aa = mul_mod(aa, aa, m);
	}

	return r;
}

/* return true if n is prime */
int is_prime(int n)
{
	int r, i;

	if (n % 2 == 0)
		return 0;

	r = (int) sqrt(n);
	for (i = 3; i <= r; i += 2)
		if (n % i == 0)
			return 0;

	return 1;
}

/* return the prime number immediatly after n */
int next_prime(int n)
{
	while (!is_prime(++n));
	return n;
}

char * get_ending(int number, char * ending)
{
	int teen = number % 100;
	if (teen == 11 || teen == 12 || teen == 13)
	{
		 strcpy_s(ending, 3, "th");
	}
	else
	{
		switch (number % 10)
		{
			case 1:
				strcpy_s(ending, 3, "st");
				break;
			case 2:
				strcpy_s(ending, 3, "nd");
				break;
			case 3:
				strcpy_s(ending, 3, "rd");
				break;
			default:
				strcpy_s(ending, 3, "th");
				break;
		}
	}

	return ending;
}

int main(int argc, char* argv[])
{
	int av, a, vmax, N, n, num, den, k, kq1, kq2, kq3, kq4, t, v, s, i;
	double sum;

	if (argc < 2 || (n = atoi(argv[1])) <= 0)
	{
		printf("Usage: pi.exe n\n");
		printf("\nn   The 0-based digit of pi to compute\n");
		printf("\nComputes ten digits of pi at the specified digit.\n");
		return 1;
	}

	N = (int) ((n + 20) * log(10) / log(13.5));
	sum = 0;

	char ending[3] = "th";
	printf("Computing the %d'%s digit of pi...\n", n, get_ending(n, ending));
	struct timeb start_time, end_time;
	ftime(&start_time);

	int progress = 0;
	printf(".....................................................................................................................\r");

	for (a = 2; a <= 3 * N; a = next_prime(a))
	{
		vmax = (int) (log(3 * N) / log(a));
		if (a == 2)
		{
			vmax += N - n;
			if (vmax <= 0)
				continue;
		}

		av = 1;
		for (i = 0; i < vmax; i++)
			av *= a;

		s = 0;
		den = 1;
		kq1 = 0;
		kq2 = -1;
		kq3 = -3;
		kq4 = -2;
		
		if (a == 2)
		{
			num = 1;
			v = -n;
		}
		else
		{
			num = pow_mod(2, n, av);
			v = 0;
		}

		for (k = 1; k <= N; k++)
		{
			t = 2 * k;
			DIVN(t, a, v, -1, kq1, 2);
			num = mul_mod(num, t, av);

			t = 2 * k - 1;
			DIVN(t, a, v, -1, kq2, 2);
			num = mul_mod(num, t, av);

			t = 3 * (3 * k - 1);
			DIVN(t, a, v, 1, kq3, 9);
			den = mul_mod(den, t, av);

			t = 3 * k - 2;
			DIVN(t, a, v, 1, kq4, 3);
			if (a != 2)
				t *= 2;
			else
				v++;
		
			den = mul_mod(den, t, av);

			if (v > 0)
			{
				t = a != 2 ? inv_mod2(den, av) : inv_mod(den, av);
				t = mul_mod(t, num, av);
				for (i = v; i < vmax; i++)
					t = mul_mod(t, a, av);

				t = mul_mod(t, 25 * k - 3, av);
				s += t;
				if (s >= av)
					s -= av;
			}
		}

		t = pow_mod(5, n - 1, av);
		s = mul_mod(s, t, av);
		sum = fmod(sum + (double) s / av, 1.0);
		for (; progress < 40 * a / N; progress++)
			printf("*");
	}

	printf("\nDecimal digits of pi at position %d: %010lld\n", n, (long long) (sum * 1e10));

	ftime(&end_time);
	double seconds = (int) (end_time.time - start_time.time) + (end_time.millitm - start_time.millitm) / 1000.0;
	printf("Time to compute: %d:%02d.%03d\n", (int) seconds / 60, (int) seconds % 60, (int) ((seconds - floor(seconds)) * 1000 + 0.5));
	printf("Largest prime computed: %d\n", a);
	return 0;
}
