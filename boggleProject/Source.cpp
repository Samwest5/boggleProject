#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <chrono>
#include <thread>
#include <algorithm>

using namespace std;

const int ALPHABET_SIZE = 26;

// Trie structure based off of Geeks for Geeks implementation 
struct TrieNode {
	TrieNode *children[ALPHABET_SIZE];
	bool isEndOfWord;
};

struct TrieNode *getNode(void) {
	TrieNode *node = new TrieNode;
	node->isEndOfWord = false;
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		node->children[i] = nullptr;
	}
	return node;
}

void insert(TrieNode *root, string key) {
	TrieNode *node = root;
	for (int i = 0; i < key.size(); i++) {
		int index = key[i] - 'a';
		if (!node->children[index]) {
			node->children[index] = getNode();
		}
		node = node->children[index];
	}
	node->isEndOfWord = true;
}

bool isWord(TrieNode *root, string key) {
	TrieNode *node = root;
	for (int i = 0; i < key.size(); i++) {
		int index = key[i] - 'a';
		if (!node->children[index]) {
			return false;
		}
		node = node->children[index];
	}
	return (node && node->isEndOfWord);
}

class Boggle {
private:
	int rounds;
	TrieNode *dictionary = getNode();
	unordered_map<int, string>players;
	vector<vector<string>>board = { { "_", "_", "_", "_"}, { "_", "_", "_", "_" }, { "_", "_", "_", "_" }, { "_", "_", "_", "_" } };
	//vector<vector<string>>board = { { "q", "u", "i", "z" },{ "d", "u", "c", "k" },{ "t", "a", "c", "o" },{ "f", "r", "o", "g" } };
	unordered_map<int, float> scores;
	unordered_map<int, float> roundScores;
	unordered_map<int, unordered_set<string>> playerWords;
	unordered_set<string> validWords;
	vector<vector<string>>dice = {
								{"r", "i", "f", "o", "b", "x"},
								{"i", "f", "e", "h", "e", "y"},
								{"d", "e", "n", "o", "w", "s"},
								{"u", "t", "o", "k", "n", "d"},
								{"h", "m", "s", "r", "a", "o"},
								{"l", "u", "p", "e", "t", "s"},
								{"a", "c", "i", "t", "o", "a"},
								{"y", "l", "g", "k", "u", "e"},
								{"qu", "b", "m", "j", "o", "a"},
								{"e", "h", "i", "s", "p", "n"},
								{"v", "e", "t", "i", "y", "t"},
								{"b", "a", "l", "i", "y", "t"},
								{"e", "z", "a", "v", "n", "d"},
								{"r", "a", "l", "e", "s", "c"},
								{"u", "w", "i", "l", "r", "g"},
								{"p", "a", "c", "e", "m", "d"}
	};
public:
	Boggle(string filePath) {
		cout << "Loading Dictionary...\n\n";

		loadDictionary(filePath);

		cout << "Enter number of rounds\n";
		cin >> rounds;

		while (rounds <= 0 || rounds >= 10) {
			cout << "Number of rounds must be above 0 and less than 10. Enter again\n";
			cin >> rounds;
		}

		cout << "Enter number of players\n";
		int numPlayers;
		cin >> numPlayers;

		while (numPlayers < 2 || numPlayers > 5) {
			cout << "Number of players must be between 2 and 5\n";
			cin >> numPlayers;
		}

		for (int i = 0; i < numPlayers; i++) {
			string name = "";
			cout << "Enter name for Player " << i + 1 << ":\n";
			cin >> name;
			setPlayerName(i, name);
		}
		// The only thing we will initialize to not be empty in the constructor is the dictionary which we read from the file
		// Everything else either changes each round or requires user input
	}

	void loadDictionary(string filePath) {
		ifstream file;
		file.open(filePath);
		string word;
		while (file >> word) {
			insert(dictionary, word);
		}
		file.close();
	}

	void printBoard() {
		cout << "\n\tCURRENT BOARD:\n";
		for (int i = 0; i < board.size(); i++) {
			for (int j = 0; j < board[i].size(); j++) {
				cout << board[i][j];
				if (board[i][j].size() == 2) cout << "\t"; // Whitespace padding for qu vs 1 char
				else cout << " \t";
			}
			cout << endl << endl;
		}
	}
	
	void generateQualityBoard() { 
		generateBoard();
		findValidWords();
		while (validWords.size() < 20) {
			generateBoard();
			findValidWords();
		}
	}

	void generateBoard() {
		bool used[16] = { {false} };
		for (int i = 0; i < board.size(); i++) {
			for (int j = 0; j < board[i].size(); j++) {
				int currDie = rand() % 16;
				while(used[currDie]){
					currDie = rand() % 16;
				}
				board[i][j] = dice[currDie][rand() % 5];
				used[currDie] = true;
			}
		}
	}

	void findValidWordsHelper(int i, int j, string candidate, vector<vector<bool>>visited) {
		if (i > 3 || j > 3) return; // out of bounds
		if (i < 0 || j < 0) return;
		if (visited[i][j]) return; // seen before

		visited[i][j] = true;

		candidate += board[i][j];

		if (!isWord(dictionary, candidate)) {
			return;
		}

		if (isWord(dictionary, candidate) && candidate.size() > 2) {
			validWords.insert(candidate);
		}

		findValidWordsHelper(i + 1, j, candidate, visited); //down
		findValidWordsHelper(i, j + 1, candidate, visited); //right
		findValidWordsHelper(i + 1, j + 1, candidate, visited); //downright
		findValidWordsHelper(i - 1, j, candidate, visited); //up
		findValidWordsHelper(i, j - 1, candidate, visited); //left
		findValidWordsHelper(i - 1, j - 1, candidate, visited); //upleft
		findValidWordsHelper(i + 1, j - 1, candidate, visited); //downleft
		findValidWordsHelper(i - 1, j + 1, candidate, visited); //upright
	}

	void findValidWords() {
		
		validWords.clear(); //	Remove valid words from last round
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				string candidate = "";
				vector<vector<bool>>visited(4, vector<bool>(4, false));
				findValidWordsHelper(i, j, candidate, visited);
			}
		}
		
		 //This function should find all the valid English words on the current board of length 3 or greater
	}

	void printValidWords() {
		cout << "Valid words for this rounds board: \n";
		int counter = 10;
		for (string word : validWords) {
			if (counter == 0) {
				cout << endl;
				counter = 12;
			}
			cout << word << " ";
			counter -= 1;
			
		}
	}

	void setNumberRounds(int n) {
		rounds = n;
		// This function accepts an int representing the number of rounds (which you will read from the user in main) & sets the appropriate member variable
	}

	void setPlayerName(int n, string name) {
		players[n] = name;
		// This function accepts an int for player number & that player's name (which you will read form the user in main) & sets it in the appropriate member variable
	}

	void setPlayerList() {
		for (int i = 0; i < players.size(); i++) {
			//printBoard();
			cout << players[i] << " enter each word you found, one at a time\n";
			cout << "Enter 'z' when done: \n";
			string word;
			while (cin >> word) {
				if (word == "z") break;
				playerWords[i].insert(word);
			}
		}
		// This function accepts a player's name & a list of words that player found (which you will read from the user in main) & sets it in the appropriate member variable
	}

	void removeInvalidWords() {
		unordered_set<string>needRemoval;
		for (int i = 0; i < players.size(); i++) {
			for (string word : playerWords[i]) {
				bool found = false;
				for (int j = 0; j < players.size(); j++) {
					if (j == i) continue;
					if (playerWords[j].count(word)) {
						found = true;
						playerWords[j].erase(word);
					}
				if (found) needRemoval.insert(word);
				if (!validWords.count(word)) needRemoval.insert(word);
				}
			}
		}
		for (string word : needRemoval) {
			for (int i = 0; i < players.size(); i++) {
				if (playerWords[i].count(word)) playerWords[i].erase(word);
			}
		}

		// This function removes invalid words from each player's list (words not in the dictionary, words not on the board, words that are too short, & words other players found)
	}

	void calculatePlayerScores() {
		for (int i = 0; i < players.size(); i++) {
			for (string word : playerWords[i]) {
				if (word.size() < 3) roundScores[i] += 0;
				else if (word.size() == 3) roundScores[i] += 1;
				else if (word.size() == 4) roundScores[i] += 1;
				else if (word.size() == 5) roundScores[i] += 2;
				else if (word.size() == 6) roundScores[i] += 3;
				else if (word.size() == 7) roundScores[i] += 4;
				else if (word.size() >= 8) roundScores[i] += 11;
			}
			scores[i] += roundScores[i];
		}
		// This function calculates each player's score for the current round & updates the overall scores
	}

	void printPlayerStats() {
		cout << endl;
		for (int i = 0; i < playerWords.size(); i++) {
			cout << "Player: " << players[i] << endl;
			cout << "\tScore: " << roundScores[i] << endl;
			cout << "\tValid Words: \n\t";

			for (string word : playerWords[i]) {
				cout << word << " ";
			}
			cout << endl << endl;
		}
		roundScores.clear();
		// This function print's each player's name, followed by their scored for the round, followed by their VALID words for that round
	}

	void announceWinner() {
		int winnerNum = 0;
		cout << endl;
		for (int i = 0; i < scores.size(); i++) {
			if (scores[i] > scores[winnerNum]) winnerNum = i;
		}

		cout << "WINNER: " << players[winnerNum] << "!!! Score: " << scores[winnerNum] << endl << endl;
		cout << "Other Players: \n";

		for (int i = 0; i < scores.size(); i++) {
			if (i == winnerNum) {
				continue;
			}
			cout << players[i] << " Score: " << scores[i] << endl << endl;
		}
		// This function print's the final scores & announces the winner by name
	}

	int getRounds() {
		return rounds;
	}



};

void playGame() {

	while (true) {
		string filePath;
		cout << "Enter path for dictionary text file\n";
		cin >> filePath;
		filePath = "C:\\Users\\Sam\\Desktop\\boggleProject\\english3.txt";
		Boggle game = Boggle(filePath);
		for (int i = 0; i < game.getRounds(); i++) {
			game.generateQualityBoard();
			game.printBoard();
			cout << "Round: " << i + 1 << endl;
			cout << "You have 2 minutes to search for words..\n";
			chrono::seconds dura(10);
			this_thread::sleep_for(dura);
			game.setPlayerList();
			game.removeInvalidWords();
			game.calculatePlayerScores();
			game.printPlayerStats();
			game.printValidWords();

		}
		game.announceWinner();
		string choice;
		cout << "Play again? Enter y or n\n";
		cin >> choice;
		if (choice != "y") break;
		cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	}
}

int main() {
	srand(time(0));
	playGame();


}