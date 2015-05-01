Note: This project is now here for mostly historical purposes. For the latest version of jpgd, see my public domain [jpeg-compressor](http://code.google.com/p/jpeg-compressor/) project, which includes the latest version of jpgd along with jpge, a full JPEG compressor in a single small source file.


---


jpgd v0.96 is a reasonably fast and simple public domain [JPEG](http://en.wikipedia.org/wiki/Jpeg) decoder class written in C++. Unlike most "small" (non-[IJG](http://www.ijg.org/)) JPEG decompressor implementations, this class supports both progressive and sequential files and upsamples the chrominance channels of [chroma subsampled](http://en.wikipedia.org/wiki/Chroma_subsampling) images in the frequency domain.

This class makes use of the equations presented in this paper:
["Fast Scheme for Image Size Change in the Compressed Domain"](http://www-sipl.technion.ac.il/new/Teaching/Projects/Winter2006/2001%20-%20A%20Fast%20Scheme%20for%20Image%20Size%20Change%20in%20the%20Compressed%20Domain.pdf) by Dugad and Ahuja, IEEE Transactions On Circuits and Systems for Video Technology, Vol. 11, No. 4, April 2001.

Includes Visual Studio 2005 projects for Win32/Win64. The GCC port (which should be straightforward) is coming soon.

Some alternatives to this library are:
  * Martin Fiedler's [NanoJPEG](http://keyj.s2000.ws/?p=137) JPEG reader.
  * Sean Barrett's [stb\_image.c](http://nothings.org/stb_image.c) public domain JPEG/PNG reader. (The source distribution actually includes stb\_image.c for comparison purposes.)

Also, if you're looking for a plain C decompressor for 8/16-bit microcontrollers that uses very little (~2.3KB) RAM, check out my [picojpeg](http://code.google.com/p/picojpeg/) project. picojpeg is a distant relative of jpgd.

**Testing**
  * The source distribution includes a simple example command line tool called "jpg2tga" that converts JPEG images to TGA using the jpeg\_decoder class. It's usage is:

```
  bin/jpg2tga source_file.jpg destination_file.tga
```

Use the -stb option to use stb\_image.c to decompress the image instead of the jpeg\_decoder class:

```
  bin/jpg2tga -stb source_file.jpg destination_file.tga
```

**Support Contact**

For any questions or problems with this class please contact Rich Geldreich at <richgel99 at gmail.com>