#include "main.hpp"
#include "tables.hpp"


#include <iostream>
using namespace std;


// Сумма по модулю 2**64 матриц
void add_rkey(unsigned int* dtn, unsigned int* src, int c) {
	unsigned int* dtn_res = new unsigned int[row];	
	unsigned int* src_res = new unsigned int[row];	

	for(int i = 0; i < c; i++) {
		for(int x = 0; x < row; x++) {
			dtn_res[x] = dtn[x + i * row];
			src_res[x] = src[x + i * row];
		}

		unsigned int val = 0;
		for(int x = 0; x < row; x++) {
			unsigned int gap = (dtn_res[x] + src_res[x] + val) >> 8;
			dtn_res[x] = (((dtn_res[x] + src_res[x] + val) >> 8) << 8) ^ (dtn_res[x] + src_res[x] + val);
			val = gap;
		}

		for(int x = 0; x < row; x++) { dtn[x + i * row] = dtn_res[x]; }
	}

	delete[] dtn_res;
	delete[] src_res;
}

// Побитовый ксор матриц
void xor_rkey(unsigned int* dtn, unsigned int* src, int c) {
	for(int i = 0; i < c * row; i++) { dtn[i] = dtn[i] ^ src[i]; }
}

// Побитовая замена матрицы
void s_block(unsigned int* state, int c) {
	for(int i = 0; i < c * row; i++) {
		if((i % 4) == 0) { state[i] = s0_table[state[i]]; }
		if((i % 4) == 1) { state[i] = s1_table[state[i]]; }
		if((i % 4) == 2) { state[i] = s2_table[state[i]]; }
		if((i % 4) == 3) { state[i] = s3_table[state[i]]; }
	}
}

// Сдвиг строк в матрице
void s_row(unsigned int* state, int c) {
	unsigned int* pattern = new unsigned int[c];
	int shift_val;

	for(int x = 0; x < row; x++) {
		for(int i = 0; i < c; i++) { pattern[i] = state[x + i * row]; }

		shift_val = ((c * 64 * x) - (c * 64 * x % 512)) / 512;
		cycle_shift_right(pattern, c, shift_val);
		for(int i = 0; i < c; i++) { state[x + i * row] = pattern[i]; }		
	}

	delete[] pattern;
}

// Циклический сдвиг последовательности в лево (сдвиг кратен 8)
void cycle_shift_left(unsigned int* state, int len, int shift) {
	cycle_shift_right(state, len, len - (shift % len));
}

// Циклический сдвиг последовательности в право (сдвиг кратен 8)
void cycle_shift_right(unsigned int* state, int len, int shift) {
	unsigned int* pattern = new unsigned int[len];
	
	for(int i = 0; i < len; i++) { pattern[(int)((i + shift) % len)] = state[i]; }
	for(int i = 0; i < len; i++) { state[i] = pattern[i]; }

	delete[] pattern;
}

// Сколярное произведение двух векторов в поле GF(256)
unsigned int vector_mul(unsigned int* row_state, unsigned int* column_state) {
	unsigned int res = mul_table[row_state[0]][column_state[0]];

	for(int i = 1; i < row; i++) { res ^= mul_table[row_state[i]][column_state[i]]; }

	return res % polynomial;
}

// Умножение двух матриц в поле GF(256)
void m_col(unsigned int* state, int c) {
	unsigned int* pattern = new unsigned int[row];
	unsigned int* res = new unsigned int[row * c];

	for(int i = 0; i < c; i++) {
		for(int x = 0; x < row; x++) { pattern[x] = state[x + i * row]; }

		for(int x = 0; x < row; x++) { res[x + i * row] = vector_mul(matrix[x], pattern); }
	}

	for(int i = 0; i < row * c; i++) { state[i] = res[i]; }

	delete[] pattern;
	delete[] res;
}

// Подсчёт количества раундов
int obtain_t(int l, int k) {
	int t;

	if(l == 128 && k == 128) { t = 10; }
	if(l == 128 && k == 256) { t = 14; }
	if(l == 256 && k == 256) { t = 14; }
	if(l == 256 && k == 512) { t = 18; }
	if(l == 512 && k == 512) { t = 18; }

	return t;
}

// Подсчёт количества столбцов в входной матрице
int obtain_c(int l) {
	return (int)(l / 64);
}

// Обратная побитовая замена матрицы
void _s_block(unsigned int* state, int c) {
	for(int i = 0; i < c * row; i++) {
		if((i % 4) == 0) { state[i] = _s0_table[state[i]]; }
		if((i % 4) == 1) { state[i] = _s1_table[state[i]]; }
		if((i % 4) == 2) { state[i] = _s2_table[state[i]]; }
		if((i % 4) == 3) { state[i] = _s3_table[state[i]]; }
	}
}

// Обратный cдвиг строк в матрице
void _s_row(unsigned int* state, int c) {
	unsigned int* pattern = new unsigned int[c];
	int shift_val;

	for(int x = 0; x < row; x++) {
		for(int i = 0; i < c; i++) { pattern[i] = state[x + i * row]; }

		shift_val = ((c * 64 * x) - (c * 64 * x % 512)) / 512;
		cycle_shift_left(pattern, c, shift_val);
		for(int i = 0; i < c; i++) { state[x + i * row] = pattern[i]; }		
	}

	delete[] pattern;
}

// Обратное умножение двух матриц в поле GF(256)
void _m_col(unsigned int* state, int c) {
	unsigned int* pattern = new unsigned int[row];
	unsigned int* res = new unsigned int[row * c];

	for(int i = 0; i < c; i++) {
		for(int x = 0; x < row; x++) { pattern[x] = state[x + i * row]; }

		for(int x = 0; x < row; x++) { res[x + i * row] = vector_mul(_matrix[x], pattern); }
	}

	for(int i = 0; i < row * c; i++) { state[i] = res[i]; }

	delete[] pattern;
	delete[] res;
}

// Вычитание по модулю 2**64 матриц
void sub_rkey(unsigned int* dtn, unsigned int* src, int c) {
	int flag = 0;

	for(int i = 0; i < c; i++) {
		for(int x = 0; x < row; x++) {
			int state = flag;
			if(dtn[x + i * row] < src[x + i * row]) { flag = 1; }
			else { 
				if(dtn[x + i * row] == src[x + i * row] && state == 1) { flag = 1; }
				else { flag = 0; }
			}
			if(x != 0) { dtn[x + i * row] = (dtn[x + i * row] - src[x + i * row] - state) & 0xff; }
			else { dtn[x + i * row] = (dtn[x + i * row] - src[x + i * row]) & 0xff; }
		}
	}
}