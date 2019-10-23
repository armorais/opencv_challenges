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

std::map<string,Mat> detect(Mat);
int loadCascades();
void saveOutput(std::map<string,Mat>);

// Paths to the trained classifiers
static String eyes_cascade_name = "../haar_data/haar_eyeglasses.xml";
static String mouth_cascade_name = "../haar_data/haarcascade_mcs_mouth.xml";
static String nose_cascade_name = "../haar_data/haarcascade_mcs_nose.xml";
static String face_cascade_name = "../haar_data/haar_face.xml";

CascadeClassifier mouth_cascade;
CascadeClassifier face_cascade;
CascadeClassifier nose_cascade;
CascadeClassifier eyes_cascade;

std::map<string,Mat> output;

static string window_name = "Face splitter";

// Inserts an item to the output map
void addOutput(string region_name, Mat roi) {
    Mat roiOutput;

    // Makes a copy of the face ROI, because the original roi is going to be modified during the detections.
    roi.copyTo(roiOutput);

    output.insert({region_name,roiOutput});
}

// Detects the itens and returns a map containing <region,image>
std::map<string,Mat> detect(Mat image)
{
    std::vector<Rect> eyes;
    std::vector<Rect> mouths;
    std::vector<Rect> noses;
    std::vector<Rect> faces;
    

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
        Mat faceROIOriginal = image(faces[0]);
        Mat faceROI ;
        // Makes a copy of the face ROI, because the original faceROI is going to be modified during the detections.
        faceROIOriginal.copyTo(faceROI);
        addOutput("face", faceROI); // Inserts the found eye into the output map

        // Detects eyes inside the face region of interest
        eyes_cascade.detectMultiScale(faceROIOriginal, eyes, 1.05, 3, CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30));

        for (size_t j = 0; j < eyes.size(); j++)
        {
            addOutput("eye"+ std::to_string(j+1), faceROI(eyes[j])); // Inserts the found eye into the output map

            // Draws a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROIOriginal, eyes[j].tl(), eyes[j].br(), Scalar(0, 0, 0), FILLED);
        }

        // Detects noses inside the face region of interest
        nose_cascade.detectMultiScale(faceROIOriginal, noses, 1.3, 6, 0, Size(10, 10));

        for (size_t j = 0; j < noses.size(); j++)
        {
            addOutput("nose", faceROI(noses[j])); // Inserts the found eye into the output map

            // Draws a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROIOriginal, noses[j].tl(), noses[j].br(), Scalar(0, 0, 0), FILLED);
            break;
        }

        // Detects mouths inside the face region of interest
        mouth_cascade.detectMultiScale(faceROIOriginal, mouths, 1.3, 6, 0, Size(10, 10));

        for (size_t j = 0; j < mouths.size(); j++)
        {
            addOutput("mouth", faceROI(mouths[j])); // Inserts the found eye into the output map

            // Draws a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROIOriginal, mouths[j].tl(), mouths[j].br(), Scalar(0, 0, 0), FILLED);
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

// Loads the cascades
int loadCascades()
{
    // Loads the face cascade
    if (!face_cascade.load(face_cascade_name))
    {
        cout << "Error loading face cascade." << endl;
        return 0;
    };

    // Loads the eyes cascade
    if (!eyes_cascade.load(eyes_cascade_name))
    {
        cout << "Error loading eye cascade." << endl;
        return 0;
    };

    // Loads the nose cascade
    if (!nose_cascade.load(nose_cascade_name))
    {
        cout << "Error loading nose cascade." << endl;
        return 0;
    };

    // Loads the mouth cascade
    if (!mouth_cascade.load(mouth_cascade_name))
    {
        cout << "Error loading mouth cascade" << endl;
        return 0;
    };

    return 1;
}

// Iterates through the output map and export its content  
void saveOutput(std::map<string,Mat> output)
{
    // Compression parameters for imwrite
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    // Clears the output folder (Only works in Linux! I didn't want to include a lib dependency to do cross-plataform checking)
    system("exec rm -r ../output/*");

    // Iterates through the map and saves the results in the output folder
    for(map<string, Mat>::iterator aux=output.begin(); aux!=output.end(); aux++) 
    {
        imwrite("../output/" + aux->first + ".png", aux->second,compression_params);
    }
}

int main(int argc, const char **argv)
{
    String path = argv[1];
    std::map<string,Mat> output;

    // Reads input image
    Mat image = imread(path, cv::IMREAD_UNCHANGED);

    // Resizes the image if needed
    //resize(image,image,Size(),0.5,0.5);

    // Loads the cascades
    if (!loadCascades())
        return -1;
    
    // Calls the detection function and saves the result, if not empty
    if(!(output = detect(image)).empty())
        saveOutput(output);
    else
        cout << "No face detected in this image." << endl;
}
