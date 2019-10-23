/*
* Haar face splitter is my solution to the second challenge.
*
* I added some stuff to make it easier to detect some parts of the face, especially the mouth.
* Depending on face given, Sometimes it would detect an eye as a mouth, so now everytime it detects
* an item it paints a filled retangule to take the detected part off the image.
*
* Extra cascades added: 
* Mouth detector by Modesto Castrillon-Santana, distributed in the opencv_contrib's face module (opencv_contrib/modules/face/data/cascades/)
* Nose detector by Modesto Castrillon-Santana, distributed in the opencv_contrib's face module (opencv_contrib/modules/face/data/cascades/)
* 
* Author: Alison Morais
*/

#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

std::vector<Mat> detect(Mat frame);
int loadCascades();
void saveOutput(std::vector<Mat>);

// Paths to the trained classifiers
static String eyes_cascade_name = "../haar_data/haar_eyeglasses.xml";
static String mouth_cascade_name = "../haar_data/haarcascade_mcs_mouth.xml";
static String nose_cascade_name = "../haar_data/haarcascade_mcs_nose.xml";
static String face_cascade_name = "../haar_data/haar_face.xml";

CascadeClassifier mouth_cascade;
CascadeClassifier face_cascade;
CascadeClassifier nose_cascade;
CascadeClassifier eyes_cascade;

static string window_name = "Face splitter";

std::vector<Mat> detect(Mat image)
{
    std::vector<Rect> eyes;
    std::vector<Rect> mouths;
    std::vector<Rect> noses;
    std::vector<Rect> faces;
    std::vector<Mat> output;

    Mat image_gray;

    // Converts input image to grayscale before equalizing
    cvtColor(image, image_gray, COLOR_BGR2GRAY);

    // Equalizes the histogram to normalize brightness and contrast
    equalizeHist(image_gray, image_gray);

    // Tries to detect faces in the equalized image
    face_cascade.detectMultiScale(image_gray, faces, 1.1, 2, 0, Size(60, 60));

    // If there's no face found, returns 0
    
    for (size_t i = 0; i < faces.size(); i++)
    {
        Mat faceROIOutput;
        Mat faceROI = image(faces[0]);
        // Make a copy of the face ROI, because the original faceROI is going to be modified during the detections.
        faceROI.copyTo(faceROIOutput);
        output.push_back(faceROIOutput);

        // Detects eyes inside the face region of interest
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.05, 3, CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30));

        for (size_t j = 0; j < eyes.size(); j++)
        {
            Mat eyeOutput;
            faceROIOutput(eyes[j]).copyTo(eyeOutput); // Copy the found eye to an auxiliar Mat
            output.push_back(eyeOutput); // Save the found eye into the output vector

            // Draw a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROI, eyes[j].tl(), eyes[j].br(), Scalar(0, 0, 0), FILLED);
        }

        // Detects noses inside the face region of interest
        nose_cascade.detectMultiScale(faceROI, noses, 1.3, 6, 0, Size(10, 10));

        for (size_t j = 0; j < noses.size(); j++)
        {
            Mat noseOutput;
            faceROIOutput(noses[j]).copyTo(noseOutput); // Copy the found nose to an auxiliar Mat
            output.push_back(noseOutput); // Save the found nose into the output vector

            // Draw a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROI, noses[j].tl(), noses[j].br(), Scalar(0, 0, 0), FILLED);
            break;
        }

        // Detects mouths inside the face region of interest
        mouth_cascade.detectMultiScale(faceROI, mouths, 1.3, 6, 0, Size(10, 10));

        for (size_t j = 0; j < mouths.size(); j++)
        {
            Mat mouthOutput;
            faceROIOutput(mouths[j]).copyTo(mouthOutput); // Copy the found mouth to an auxiliar Mat
            output.push_back(mouthOutput); // Save the found mouth into the output vector

            // Draw a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROI, mouths[j].tl(), mouths[j].br(), Scalar(0, 0, 0), FILLED);
            break;
        }

        break;
    }

/*     for (int i = 0; i < output.size(); i++)
    {
        imshow(window_name, output[i]);
        waitKey(0);
    } */
    
    return output;
}

int loadCascades()
{
    // Load the face cascade
    if (!face_cascade.load(face_cascade_name))
    {
        cout << "Error loading face cascade." << endl;
        return 0;
    };

    // Load the eyes cascade
    if (!eyes_cascade.load(eyes_cascade_name))
    {
        cout << "Error loading eye cascade." << endl;
        return 0;
    };

    // Load the nose cascade
    if (!nose_cascade.load(nose_cascade_name))
    {
        cout << "Error loading nose cascade." << endl;
        return 0;
    };

    // Load the mouth cascade
    if (!mouth_cascade.load(mouth_cascade_name))
    {
        cout << "Error loading mouth cascade" << endl;
        return 0;
    };

    return 1;
}

void saveOutput(std::vector<Mat> output)
{
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    cout << output.size() <<endl;

    for(int i=0; i<output.size(); i++) {
        imwrite("../output/" + to_string(i) + ".jpg", output[i],compression_params);
    }
}

int main(int argc, const char **argv)
{
    String path = argv[1];
    std::vector<Mat> output;

    // Read input image
    Mat image = imread(path, cv::IMREAD_UNCHANGED);

    // Resize if needed
    //resize(image,image,Size(),0.5,0.5);

    // Load the cascades
    if (!loadCascades())
        return -1;
    
    // Call the detection function and save the result, if not empty
    if(!(output = detect(image)).empty())
        saveOutput(output);
    else
        cout << "No face detected in this image." << endl;
}
