#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include "ConsoleColor.h"
#include <filesystem>
#include <unordered_set>
#include <chrono>
using namespace std;
namespace fs = filesystem;

string Cipher(short key, string plaintext);
string Decryptor(short key, string encrypted_text);
string Bruteforce(string encrypted_text);
string SplitFilename(string filename);
void setWordLengthBounds(string text, unordered_set<short> &word_len);
void readVocab(vector<string> &vocab, string filename, unordered_set<short> word_len);
bool Bruteforce_Successful(string plaintext);

vector<string> vocab;

int main()
{
	string plaintext;
	string encrypted_text;
	unordered_set<short> word_len;
	short key;
	char choice;
	bool cracked = false;
	cout << green << "Welcome to (De)CipherMe please select an option: \n";
	cout << "1. Encrypt a text \n";
	cout << "2. Encrypt a text from a txt file \n";
	cout << "3. Decrypt a text (with a known key) \n";
	cout << "4. Bruteforce decryption \n";
	cin >> choice;
	system("cls");
	cin.ignore();
	switch(choice)
	{
		case '1':
		{
			cout << "Enter your text: " << white;
			getline(cin, plaintext);
			cout << green << "\nEnter your key: " << white;
			cin >> key;
			cout << endl;
			cout << green << "Encrypting text: " << blue << plaintext << endl;
			encrypted_text = Cipher(key, plaintext);
			cout << green << "\nYour enrcypted text is: " << red << encrypted_text << white;
			break;
		}
		case '2':
		{
			string filename;
			cout << "Enter filename: " << white;
			cin >> filename;
			cout << endl;
			cout << green << "Enter your key: " << white;
			cin >> key;
			cout << endl;
			ifstream input(filename);
			string line;
			if (input.is_open())
			{
				while (getline(input, line)){plaintext += line;}
				input.close();
				cout << green << "Encrypting text: " << plaintext << "\n";
				cout << endl;
				encrypted_text = Cipher(key, plaintext);
				cout << green << "Your encrypted text is: " << red << encrypted_text << white;
				cout << endl;
			}
			else cout << red << "Error, the file is not open!\n" << white;
			break;
		}
		case '3':
		{
			char isfile;
			cout << "Decrypt from file or enter enter text via console?(1/0): " << white;
			cin >> isfile;
			if (isfile == '1')
			{
				string filename, line;
				cout << green << "Enter filename: " << white;
				cin >> filename;
				cout << green << "Enter the key: " << white;
				cin >> key;
				ifstream input(filename);
				if (input.is_open())
				{
					while (getline(input, line)) { encrypted_text += line; }
					cout << green << "Decrypting text: " << red <<  encrypted_text << endl;
					input.close();
					plaintext = Decryptor(key, encrypted_text);
					cout << green << "Decrypted text is : " << blue << plaintext << endl;
				}
				else cout << "Error while opening the file!\n";
			}
			else
			{
				cin.ignore();
				cout << green << "Enter the encrypted text: " << white;
				getline(cin, encrypted_text);
				cout << green << "Enter the key: " << white;
				cin >> key;
				cout << green << "Decrypting text: " << red << encrypted_text;
				plaintext = Decryptor(key, encrypted_text);
				cout << endl;
				cout << green << "Decrypted text is: " << blue << plaintext << white;
			}
			break;
		}
		case '4':
		{
			cout << green << "Available dictionaries: \n";
			cout << endl;
			string path = fs::current_path().string() + "/dictionaries/";
			for (const auto& entry : fs::directory_iterator(path))
			{
				string file = entry.path().string();
				cout << yellow << SplitFilename(file) << endl;
			}
			cout << endl;
			cout << green << "Enter dictionary filename: " << white;
			string voc_name;
			cin >> voc_name;
			char isfile;
			cout << "Decrypt from file or enter enter text via console?(1/0): " << white;
			cin >> isfile;
			cout << endl;
			if (isfile == '1')
			{
				string filename;
				cout << green << "Enter the filename: " << white;
				cin >> filename;
				cout << endl;
				ifstream input(filename);
				if (input.is_open())
				{
					string line;
					while (getline(input, line)) { encrypted_text += line; }
					setWordLengthBounds(encrypted_text, word_len);
					readVocab(vocab, path + voc_name, word_len);
					short i = 1;
					cout << green << "Bruteforcing decryption of text: " << red << encrypted_text << endl;
					cout << endl;
					std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
					while (i != 26)
					{
						cout << green << "Trying key: " << red <<  i << yellow << " ..." << endl;
						plaintext = Decryptor(i, encrypted_text);
						if (Bruteforce_Successful(plaintext))
						{
							cracked = true;
							cout << endl;
							cout << green << "Text has been decrypted with key: " << blue <<  i  << green <<
								" Your text is: ";
							cout << blue << plaintext << white << endl;
							break;
						}
						i++;
					}
					std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
					std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
					std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[s]" << std::endl;
					if (!cracked) cout << red << "Bruteforce failed, try another dictionary or expand current one.\n" << white;
				}
				else cout << red << "Error while opening the file!\n" << white;
			}
		}
		break;
		default:
			cout << "Error\n";
	}

}

string Cipher(short key, string plaintext)
{
	transform(plaintext.begin(), plaintext.end(), plaintext.begin(),[](unsigned char c) { return std::tolower(c);});
	regex pattern("[ ,-\.\?!:]");
	for (int i = 0; i < plaintext.length(); i++)
	{
		char cipher_letter;
		string letter = "";
		letter += plaintext[i];
		if (regex_match(letter, pattern)) 
			cipher_letter = plaintext[i];
		else
		{
			if (plaintext[i] + key > 'z')
				cipher_letter = toupper(plaintext[i] - 'z' + 'a' - 1 + key);
			else cipher_letter = toupper(plaintext[i] + key);
		}
		plaintext[i] = cipher_letter;
	}
	return plaintext;
}

string Decryptor(short key, string encrypted_text)
{
	transform(encrypted_text.begin(), encrypted_text.end(), encrypted_text.begin(), [](unsigned char c) { return std::tolower(c); });
	regex pattern("[ ,'-\.\?!:]");
	for (int i = 0; i < encrypted_text.length(); i++)
	{
		char decrypted_letter;
		string letter;
		letter += encrypted_text[i];
		if (regex_match(letter, pattern)) 
			decrypted_letter = encrypted_text[i];
		else
		{
			if (encrypted_text[i] - key < 'a') 
				decrypted_letter = toupper(encrypted_text[i] + 'z' - 'a' + 1 - key);
			else decrypted_letter = toupper(encrypted_text[i] - key);
		}
		encrypted_text[i] = decrypted_letter;
	}
	return encrypted_text;
}

void readVocab(vector<string> &vocab, string filename, unordered_set<short> word_len)
{
	ifstream input(filename);
	string word;
	if (input.is_open())
	{
		int i = 0;
		while (getline(input, word))
		{
			if(word_len.find(word.length()) != word_len.end()) vocab.push_back(word);
			i++;
		}
		input.close();
	}
	else cout << "Error while opening the file!\n";
	
}

bool Bruteforce_Successful(string plaintext)
{
	smatch m;
	for (int i = 0; i < vocab.size(); i++)
	{
		string word = vocab[i];
		regex r("\\b" + word + "\\b");
		if(regex_search(plaintext, m, r))
			return true;
	}
	return false;
}

string SplitFilename(string filename)
{
	int len = filename.length();
	size_t found = filename.find_last_of("/\\");
	return filename.substr(found + 1);
}

void setWordLengthBounds(string text, unordered_set<short> &word_len)
{
	istringstream ss(text);
	do
	{
		string word;
		ss >> word;
		if (!ss) break;
		word_len.insert(word.length());
	} while (ss);
	unordered_set<short> ::iterator itr;
	for (itr = word_len.begin(); itr != word_len.end(); itr++)
		cout << (*itr) << endl;
}