// 205616634 Eldar Shlomi
#include <stdbool.h>
/* change it from function to macros because macros is more efficient*/

/* Compute min and max of two integers, respectively  */
#define  min(A, B) ((A) < (B) ? (A) : (B))
#define  max(A, B) ((A) > (B) ? (A) : (B))
#define calcIndex(i, j, n) ((i)*(n)+(j))
#define multiply9(a)    (((a) << 3) + (a))

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

/* I will explain most of the changes here, and explain small or one-time changes during the function. A guiding principle is that
   unlike writing regular code, I did not think about the visibility of the code and did not count on duplicating code, but only
   dealt with efficiency
1. Calling to functions cost time - in back stage, happend operations like allocating space in the stack, resetting registers,
   transferring the variables to registers and resetting them at the end. so To save this, all the code is listed as one large
   function, containing all the relevant variables.
2. I notice that the function which most affects running time is smooth. which makes the operation of blur and sharpen, depends
   at the input. the function. as given' "smooth" sum the rgb of every pixel "neighbors" (= the pixel that in the karnel of the
   chosen pixel, in our case the pixels that in the 3x3 matrix, when the middle pixel is the chosen), calculate by  by itself in
   complex way: for every pixel, Create a variable and each time give it the value of another pixel in the matrix, and sum the rgb
   to one sum (so we actually do 27 schema operations! And 9 memory allocation operations for calculating the data for only one pixel!!!
   This costs a lot of time! Instead, we used In k * k we point so that we have saved the memory allocation operation and instead of 27
   sum calculation operations (for red, blue and green, 9 for each) we calculate this at one timefor each color.
3. Instead of the function "copyPixel", that copy the original image to other copy, and do it pixel by pixel. we use "memcpy" function,
   which do the same it in less time.
4. I omitted unnecessary things like the pixel_sum structure, and the charsToPixels and pixelToChar functions. 
5. I use macro expression insted of normal function for calculating because normal function do the calculating during the compailing time,
   which cost more time.
6. I change all the "i++" to "++i" because When doing "i++", we keep the value of the current I in memory, then create a new copy, increase
   the value by one and then release the original variable. So by doing "++i" we save time.
7. We don't need "pixelsImg" to alocate dynamic, so we change it.
8. I change the "filter" and make it without loop because i, until the time it use, has all the variables whose needed for, so i use it
   without the loop, and almost without using of new variables and less statements.
9. Instead of multiple by 9 as "x * 9" i use bitwise <<, because it save time/ 
10.Use to prefix word Register for variables that repeat, because when variaible is in the register (and not in the stack) the acess to is faster.
*/
void doConvolution(Image *image, int kernelSize, int kernel[kernelSize][kernelSize], int kernelScale, bool filter) {
    // variables decleration:
    register pixel* pixelsImg = (pixel*)image->data;
    register pixel* backupOrg = malloc(m * n * sizeof(pixel));
    int row, col, i, j, x;
    register pixel current_pixel;
    memcpy(backupOrg, pixelsImg, m * n * sizeof(pixel)); 
    
    // if do function without filter:
    if (!filter) {
        // using blur:
        if (kernel[0][0] == 1) {
            for (i = 1; i < m - 1; ++i) {        
                int first_square = (i - 1) * n;
                for (j = 1, x = 0; j < m - 1 - 1; ++j, ++x) {
                    int kernel_red, kernel_green, kernel_blue;
                    // first line pixels decleration:
                    register pixel* pixel1_1 = &backupOrg[first_square + x];
                    register pixel* pixel1_2 = pixel1_1 + 1;
                    register pixel* pixel1_3 = pixel1_1 + 2;
                    // second line pixels decleration:
                    register pixel* pixel2_1 = pixel1_1 + m;
                    register pixel* pixel2_2 = pixel2_1 + 1;
                    register pixel* pixel2_3 = pixel2_1 + 2;
                    // three line pixels decleration:
                    register pixel* pixel3_1 = pixel2_1 + m;
                    register pixel* pixel3_2 = pixel3_1 + 1;
                    register pixel* pixel3_3 = pixel3_1 + 2;

                    // suming in "one shot" the red, green and blue values of all the pixel in the kernel, and devide by 9 all of them.
                    kernel_red = ((int)pixel1_1->red + (int)pixel1_2->red + (int)pixel1_3->red + (int)pixel2_1->red + (int)pixel2_2->red + 
                        (int)pixel2_3->red + (int)pixel3_1->red + (int)pixel3_2->red + (int)pixel3_3->red) / kernelScale;
                    kernel_green = ((int)pixel1_1->green + (int)pixel1_2->green + (int)pixel1_3->green + (int)pixel2_1->green +
                        (int)pixel2_2->green + (int)pixel2_3->green + (int)pixel3_1->green + (int)pixel3_2->green + (int)pixel3_3->green) / kernelScale;
                    kernel_blue = ((int)pixel1_1->blue + (int)pixel1_2->blue + (int)pixel1_3->blue + (int)pixel2_1->blue +
                        (int)pixel2_2->blue + (int)pixel2_3->blue + (int)pixel3_1->blue + (int)pixel3_2->blue + (int)pixel3_3->blue) / kernelScale;

                    // omit the max with 0 check because in blur the values always will be positive.
                    current_pixel.red = (unsigned char)(min(kernel_red, 255));
                    current_pixel.green = (unsigned char)(min(kernel_green, 255));
                    current_pixel.blue = (unsigned char)(min(kernel_blue, 255));
                    pixelsImg[calcIndex(i, j, n)] = current_pixel;
                }
            }
        }

        // its sharp: (when sharp, all the kernel cells are -1 except the middle one, which is 9). 
        else if (kernel[0][0] == -1) {
            for (i = 1; i < m - 1; ++i) {
                int first_square = (i - 1) * n;
                for (j = 1, x = 0; j < m - 1 - 1; ++j, ++x) {
                    register int kernel_red, kernel_green, kernel_blue;
                    // first line pixels decleration:
                    register pixel* pixel1_1 = &backupOrg[first_square + x];
                    register pixel* pixel1_2 = pixel1_1 + 1;
                    register pixel* pixel1_3 = pixel1_1 + 2;
                    // second line pixels decleration:
                    register pixel* pixel2_1 = pixel1_1 + m;
                    register pixel* pixel2_2 = pixel2_1 + 1;
                    register pixel* pixel2_3 = pixel2_1 + 2;
                    // three line pixels decleration:
                    register pixel* pixel3_1 = pixel2_1 + m;
                    register pixel* pixel3_2 = pixel3_1 + 1;
                    register pixel* pixel3_3 = pixel3_1 + 2;

                    // suming in "one shot" the red, green and blue values of all the pixel in the kernel, and multiple by -1 all of them except the middle one
                    // which mult by 9(for sharping)
                    kernel_red = (int)multiply9(pixel2_2->red) + (-1) * ((int)pixel1_1->red + (int)pixel1_2->red + (int)pixel1_3->red +
                        (int)pixel2_1->red + (int)pixel2_3->red + (int)pixel3_1->red + (int)pixel3_2->red + (int)pixel3_3->red);

                    kernel_green = (int)multiply9(pixel2_2->green) + (-1) * ((int)pixel1_1->green + (int)pixel1_2->green + (int)pixel1_3->green +
                        (int)pixel2_1->green + (int)pixel2_3->green + (int)pixel3_1->green + (int)pixel3_2->green + (int)pixel3_3->green);

                    kernel_blue = (int)multiply9(pixel2_2->blue) + (-1) * ((int)pixel1_1->blue + (int)pixel1_2->blue + (int)pixel1_3->blue + 
                        (int)pixel2_1->blue + (int)pixel2_3->blue + (int)pixel3_1->blue + (int)pixel3_2->blue + (int)pixel3_3->blue);

                    // making the rgb first be the maximum between the value which got before to 0, and after
                   // maing him (red, blue and green seperate) be the min between the value to 255) 
                    current_pixel.red = (unsigned char)min(max(kernel_red, 0), 255);
                    current_pixel.green = (unsigned char)min(max(kernel_green, 0), 255);
                    current_pixel.blue = (unsigned char)min(max(kernel_blue, 0), 255);
                    pixelsImg[calcIndex(i, j, n)] = current_pixel;
                }
            }
        }
    }
    // if do function with filter:
    else if (filter) {
        // its blur: (when blur, all the kernel cells are 1).
        if (kernel[0][0] == 1) {
            for (i = 1; i < m - 1; ++i) {
                int first_square = (i - 1) * n;
                register int tempSum;
                for (j = 1, x = 0; j < m - 1 - 1; ++j, ++x) {
                    int kernel_red, kernel_green, kernel_blue;
                    // first line pixels decleration:
                    register pixel* pixel1_1 = &backupOrg[first_square + x];
                    register pixel* pixel1_2 = pixel1_1 + 1;
                    register pixel* pixel1_3 = pixel1_1 + 2;
                    // second line pixels decleration:
                    register pixel* pixel2_1 = pixel1_1 + m;
                    register pixel* pixel2_2 = pixel2_1 + 1;
                    register pixel* pixel2_3 = pixel2_1 + 2;
                    // three line pixels decleration:
                    register pixel* pixel3_1 = pixel2_1 + m;
                    register pixel* pixel3_2 = pixel3_1 + 1;
                    register pixel* pixel3_3 = pixel3_1 + 2;

                    // suming in "one shot" the red, green and blue values of all the pixel in the kernel.
                    kernel_red = ((int)pixel1_1->red + (int)pixel1_2->red + (int)pixel1_3->red + (int)pixel2_1->red + (int)pixel2_2->red +
                        (int)pixel2_3->red + (int)pixel3_1->red + (int)pixel3_2->red + (int)pixel3_3->red);
                    kernel_green = ((int)pixel1_1->green + (int)pixel1_2->green + (int)pixel1_3->green + (int)pixel2_1->green +
                        (int)pixel2_2->green + (int)pixel2_3->green + (int)pixel3_1->green + (int)pixel3_2->green + (int)pixel3_3->green);
                    kernel_blue = ((int)pixel1_1->blue + (int)pixel1_2->blue + (int)pixel1_3->blue + (int)pixel2_1->blue +
                        (int)pixel2_2->blue + (int)pixel2_3->blue + (int)pixel3_1->blue + (int)pixel3_2->blue + (int)pixel3_3->blue);

                    // the filter function:
                    // variable to check who is the max and who is the min value pixels.
                    register int min_intensity = (int)pixel1_1->red + (int)pixel1_1->green + (int)pixel1_1->blue;
                    register int max_intensity = min_intensity;
                    // pointers for the check
                    pixel* min_pixel = pixel1_1;
                    pixel* max_pixel = pixel1_1;

                    tempSum = (int)pixel1_2->red + (int)pixel1_2->green + (int)pixel1_2->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel1_2;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel1_2;
                    }
                    tempSum = (int)pixel1_3->red + (int)pixel1_3->green + (int)pixel1_3->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel1_3;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel1_3;
                    }
                    tempSum = (int)pixel2_1->red + (int)pixel2_1->green + (int)pixel2_1->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel2_1;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel2_1;
                    }
                    tempSum = (int)pixel2_2->red + (int)pixel2_2->green + (int)pixel2_2->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel2_2;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel2_2;
                    }
                    tempSum = (int)pixel2_3->red + (int)pixel2_3->green + (int)pixel2_3->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel2_3;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel2_3;
                    }
                    tempSum = (int)pixel3_1->red + (int)pixel3_1->green + (int)pixel3_1->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel3_1;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel3_1;
                    }
                    tempSum = (int)pixel3_2->red + (int)pixel3_2->green + (int)pixel3_2->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel3_2;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel3_2;
                    }
                    tempSum = (int)pixel3_3->red + (int)pixel3_3->green + (int)pixel3_3->blue;
                    if (min_intensity >= tempSum) {
                        min_intensity = tempSum;
                        min_pixel = pixel3_3;
                    }
                    else if (max_intensity < tempSum) {
                        max_intensity = tempSum;
                        max_pixel = pixel3_3;
                    }
                    kernel_red -= ((int)max_pixel->red + (int) min_pixel->red);
                    kernel_green -= ((int)max_pixel->green + (int)min_pixel->green);
                    kernel_blue -= ((int)max_pixel->blue + (int)min_pixel->blue);

                    kernel_red /= kernelScale;
                    kernel_green /= kernelScale;
                    kernel_blue /= kernelScale;

                    // omit the max with 0 check because in blur the values always will be positive.
                    current_pixel.red = (unsigned char)(min(kernel_red, 255));
                    current_pixel.green = (unsigned char)(min(kernel_green, 255));
                    current_pixel.blue = (unsigned char)(min(kernel_blue, 255));

                    pixelsImg[calcIndex(i, j, n)] = current_pixel;
                }
            }
        }

        // its sharp:
        else if (kernel[0][0] == -1) {
            for (i = 1; i < m - 1; ++i) {

                int first_square = (i - 1) * n;
                for (j = 1, x = 0; j < m - 1 - 1; ++j, ++x) {
                    register int kernel_red, kernel_green, kernel_blue;
                    // first line pixels decleration:
                    register pixel* pixel1_1 = &backupOrg[first_square + x];
                    register pixel* pixel1_2 = pixel1_1 + 1;
                    register pixel* pixel1_3 = pixel1_1 + 2;
                    // second line pixels decleration:
                    register pixel* pixel2_1 = pixel1_1 + m;
                    register pixel* pixel2_2 = pixel2_1 + 1;
                    register pixel* pixel2_3 = pixel2_1 + 2;
                    // three line pixels decleration:
                    register pixel* pixel3_1 = pixel2_1 + m;
                    register pixel* pixel3_2 = pixel3_1 + 1;
                    register pixel* pixel3_3 = pixel3_1 + 2;
                    
                    // suming in "one shot" the red, green and blue values of all the pixel in the kernel, and multiple by -1 all of them except the middle one
                    // which mult by 9(for sharping)
                    kernel_red = (int)multiply9(pixel2_2->red) + (-1) * ((int)pixel1_1->red + (int)pixel1_2->red + (int)pixel1_3->red +
                        (int)pixel2_1->red + (int)pixel2_3->red + (int)pixel3_1->red + (int)pixel3_2->red + (int)pixel3_3->red);
                   
                    kernel_green = (int)multiply9(pixel2_2->green) + (-1) * ((int)pixel1_1->green + (int)pixel1_2->green + (int)pixel1_3->green + 
                        (int)pixel2_1->green + (int)pixel2_3->green + (int)pixel3_1->green + (int)pixel3_2->green + (int)pixel3_3->green);
            
                    kernel_blue = (int)multiply9(pixel2_2->blue) + (-1) * ((int)pixel1_1->blue + (int)pixel1_2->blue + (int)pixel1_3->blue +
                        (int)pixel2_1->blue + (int)pixel2_3->blue + (int)pixel3_1->blue + (int)pixel3_2->blue + (int)pixel3_3->blue);
                    // making the rgb first be the maximum between the value which got before to 0, and after
                    // maing him (red, blue and green seperate) be the min between the value to 255) 
                    current_pixel.red = (unsigned char)min(max(kernel_red, 0), 255);
                    current_pixel.green = (unsigned char)min(max(kernel_green, 0), 255);
                    current_pixel.blue = (unsigned char)min(max(kernel_blue, 0), 255);
                    pixelsImg[calcIndex(i, j, n)] = current_pixel;
                }
            }
        }
    }
    free(backupOrg);
}

void myfunction(Image *image, char* srcImgpName, char* blurRsltImgName, char* sharpRsltImgName, char* filteredBlurRsltImgName, char* filteredSharpRsltImgName, char flag) {

    /*
    * [1, 1, 1]
    * [1, 1, 1]
    * [1, 1, 1]
    */
    int blurKernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

    /*
    * [-1, -1, -1]
    * [-1, 9, -1]
    * [-1, -1, -1]
    */
    int sharpKernel[3][3] = {{-1,-1,-1},{-1,9,-1},{-1,-1,-1}};

    if (flag == '1') {
        // blur image
        doConvolution(image, 3, blurKernel, 9, false);

        // write result image to file
        writeBMP(image, srcImgpName, blurRsltImgName);

        // sharpen the resulting image
        doConvolution(image, 3, sharpKernel, 1, false);

        // write result image to file
        writeBMP(image, srcImgpName, sharpRsltImgName);
    } else {
        // apply extermum filtered kernel to blur image
        doConvolution(image, 3, blurKernel, 7, true);

        // write result image to file
        writeBMP(image, srcImgpName, filteredBlurRsltImgName);

        // sharpen the resulting image
        doConvolution(image, 3, sharpKernel, 1, false);

        // write result image to file
        writeBMP(image, srcImgpName, filteredSharpRsltImgName);
    }
}

