#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream> 
using namespace std;

string Cipher(short key, string plaintext);
string Decryptor(short key, string encrypted_text);
string Bruteforce(string encrypted_text);
void readVocab(vector<string> &vocab);
bool Bruteforce_Successful(string plaintext);

vector<string> vocab;

int main()
{
	string plaintext;
	string encrypted_text;
	short key;
	char choice;
	cout << "Welcome to (De)CipherMe please select an option: \n";
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
			cout << "Enter your text: ";
			getline(cin, plaintext);
			cout << "\nEnter your key: ";
			cin >> key;
			encrypted_text = Cipher(key, plaintext);
			cout << "\nYour enrcypted text is: " << encrypted_text;
			break;
		}
		case '2':
		{
			string filename;
			cout << "Enter filename: ";
			cin >> filename;
			cout << "Enter your key: ";
			cin >> key;
			ifstream input(filename);
			string line;
			if (input.is_open())
			{
				while (getline(input, line)){plaintext += line;}
				input.close();
				encrypted_text = Cipher(key, plaintext);
				cout << "Your encrypted text is: " << encrypted_text;
			}
			else cout << "Error, the file is not open!\n";
			break;
		}
		case '3':
		{
			char isfile;
			cout << "Decrypt from file or enter enter text via console?: (1/0)";
			cin >> isfile;
			if (isfile == '1')
			{
				string filename, line;
				cout << "Enter filename: ";
				cin >> filename;
				ifstream input(filename);
				if (input.is_open())
				{
					input >> key;
					while (getline(input, line)) { encrypted_text += line; }
					input.close();
					plaintext = Decryptor(key, encrypted_text);
					cout << "Decrypted text is : " << plaintext << endl;
				}
				else cout << "Error while opening the file!\n";
			}
			else
			{
				cin.ignore();
				cout << "Enter the encrypted text: ";
				getline(cin, encrypted_text);
				cout << "Enter the key: ";
				cin >> key;
				plaintext = Decryptor(key, encrypted_text);
				cout << "Decrypted text is: " << plaintext << endl;
			}
			break;
		}
		case '4':
		{
			vocab.reserve(1001);
			readVocab(vocab);
			char isfile;
			cout << "Decrypt from file or enter enter text via console?: (1/0)";
			cin >> isfile;
			if (isfile == '1')
			{
				string filename;
				cout << "Enter the filename: ";
				cin >> filename;
				ifstream input(filename);
				if (input.is_open())
				{
					string line;
					while (getline(input, line)) { encrypted_text += line; }
					short i = 0;
					cout << "Bruteforcing decryption of text: " << encrypted_text << endl;
					while (i != 26)
					{
						cout << "Trying key: " << i << "..." << endl;
						plaintext = Decryptor(i, encrypted_text);
						if (Bruteforce_Successful(plaintext))
						{
							cout << "Text has been decrypted with key: " << i << ". Your text is: \n";
							cout << plaintext << endl;
							break;
						}
						i++;
					}
				}
				else cout << "Error while opening the file!\n";
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
	regex pattern("[ ,-\.\?!:]");
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

string Bruteforce(string encrypted_text)
{
	vector<string> split_words;
	string possible_plaintext, temp;
	int counter = 0;
	while (counter != 26)
	{
		possible_plaintext = Decryptor(counter, encrypted_text);
		cout << possible_plaintext << endl;
		stringstream ss(possible_plaintext);
		while (getline(ss, temp, ' ')) split_words.push_back(temp);
		for (int i = 0; i < split_words.size(); i++)
		{
			cout << split_words[i] << endl;
		}
		system("pause");
		counter++;
	}
	return possible_plaintext;
}

void readVocab(vector<string> &vocab)
{
	ifstream input("vocabulary.txt");
	string word;
	if (input.is_open())
	{
		int i = 0;
		while (getline(input, word))
		{
			vocab.push_back(word);
			i++;
		}
		input.close();
	}
	else cout << "Error while opening the file!\n";
	
}

bool Bruteforce_Successful(string plaintext)
{
	for (int i = 0; i < vocab.size(); i++)
	{
		string word = vocab[i];
		regex r("\\b" + word + "\\b");
		smatch m;
		if(regex_search(plaintext, m, r))
			return true;
	}
	return false;
}