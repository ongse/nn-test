#include "BinBCD.h"
#include "string_ffb.h"

#define ASCII_0	48
#define ASCII_1	49

const U8 Bin2BCD_data[] = {	1,1,1,2,1,4,1,8,2,6,1,2,2,3,2,4,6,3,8,2,1,3,6,5,2,
								3,2,1,5,4,4,2,0,1,4,8,4,0,2,4,6,9,0,4,4,2,9,1,8,5,
								4,8,3,6,1,5,8,6,7,2,3,5,6,3,5,5,6,6,2,7,0,1,3,1,6,
								4,4,1,2,6,2,6,8,8,2,4,2,5,7,6,7,5,8,4,0,1,7,2,5,1,
								7,9,0,2,7,4,0,3,4,9,1,4,7,8,0,6,8,8,3,8,8,6,1,2,7,
								7,7,6,1,8,2,3,4,4,5,5,3,3,8,4,6,8,8,0,1,7,6,9,8,2,
								7,7,1,2,4,3,1,9,6,5,4,5,3,4,8,6,2,9,2,1,9,0,7,8,6,
								3,5,10,4,2,8,1,4,7,3,7,0,1,10,8,4,6,3,8,4,7,4,1,2 };

const U8 ASCIIBIN_data[16][4] = {	{ASCII_0, ASCII_0, ASCII_0, ASCII_0},
									{ASCII_0, ASCII_0, ASCII_0, ASCII_1},
									{ASCII_0, ASCII_0, ASCII_1, ASCII_0},	
									{ASCII_0, ASCII_0, ASCII_1, ASCII_1},
									{ASCII_0, ASCII_1, ASCII_0, ASCII_0},	
									{ASCII_0, ASCII_1, ASCII_0, ASCII_1},
									{ASCII_0, ASCII_1, ASCII_1, ASCII_0},
									{ASCII_0, ASCII_1, ASCII_1, ASCII_1},
									{ASCII_1, ASCII_0, ASCII_0, ASCII_0},
									{ASCII_1, ASCII_0, ASCII_0, ASCII_1},
									{ASCII_1, ASCII_0, ASCII_1, ASCII_0},
									{ASCII_1, ASCII_0, ASCII_1, ASCII_1},
									{ASCII_1, ASCII_1, ASCII_0, ASCII_0},
									{ASCII_1, ASCII_1, ASCII_0, ASCII_1},
									{ASCII_1, ASCII_1, ASCII_1, ASCII_0},
									{ASCII_1, ASCII_1, ASCII_1, ASCII_1}};

U32 xor128_seed_x, xor128_seed_y, xor128_seed_z, xor128_seed_w;  //for xor128 random seed

U8 Bin2BCD(U32 bin, U8 * temp)	//for 32bit only
{
    U8 i, j;
    U8 track = 0;

    for(i=0; i<11; i++) temp[i] = 0;

    if(bin == 0)
    {
		*temp = 0;
		return 1;
    }

    for(i=0; i<32; i++)
    {
		if((bin & 1<<i) > 0)
		{
		    j = BCDAdder_Bin2BCD(temp, &Bin2BCD_data[track+1], Bin2BCD_data[track]);
		    j = Bin2BCD_data[track] + j;
		}
    	track = track + Bin2BCD_data[track] + 1;
    }
    return j;
}

U32 BCD2Bin(U8* str, U8 length)
{
    U32 i= 0;
    U32 j = 1;

    if(length == 0) return 0;

    while(length > 0)
    {
	i = i+ (*str * j);
	str++;
	j *= 10;
	length--;
    }

    return i;
}

U8 BCDAdder_Bin2BCD(U8* ptr, const U8* ptr2, const U8 length)
{
    U8 i = 0;
    U8 j = 0;

    while(1)
    {
	j = 0;	
	ptr[i] = ptr[i] + ptr2[i];
	if(ptr[i] > 9)
	{
	    ptr[i] = ptr[i] - 10;
	    ptr[i+1]++;
	    j = 1;
	}
	i++;
	if(i >= length) return j;
    }
}

U8 BCDAdder(U8* ptr, U8* ptr2, U8 length)
{
    U8 i = 0;
    U8 j = 0;

    while(1)
    {
	j = 0;	
	ptr[i] = ptr[i] + ptr2[i];
	if(ptr[i] > 9)
	{
	    ptr[i] = ptr[i] - 10;
	    ptr[i+1]++;
	    j = 1;
	}
	i++;
	if(i >= length) return j;
    }
}

void ASCII2BCD(U8 *ptr, U8 length)
{
    do
    {
	*ptr = *ptr - 0x30;
	ptr++;
	length--;
    }while(length > 0);
}


void BCD2ASCII(U8 *ptr, U8 length)
{
    do
    {
	*ptr = *ptr + 0x30;
	ptr++;
	length--;
    }while(length > 0);
}

void flip(U8* ptr, U8 length)
{
    U8 i = 0;
    U8 j;

    if(length == 0)
	return;

    while(1)
    {
     	j = ptr[i];
	ptr[i] = ptr[length-1-i];
	ptr[length-1-i] = j;
        i++;
	if((length-1-i) <= i)
	    return;
    }
}

U32 checksum(U8* p, U8 l)
{
    U32 t = 0;
    while(l > 0)
    {
	t = t + *p;
	p++;
	l--;
    }
    return t;
}
/*
U8 FloatString2Bin_fixDigit(U8* ptr, U8 decimalPlace, U32* result)	//input format must be "0.1122547" or "123.4567880"
{
	U8 a, b;
	S8 d, t;
	U8 tempArray[10];

	t = 0;
	d = -1;
	b = countString(ptr);
	for(a=0; a < b; a++)
	{
		if((ptr[a] > 47)&&(ptr[a] < 58))
		{
			tempArray[t] = ptr[a];
			tempArray[t+1] = 0;
			t++;
		}
		else if(ptr[a] == '.')
			d = t;
	}

	if(t==0)							//no numerical no.
		return 0;
	else if(d == -1)						//no decimal point
	{
		for(a=0; a<decimalPlace; a++)
			tempArray[t++] = 0x30;
	}
	else if((t-d) > decimalPlace)	//more than 5 decimal place
	{
		tempArray[d+decimalPlace] = 0;
		t = d + 5;
	}
	else if((t-d) < decimalPlace)
	{
		for(a=t;a<(d+decimalPlace);a++)
		{
			tempArray[a] = 0x30;
			tempArray[a+1] = 0;
		}
		t = d + 5;
	}

	flip(tempArray, t);
	ASCII2BCD(tempArray, t);
	*result = BCD2Bin(tempArray, t);
	return 1;
}
*/

void String2Bin(U8* ptr, U32* result, U8 digit)
{
	U8* temp_ptr;
	U8 cnt;

	temp_ptr = ptr;
	if(digit > 0)
	{
		while(*temp_ptr != '.') temp_ptr++;
		*(temp_ptr+digit+1) = 0;
	}
	eliminateCharsString(ptr,".");
	cnt = countString(ptr);
	flip(ptr, cnt);
	ASCII2BCD(ptr, cnt);
	*result = BCD2Bin(ptr, cnt);
}

void Bin2FloatString(U32 value, U8 digit, U8* ptr)
{
	U8 cnt, i;

	cnt = Bin2BCD(value, ptr);
	BCD2ASCII(ptr, cnt);
	flip(ptr, cnt);

	if(cnt <= digit)
	{
		ptr[0] = '0';
		ptr[1] = 0;
		return;
	}

	for(i=cnt-1; i > (cnt - digit - 1); i--)
	{
		ptr[i+1] = ptr[i];
	}
	ptr[cnt - digit] = '.';
	ptr[cnt+1] = 0;	//place NULL at the back
}

U32 AsciiDec2Bin(U8* ptr)
{
	U8 cnt;
	cnt = countString(ptr);
	flip(ptr, cnt);
	ASCII2BCD(ptr, cnt);
	return BCD2Bin(ptr, cnt);
}

void Bin2AsciiDec(U32 no, U8* ptr)
{
	U8 a;
	a = Bin2BCD(no, ptr);
	BCD2ASCII(ptr, a);
	flip(ptr, a);
	ptr[a] = 0;
}

U32 minutesDec2DegreeDec(U32 mins, U8 digit)
{
	U8 a;
	U32 div = 1;
	U32	main, temp, result;

	for(a=0;a<digit;a++)
		div = div * 10;
	
	main = mins / div;
	main = main * div;
	temp = mins - main;
	temp = temp * 100;
	temp = temp /60;
	result = main + temp;
	return result;
}

void U8toASCIIHEX(U8 value, U8 *result)
{
	U8 temp, HighNibble, LowNibble;

	temp = (value >> 4) & 0x0F;	//high nibble (4-bit)
	if(temp <10)
	{	HighNibble = temp | 0x30; }
	else
	{	HighNibble = 0x40 + (temp - 9); }

	temp = value & 0x0F;	//low nibble (4-bit)
	if(temp <10)
	{	LowNibble = temp | 0x30; }
	else
	{	LowNibble = 0x40 + (temp - 9); }

	*result = HighNibble;
	result++;	
	*result = LowNibble;
	result++;	
	*result = 0;	
}

void U32toASCIIHEX(U32 value, U8 *result)
{
	U8 i, p, temp;
	
	p = 60;

	for(i=0;i<16;i++)
	{
		temp = (U8)((value >> p) & 0x0F);
		
		if(temp <10)
		{	*result = temp | 0x30; }
		else
		{	*result = 0x40 + (temp - 9); }

		result++;
		p=p-4;
	}
}

U8 ASCIIHEX2BinByte(U8 *string)
{
	U8 result = 0;
	U8 highNibble = 0;
	U8 lowNibble=0;
	
	highNibble = *string;
	if((highNibble >= 0x30)&&(highNibble <= 0x39))
		highNibble = highNibble & 0x0F;
	else if((highNibble >= 0x41)&&(highNibble <= 0x46))
		highNibble = (highNibble & 0x0F) + 9;
		
	string++;
	lowNibble =  *string;
	if((lowNibble >= 0x30)&&(lowNibble <= 0x39))
		lowNibble = lowNibble & 0x0F;
	else if((lowNibble >= 0x41)&&(lowNibble <= 0x46))
		lowNibble = (lowNibble & 0x0F) + 9;
	
	result = (highNibble << 4) + lowNibble;
	return result;
}

void U8toAsciiBin(U8 value, U8 *result)
{
	U8 x;
	
	x = (value & 0xf0) >> 4;
	*result = ASCIIBIN_data[x][0];
	result++;
	*result = ASCIIBIN_data[x][1];
	result++;
	*result = ASCIIBIN_data[x][2];
	result++;
	*result = ASCIIBIN_data[x][3];
	result++;

	x = value & 0x0f;
	*result = ASCIIBIN_data[x][0];
	result++;
	*result = ASCIIBIN_data[x][1];
	result++;
	*result = ASCIIBIN_data[x][2];
	result++;
	*result = ASCIIBIN_data[x][3];
}

void xor128_seed(void)
{
	srand(time(0));		//seed it with time
	xor128_seed_x = rand();
	xor128_seed_y = rand();
	xor128_seed_z = rand();
	xor128_seed_w = rand();
}

U32 xor128(void)
{
  U32 t;

  t = xor128_seed_x ^ (xor128_seed_x << 11);
  xor128_seed_x = xor128_seed_y; xor128_seed_y = xor128_seed_z; xor128_seed_z = xor128_seed_w;
  return xor128_seed_w = xor128_seed_w ^ (xor128_seed_w >> 19) ^ (t ^ (t >> 8));
}

