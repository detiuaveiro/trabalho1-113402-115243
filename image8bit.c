/// image8bit - A simple image processing module.
///
/// This module is part of a programming project
/// for the course AED, DETI / UA.PT
///
/// You may freely use and modify this code, at your own risk,
/// as long as you give proper credit to the original and subsequent authors.
///
/// João Manuel Rodrigues <jmr@ua.pt>
/// 2013, 2023

// Student authors (fill in below):
// NMec:113402  Name:Hugo Santos Ribeiro
// NMec:115243  Name:Ricardo Alexandre Antunes
// 
// 
// Date:
//

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#include "image8bit.h"

#include <math.h>
#include <assert.h>
#define NDEBUG
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "instrumentation.h"



// The data structure
//
// An image is stored in a structure containing 3 fields:
// Two integers store the image width and height.
// The other field is a pointer to an array that stores the 8-bit gray
// level of each pixel in the image.  The pixel array is one-dimensional
// and corresponds to a "raster scan" of the image from left to right,
// top to bottom.
// For example, in a 100-pixel wide image (img->width == 100),
//   pixel position (x,y) = (33,0) is stored in img->pixel[33];
//   pixel position (x,y) = (22,1) is stored in img->pixel[122].
// 
// Clients should use images only through variables of type Image,
// which are pointers to the image structure, and should not access the
// structure fields directly.

// Maximum value you can store in a pixel (maximum maxval accepted)
const uint8 PixMax = 255;

// Internal structure for storing 8-bit graymap images
struct image {
  int width;
  int height;
  int maxval;   // maximum gray value (pixels with maxval are pure WHITE)
  uint8* pixel; // pixel data (a raster scan)
};

int* valuesum1;
int* valuesum2;

static void InitializeSum1(Image img){
  valuesum1 = (int*) malloc(sizeof(int*) * img->height * img->width);
}

static void InitializeSum2(Image img){
  valuesum2 = (int*) malloc(sizeof(int*) * img->height * img->width);
}


// This module follows "design-by-contract" principles.
// Read `Design-by-Contract.md` for more details.

/// Error handling functions

// In this module, only functions dealing with memory allocation or file
// (I/O) operations use defensive techniques.
// 
// When one of these functions fails, it signals this by returning an error
// value such as NULL or 0 (see function documentation), and sets an internal
// variable (errCause) to a string indicating the failure cause.
// The errno global variable thoroughly used in the standard library is
// carefully preserved and propagated, and clients can use it together with
// the ImageErrMsg() function to produce informative error messages.
// The use of the GNU standard library error() function is recommended for
// this purpose.
//
// Additional information:  man 3 errno;  man 3 error;

// Variable to preserve errno temporarily
static int errsave = 0;

// Error cause
static char* errCause;

/// Error cause.
/// After some other module function fails (and returns an error code),
/// calling this function retrieves an appropriate message describing the
/// failure cause.  This may be used together with global variable errno
/// to produce informative error messages (using error(), for instance).
///
/// After a successful operation, the result is not garanteed (it might be
/// the previous error cause).  It is not meant to be used in that situation!
char* ImageErrMsg() { ///
  return errCause;
}


// Defensive programming aids
//
// Proper defensive programming in C, which lacks an exception mechanism,
// generally leads to possibly long chains of function calls, error checking,
// cleanup code, and return statements:
//   if ( funA(x) == errorA ) { return errorX; }
//   if ( funB(x) == errorB ) { cleanupForA(); return errorY; }
//   if ( funC(x) == errorC ) { cleanupForB(); cleanupForA(); return errorZ; }
//
// Understanding such chains is difficult, and writing them is boring, messy
// and error-prone.  Programmers tend to overlook the intricate details,
// and end up producing unsafe and sometimes incorrect programs.
//
// In this module, we try to deal with these chains using a somewhat
// unorthodox technique.  It resorts to a very simple internal function
// (check) that is used to wrap the function calls and error tests, and chain
// them into a long Boolean expression that reflects the success of the entire
// operation:
//   success = 
//   check( funA(x) != error , "MsgFailA" ) &&
//   check( funB(x) != error , "MsgFailB" ) &&
//   check( funC(x) != error , "MsgFailC" ) ;
//   if (!success) {
//     conditionalCleanupCode();
//   }
//   return success;
// 
// When a function fails, the chain is interrupted, thanks to the
// short-circuit && operator, and execution jumps to the cleanup code.
// Meanwhile, check() set errCause to an appropriate message.
// 
// This technique has some legibility issues and is not always applicable,
// but it is quite concise, and concentrates cleanup code in a single place.
// 
// See example utilization in ImageLoad and ImageSave.
//
// (You are not required to use this in your code!)


// Check a condition and set errCause to failmsg in case of failure.
// This may be used to chain a sequence of operations and verify its success.
// Propagates the condition.
// Preserves global errno!
static int check(int condition, const char* failmsg) {
  errCause = (char*)(condition ? "" : failmsg);
  return condition;
}



/// Init Image library.  (Call once!)
/// Currently, simply calibrate instrumentation and set names of counters.
void ImageInit(void) { ///
  InstrCalibrate();
  InstrName[0] = "pixmem"; // InstrCount[0] will count pixel array acesses
  InstrName[1] = "pixcomp"; // InstrCount[1] will count pixel array comparisons
  // Name other counters here...
  
}

// Macros to simplify accessing instrumentation counters:
#define PIXMEM InstrCount[0]
#define PIXCOMP InstrCount[1]
// Add more macros here...

// TIP: Search for PIXMEM or InstrCount to see where it is incremented!


/// Image management functions

/// Create a new black image.
///   width, height : the dimensions of the new image.
///   maxval: the maximum gray level (corresponding to white).
/// Requires: width and height must be non-negative, maxval > 0.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCreate(int width, int height, uint8 maxval) { ///
  assert (width >= 0);
  assert (height >= 0);
  assert (0 < maxval && maxval <= PixMax);
  Image image = (Image)malloc(sizeof(Image)); //allocated memory for image
  if(check(image != NULL, "Failed memory allocation")){ //checks if allocation was successful

    image->width = width;
    image->height = height;
    image->maxval = maxval;
    image->pixel = (uint8*) malloc(sizeof(uint8*)*height*width); //allocated memory for pixels (initialization)
    if(check(image != NULL, "Failed memory allocation")){ //checks if allocation was successful
      return image;
    }
    else{ //frees memory if unsuccessful
      free(image->pixel);
      free(image);
      return NULL;
    }
  }
  else{ //frees memory if unsuccessful (pixel not created yet)
    free(image);
    return NULL;
  }
}

/// Destroy the image pointed to by (*imgp).
///   imgp : address of an Image variable.
/// If (*imgp)==NULL, no operation is performed.
/// Ensures: (*imgp)==NULL.
/// Should never fail, and should preserve global errno/errCause.
void ImageDestroy(Image* imgp) { ///frees everything
  assert (imgp != NULL);
  free((*imgp)->pixel);
  free((*imgp));
}


/// PGM file operations

// See also:
// PGM format specification: http://netpbm.sourceforge.net/doc/pgm.html

// Match and skip 0 or more comment lines in file f.
// Comments start with a # and continue until the end-of-line, inclusive.
// Returns the number of comments skipped.
static int skipComments(FILE* f) {
  char c;
  int i = 0;
  while (fscanf(f, "#%*[^\n]%c", &c) == 1 && c == '\n') {
    i++;
  }
  return i;
}

/// Load a raw PGM file.
/// Only 8 bit PGM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageLoad(const char* filename) { ///
  int w, h;
  int maxval;
  char c;
  FILE* f = NULL;
  Image img = NULL;

  int success = 
  check( (f = fopen(filename, "rb")) != NULL, "Open failed" ) &&
  // Parse PGM header
  check( fscanf(f, "P%c ", &c) == 1 && c == '5' , "Invalid file format" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &w) == 1 && w >= 0 , "Invalid width" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &h) == 1 && h >= 0 , "Invalid height" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d", &maxval) == 1 && 0 < maxval && maxval <= (int)PixMax , "Invalid maxval" ) &&
  check( fscanf(f, "%c", &c) == 1 && isspace(c) , "Whitespace expected" ) &&
  // Allocate image
  (img = ImageCreate(w, h, (uint8)maxval)) != NULL &&
  // Read pixels
  check( fread(img->pixel, sizeof(uint8), w*h, f) == w*h , "Reading pixels" );
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (!success) {
    errsave = errno;
    ImageDestroy(&img);
    errno = errsave;
  }
  if (f != NULL) fclose(f);
  return img;
}

/// Save image to PGM file.
/// On success, returns nonzero.
/// On failure, returns 0, errno/errCause are set appropriately, and
/// a partial and invalid file may be left in the system.
int ImageSave(Image img, const char* filename) { ///
  assert (img != NULL);
  int w = img->width;
  int h = img->height;
  uint8 maxval = img->maxval;
  FILE* f = NULL;

  int success =
  check( (f = fopen(filename, "wb")) != NULL, "Open failed" ) &&
  check( fprintf(f, "P5\n%d %d\n%u\n", w, h, maxval) > 0, "Writing header failed" ) &&
  check( fwrite(img->pixel, sizeof(uint8), w*h, f) == w*h, "Writing pixels failed" ); 
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (f != NULL) fclose(f);
  return success;
}


/// Information queries

/// These functions do not modify the image and never fail.

/// Get image width
int ImageWidth(Image img) { ///
  assert (img != NULL);
  return img->width;
}

/// Get image height
int ImageHeight(Image img) { ///
  assert (img != NULL);
  return img->height;
}

/// Get image maximum gray level
int ImageMaxval(Image img) { ///
  assert (img != NULL);
  return img->maxval;
}

/// Pixel stats
/// Find the minimum and maximum gray levels in image.
/// On return,
/// *min is set to the minimum gray level in the image,
/// *max is set to the maximum.
void ImageStats(Image img, uint8* min, uint8* max) { ///
  assert (img != NULL);
  *min = ImageGetPixel(img, 0, 0); //setting min to the first pixel
  *max = ImageGetPixel(img, 0, 0); //setting max to the first pixel
  int hei = img->height;
  int wid = img->width;
  for (int x = 0; x < wid; x++){
    for (int y = 0; y < hei; y++){
      uint8 pix = ImageGetPixel(img, x, y); //saving the value in pix so that we don't need to constantly access it
      *min = MIN(*min, pix); //updating *min accordingly
      *max = MAX(*max, pix); //updating *max accordingly
    }
  }
}

/// Check if pixel position (x,y) is inside img.
int ImageValidPos(Image img, int x, int y) { ///
  assert (img != NULL);
  return (0 <= x && x < img->width) && (0 <= y && y < img->height); //x must be between 0 and width - 1 (inclusive) / y must be between 0 and height - 1 (inclusive)
}

/// Check if rectangular area (x,y,w,h) is completely inside img.
int ImageValidRect(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  int hei = img->height;
  int wid = img->width;
  return (x + w <= wid && y + h <= hei); //x + w must be at most width / y + h must be at most height
}

/// Pixel get & set operations

/// These are the primitive operations to access and modify a single pixel
/// in the image.
/// These are very simple, but fundamental operations, which may be used to 
/// implement more complex operations.

// Transform (x, y) coords into linear pixel index.
// This internal function is used in ImageGetPixel / ImageSetPixel. 
// The returned index must satisfy (0 <= index < img->width*img->height)
static inline int G(Image img, int x, int y) {
  /*
  The image begins in (0,0) and goes sideways as in (1,0), (2,0), (3,0) etc until it reaches (width - 1, 0)
  Once it switches lines to (0, 1) the value of the index becomes width - 1 + 1 = width.
  The same process repeats so the value of the index is x + y*width
  */
  int index = x + y*(img->width);
  assert (0 <= index && index < img->width*img->height);
  return index;
}

/// Get the pixel (level) at position (x,y).
uint8 ImageGetPixel(Image img, int x, int y) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (read)
  return img->pixel[G(img, x, y)];
} 

/// Set the pixel at position (x,y) to new level.
void ImageSetPixel(Image img, int x, int y, uint8 level) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (store)
  img->pixel[G(img, x, y)] = level;
} 


/// Pixel transformations

/// These functions modify the pixel levels in an image, but do not change
/// pixel positions or image geometry in any way.
/// All of these functions modify the image in-place: no allocation involved.
/// They never fail.


/// Transform image to negative image.
/// This transforms dark pixels to light pixels and vice-versa,
/// resulting in a "photographic negative" effect.
void ImageNegative(Image img) { ///
  assert (img != NULL);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      ImageSetPixel(img, x, y, img->maxval - ImageGetPixel(img, x, y)); //maxval is white, therefore doing this operation sets the pixel to the other side of the "brightness" spectrum
    }
  }
}

/// Apply threshold to image.
/// Transform all pixels with level<thr to black (0) and
/// all pixels with level>=thr to white (maxval).
void ImageThreshold(Image img, uint8 thr) { ///
  assert (img != NULL);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      ImageSetPixel(img, x, y, (ImageGetPixel(img, x, y) < thr) ? 0 : img->maxval); //sets the value to 0 if the original value is below thr, otherwise maxval
    }
  }
}

/// Brighten image by a factor.
/// Multiply each pixel level by a factor, but saturate at maxval.
/// This will brighten the image if factor>1.0 and
/// darken the image if factor<1.0.
void ImageBrighten(Image img, double factor) { ///
  assert (img != NULL);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      ImageSetPixel(img, x, y, MIN((uint8)(ImageGetPixel(img, x, y) * factor + 0.5), img->maxval)); //a negative factor also negates the image while applying the brightening factor. 0.5 is added for rounding purposes (the value is floored by default)
    }
  }
}


/// Geometric transformations

/// These functions apply geometric transformations to an image, a
/// returning a new image as a result.
/// 
/// Success and failure are treated as in ImageCreate:
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.

// Implementation hint: 
// Call ImageCreate whenever you need a new image!

/// Rotate an image.
/// Returns a rotated version of the image.
/// The rotation is 90 degrees anti-clockwise.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageRotate(Image img) { ///
  assert (img != NULL);
  int wid = img->width;
  int hei = img->height; 
  uint8 maxvalu = img->maxval;
  Image rotated = ImageCreate(hei, wid, maxvalu); // in a 400x200 picture, the rotated version will be 200x400
  for (int x = 0; x<hei; x++){
    for (int y = 0; y<wid; y++){
      ImageSetPixel(rotated, x, y, ImageGetPixel(img, img->width - 1 - y, x)); // anti-clockwise rotation
    }
  }
  return rotated;
}

/// Mirror an image = flip left-right.
/// Returns a mirrored version of the image.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageMirror(Image img) { ///
  assert (img != NULL);
  int wid = img->width;
  int hei = img->height; 
  uint8 maxvalu = img->maxval;
  Image mirror = ImageCreate(wid, hei, maxvalu);
  for (int x = 0; x<wid; x++){
    for (int y = 0; y<hei; y++){
      ImageSetPixel(mirror, x, y, ImageGetPixel(img, img->width - 1 - x, y)); //mirrored image
    }
  }
  return mirror;
}

/// Crop a rectangular subimage from img.
/// The rectangle is specified by the top left corner coords (x, y) and
/// width w and height h.
/// Requires:
///   The rectangle must be inside the original image.
/// Ensures:
///   The original img is not modified.
///   The returned image has width w and height h.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCrop(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  assert (ImageValidRect(img, x, y, w, h));
  uint8 maxvalu = img->maxval;
  Image subimg = ImageCreate(w, h, maxvalu);
  for (int xa = 0; xa<w; xa++){
    for (int ya = 0; ya<h; ya++){ //iterates through the pixels of the cropped image
      ImageSetPixel(subimg, xa, ya, ImageGetPixel(img, xa + x, ya + y)); //sets the pixels by correctly locating them in the bigger picture
    }
  }
  return subimg;
}


/// Operations on two images

/// Paste an image into a larger image.
/// Paste img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
void ImagePaste(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidRect(img1, x, y, img2->width, img2->height));
  for (int xa = 0; xa<img2->width; xa++){
    for (int ya = 0; ya<img2->height; ya++){
      ImageSetPixel(img1, xa + x, ya + y, ImageGetPixel(img2, xa, ya)); //copies img2 onto img1
    }
  }
}

/// Blend an image into a larger image.
/// Blend img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
/// alpha usually is in [0.0, 1.0], but values outside that interval
/// may provide interesting effects.  Over/underflows should saturate.
void ImageBlend(Image img1, int x, int y, Image img2, double alpha) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidRect(img1, x, y, img2->width, img2->height));
  for (int xa = 0; xa<img2->width; xa++){
    for (int ya = 0; ya<img2->height; ya++){
      PIXMEM++;
      img1->pixel[G(img1, xa + x, ya + y)] = ImageGetPixel(img2,xa,ya)*alpha + ImageGetPixel(img1, xa + x, ya + y)*(1-alpha) + 0.5;
    }
  }
}

/// Compare an image to a subimage of a larger image.
/// Returns 1 (true) if img2 matches subimage of img1 at pos (x, y).
/// Returns 0, otherwise.
int OldImageMatchSubImage(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidPos(img1, x, y));
  for (int xa = 0; xa<img2->width; xa++){
    for (int ya = 0; ya<img2->height; ya++){
      PIXCOMP++;
      if (ImageGetPixel(img2,xa,ya) != ImageGetPixel(img1, xa+x, ya+y)){
        return 0;
      }
    }
  }
  return 1;
}

int ImageMatchSubImage(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidPos(img1, x, y));
  int xstart, xend, ystart, yend;
  for (int xa = img2->width - 1; xa >= 0; xa--){
    int x1 = xa + x;
    int y1 = y + ImageHeight(img2) - 1;
    int value1 = valuesum1[G(img1, x1, y1)] - ((x>0) ? valuesum1[G(img1, x-1, y1)] : 0) - ((y>0) ? valuesum1[G(img1, x1, y - 1)] : 0) + ((x>0 && y>0) ? valuesum1[G(img1, x - 1, y - 1)] : 0);
    if (value1 != valuesum2[G(img2, xa, img2->height - 1)]){
      return 0;
    }
  }
  for (int ya = img2->height - 1; ya >= 0 ; ya--){
    int x1 = x + ImageWidth(img2) - 1;
    int y1 = y + ya;
    int value1 = valuesum1[G(img1, x1, y1)] - ((x>0) ? valuesum1[G(img1, x-1, y1)] : 0) - ((y>0) ? valuesum1[G(img1, x1, y - 1)] : 0) + ((x>0 && y>0) ? valuesum1[G(img1, x - 1, y - 1)] : 0);
    if (value1 != valuesum2[G(img2, img2->width - 1, ya)]){
      return 0;
    }
  }
  return 1;
}

/// Locate a subimage inside another image.
/// Searches for img2 inside img1.
/// If a match is found, returns 1 and matching position is set in vars (*px, *py).
/// If no match is found, returns 0 and (*px, *py) are left untouched.
int ImageLocateSubImage(Image img1, int* px, int* py, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  InitializeSum1(img1);
  for (int x = 0; x < img1->width; x++){
    for (int y = 0; y < img1->height; y++){
      valuesum1[G(img1, x, y)] = pow((int)ImageGetPixel(img1, x, y),2) + ((x > 0) ? valuesum1[G(img1, x-1, y)] : 0) + ((y > 0) ? valuesum1[G(img1, x, y-1)] : 0) - ((x > 0 && y > 0) ? valuesum1[G(img1, x-1, y-1)] : 0);
    }
  }

  InitializeSum2(img2);
  for (int x2 = 0; x2 < img2->width; x2++){
    for (int y2 = 0; y2 < img2->height; y2++){
      valuesum2[G(img2, x2, y2)] = pow((int)ImageGetPixel(img2, x2, y2),2) + ((x2 > 0) ? valuesum2[G(img2, x2-1, y2)] : 0) + ((y2 > 0) ? valuesum2[G(img2, x2, y2-1)] : 0) - ((x2 > 0 && y2 > 0) ? valuesum2[G(img2, x2-1, y2-1)] : 0);
    }
  }
  for (int xa = 0; xa<img1->width - img2->width + 1 ; xa++){
    for (int ya = 0; ya<img1->height - img2->height + 1; ya++){
      if (ImageMatchSubImage(img1, xa, ya, img2)){
        *px = xa;
        *py = ya;
        return 1;
      }
    }
  }
  return 0;
}


/// Filtering

/// Blur an image by a applying a (2dx+1)x(2dy+1) mean filter.
/// Each pixel is substituted by the mean of the pixels in the rectangle
/// [x-dx, x+dx]x[y-dy, y+dy].
/// The image is changed in-place.
void ImageOldBlur(Image img, int dx, int dy) { 
  int sum;
  int count;
  Image imgaux = ImageCreate(img->width+1, img->height+1, img->maxval);
  ImagePaste(imgaux,0, 0, img);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      sum = 0;
      count = 0;
      for (int xa = x - dx; xa <= x + dx; xa++){
        for (int ya = y - dy; ya <= y + dy; ya++ ){
          PIXCOMP++;
          if (ImageValidPos(img, xa, ya)){
            sum += ImageGetPixel(imgaux, xa, ya);
            count += 1;
          }
        }
      }
      ImageSetPixel(img, x, y, (sum + count/2)/count);
    }
  }
}

void ImageBlur(Image img, int dx, int dy) {
  int* valuesum;
  int blurval, xstart, xend, ystart, yend, xlen, ylen, count;
  valuesum = (int*) malloc(sizeof(int*) * img->height * img->width);
  if(check(valuesum != NULL, "Failed memory allocation")){
    for (int x = 0; x < img->width; x++){
      for (int y = 0; y < img->height; y++){
        valuesum[G(img, x, y)] = (int)ImageGetPixel(img, x, y) + ((x > 0) ? valuesum[G(img, x-1, y)] : 0) + ((y > 0) ? valuesum[G(img, x, y-1)] : 0) - ((x > 0 && y > 0) ? valuesum[G(img, x-1, y-1)] : 0);
      }
    }
    for (int x = 0; x < img->width; x++){
      for (int y = 0; y < img->height; y++){
        xstart = MAX(x - dx, 0);
        ystart = MAX(y - dy, 0);
        xend = MIN(x + dx, img->width-1);
        yend = MIN(y + dy, img->height-1);
        xlen = xend - xstart + 1;
        ylen = yend - ystart + 1;
        count = ylen * xlen;
        blurval = valuesum[G(img, xend, yend)] - ((ystart > 0) ? valuesum[G(img, xend, ystart - 1)] : 0) - ((xstart > 0) ? valuesum[G(img, xstart - 1, yend)] : 0) + ((xstart > 0 && ystart > 0) ? valuesum[G(img, xstart - 1, ystart - 1)] : 0);
        blurval = (blurval + count / 2)/count;
        ImageSetPixel(img, x, y, blurval);
      }
    }
  }
  else {
    free(valuesum);
  }
}