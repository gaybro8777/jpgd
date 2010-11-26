//------------------------------------------------------------------------------
// jpg2tga.cpp
// JPEG to TGA file conversion example program.
// Public domain, Rich Geldreich <richgel99@gmail.com>
// Last updated Nov. 26, 2010
//------------------------------------------------------------------------------
#include "jpegdecoder.h"
#include "writetga.h"
#include "timer.h"
#include "stb_image.c"
//------------------------------------------------------------------------------
// Uncomment to disable output file writing (for VTune benchmarking)
//#define NO_OUTPUT
//------------------------------------------------------------------------------
static int print_usage()
{
	printf("Usage: jpg2tga <-stb> [source_file] [dest_file]\n");
	printf("Outputs greyscale and truecolor 24-bit TGA files.\n");
	printf("If -stb option is specified, stb_image.c is used to load the image instead.\n");
	return EXIT_FAILURE;
}
//------------------------------------------------------------------------------
int main(int arg_c, char *arg_v[])
{
  printf("JPEG to TGA file conversion example. Compiled %s %s\n", __DATE__, __TIME__);
    
  if ((arg_c != 3) && (arg_c != 4))
		return print_usage();
  	
	int n = 1;
	bool bUseSTB = false;
	if (arg_v[1][0] == '-')
	{
		bUseSTB = (strcmp(arg_v[1], "-stb") == 0);
		if (!bUseSTB)
			return print_usage();
		n++;
	}

  const char *pSrc_filename = arg_v[n++];
  const char *pDst_filename = arg_v[n++];

  printf("Source file:      \"%s\"\n", pSrc_filename);
  printf("Destination file: \"%s\"\n", pDst_filename);

  jpeg_decoder_file_stream_ptr_t pInput_stream = NULL;
	jpeg_decoder_ptr_t pDecoder = NULL;
	int width = 0, height = 0, num_comps = 0;
	unsigned char* pImage_data = NULL;
	double total_decomp_time = 0.0f;
	timer t;

	if (bUseSTB)
	{
		t.start();
		pImage_data = stbi_load(pSrc_filename, &width, &height, &num_comps, 3);
		total_decomp_time = t.get_elapsed_ms();

		if (!pImage_data)
		{
			printf("Failed loading image: %s\n", pSrc_filename);
			return EXIT_FAILURE;
		}

		num_comps = 3;
	}
	else
	{
		pInput_stream = new jpeg_decoder_file_stream();

		if (pInput_stream->open(pSrc_filename))
		{
			delete pInput_stream;
			printf("Error: Unable to open file \"%s\" for reading!\n", pSrc_filename);
			return EXIT_FAILURE;
		}
		
		pDecoder = new jpeg_decoder(pInput_stream);

		if (pDecoder->get_error_code() != 0)
		{
			printf("Error: Decoder failed! Error status: %i\n", pDecoder->get_error_code());

			// Always be sure to delete the input stream object _after_
			// the decoder is deleted. Reason: the decoder object calls the input
			// stream's detach() method.
			delete pDecoder;
			delete pInput_stream;

			return EXIT_FAILURE;
		}

		width = pDecoder->get_width();
		height = pDecoder->get_height();
		num_comps = pDecoder->get_num_components();
	}

#ifdef NO_OUTPUT
  tga_writer_ptr_t pTGA_writer = NULL;
#else
  tga_writer_ptr_t pTGA_writer = new tga_writer();

  bool status = pTGA_writer->open(pDst_filename, width, height, (num_comps == 1) ? TGA_IMAGE_TYPE_GREY : TGA_IMAGE_TYPE_BGR);

  if (status)
  {
    delete pDecoder;
    delete pInput_stream;
    delete pTGA_writer;
		stbi_image_free(pImage_data);

    printf("Error: Unable to open file \"%s\" for writing!\n", pDst_filename);

    return EXIT_FAILURE;
  }
#endif

  printf("Width: %i\n", width);
  printf("Height: %i\n", height);
  printf("Components: %i\n", num_comps);
			
	if (pDecoder)
	{
		t.start();
		if (pDecoder->begin())
		{
			printf("Error: Decoder failed! Error status: %i\n", pDecoder->get_error_code());

			delete pDecoder;
			delete pInput_stream;
			delete pTGA_writer;
			remove(pDst_filename);

			return EXIT_FAILURE;
		}
		t.stop();
		total_decomp_time += t.get_elapsed_ms();
	}

  uint8 *pScanline_buf = NULL;
  if (num_comps == 3)
  {
    pScanline_buf = (uint8 *)malloc(width * 3);
    if (!pScanline_buf)
    {
      printf("Error: Out of memory!\n");

      delete pDecoder;
      delete pInput_stream;
      delete pTGA_writer;
			stbi_image_free(pImage_data);
      remove(pDst_filename);

      return EXIT_FAILURE;
    }
  }
	
	const int bytes_per_pixel = pDecoder ? pDecoder->get_bytes_per_pixel() : num_comps;

  int lines_decoded = 0;

  for ( ; ; )
  {
    const void *pScan_line;
    uint scan_line_len;

		if (pDecoder)
		{
			t.start();
			if (pDecoder->decode(&pScan_line, &scan_line_len))
				break;
			total_decomp_time += t.get_elapsed_ms();
		}
		else
		{
			pScan_line = pImage_data + lines_decoded * width * num_comps;
			if (lines_decoded == height)
				break;
		}

    lines_decoded++;
		
#ifndef NO_OUTPUT
    bool status;

    if (num_comps == 3)
    {
      uint8 *Psb = (uint8 *)pScan_line;
      uint8 *Pdb = pScanline_buf;
      int src_bpp = bytes_per_pixel;

      for (int x = width; x > 0; x--, Psb += src_bpp, Pdb += 3)
      {
        Pdb[0] = Psb[2];
        Pdb[1] = Psb[1];
        Pdb[2] = Psb[0];
      }

      status = pTGA_writer->write_line(pScanline_buf);
    }
    else
    {
      status = pTGA_writer->write_line(pScan_line);
    }

    if (status)
    {
      printf("Error: Unable to write to file \"%s\"!\n", pDst_filename);

      free(pScanline_buf);
      delete pDecoder;
      delete pInput_stream;
      delete pTGA_writer;
			stbi_image_free(pImage_data);
      remove(pDst_filename);

      return EXIT_FAILURE;
    }
#endif
  }

	printf("Total decompress-only time: %f ms\n", total_decomp_time);

  free(pScanline_buf);
	pScanline_buf = NULL;

#ifndef NO_OUTPUT
  if (pTGA_writer->close())
  {
    printf("Error: Unable to write to file \"%s\"!\n", pDst_filename);

    delete pDecoder;
    delete pInput_stream;
    delete pTGA_writer;
		stbi_image_free(pImage_data);
    remove(pDst_filename);

    return EXIT_FAILURE;
  }

  delete pTGA_writer;
	pTGA_writer = NULL;
#endif

	stbi_image_free(pImage_data);
	pImage_data = NULL;
	
  if (pDecoder)
	{
		if (pDecoder->get_error_code())
		{
			printf("Error: Decoder failed! Error status: %i\n", pDecoder->get_error_code());

			delete pDecoder;
			delete pInput_stream;
			return EXIT_FAILURE;
		}

		printf("Input file size:  %i\n", pInput_stream->get_size());
		printf("Input bytes actually read: %i\n", pDecoder->get_total_bytes_read());
		
		delete pDecoder;
	}

	delete pInput_stream;
	pInput_stream = NULL;
	
	printf("Lines decoded: %i\n", lines_decoded);
  
  return EXIT_SUCCESS;
}
//------------------------------------------------------------------------------

