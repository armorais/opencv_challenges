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

Mat applyThreshold(Mat input, int value)
{
    Mat ret;

    return ret;
}

int main(int argc, char **argv)
{
    string path = argv[1];
    int min = stoi(argv[2]);
    int max = stoi(argv[3]);

    // Load OpenCV image withou change anything on their color pattern
    Mat image = imread(path, cv::IMREAD_UNCHANGED);
    //resize(image,image,Size(),0.5,0.5);
    Mat hsv;

    // Get image dimensions
    int cols = image.cols;
    int rows = image.rows;

    cvtColor(image, hsv, COLOR_BGR2HSV);

    Mat mask1, mask2;
    // Creating masks to detect the upper and lower red color.
    inRange(hsv, Scalar(0+min, 120, 70), Scalar(30-max, 255, 255), mask1);
    inRange(hsv, Scalar(150+min, 120, 70), Scalar(180-max, 255, 255), mask2);

    // Generating the final mask
    mask1 = mask1 + mask2;

    Mat kernel = Mat::ones(3,3, CV_32F);
    morphologyEx(mask1,mask1,cv::MORPH_OPEN,kernel);
    morphologyEx(mask1,mask1,cv::MORPH_DILATE,kernel);
    
    // creating an inverted mask to segment out the cloth from the frame
    bitwise_not(mask1,mask2);
    Mat res1, res2, final_output;

    //cv::GaussianBlur(mask1, mask1, cv::Size(1,1),10,10);
    
    // Segmenting the cloth out of the frame using bitwise and with the inverted mask
    bitwise_and(hsv,hsv,res1,mask1);

    cout << mask2.type() << endl;

    cvtColor(res1,res1,COLOR_HSV2BGR);

    showImage(unifyImages({image, res1}, 1));

    return 0;
}
