#include "fonction.h"
#include <iostream>

using namespace std;
qrCode qrGen(string const& input)
{
	//On construit le QR Code au fur et à mesure donc ici on peut afficher n'importe quelle étape

	qrCode coco(input);
	coco.byteModeEncoding();
	//coco.printMessage();
	//coco.printMessageBin();
	//coco.printVersion();
	//cout << "HOLY" << endl;
	//coco.printCharCountBin();
	//coco.printTaille();
	coco.fillMessage();
	//cout << "HOLY2" << endl;
	//coco.printTaille();
	//coco.printMessageBin();
	//coco.printGenPolyCount();
	coco.generatorPoly();
	//cout << "HOLY3 \n";
	//coco.printGenPolyCount();
	coco.fillMessagePoly();
	//cout << "HOLY4\n";
	//coco.printMessagePoly();
	//coco.printMessageDiv();
	coco.divideMessagePoly();
	//cout << "HOLY5\n";
	//coco.printMessageDiv();
	coco.reedSolomon();
	//cout << "HOLY6\n";
	//coco.printMessagePoly();
	//coco.printEccPoly();
	//coco.printMessageDiv();
	//coco.printEccDiv();
	//coco.printFinalMessage();
	coco.interleaveData();
	//coco.printFinalMessage();
	coco.interleaveEcc();
	//coco.printFinalMessage();
	//coco.printFinalMessageBin();
	coco.remindAndBitFinal();
	//coco.printFinalMessageBin();
	//cout << "\n FIN DE LA PARTIE ENCODAGE DES DONNEES !! \n";

	//coco.afficherQr();
	coco.qrSize();
	coco.drawPattern();
	coco.drawSeparator();
	coco.drawAlignement();
	coco.drawTiming();
	coco.drawReserved();
	//coco.afficherQr();
	coco.drawData();
	//coco.afficherQr();
	coco.masking();
	coco.afficherQr();
	coco.generateFormat();
	coco.drawFormat();
	coco.drawVersion();
	coco.silentZone();
	//coco.afficherFormat();
	//coco.debug();
	return coco;
}


//String vers hexadecimal
string stringToHex(const string& input)
{
	static const char hex_digits[] = "0123456789ABCDEF";

	string output;
	output.reserve(input.length() * 2);
	for (unsigned char c : input)
	{
		//https://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Companion/cxx_crib/shift.html
			//On décale les bits de c de 4 vers la droite et on ajoute des zéros à gauche
		output.push_back(hex_digits[c >> 4]);
		//Opérateur bit a bit ET
		output.push_back(hex_digits[c & 15]);
	}
	return output;
}



// Hexadecimale vers nombre binaire
string hexToBin(const string& s)
{
	string out;
	for (int i : s) {
		int n;
		if (i <= '9' and i >= '0')
			n = i - '0';
		else
			n = 10 + i - 'A';
		for (int j = 3; j >= 0; --j)
			out.push_back((n & (1 << j)) ? '1' : '0');
	}

	return out;
}

//Pour convertir un entier en binaire
vector<int> intToBin(int aConvertir)
{
	vector<int> nbBin;
	while (aConvertir > 0)
	{
		nbBin.push_back(aConvertir % 2);
		aConvertir /= 2;
	}
	reverse(nbBin.begin(), nbBin.end());
	return nbBin;
}

//Pour convertir un entier en binaire sur exactement 8 bits
//Retourne 0 si c'est pas bon
vector<int> intTo8Bit(int aConvertir)
{
	if (aConvertir > 255) { return {0};
}
	vector<int> nbBin;
	while (aConvertir > 0)
	{
		nbBin.push_back(aConvertir % 2);
		aConvertir /= 2;
	}
	while(nbBin.size() < 8)
	{
		nbBin.push_back(0);
	}
	reverse(nbBin.begin(), nbBin.end());
	return nbBin;
}

vector<int> intToBinFormat(int aConvertir)
{
	if (aConvertir > 7) {
		return { 0 };
	}
	vector<int> nbBin;
	while (aConvertir > 0)
	{
		nbBin.push_back(aConvertir % 2);
		aConvertir /= 2;
	}
	while (nbBin.size() < 3)
	{
		nbBin.push_back(0);
	}
	reverse(nbBin.begin(), nbBin.end());
	return nbBin;
}

int cond1(vector<vector<int>> _qrCope)
{
	int taille = _qrCope.size();
	int i = 0;
	int j = 0;
	int verticalPenalty = 0;
	int horizontalPenalty = 0;
	while (i < taille - 1)
	{
		int k1 = 0;
		int k2 = 0;
		while (j < taille - 1)
		{
			if (_qrCope[i][j] != _qrCope[i][j + 1])
			{
				if (k1 >= 5)
				{
					horizontalPenalty += k1 - 2;
				}
				k1 = 0;
			}
			k1++;
			if (_qrCope[j][i] != _qrCope[j][i + 1])
			{
				if (k2 >= 5)
				{
					verticalPenalty += k2 - 2;
				}
				k2 = 0;
			}
			j++;
		}
		i++;
	}
	return verticalPenalty + horizontalPenalty;
}

int cond2(vector<vector<int>> _qrCope)
{

	int taille = _qrCope.size();
	int result = 0;
	for (int y = 0; y < taille - 1; y++) {
		for (int x = 0; x < taille - 1; x++) {
			int  color = _qrCope[x][y];
			if (color == _qrCope[x + 1][y] &&
				color == _qrCope[x][y + 1] &&
				color == _qrCope[x + 1][y + 1])
			{
				result += 3;
			}
		}
	}
	return result;
}
int cond3(vector<vector<int>> _qrCope)
{

	int result = 0;
	int taille = _qrCope.size();
	for (int i = 0; i < taille - 10; i++) 
	{
		for (int j = 0; j < taille - 10; j++) 
		{
			if (
				(_qrCope[i][j] == 1 &&
				_qrCope[i][j + 1] == 0 &&
				_qrCope[i][j + 2] == 1 &&
				_qrCope[i][j + 3] == 1 &&
				_qrCope[i][j + 4] == 1 &&
				_qrCope[i][j + 5] == 0 &&
				_qrCope[i][j + 6] == 1 &&
				_qrCope[i][j + 7] == 0 &&
				_qrCope[i][j + 8] == 0 &&
				_qrCope[i][j + 9] == 0 &&
				_qrCope[i][j + 10] == 0 ) || (
				_qrCope[i][j] == 0 &&
				_qrCope[i][j + 1] == 0 &&
				_qrCope[i][j + 2] == 0 &&
				_qrCope[i][j + 3] == 0 &&
				_qrCope[i][j + 4] == 1 &&
				_qrCope[i][j + 5] == 0 &&
				_qrCope[i][j + 6] == 1 &&
				_qrCope[i][j + 7] == 1 &&
				_qrCope[i][j + 8] == 1 &&
				_qrCope[i][j + 9] == 0 &&
				_qrCope[i][j + 10] == 1)
				)
			{
				result += 40;
			}
			if (
				(_qrCope[i][j] == 1 &&
					_qrCope[i+1][j] == 0 &&
					_qrCope[i+2][j] == 1 &&
					_qrCope[i+3][j] == 1 &&
					_qrCope[i+4][j] == 1 &&
					_qrCope[i+5][j] == 0 &&
					_qrCope[i+6][j] == 1 &&
					_qrCope[i+7][j] == 0 &&
					_qrCope[i+8][j] == 0 &&
					_qrCope[i+9][j] == 0 &&
					_qrCope[i+10][j] == 0) || (
						_qrCope[i][j] == 0 &&
						_qrCope[i+1][j] == 0 &&
						_qrCope[i+2][j] == 0 &&
						_qrCope[i+3][j] == 0 &&
						_qrCope[i+4][j] == 1 &&
						_qrCope[i+5][j] == 0 &&
						_qrCope[i+6][j] == 1 &&
						_qrCope[i+7][j] == 1 &&
						_qrCope[i+8][j] == 1 &&
						_qrCope[i+9][j] == 0 &&
						_qrCope[i+10][j] == 1)
				)
			{
				result += 40;
			}
		}
	}
	return result;
}
int cond4(vector<vector<int>> _qrCope)
{

	// Balance of black and white modules
	int taille = _qrCope.size();
	int black = 0;
	int result = 0;
	for (vector<int> row : _qrCope)
	{
		for (int color : row)
		{
			if (color == 1) {
				black++;
			}
		}
	}
	int total = taille * taille;
	float percent = black / total * 100;
	int precMult = percent - (int)percent % 5;
	int suivMult = percent + (5 - (int)percent % 5);
	precMult = abs(precMult - 50)/5;
	suivMult = abs(suivMult - 50)/5;
	if (precMult < suivMult) {
		result = precMult * 10;
	}
	else {
		result = suivMult * 10;
	}
	return result;
}

void mask1(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if (i % 2 == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}
void mask2(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if (j % 3 == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}
void mask3(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if ((i + j) % 3 == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}
void mask4(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if ((int)(floor(i / 2) + floor(j / 3)) % 2 == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}
void mask5(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if ((((i * j) % 2) + (i * j) % 3) == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}
void mask6(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if ((((i * j) % 2) + (i * j) % 3) % 2 == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}
void mask7(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if ((((i + j) % 2) + (i * j) % 3) % 2 == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}
void mask0(vector<vector<int>>& _qrCope, vector<vector<int>>& const _qrRef)
{
	int taille = _qrCope.size();
	//Mask Pattern 0
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			if ((i + j) % 2 == 0 && _qrRef[i][j] == -1)
			{
				if (_qrCope[i][j] == 1)
				{
					_qrCope[i][j] = 0;
				}
				else {
					_qrCope[i][j] = 1;
				}
			}
		}
	}
}