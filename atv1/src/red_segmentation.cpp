#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

#define LOWER_RED_MIN 0
#define LOWER_RED_MAX 30
#define UPPER_RED_MIN 150
#define UPPER_RED_MAX 180

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

Mat applyThreshold(Mat input, int value)
{
    Mat ret;

    return ret;
}

void setMaxMin(int *min, int *max, int argMin, int argMax) {
    // Set the min and max malues of red depending on user input
    *min = argMin;
    *max = argMax;

    *min = *min * (LOWER_RED_MAX - LOWER_RED_MIN)/100;
    *max = *max * (UPPER_RED_MAX - UPPER_RED_MIN)/100;
}

int main(int argc, char **argv)
{
    /*
    *   argv[1]: Image Path
    *   argv[2]: Min value of red [0 ... 100]
    *   argv[3]: Max value of red [0 ... 100]
    * 
    *   The values are given in % of the max and min values defined for the red spectrum. For example:
    *   Using the 0-30 for lower red and 150-180 for upper red
    *  
    *   min = 0 and max = 100 -> will get everything from lower 0-30 and upper 150-180
    *   min = 20 and max = 80 -> will get from lower 6-24 and upper 156-174
    *   min = 0 -> will get everything from lower 0-30 and upper 150-180, max will be 100
    * 
    * 
    *   Example of use: ./red_segmentation ../data/car.jpg 0 100
    */

    string path = argv[1];
    int min;
    int max;

    if(!argv[3]) {
        setMaxMin(&min, &max, stoi(argv[2]), 100);
    } else {
        setMaxMin(&min, &max, stoi(argv[2]), stoi(argv[3]));
    }

    // Load OpenCV image without changing anything on their color pattern
    Mat image = imread(path, cv::IMREAD_UNCHANGED);
    Mat image_hsv, mask1, mask2, result;

    // Resize the image to fit low resolution screens
    //resize(image,image,Size(),0.5,0.5);

    // Convert from BGR color space to HSV
    cvtColor(image, image_hsv, COLOR_BGR2HSV);
    
    // Create mask to detect the lower red color (from LOWER_RED_MIN to LOWER_RED_MAX)
    inRange(image_hsv, Scalar(LOWER_RED_MIN+min, 120, 70), Scalar(LOWER_RED_MIN+max, 255, 255), mask1);
    
    // Create mask to detect the upper red color (from LOWER_RED_MIN to LOWER_RED_MAX)
    inRange(image_hsv, Scalar(UPPER_RED_MIN+min, 120, 70), Scalar(UPPER_RED_MIN+max, 255, 255), mask2);

    // Generating the final mask
    mask1 = mask1 + mask2;
 
    // Mask the HSV image with the final mask
    bitwise_and(image_hsv,image_hsv,result,mask1);

    // Convert the result from HSV color space to BGR
    cvtColor(result,result,COLOR_HSV2BGR);

    showImage(unifyImages({image, result}, 1));

    return 0;
}
