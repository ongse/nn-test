#include "def.h"

U8 Bin2BCD(U32 bin, U8 * ptr);					//bin=345  return=3, ptr[0]~[3]=5,4,3
U32 BCD2Bin(U8 * ptr, U8 length);				//ptr[0]~[2]=3,6,4 length=3  return 463
void ASCII2BCD(U8 *ptr, U8 length);				//ptr[0]~[2]='3','4','5'  return ptr[0]~[2]=3,4,5
void BCD2ASCII(U8 *ptr, U8 length);				//ptr[0]~[2]=3,4,5  return ptr[0]~[2]='3','4','5'
U8 BCDAdder_Bin2BCD(U8* ptr, const U8* ptr2, U8 length);
U8 BCDAdder(U8* ptr, U8* ptr2, U8 length);
void flip(U8 * ptr, U8 length);					//ptr[0]~[2]=3,6,4  return ptr[0]~[2]=4, 6, 3
U32 checksum(U8 * p, U8 l);
//U8 FloatString2Bin_fixDigit(U8* ptr, U8 decimalPlace, U32* result);	//input format must be "0.1122547" or "123.4567880"void String2Bin(U8* ptr, U32* result);
void String2Bin(U8* ptr, U32* result, U8 digit);		//abc.defgh(ASCII) 3-digit ==> abcdef(bin)
void Bin2FloatString(U32 value, U8 digit, U8* ptr);	//abcdef(bin) 5-digit ==> a.bcdef(ASCII)
U32 AsciiDec2Bin(U8* ptr);						//ptr[0]~[2]=3,4,5  return 345
void Bin2AsciiDec(U32 no, U8* ptr);				//no=345  ptr[0]~[2]=3,4,5
U32 minutesDec2DegreeDec(U32 mins, U8 digit);	//convert minutes decimal to degree decimal, eg. 5°21.798,100°17.584 = 5.36330,100.29307
void U8toASCIIHEX(U8 value, U8 *result);		//e.g value=173   result[0]='A'   result[1]='D'
void U32toASCIIHEX(U32 value, U8 *result);		//e.g value=3613923842   result[0]~[7] = 'D','7','6','8','1','A','0','2'
U8 ASCIIHEX2BinByte(U8 *string);					//e.g string[0]='A',string[1]='D',return=173
void U8toAsciiBin(U8 value, U8 *result);		//e.g. value=5e  result[0] ~ result[7] = 0101_0111
void xor128_seed(void);							//call once before use xor128
U32 xor128(void);								//random number generator using xorshift method
