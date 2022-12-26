//
// 	STARTER CODE: util.h
//
//  By: Sergio Ambelis Diaz
//

#include <iostream>
#include <fstream>
#include <map>
#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <functional>     // std::greater
#include <string>
#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"
#pragma once

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

// Priority queue class to count for HuffmanNode
class prioritize {
 public:
  bool operator()(const pair<HuffmanNode*, int>& p1,
				  const pair<HuffmanNode*, int>& p2) const {
    return p1.second > p2.second;  // ascending order
  }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
  if (node != nullptr) {  // recursion call while not null
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
  }  // else node nullptr
  return;
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmap &map) {
	if (isFile) {  // isFile is true then it's a filename
		ifstream inFS(filename);  // open filename
		char c;
		while (inFS.get(c)) {  // passes c as key to get value
		int nKey = int(c);  // typecast char c to an int c
		if (map.containsKey(nKey)) {  // if key is in map (true/false)
			int value = map.get(nKey);  // get new value
			value++;  // add to value. (frequency)
			map.put(nKey, value);  // inputs the new incremented value
		} else {
			map.put(nKey, 1);  // put in 1 for empty file
			}
		}
	} else {  // isFile is false then it's a string
		for (char c : filename) {
		int nKey = int(c);  // typecast char c to an int c
		if (map.containsKey(nKey)) {  // if key is in map
			int value = map.get(nKey);  // get new value
			value++;  // add to value the count of characters
			map.put(nKey, value);  // inputs the new incremented value
		} else {
			map.put(nKey, 1);  // put in 1 for empty
			}
		}
	}
	// Input the EOF pseudo-code
	map.put(256, 1);
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmap &map) {
	priority_queue<
	  pair<HuffmanNode*, int>,   // (key,value) pair
	  vector<pair<HuffmanNode*, int>>,  // store pairs in a vector
	  prioritize> pq;  // function object

	  // Assign values to the priority queue map
	  for (int x : map.keys()) {
		HuffmanNode* node = new HuffmanNode();  // create new instance of node
		int value = map.get(x);  // get value and update below:
		node->character = x;
		node->count = value;
		node->zero = nullptr;
		node->one = nullptr;
		// push accordingly in queue
		pq.push(pair<HuffmanNode*, int>(node, node->count));
	  }

	  HuffmanNode* node = nullptr;  // create new instance of node
	  HuffmanNode* first = nullptr;
	  HuffmanNode* second = nullptr;

	  // create an enconding tree
	  while (1 < pq.size()) {  // stop prior to last node
		pair<HuffmanNode*, int> topN = pq.top();  // grab first node
		pq.pop();  // pop it
		first = topN.first;  // put first topN node in first
		topN = pq.top();  // grab second node
		pq.pop();
		second = topN.first;
		node = new HuffmanNode();  // create a new node
		node->character = NOT_A_CHAR;  // create new node N_A_C
		// link them up to become a binary tree
		node->count = first->count + second->count;
		node->zero = first;  // assign first node to the left 0
		node->one = second;  // assign second node to the right 1
		pq.push(pair<HuffmanNode*, int> (node, node->count));  // enqueue new node
	  }  // get last node from the root
	  pair<HuffmanNode*, int> root = pq.top();
	  pq.pop();  // pop root node
    return root.first;
}

// helper for buildEncodingMap
void iterEncoding(HuffmanNode* node,
                 mymap<int, string>& encodingMap, string str) {
	if (node != nullptr) {
		if (node->character != NOT_A_CHAR) {  // assign key and value to map
		  encodingMap.put(node->character, str);
		}  // else node->char is a N_A_C
		str += "0";  // add a zero to go left
		iterEncoding(node->zero, encodingMap, str);  // traverse to left
		str.pop_back();  // pop from tail of string
		str += "1";  // add a 1 to go right
		iterEncoding(node->one, encodingMap, str);  // traverse to right
		str.pop_back();  // pop from tail in string
  }
  return;
}

//
// *This function builds the encoding map from an encoding tree.
//
mymap <int, string> buildEncodingMap(HuffmanNode* tree) {
    mymap <int, string> encodingMap;
    string str;
	iterEncoding(tree, encodingMap, str);  // helper function
    return encodingMap;
}


// helper function to traverse encoding
void traverseEncode(string str, mymap <int, string>& map,
                int &size, bool stmt, string &result) {
  if (stmt) {  // true: encode the whole string
    for (char c : str) {  // for each elem in string binary
      for (char x : map.get(c)) {  // get each value
		size++;
        result += x;  // append value in result
      }
    }
  } else {
    // encode the end statement
    for (char x : map.get('\n')) {  
      size++;
      result += x;   // append last value in result
    }
  }
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, mymap <int, string> &encodingMap,
              ofbitstream& output, int &size, bool makeFile) {
  string result, str;  // result and str (holds the binary string)
  getline(input, str);  // get from input and store in str the binary code
  // traverse or loop tree function
  traverseEncode(str, encodingMap, size, true, result);

  while (!input.eof()) {  // traverse using helper function
	traverseEncode(str, encodingMap, size, false, result);
	getline(input, str);
	traverseEncode(str, encodingMap, size, true, result);
  }
  // append EOF character
  for (char x : encodingMap.get(PSEUDO_EOF)) {
	result += x;
	size++;
  }
  if (makeFile == false) {  // false: writeBit 0
    } else {  // else 1 if true makeFile
		for (char c : result) {
			if (c == '1') {
			  output.writeBit(1);
	  } else {  // else we write bit 0
		output.writeBit(0);
	  }
	}
  }
  return result;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
	HuffmanNode* node = encodingTree;  // create a tree node to track noderent
	string result;  // holds the result string

	while (!input.eof()) {  // traverse while not end
		int bit = input.readBit();  // bit to tell us if we need to go left or rigth
		if (bit == 1) {  // if bit 1 then node will step to right
		  node = node->one;
		} else {  // if bit 0 then node will step to left
		  node = node->zero; 
		}
		if (node->character != NOT_A_CHAR) {  // if not N_A_C then its a char
		  if (node->character == PSEUDO_EOF) {
			  break;
		  }
		  result += char(node->character);  // store char
		  output << char(node->character);
		  node = encodingTree;
		} 
	}

	return result;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
	hashmap frequencyMap;
	buildFrequencyMap(filename, true, frequencyMap);
	HuffmanNode* nodeTree = buildEncodingTree(frequencyMap);
	mymap<int, string> encoded = buildEncodingMap(nodeTree);
	int size = 0;
	ifstream inFS(filename);
	ofbitstream outFS(filename + ".huf");
	outFS << frequencyMap;
	string result = encode(inFS, encoded, outFS, size, true);
	inFS.close();
	outFS.close();
	freeTree(nodeTree);
	return result;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
	int pos = filename.find(".huf");
	if ((int)pos >= 0) {
		filename = filename.substr(0, pos);
	}
	pos = filename.find(".");
	string ext = filename.substr(pos, filename.length() - pos);
	filename = filename.substr(0, pos);
	ifbitstream input(filename + ext + ".huf");
	ofstream output(filename + "_unc" + ext);

	hashmap dump;
	input >> dump;  // get rid of frequency map at top of file
	HuffmanNode* encodingTree = buildEncodingTree(dump);
	string decodeStr  = decode(input, encodingTree, output);
	freeTree(encodingTree);
	return decodeStr;
}
