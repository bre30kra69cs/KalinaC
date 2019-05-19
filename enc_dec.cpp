#include "main.hpp"
#include "support.hpp"


#include <iostream>
using namespace std;


// Шифрование
unsigned int* encryption(unsigned int* input, unsigned int** keys, int t, int c) {
	unsigned int* output = new unsigned int[c * row];
	mascpy(output, input, c * row);

	add_rkey(output, keys[0], c);
	for(int i = 1; i < t + 1; i++) {
		s_block(output, c);
		s_row(output, c);
		m_col(output, c);
		if(i != t) { xor_rkey(output, keys[i], c); }
		else { add_rkey(output, keys[i], c); }
	}

	return output;
}

// Дешифрование
unsigned int* decryption(unsigned int* input, unsigned int** keys, int t, int c) {
	unsigned int* output = new unsigned int[c * row];
	mascpy(output, input, c * row);

	for(int i = t; i > 0; i--) {
		if(i == t) { sub_rkey(output, keys[i], c); }
		else { xor_rkey(output, keys[i], c); }
		_m_col(output, c);
		_s_row(output, c);
		_s_block(output, c);
	}
	sub_rkey(output, keys[0], c);
	
	return output;
}