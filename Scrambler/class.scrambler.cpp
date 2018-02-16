// Filename: class.scrambler.cpp
// Author:   Patryk Po³eæ

#include "engine.h"

////////////////////////////////////////////////////////////////////////////////////////////////// Constructor and destructor
C_Scrambler::C_Scrambler()
{
	
}

C_Scrambler::~C_Scrambler()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////// Public methods
string C_Scrambler::Encrypt(string _data, int _key)
{
	if(_data == "") return "";

	if(_key <   0) _key = 0;
	if(_key > 128) _key = 128;

	string data = "";
	char sign[10];

	for(int l = 0; l < (int)_data.length(); l++)
	{
		     if(_data[l] == '\r') data += "0d";
		else if(_data[l] == '\n') data += "0a";
		else {_itoa_s(_data[l] + _key, sign, 16); data += sign;}
	}

	return data;
}

string C_Scrambler::Decrypt(string _data, int _key)
{
	if(_data == "") return "";

	if(_key <   0) _key = 0;
	if(_key > 128) _key = 128;

	string data = "";
	char sign[10];


	for(int l = 0; l < (int)_data.length(); l++)
	{
		if(l%2 == 0)
		{
				 if(l == (int)_data.length() - 1) return data;
			else if(_data[l] == '0' && _data[l+1] == 'd') data += "\r";
			else if(_data[l] == '0' && _data[l+1] == 'a') data += "\n";
			else {sign[0] = _data[l]; sign[1] = _data[l+1]; data += (char)(strtol(sign, NULL, 16) - _key);}
		}
	}

	return data;
}