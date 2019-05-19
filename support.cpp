#include "support.hpp"

#include <iostream>
using namespace std;


const int line_len = 16;


void masprint(unsigned int* state, int len) {

	for(int x = 0; x < (int)(len / line_len); x++) {
		for(int i = 0; i < line_len; i++) { printf("%x ", state[i + x * line_len]); }
		printf("\n");
	}
}

void mascpy(unsigned int* dst, unsigned int* src, int len) {
	for(int i = 0; i < len; i++) { dst[i] = src[i]; }
}

void pause() {
	int i = 0;
	cin >> i;
}

void mascmp(unsigned int* dst, unsigned int* src, int len) {
	bool flag = true;
	for(int i = 0; i < len; i++) {
		if(dst[i] != src[i]) { flag = false; } 
	}
	if(flag) { cout << "CMP TRUE" << "\n"; }
	else { cout << "CMP FALSE" << "\n"; }
}