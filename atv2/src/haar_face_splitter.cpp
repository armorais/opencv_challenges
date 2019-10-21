#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
String left_eye_cascade_name = "../haar_data/haarcascade_mcs_lefteye.xml";
String right_eye_cascade_name = "../haar_data/haarcascade_mcs_righteye.xml";
String mouth_cascade_name = "../haar_data/haarcascade_mcs_mouth.xml";
String face_cascade_name = "../haar_data/haar_face.xml";
CascadeClassifier left_eye_cascade;
CascadeClassifier right_eye_cascade;
CascadeClassifier mouth_cascade;
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

/** @function main */
int main( int argc, const char** argv ) {
  String path = argv[1];
  
  // Read
  Mat image = imread(path, cv::IMREAD_UNCHANGED);

  //resize(image,image,Size(),0.5,0.5);

  // Load the cascades
  if( !left_eye_cascade.load( left_eye_cascade_name ) ){ printf("--(!)Error loading left eye\n"); return -1; };
  if( !right_eye_cascade.load( right_eye_cascade_name ) ){ printf("--(!)Error loading right eye\n"); return -1; };
  if( !mouth_cascade.load( mouth_cascade_name ) ){ printf("--(!)Error loading mouth\n"); return -1; };
  if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face\n"); return -1; };

  // Calls the detection function
  detectAndDisplay( image );
  return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat image ) {
  std::vector<Rect> left_eyes;
  std::vector<Rect> right_eyes;
  std::vector<Rect> mouths;
  std::vector<Rect> faces;
  Mat image_gray;
  
  // Converts to grayscale
  cvtColor( image, image_gray, COLOR_BGR2GRAY );

  // Equalizes the histogram to normalize brightness and contrast
  equalizeHist( image_gray, image_gray );

  // Detects face
  face_cascade.detectMultiScale( image_gray, faces, 1.1, 2, 0, Size(60, 60) );

  for( size_t i = 0; i < faces.size(); i++ ) {
        rectangle(image, faces[i].tl(), faces[i].br(), Scalar( 255, 0, 0 ), 2);

        Mat faceROI = image( faces[0] );
        
        // Detects mouth
        mouth_cascade.detectMultiScale( faceROI, mouths, 1.4, 6, 0, Size(10, 10) );

        for( size_t j = 0; j < mouths.size(); j++ ) {
              rectangle(faceROI, mouths[j].tl(), mouths[j].br(), Scalar( 255, 0, 255 ), 2);
              break;
        }

        // Detects left eye
        left_eye_cascade.detectMultiScale( faceROI, left_eyes, 1.3, 6, 0, Size(10, 10) );

        for( size_t j = 0; j < left_eyes.size(); j++ ) {
              rectangle(faceROI, left_eyes[j].tl(), left_eyes[j].br(), Scalar( 0, 0, 255 ), 2);
              break;
        }

        // Detects right eye
        right_eye_cascade.detectMultiScale( faceROI, right_eyes, 1.4, 6, 0, Size(10, 10) );

        for( size_t j = 0; j < right_eyes.size(); j++ ) {
              rectangle(faceROI, right_eyes[j].tl(), right_eyes[j].br(), Scalar( 0, 0, 0 ), 2);
              break;
        }

        break;
  }
  resize(image, image, Size() ,0.5, 0.5);
  imshow( window_name, image );
  waitKey(0);
}

