/*
Author: Michael Puthawala
		michaelputhawala@gmail.com
Date: 12/4/2016
	  11:50pm

The following program plays the game of hangman. Hangman is a game which is usually taught to children where one player (the hider) thinks of a word, and the other player (the guesser) tried to guess that person's word by guessing letters one at a time. If the guesser guesses a word correctly, then the hider tells the guesser all positions where that letter appears. The game ends when the guesser successfully guesses all letters, or guesses incorrectly 9 times, whatever comes first.

This program plays the guesser.

Feel free use, alter, change, moodify, etc. this program as you fit. All I ask is that if you do modify this, then you include these comments.


Inspiration for this program came from Freakenomics episode 246 "How to Win Games and Beat People," and the following website:
	http://www.datagenetics.com/blog/april12012/
*/


#include <iostream>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <ctime>
#include <stdlib.h>

// Set this to 1 to have the program output where it looks for the dictionary on your computer.
#define _Loading_Output_ 0

// Set to true if you want to see what distributions the program calculates at each guess.
#define _Frequency_Output_ 0

// Set to true to generate a ton of output. Useful if you accidentally broke something and want to fix it.
#define _Debug_Output_ 0

// Set to true to have the program tell you what keys it found.
#define _Key_Output_ 0

// Set to true to display the instructions when the program starts up.
#define _Show_Instructions_ 0

#if _Debug_Output_ == 0
#define _Loading_Output_ 0
#endif

using namespace std;


/*
Generates a key (int) from a string and a character. This is combined with the map to store the distributions
*/
inline unsigned int int_from_string(const string& str, char c){
	unsigned int r = 0;
	unsigned int inc = 1;
	for (auto x : str){
		r += (x == c)*inc;
		inc *= 2;
	}
	return r;
}
/*
This functions loads the dictionary into the program, and stores them into words. words[3] contains all words of length 4, words[4] contains all words of length 5, etc...

*/
void load_words(vector<vector<string> > &words, ifstream& fin){
	string str;
	cout << "Loading words..." << endl;
	while (!fin.eof()){
		fin >> str;
		words[str.length() - 1].push_back(str);
	}
	cout << "Loaded:" << endl;
#if _Frequency_Output_ == 1
	for (auto x : words){
		if (x.size())
			cout << "\t" << x.size() << " words of length " << x[0].size() << endl;
	}
#endif
	cout << endl;

}
/*
This function defines how I compare two pairs. This function is used by the map data structure to organize the BST.
*/
bool comp(const pair <char, int> &a, const pair<char, int> &b) { return a.second > b.second; }

/*
This function can be used to load all words. This will only load words of length length from file stream fin.
*/
void load_words(vector<string>& words, size_t length, ifstream& fin){
	string str;
#if _Loading_Output_ == 1
	cout << "Loading words..." << endl;
#endif
	while (!fin.eof()){
		fin >> str;
		if (str.size() != length)
			continue;
		words.push_back(str);
	}
#if _Loading_Output_ == 1
	cout << "Loaded:" << endl;
#endif
#if _Frequency_Output_ == 1
	cout << "\t" << words.size() << " words of length " << length << endl;
	
#endif
	cout << endl;
}
/*
This function looks for a file with a given title, and produces some output if it does.
*/
bool look_for_file(const string& title){
#if _Loading_Output_ == 1
	cout << "Looking for a file named: \"" << title << "\"" << endl;
#endif
	ifstream fin(title);
	if (fin.is_open()){
#if _Loading_Output_ == 1
		cout << "Opening file \"" << title << "\"" << endl;
#endif
		return true;
	}
	else{
#if _Loading_Output_ == 1
		cout << "Did not find file \"" << title << "\"" << endl;
#endif
		return false;
	}
}

void get_letter_frequency(vector<pair<int, int>>& letter_frequency, const vector<string>& possible_words){
	int n = possible_words.size();
	for (int i = 0; i < n; i++){
		vector<bool> duplicate_letter(26, false);
		for (unsigned int j = 0, m = possible_words[i].size(); j < m; j++){
			char c = possible_words[i][j];
			if (!duplicate_letter[c - 'a']){
				duplicate_letter[c - 'a'] = true;
				letter_frequency[c - 'a'].first++;
			}
		}
	}
	for (auto& x : letter_frequency){
		x.second = n - x.first;
	}
#if _Frequency_Output_ == 1
	vector<pair<char, int> > tmp; (letter_frequency);
	for (int i = 0; i < letter_frequency.size(); i++)
		tmp.push_back(pair<char, int>((i + 'a'), letter_frequency[i].first));
	sort(tmp.begin(), tmp.end(),comp);
	for (unsigned int i = 0; i < 26; i++){
		cout << "\twords that contain " << tmp[i].first  << ": " << tmp[i].second << endl;
	}
#endif

}
/*
This function is unused.
unsigned int get_best_guess(const vector<pair<int, int>>& letter_frequency){
	int best_guess = 0;
	int min_max = 1e9;
	for (int i = 0; i < 26; i++){
		if (max(letter_frequency[i].first, letter_frequency[i].second) < min_max){
			best_guess = i;
			min_max = max(letter_frequency[i].first, letter_frequency[i].second);
		}
	}
	return best_guess;
}
*/
/*
This function returns true if el is in vec, and false otherwise.
*/
template<typename T>
inline bool in_vec(const vector<T>& vec, const T& el){
	for (auto x : vec)
		if (x == el)
			return true;
	return false;
}
/*
Once the distributions are computed, this function returns the best possible guess by taking the letter whose maximal bucket is minimal.
*/
size_t max_min(vector<map<int, int> >& letter_distributions ,
	const vector<vector<int> >& possible_keys,int total_words){
	vector<int> min(26);
	int min_tmp;
	int max = 0;
	size_t best_guess = 0;
	int key;
	for (unsigned int i = 0; i < 26; i++){
		min_tmp = 1e10;
		for (unsigned int j = 0; j < possible_keys[i].size(); j++){
			key = possible_keys[i][j];
			if (total_words - letter_distributions[i][key] < min_tmp)
				min_tmp = total_words - letter_distributions[i][key];
		}
		min[i] = min_tmp;
	}

	for (unsigned int i = 0; i < 26; i++){
		if (letter_distributions[i].size() == 0)
			continue;
#if _Debug_Output_ == 1
		cout << "Letter " << char('a' + i) << " has min: " << min[i] << endl;
#endif
		if (min[i] > max){
			max = min[i];
			best_guess = i;
		}
	}
#if _Debug_Output_ == 1
	cout << "Best guess: " << char('a' + best_guess) << endl;
#endif
	return best_guess;
}
/*
Most important function. this function is given a list of possible words, and from that list computes the best guess. This uses what we discussed in discussion.
*/
unsigned int get_best_guess(const vector<string>& possible_words, 
	const vector<char>& guessed_letters){
	vector<map<int, int> > letter_distributions(26);
	vector<vector<int> > possible_keys(26);
	vector<int> letter_totals(26,0);
	
	unsigned int n = possible_words.size();
	unsigned int m = possible_words[0].size();
	vector<char> used_letters;
	char c;
	int key;
	for (unsigned int i = 0; i < n; i++){
		// for each possible word
		used_letters = guessed_letters;
		for (unsigned int j = 0; j < m; j++){
			c = possible_words[i][j];
			if (!in_vec(used_letters, c)){
				used_letters.push_back(c);
				key = int_from_string(possible_words[i], c);
				if (letter_distributions[c - 'a'][key] == 0){
					letter_distributions[c - 'a'][key] = 1;
					possible_keys[c - 'a'].push_back(key);
				}
				else
					letter_distributions[c - 'a'][key]++;
				letter_totals[c - 'a']++;
			}
		}
	}
	for (unsigned int i = 0; i < 26; i++){
		if ((n - letter_totals[i]) && !in_vec(guessed_letters, char(i + 'a'))){
			possible_keys[i].push_back(0);
			letter_distributions[i][0] = n - letter_totals[i];
		}
	}
#if _Key_Output_
	cout << "Out of the words that you gave me, I found:" << endl;
	for (unsigned int j = 0; j < 26; j++){
		cout << "Letter: " << char(j + 'a') << endl;
		for (unsigned int i = 0; i < possible_keys[j].size(); i++){
			key = possible_keys[j][i];
			cout << "\t" << letter_distributions[j][key] << " had key " << key << endl;
		}
	}
#endif
	
	return max_min(letter_distributions,possible_keys,possible_words.size());
}
/*
Once a letter is guessed and the answer is given, this function updates the possible words.
*/
void update_possible_words(vector<string>& possible_words, int c, vector<int> positions){
	vector<string> r;
	int m = positions.size();
	if (m){
		for (size_t i = 0, n = possible_words.size(); i < n; i++){
			bool eligible = true;
			for (size_t j = 0; j < m; j++){
				if (possible_words[i][positions[j]] != c + 'a'){
					eligible = false;
					break;
				}
			}
			if (eligible){
				int count = 0;
				for (size_t k = 0, o = possible_words[i].size(); k < o; k++){
					if (possible_words[i][k] == c + 'a')
						count++;
				}
				if (count == positions.size())
					r.push_back(possible_words[i]);
			}
		}
	}
	else{
		for (size_t i = 0, n = possible_words.size(); i < n; i++){
			bool eligible = true;
			for (size_t j = 0; j < possible_words[i].size(); j++){
				if (possible_words[i][j] == c + 'a'){
					eligible = false;
					break;
				}
			}
			if (eligible)
				r.push_back(possible_words[i]);
		}
	}
	possible_words = r;
}
/*
This function outputs all of the guesses so far
*/
void print_guesses(const vector<bool>& guesses, const string& word){
	cout << "Your word:\t";
	for (size_t i = 0; i < word.size(); i++){
		cout << word[i] << " ";
	}
	cout << endl;
	cout << "Guessed Letters:\t";
	for (size_t i = 0; i < 26; i++){
		if (guesses[i])
			cout << char('a' + i) << " ";
	}
	cout << endl;
}
/*
This function outputs the gallows. To change the look of the gallows, change this function
*/
void print_gallows(int wrong_guesses){
	switch (wrong_guesses){
	case 0:
		cout << "               \n";
		cout << "               \n";
		cout << "               \n";
		cout << "               \n";
		cout << "               \n";
		cout << "               \n";
		cout << "               \n";
		break;
	case 1:
		cout << "               \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 2:
		cout << " _________     \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 3:
		cout << " _________     \n";
		cout << "|         |    \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 4:
		cout << " _________     \n";
		cout << "|         |    \n";
		cout << "|         0    \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 5:
		cout << " _________     \n";
		cout << "|         |    \n";
		cout << "|         0    \n";
		cout << "|         |    \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 6:
		cout << " _________     \n";
		cout << "|         |    \n";
		cout << "|         0    \n";
		cout << "|        /|    \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 7:
		cout << " _________     \n";
		cout << "|         |    \n";
		cout << "|         0    \n";
		cout << "|        /|\\  \n";
		cout << "|              \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 8:
		cout << " _________     \n";
		cout << "|         |    \n";
		cout << "|         0    \n";
		cout << "|        /|\\  \n";
		cout << "|        /     \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	case 9:
		cout << " _________     \n";
		cout << "|         |    \n";
		cout << "|         0    \n";
		cout << "|        /|\\  \n";
		cout << "|        / \\  \n";
		cout << "|              \n";
		cout << "|              \n";
		break;
	}
}
/*
int main(){
	vector<vector<string>> words(3);
	ifstream fin("words/words_test.txt");
	load_words(words, fin);
	vector<string> possible_words = words[2];
	//vector<string> possible_words = { "aaa", "aab" };
	vector<char> guessed_letters;
	guessed_letters.push_back('a');
	get_best_guess(possible_words,guessed_letters);

	system("pause");
}
//*/

/*
This function takes input from the user, and makes sure that the input is actually valid, so that the user does not enter something invalid like a letter of non-numeric symbol.
*/
void get_input(vector<int>& o_guesses){
	stringstream sstrm;
	string str;
	int i;
	bool input_is_valid;

	do{
		input_is_valid = true;
		getline(cin, str);
		for (size_t i = 0; i < str.size(); i++)
			if (str[i] != ' ' && !(str[i] >= '0' && str[i] <= '9') && str[i] != '\n')
				input_is_valid = false;
	} while (!input_is_valid);
	sstrm << str;

	while (sstrm >> i)
		o_guesses.push_back(i - 1);

	char c;

}
/*
This function returns a string which are the instructions that are going to be displayed.
*/
inline string instructions(){
	string r = "Ok, now I'm going to guess some letters. \n";
	r += "If I guess right, great! Write down the positions where I guessed your letter, seperated by spaces, and then press enter.\n";
	r += "If I guess wrong, then just go ahead and press enter.";
	return r;
}
/*
Simple utility function. Only used in example_word function to display example guesses and response.
*/
string find_char_in_string(const string& str, char c){
	string r;
	for (size_t i = 0, n = str.size(); i < n; i++)
		if (str[i] == c)
			r += to_string(i + 1) + " ";
	return r;
}

/*
This function provides an example of a word that the user could be thinking of, and a guess that the computer might make.
*/
inline string example_word(const vector<string>& possible_words){
	string r = "For example, let's say that your word was: ";
	string word = possible_words[rand()% possible_words.size()];
	r += word + "\n";
	r += "and I guessed the letter: ";
	char c = word[rand() % word.size()];
	r += c;
	r+= "\n";
	r += "Then you should type:\n";
	r += find_char_in_string(word, c);
	r += "[Enter]\n";
	return r;
}

/*
int main(){
	vector<int> v;
	get_input(v);
}
*/

///*
int main(){
	srand(time(nullptr));
	vector<string> default_file_names = {
		"Words.txt",
		"words.txt",
		"Dictionary.txt",
		"dictionary.txt",
		"wordsEn.txt",
		"WordsEn.txt",
		"DictionaryEn.txt",
		"dictionaryEn.txt"
	};//{ "wordsEn.txt", "wordsEn.txt", "words.txt", "words/words.txt" };
	string f_name;
	bool flag_prompt_for_input_file = true;
	for (size_t i = 0; i < default_file_names.size(); i++)
		if (look_for_file(default_file_names[i])){
			flag_prompt_for_input_file = false;
			f_name = default_file_names[i];
			break;
		}
	if (flag_prompt_for_input_file){
		cout << "Could not find any of the default dictionaries" << endl;
		cout << "Could you tell me where I can find a dictionary?" << endl;
		cout << "A file name would be best" << endl;
		cin >> f_name;
	}
	ifstream fin(f_name);
	
	while (!fin.is_open()){
		cout << "I couldn't find the dictionary \"" << f_name << "\"" << endl;
		cout << "Don't worry, I am very patient.\nTry again, and be sure to remember the .txt at the end!" << endl;
		cin >> f_name;
		fin.open(f_name);
	}
	
	vector< vector<string> > words;
	words.resize(28);
	string str;

	if (!fin.is_open())
		cout << "Could not open file: " << f_name << endl;
	else{
		//load_words(words, fin);
		//fin.close();

		cout << "How long is your word?" << endl;
		int word_length;
		cin >> word_length;
		cin.get();

		vector<string> possible_words;// = words[word_length - 1];

		load_words(possible_words, word_length, fin);
		fin.close();
		vector<bool> guessed(26, false);
		vector<char> guessed_letters;
		string word;

		for (int i = 0; i < word_length; i++)
			word = word + "_";
#if _Show_Instructions_ == 1
		cout << instructions() << endl;
		string more_examples;
		do{
			cout << endl << example_word(possible_words) << endl;
			cout << "Would you like to see another example?" << endl;
			cout << "[yes/no]";
			cin >> more_examples;
		} while (more_examples == "yes");
		cin.get();
#endif
		int guesses = 1;
		int wrong_guesses = 0;
		while (possible_words.size() > 1 && wrong_guesses < 9){
			// First entry, number of words with letter, second, number without
			vector<pair<int, int> > letter_frequency(26, pair<int, int>());
			get_letter_frequency(letter_frequency, possible_words);
			//int best_guess = get_best_guess(letter_frequency);
			int best_guess = get_best_guess(possible_words, guessed_letters);
			cout << "Best guess: " << char('a' + best_guess) << endl;
			cout << "Guess " << guesses << endl;
			guesses++;
			cout << "Does your word have a " << char(best_guess + 'a') << "?" << endl;
			guessed[best_guess] = true;
			guessed_letters.push_back(best_guess + 'a');
			int input;
			vector<int> known;
			
			
			
			//do{
			//	cin >> input;
			//	if (input != -1){
			//		known.push_back(input);
			//		word[input] = best_guess + 'a';
			//	}
			//} while (input != -1);
			
			get_input(known);

			for (size_t i = 0; i < known.size(); i++){
				word[known[i]] = best_guess + 'a';
			}
			
			if (!known.size())
				wrong_guesses++;

			update_possible_words(possible_words, best_guess, known);

			
			//if (possible_words.size() < 50){
			//	cout << "The words that I was thinking of were:" << endl;
			//		for (auto x : possible_words){
			//			cout << x << endl;
			//		}
			//}
			
			print_gallows(wrong_guesses);
			print_guesses(guessed, word);

			if (possible_words.size() == 0){
				cout << "Huh. I don't think that I know the word that you are thinking of." << endl;
				cout << "It must not be in my dictionary. If only I had a bigger one..." << endl;
				wrong_guesses = 9;
				break;
			}
		}
		if (wrong_guesses == 9){
			cout << "I lose =[" << endl;
			cout << "But really though, well done!" << endl;
			cout << "The words that I thought might be yours were:\n";
			for (auto x : possible_words){
				cout << "\a" << x << "\n";
			}
		}
		else
			cout << "Is your word: " << possible_words[0] << "?" << endl;
	}
	system("pause");
	return 0;
}
//*/