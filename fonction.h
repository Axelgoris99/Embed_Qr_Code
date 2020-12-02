#pragma once
#include <string>
#include <vector>
#include "qrCode.h"
/* Ce fichier nous sert pour d�finir des fonctions interm�diaires lors de la cr�ation de notre QR Code */
//LA fonction des QR CODE
qrCode qrGen(std::string const& input);

//Conversion de chaine de caract�re vers hexadecimal
std::string stringToHex(const std::string& input);
//Conversion de hexadecimale vers binaire
std::string hexToBin(const std::string& s);
//Je devrai juste rajouter un argument pour la taille mais bon, trop tard pour �a
//Conversion de entier vers binaire
std::vector<int> intToBin(int aConvertir);
//Conversion de entier vers binaire � 8 bit
std::vector<int> intTo8Bit(int aConvertir);
std::vector<int> intToBinFormat(int aConvertir);

//Une p�nalit� de mask
int cond1(std::vector<std::vector<int>> _qrCope);
int cond2(std::vector<std::vector<int>> _qrCope);
int cond3(std::vector<std::vector<int>> _qrCope);
int cond4(std::vector<std::vector<int>> _qrCope);

//Les diff�rents types de mask
void mask0(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);
void mask1(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);
void mask2(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);
void mask3(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);
void mask4(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);
void mask5(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);
void mask6(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);
void mask7(std::vector<std::vector<int>>& _qrCope, std::vector<std::vector<int>>& const _qrRef);