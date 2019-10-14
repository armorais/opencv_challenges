#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void showImage(Mat image) {
  // Present imagem on the screen
  imshow("image", image);
  waitKey(0);
}

Mat unifyImages(vector<Mat> images, int orientation) {
  // Put images with same pattern on one unique image
  Mat output;
  images[0].copyTo(output);
  switch(orientation) {
    case 0:
    // Vertical
      for(int i = 1; i < images.size(); i++) {
        vconcat(output, images[i], output);
      }
    break;
    case 1:
    // Horizontal
      for(int i = 1; i < images.size(); i++) {
        hconcat(output, images[i], output);
      }
    break;
  }
  
  return output;
}

int main(int argc, char ** argv) {
  string path = argv[1];
  double sigma = stoi(argv[2]);
  
      Mat img = imread(path);
    Mat a,b,c,d,e,f;
    //double sigma = 80; // vignette 'aperture', the param to play with
    a = getGaussianKernel(img.cols,sigma,CV_32F);
    b = getGaussianKernel(img.rows,sigma,CV_32F);
    c=b*a.t();
    double minVal;     
    double maxVal;          
    cv::minMaxLoc(c, &minVal, &maxVal);
    d = c/maxVal;
    d.convertTo(d,CV_8U,255);      // hrmff, there should
    cvtColor(d,d,COLOR_GRAY2RGB);  // be a way 
    d.convertTo(d,CV_32F,1.0/255); // without all this muck
    
    multiply(img,d,e,1,CV_8U);
    showImage(unifyImages({img, e}, 1));

}
