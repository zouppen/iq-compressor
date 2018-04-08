/* -*- mode: c; c-file-style: "linux" -*-
 *
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <err.h>

int main(int argc, char **argv)
{
	while (true) {
		// Get a byte from stream
		float sample_float;
		if (fread(&sample_float, sizeof(float), 1, stdin) != 1) {
			// Error checks
			if (feof(stdin)) {
				// Stop gracefully if EOF.
				break;
			} else {
				// Something more evil happened.
				err(1,"Unable to read stdin");
			}
		}

		// Type conversion
		int16_t sample_s16 = sample_float * 0xffff;
		printf("%f\t%d\n", sample_float, sample_s16);
	}
	return 0;
}
