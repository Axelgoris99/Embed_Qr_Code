/* QR_Code Generator : 
Ce programme a pour but d'être un générateur de QR Code complet. 
Il prendra en compte toutes les versions possibles (1 à 40) mais se limitera au code correcteur d'erreur de qualité High (H)
De plus, il ne marche que pour les charactères alpha numériques.

%%%COPYRIGHT Axel Goris - Aubane Espérance - 2020%%%

*/
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include "qrCode.h"
#include "fonction.h"
#include "cv.h";
#define ESC_KEY 27

using namespace std;
using namespace cv;


int main(void )
{
    //Normalement tout marche pour générer des QR code
    string test;
    cout << "Entrez la phrase a convertir \n";
    getline(cin, test);
    
    qrCode coco = qrGen(test);
    coco.afficherQr();
    vector<vector<int>> qrTab = coco.getQr();
    vector<vector<int>> qrRef = coco.getRef();
    Mat qr;
    Mat ref;
    coco.afficherRef();
    int tailleModule = 15;
    qr = generateQrPic(qrTab, tailleModule,0);
    ref = generateQrPic(qrRef, tailleModule,1);
    Mat pic;
    string image;
    cout << "Entrez le nom de l'image que vous voulez utilisez \n";
    getline(cin, image);
    string sortie;
    cout << "Entrez le nom de l'image avec le Qr Code en sortie \n";
    getline(cin, sortie);
    //image = "4k2.jpg";
    pic = loadImage(image);
    int tailleImage = qr.rows;
    Size size(tailleImage, tailleImage);
    //Notre méthode maison n'est peut-être pas la plus utile...
    //pic = scalePic(pic, tailleImage);
    cv::resize(pic, pic, size);
    //pour vérifier ce qu'on fait au cas où
    imwrite("result/pic.jpg", pic);
    Mat blueNoise;
    string noise;
    //cout << " On définit le bruit qui est : blue_noise.jpg ";
    noise = ("blue_noise.jpg");
    //getline(cin, noise);
    blueNoise = loadImage(noise);
    
    cv::resize(blueNoise, blueNoise, size);
    //blueNoise = scalePic(blueNoise, tailleImage);
    //pour vérifier ce qu'on fait au cas où
    imwrite("result/blueNoise.jpg", blueNoise);
    //alphaBlend(qr, pic, ref);
    Mat mask;
    int px = 3;
    mask = pixelMaskSelect(px, tailleImage, tailleModule );
    vector<vector<float>> luminance;
    int selectLocal = 10;
    if (selectLocal > tailleModule) { selectLocal = tailleModule; }
    luminance = luminanceSelectLocal(qr, mask, blueNoise, pic, selectLocal);

    Mat finalImage;
    finalImage = finalColor(ref, pic, luminance);
    //afficherImage("L'ultime fusion", finalImage);
    string output("result/");
    output += sortie;
    imwrite(output, finalImage);
    return EXIT_SUCCESS;
}