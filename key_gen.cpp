#include "main.hpp"


// Создание промежуточного входа
unsigned int* create_intermediate_input(int l, int k, int c) {
	unsigned int* intermediate_input = new unsigned int[c * row];	

	for(int i = 0; i < c * row; i++) { intermediate_input[i] = 0; }
	intermediate_input[0] = (int)((l + k + 64) / 64);

	return intermediate_input;
}

// Разедление ключа на два
unsigned int** split_key(unsigned int* key, int l, int k, int c) {
	unsigned int** state = new unsigned int*[2];

	for(int i = 0; i < 2; i++) { state[i] = new unsigned int[c * row]; }
	if(l == k) {
		for(int i = 0; i < c * row; i++) {
			state[0][i] = key[i];
			state[1][i] = key[i];
		}
	}
	else {
		for(int i = 0; i < c * row; i++) { state[0][i] = key[i]; }
		for(int i = c * row; i < 2 * c * row; i++) { state[1][(int)(i % (c * row))] = key[i]; }
	}

	return state;
}

// Смещение ключевой пстоянной
void tmv(unsigned int* input, int c, int iterator) {
	for(int i = 0; i < c * row; i = i + 2) { 
		input[i + 1] = (input[i] << (int)(iterator / 2)) >> 8;
		input[i] = (input[i] << (int)(iterator / 2)) & 0x000000FF;
	}
}

// Создание промежуточного ключа
void create_intermediate_key(unsigned int* intermediate_input, unsigned int** key_mas, int c) {
	for(int i = 0; i < 3; i++) {
		if(i != 1) { add_rkey(intermediate_input, key_mas[0], c); }
		else { xor_rkey(intermediate_input, key_mas[1], c); }
		s_block(intermediate_input, c);
		s_row(intermediate_input, c);
		m_col(intermediate_input, c);
	}
}

// Создание раундового ключа
unsigned int* gen_round_key(unsigned int* input, unsigned int* intermediate_key, int c, int iterator) {
	unsigned int* state = new unsigned int[row * c];
	for(int i = 0; i < c * row; i++) { state[i] = input[i]; }

	unsigned int* v_const = new unsigned int[c * row];
	for(int i = 0; i < c * row; i = i + 2) { 
		v_const[i] = 0x01;
		v_const[i + 1] = 0x00;
	}
	tmv(v_const, c, iterator);
	add_rkey(v_const, intermediate_key, c);

	for(int i = 0; i < 2; i++) {
		if(i == 0) { add_rkey(state, v_const, c); }
		else { xor_rkey(state, v_const, c); }			
		s_block(state, c);
		s_row(state, c);
		m_col(state, c);
	}
	add_rkey(state, v_const, c);

	return state;
}

// Смещение ключа и взятие высших битов
unsigned int* id_right(unsigned int* key, int c, int shift, int iterator) {
	unsigned int* state = new unsigned int[c * 2 * row];
	unsigned int* result = new unsigned int[c * row];

	for(int i = 0; i < 2 * c * row; i++) { state[i] = key[i]; }
	cycle_shift_left(state, 2 * c * row, shift);
	for(int i = c * row; i < 2 * c * row; i++) { result[(int)(i % (c * row))] = state[i]; }

	delete[] state;
	return result;
}

// Смещение ключа и взятие низших битов
unsigned int* id_left(unsigned int* key, int c, int shift, int iterator) {
	unsigned int* state = new unsigned int[c * 2 * row];
	unsigned int* result = new unsigned int[c * row];

	for(int i = 0; i < 2 * c * row; i++) { state[i] = key[i]; }
	cycle_shift_left(state, 2 * c * row, shift);
	for(int i = 0; i < c * row; i++) { result[i] = state[i]; }

	delete[] state;
	return result;
}

// Генерация подключей
unsigned int** key_gen(unsigned int* key, int l, int k, int t, int c) {
	unsigned int* intermediate_input = create_intermediate_input(l, k, c);
	unsigned int** keys = gen_key_input(t, c);

	unsigned int** key_mas = split_key(key, l, k, c);
	create_intermediate_key(intermediate_input, key_mas, c);

	for(int i = 0; i < t + 1; i++) {
		if((int)(i % 2) == 0) { 
			if((int)(k / l) > 1) {
				if((int)(i % 4) == 0) {
					int shift = i * 16 / 8;
					unsigned int* round_state = id_left(key, c, shift, i);
					keys[i] = gen_round_key(round_state, intermediate_input, c, i); 
					delete[] round_state;
				}
				else {
					int shift = (int)(i / 4) * 64 / 8;
					unsigned int* round_state = id_right(key, c, shift, i);
					keys[i] = gen_round_key(round_state, intermediate_input, c, i); 
					delete[] round_state;
				}
			}
			else {
				int shift = i * 32 / 8;
				unsigned int* round_state = new unsigned int[c * row];
				for(int ii = 0; ii < c * row; ii++) { round_state[ii] = key[ii]; }
				cycle_shift_left(round_state, c * row, shift);
				keys[i] = gen_round_key(round_state, intermediate_input, c, i); 
				delete[] round_state;
			}
		}
		else {
			unsigned int* round_state = new unsigned int[c * row];
			for(int h = 0; h < c * row; h++) { round_state[h] = keys[i - 1][h]; }
			cycle_shift_left(round_state, c * row, (int)(((int)(l / 4) + 24)) / 8);
			for(int h = 0; h < c * row; h++) { keys[i][h] = round_state[h]; }
			delete[] round_state;
		}
	}

	return keys;
}

// Смещение ключа 
unsigned int* id(unsigned int* key, int c, int shift) {
	unsigned int* result = new unsigned int[c * row];

	for(int i = 0; i < c * row; i++) { result[i] = key[i]; }
	for(int i = 0; i < row; i ++) {
		unsigned int* mid_state = new unsigned int[c];		
		for(int x = 0; x < c; x++) { mid_state[x] = result[i + x * row]; }
		cycle_shift_right(mid_state, c, shift);
		for(int x = 0; x < c; x++) { result[i + x * row] = mid_state[x]; }
	}

	return result;
}

unsigned int** gen_key_input(int t, int c) {
	unsigned int** keys = new unsigned int*[t + 1];

	for(int i = 0; i < t; i++) { keys[i] = new unsigned int[c * row]; }

	return keys;
}