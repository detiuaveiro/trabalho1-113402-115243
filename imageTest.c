// imageTest - A program that performs some image processing.
//
// This program is an example use of the image8bit module,
// a programming project for the course AED, DETI / UA.PT
//
// You may freely use and modify this code, NO WARRANTY, blah blah,
// as long as you give proper credit to the original and subsequent authors.
//
// João Manuel Rodrigues <jmr@ua.pt>
// 2023

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image8bit.h"
#include "instrumentation.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    error(1, 0, "Usage: imageTest input.pgm output.pgm");
  }

  ImageInit();
  
  printf("# LOAD image\n");
  InstrReset(); // to reset instrumentation
  Image img1 = ImageLoad(argv[1]);
  // Image img2 = ImageLoad(argv[2]);
  if (img1 == NULL) {
    error(2, errno, "Loading %s: %s", argv[1], ImageErrMsg());
  }
  InstrPrint(); // to print instrumentation
  InstrReset();
  printf("\n# CROP image\n");
  Image img2 = ImageCrop(img1, ImageWidth(img1)/2, ImageHeight(img1)/2, ImageWidth(img1)/4, ImageHeight(img1)/4);
  InstrPrint();

  Image cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageWidth(img2));
  Image cp2 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageWidth(img2));

  InstrReset();
  printf("\n# BLUR image (normal - 7x7)\n");
  ImageOldBlur(cp1, 7, 7);
  InstrPrint();

  InstrReset();
  printf("\n# BLUR image (normal - 15x15)\n");
  ImageOldBlur(cp1, 15, 15);
  InstrPrint();

  InstrReset();
  printf("\n# BLUR image (summation table - 7x7)\n");
  ImageBlur(cp1, 7, 7);
  InstrPrint();

  InstrReset();
  printf("\n# BLUR image (summation table - 15x15)\n");
  ImageBlur(cp1, 15, 15);
  InstrPrint();

  
  // Try changing the behaviour of the program by commenting/uncommenting
  // the appropriate lines.

  int px, py;
  int output = ImageLocateSubImage(img1, &px, &py, img2);
  printf("%d %d %d\n", output, px, py);
  if (img2 == NULL) {
    error(2, errno, "Rotating img2: %s", ImageErrMsg());
  }


/*  ImageNegative(img2);
  ImageThreshold(img2, 100);
  ImageBrighten(img2, 1.3); */

  if (ImageSave(img2, argv[2]) == 0) {
    error(2, errno, "%s: %s", argv[2], ImageErrMsg());
  }

  ImageDestroy(&img1);
  ImageDestroy(&img2);  

 return 0;
}

