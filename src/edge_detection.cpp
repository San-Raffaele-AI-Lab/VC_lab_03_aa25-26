#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "../include/image.h"

#define M_PI 3.14159265358979323846


/*
Smooths a grayscale image by convolving it with a Gaussian kernel of standard deviation sigma.
Input:
    Image im: the input image
    float sigma: the standard deviation of the Gaussian kernel
Output:
    Image: the smoothed image
*/
Image smooth_image(const Image& im, float sigma)
{
    Image filter = make_gaussian_filter(sigma);
    Image im_smooth = convolve_image(im, filter, false);

    return im_smooth;
}


/*
Computes the magnitude and direction of the gradient of an image.
Input:
    Image im: the input image
Output:
    pair<Image,Image>: the magnitude and direction of the gradient of the image
                       with magnitude in [0,1] and direction in [-pi,pi]
*/
pair<Image,Image> compute_gradient(const Image& im)
{
    pair<Image, Image> sobel = sobel_image(im);
    Image mag = sobel.first;
    Image dir = sobel.second;
    feature_normalize(mag);

    return{mag, dir};
}


/*
Performs non-maximum suppression on an image.
Input:
    Image mag: the magnitude of the gradient of the image [0,1]
    Image dir: the direction of the gradient of the image [-pi,pi]
Output:
    Image: the image after non-maximum suppression
*/
Image non_maximum_suppression(const Image& mag, const Image& dir)
{
    Image nms(mag.w, mag.h, 1);
    float neighbor1, neighbor2;

    // Iterate through the image and perform non-maximum suppression
    for (int y = 0; y < mag.h; y++) {
        for (int x = 0; x < mag.w; x++) {
            float direction = dir(x, y, 0);
            if (direction < 0) {
                direction += M_PI;
            }

            // Round the direction to the nearest multiple of PI/4
            // Get the magnitude of the gradient of the two neighbors along that direction
            int angle = abs(round(direction / (M_PI / 4)));
            switch (angle)
            {
            case 1:
                neighbor1 = mag.clamped_pixel(x - 1, y - 1, 0);
                neighbor2 = mag.clamped_pixel(x + 1, y + 1, 0);
                break;
            case 2:
                neighbor1 = mag.clamped_pixel(x, y - 1, 0);
                neighbor2 = mag.clamped_pixel(x, y + 1, 0);
                break;
            case 3:
                neighbor1 = mag.clamped_pixel(x + 1, y - 1, 0);
                neighbor2 = mag.clamped_pixel(x - 1, y + 1, 0);
                break;
            case 0:
            case 4:
                neighbor1 = mag.clamped_pixel(x - 1, y, 0);
                neighbor2 = mag.clamped_pixel(x + 1, y, 0);
                break;
            default:
                printf("Error: angle not recognized %d\n", angle);
                break;
            }

            // If the magnitude of the gradient of the current pixel is greater than that of both neighbors,
            // then it is a local maximum
            if (mag(x, y, 0) >= neighbor1 && mag(x, y, 0) >= neighbor2) {
                nms(x, y, 0) = mag(x, y, 0);
            }
            else {
                nms(x, y, 0) = 0;
            }
        }
    }

    return nms;
}



/*
    Applies double thresholding to an image.
    Input:
        Image im: the input image
        float lowThreshold: the low threshold value
        float highThreshold: the high threshold value
        float strongVal: the value to use for strong edges
        float weakVal: the value to use for weak edges
    Output:
        Image: the thresholded image
*/
Image double_thresholding(const Image& im, float lowThreshold, float highThreshold, float strongVal, float weakVal)
{
    Image res(im.w, im.h, im.c);

    for (int y = 0; y < im.h; y ++) {
        for (int x = 0; x < im.w; x ++) {
            // strong
            if(im(x,y,0) >= highThreshold)
                res(x,y,0) = strongVal;
            // weak
            else if (im(x,y,0) >= lowThreshold)
                res(x,y,0) = weakVal;
            // zeros
            else
                res(x,y,0) = 0.0;
        }
    }
    return res;
}


/*
    Applies hysteresis thresholding to an image.
    Input:
        Image im: the input image
        float weak: the value of the weak edges
        float strong: the value of the strong edges
    Output:
        Image: the image after hysteresis thresholding, with only strong edges
*/
Image edge_tracking(const Image& im, float weak, float strong)
{
    Image res(im.w, im.h, im.c);

    for (int y=0; y < im.h; ++y) {
        for (int x=0; x < im.w; ++x) {
            if (im(x,y,0) == strong)
                res(x,y,0) = 1.0;
            else if(im(x,y,0) == weak){
                if((im.clamped_pixel(x-1, y-1, 0) == strong) or
                   (im.clamped_pixel(x,   y-1, 0) == strong) or
                   (im.clamped_pixel(x+1, y-1, 0) == strong) or
                   (im.clamped_pixel(x-1, y,   0) == strong) or
                   (im.clamped_pixel(x+1, y,   0) == strong) or
                   (im.clamped_pixel(x-1, y+1, 0) == strong) or
                   (im.clamped_pixel(x,   y+1, 0) == strong) or
                   (im.clamped_pixel(x+1, y+1, 0) == strong))
                    res(x,y,0) = 1.0;
                else
                    res(x,y,0) = 0.0;
            }
            else
                res(x,y,0) = 0.0;
        }
    }
    return res;

}