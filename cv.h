#pragma once
#include <opencv2/core.hpp>
#include <vector>
#include <string>
 

//Fonction afficherImage
//Permet d'afficher une image et d'afficher le texte de la fenêtre
void afficherImage( std::string const& texte, cv::Mat const& image);

//Fonction generateQrPic
//Entree : Le tableau de tableau représentant notre QRCode
//Sortie : L'image sous forme de Mat
//Permet de générer l'image sous forme de mat en précisant son échelle et de la sauvegarder dans le dossier résultat sous le nom test.jpg
//Affiche l'image
//Le mode permet de déterminer quelle sera le nom de la sortie
cv::Mat generateQrPic(std::vector<std::vector<int>> const qrTab, int const px, int const mode);

//Fonction alphaBlend
//Entree : La matrice du QrCode, celle de l'image que l'on veut utiliser
//Sortie : L'image fusionnée des deux sous forme de Mat 
//Permet de générer l'image sous forme de mat en précisant son échelle et de la sauvegarder dans le dossier résultat sous le nom alphaBlend.jpg
//Affiche l'image
//Evitons les copies inutiles
//POSSEDE DEJA UNE METHODE POUR SCALE
cv::Mat alphaBlend(cv::Mat qr, cv::Mat pic, cv::Mat qrRef);

//Permet de charger l'image et de l'afficher
cv::Mat loadImage(std::string const& image);
//Permet de mettre l'image à la bonne échelle
cv::Mat scalePic(cv::Mat pic, int taille);

//Permet de sélectionner uniquement les pixels du centre du module
//si l'on connait le nombre de pixel d'un côté du module, on peut déterminer le centre
//Sans souci
//Pour déterminer le nombre de pixel à modifier, déterminant pour la robustesse, on le passe en argument
//px = da si on reprend le schéma
//Si tailleModule est pair, alors px aussi
//Si tailleModule est impair, px aussi
cv::Mat pixelMaskSelect(int const px, int const tailleMask, int const tailleModule);

/*
Luminance Select
On selectionne dans cette fonction le niveau de luminance du pixel final voulue
Cela dépend de pas mal de choses : Le qrCode, le mask et le bruit
b (=0.3) if Mi,j = 0 , qi,j = 1, Ipc,(i,j) = 1
a (=0.7) if Mi,j = 0 , qi,j = 0, Ipc,(i,j) = 1
bc (=0) if Mi,j = 1 , qi,j = 1
ac (=1) if Mi,j = 1 , qi,j = 0
Yi,j (= -1) otherwise, where Yi,j is the luminance of the normal image
*/
std::vector<std::vector<float>> luminanceSelect(cv::Mat const& qr, cv::Mat const& mask, cv::Mat const& noise);

//Des fonctions intermédiaires dont on va se servir à divers endroits
std::vector<float> bgr2HSL(float b, float g, float r);
void divEuclidienne(float& h, int modulo);
std::vector<float> hsl2BGR(float h, float s, float l);
float fL(float h, float s, float l);
cv::Vec3b lFinal(cv::Vec3b const& bgr, float const lumi);

//Tout pareil sauf que b et a vont dépendre de la zone local de l'image où ils sont
//On prend une zone de 
std::vector<std::vector<float>> luminanceSelectLocal(cv::Mat const& qr, cv::Mat const& mask, cv::Mat const& noise, cv::Mat const& pic, int tailleLocal);


/*
Color Select : La dernière étape où on calcule l'image finale
La luminance est définie comme :
Y = 0.2989R + 0.5870G + 0.114B
On a RGB -> HSL
puis L* = argmin|f(L)-luminancePixel| pour les valeurs de L
où Y = f(L) = Transpose(w) * (R,G,B);
on prend RGB entre 0 et 1
w est le poids de la luminance sur les différentes composantes
w = [0.298, 0.587, 0.1140];

*/
cv::Mat finalColor(cv::Mat mask, cv::Mat pic, std::vector<std::vector<float>> const& luminanceVoulue);