#include "main.hpp"
#include "test.hpp"
#include "support.hpp"


#include <iostream>
using namespace std;


int main() {
	unsigned int* pl_res[5] = { encryption_res_128_128, encryption_res_128_256, encryption_res_256_256,
							 encryption_res_256_512, encryption_res_512_512 };
	unsigned int* pl_text[5] = { input_128_128, input_128_256, input_256_256,
						 	  input_256_512, input_512_512 };\
	unsigned int* pl_key[5] = { key_128_128, key_128_256, key_256_256,
					 	  	 key_256_512, key_512_512 };
	int l_mas[5] = {128, 128, 256, 256, 512};
	int k_mas[5] = {128, 256, 256, 512, 512};

	for(int iter = 0; iter < 5; iter++) {
		unsigned int* close_key = pl_key[iter];
		unsigned int* close_text = pl_text[iter];
		unsigned int* close_res = pl_res[iter];
		int l = l_mas[iter];
		int k = k_mas[iter];
		int t = obtain_t(l, k);
		int c = obtain_c(l);	

		
		cout << "\n";
		cout << l << " " << k << "\n";
		cout << "KEY:" << "\n";
		masprint(close_key, k / 8);
		cout << "PLAIN TEXT:" << "\n";
		masprint(close_text, c * row);

		unsigned int** keys = key_gen(close_key, l, k, t, c);	
		unsigned int* text = encryption(close_text, keys, t, c);

		cout << "RES TEXT:" << "\n";
		masprint(close_res, c * row);	
		cout << "CLOSE TEXT:" << "\n";
		masprint(text, c * row);	
		
		mascmp(close_res, text, c * row);

		delete[] keys;
		delete[] text;
	}


	unsigned int* res[5] = { decryption_res_128_128, decryption_res_128_256, decryption_res_256_256,
							 decryption_res_256_512, decryption_res_512_512 };
	unsigned int* text[5] = { close_text_128_128, close_text_128_256, close_text_256_256,
						 	  close_text_256_512, close_text_512_512 };\
	unsigned int* key[5] = { close_key_128_128, close_key_128_256, close_key_256_256,
					 	  	 close_key_256_512, close_key_512_512 };

	for(int iter = 0; iter < 5; iter++) {
		unsigned int* close_key = key[iter];
		unsigned int* close_text = text[iter];
		unsigned int* close_res = res[iter];
		int l = l_mas[iter];
		int k = k_mas[iter];
		int t = obtain_t(l, k);
		int c = obtain_c(l);	

		
		cout << "\n";
		cout << l << " " << k << "\n";
		cout << "KEY:" << "\n";
		masprint(close_key, k / 8);
		cout << "CLOSE TEXT:" << "\n";
		masprint(close_text, c * row);

		unsigned int** keys = key_gen(close_key, l, k, t, c);	
		unsigned int* text = decryption(close_text, keys, t, c);

		cout << "RES TEXT:" << "\n";
		masprint(close_res, c * row);	
		cout << "PLAIN TEXT:" << "\n";
		masprint(text, c * row);	
		
		mascmp(close_res, text, c * row);

		delete[] keys;
		delete[] text;	
	}

	pause();
	return 0;
}

