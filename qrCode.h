#pragma once
#include <vector>
#include <string>
#include <iostream>

/*
On va majoritairement travailler avec des tableau d'entiers. La chaine de caract�res au d�but sera tr�s vite transform�s en tableau et utilis� ensuite comme un tableau.
Les �tapes :
	Determine which encoding mode to use
	Encode the data
	Generate error correction codewords
	Interleave blocks if necessary
	Place the data and error correction bits in the matrix
	Apply the mask patterns and determine which one results in the lowest penalty
	Add format and version information

*/


class qrCode
{
public:
	/*====================== METHODE =====================*/
	//On passe une chaine de charact�re au constructeur
	qrCode(const std::string& input);

	/*====================== FONCTION POUR LA CREATION DU QR CODE =================*/
	//On choisit la version du QR Code a utilis� en fonction du nombre de caract�res
	void chooseVersion();
	//On encode les donn�es
	//Entr�e : String
	//Sortie : Vector<Int> pour le binaire (On aurait pu prendre vector<bool>)
	void byteModeEncoding();
	//La chaine de caract�re doit avoir une taille pr�cise donc on la compl�te avec
	//Le terminator : 4 z�ros au max
	//Des pad bytes : 11101100 et 00010001 en alternance
	void fillMessage();
	//Le polynome g�n�rateur pour notre code correcteur d'erreur
	//il NE d�pend PAS du bloc que l'on �tudie actuellement
	void generatorPoly();
	//Pour compl�ter en version d�cimale le polynome contenant le message
	void fillMessagePoly();
	//Pour diviser le message decimale en bloc et groupe
	void divideMessagePoly();
	//Pour g�n�rer les codes correcteurs d'erreurs !
	void reedSolomon();
	//Pour interleave les data si la version le n�cessite
	void interleaveData();
	//Pour interleave les ECC si la version le n�cessite
	void interleaveEcc();
	//Convertir en binaire et rajouter les derniers reminder bits
	void remindAndBitFinal();
	
	//Pour mettre nos tableaux � la bonne taille
	void qrSize();
	//Ajouts des finder Pattern;
	void drawPattern();
	//Ajouter des s�parateurs
	void drawSeparator();
	//Ajouter les alignements
	void drawAlignement();
	//Ajouter le "timing" pattern
	void drawTiming();
	//Les zones r�serv�es � ajouter
	void drawReserved();
	//On remplit enfin notre QR CODE !!!!!!!
	void drawData();
	//On applique les 8 masks, on teste les 4 p�nalit�s, et on choisit le mask qui a la moins haute p�nalit�
	void masking();
	//On compl�te les cases de format
	void generateFormat();
	void drawFormat();
	//On compl�te les cases de version si la version est sup�rieure � 7
	void drawVersion();
	//On rajoute la zone silencieuse tout autour de notre QR CODE
	//En gros, 4 module de 0 tout autour de notre QR CODE
	void silentZone();
	//void drawQrCode();
	
	inline void printVersion() { std::cout << "La version est : " << _version << "\n"; }
	inline void printMessage() { std::cout << "Le message est : \n"<< _message << "\n"; }
	//Renvoie la taille de _messageBin
	inline void getTaille() { std::cout << "Les donnees sont de taille : \n" << _messageBin.size() << "\n"; }
	//Renvoie le message en binaire
	void printMessageBin();
	//Renvoie le message sous forme de coef de polynome
	void printMessagePoly();
	//Renvoie le code correcteur d'erreur sous forme de coef de poly
	void printEccPoly();
	//Renvoie le nombre de caractere en binaire
	void printCharCountBin();
	//Renvoie le polynome g�n�rateur
	void printGenPolyCount();
	//Renvoie le message divis� en groupe et bloc
	void printMessageDiv();
	//Renvoie le code correcteur d'erreur divis� en groupe et bloc
	void printEccDiv();
	//Renvoie le message final
	void printFinalMessage();
	void printFinalMessageBin();
	void afficherFormat();
	void afficherQr();
	void afficherRef();
	//La fonction DEBUG
	void debug();

	//A utiliser en dehors 
	inline std::vector<std::vector<int>> getQr() { return _qr; }
	inline std::vector<std::vector<int>> getRef() { return _qrRef; }

protected:
	/*====================== ATTRIBUTS =====================*/
	//Le mode du QR Code utilis� : Ici byte donc 0,1,0,0 mais on pr�voit si jamais le code vient � �voluer
	std::vector<int> _mode;
	//La version que l'on va utiliser en fonction de la taille du programme
	//D�pend du nombre de caract�res � mettre
	int _version;
	//La taille de la chaine de caract�res (doit �tre inf�rieure � 1273 - Version 40-H)
	//Taille = 21+(version-1)*4
	int _taille;
	//Pour Bytes mode : 8 bits pour les versions 1 � 9
	//16 bits pour les versions 10 � 26
	//16 bits pour les versions 27 � 40
	/*Count the number of characters in the original input text, then convert that number into binary. 
	The length of the character count indicator depends on the encoding mode and the QR code version that will be in use.
	To make the binary string the appropriate length, pad it on the left with 0s. */
	int _charCountIndicatorSize;
	std::vector<int> _charCountIndicator;
	//Le message r�cup�r� � la premi�re ligne du programme
	std::string _message;
	//Le message en binaire
	std::vector<int> _messageBin;

	//Les donn�es encod�es sous forme d'une simple liste pour le polynome
	std::vector<int> _messagePoly;
	//On r�partit le messagePoly sous la forme de Groupe -> Block -> Codeword
	std::vector<std::vector<std::vector<int>>> _messageGroup;
	//Le polynome g�n�rateur pour le code correcteur d'erreur
	//On s'en sert que dans une fonction, on pourrait donc le mettre dans la dite fonction
	//Les coefficients sont sous forme de alpha donc dans GF256
	std::vector<int> _genPoly;
	//Le code correcteur d'erreur sous forme d'une simple liste 
	std::vector<int> _eccPoly;
	//On r�partit les code correcteur par groupe->bloc->Codeword
	std::vector<std::vector<std::vector<int>>> _eccGroup;
	//Le message final avec tout bien interleave
	std::vector<int> _finalMessage;
	//LE message final en binaire avec les bits ajout�s si n�cessaire
	std::vector<int> _finalMessageBin;
	//Le Qr Code final 
	std::vector<std::vector<int>> _qr;
	//Si jamais on a besoin d'un qrCode de ref, m�me si normalement on n'en aura pas besoin si je m'en sors bien
	std::vector<std::vector<int>> _qrRef;
	//Le masque a transf�rer
	int _mask;
	//Pour le format
	std::vector<int> _format;

private:
	

};

