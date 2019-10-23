/*
* Haar face splitter is my solution to the second challenge.
*
* I added some stuff to make it easier to detect some parts of the face, especially the mouth.
* Depending on face given, Sometimes it would detect an eye as a mouth, so now everytime it detects
* an item it paints a filled retangule to take the detected part off the image.
*
* The output images are saved in the output folder.
*
* You can use the SHOW_IMAGE flag if you want to 
* control if the program shoud show the output images or not.
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

// Flag to control if the program shoud show the output images
const int SHOW_IMAGE = 1; // Change the value to 0 if you don't want to show the image

std::map<String,Mat> detect(Mat);
int loadCascades();
void showImage(String name, Mat image);
void saveOutput(std::map<String,Mat>);

// Paths to the trained classifiers
const String EYES_CASCADE_NAME = "../haar_data/haar_eyeglasses.xml";
const String MOUTH_CASCADE_NAME = "../haar_data/haarcascade_mcs_mouth.xml";
const String NOSE_CASCADE_NAME = "../haar_data/haarcascade_mcs_nose.xml";
const String FACE_CASCADE_NAME = "../haar_data/haar_face.xml";

CascadeClassifier mouth_cascade;
CascadeClassifier face_cascade;
CascadeClassifier nose_cascade;
CascadeClassifier eyes_cascade;

// Detects the itens and returns a map containing <region,image>
std::map<String,Mat> detect(Mat image)
{
    std::vector<Rect> faces;
    std::vector<Rect> eyes;
    std::vector<Rect> noses;
    std::vector<Rect> mouths;

    std::map<string,Mat> output;

    Mat image_gray;

    // Converts input image to grayscale before equalizing
    cvtColor(image, image_gray, COLOR_BGR2GRAY);

    // Equalizes the histogram to normalize brightness and contrast
    equalizeHist(image_gray, image_gray);

    // Tries to detect faces in the equalized image
    face_cascade.detectMultiScale(image_gray, faces, 1.1, 2, CASCADE_FIND_BIGGEST_OBJECT, Size(60, 60));

    // If there's no face found, returns 0
    
    for (size_t i = 0; i < faces.size(); i++)
    {
        Mat faceROIOriginal = image(faces[0]);
        Mat faceROI ;
        // Makes a copy of the face ROI, because the original faceROI is going to be modified during the detections.
        faceROIOriginal.copyTo(faceROI);

        output.insert({"face",faceROIOriginal}); // Inserts the found face into the output map

        // Detects eyes inside the face region of interest
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.05, 3, CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30));

        for (size_t j = 0; j < eyes.size(); j++)
        {
            output.insert({"eyes" + to_string(j+1),faceROIOriginal(eyes[j])}); // Inserts the found eye into the output map

            // Draws a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROI, eyes[j].tl(), eyes[j].br(), Scalar(0, 0, 0), FILLED);
        }

        // Detects noses inside the face region of interest
        nose_cascade.detectMultiScale(faceROI, noses, 1.3, 6, 0, Size(10, 10));

        for (size_t j = 0; j < noses.size(); j++)
        {
            output.insert({"nose",faceROIOriginal(noses[j])}); // Inserts the found nose into the output map

            // Draws a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROI, noses[j].tl(), noses[j].br(), Scalar(0, 0, 0), FILLED);
            break;
        }

        // Detects mouths inside the face region of interest
        mouth_cascade.detectMultiScale(faceROI, mouths, 1.3, 6, 0, Size(10, 10));

        for (size_t j = 0; j < mouths.size(); j++)
        {
            output.insert({"mouth",faceROIOriginal(mouths[j])}); // Inserts the found mouth into the output map

            // Draws a filled rectangle. I used this to "eliminate" already detected regions from the faceROI.
            rectangle(faceROI, mouths[j].tl(), mouths[j].br(), Scalar(0, 0, 0), FILLED);
            break;
        }

        break;
    }

    return output;
}

// Loads the cascades
int loadCascades()
{
    // Loads the face cascade
    if (!face_cascade.load(FACE_CASCADE_NAME))
    {
        cout << "Error loading face cascade." << endl;
        return 0;
    };

    // Loads the eyes cascade
    if (!eyes_cascade.load(EYES_CASCADE_NAME))
    {
        cout << "Error loading eye cascade." << endl;
        return 0;
    };

    // Loads the nose cascade
    if (!nose_cascade.load(NOSE_CASCADE_NAME))
    {
        cout << "Error loading nose cascade." << endl;
        return 0;
    };

    // Loads the mouth cascade
    if (!mouth_cascade.load(MOUTH_CASCADE_NAME))
    {
        cout << "Error loading mouth cascade" << endl;
        return 0;
    };

    return 1;
}

void showImage(String name, Mat image)
{
    imshow(name, image);
    waitKey(0);
}

// Iterates through the output map and export its content  
void saveOutput(std::map<String,Mat> output)
{
    // Compression parameters for imwrite
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    // Clears the output folder (Only works in Linux! I didn't want to include a lib dependency to do cross-plataform checking)
    system("exec rm -r ../output/*");

    // Iterates through the map and saves the results in the output folder
    for(map<String, Mat>::iterator aux=output.begin(); aux!=output.end(); aux++) 
    {
        imwrite("../output/" + aux->first + ".png", aux->second,compression_params);
        if(SHOW_IMAGE)
            showImage(aux->first, aux->second);
    }
}

int main(int argc, const char **argv)
{
    String path = argv[1];
    std::map<String,Mat> output;

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
