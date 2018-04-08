/* -*- mode: c; c-file-style: "linux" -*-
 *
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <err.h>
#include "FLAC/metadata.h"
#include "FLAC/stream_encoder.h"

#define CHECK(CONDITION, EXIT, MSG...) if (!(CONDITION)) { errx(EXIT, MSG); } else {}

// Prototypes
static bool append_comment(FLAC__StreamMetadata *metadata, const char *k, const char *v);
static void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written, FLAC__uint64 samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data);

static bool append_comment(FLAC__StreamMetadata *metadata, const char *k, const char *v)
{
	FLAC__StreamMetadata_VorbisComment_Entry entry;
	return (FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, k, v) &&
		FLAC__metadata_object_vorbiscomment_append_comment(metadata, entry, false));
}

void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written, FLAC__uint64 samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data)
{
	(void)encoder, (void)client_data;

	fprintf(stderr, "wrote %lu bytes, %lu samples, %u frames\n", bytes_written, samples_written, frames_written);
}


int main(int argc, char **argv)
{
	// Initialize FLAC encoder
	FLAC__StreamEncoder *encoder = 0;
	FLAC__StreamMetadata *metadata[2];
	encoder = FLAC__stream_encoder_new();
	CHECK(encoder != NULL, 3, "Error allocating FLAC encoder");
	CHECK(FLAC__stream_encoder_set_verify(encoder, true),
	      3, "Error setting FLAC verify parameter");
	CHECK(FLAC__stream_encoder_set_compression_level(encoder, 5),
	      3, "Error setting FLAC compression level");
	CHECK(FLAC__stream_encoder_set_channels(encoder, 2),
	      3, "Error setting FLAC channel count");
	CHECK(FLAC__stream_encoder_set_bits_per_sample(encoder, 16),
	      3, "Error setting FLAC bits per sample");
	// Prepare metadata
	metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);
	metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING);
	CHECK(metadata[0] != NULL && metadata[1] != NULL,
	      3, "Error creating FLAC metadata");
	metadata[1]->length = 1024; // set the padding length
	// Some IQ data specific entries
	CHECK(append_comment(metadata[0], "FREQUENCY", "145000000"),
	      3, "Error adding metadata");
	// Encoder supports only specific sample rates, so ignore the
	// sample rate and write correct sample rate to comment
	// field. It won't affect compression rates at all.
	CHECK(append_comment(metadata[0], "IQ_SAMPLE_RATE", "2000000"),
	      3, "Error adding metadata");
	CHECK(FLAC__stream_encoder_set_metadata(encoder, metadata, 2),
	      3, "Unable to set metadata");
	// Initialize encoder
	FLAC__StreamEncoderInitStatus init_status;
	init_status = FLAC__stream_encoder_init_file(encoder, argv[1], progress_callback, NULL);
	CHECK(init_status == FLAC__STREAM_ENCODER_INIT_STATUS_OK,
	      3, "FLAC initialization failed: %s",
	      FLAC__StreamEncoderInitStatusString[init_status]);

	off_t file_location = 0;
	
	while (true) {
		// Get a byte from stream
		float sample_float[2];
		if (fread(sample_float, sizeof(float), 2, stdin) != 2) {
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
		if (sample_float[0] < -1 || sample_float[0] > 1 ||
		    sample_float[1] < -1 || sample_float[1] > 1) {
			errx(2, "IQ samples must be between -1 and 1, inclusive. Got (%f,%f) at %ld", sample_float[0], sample_float[1], file_location);
		}

		// Type conversion
		FLAC__int32 sample[2];
		sample[0] = sample_float[0] * 0xffff;
		sample[1] = sample_float[1] * 0xffff;
		FLAC__stream_encoder_process_interleaved(encoder, sample, 1);
		
		file_location += sizeof(float) * 2;
	}

	CHECK(FLAC__stream_encoder_finish(encoder), 3, "FLAC encoding unfinished");
	return 0;
}
