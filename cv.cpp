#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <string>
#include "cv.h"
#include <vector>

using namespace cv;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


/*============AFFICHER IMAGE =============*/
void afficherImage(string const& texte, Mat const& image)
{
    namedWindow(texte, WINDOW_AUTOSIZE);
    moveWindow(texte, 20, 20);
    imshow(texte, image);
    waitKey(0);
}
/*========== GENERATION D'IMAGE ===============*/
Mat generateQrPic(vector<vector<int>> const qrTab, int const px, int const mode)
{
    Mat qrCode;
    int taille = qrTab.size();
    //Attention à bien mettre CV_8UC3 parceque sinon il râle way down the road (dans alpha blending pour la fusion)
    qrCode.create(px * taille, px * taille, CV_8UC3);
    for (int i = 0; i < taille; i++)
    {
        for (int j = 0; j < taille; j++)
        {
            //On peut faire un switch ça serait peut-être plus propre
            if (qrTab[i][j] == 0)
            {
                for (int k = 0; k < px; k++)
                {
                    for (int n = 0; n < px; n++)
                    {
                        Vec3b blanc((uchar)255, (uchar)255, (uchar)255);
                        qrCode.at<Vec3b>(px * i + k, px * j + n) = (Vec3b)blanc;
                        //qrCode.at<uchar>(px * i + k, px * j + n) = (uchar)255;
                    }
                }
            }
            if(qrTab[i][j] == 1){
                for (int k = 0; k < px; k++)
                {
                    for (int n = 0; n < px; n++)
                    {
                        Vec3b noir((uchar)0, (uchar)0, (uchar)0);
                        qrCode.at<Vec3b>(px * i + k, px * j + n) = (Vec3b)noir;
                        //qrCode.at<uchar>(px * i + k, px * j + n) = (uchar)0;
                    }
                }
            }
            if(qrTab[i][j] != 0 && qrTab[i][j] != 1) {
                for (int k = 0; k < px; k++)
                {
                    for (int n = 0; n < px; n++)
                    {
                        Vec3b gris((uchar)128, (uchar)128, (uchar)128);
                        qrCode.at<Vec3b>(px * i + k, px * j + n) = (Vec3b)gris;
                    }
                }
            }
        }
    }
    if (mode == 0)
    {
        imwrite("result/test.jpg", qrCode);
    }
    else { imwrite("result/ref.jpg", qrCode); }
    //afficherImage("Notre premier QrCode en image !", qrCode);

    return qrCode;
}

Mat loadImage()
{
    Mat pic;
    string chemin("Image/");
    string image;
    cout << "Entrez le nom de l'image que vous voulez utilisez \n";
    getline(cin, image);
    string fullPic = chemin.append(image);
    cout << fullPic << endl;
    pic = imread(chemin, IMREAD_COLOR);
    //afficherImage("Test", pic);

    return pic;
}

/*============== SCALING =====================*/

Mat scalePic(cv::Mat pic, int taille)
{
    Mat scaleDown;
    float scale = pic.rows / (float)taille;
    scaleDown.create(taille, taille, pic.type());
    for (int i = 0; i < taille; i++)
    {
        for (int j = 0; j < taille; j++)
        {
            scaleDown.at<Vec3b>(i, j) = pic.at<Vec3b>((int)floor(i * scale), (int)floor(j * scale));
        }
    }
    
    //afficherImage("Scaled", scaleDown);
    return scaleDown;
}

/*======================= MASK ======================*/
cv::Mat pixelMaskSelect(int const px, int const tailleMask, int const tailleModule)
{
    Mat mask;
    mask.create(tailleMask, tailleMask, CV_8U);
    mask = Mat::zeros(tailleMask, tailleMask, CV_8U);
    //même si normalement ça doit etre genre 291/9 vu qu'on le construit comme ça, sait on jamais
    int parcours = (int)floor(tailleMask / tailleModule);
    //poour se décaler du bord du module et commencer à la bonne case
    int decalage = (int)floor((tailleModule - px) / 2);
    for (int i = 0; i < parcours; i++)
    {
        for (int j = 0; j < parcours; j++)
        {
            for (int k = 0; k < px; k++)
            {
                for (int n = 0; n < px; n++)
                {
                    mask.at<uchar>(i * tailleModule + decalage + k, j * tailleModule + decalage + n) = (uchar)255;
                }
            }
        }
    }

    imwrite("result/mask.jpg", mask);
    return mask;
}

/* ===============LUMINANCE SELECTION ==================*/
vector<vector<int>> luminanceSelect(Mat const& qr,Mat const& mask, Mat const& noise)
{   
    //Le masque est déjà en niveau de gris mais pas le reste
    Mat qrGray;
    Mat noiseGray;
    
    cvtColor(qr, qrGray, COLOR_BGR2GRAY);
    cvtColor(noise, noiseGray, COLOR_BGR2GRAY);

    int seuilNoise = 128;
    vector<vector<int>> luminance;
    int ligne = qr.rows;
    int colonne = qr.cols;
    for (int i = 0; i < ligne; i++)
    {
        vector<int> ligne;
        luminance.push_back(ligne);
        for (int j = 0; j < colonne; j++)
        {
            if (noiseGray.at<uchar>(i, j) >= seuilNoise || mask.at<uchar>(i, j) >= 128)
            {
                if (mask.at<uchar>(i, j) < 128 && noiseGray.at<uchar>(i, j) >= seuilNoise)
                {
                    if (qrGray.at<uchar>(i, j) < 128)
                    {
                        luminance[i].push_back(0.3);
                    }
                    else {
                        luminance[i].push_back(0.7);
                    }
                }
                if (mask.at<uchar>(i, j) >= 128)
                {
                    if (qrGray.at<uchar>(i, j) < 128)
                    {
                        luminance[i].push_back(0);
                    }
                    else { luminance[i].push_back(1); }
                }
            }
            else{ luminance[i].push_back(-1); }

        }
    }
    return luminance;
}

/*=========================L'IMAGE FINALE ==================*/
vector<float>  bgr2HSL(float b, float g, float r)
{
    b = b / 255;
    g = g / 255;
    r = r / 255;
    float cMax = r;
    float cMin = r;
    if (b > cMax) { cMax = b; }
    if (g > cMax) { cMax = g; }
    if (b < cMin) { cMin = b; }
    if (g < cMin) { cMin = g; }
    float H;
    float S; 
    float L;
    float delta = cMax - cMin;
    //Pour L
    L = (cMax + cMin) / 2;
    if (delta == 0)
    {
        H = 0;
        S = 0;
    }
    else {
        S = delta / (1 - abs(2 * L - 1));
        if (cMax == r)
        {
            H = 60*(())
        }
        if (cMax == g)
        {

        }
        if (cMax == b)
        {

        }
    }


    vector<float> hsl;
    hsl[0] = H;
    hsl[1] = S;
    hsl[2] = L;
    return hsl;
}

vector<float> hsl2BGR(float h, float s, float l)
{


}

float fL(float h, float s, float l)
{

}
Vec3b lFinal(Vec3b const & bgr, float const lumi)
{
    float L=2;
    float valeurActuelle = 255;
    float test = 0;
    float pas = 0.01;
    vector<float> weight{ 0.298,0.587,0.1140 };
    vector<float> h;
    h = bgr2HSL(bgr[0], bgr[1], bgr[2]);
    while (test < 1)
    {
        float L2 = abs(fL(h[0], h[1], test) - lumi);
        if ( L2 < valeurActuelle)
        {
            valeurActuelle = L2;
            L = test;
        }
        test += pas;
    }
    h = hsl2BGR(h[0], h[1], L);
    Vec3b b2g2r2;
    b2g2r2[0] = h[0];
    b2g2r2[1] = h[1];
    b2g2r2[2] = h[2];
    return b2g2r2;
}


Mat finalColor(Mat const& mask, Mat const& pic, vector<vector<int>> const& luminanceVoulue)
{
    int ligne = pic.rows;
    int colonne = pic.cols;
    Mat final;
    for (int i = 0; i < ligne; i++)
    {
        for (int j = 0; j < colonne; j++)
        {
            Vec3b pixFinal;
            final.at<Vec3b>(i, j) = pixFinal;
        }
    }

    return final;
}

/*========= TENTATIVE DE FUSION */
Mat alphaBlend(Mat qr, Mat pic, Mat qrRef)
{
    double alpha = 0.5; double beta; double input;
    Mat dst;
    cout << " Simple Linear Blender " << endl;
    cout << "-----------------------" << endl;
    cout << "* Enter alpha [0.0-1.0]: ";
    cin >> input;
    // We use the alpha provided by the user if it is between 0 and 1
    if (input >= 0 && input <= 1)
    {
        alpha = input;
    }




    //===============SCALE pic================//
    Mat scaleDown;
    int nbL = qr.rows;
    float scale = (pic.rows / (float)nbL);
    scaleDown.create(nbL, nbL, pic.type());
    Vec3b gris((uchar)128, (uchar)128, (uchar)128);
    for (int i = 0; i < nbL; i++)
    {
        for (int j = 0; j < nbL; j++)
        {
            if (qrRef.at<Vec3b>(i, j) == gris) {
                scaleDown.at<Vec3b>(i, j) = pic.at<Vec3b>((int)floor(i * scale), (int)floor(j * scale));
            }
            else {
                
                scaleDown.at<Vec3b>(i, j) = qr.at<Vec3b>(i, j);
            }
        }
    }
    beta = (1.0 - alpha);
    Mat test;
    test = qr;
    addWeighted(scaleDown, alpha, test, beta, 0.0, dst);
    imwrite("result/alphaBlend.jpg", dst);
    afficherImage("Linear Blend", dst);
    return dst;
}




