#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
String left_eye_cascade_name = "../haar_data/haarcascade_mcs_lefteye.xml";
String mouth_cascade_name = "../haar_data/haarcascade_mcs_mouth.xml";
CascadeClassifier left_eye_cascade;
CascadeClassifier mouth_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

/** @function main */
int main( int argc, const char** argv ) {
  String path = argv[1];
  
  // Read
  Mat image = imread(path, cv::IMREAD_UNCHANGED);

  resize(image,image,Size(),0.5,0.5);

  // Load the cascades
  if( !left_eye_cascade.load( left_eye_cascade_name ) ){ printf("--(!)Error loading eye\n"); return -1; };
  if( !mouth_cascade.load( mouth_cascade_name ) ){ printf("--(!)Error loading eye\n"); return -1; };

  // Calls the detection function
  detectAndDisplay( image );
  return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat image ) {
  std::vector<Rect> eyes;
  std::vector<Rect> mouths;
  Rect face;
  Mat image_gray;
  
  // Converts to grayscale
  cvtColor( image, image_gray, COLOR_BGR2GRAY );

  // Equalizes the histogram to normalize brightness and contrast
  equalizeHist( image_gray, image_gray );

  // Detects mouth
  mouth_cascade.detectMultiScale( image_gray, mouths, 1.1, 100, 1|CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30) );


  for( size_t i = 0; i < mouths.size(); i++ ) {
    Point center( mouths[i].x + mouths[i].width*0.5, mouths[i].y + mouths[i].height*0.5 );
    ellipse( image, center, Size( mouths[i].width*0.5, mouths[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    Mat faceROI = image_gray( mouths[i] );
  }
  // Show the results
  imshow( window_name, image );
  waitKey(0);
}

