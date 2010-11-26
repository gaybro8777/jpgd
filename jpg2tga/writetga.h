//------------------------------------------------------------------------------
// File: writetga.h
// Simple TGA writer -- handles 24-bit truecolor, 8-bit greyscale
// Public domain, Rich Geldreich <richgel99@gmail.com>
//------------------------------------------------------------------------------
#ifndef WRITETGA_H
#define WRITETGA_H
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
//------------------------------------------------------------------------------
typedef unsigned char uint8;
//------------------------------------------------------------------------------
typedef enum
{
  TGA_IMAGE_TYPE_NULL = 0,
  TGA_IMAGE_TYPE_BGR,
  TGA_IMAGE_TYPE_GREY,
} tga_image_type_t;
//------------------------------------------------------------------------------
class tga_writer
{
  FILE *Pfile;
  int width, height;
  int bytes_per_pixel, bytes_per_line;
  tga_image_type_t image_type;

public:

  tga_writer();

  ~tga_writer();

  bool open(const char *Pfilename,
            int width, int height,
            tga_image_type_t image_type);

  bool close(void);

  bool write_line(const void *Pscan_line);
};
//------------------------------------------------------------------------------
typedef tga_writer *tga_writer_ptr_t;
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

