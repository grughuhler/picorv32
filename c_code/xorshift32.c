/* xorshift32 is a pseudorandom number generator with a period
   of 2**32-1.  It is likely not a very good generator from a
   statistical point of view, but it avoids multiplies.  See
   wikipedia "xorshift".

   state must be initialized non-zero.
*/

unsigned int xorshift32(unsigned int *state)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	unsigned int x = *state;
	
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	
	return *state = x;
}
