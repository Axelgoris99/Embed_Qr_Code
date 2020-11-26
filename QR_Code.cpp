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
    int tailleModule = 9;
    qr = generateQrPic(qrTab, tailleModule,0);
    ref = generateQrPic(qrRef, tailleModule,1);
    Mat pic;
    pic = loadImage();
    int tailleImage = qr.rows;
    pic = scalePic(pic, tailleImage);
    Mat blueNoise;
    cout << " On définit le bruit qui est : blue_noise.jpg ";
    blueNoise = loadImage();
    blueNoise = scalePic(blueNoise, tailleImage);
    //alphaBlend(qr, pic, ref);
    Mat mask;
    int px = 3;
    mask = pixelMaskSelect(px, tailleImage, tailleModule );
    vector<vector<int>> luminance;
    luminance = luminanceSelect(qr, mask, blueNoise);
    Mat finalImage;
    finalImage = finalColor(ref, pic, luminance);
    return EXIT_SUCCESS;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
