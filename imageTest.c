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

#define PIXMEM InstrCount[0]
#define PIXCOMP InstrCount[1]

int main(int argc, char* argv[]) {
  char* str[100];
  /*
  if (argc != 3) {
    error(1, 0, "Usage: imageTest input.pgm output.pgm");
  }
  */

  int px;
  int py;

  for (int i = 1; i < argc; i++){

    ImageInit();
    
    printf("\n# LOAD NEW image : %s\n", argv[i]);
    InstrReset(); // to reset instrumentation
    Image img1 = ImageLoad(argv[i]);
    int n = PIXMEM;
    // Image img2 = ImageLoad(argv[2]);
    if (img1 == NULL) {
      error(2, errno, "Loading %s: %s", argv[i], ImageErrMsg());
    }
    InstrPrint(); // to print instrumentation
    Image cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# NORMAL BLUR image (size: %d - window 7x7)\n", n);
    ImageOldBlur(cp1, 7, 7);
    InstrPrint();

    sprintf(str,"tests/nblur%d_7x7.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# NORMAL BLUR image (size: %d - window 15x15)\n", n);
    ImageOldBlur(cp1, 15, 15);
    InstrPrint();

    sprintf(str,"tests/nblur%d_15x15.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# NORMAL BLUR image (size: %d - window 50x50)\n", n);
    ImageOldBlur(cp1, 50, 50);
    InstrPrint();


    sprintf(str,"tests/nblur%d_50x50.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));


    InstrReset();
    printf("\n# SUMMATION TABLE BLUR image (size: %d - window 7x7)\n", n);
    ImageBlur(cp1, 7, 7);
    InstrPrint();

    sprintf(str,"tests/stblur%d_7x7.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# SUMMATION TABLE BLUR image (size: %d- window 15x15)\n", n);
    ImageBlur(cp1, 15, 15);
    InstrPrint();

    sprintf(str,"tests/stblur%d_15x15.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# SUMMATION TABLE BLUR image (size: %d- window 50x50)\n", n);
    ImageBlur(cp1, 50, 50);
    InstrPrint();

    sprintf(str,"tests/stblur%d_50x50.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));
    InstrReset();

    printf("\n===========\n");

    printf("\n# SUBIMAGE LOCATING size 1/16 - BEST CASE\n");
    Image sub16b = ImageCrop(img1, 0, 0, ImageWidth(img1)/16, ImageHeight(img1)/16);
    ImageLocateSubImage(img1, &px, &py, sub16b);
    InstrPrint();
    InstrReset();
    printf("\n------\n");

    printf("\n# SUBIMAGE LOCATING size 1/16 - WORST CASE\n");
    Image sub16w = ImageCrop(img1, ImageWidth(img1) - ImageWidth(img1)/16, ImageHeight(img1) - ImageHeight(img1)/16, ImageWidth(img1)/16, ImageHeight(img1)/16);
    ImageLocateSubImage(img1, &px, &py, sub16w);
    InstrPrint();
    InstrReset();
    printf("\n------\n");

    printf("\n# SUBIMAGE LOCATING size 1/8 - BEST CASE\n");
    Image sub8b = ImageCrop(img1, 0, 0, ImageWidth(img1)/8, ImageHeight(img1)/8);
    ImageLocateSubImage(img1, &px, &py, sub8b);
    InstrPrint();
    InstrReset();

    printf("\n# SUBIMAGE LOCATING size 1/8 - WORST CASE\n");
    Image sub8w = ImageCrop(img1, ImageWidth(img1) - ImageWidth(img1)/8, ImageHeight(img1) - ImageHeight(img1)/8, ImageWidth(img1)/8, ImageHeight(img1)/8);
    ImageLocateSubImage(img1, &px, &py, sub8w);
    InstrPrint();
    InstrReset();
    printf("\n------\n");

    printf("\n# SUBIMAGE LOCATING size 1/4 - BEST CASE\n");
    Image sub4b = ImageCrop(img1, 0, 0, ImageWidth(img1)/4, ImageHeight(img1)/4);
    ImageLocateSubImage(img1, &px, &py, sub4b);
    InstrPrint();
    InstrReset();

    printf("\n# SUBIMAGE LOCATING size 1/4 - WORST CASE\n");
    Image sub4w = ImageCrop(img1, ImageWidth(img1) - ImageWidth(img1)/4, ImageHeight(img1) - ImageHeight(img1)/4, ImageWidth(img1)/4, ImageHeight(img1)/4);
    ImageLocateSubImage(img1, &px, &py, sub4w);
    InstrPrint();
    InstrReset();
    printf("\n------\n");

        printf("\n# SUBIMAGE LOCATING size 1/2 - BEST CASE\n");
    Image sub2b = ImageCrop(img1, 0, 0, ImageWidth(img1)/2, ImageHeight(img1)/2);
    ImageLocateSubImage(img1, &px, &py, sub2b);
    InstrPrint();
    InstrReset();

    printf("\n# SUBIMAGE LOCATING size 1/2 - WORST CASE\n");
    Image sub2w = ImageCrop(img1, ImageWidth(img1) - ImageWidth(img1)/2, ImageHeight(img1) - ImageHeight(img1)/2, ImageWidth(img1)/2, ImageHeight(img1)/2);
    ImageLocateSubImage(img1, &px, &py, sub2w);
    InstrPrint();
    InstrReset();
    printf("\n------\n");

        printf("\n# SUBIMAGE LOCATING size 1/1 - BEST CASE\n");
    Image sub1b = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));
    ImageLocateSubImage(img1, &px, &py, sub1b);
    InstrPrint();
    InstrReset();

    printf("\n# SUBIMAGE LOCATING size 1/1 - WORST CASE\n");
    Image sub1w = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));
    ImageLocateSubImage(img1, &px, &py, sub1w);
    InstrPrint();
    InstrReset();
    printf("\n------\n");

    ImageDestroy(&img1);
    ImageDestroy(&cp1);
  }



  
  // Try changing the behaviour of the program by commenting/uncommenting
  // the appropriate lines.
/*
  int px, py;
  int output = ImageLocateSubImage(img1, &px, &py, img2);
  printf("%d %d %d\n", output, px, py);
  if (img2 == NULL) {
    error(2, errno, "Rotating img2: %s", ImageErrMsg());
  }


  ImageNegative(img2);
  ImageThreshold(img2, 100);
  ImageBrighten(img2, 1.3);

  if (ImageSave(img2, argv[2]) == 0) {
    error(2, errno, "%s: %s", argv[2], ImageErrMsg());
  }

  ImageDestroy(&img1);
  ImageDestroy(&img2);  */

 return 0;
}

