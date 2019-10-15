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

int main(int argc, char **argv)
{
    string path = argv[1];
    double sigma = stoi(argv[2]);
    int x = stoi(argv[3]);
    int y = stoi(argv[4]);

    Mat image = imread(path);
    Mat kernel_x, kernel_y, kernel, mask, output;
    Rect roi;
    
    resize(image,image,Size(), 0.8,0.8);

    // getGaussianKernel recieves (aperture size, gaussian standard deviation, type of filter coefficients)
    kernel_x = getGaussianKernel(int(2.5*image.cols), sigma, CV_32F);
    kernel_y = getGaussianKernel(int(2.5*image.rows), sigma, CV_32F);

    kernel = (kernel_y * kernel_x.t());
    
    mask = 255 * kernel / norm(kernel);

    int offset_x = int(image.cols)/2;
    int offset_y = int(image.rows)/2;

    x = (-2*x)+200;
    y = 2*y;

    double a = ((x - 50.0)/100.0);
    double b = ((y - 50.0)/100.0);
    roi.x = int(offset_x + (offset_x * a));
    roi.y = int(offset_y + (offset_y * b));

    roi.width = image.cols;
    roi.height = image.rows;

    mask = mask(roi);

    mask.convertTo(mask, CV_8U, 255);
    cvtColor(mask, mask, COLOR_GRAY2RGB);
    mask.convertTo(mask, CV_32F, 1.0 / 255);

    multiply(image, mask, output, 1, CV_8U);

    showImage(unifyImages({image, output}, 1));
}
