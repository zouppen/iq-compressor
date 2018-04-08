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
	off_t file_location = 0;
	
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

		// Validity checks
		if (sample_float < -1 || sample_float > 1) {
			errx(2, "IQ samples must be between -1 and 1, inclusive. Got %f at %ld", sample_float, file_location);
		}

		// Type conversion
		int16_t sample_s16 = sample_float * 0xffff;
		printf("%10ld %10f\t%6d\n", file_location, sample_float, sample_s16);

		file_location += sizeof(float);
	}
	return 0;
}
