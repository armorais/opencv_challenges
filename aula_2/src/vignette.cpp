#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void showImage(Mat image)
{
    // Present imagem on the screen
    imshow("image", image);
    waitKey(0);
}

Mat unifyImages(vector<Mat> images, int orientation)
{
    // Put images with same pattern on one unique image
    Mat output;
    images[0].copyTo(output);
    switch (orientation)
    {
    case 0:
        // Vertical
        for (int i = 1; i < images.size(); i++)
        {
            vconcat(output, images[i], output);
        }
        break;
    case 1:
        // Horizontal
        for (int i = 1; i < images.size(); i++)
        {
            hconcat(output, images[i], output);
        }
        break;
    }

    return output;
}

Rect maskCropRoi(int cols, int rows, int x, int y) {
    // Returns a rectangular region of interest that is gonna be used to crop
    // the mask. to position the center of the vignette effect.
    Rect roi;
    int offset_x = int(cols)/2;
    int offset_y = int(rows)/2;

    x = (-2*x)+200;
    y = 2*y;

    double a = ((x - 50.0)/100.0);
    double b = ((y - 50.0)/100.0);
    roi.x = int(offset_x + (offset_x * a));
    roi.y = int(offset_y + (offset_y * b));

    roi.width = cols;
    roi.height = rows;

    return roi;
}

int main(int argc, char **argv)
{
    /*
    *   argv[1]: Image Path
    *   argv[2]: Gaussian Standard Deviation. Controls the "size" of the effect
    *   argv[3]: Position of the x axis [0 ... 100]
    *   argv[4]: Position of the y axis [0 ... 100]
    * 
    *   The x and y positions are given in %. For example: 
    *   x = 0 and y = 0 -> the focus will be in the bottom left corner of the image 
    *   x = 50 and y = 50 -> the focus will be in the center of the image 
    *   x = 0 and y = 0 -> the focus will be in the top right corner of the image 
    * 
    *   Example: ./vignette ../data/car.jpg 100 50 50
    */
    string path = argv[1];
    double sigma = stoi(argv[2]);
    int x = stoi(argv[3]);
    int y = stoi(argv[4]);

    Mat image = imread(path);
    Mat kernel_x, kernel_y, kernel, mask, output;
    Rect roi;
    
    // Resize the image to fit low resolution screens
    resize(image,image,Size(), 0.8,0.8);

    // Creates smoothing kernels passing aperture size, gaussian standard deviation (sigma) and type of filter
    kernel_x = getGaussianKernel(int(2.5*image.cols), sigma, CV_32F);
    kernel_y = getGaussianKernel(int(2.5*image.rows), sigma, CV_32F);

    // Creates the effect using the gaussian kernels
    kernel = (kernel_y * kernel_x.t());
    mask = 255 * kernel / norm(kernel);

    // Find the rectangular area to crop the mask
    roi = maskCropRoi(image.cols, image.rows, x, y);

    // Uses copy constructor to crop the mask
    mask = mask(roi);

    // These steps are used to convert the mask before using the multipky function
    mask.convertTo(mask, CV_8U, 255);
    cvtColor(mask, mask, COLOR_GRAY2RGB);
    mask.convertTo(mask, CV_32F, 1.0 / 255);

    // Calculate the product of image and the mask, and save it to the output variable
    multiply(image, mask, output, 1, CV_8U);

    showImage(unifyImages({image, output}, 1));
}
