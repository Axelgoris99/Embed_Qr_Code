#include "qrCode.h"
#include "fonction.h"
using namespace std;

/*====================== COMMENTAIRES CONSTANTES UTILES =====================*/

//Le nb de bits par version (0,1,2,3,4,5,6,7,8,9, ...) avec le code correcteur d'erreur H
//Le code correcteur a une taille fixée pour une version et un niveau de correction donc vu qu'il y a n blocks de libres dans un QR code, on a n bits de libres pour le message
/* Exemple Version 1 - H
441 bits au total (21*21)
Il y a
mode + terminator = 8 bits
Character count indicator = 8 bits
Separators = 8*8*3 = 192 bits
Timing pattern = 8 bits
dark module = 1bits
version info = 8+8+7+7 = 30 bits
*/

/*
This means that the data codewords must be split up into up to two groups, and
within each group, the data codewords may be further broken up into blocks.
The data codewords are broken up sequentially (i.e. starting with codeword 1, then codeword 2, and so on.) */
vector<int> nbGroupeVersion{ 0,
	1,1,1,1,2,1,2,2,2,
	2,2,2,2,2,2,2,2,2,
	2,2,2,1,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,
	2,2,2,2
};

vector<int> nbBlockGroup1{ 0,
	1,1,2,4,2,4,4,4,4,
	6,3,7,12,11,11,3,2,2,
	9,15,19,34,16,30,22,33,12,
	11,19,23,23,19,11,59,22,2,
	24,42,10,20
};

vector<int> nbBlockGroup2{ 0,
	0,0,0,0,2,0,1,2,4,
	2,8,4,4,5,7,13,17,19,
	16,10,6,0,14,2,13,4,28,
	31,26,25,28,35,46,1,41,64,
	46,32,67,61
};

vector<int> nbDataBlocGroup1{ 0,
	9,16,13,9,11,15,13,14,12,
	15,12,14,11,12,12,15,14,14,
	13,15,16,13,15,16,15,16,15,
	15,15,15,15,15,15,16,15,15,
	15,15,15,15
};
vector<int> nbDataBlocGroup2{ 0,
	0,0,0,0,12,0,14,15,13,
	16,13,15,12,13,13,16,15,15,
	14,16,17,0,16,17,16,17,16,
	16,16,16,16,16,16,17,16,16,
	16,16,16,16
};
vector<vector<int>> nbDataBlockGroup{ { 0,
	9,16,13,9,11,15,13,14,12,
	15,12,14,11,12,12,15,14,14,
	13,15,16,13,15,16,15,16,15,
	15,15,15,15,15,15,16,15,15,
	15,15,15,15},
	{0,
	0,0,0,0,12,0,14,15,13,
	16,13,15,12,13,13,16,15,15,
	14,16,17,0,16,17,16,17,16,
	16,16,16,16,16,16,17,16,16,
	16,16,16,16} };

vector<vector<int>> nbBlockGroup{ { 0,
	1,1,2,4,2,4,4,4,4,
	6,3,7,12,11,11,3,2,2,
	9,15,19,34,16,30,22,33,12,
	11,19,23,23,19,11,59,22,2,
	24,42,10,20},
	{0,
	0,0,0,0,2,0,1,2,4,
	2,8,4,4,5,7,13,17,19,
	16,10,6,0,14,2,13,4,28,
	31,26,25,28,35,46,1,41,64,
	46,32,67,61} };

vector<int> nbCodeWord{ 0,
	17,28,22,16,22,28,26,26,24,
	28,24,28,22,24,24,30,28,28,
	26,28,30,24,30,30,30,30,30,
	30,30,30,30,30,30,30,30,30,
	30,30,30,30
};

vector<vector<int>> alignementLoc{
	{},
	{},
	{6,	18},
	{6,	22},
	{6,	26},
	{6,	30},
	{6,	34},
	{6,	22,	38},
	{6,	24, 42},
	{6,	26,	46},
	{6,	28,	50},
	{6,	30,	54},
	{6,	32,	58},
	{6,	34,	62},
	{6,	26,	46,	66},
	{6,	26,	48,	70},
	{6,	26,	50,	74},
	{6,	30,	54,	78},
	{6,	30,	56,	82},
	{6,	30,	58,	86},
	{6,	34,	62,	90},
	{6,	28,	50,	72,	94},
	{6,	26,	50,	74,	98},
	{6,	30,	54,	78,	102},
	{6,	28,	54,	80,	106},
	{6,	32,	58,	84,	110},
	{6,	30,	58,	86,	114},
	{6,	34,	62,	90,	118},
	{6,	26,	50, 74,	98,	122},
	{6,	30,	54,	78,	102, 126},
	{6,	26,	52,	78,	104, 130},
	{6,	30,	56,	82,	108, 134},
	{6,	34,	60,	86,	112, 138},
	{6,	30,	58,	86,	114, 142},
	{6,	34,	62,	90,	118, 146},
	{6,	30,	54,	78,	102, 126, 150},
	{6,	24,	50,	76,	102, 128, 154},
	{6,	28,	54,	80,	106, 132, 158},
	{6,	32,	58,	84,	110, 136, 162},
	{6,	26,	54,	82,	110, 138, 166},
	{6,	30,	58,	86,	114, 142, 170},
};

vector<int> reminderBit{ 0,
0,7,7,7,7,7,0,0,0,0,0,0,0,3,3,3,3,3,3,3,4,4,4,4,4,4,4,3,3,3,3,3,3,3,0,0,0,0,0,0 };

//Permet de trouver les exposant alpha, le truc bizarre pour passer dans le champ de Galois 256
vector<int> logGF256{ 1,2,4,8,16,32,64,128,29,58,116,232,205,135,19,38,76,152,45,90,180,117,234,201,143,3,6,12,24,48,96,192,157,39,78,156,37,74,148,53,106,212,181,119,238,193,159,35,70,140,5,10,20,40,80,160,93,186,105,210,185,111,222,161,95,190,97,194,153,47,94,188,101,202,137,15,30,60,120,240,253,231,211,187,107,214,177,127,254,225,223,163,91,182,113,226,217,175,67,134,17,34,68,136,13,26,52,104,208,189,103,206,129,31,62,124,248,237,199,147,59,118,236,197,151,51,102,204,133,23,46,92,184,109,218,169,79,158,33,66,132,21,42,84,168,77,154,41,82,164,85,170,73,146,57,114,228,213,183,115,230,209,191,99,198,145,63,126,252,229,215,179,123,246,241,255,227,219,171,75,150,49,98,196,149,55,110,220,165,87,174,65,130,25,50,100,200,141,7,14,28,56,112,224,221,167,83,166,81,162,89,178,121,242,249,239,195,155,43,86,172,69,138,9,18,36,72,144,61,122,244,245,247,243,251,235,203,139,11,22,44,88,176,125,250,233,207,131,27,54,108,216,173,71,142,1 };
//Permet de revenir du champ de Galois 256 !
vector<int> antiLogGF256{ 0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75, 4, 100, 224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113, 5, 138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69, 29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 166, 6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145, 34, 136, 54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92, 131, 56, 70, 64, 30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40, 84, 250, 133, 186, 61, 202, 94, 155, 159, 10, 21, 121, 43, 78, 212, 229, 172, 115, 243, 167, 87, 7, 112, 192, 247, 140, 128, 99, 13, 103, 74, 222, 237, 49, 197, 254, 24, 227, 165, 153, 119, 38, 184, 180, 124, 17, 68, 146, 217, 35, 32, 137, 46, 55, 63, 209, 91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190, 97, 242, 86, 211, 171, 20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162, 31, 45, 67, 216, 183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246, 108, 161, 59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90, 203, 89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215, 79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 88, 175, 255 };

/*====================== CONSTRUCTEUR =====================*/
qrCode::qrCode(string const& input)
{
	_message = input;
	_mode = { 0,1,0,0 };
	chooseVersion();
	_taille = 21 + (_version - 1) * 4;
}

/*====================== ENCODAGE DES INFOS GENERALES =====================*/

void qrCode::chooseVersion()
{
	vector<int> nbCharVerEcc{ 0, 7, 14, 24, 34, 44, 58, 64, 84, 98,119,
						137, 155, 177, 194, 220, 250, 280, 310, 338, 382,
						403, 439, 461, 511, 535, 593, 625, 658, 698, 742,
						790, 842, 898, 958, 983, 1051, 1093, 1139, 1219, 1273 };
	//On obtient la version grâce à l'ensemble des nombres de caractères possible
	int taille;
	taille = _message.size();
	int i = 0;
	while (taille > nbCharVerEcc[i]) { i++; }
	_version = i;
	//On s'occupe de l'indicateur du nombre de caractères
	//Attention, ceci dépend du mode donc à modifier si on upgrade notre générateur
	if (_version < 10) { _charCountIndicatorSize = 8; }
	else { _charCountIndicatorSize = 16; }
	_charCountIndicator = intToBin(taille);
	//Si jamais il n'y a pas assez de caractères pour compléter le caractère count indicator,
	//On remplit avec des 0 à gauche

	while (_charCountIndicator.size() != _charCountIndicatorSize)
	{
		_charCountIndicator.insert(_charCountIndicator.begin(), 0);
	}

}

void qrCode::byteModeEncoding()
{
	//Pas opti mais pas bien grave, au moins je suis sûr du résultat
	string hex;
	hex = stringToHex(_message);
	string bin;
	bin = hexToBin(hex);	int taille = bin.size();
	for (int i = 0; i < taille; i++)
	{
		_messageBin.push_back((int)bin[i] % 48);
	}
}
/*================================== ENCODAGE MESSAGE ==========================================*/
void qrCode::fillMessage()
{
	vector<int> nbBitsVerEcc{ 0, 72, 128, 208, 288, 368, 480, 528, 688, 800, 976, 1120,
						1264, 1440, 1576, 1784, 2024, 2264, 2504, 2728, 3080, 3248,
						3536, 3712, 4112, 4304, 4768, 5024, 5288, 5608, 5960, 6344,
						6760, 7208, 7688, 7888, 8432, 8768, 9136, 9776, 10208 };

	vector<int> padBytes0{ 1,1,1,0,1,1,0,0 };
	vector<int> padBytes1{ 0,0,0,1,0,0,0,1 };
	int i = 0;
	_messageBin.insert(_messageBin.begin(), _charCountIndicator.begin(), _charCountIndicator.end());
	_messageBin.insert(_messageBin.begin(), _mode.begin(), _mode.end());
	//On rajoute max 4 bits à 0 qui sont le terminator
	while (i < 4 && _messageBin.size() != nbBitsVerEcc[_version])
	{
		_messageBin.push_back(0);
		i++;
	}
	//On fait de notre message un multiple de 8
	if (_messageBin.size() % 8 != 0)
	{
		int j = (8 - (_messageBin.size() % 8));
		for (int k = 0; k < j; k++)
		{
			_messageBin.push_back(0);
		}
	}
	//Si on est toujours pas à la bonne taille pour le QR Code, on utilise les pad bytes !
	int nbPadBytes = (nbBitsVerEcc[_version] - _messageBin.size()) / 8;
	//On alterne les padBytes
	for (int k = 0; k < nbPadBytes; k++)
	{
		if (k % 2 == 0)
		{
			_messageBin.insert(_messageBin.end(), padBytes0.begin(), padBytes0.end());
		}
		else
		{
			_messageBin.insert(_messageBin.end(), padBytes1.begin(), padBytes1.end());
		}
	}
	_taille = _messageBin.size();
}


/*================================== ECC ==========================================*/
void qrCode::generatorPoly()
{
	//Notre polynome générateur a la taille du nombre de code correcteur qu'il faut par bloc
	/*
	* On travaille dans le champ de Galois 256
	The very first polynomial that you start with in the first step is always (a0x1 + a0x0).
	For each multiplication step, you multiply the current polynomial by (a0x1 + ajx0)
	where j is 1 for the first multiplication, 2 for the second multiplication, 3 for the third, and so on.
	If there are any exponents that are larger than 255, fix them by putting them through the formula
	(exponent % 256) + floor(exponent / 256)
	In this case, all the exponents are less than or equal to 255, so we can continue.
	When we multiply a251 * a10, the result is a6 for example.
	Because of the nature of the galois field, we can't simply add the numbers together. We have to XOR them.

	This will cause some unexpected results. For example:
	a201x2 + a199x2 = 56x2 + 14x2

	If we add those together normally, we get 70x2.
	This number is not larger than 255, so it would seem acceptable.
	However, if we perform an XOR instead of addition, we get:
	56 ^ 14 = 54.
	This is the correct result.
	*/
	//J'aime pas les polynomes.

	int taille = nbCodeWord[_version];
	//Un polynome de degree 17 a 18 TERMES !
	taille++;
	//Ca c'est pour que le polynome que l'on complète soit de la bonne taille à la fin
	_genPoly.resize(taille);
	for (int n = 0; n < (taille - 1); n++) { _genPoly[n] = -1; }

	vector<int> poly{ 0,0 };
	int i, j, k;
	int degre = 2;
	//On va rajouter un terme à chaque fois
	for (i = 1; i < taille - 1; i++)
	{	//On initialise tous les termes à -1 pour les compléter vu qu'on a fait une copie du précédent polynome
		for (int n = 0; n < (taille - 1); n++) { _genPoly[n] = -1; }
		vector<int> polyMul{ i, 0 };
		//On va calculer chaque terme du polynome, en faisant le produit de chaque terme
		for (j = 0; j < degre; j++)
		{
			for (k = 0; k < 2; k++)
			{
				//Si il y a déjà un terme dans la case 
				if (_genPoly[j + k] != -1)
				{
					//On passe dans le domaine normal pour faire les XOR ensuite
					int resultat = logGF256[_genPoly[j + k]] ^ logGF256[(poly[j] + polyMul[k]) % 256 + floor((poly[j] + polyMul[k]) / 256)];
					//SI c'est pas 0, on recalcule notre coefficient alpha
					if (resultat != 0) {
						_genPoly[j + k] = antiLogGF256[resultat - 1];
					}
					//Sinon on remet à 0
					else { _genPoly[j + k] = 0; }
				}
				//S'il n'y a pas encore de terme dans la case, pas besoin de XOR donc pas besoin de sortir de GF256
				else {
					_genPoly[j + k] = (poly[j] + polyMul[k]) % 256 + floor((poly[j] + polyMul[k]) / 256);
				}
			}
		}
		degre++;
		poly = _genPoly;

	}
	reverse(_genPoly.begin(), _genPoly.end());
}

void qrCode::fillMessagePoly()
{
	int taille = _messageBin.size();
	int somme(0);
	for (int i = 0; i < taille; i++)
	{
		somme += _messageBin[i] * pow(2, 7 - (i % 8));
		if ((i + 1) % 8 == 0)
		{
			_messagePoly.push_back(somme);
			somme = 0;
		}
	}
}

void qrCode::divideMessagePoly()
{
	reverse(_messagePoly.begin(), _messagePoly.end());
	int nbGroup = nbGroupeVersion[_version];
	//On crée le bon nombre de groupe pour le message et pour les codes correcteurs d'erreurs
	_messageGroup.resize(nbGroup);
	_eccGroup.resize(nbGroup);
	int n = 0;
	for (int k = 0; k < nbGroup; k++)
	{
		int nbBloc = nbBlockGroup[k][_version];
		//On crée le bon nombre de bloc dépendant du groupe
		_messageGroup[k].resize(nbBloc);
		//_eccGroup[k].resize(nbBloc);
		for (int i = 0; i < nbBloc; i++)
		{
			int nbData = nbDataBlockGroup[k][_version];

			for (int j = 0; j < nbData; j++)
			{
				//On ajoute autant de data codeword que le nécessite le block du group k
				_messageGroup[k][i].push_back(_messagePoly[_messagePoly.size() - 1]);
				_messagePoly.pop_back();
			}
			n++;
		}
	}
}

void qrCode::reedSolomon()
{
	int nbGroup = nbGroupeVersion[_version];
	for (int k = 0; k < nbGroup; k++)
	{
		int nbBloc = nbBlockGroup[k][_version];
		for (int i = 0; i < nbBloc; i++)
		{
			_eccPoly = _messageGroup[k][i];
			int taille = nbCodeWord[_version];
			for (int j = 0; j < taille; j++)
			{
				_eccPoly.push_back(-1);
			}
			taille = _messageGroup[k][i].size();
			for (int j = 0; j < taille - 1; j++)
			{
				_genPoly.push_back(-1);
			}

			for (int j = 0; j < taille; j++)
			{
				int nbCoef = _genPoly.size();
				//Le polynome générateur ne change jamais donc on va en faire une copie pour 
				//travailler dessus dans chaque boucle
				vector<int> genPoly(_genPoly.begin(), _genPoly.end());
				for (int p = 0; p < nbCoef; p++) {
					if (_eccPoly[0] != 0)
					{
						int coefAjout = antiLogGF256[_eccPoly[0] - 1];
						if (genPoly[p] != -1)
						{
							genPoly[p] = ((_genPoly[p] + coefAjout) % 255);
						}
					}
				}

				for (int d = 0; d < nbCoef; d++)
				{

					//Opération XOR
					//Si le polynome générateur est différent de -1 et celui du message aussi, on XOR, sinon on le laisse identique
					if (genPoly[d] != -1 && _eccPoly[d] != -1)
					{
						_eccPoly[d] = (_eccPoly[d] ^ logGF256[genPoly[d]]);
					}
					//Si le polynome générateur est différent de -1 et celui du message vaut 1,
					//on XOR avec rien ce qui revient à attribuer la valeur

					if (genPoly[d] != -1 && _eccPoly[d] == -1)
					{
						_eccPoly[d] = logGF256[genPoly[d]];
					}
				}
				_eccPoly.erase(_eccPoly.begin());
				if (_genPoly[_genPoly.size() - 1] == -1) { _genPoly.pop_back(); }
			}
			while (_genPoly[_genPoly.size() - 1] == -1) { _genPoly.pop_back(); }
			_eccGroup[k].push_back(_eccPoly);
		}

	}
}

/*============================= STRUCTURE FINAL MESSAGE ================================= */

//Ici, on va "interleave" les données si c'est nécessaire
//En gros : DataCodeword 1 du bloc 1 puis 1 du bloc 2, 1 du 3, 1 du 4, 2 du 1, 2 du 2 ...
void qrCode::interleaveData()
{
	int dataBlock = nbDataBlockGroup[0][_version];
	for (int i = 0; i < dataBlock; i++)
	{
		int nbBlock1 = nbBlockGroup1[_version];
		for (int j = 0; j < nbBlock1; j++)
		{
			_finalMessage.push_back(_messageGroup[0][j][i]);
		}

		int nbBlock2 = nbBlockGroup2[_version];
		for (int j = 0; j < nbBlock2; j++)
		{
			_finalMessage.push_back(_messageGroup[1][j][i]);
		}
	}
	//S'il y a une différence de nombres de données, elle sera toujours de +1 seulement
	//CF table ECC
	if (nbDataBlocGroup2[_version] > nbDataBlocGroup1[_version])
	{
		int nbBlock2 = nbBlockGroup2[_version];
		int i = nbDataBlocGroup2[_version] - 1;
		for (int j = 0; j < nbBlock2; j++)
		{
			_finalMessage.push_back(_messageGroup[1][j][i]);
		}
	}
}

void qrCode::interleaveEcc()
{
	int dataBlock = nbCodeWord[_version];
	//Ici quelque soit le block, on a le même nombre de données
	for (int i = 0; i < dataBlock; i++)
	{
		int nbBlock1 = nbBlockGroup1[_version];
		for (int j = 0; j < nbBlock1; j++)
		{
			_finalMessage.push_back(_eccGroup[0][j][i]);
		}

		int nbBlock2 = nbBlockGroup2[_version];
		for (int j = 0; j < nbBlock2; j++)
		{
			_finalMessage.push_back(_eccGroup[1][j][i]);
		}
	}
}


void qrCode::remindAndBitFinal()
{
	int taille = _finalMessage.size();
	for (int i = 0; i < taille; i++)
	{
		vector<int> nbBin = intTo8Bit(_finalMessage[i]);
		for (int j : nbBin)
		{
			_finalMessageBin.push_back(j);
		}
	}
	int nbReminder = reminderBit[_version];
	for (int i = 0; i < nbReminder; i++)
	{
		_finalMessageBin.push_back(0);
	}
}

/*====================== CREATION DU QR CODE AFFICHE ==============*/
// Les ## sont les carrés théoriquement noirs !
// "  " sont les carrés théoriquement blancs!
//Pour le moment, on met 0 ou 1 en fait
//White = 0 = "  "
//Black = 1 = "##"

void qrCode::qrSize()
{
	int taille(((_version - 1) * 4) + 21);
	_qr.resize(taille);
	for (int i = 0; i < taille; i++)
	{
		_qr[i].resize(taille);
	}
	//ref
	_qrRef.resize(taille);
	for (int j = 0; j < taille; j++)
	{
		_qrRef[j].resize(taille);
	}
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			_qr[i][j] = -1;
			_qrRef[i][j] = -1;
		}
	}
}

void qrCode::drawPattern()
{
	//Le carré en haut à gauche
	//On le colorie intégralement en noir
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			_qr[i][j] = 1;
			//ref
			_qrRef[i][j] = 1;
		}
	}

	//On rajoute le blanc où il faut
	for (int i = 0; i < 5; i++)
	{
		_qr[i + 1][1] = 0;
		_qr[1][i + 1] = 0;
		_qr[5][i + 1] = 0;
		_qr[i + 1][5] = 0;

		//ref
		_qrRef[i + 1][1] = 0;
		_qrRef[1][i + 1] = 0;
		_qrRef[5][i + 1] = 0;
		_qrRef[i + 1][5] = 0;
	}

	int taille = (((_version - 1) * 4) + 21);
	//Le carré en bas à gauche et en haut à droite
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			_qr[taille - 7 + i][j] = 1;
			_qr[i][taille - 7 + j] = 1;

			//ref
			_qrRef[taille - 7 + i][j] = 1;
			_qrRef[i][taille - 7 + j] = 1;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		_qr[taille - 6 + i][1] = 0;
		_qr[taille - 6 + i][5] = 0;
		_qr[1][taille - 6 + i] = 0;
		_qr[5][taille - 6 + i] = 0;
		_qr[taille - 6][i + 1] = 0;
		_qr[taille - 2][i + 1] = 0;
		_qr[i + 1][taille - 2] = 0;
		_qr[i + 1][taille - 6] = 0;

		//ref
		_qrRef[taille - 6 + i][1] = 0;
		_qrRef[taille - 6 + i][5] = 0;
		_qrRef[1][taille - 6 + i] = 0;
		_qrRef[5][taille - 6 + i] = 0;
		_qrRef[taille - 6][i + 1] = 0;
		_qrRef[taille - 2][i + 1] = 0;
		_qrRef[i + 1][taille - 2] = 0;
		_qrRef[i + 1][taille - 6] = 0;
	}
}

void qrCode::drawSeparator()
{
	int taille(((_version - 1) * 4) + 21);
	for (int i = 0; i < 8; i++)
	{
		//Haut Gauche
		_qr[7][i] = 0;
		_qr[i][7] = 0;
		//Bas gauche et haut droite
		_qr[taille - 7 - 1][i] = 0;
		_qr[i][taille - 7 - 1] = 0;

		_qr[taille - 7 - 1 + i][7] = 0;
		_qr[7][taille - 7 - 1 + i] = 0;

		//Ref
		_qrRef[7][i] = 0;
		_qrRef[i][7] = 0;
		_qrRef[taille - 7 - 1][i] = 0;
		_qrRef[i][taille - 7 - 1] = 0;
		_qrRef[taille - 7 - 1 + i][7] = 0;
		_qrRef[7][taille - 7 - 1 + i] = 0;
	}
}

//Les centres de chaque pattern d'alignement sont dans la liste alignementLoc
void qrCode::drawAlignement()
{
	//La boucle qui permet de tester le centre de chaque pattern d'alignement
	vector<int> mid{ alignementLoc[_version] };
	int taille = mid.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			//On teste si le pattern d'alignement n'overlap aucun des finder ou separator pattern
			bool affiche(true);
			if (_qr[mid[i] - 2][mid[j] - 2] != -1 ||
				_qr[mid[i] - 2][mid[j] + 2] != -1 ||
				_qr[mid[i] + 2][mid[j] - 2] != -1 ||
				_qr[mid[i] + 2][mid[j] + 2] != -1)
			{
				affiche = false;
			}
			if (affiche)
			{
				//On fait un carré tout noir
				for (int x = 0; x < 5; x++)
				{
					for (int y = 0; y < 5; y++)
					{
						_qr[mid[i] - 2 + x][mid[j] - 2 + y] = 1;
						_qrRef[mid[i] - 2 + x][mid[j] - 2 + y] = 1;
					}
				}
				//On fait un carré tout blanc au centre
				for (int x = 0; x < 3; x++)
				{
					for (int y = 0; y < 3; y++)
					{
						_qr[mid[i] - 1 + x][mid[j] - 1 + y] = 0;
						_qrRef[mid[i] - 1 + x][mid[j] - 1 + y] = 0;
					}
				}
				_qr[mid[i]][mid[j]] = 1;
				_qrRef[mid[i]][mid[j]] = 1;
				//On rajoute le noir au milieu
			}
		}
	}
}

void qrCode::drawTiming()
{
	//Pour la suite, il faut que le timing n'overlap pas les alignements dans les références
	//Du coup, on va devoir modifier un peu notre code
	int taille(((_version - 1) * 4) + 21);
	bool dark(true);
	for (int i = 2; i < taille - 14; i++)
	{
		if (dark)
		{
			_qr[6][6 + i] = 1;
			_qr[6 + i][6] = 1;
			if (_qrRef[6][6 + i] == -1)
			{
				_qrRef[6][6 + i] = 10;
				_qrRef[6 + i][6] = 10;
			}
			dark = !dark;
		}
		else
		{
			_qr[6][6 + i] = 0;
			_qr[6 + i][6] = 0;
			if (_qrRef[6][6 + i] == -1)
			{
				_qrRef[6][6 + i] = 10;
				_qrRef[6 + i][6] = 10;
			}
			dark = !dark;
		}
	}

}

void qrCode::drawReserved()
{
	int taille = (((_version - 1) * 4) + 21);
	_qr[(4 * _version) + 9][8] = 10;
	for (int i = 0; i < 9; i++)
	{
		//On pourrait faire un seul test vu que c'est symétrique
		if (_qr[8][i] == -1) {
			_qr[8][i] = 10;
			//ref
			_qrRef[8][i] = 10;
		}
		if (_qr[i][8] == -1) {
			_qr[i][8] = 10;
			//ref
			_qrRef[i][8] = 10;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		_qr[(4 * _version) + 9 + i][8] = 10;
		_qr[8][(4 * _version) + 9 + i] = 10;
		//Ref
		_qrRef[(4 * _version) + 9 + i][8] = 10;
		_qrRef[8][(4 * _version) + 9 + i] = 10;
	}

	//Il y a un bloc de 3*6 réservé en haut a droite et en bas à gauche 
	//Pour les versions 7 et supérieures
	if (_version >= 7)
	{
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				_qr[(4 * _version) + 6 + j][i] = 10;
				_qr[i][(4 * _version) + 6 + j] = 10;
				//Ref
				_qrRef[(4 * _version) + 6 + j][i] = 10;
				_qrRef[i][(4 * _version) + 6 + j] = 10;
			}
		}
	}
}

void qrCode::drawData()
{
	int taille(((_version - 1) * 4) + 21);
	int i = taille - 1;
	int j = taille - 1;
	int k = 0;
	bool montant(true);
	while (j > 0)
	{
		//La partie où l'on monte
		if (j == 6) { j--; }
		while (i >= 0 && montant)
		{
			if (_qr[i][j] == -1)
			{
				_qr[i][j] = _finalMessageBin[k];
				k++;
			}
			if (_qr[i][j - 1] == -1)
			{
				_qr[i][j - 1] = _finalMessageBin[k];
				k++;
			}
			i--;
		}
		i = 0;
		montant = !montant;
		j -= 2;
		if (j == 6) {j--;}
		//La partie où l'on descend
		while (i < taille && !montant)
		{
			if (_qr[i][j] == -1)
			{
				_qr[i][j] = _finalMessageBin[k];
				k++;
			}
			if (_qr[i][j - 1] == -1)
			{
				_qr[i][j - 1] = _finalMessageBin[k];
				k++;
			}
			i++;
		}
		i = taille - 1;
		montant = !montant;
		j -= 2;

	}

	while (k != _finalMessageBin.size())
	{
		if (montant && i >= 0) {
			if (_qr[i][0] == -1)
			{
				_qr[i][0] = _finalMessageBin[k];
				k++;
			}
			i--;
		}

		if (!montant && i < taille) {
			if (_qr[i][0] == -1)
			{
				_qr[i][0] = _finalMessageBin[k];
				k++;
			}
			i++;
		}

	}
}

//Bientot finis !!!

void qrCode::masking()
{
	vector<int> penalite;
	penalite.resize(8);
	//Pour déterminer les masks, on va devoir copier les différents masks
	// il  y a 7 masks différents à tester
	// il y a 4 conditions de pénalité à calculer
	vector<vector<int>> _qrCope(_qr.begin(), _qr.end());
	mask0(_qrCope, _qrRef);
	//Conditions
	int pena = cond1(_qrCope);
	penalite[0] = pena;
	pena = cond2(_qrCope);
	penalite[0] += pena;
	pena = cond3(_qrCope);
	penalite[0] += pena;
	pena = cond4(_qrCope);
	penalite[0] += pena;

	//Mask Pattern 1
	_qrCope = _qr;
	mask1(_qrCope, _qrRef);
	//Conditions
	pena = cond1(_qrCope);
	penalite[1] = pena;
	pena = cond2(_qrCope);
	penalite[1] += pena;
	pena = cond3(_qrCope);
	penalite[1] += pena;
	pena = cond4(_qrCope);
	penalite[1] += pena;
	//Mask Pattern 2
	_qrCope = _qr;
	mask2(_qrCope, _qrRef);

	//Conditions
	pena = cond1(_qrCope);
	penalite[2] = pena;
	pena = cond2(_qrCope);
	penalite[2] += pena;
	pena = cond3(_qrCope);
	penalite[2] += pena;
	pena = cond4(_qrCope);
	penalite[2] += pena;
	//Mask Pattern 3
	_qrCope = _qr;
	mask3(_qrCope, _qrRef);

	//Conditions
	pena = cond1(_qrCope);
	penalite[3] = pena;
	pena = cond2(_qrCope);
	penalite[3] += pena;
	pena = cond3(_qrCope);
	penalite[3] += pena;
	pena = cond4(_qrCope);
	penalite[3] += pena;
	//Mask Pattern 4
	_qrCope = _qr;
	mask4(_qrCope, _qrRef);

	//Conditions
	pena = cond1(_qrCope);
	penalite[4] = pena;
	pena = cond2(_qrCope);
	penalite[4] += pena;
	pena = cond3(_qrCope);
	penalite[4] += pena;
	pena = cond4(_qrCope);
	penalite[4] += pena;
	//Mask Pattern 5
	_qrCope = _qr;
	mask5(_qrCope, _qrRef);

	//Conditions
	pena = cond1(_qrCope);
	penalite[5] = pena;
	pena = cond2(_qrCope);
	penalite[5] += pena;
	pena = cond3(_qrCope);
	penalite[5] += pena;
	pena = cond4(_qrCope);
	penalite[5] += pena;
	//Mask Pattern 6
	_qrCope = _qr;
	mask6(_qrCope, _qrRef);

	//Conditions
	pena = cond1(_qrCope);
	penalite[6] = pena;
	pena = cond2(_qrCope);
	penalite[6] += pena;
	pena = cond3(_qrCope);
	penalite[6] += pena;
	pena = cond4(_qrCope);
	penalite[6] += pena;
	//Mask Pattern 7
	_qrCope = _qr;
	mask7(_qrCope, _qrRef);

	//Conditions
	pena = cond1(_qrCope);
	penalite[7] = pena;
	pena = cond2(_qrCope);
	penalite[7] += pena;
	pena = cond3(_qrCope);
	penalite[7] += pena;
	pena = cond4(_qrCope);
	penalite[7] += pena;

	_mask = 0;
	int nb = penalite.size();
	int min = penalite[0];
	for (int i = 0; i < nb; i++)
	{
		if (penalite[i] < min)
		{
			_mask = i;
			min = penalite[i];
		}
	}

	//On applique le bon mask à notre qr Code
	switch (_mask)
	{
	case 0:
		mask0(_qr, _qrRef);
		break;
	case 1:
		mask1(_qr, _qrRef);
		break;
	case 2:
		mask2(_qr, _qrRef);
		break;
	case 3:
		mask3(_qr, _qrRef);
		break;
	case 4:
		mask4(_qr, _qrRef);
		break;
	case 5:
		mask5(_qr, _qrRef);
		break;
	case 6:
		mask6(_qr, _qrRef);
		break;
	case 7:
		mask7(_qr, _qrRef);
		break;
	}
}

void qrCode::generateFormat()
{
	/*
	//C'est bien beau mais autant hardcoder les valeurs directmeent, ça prendra probablement moins de temps
	//On ne génère qu'en niveau H
	_format.push_back(1);
	_format.push_back(0);
	vector<int> mask;
	mask = intToBinFormat(_mask);
	_format.insert(_format.end(), mask.begin(), mask.end());
	//On a les 5 bits pour le format
	//On va générer un dernier code correcteur d'erreur
	vector<int> format(_format.begin(), _format.end());
	vector<int> genPolyRef{ 1,0,1,0,0,1,1,0,1,1,1 };
	for (int i = 0; i < 10; i++)
	{
		format.push_back(0);
	}
	while (format.size() > 11)
	{
		vector<int> genPoly(genPolyRef.begin(), genPolyRef.end());
		while (format[0] == 0)
		{
			format.erase(format.begin());
		}
		int taille = format.size();
		while (genPoly.size() < taille)
		{
			genPoly.push_back(0);
		}
		for(int i = 0 ; i<taille; i++)
		{
			if (format[i] == genPoly[i]) { format[i] = 0; }
			else { format[i] = 1; }
		}
	}
	if (format.size() > 10)
	{
		int taille = format.size();
		for (int i = 0; i < taille; i++)
		{
			if (format[i] == genPolyRef[i]) { format[i] = 0; }
			else { format[i] = 1; }
		}
		format.erase(format.begin());
	}

	while (format.size() < 10) { format.push_back(0); }
	_format.insert(_format.end(), format.begin(), format.end());
	vector<int> xorWith{ 1,0,1,0,1,0,0,0,0,0,1,0,0,1,0 };
	int taille = _format.size();
	for(int i = 0; i < taille; i++)
	{
		if (_format[i] == xorWith[i]) { _format[i] = 0; }
		else { _format[i] = 1; }
	}
	*/
	//Voilà, ça marche bien aussi comme ça
	vector<vector<int>> formatMask{
		{0,0,1,0,1,1,0,1,0,0,0,1,0,0,1},
		{0,0,1,0,0,1,1,1,0,1,1,1,1,1,0},
		{0,0,1,1,1,0,0,1,1,1,0,0,1,1,1},
		{0,0,1,1,0,0,1,1,1,0,1,0,0,0,0},
		{0,0,0,0,1,1,1,0,1,1,0,0,0,1,0},
		{0,0,0,0,0,1,0,0,1,0,1,0,1,0,1},
		{0,0,0,1,1,0,1,0,0,0,0,1,1,0,0},
		{0,0,0,1,0,0,0,0,0,1,1,1,0,1,1}
	};
	_format = formatMask[_mask];
}

void qrCode::drawFormat()
{
	for (int i = 0; i < 6; i++)
	{
		_qr[8][i] = _format[i];
		_qr[i][8] = _format[_format.size() - 1 - i];
	}
	_qr[7][8] = _format[8];
	_qr[8][7] = _format[6];
	_qr[8][8] = _format[7];
	for (int i = 0; i < 7; i++)
	{
		_qr[_qr.size() - 1 - i][8] = _format[i];
		_qr[8][_qr.size() - 8 + i] = _format[7 + i];
	}
	_qr[8][_qr.size() - 1] = _format[_format.size() - 1];

	_qr[(4 * _version) + 9][8] = 1;
}

void qrCode::drawVersion()
{
	//Comme précédemment on pourrait les générer avec des code correcteurs d'erreur mais
	//Franchement pas très envie de remettre les mains dans le cambouis
	vector<vector<int>> versionInfo{
		{0},//0
		{0},//1
		{0},//2
		{0},//3
		{0},//4
		{0},//5
		{0},//6
		{0,0,0,1,1,1,1,1,0,0,1,0,0,1,0,1,0,0},//7
		{0,0,1,0,0,0,0,1,0,1,1,0,1,1,1,1,0,0},//8
		{0,0,1,0,0,1,1,0,1,0,1,0,0,1,1,0,0,1},//9
		{0,0,1,0,1,0,0,1,0,0,1,1,0,1,0,0,1,1},//10
		{0,0,1,0,1,1,1,0,1,1,1,1,1,1,0,1,1,0},//11
		{0,0,1,1,0,0,0,1,1,10,1,1,0,0,0,1,0},//12
		{0,0,1,1,0,1,1,0,0,0,0,1,0,0,0,1,1,1},//13
		{0,0,1,1,1,0,0,1,1,0,0,0,0,0,1,1,0,1},//14
		{0,0,1,1,1,1,1,0,0,1,0,0,1,0,1,0,0,0},//15
		{0,1,0,0,0,0,1,0,1,1,0,1,1,1,1,0,0,0},//16
		{0,1,0,0,0,1,0,1,0,0,0,1,0,1,1,1,0,1},//17
		{0,1,0,0,1,0,1,0,1,0,0,0,0,1,0,1,1,1},//18
		{0,1,0,0,1,1,0,1,0,1,0,0,1,1,0,0,1,0},//19
		{0,1,0,1,0,0,1,0,0,1,1,0,1,0,0,1,1,0},
{0,1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,1,1},
{0,1,0,1,1,0,1,0,0,0,1,1,0,0,1,0,0,1},
{0,1,0,1,1,1,0,1,1,1,1,1,1,0,1,1,0,0},
{0,1,1,0,0,0,1,1,1,0,1,1,0,0,0,1,0,0},
{0,1,1,0,0,1,0,0,0,1,1,1,1,0,0,0,0,1},
{0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1},
{0,1,1,0,1,1,0,0,0,0,1,0,0,0,1,1,1,0},
{0,1,1,1,0,0,1,1,0,0,0,0,0,1,1,0,1,0},
{0,1,1,1,0,1,0,0,1,1,0,0,1,1,1,1,1,1},
{0,1,1,1,1,0,1,1,0,1,0,1,1,1,0,1,0,1},
{0,1,1,1,1,1,0,0,1,0,0,1,0,1,0,0,0,0},
{1,0,0,0,0,0,1,0,0,1,1,1,0,1,0,1,0,1},
{1,0,0,0,0,1,0,1,1,0,1,1,1,1,0,0,0,0},
{1,0,0,0,1,0,1,0,0,0,1,0,1,1,1,0,1,0},
{1,0,0,0,1,1,0,1,1,1,1,0,0,1,1,1,1,1},
{1,0,0,1,0,0,1,0,1,1,0,0,0,0,1,0,1,1},
{1,0,0,1,0,1,0,1,0,0,0,0,1,0,1,1,1,0},
{1,0,0,1,1,0,1,0,1,0,0,1,1,0,0,1,0,0},
{1,0,0,1,1,1,0,1,0,1,0,1,0,0,0,0,0,1},
{1,0,1,0,0,0,1,1,0,0,0,1,1,0,1,0,0,1},

	};

	vector<int> info = versionInfo[_version];
	int k = 0;
	if (_version > 6)
	{
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				// - 11
				_qr[_qr.size() - 11 + j][i] = info[info.size()-1-k];
				_qr[i][_qr.size() - 11 + j] = info[info.size()-1-k];
				k++;
			}
		}
	}
}

void qrCode::silentZone()
{
	int taille = _qr.size();
	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			_qr[i].insert(_qr[i].begin(), 0);
			_qr[i].push_back(0);
			_qrRef[i].insert(_qrRef[i].begin(), 10);
			_qrRef[i].push_back(10);
		}
	}

	taille = _qr[0].size();
	vector<int> zero(taille, 0);
	vector<int> dix(taille, 10);
	for (int j = 0; j < 4; j++)
	{
		_qr.insert(_qr.begin(), zero);
		_qr.push_back(zero);
		_qrRef.insert(_qrRef.begin(), dix);
		_qrRef.push_back(dix);
	}

}

/*====================== FONCTION D'AFFICHAGE =====================*/
void qrCode::printMessageBin()
{
	cout << "Le message sous forme binaire " << endl;
	int taille = _messageBin.size();
	for (int i = 0; i < taille; i++)
	{
		if ((i + 1) % 8 == 0) { cout << _messageBin[i] << "\n"; }
		else { cout << _messageBin[i]; }
	}
	cout << endl;
}

void qrCode::printCharCountBin()
{
	cout << "Le compte des characteres" << endl;
	int taille = _charCountIndicator.size();
	for (int i = 0; i < taille; i++)
	{
		cout << _charCountIndicator[i];
	}
	cout << endl;
}

void qrCode::printGenPolyCount()
{
	cout << "Le polynome generateur " << endl;
	int taille = _genPoly.size();
	for (int i = 0; i < taille; i++)
	{
		cout << _genPoly[i] << ", ";
	}
	cout << endl;
}

void qrCode::printMessagePoly()
{
	cout << "Le message sous forme de polynome" << endl;
	for (auto k : _messagePoly)
	{
		cout << k << " ";
	}
	cout << endl;
}
void qrCode::printEccPoly()
{
	cout << "Le polynome contenant les coefficients de chaque code correcteur d'erreur" << endl;
	for (auto k : _eccPoly)
	{
		cout << k << " ";
	}
	cout << endl;
}

void qrCode::printMessageDiv()
{
	cout << "Le polynome sous forme de groupe" << endl;
	int n = 1;
	cout << "Nb de groupe " << _messageGroup.size() << endl;
	for (vector<vector<int>> k : _messageGroup)
	{
		cout << "Nb de bloc " << k.size() << endl;
		for (vector<int> i : k)
		{
			cout << "Nb de data " << i.size() << endl;

			for (int j : i)
			{
				//cout << n <<": " << j << " / ";
				cout << j << " ";
				n++;
			}
			cout << endl;
		}
	}
	cout << endl;
}

void qrCode::printEccDiv()
{
	cout << "Le code correcteur d'erreur sous forme de groupe" << endl;
	int n = 1;
	cout << "Nb de groupe " << _eccGroup.size() << endl;
	for (vector<vector<int>> k : _eccGroup)
	{
		cout << "Nb de bloc " << k.size() << endl;
		for (vector<int> i : k)
		{
			cout << "Nb de data " << i.size() << endl;

			for (int j : i)
			{
				//cout << n << ": " << j << " / ";
				cout << j << " ";
				n++;
			}
			cout << endl;
		}
	}
	cout << endl;
}


void qrCode::printFinalMessage()
{
	cout << "Le message final " << endl;
	for (int i : _finalMessage)
	{
		cout << i << " ";
	}
	cout << endl;
}

void qrCode::printFinalMessageBin()
{
	cout << "Le message FINAL sous forme binaire " << endl;
	int taille = _finalMessageBin.size();
	for (int i = 0; i < taille; i++)
	{
		if ((i + 1) % 8 == 0) { cout << _finalMessageBin[i] << "\n"; }
		else { cout << _finalMessageBin[i]; }
	}
	cout << endl;
}

void qrCode::afficherFormat()
{
	cout << "Les bits de format " << endl;
	for (int i : _format)
	{
		cout << i;
	}
	cout << endl;
}

void qrCode::afficherQr()
{

	cout << "Le QR Code " << endl << endl << endl << endl;
	int taille = _qr.size();
	for (int i = 0; i < taille; i++)
	{
		cout << "                       ";
		for (int j = 0; j < taille; j++)
		{
			string a("--");
			//a = (_qr[i][j] == 1) ? "##" : "--";
			if (_qr[i][j] == 1) { a = "##"; }
			if (_qr[i][j] == 0) { a = "  "; }
			if (_qr[i][j] == 10) { a = "**"; }
			cout << a;
		}
		cout << endl;
	}
	cout << endl << endl << endl << endl;
}


void qrCode::afficherRef()
{

	cout << "Le QR Ref" << endl << endl << endl << endl;
	int taille = _qr.size();
	for (int i = 0; i < taille; i++)
	{
		cout << "                       ";
		for (int j = 0; j < taille; j++)
		{
			string a("--");
			//a = (_qr[i][j] == 1) ? "##" : "--";
			if (_qrRef[i][j] == 1) { a = "##"; }
			if (_qrRef[i][j] == 0) { a = "  "; }
			if (_qrRef[i][j] == 10) { a = "**"; }
			cout << a;
		}
		cout << endl;
	}
	cout << endl << endl << endl << endl;
}


void qrCode::debug()
{
	cout << "Mode : ";
	for(auto i:_mode)
	{
		cout << i;
	}
	cout << "\n";
	cout << "Version : " << _version << endl;
	cout << "Taille : " << _qr.size() << endl;
	cout << "Mask : " << _mask << endl;

}
/*========================POUBELLE / CODE NON UTILISE ===============================
/*
Pour le polynome générateur !


	/* Bon cette méthode marche pas, je sais pas d'où vient le problème mais celle du dessus est plus intuitive et marche mieux
	int taille = nbCodeWord[_version];
	_genPoly.resize(2);
	_genPoly[0] = 0;
	_genPoly[1] = 0;
	for (int i = 1; i < taille; i++)
	{
		_genPoly.push_back(0);
		vector<int> genPolyCopie(_genPoly.begin(), _genPoly.end());
		vector<int> polyMul{ i, 0};
		int degre = _genPoly.size();
		vector<int> termePositionK;
		vector<int> termePositionK1;
		for (int k = 0; k < degre - 1; k++)
		{
			int exposantPositionK = genPolyCopie[k+1] + polyMul[0];
			cout << "PositionK : " << exposantPositionK << endl;
			termePositionK.push_back(exposantPositionK % 256 + floor(exposantPositionK / 256));
			int exposantPositionK1 = genPolyCopie[k] + polyMul[1];
			cout << "PositionK1 : " << exposantPositionK1 << endl;

			termePositionK1.push_back(exposantPositionK1 % 256 + floor(exposantPositionK1 / 256));
		}
		int termePos0 = _genPoly[0] + polyMul[0];
		_genPoly[0] = termePos0 % 256 + floor(termePos0 / 256);
		for (int k = 1; k < degre - 1; k++)
		{
			if (logGF256[termePositionK[k - 1]] ^ logGF256[termePositionK1[k - 1]] != 0)
			{
				_genPoly[k] = antiLogGF256[logGF256[termePositionK[k - 1]] ^ logGF256[termePositionK1[k - 1]] - 1];
			}
			else { _genPoly[k] = 0; }
		}
	}
	///*
	/*
	int taille = nbCodeWord[_version];
	_genPoly.resize(taille);
	cout << "nb CodeWords " << taille << endl;
	for (int i = 0; i < taille; i++)
	{
		_genPoly[i] = 0;
	}
	//vector<int> multiPoly{ 0, 1 };
	for (int i = 0; i <= taille-1; i++)
	{
		vector<int> genPolyCop(_genPoly.begin(), _genPoly.end());
		vector<int> multiPoly{ 0, i };
		cout << "multiPoly :";
		for (auto p : multiPoly) { cout << p << " "; }
		cout << "\n GenPolyCopié :";
		for (auto p : genPolyCop) { cout << p << " "; }
		cout << "\n GenPoly :";
		for (auto p : _genPoly) { cout << p << " "; }
		cout << "\n polynome " << i << endl;
		for (int k = 0; k < i; k++)
		{
			cout << "Le terme multiplie " << (multiPoly[1] + genPolyCop[k]) % 256 + floor((multiPoly[1] + genPolyCop[k]) / 256) << endl;
			if (genPolyCop[k] != 0)
			{
				_genPoly[k] = logGF256[antiLogGF256[genPolyCop[k]] ^ antiLogGF256[(multiPoly[1] + genPolyCop[k]) % 256 + floor((multiPoly[1] + genPolyCop[k]) / 256)]];
			}
			else {
				_genPoly[k] = (multiPoly[1] + genPolyCop[k]) % 256 + floor((multiPoly[1] + genPolyCop[k]) / 256);
			}
			if (genPolyCop[k + 1] != 0)
			{
				_genPoly[k + 1] = logGF256[antiLogGF256[genPolyCop[k + 1]] ^ antiLogGF256[(multiPoly[0] + genPolyCop[k]) % 256 + floor((multiPoly[0] + genPolyCop[k]) / 256)]];
			}
			else {
				_genPoly[k + 1] = (multiPoly[0] + genPolyCop[k]) % 256 + floor((multiPoly[0] + genPolyCop[k]) / 256);

			}
		}

	}*/


	/*
	void qrCode::reedSolomon()

	int nbGroup = nbGroupeVersion[_version];
	int taille = nbCodeWord[_version];
	for (int p = 0; p < nbGroup; p++)
	{
		int nbBlock = nbBlockGroup[p][_version];
		cout << "test1" << endl;
		for (int i = 0; i < nbBlock; i++)
		{
			cout << "i " << i << endl;
			_eccPoly = _messageGroup[p][i];
			this->getEccPoly();
			//On commence par mettre à la bonne taille
			int taille = nbCodeWord[_version];
			for (int j = 0; j < taille; j++)
			{
				_eccPoly.push_back(-1);
			}
			//Le polynome générateur doit avoir la même taille que le polynome message !
			taille = _eccPoly.size();
			for (int j = 0; j < taille - 1; j++)
			{
				_genPoly.push_back(-1);
			}
			this->getEccPoly();
			int nbCode = _eccPoly.size();
			for (int k = 0; k < nbCode; k++)
			{
				taille = _genPoly.size();
				//Le polynome générateur ne change jamais donc on va en faire une copie pour
				//travailler dessus dans chaque boucle
				vector<int> genPoly(_genPoly.begin(), _genPoly.end());
				for (int i = 0; i < taille; i++) {
					int coefAjout = antiLogGF256[_eccPoly[0] - 1];
					if (genPoly[i] != -1)
					{
						genPoly[i] = ((_genPoly[i] + coefAjout) % 255);
					}
				}

				for (int d = 0; d < taille; d++)
				{
					//Opération XOR
					//Si le polynome générateur est différent de -1 et celui du message aussi, on XOR, sinon on le laisse identique
					if (genPoly[d] != -1 && _eccPoly[d] != -1)
					{
						_eccPoly[d] = (_eccPoly[d] ^ logGF256[genPoly[d]]);
					}
					//Si le polynome générateur est différent de -1 et celui du message vaut 1,
					//on XOR avec rien ce qui revient à attribuer la valeur

					if (genPoly[d] != -1 && _eccPoly[d] == -1)
					{
						_eccPoly[d] = logGF256[genPoly[d]];
					}
				}
				_eccPoly.erase(_eccPoly.begin());
				if (_genPoly[_genPoly.size() - 1] == -1) { _genPoly.pop_back(); }
			}
			_eccGroup[p].push_back(_eccPoly);
		}
	}
	}*/
