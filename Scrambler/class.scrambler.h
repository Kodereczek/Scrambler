// Filename: class.scrambler.h
// Author:   Patryk Po³eæ

#pragma once

class C_Scrambler
{
	// Constructor and destructor
	public:
		C_Scrambler();
		~C_Scrambler();

	// Public methods
	public:
	  string Encrypt(string _data, int _key);
	  string Decrypt(string _data, int _key);

	// Private methods
	private:

	// Public fields
	public:

	// Private fields
	private:

};