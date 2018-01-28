/**
 * @file C128.h
 * @brief The Code128 encoding library
 *
 * C128 - Code 128 library
 * Copyright (C) 2015 Brad Barnhill
 * mail: bradbarnhill@hotmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */
 
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "C128_Defines.h"
#include "bmpfile.h"
using namespace std;

enum TYPES { DYNAMIC, A, B, C };
enum COLUMN { COLUMN_VALUE, COLUMN_A, COLUMN_B, COLUMN_C, COLUMN_ENCODING };

struct element_t
{
	string Value;
	string A;
	string B;
	string C;
	string Encoding;
	element_t(){};

	element_t(string value, string a, string b, string c, string encoding) : Value(value), A(a), B(b), C(c), Encoding(encoding) { }
};

class Code128
{
private:
	string Raw_Data;
	string Encoded_Data;
	TYPES Type;
	element_t StartCharacter;
	vector<element_t*>* C128_Code;
	vector<string> mFormattedData;
	string ErrorMsg;

	element_t startAElement;
	element_t startBElement;
	element_t startCElement;
	element_t codeAElement;
	element_t codeBElement;
	element_t codeCElement;
	element_t codeStop;

	void init_Code128(string, TYPES);
	string CalculateCheckDigit();
	void BreakUpDataForEncoding();
	void InsertStartandCodeCharacters();
	string GetEncoding();
	vector<element_t> FindStartorCodeCharacter(string, int &);
	int findCharEncodingRow(COLUMN, string);
	char* intToAlpha(int value, char* result, int base);
	bool isNumber(const string &);
	void Error(string errMsg);
public:
	Code128(string);
	Code128(string, TYPES);
	void writeImage(int, unsigned int, string);
	~Code128();
};

Code128::Code128(string toEncode)
{
	Code128(toEncode, DYNAMIC);
}

Code128::Code128(string toEncode, TYPES type)
{
	Raw_Data = toEncode;

	init_Code128(toEncode, type);
}

Code128::~Code128()
{
	delete C128_Code;
}

void Code128::init_Code128(string toEncode, TYPES type)
{
	Raw_Data = toEncode;
	Type = type;
	ErrorMsg = "";
	Encoded_Data = "";

	C128_Code = new vector<element_t*>();
	C128_Code->push_back(new element_t("0", " ", " ", "00", "11011001100"));
	C128_Code->push_back(new element_t("1", "!", "!", "01", "11001101100"));
	C128_Code->push_back(new element_t("2", "\"", "\"", "02", "11001100110"));
	C128_Code->push_back(new element_t("3", "#", "#", "03", "10010011000"));
	C128_Code->push_back(new element_t("4", "$", "$", "04", "10010001100"));
	C128_Code->push_back(new element_t("5", "%", "%", "05", "10001001100"));
	C128_Code->push_back(new element_t("6", "&", "&", "06", "10011001000"));
	C128_Code->push_back(new element_t("7", "'", "'", "07", "10011000100"));
	C128_Code->push_back(new element_t("8", "(", "(", "08", "10001100100"));
	C128_Code->push_back(new element_t("9", ")", ")", "09", "11001001000"));
	C128_Code->push_back(new element_t("10", "*", "*", "10", "11001000100"));
	C128_Code->push_back(new element_t("11", "+", "+", "11", "11000100100"));
	C128_Code->push_back(new element_t("12", ",", ",", "12", "10110011100"));
	C128_Code->push_back(new element_t("13", "-", "-", "13", "10011011100"));
	C128_Code->push_back(new element_t("14", ".", ".", "14", "10011001110"));
	C128_Code->push_back(new element_t("15", "/", "/", "15", "10111001100"));
	C128_Code->push_back(new element_t("16", "0", "0", "16", "10011101100"));
	C128_Code->push_back(new element_t("17", "1", "1", "17", "10011100110"));
	C128_Code->push_back(new element_t("18", "2", "2", "18", "11001110010"));
	C128_Code->push_back(new element_t("19", "3", "3", "19", "11001011100"));
	C128_Code->push_back(new element_t("20", "4", "4", "20", "11001001110"));
	C128_Code->push_back(new element_t("21", "5", "5", "21", "11011100100"));
	C128_Code->push_back(new element_t("22", "6", "6", "22", "11001110100"));
	C128_Code->push_back(new element_t("23", "7", "7", "23", "11101101110"));
	C128_Code->push_back(new element_t("24", "8", "8", "24", "11101001100"));
	C128_Code->push_back(new element_t("25", "9", "9", "25", "11100101100"));
	C128_Code->push_back(new element_t("26", ":", ":", "26", "11100100110"));
	C128_Code->push_back(new element_t("27", ";", ";", "27", "11101100100"));
	C128_Code->push_back(new element_t("28", "<", "<", "28", "11100110100"));
	C128_Code->push_back(new element_t("29", "=", "=", "29", "11100110010"));
	C128_Code->push_back(new element_t("30", ">", ">", "30", "11011011000"));
	C128_Code->push_back(new element_t("31", "?", "?", "31", "11011000110"));
	C128_Code->push_back(new element_t("32", "@", "@", "32", "11000110110"));
	C128_Code->push_back(new element_t("33", "A", "A", "33", "10100011000"));
	C128_Code->push_back(new element_t("34", "B", "B", "34", "10001011000"));
	C128_Code->push_back(new element_t("35", "C", "C", "35", "10001000110"));
	C128_Code->push_back(new element_t("36", "D", "D", "36", "10110001000"));
	C128_Code->push_back(new element_t("37", "E", "E", "37", "10001101000"));
	C128_Code->push_back(new element_t("38", "F", "F", "38", "10001100010"));
	C128_Code->push_back(new element_t("39", "G", "G", "39", "11010001000"));
	C128_Code->push_back(new element_t("40", "H", "H", "40", "11000101000"));
	C128_Code->push_back(new element_t("41", "I", "I", "41", "11000100010"));
	C128_Code->push_back(new element_t("42", "J", "J", "42", "10110111000"));
	C128_Code->push_back(new element_t("43", "K", "K", "43", "10110001110"));
	C128_Code->push_back(new element_t("44", "L", "L", "44", "10001101110"));
	C128_Code->push_back(new element_t("45", "M", "M", "45", "10111011000"));
	C128_Code->push_back(new element_t("46", "N", "N", "46", "10111000110"));
	C128_Code->push_back(new element_t("47", "O", "O", "47", "10001110110"));
	C128_Code->push_back(new element_t("48", "P", "P", "48", "11101110110"));
	C128_Code->push_back(new element_t("49", "Q", "Q", "49", "11010001110"));
	C128_Code->push_back(new element_t("50", "R", "R", "50", "11000101110"));
	C128_Code->push_back(new element_t("51", "S", "S", "51", "11011101000"));
	C128_Code->push_back(new element_t("52", "T", "T", "52", "11011100010"));
	C128_Code->push_back(new element_t("53", "U", "U", "53", "11011101110"));
	C128_Code->push_back(new element_t("54", "V", "V", "54", "11101011000"));
	C128_Code->push_back(new element_t("55", "W", "W", "55", "11101000110"));
	C128_Code->push_back(new element_t("56", "X", "X", "56", "11100010110"));
	C128_Code->push_back(new element_t("57", "Y", "Y", "57", "11101101000"));
	C128_Code->push_back(new element_t("58", "Z", "Z", "58", "11101100010"));
	C128_Code->push_back(new element_t("59", "[", "[", "59", "11100011010"));
	C128_Code->push_back(new element_t("60", "\\", "\\", "60", "11101111010"));
	C128_Code->push_back(new element_t("61", "]", "]", "61", "11001000010"));
	C128_Code->push_back(new element_t("62", "^", "^", "62", "11110001010"));
	C128_Code->push_back(new element_t("63", "_", "_", "63", "10100110000"));
	C128_Code->push_back(new element_t("64", NUL, "`", "64", "10100001100"));
	C128_Code->push_back(new element_t("65", SOH, "a", "65", "10010110000"));
	C128_Code->push_back(new element_t("66", STX, "b", "66", "10010000110"));
	C128_Code->push_back(new element_t("67", ETX, "c", "67", "10000101100"));
	C128_Code->push_back(new element_t("68", EOT, "d", "68", "10000100110"));
	C128_Code->push_back(new element_t("69", ENQ, "e", "69", "10110010000"));
	C128_Code->push_back(new element_t("70", ACK, "f", "70", "10110000100"));
	C128_Code->push_back(new element_t("71", BEL, "g", "71", "10011010000"));
	C128_Code->push_back(new element_t("72", BS, "h", "72", "10011000010"));
	C128_Code->push_back(new element_t("73", TAB, "i", "73", "10000110100"));
	C128_Code->push_back(new element_t("74", LF, "j", "74", "10000110010"));
	C128_Code->push_back(new element_t("75", VT, "k", "75", "11000010010"));
	C128_Code->push_back(new element_t("76", FF, "l", "76", "11001010000"));
	C128_Code->push_back(new element_t("77", CR, "m", "77", "11110111010"));
	C128_Code->push_back(new element_t("78", SO, "n", "78", "11000010100"));
	C128_Code->push_back(new element_t("79", SI, "o", "79", "10001111010"));
	C128_Code->push_back(new element_t("80", DLE, "p", "80", "10100111100"));
	C128_Code->push_back(new element_t("81", DC1, "q", "81", "10010111100"));
	C128_Code->push_back(new element_t("82", DC2, "r", "82", "10010011110"));
	C128_Code->push_back(new element_t("83", DC3, "s", "83", "10111100100"));
	C128_Code->push_back(new element_t("84", DC4, "t", "84", "10011110100"));
	C128_Code->push_back(new element_t("85", NAK, "u", "85", "10011110010"));
	C128_Code->push_back(new element_t("86", SYN, "v", "86", "11110100100"));
	C128_Code->push_back(new element_t("87", ETB, "w", "87", "11110010100"));
	C128_Code->push_back(new element_t("88", CAN, "x", "88", "11110010010"));
	C128_Code->push_back(new element_t("89", EM, "y", "89", "11011011110"));
	C128_Code->push_back(new element_t("90", SUB, "z", "90", "11011110110"));
	C128_Code->push_back(new element_t("91", ESC, "{", "91", "11110110110"));
	C128_Code->push_back(new element_t("92", FS, "|", "92", "10101111000"));
	C128_Code->push_back(new element_t("93", GS, "}", "93", "10100011110"));
	C128_Code->push_back(new element_t("94", RS, "~", "94", "10001011110"));
	C128_Code->push_back(new element_t("95", US, DEL, "95", "10111101000"));
	C128_Code->push_back(new element_t("96", FNC3, FNC3, "96", "10111100010"));
	C128_Code->push_back(new element_t("97", FNC2, FNC2, "97", "11110101000"));
	C128_Code->push_back(new element_t("98", SHIFT, SHIFT, "98", "11110100010"));
	C128_Code->push_back(new element_t("99", CODE_C, CODE_C, "99", "10111011110"));
	C128_Code->push_back(new element_t("100", CODE_B, FNC4, CODE_B, "10111101110"));
	C128_Code->push_back(new element_t("101", FNC4, CODE_A, CODE_A, "11101011110"));
	C128_Code->push_back(new element_t("102", FNC1, FNC1, FNC1, "11110101110"));
	C128_Code->push_back(new element_t("103", START_A, START_A, START_A, "11010000100"));
	C128_Code->push_back(new element_t("104", START_B, START_B, START_B, "11010010000"));
	C128_Code->push_back(new element_t("105", START_C, START_C, START_C, "11010011100"));
	C128_Code->push_back(new element_t("", STOP, STOP, STOP, "11000111010"));

	startAElement = element_t("103", START_A, START_A, START_A, "11010000100");
	startBElement = element_t("104", START_B, START_B, START_B, "11010010000");
	startCElement = element_t("105", START_C, START_C, START_C, "11010011100");
	codeAElement  = element_t("101", FNC4, CODE_A, CODE_A, "11101011110");
	codeBElement  = element_t("100", CODE_B, FNC4, CODE_B, "10111101110");
	codeCElement  = element_t("99", CODE_C, CODE_C, "99", "10111011110");
	codeStop = element_t("", STOP, STOP, STOP, "11000111010");
}

string Code128::CalculateCheckDigit()
{
	string currentStartChar = mFormattedData[0];
	unsigned int CheckSum = 0;
	string s;
	element_t* row = new element_t();

	for (unsigned int i = 0; i < mFormattedData.size(); ++i)
	{
		//get char to find
		s = mFormattedData[i];

		//try to find value in the A column
		int index = findCharEncodingRow(COLUMN_A, s);
		if (index > -1)
		{
			//found in encoding type A
			row = C128_Code->at(index);
		}
		else
		{
			index = findCharEncodingRow(COLUMN_B, s);
			if (index > -1)
			{
				//found in encoding B
				row = C128_Code->at(index);
			}
			else
			{
				index = findCharEncodingRow(COLUMN_C, s);
				if (index > -1)
				{
					//found in encoding C
					row = C128_Code->at(index);
				}
				else
				{
					//TODO: not found in A B or C so this is an invalid character, how should we handle that?
				}
			}
		}

		unsigned int value = atoi(row->Value.c_str());
		unsigned int addition = value * ((i == 0) ? 1 : i);
		CheckSum +=  addition;
	}//for

	unsigned int Remainder = (CheckSum % 103);
	char * strRem = new char[0];
	row = C128_Code->at(findCharEncodingRow(COLUMN_VALUE, intToAlpha(Remainder, strRem, 10)));
	return row->Encoding;
}

void Code128::BreakUpDataForEncoding()
{
	string temp = "";
	string tempRawData = Raw_Data;

	//breaking the raw data up for code A and code B will mess up the encoding
	if (Type == A || Type == B)
	{
		for (unsigned int i = 0; i < Raw_Data.size(); ++i)
		{

			mFormattedData.push_back("" + Raw_Data[i]);
		}
		return;
	}//if
	else if (Type == C)
	{
		if (!isNumber(Raw_Data))
		{
			Error("EC128-6: Only numeric values can be encoded with C128-C.");
		}

		//CODE C: adds a 0 to the front of the Raw_Data if the length is not divisible by 2
		if (Raw_Data.size() % 2 > 0)
		{
			tempRawData = "0" + Raw_Data;
		}
	}//if

	for (unsigned int i = 0; i < tempRawData.size(); ++i)
	{
		char c = tempRawData[i];
		if (isdigit(c))
		{
			if (temp == "")
			{
				temp += c;
			}//if
			else
			{
				mFormattedData.push_back(temp + c);
				temp = "";
			}//else
		}//if
		else
		{
			if (temp != "")
			{
				mFormattedData.push_back(temp);
				temp = "";
			}//if
			mFormattedData.push_back("" + c);
		}//else
	}//for

	//if something is still in temp go ahead and push it onto the queue
	if (temp != "")
	{
		mFormattedData.push_back(temp);
		temp = "";
	}//if
}

void Code128::InsertStartandCodeCharacters()
{
	string currentCodeString = "";
	std::vector<string>::iterator it;
	it = mFormattedData.begin();

	if (Type != DYNAMIC)
	{
		switch (Type)
		{
		case A: mFormattedData.insert(it, START_A);
			break;
		case B: mFormattedData.insert(it, START_B);
			break;
		case C: mFormattedData.insert(it, START_C);
			break;
		default: Error("EC128-4: Unknown start type in fixed type encoding.");
			break;
		}
	}//if
	else
	{
		try
		{
			for (unsigned int i = 0; i < (mFormattedData.size()); ++i)
			{
				int col = 0;
				vector<element_t> tempStartChars = FindStartorCodeCharacter(mFormattedData[i], col);

				//check all the start characters and see if we need to stay with the same codeset or if a change of sets is required
				bool sameCodeSet = false;
				element_t row;
				for (unsigned int i = 0; i < tempStartChars.size(); ++i)
				{
					row = tempStartChars[i];
					if (row.A == currentCodeString || row.B == currentCodeString || row.C == currentCodeString)
					{
						sameCodeSet = true;
						break;
					}//if
				}//foreach

				//only insert a new code char if starting a new codeset
				//if (CurrentCodeString == "" || !tempStartChars[0][col].ToString().EndsWith(CurrentCodeString)) /* Removed because of bug */

				if (currentCodeString == "" || !sameCodeSet)
				{
					element_t CurrentCodeSet = tempStartChars[0];

					if (CurrentCodeSet.A == START_A || CurrentCodeSet.B == START_A || CurrentCodeSet.C == START_A)
					{
						currentCodeString = START_A;
					}
					else if (CurrentCodeSet.A == START_B || CurrentCodeSet.B == START_B || CurrentCodeSet.C == START_B)
					{
						currentCodeString = START_B;
					}
					else if (CurrentCodeSet.A == START_C || CurrentCodeSet.B == START_C || CurrentCodeSet.C == START_C)
					{
						currentCodeString = START_C;
					}
					else
					{
						Error("No start character found in CurrentCodeSet.");
					}

					mFormattedData.insert(it++, currentCodeString);
				}//if
			}//for
		}
		catch (const std::exception& e)
		{
			Error("EC128-3: Could not insert start and code characters.\n Message: " + (string)e.what());
		}//catch
	}//else
}

vector<element_t> Code128::FindStartorCodeCharacter(string s, int &col)
{
	vector<element_t> rows = vector<element_t>();

	//if two chars are numbers (or FNC1) then START_C or CODE_C
	if (s.size() > 1 && (isdigit(s[0]) || &s[0] == FNC1) && (isdigit(s[1]) || &s[1] == FNC1))
	{
		if (!&StartCharacter)
		{
			StartCharacter = startCElement;
			rows.push_back(StartCharacter);
		}//if
		else
			rows.push_back(startCElement);

		col = 1;
	}//if
	else
	{
		bool AFound = false;
		bool BFound = false;
		for (unsigned int i = 0; i < C128_Code->size(); ++i)
		{
			element_t* row = C128_Code->at(i);
			try
			{
				if (!AFound && s == row->A)
				{
					AFound = true;
					col = 2;

					if (!&StartCharacter)
					{
						StartCharacter = startAElement;
						rows.insert(rows.begin(), StartCharacter);
					}//if
					else
					{
						rows.insert(rows.begin(), codeAElement);
					}//else
				}//if
				else if (!BFound && s == row->B)
				{
					BFound = true;
					col = 1;

					if (!&StartCharacter)
					{
						StartCharacter = startBElement;
						rows.insert(rows.begin(), StartCharacter);
					}//if
					else
					{
						rows.insert(rows.begin(), codeBElement);
					}
				}//else
				else if (AFound && BFound)
				{
					break;
				}
			}//try
			catch (const std::exception& e)
			{
				Error("EC128-1: " + (string)e.what());
			}//catch
		}//foreach

		if (rows.size() <= 0)
		{
			Error("EC128-2: Could not determine start character.");
		}
	}//else

	return rows;
}

string Code128::GetEncoding()
{
	mFormattedData.clear();

	//break up data for encoding
	BreakUpDataForEncoding();

	//insert the start characters
	InsertStartandCodeCharacters();

	string CheckDigit = CalculateCheckDigit();

	Encoded_Data = "";
	for (unsigned int i = 0; i < mFormattedData.size(); ++i)
	{
		string s = mFormattedData[i];

		//handle exception with apostrophes in select statements
		element_t* E_Row = NULL;

		//select encoding only for type selected
		switch (Type)
		{
			int ind;
		case A:
			ind = findCharEncodingRow(COLUMN_A, s);
			if (ind >= 0)
			{
				E_Row = C128_Code->at(ind);
			}
			break;
		case B:
			ind = findCharEncodingRow(COLUMN_B, s);
			if (ind >= 0)
			{
				E_Row = C128_Code->at(ind);
			}
			break;
		case C:
			ind = findCharEncodingRow(COLUMN_C, s);
			if (ind >= 0)
			{
				E_Row = C128_Code->at(ind);
			}
			break;
		case DYNAMIC:
			ind = findCharEncodingRow(COLUMN_A, s);

			if (ind < 0)
			{
				ind = findCharEncodingRow(COLUMN_B, s);

				if (ind < 0)
				{
					ind = findCharEncodingRow(COLUMN_C, s);
				}//if
			}//if

			if (ind >= 0)
			{
				E_Row = C128_Code->at(ind);
			}
			break;
		default:
			break;
		}//switch

		if (E_Row == NULL)
		{
			Error("EC128-5: Could not find encoding of a value( " + s + " ) in C128");
			return NULL;
		}

		Encoded_Data += E_Row->Encoding;
	}//foreach

	//add the check digit
	Encoded_Data += CheckDigit;

	//add the stop character
	Encoded_Data += codeStop.Encoding;

	//add the termination bars
	Encoded_Data += "11";

	return Encoded_Data;
}

void Code128::Error(string errMsg)
{
	//TODO: Handle exceptions uniformly
}

string replaceOnce(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return str;
	str.replace(start_pos, from.length(), to);
	return str;
}

int Code128::findCharEncodingRow(COLUMN codeType, string value)
{
	for (unsigned int i = 0; i < C128_Code->size() - 1; ++i)
	{
		switch(codeType)
		{
		case COLUMN_A:
			if (C128_Code->at(i)->A == value)
			{
				return i;
			}
			break;
		case COLUMN_B:
			if (C128_Code->at(i)->B == value)
			{
				return i;
			}
			break;
		case COLUMN_C:
			if (C128_Code->at(i)->C == value)
			{
				return i;
			}
			break;
		case COLUMN_VALUE:
			if (C128_Code->at(i)->Value == value)
			{
				return i;
			}
			break;
		default:
			break;
		};
	}

	return -1;
}

/**
* REQUIRED due to itoa not being a standard in ANSI C++
*
* C++ version 0.4 char* style "itoa":
* Written by Luk·s Chmela
* Released under GPLv3.
*/
char* Code128::intToAlpha(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) 
	{ 
		*result = '\0'; 
		return result; 
	}

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) 
		*ptr++ = '-';

	*ptr-- = '\0';

	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

bool Code128::isNumber(const string& s)
{
	return !s.empty() && s.find_first_not_of("-.0123456789") == string::npos;
}

void Code128::writeImage(int barWidthInPx, unsigned int heightInPx, string fileName)
{
	string encodedValue = GetEncoding();
	unsigned int widthInPx = encodedValue.size() * barWidthInPx;

	bmpfile_t *bmp;
	rgb_pixel_t wh = {255, 255, 255, 0};
	rgb_pixel_t bl = {0, 0, 0, 0};

	if ((bmp = bmp_create(widthInPx, heightInPx, 1)) != NULL) 
	{
		for (unsigned int y = 0; y < heightInPx; ++y)
		{
			for (unsigned int x = 0; x < encodedValue.length(); ++x)
			{
				for (int pxs = 0; pxs < barWidthInPx; ++pxs)
				{
					bmp_set_pixel(bmp, ((x * barWidthInPx) + pxs), y, encodedValue[x] == '1' ? bl : wh);
				}
			}
		}

		bmp_save(bmp, fileName.c_str());
		bmp_destroy(bmp);
	}
}
