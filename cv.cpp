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
    //else { imwrite("result/ref.jpg", qrCode); }
    //afficherImage("Notre premier QrCode en image !", qrCode);

    return qrCode;
}

Mat loadImage(string const& image)
{
    Mat pic;
    string chemin("Image/");
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

    //imwrite("result/mask.jpg", mask);
    return mask;
}

/* ===============LUMINANCE SELECTION ==================*/
vector<vector<float>> luminanceSelect(Mat const& qr,Mat const& mask, Mat const& noise)
{   
    //Le masque est déjà en niveau de gris mais pas le reste
    Mat qrGray;
    Mat noiseGray;
    cvtColor(qr, qrGray, COLOR_BGR2GRAY);
    cvtColor(noise, noiseGray, COLOR_BGR2GRAY);
    cvtColor(noise, noiseGray, COLOR_BGR2GRAY);

    int seuilNoise = 128;
    vector<vector<float>> luminance;
    int ligne = qr.rows;
    int colonne = qr.cols;
    for (int i = 0; i < ligne; i++)
    {
        vector<float> ligne;
        luminance.push_back(ligne);
        for (int j = 0; j < colonne; j++)
        {
            //cout << "noiseGray" <<  (int)noiseGray.at<uchar>(i, j) << endl;
            //cout << "mask " <<(int) mask.at<uchar>(i, j) << endl;
            //On vient faire tous les tests comme définie en fonction
            if (noiseGray.at<uchar>(i, j) >= seuilNoise || mask.at<uchar>(i, j) >= 128)
            {
                //cout << "qrGray" << (int)qrGray.at<uchar>(i, j) << endl;
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
vector<vector<float>> luminanceSelectLocal(Mat const& qr, Mat const& mask, Mat const& noise, Mat const& pic, int tailleLocal)
{
    //Le masque est déjà en niveau de gris mais pas le reste
    Mat qrGray;
    Mat noiseGray;
    cvtColor(qr, qrGray, COLOR_BGR2GRAY);
    cvtColor(noise, noiseGray, COLOR_BGR2GRAY);

    int seuilNoise = 128;
    vector<vector<float>> luminance;


    int ligne = qr.rows;
    int colonne = qr.cols;
    int tailleLocalI = tailleLocal;
    int tailleLocalJ = tailleLocal;

    float alpha = 0.6;
    float beta = 0.4;
    float alphaC = 0.9;
    float betaC = 0.1;
    //On va déterminer des carrés de manière locale et dans ses carrés, on va avoir une luminance différente
    //On rajoute un +1 pour ne pas avoir de problème de taille;
    int localX = (int)floor(ligne / tailleLocal)+1;
    int localY = (int)floor(colonne / tailleLocal)+1;

    vector<vector<vector<float>>> luminanceLocal;
    luminanceLocal.resize(ligne);
    for (int i = 0; i < ligne; i++)
    {
        luminanceLocal[i].resize(colonne);

    }

    for (int i = 0; i < localX; i++)
    {
        for (int j = 0; j < localY; j++)
        {
            //Les luminances du carré
            float lumiMax =0;
            float lumiMin =1;
            //vu qu'on a rajouté un +1, si jamais, ça dépasse, on s'assure que ça ne dépasse pas aussi bien sur i que sur j
            if (i * tailleLocal + tailleLocal > ligne) { tailleLocalI = ligne - i * tailleLocal; }
            else { tailleLocalI = tailleLocal; }
            if (j * tailleLocal + tailleLocal > colonne) { tailleLocalJ = ligne - j * tailleLocal; }
            else { tailleLocalJ = tailleLocal; }

            //cout << "tailleLocalI" << tailleLocalI << " tailleLocalJ" << tailleLocalJ << endl;
            //cout << i * tailleLocal + tailleLocalI<<endl;
            //cout << i * tailleLocal + tailleLocalJ << endl;
            for (int k = 0; k < tailleLocalI; k++)
            {
                for (int n = 0; n < tailleLocalJ; n++)
                {
                    
                    Vec3b local;
                    local = pic.at<Vec3b>(i * tailleLocal + k, j * tailleLocal + n);
                    
                    float lumi = bgr2HSL(local[0], local[1], local[2])[2];
                    if (lumi > lumiMax) { lumiMax = lumi; }
                    if (lumi < lumiMin) { lumiMin = lumi; }
                }
            }

            for (int k = 0; k < tailleLocalI; k++)
            {
                for (int n = 0; n < tailleLocalJ; n++)
                {   
                    luminanceLocal[i*tailleLocal+k][j*tailleLocal+n].push_back(lumiMin);
                    luminanceLocal[i*tailleLocal+k][j*tailleLocal+n].push_back(lumiMax);
                   
                }
            }
        }
    }
    //On met la liste de luminance à jour
    for (int i = 0; i < ligne; i++)
    {
        vector<float> ligne;
        luminance.push_back(ligne);
        for (int j = 0; j < colonne; j++)
        {
            //lumiMax est en deuxième position et lumiMin en premier
            //cout << "i " << i << " j " << j << endl;
            float lumiMin= luminanceLocal[i][j][0];
            float lumiMax = luminanceLocal[i][j][1];
            //Pour avoir une luminance qui n'est pas partout la même
            beta = (lumiMax + lumiMin) / 2 * 0.7;
            alpha = (lumiMax + lumiMin) / 2 * 1.3;
            if (lumiMin > beta) { lumiMin = beta; }
            if(lumiMin > 0.3){ lumiMin = 0.3; }
            betaC = lumiMin;
            if (lumiMax < alpha ) { lumiMax = alpha; }
            if (lumiMax < 0.7 ) { lumiMax = 0.7; }
            alphaC = lumiMax;
            //cout << "noiseGray" <<  (int)noiseGray.at<uchar>(i, j) << endl;
            //cout << "mask " <<(int) mask.at<uchar>(i, j) << endl;
            //On vient faire tous les tests comme définie en fonction
            if (noiseGray.at<uchar>(i, j) >= seuilNoise || mask.at<uchar>(i, j) >= 128)
            {
                //cout << "qrGray" << (int)qrGray.at<uchar>(i, j) << endl;
                if (mask.at<uchar>(i, j) < 128 && noiseGray.at<uchar>(i, j) >= seuilNoise)
                {
                    if (qrGray.at<uchar>(i, j) < 128)
                    {
                        luminance[i].push_back(beta);
                    }
                    else {
                        luminance[i].push_back(alpha);
                    }
                }
                if (mask.at<uchar>(i, j) >= 128)
                {
                    if (qrGray.at<uchar>(i, j) < 128)
                    {
                        luminance[i].push_back(betaC);
                    }
                    else { luminance[i].push_back(alphaC); }
                }
            }
            else { luminance[i].push_back(-1); }
        }
    }
    return luminance;
}

/*=========================L'IMAGE FINALE ==================*/
void divEuclidienne(float& h, int modulo) {
    while (h > modulo) { h -= modulo; }
}

vector<float>  bgr2HSL(float b, float g, float r)
{
    //cout << " 1 : b " << b << " g " << g << " r " << r << endl;
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
    //Pour H et S
    if (delta == 0)
    {
        H = 0;
        S = 0;
    }
    else {
        //S
        S = delta / (1 - abs(2 * L - 1));
        //H
        if (cMax == r)
        {
            float x = (g - b) / delta;
            divEuclidienne(x, 6);
            H = 60 * x;
        }
        if (cMax == g)
        {
            H = 60 * (((b - r) / delta)+2);
        }
        if (cMax == b)
        {
            H = 60 * (((r - g) / delta)+4);
        }
    }

    vector<float> hsl(3,0);
    hsl[0] = H;
    hsl[1] = S;
    hsl[2] = L;
    //cout << " 1 : h " << H << " s " << S << " l " << L << endl;
    return hsl;
}


vector<float> hsl2BGR(float h, float s, float l)
{
    float C = (1 - abs(2 * l - 1)) * s;
    float h2 = h / 60;
    divEuclidienne(h2, 2);
    float X = C * (1 - abs(h2 - 1));
    float m = l - C / 2;
    float r = 0;
    float g = 0;
    float b = 0;
    if (0 <= h && h < 60)
    {
        r = C;
        g = X;
        b = 0;
    }
    if (60 <= h && h < 120)
    {
        r = X;
        g = C;
        b = 0;
    }
    if (120 <= h && h < 180)
    {
        r = 0;
        g = C;
        b = X;
    }
    if (180 <= h && h < 240)
    {
        r = 0;
        g = X;
        b = C;
    }
    if (240 <= h && h < 300)
    {
        r = X;
        g = 0;
        b = C;
    }
    if (300 <= h && h < 360)
    {
        r = C;
        g = 0;
        b = X;
    }

    vector<float> bgr(3,0);
    r = (r + m) * 255;
    b = (b + m) * 255;
    g = (g + m) * 255;

    bgr[2] = r;
    bgr[1] = g;
    bgr[0] = b;

    return bgr;
}

float fL(float h, float s, float l)
{
    vector<float> weight{ 0.298,0.587,0.1140 };
    vector<float> t1;
    t1 = hsl2BGR(h, s, l);
    float L;
    //Pour L, on prend RGB entre 0 et 1
    L = weight[0] * t1[0]/255 + weight[1] * t1[1]/255 + weight[2] * t1[2]/255;
    //cout << "L " << L << endl;
    return L;
}


Vec3b lFinal(Vec3b const& bgr, float const lumi)
{
    float L = 2;
    float valeurActuelle = 255;
    float test = 0;
    float pas = 0.01;
    vector<float> h;
    vector<float> r;

    //cout << "lumi " << lumi;

    /*
    //On fait un test con mais utile là...
    r = { (float)bgr[0], (float)bgr[1], (float)bgr[2] };
    cout << "b " << r[0] << " g " << r[1] << " r " << r[2] << endl;
    h = bgr2HSL((float)bgr[0], (float)bgr[1], (float)bgr[2]);
    cout << "h " << h[0] << " s " << h[1] << " l " << h[2]<<endl;
    r = hsl2BGR(h[0], h[1], h[2]);
    cout << "b " << r[0] << " g " << r[1] << " r " << r[2] << endl;
    h = bgr2HSL(r[0], r[1], r[2]);
    cout << "h " << h[0] << " s " << h[1] << " l " << h[2] <<endl;
    */
    h = bgr2HSL((float)bgr[0], (float)bgr[1], (float)bgr[2]);
    while (test < 1)
    {
        float L2 = abs(fL(h[0], h[1], test) - lumi);
        if (L2 < valeurActuelle)
        {
            valeurActuelle = L2;
            L = test;
        }
        test += pas;
    }
    //Histoire de raffiner encore un peu le calcul de la luminance
    if (L > 0.005) { test = L - 0.005; }
    else { test = 0; }
    float sup = L + 0.005;
    if (sup > 1) { sup = 1; }
    pas = 0.001;
    while (test < sup)
    {
        float L2 = abs(fL(h[0], h[1], test) - lumi);
        if (L2 < valeurActuelle)
        {
            valeurActuelle = L2;
            L = test;
        }
        test += pas;
    }
    //cout << "h " << h[0] << " s " << h[1] << " L " << L << endl;
    h = hsl2BGR(h[0], h[1], L);
    uchar b2 = (uchar)round(h[0]);
    uchar g2 = (uchar)round(h[1]);
    uchar r2 = (uchar)round(h[2]);
    //cout << "b " << b2 << " g " << g2 << " r " << r2 << endl << endl;
    Vec3b b2g2r2(b2, g2, r2);

    return b2g2r2;
}


Mat finalColor(Mat mask, Mat pic, vector<vector<float>> const& luminanceVoulue)
{
    int ligne = pic.rows;
    int colonne = pic.cols;
    cout << "ligne " << ligne << endl;
    cout << "ligne ref " << mask.rows << endl;
    Mat final;
    final.create(ligne, colonne, CV_8UC3);

    for (int i = 0; i < ligne; i++)
    {
        //cout << " i Final Color " << i << endl;
        for (int j = 0; j < colonne; j++)
        {
            //cout << " j Final Color " << j << endl;
            if ((int)mask.at<Vec3b>(i, j)[0] == 128) 
            {
                Vec3b pixFinal;
                Vec3b rgb;
                rgb = pic.at<Vec3b>(i, j);
                float lumi = luminanceVoulue[i][j];
                if (lumi == -1)
                {
                    pixFinal = rgb;
                }
                else {
                    pixFinal = lFinal(rgb, luminanceVoulue[i][j]);
                }
                final.at<Vec3b>(i, j) = pixFinal;
            }
            else 
            {
                final.at<Vec3b>(i, j) = (Vec3b) mask.at<Vec3b>(i, j);
            }
        }
    }

    return final;
}


/* ========= RETIRER ZONE DE SILENCE ==============*/
void retirerBord(Mat & finalImage, int const& tailleModule, Mat const& pic)
{
    int taille = tailleModule * 4;
    int tailleGrand = finalImage.rows;
    for (int i = 0; i < taille; i++)
    {
        for (int j = 0; j < tailleGrand; j++)
        {
            finalImage.at<Vec3b>(i, j) = pic.at<Vec3b>(i, j);
            finalImage.at<Vec3b>(j, i) = pic.at<Vec3b>(j, i);
            finalImage.at<Vec3b>(tailleGrand-i-1, j) = pic.at<Vec3b>(tailleGrand-i-1, j);
            finalImage.at<Vec3b>(j, tailleGrand-1-i) = pic.at<Vec3b>(j, tailleGrand-i-1);
        }
    }
}


/*========= FUSION PAR TRANSPARENCE ====================== */
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




