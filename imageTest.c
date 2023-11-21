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
    error(1, 0, "Usage: imageTest input.pgm output.pgm"); a
  }
  */

  int px;
  int py;
  int res;

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
    InstrPrint();
    fflush(stdout); // to print instrumentation
    Image cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# NORMAL BLUR image (size: %d - window 7x7)\n", n);
    ImageOldBlur(cp1, 7, 7);
    InstrPrint();
    fflush(stdout);

    sprintf(str,"tests/nblur%d_7x7.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# NORMAL BLUR image (size: %d - window 15x15)\n", n);
    ImageOldBlur(cp1, 15, 15);
    InstrPrint();
    fflush(stdout);

    sprintf(str,"tests/nblur%d_15x15.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# NORMAL BLUR image (size: %d - window 50x50)\n", n);
    ImageOldBlur(cp1, 50, 50);
    InstrPrint();
    fflush(stdout);


    sprintf(str,"tests/nblur%d_50x50.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));


    InstrReset();
    printf("\n# SUMMATION TABLE BLUR image (size: %d - window 7x7)\n", n);
    ImageBlur(cp1, 7, 7);
    InstrPrint();
    fflush(stdout);

    sprintf(str,"tests/stblur%d_7x7.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# SUMMATION TABLE BLUR image (size: %d- window 15x15)\n", n);
    ImageBlur(cp1, 15, 15);
    InstrPrint();
    fflush(stdout);

    sprintf(str,"tests/stblur%d_15x15.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));

    InstrReset();
    printf("\n# SUMMATION TABLE BLUR image (size: %d- window 50x50)\n", n);
    ImageBlur(cp1, 50, 50);
    InstrPrint();
    fflush(stdout);

    sprintf(str,"tests/stblur%d_50x50.pgm", i);
    if (ImageSave(cp1, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    cp1 = ImageCrop(img1, 0, 0, ImageWidth(img1), ImageHeight(img1));
    InstrReset();

    printf("\n===========\n");


    cp1 = ImageCrop(cp1, 0, 0, ImageWidth(cp1), ImageHeight(cp1));  
    ImageThreshold(cp1,0);

    
    Image sub16b = ImageCrop(cp1, 0, 0, ImageWidth(cp1)/16, ImageHeight(cp1)/16);
    res = ImageLocateSubImage(cp1, &px, &py, sub16b);
    printf("\n# SUBIMAGE LOCATING size 1/16 - BEST CASE (success: %d)\n", res);
    InstrPrint();
    fflush(stdout);
    printf("\n------\n");

    
    Image sub16w = ImageCrop(cp1, ImageWidth(cp1) - ImageWidth(cp1)/16, ImageHeight(cp1) - ImageHeight(cp1)/16, ImageWidth(cp1)/16, ImageHeight(cp1)/16);
    ImageSetPixel(sub16w, ImageWidth(sub16w)-1, ImageHeight(sub16w)-1, 255 - ImageGetPixel(sub16w,ImageWidth(sub16w)-1, ImageHeight(sub16w)-1));
    InstrReset();
    res = ImageLocateSubImage(cp1, &px, &py, sub16w);
    printf("\n# SUBIMAGE LOCATING size 1/16 - WORST CASE (sucesss: %d)\n", res);
    InstrPrint();
    fflush(stdout);
    sprintf(str,"tests/sub16w%d.pgm", i);
    if (ImageSave(sub16w, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    printf("\n------\n");


    Image sub8b = ImageCrop(cp1, 0, 0, ImageWidth(cp1)/8, ImageHeight(cp1)/8);
    InstrReset();
    res = ImageLocateSubImage(cp1, &px, &py, sub8b);
    printf("\n# SUBIMAGE LOCATING size 1/8 - BEST CASE (success: %d)\n", res);
    InstrPrint();
    fflush(stdout);
    

    printf("\n# SUBIMAGE LOCATING size 1/8 - WORST CASE\n");
    Image sub8w = ImageCrop(cp1, ImageWidth(cp1) - ImageWidth(cp1)/8, ImageHeight(cp1) - ImageHeight(cp1)/8, ImageWidth(cp1)/8, ImageHeight(cp1)/8);
    ImageSetPixel(sub8w, ImageWidth(sub8w)-1, ImageHeight(sub8w)-1, 255 - ImageGetPixel(sub8w,ImageWidth(sub8w)-1, ImageHeight(sub8w)-1));
    InstrReset();
    ImageLocateSubImage(cp1, &px, &py, sub8w);
    InstrPrint();
    fflush(stdout);
    sprintf(str,"tests/sub8w%d.pgm", i);
    if (ImageSave(sub16w, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    
    printf("\n------\n");

    printf("\n# SUBIMAGE LOCATING size 1/4 - BEST CASE\n");
    Image sub4b = ImageCrop(cp1, 0, 0, ImageWidth(cp1)/4, ImageHeight(cp1)/4);
    InstrReset();
    ImageLocateSubImage(cp1, &px, &py, sub4b);
    InstrPrint();
    fflush(stdout);
    

    printf("\n# SUBIMAGE LOCATING size 1/4 - WORST CASE\n");
    Image sub4w = ImageCrop(cp1, ImageWidth(cp1) - ImageWidth(cp1)/4, ImageHeight(cp1) - ImageHeight(cp1)/4, ImageWidth(cp1)/4, ImageHeight(cp1)/4);
    ImageSetPixel(sub4w, ImageWidth(sub4w)-1, ImageHeight(sub4w)-1, 255 - ImageGetPixel(sub4w,ImageWidth(sub4w)-1, ImageHeight(sub4w)-1));
    InstrReset();
    ImageLocateSubImage(cp1, &px, &py, sub4w);
    InstrPrint();
    fflush(stdout);
    sprintf(str,"tests/sub4w%d.pgm", i);
    if (ImageSave(sub16w, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    
    printf("\n------\n");

    printf("\n# SUBIMAGE LOCATING size 1/2 - BEST CASE\n");
    Image sub2b = ImageCrop(cp1, 0, 0, ImageWidth(cp1)/2, ImageHeight(cp1)/2);
    InstrReset();
    ImageLocateSubImage(cp1, &px, &py, sub2b);
    InstrPrint();
    fflush(stdout);
    

    printf("\n# SUBIMAGE LOCATING size 1/2 - WORST CASE\n");
    Image sub2w = ImageCrop(cp1, ImageWidth(cp1) - ImageWidth(cp1)/2, ImageHeight(cp1) - ImageHeight(cp1)/2, ImageWidth(cp1)/2, ImageHeight(cp1)/2);
    ImageSetPixel(sub2w, ImageWidth(sub2w)-1, ImageHeight(sub2w)-1, 255 - ImageGetPixel(sub2w,ImageWidth(sub2w)-1, ImageHeight(sub2w)-1));
    InstrReset();
    ImageLocateSubImage(cp1, &px, &py, sub2w);
    InstrPrint();
    fflush(stdout);
    sprintf(str,"tests/sub4w%d.pgm", i);
    if (ImageSave(sub16w, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    
    printf("\n------\n");

    printf("\n# SUBIMAGE LOCATING size 1/1 - BEST CASE\n");
    Image sub1b = ImageCrop(cp1, 0, 0, ImageWidth(cp1), ImageHeight(cp1));
    InstrReset();
    ImageLocateSubImage(cp1, &px, &py, sub1b);
    InstrPrint();
    fflush(stdout);
    

    printf("\n# SUBIMAGE LOCATING size 1/1 - WORST CASE\n");
    Image sub1w = ImageCrop(cp1, 0, 0, ImageWidth(cp1), ImageHeight(cp1));
    ImageSetPixel(sub1w, ImageWidth(sub1w)-1, ImageHeight(sub1w)-1, 255 - ImageGetPixel(sub1w,ImageWidth(sub1w)-1, ImageHeight(sub1w)-1));
    InstrReset();
    ImageLocateSubImage(cp1, &px, &py, sub1w);
    InstrPrint();
    fflush(stdout);
    sprintf(str,"tests/sub4w%d.pgm", i);
    if (ImageSave(sub1w, str) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }
    printf("\n------\n");

    ImageDestroy(&img1);
    ImageDestroy(&cp1);
  }

  /*printf("# LOAD image");
  ImageInit();
  InstrReset(); // Reset instrumentation
  Image img1 = ImageLoad(argv[1]);
  printf("\nHeight: %d, Width: %d\n", ImageHeight(img1), ImageWidth(img1));

  printf("\n# CROP image");
  Image cp1 = ImageCrop(img1, 0, 0, 150, 150);
  printf("\nCropped Image - Height: %d, Width: %d\n", ImageHeight(cp1), ImageWidth(cp1));

  printf("\n# LOCATE subimage");
  int x, y;
  InstrReset(); // Reset instrumentation
  if (ImageLocateSubImage(img1, &x, &y, cp1)) {
    printf("\nSubimage found at coordinates - x: %d, y: %d\n", x, y);
  } else {
    printf("\nSubimage not found.\n");
  }
  InstrPrint(); // Print instrumentation
  // Free memory
  ImageDestroy(&img1);
  ImageDestroy(&cp1);



  
  // Try changing the behaviour of the program by commenting/uncommenting
  // the appropriate lines.
/*
  int px, py;
  int output = ImageLocateSubImage(cp1, &px, &py, img2);
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

