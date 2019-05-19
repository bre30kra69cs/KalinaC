#include "main.hpp"
#include "support.hpp"


#include <iostream>
using namespace std;


// Дебаггинг дешифрования
unsigned int* db_decryption(unsigned int* input, unsigned int** keys, int t, int c) {
	unsigned int* output = new unsigned int[c * row];
	mascpy(output, input, c * row);

	for(int i = t; i > 0; i--) {
		cout << "raund[" << i << "].rkey:" << "\n";
		masprint(keys[i], c * row);

		if(i == t) { 
			sub_rkey(output, keys[i], c); 
			cout << "raund[" << i << "].sub_rkey:" << "\n";
			masprint(output, c * row);
		}
		else { 
			xor_rkey(output, keys[i], c); 
			cout << "raund[" << i << "].xor_rkey:" << "\n";
			masprint(output, c * row);
		}
		_m_col(output, c);
		cout << "raund[" << i << "]._m_col:" << "\n";
		masprint(output, c * row);

		_s_row(output, c);
		cout << "raund[" << i << "]._s_row:" << "\n";
		masprint(output, c * row);

		_s_block(output, c);
		cout << "raund[" << i << "]._s_block:" << "\n";
		masprint(output, c * row);
	}
	cout << "raund[" << 0 << "].rkey:" << "\n";
	masprint(keys[0], c * row);

	sub_rkey(output, keys[0], c);
	cout << "raund[" << 0 << "].sub_rkey:" << "\n";
	masprint(output, c * row);

	return output;
}