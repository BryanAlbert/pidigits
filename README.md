# pidigits.c

Computation of the n'th decimal digit of pi with very little memory.
Written by Fabrice Bellard on February 26, 1997.

Code at https://bellard.org/pi/pi1.c, see https://bellard.org/pi/ for 
discussion.

## From the pi.c file comment
> We use a slightly modified version of the method described by Simon
Plouffe in "On the Computation of the n'th decimal digit of various
transcendental numbers" (November 1996). We have modified the algorithm
to get a running time of O(n^2) instead of O(n^3log(n)^3).

> This program uses a variation of the formula found by Gosper in 1974 :
pi = sum((25*n-3)/(binomial(3*n,n)*2^(n-1)), n=0..infinity);

> This program uses mostly integer arithmetic. It may be slow on some
hardware where integer multiplication and divison must be done by
software. We have supposed that 'int' has a size of at least 32 bits. If
your compiler supports 'long long' integers of 64 bits, you may use the
integer version of 'mul_mod' (see HAS_LONG_LONG).

> From https://bellard.org/pi/pi1.c, see https://bellard.org/pi/
Refactored and modified to display 10 digits instead of 9 (which seems
accurate?), to time the computation and to show a rough progress bar.

## Changes and Features
* HAS_LONG_LONG is defined to use long long type for mul_mod calculation 
on 64 bit machines
* Refactored quite a bit
* Displays 10 digits instead of 9 (which seems accurate)
* Implements a rough progress bar and a timer for longer calculations
