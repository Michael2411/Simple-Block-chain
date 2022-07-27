// 1701121.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string.h>
#ifdef __GNUC__
# define __rdtsc __builtin_ia32_rdtsc
#else
# include<intrin.h>
#endif
using namespace std;

typedef unsigned long long u64;


u64 hexToDec(const char* data) {
	u64 ret = 0;
	for (;; ++data) //read left-to-right
	{
		unsigned char dec = *data - '0';
		if (dec < 10)
			ret = ret << 4 | dec;
		else {
			unsigned char upper = (*data & 0xDF) - 'A';
			if (upper > 5)
				break;
			ret = ret << 4 | upper + (u64)10;
		}
	}
	return ret;
}

u64 roundkey[16];

int sbox[]{
			14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
			0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
			4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
			15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,

			15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
			3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
			0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
			13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,

			10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
			13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
			13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
			1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,

			7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
			13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
			10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
			3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,

			2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
			14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
			4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
			11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,

			12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
			10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
			9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
			4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,

			4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
			13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
			1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
			6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,

			13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
			1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
			7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
			2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
};


////shifting left
u64 shiftLeft(u64 half, int shifts) {
	u64 shiftedKey=0;
	if (shifts == 1) {
		shiftedKey = ((half << shifts) & ~(1 << 28) | (half >> (28 - shifts)));
	}
	else {
		shiftedKey = ((half << shifts) & ~(11 << 28) | (half >> (28 - shifts)));
	}
	return shiftedKey;
}
//

//permuation choice one 
u64 pc1(u64 key) {
	u64 permutatedkey = 0;
	int pc1table[56] = {
						57,49,41,33,25,17,9,
						1,58,50,42,34,26,18,
						10,2,59,51,43,35,27,
						19,11,3,60,52,44,36,
						63,55,47,39,31,23,15,
						7,62,54,46,38,30,22,
						14,6,61,53,45,37,29,
						21,13,5,28,20,12,4
	};
	for (int i = 0; i < 56; i++) {
		permutatedkey |= (key >> (64 - pc1table[56 - 1 - i]) & 1) << i; //in=input
	}
	return permutatedkey;
}

//permuatation choice two
u64 pc2(u64 key) {
	u64 permutatedkey = 0;
	int pc2table[48] = { 14, 17, 11, 24, 1, 5,
						 3, 28, 15, 6, 21, 10,
						 23, 19, 12, 4, 26, 8,
						 16, 7, 27, 20, 13, 2,
						 41, 52, 31, 37, 47, 55,
						 30, 40, 51, 45, 33, 48,
						 44, 49, 39, 56, 34, 53,
						 46, 42, 50, 36, 29, 32
	};
	for (int i = 0; i < 48; i++) {
		permutatedkey |= (key >> (56 - pc2table[48 - 1 - i]) & 1) << i; //in=input
	}
	return permutatedkey;

}

////Direct Permutations
u64 permute(u64 x) {
	u64 y = 0;
	int P_table[32] = { 16, 7, 20, 21,
					29, 12, 28, 17,
					1, 15, 23, 26,
					5, 18, 31, 10,
					2, 8, 24, 14,
					32, 27, 3, 9,
					19, 13, 30, 6,
					22, 11, 4, 25 };
	for (int i = 0; i < 32; i++) {
		y |= (x >> (32 - P_table[32 - 1 - i]) & 1) << i; //in=input
	}
	return y;
}
////initial permutation

u64 initital_per(u64 text) {
	u64 per_text = 0;
	int ip_table[64] = { 58, 50, 42, 34, 26, 18, 10, 2,
							 60, 52, 44, 36, 28, 20, 12, 4,
							 62, 54, 46, 38, 30, 22, 14, 6,
							 64, 56, 48, 40, 32, 24, 16, 8,
							 57, 49, 41, 33, 25, 17, 9, 1,
							 59, 51, 43, 35, 27, 19, 11, 3,
							 61, 53, 45, 37, 29, 21, 13, 5,
							 63, 55, 47, 39, 31, 23, 15, 7 };

	for (int i = 0; i < 64; i++) {
		per_text |= (text >> (64 - ip_table[64 - 1 - i]) & 1) << i; //in=input
	}
	return per_text;
}

//expansion permutations
u64 expand(u64 right) {
	u64 exp_right = 0;
	int expansionTable[48] = { 32, 1, 2, 3, 4, 5, 4, 5,
							 6, 7, 8, 9, 8, 9, 10, 11,
							 12, 13, 12, 13, 14, 15, 16, 17,
							 16, 17, 18, 19, 20, 21, 20, 21,
							 22, 23, 24, 25, 24, 25, 26, 27,
							 28, 29, 28, 29, 30, 31, 32, 1 };
	for (int i = 0; i < 48; i++) {
		exp_right |= (right >> (32 - expansionTable[48 - 1 - i]) & 1) << i; //in=input
	}
	return exp_right;
}

//final permutations
u64 final(u64 last) {
	u64 last_mod = 0;
	int final_permute[64] = { 40, 8, 48, 16, 56, 24, 64, 32,
						  39, 7, 47, 15, 55, 23, 63, 31,
						  38, 6, 46, 14, 54, 22, 62, 30,
						  37, 5, 45, 13, 53, 21, 61, 29,
						  36, 4, 44, 12, 52, 20, 60, 28,
						  35, 3, 43, 11, 51, 19, 59, 27,
						  34, 2, 42, 10, 50, 18, 58, 26,
						  33, 1, 41, 9, 49, 17, 57, 25 };

	for (int i = 0; i < 64; i++) {
		last_mod |= (last >> (64 - final_permute[64 - 1 - i]) & 1) << i; //in=input
	}
	return last_mod;
}


///// Function to make the key rounds 
void rounds(u64 key) {
	int shifting_table[16] = { 1, 1, 2, 2,
							2, 2, 2, 2,
							1, 2, 2, 2,
							2, 2, 2, 1 };

	u64 left = (key >> (u64)28 & (0xFFFFFFF));
	u64 right = (key & (0xFFFFFFF));

	for (int i = 0; i < 16; i++)
	{
		left = shiftLeft(left, shifting_table[i]);
		right = shiftLeft(right, shifting_table[i]);
		roundkey[i] = pc2(((left<< (u64)(28)) | (right)));
	}
}

void encrypt(string type, u64 text, u64 key) {

	key = pc1(key);


	rounds(key);

	u64 reversed_rounds[16];
	//Function to reverse key rounds in decryption
	if (type == "decrypt") {
		for (int i = 0; i < 16; i++)
		{
			reversed_rounds[15 - i] = roundkey[i];
		}
		for (int i = 0; i < 16; i++)
		{
			roundkey[i] = reversed_rounds[i];

		}
	}
	long long t1 = __rdtsc();
	text = initital_per(text);

	u64 left = (text >> (u64)(32)) & (0xFFFFFFFF);
	u64 right = (text & (0xFFFFFFFF));
	for (int i = 0; i < 16; i++)
	{
		u64 right2 = expand(right);
		u64 xored_right = (right2 ^ roundkey[i]);
		u64 output = 0;

		//sbox
		int* box;
		int idx = 0;
		output = 0;
		for (int i = 0; i < 8; i++)
		{
			box = sbox + i * 64;
			idx = xored_right >> (7 - i) * 6 & 0x3F;
			idx = idx >> 1 & 15 | (idx & 1) << 4 | idx & 0x20;
			output |= ((u64)box[idx]) << (7 - i) * 4;
		}
		output = permute(output);
		xored_right = (output ^ left);
		left = xored_right;
		if (i != 15) {
			swap(left, right);
		}
	}

	u64 encyrptedmessage = final(((left << (u64)32) | (right)));
	long long t2 = __rdtsc();
	if (type == "encrypt")
	{
		printf("cipher: %016llX\n", encyrptedmessage);
		printf("Cycles: %lld\n", t2 - t1);

	}
	else {
		printf("plain: %016llX\n", encyrptedmessage);
		printf("Cycles: %lld\n", t2 - t1);

	}

}

int main(int argc, char** argv)
{

	string text, key, type;

	type = argv[1];
	text = argv[2];
	key = argv[3];

	u64 text1 = hexToDec(text.c_str());
	u64 key1 = hexToDec(key.c_str());

	encrypt(type, text1, key1);
	return 0;
}


