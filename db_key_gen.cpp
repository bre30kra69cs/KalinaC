#include "main.hpp"
#include "support.hpp"


#include <iostream>
using namespace std;


// Создание промежуточного входа
unsigned int* db_create_intermediate_input(int l, int k, int c) {
	unsigned int* intermediate_input = new unsigned int[c * row];	

	for(int i = 0; i < c * row; i++) { intermediate_input[i] = 0; }
	intermediate_input[0] = (int)((l + k + 64) / 64);

	return intermediate_input;
}

// Разедление ключа на два
unsigned int** db_split_key(unsigned int* key, int l, int k, int c) {
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
void db_tmv(unsigned int* input, int c, int iterator) {
	for(int i = 0; i < c * row; i = i + 2) { 
		input[i + 1] = (input[i] << (int)(iterator / 2)) >> 8;
		input[i] = (input[i] << (int)(iterator / 2)) & 0x000000FF;
	}
}

// Создание промежуточного ключа
void db_create_intermediate_key(unsigned int* intermediate_input, unsigned int** key_mas, int c) {
	for(int i = 0; i < 3; i++) {
		if(i != 1) { 
			add_rkey(intermediate_input, key_mas[0], c);
			cout << "\n" << "state[0].add_rkey:" << "\n";
			masprint(intermediate_input, c * row);
		}
		else { 
			xor_rkey(intermediate_input, key_mas[1], c); 
			cout << "\n" << "state[0].xor_rkey:" << "\n";
			masprint(intermediate_input, c * row);			
		}
		s_block(intermediate_input, c);
		cout << "\n" << "state[0].s_box:" << "\n";
		masprint(intermediate_input, c * row);

		s_row(intermediate_input, c);
		cout << "\n" << "state[0].s_row:" << "\n";
		masprint(intermediate_input, c * row);

		m_col(intermediate_input, c);
		cout << "\n" << "state[0].m_col:" << "\n";
		masprint(intermediate_input, c * row);
	}
}

// Создание раундового ключа
unsigned int* db_gen_round_key(unsigned int* input, unsigned int* intermediate_key, int c, int iterator) {
	unsigned int* state = new unsigned int[row * c];
	for(int i = 0; i < c * row; i++) { state[i] = input[i]; }

	unsigned int* v_const = new unsigned int[c * row];
	for(int i = 0; i < c * row; i = i + 2) { 
		v_const[i] = 0x01;
		v_const[i + 1] = 0x00;
	}
	db_tmv(v_const, c, iterator);
	cout << "\n" << "state[" << iterator << "].tmv:" << "\n";
	masprint(v_const, c * row);

	//cout << "\n" << "state[" << iterator << "].id:" << "\n";
	//masprint(input, c * row);

	add_rkey(v_const, intermediate_key, c);
	cout << "\n" << "state[" << iterator << "].add_rkey (tmv):" << "\n";
	masprint(v_const, c * row);	

	for(int i = 0; i < 2; i++) {
		if(i == 0) { 
			add_rkey(state, v_const, c);
			cout << "\n" << "state[" << iterator << "].add_rkey (kt_raund):" << "\n";
			masprint(state, c * row);	 
		}
		else { 
			xor_rkey(state, v_const, c); 
			cout << "\n" << "state[" << iterator << "].xor_rkey (kt_raund):" << "\n";
			masprint(state, c * row);	
		}			
		s_block(state, c);
		cout << "\n" << "state[" << iterator << "].s_box:" << "\n";
		masprint(state, c * row);	

		s_row(state, c);
		cout << "\n" << "state[" << iterator << "].s_row:" << "\n";
		masprint(state, c * row);	

		m_col(state, c);
		cout << "\n" << "state[" << iterator << "].m_col:" << "\n";
		masprint(state, c * row);	
	}
	add_rkey(state, v_const, c);
	cout << "\n" << "state[" << iterator << "].add_rkey (kt_raund):" << "\n";
	masprint(state, c * row);	

	return state;
}

// Смещение ключа и взятие высших битов
unsigned int* db_id_right(unsigned int* key, int c, int shift, int iterator) {
	unsigned int* state = new unsigned int[c * 2 * row];
	unsigned int* result = new unsigned int[c * row];

	/*
	for(int i = 0; i < 2 * c * row; i++) { state[i] = key[i]; }
	for(int i = 0; i < row; i ++) {
		unsigned int* mid_state = new unsigned int[2 * c];		
		for(int x = 0; x < 2 * c; x++) { mid_state[x] = state[i + x * row]; }
		cycle_shift_right(mid_state, 2 * c, shift);
		for(int x = 0; x < 2 * c; x++) { state[i + x * row] = mid_state[x]; }
	}
	for(int i = c * row; i < 2 * c * row; i++) { result[(int)(i - c * row)] = state[i]; }
	*/
	for(int i = 0; i < 2 * c * row; i++) { state[i] = key[i]; }
	cycle_shift_left(state, 2 * c * row, shift);
	for(int i = c * row; i < 2 * c * row; i++) { result[(int)(i % (c * row))] = state[i]; }
	cout << "\n" << "state[" << iterator << "].id:" << "\n";
	masprint(state, 2 * c * row);

	delete[] state;
	return result;
}

// Смещение ключа и взятие низших битов
unsigned int* db_id_left(unsigned int* key, int c, int shift, int iterator) {
	unsigned int* state = new unsigned int[c * 2 * row];
	unsigned int* result = new unsigned int[c * row];

	/*
	for(int i = 0; i < 2 * c * row; i++) { state[i] = key[i]; }
	for(int i = 0; i < row; i++) {
		unsigned int* mid_state = new unsigned int[2 * c];		
		for(int x = 0; x < 2 * c; x++) { mid_state[x] = state[i + x * row]; }
		cycle_shift_right(mid_state, 2 * c, shift);
		for(int x = 0; x < 2 * c; x++) { state[i + x * row] = mid_state[x]; }
	}
	for(int i = 0; i < c * row; i++) { result[i] = state[i]; }
	*/
	for(int i = 0; i < 2 * c * row; i++) { state[i] = key[i]; }
	cycle_shift_left(state, 2 * c * row, shift);
	cout << "\n" << "state[" << iterator << "].id:" << "\n";
	masprint(state, 2 * c * row);

	for(int i = 0; i < c * row; i++) { result[i] = state[i]; }

	delete[] state;
	return result;
}

// Генерация подключей
void db_key_gen(unsigned int** keys, unsigned int* key, int l, int k, int t, int c) {
	cout << "\n" << "KEY:" << "\n";
	masprint(key, 2 * c * row);

	unsigned int* intermediate_input = create_intermediate_input(l, k, c);
	cout << "\n" << "intermediate key (KT) generation" << "\n";
	cout << "\n" << "state[0]:" << "\n";
	masprint(intermediate_input, c * row);	

	unsigned int** key_mas = db_split_key(key, l, k, c);
	cout << "\n" << "state[0].k0:" << "\n";
	masprint(key_mas[0], c * row);
	cout << "\n" << "state[0].k1:" << "\n";
	masprint(key_mas[1], c * row);

	db_create_intermediate_key(intermediate_input, key_mas, c);
	cout << "\n" << "KT" << "\n";
	masprint(intermediate_input, c * row);

	cout << "\n\n\n" << "Raund keys for Kalyna-" << l << "/" << k <<" (even indexes)" << "\n";

	cout << "\n" << "state[0]:" << "\n";
	masprint(intermediate_input, c * row);

	cout << "\n" << "state[0].KT:" << "\n";
	masprint(intermediate_input, c * row);

	for(int i = 0; i < t + 1; i++) {
		if((int)(i % 2) == 0) { 
			if((int)(k / l) > 1) {
				if((int)(i % 4) == 0) {
					cout << "3" << "\n";
					int shift = i * 16 / 8;
					unsigned int* round_state = db_id_left(key, c, shift, i);
					keys[i] = db_gen_round_key(round_state, intermediate_input, c, i); 
					delete[] round_state;
				}
				else {
					cout << "4" << "\n";
					int shift = (int)(i / 4) * 64 / 8;
					unsigned int* round_state = db_id_right(key, c, shift, i);
					keys[i] = db_gen_round_key(round_state, intermediate_input, c, i); 
					delete[] round_state;
				}
			}
			else {
				cout << "1" << "\n";
				int shift = i * 32 / 8;
				//unsigned int* round_state = id(key, c, shift);
				unsigned int* round_state = new unsigned int[c * row];
				for(int ii = 0; ii < c * row; ii++) { round_state[ii] = key[ii]; }
				cycle_shift_left(round_state, c * row, shift);
				cout << "\n" << "state[" << i << "].id:" << "\n";
				masprint(round_state, c * row);
				keys[i] = db_gen_round_key(round_state, intermediate_input, c, i); 
				delete[] round_state;
			}
		}
		else {
			cout << "2" << "\n";
			unsigned int* round_state = new unsigned int[c * row];
			for(int h = 0; h < c * row; h++) { round_state[h] = keys[i - 1][h]; }
			cycle_shift_left(round_state, c * row, (int)(((int)(l / 4) + 24)) / 8);
			for(int h = 0; h < c * row; h++) { keys[i][h] = round_state[h]; }
			delete[] round_state;
		}
		int kop;
		cin >> kop;
	}

	for(int i = 0; i < t + 1; i++) {
		cout << "\n" << "raund[" << i << "].rkey:" << "\n";
		masprint(keys[i], c * row);
	}
}

// Смещение ключа 
unsigned int* db_id(unsigned int* key, int c, int shift) {
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