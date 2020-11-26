#pragma once
#include <opencv2/core.hpp>
#include <vector>
#include <string>

//Fonction afficherImage
//Permet d'afficher une image et d'afficher le texte de la fen�tre
void afficherImage( std::string const& texte, cv::Mat const& image);

//Fonction generateQrPic
//Entree : Le tableau de tableau repr�sentant notre QRCode
//Sortie : L'image sous forme de Mat
//Permet de g�n�rer l'image sous forme de mat en pr�cisant son �chelle et de la sauvegarder dans le dossier r�sultat sous le nom test.jpg
//Affiche l'image
//Le mode permet de d�terminer quelle sera le nom de la sortie
cv::Mat generateQrPic(std::vector<std::vector<int>> const qrTab, int const px, int const mode);

//Fonction alphaBlend
//Entree : La matrice du QrCode, celle de l'image que l'on veut utiliser
//Sortie : L'image fusionn�e des deux sous forme de Mat 
//Permet de g�n�rer l'image sous forme de mat en pr�cisant son �chelle et de la sauvegarder dans le dossier r�sultat sous le nom alphaBlend.jpg
//Affiche l'image
//Evitons les copies inutiles
//POSSEDE DEJA UNE METHODE POUR SCALE
cv::Mat alphaBlend(cv::Mat qr, cv::Mat pic, cv::Mat qrRef);

//Permet de charger l'image et de l'afficher
cv::Mat loadImage();
//Permet de mettre l'image � la bonne �chelle
cv::Mat scalePic(cv::Mat pic, int taille);

//Permet de s�lectionner uniquement les pixels du centre du module
//si l'on connait le nombre de pixel d'un c�t� du module, on peut d�terminer le centre
//Sans souci
//Pour d�terminer le nombre de pixel � modifier, d�terminant pour la robustesse, on le passe en argument
//px = da si on reprend le sch�ma
//Si tailleModule est pair, alors px aussi
//Si tailleModule est impair, px aussi
cv::Mat pixelMaskSelect(int const px, int const tailleMask, int const tailleModule);

/*
Luminance Select
On selectionne dans cette fonction le niveau de luminance du pixel final voulue
Cela d�pend de pas mal de choses : Le qrCode, le mask et le bruit
b (=0.3) if Mi,j = 0 , qi,j = 1, Ipc,(i,j) = 1
a (=0.7) if Mi,j = 0 , qi,j = 0, Ipc,(i,j) = 1
bc (=0) if Mi,j = 1 , qi,j = 1
ac (=1) if Mi,j = 1 , qi,j = 0
Yi,j (= -1) otherwise, where Yi,j is the luminance of the normal image
*/
std::vector<std::vector<int>> luminanceSelect(cv::Mat const& qr, cv::Mat const& mask, cv::Mat const& noise);


/*
Color Select : La derni�re �tape o� on calcule l'image finale
La luminance est d�finie comme :
Y = 0.2989R + 0.5870G + 0.114B
On a RGB -> HSL
puis L* = argmin|f(L)-luminancePixel| pour les valeurs de L
o� Y = f(L) = Transpose(w) * (R,G,B);
w est le poids de la luminance sur les diff�rentes composantes
w = [0.298, 0.587, 0.1140];

*/
cv::Mat finalColor(cv::Mat const& mask, cv::Mat const& pic, std::vector<std::vector<int>> const& luminanceVoulue);