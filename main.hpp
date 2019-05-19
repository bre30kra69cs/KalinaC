const int row = 8;
const unsigned int polynomial = 0x11d;


// Raund generation
void cycle_shift_right(unsigned int*, int, int);

void cycle_shift_left(unsigned int*, int, int);

void s_row(unsigned int*, int);

void s_block(unsigned int*, int);

unsigned int vector_mul(unsigned int*, unsigned int*);

void m_col(unsigned int*, int);

int obtain_t(int, int);

int obtain_c(int);

void add_rkey(unsigned int*, unsigned int*, int);

void xor_rkey(unsigned int*, unsigned int*, int);

void _s_block(unsigned int*, int);

void _s_row(unsigned int*, int);

void _m_col(unsigned int*, int);

void sub_rkey(unsigned int*, unsigned int*, int);


// Key generation
unsigned int* create_intermediate_input(int, int, int);

unsigned int** split_key(unsigned int*, int, int, int);

void create_intermediate_key(unsigned int*, unsigned int**, int);

void tmv(unsigned int*, int, int);

unsigned int* gen_round_key(unsigned int*, unsigned int*, int, int);

unsigned int* id_left(unsigned int*, int, int);

unsigned int* id_right(unsigned int*, int, int);

unsigned int* id(unsigned int*, int, int);

unsigned int** key_gen(unsigned int*, int, int, int, int);

unsigned int** gen_key_input(int, int);


// Encryption Decryption
unsigned int* encryption(unsigned int*, unsigned int**, int, int);

unsigned int* decryption(unsigned int*, unsigned int**, int, int);


// Debugging Encryption Decryption
unsigned int* db_decryption(unsigned int*, unsigned int**, int, int);


// Debugging
unsigned int* db_create_intermediate_input(int, int, int);

unsigned int** db_split_key(unsigned int*, int, int, int);

void db_create_intermediate_key(unsigned int*, unsigned int**, int);

void db_tmv(unsigned int*, int, int);

unsigned int* db_gen_round_key(unsigned int*, unsigned int*, int, int);

unsigned int* db_id_left(unsigned int*, int, int, int);

unsigned int* db_id_right(unsigned int*, int, int, int);

unsigned int* db_id(unsigned int*, int, int);

void db_key_gen(unsigned int**, unsigned int*, int, int, int, int);