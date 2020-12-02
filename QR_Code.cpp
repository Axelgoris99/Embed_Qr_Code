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

void embedQrPic(string const& input, string const& entre, string const& sortie, string const& bruit);

int main(void )
{
    //Normalement tout marche pour générer des QR code
    string message;
    cout << "Entrez la phrase a convertir \n";
    getline(cin, message);

    string image;
    cout << "Entrez le nom de l'image que vous voulez utilisez \n";
    getline(cin, image);

    string sortie;
    cout << "Entrez le nom de l'image avec le Qr Code en sortie \n";
    getline(cin, sortie);

    string noise;
    cout << " On definit le bruit qui est de la forme : blue_noise.jpg \n";
    getline(cin, noise);

    embedQrPic(message, image, sortie, noise);

    return EXIT_SUCCESS;
}


void embedQrPic(string const& input, string const& entre, string const& sortie, string const& bruit) {

    //On génère le Qr Code
    qrCode coco = qrGen(input);
    coco.afficherQr();
    vector<vector<int>> qrTab = coco.getQr();
    vector<vector<int>> qrRef = coco.getRef();
    Mat qr;
    Mat ref;
    coco.afficherRef();
    int tailleModule = 15;
    qr = generateQrPic(qrTab, tailleModule, 0);
    ref = generateQrPic(qrRef, tailleModule, 1);
    //On charge l'image en entrée
    Mat pic;
    pic = loadImage(entre);
    int tailleImage = qr.rows;
    Size size(tailleImage, tailleImage);
    cv::resize(pic, pic, size);
    //On charge le bruit et sélectionne le mask
    Mat blueNoise;
    blueNoise = loadImage(bruit);
    cv::resize(blueNoise, blueNoise, size);
    Mat mask;
    int px = 3;
    mask = pixelMaskSelect(px, tailleImage, tailleModule);
    //On sélectionne les luminances voulues des pixels
    vector<vector<float>> luminance;
    int selectLocal = 1;
    luminance = luminanceSelectLocal(qr, mask, blueNoise, pic, selectLocal);
    
    //L'image finale
    Mat finalImage;
    finalImage = finalColor(ref, pic, luminance);
    //afficherImage("L'ultime fusion", finalImage);
    //Pas optimisé car on repasse sur ce que l'on a déjà fais mais je ne sais pas si ça marche bien comme ça
    //retirerBord(finalImage, tailleModule, pic);

    string output("result/");
    output += sortie + ".jpg";
    imwrite(output, finalImage);
}