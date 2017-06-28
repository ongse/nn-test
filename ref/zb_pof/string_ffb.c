//internal use
#include "string_ffb.h"

#define NOTMATCH		5
#define MATCH			6
#define NULL			0

U8 MemoryArrayString[StringMemoryPoolSize];
U8 *MemoryString = MemoryArrayString;
U8  tempStorageString[TempStringSize];					//temporary working array

Ux countString(U8* stringptr)			//actually is countString_pointer
{
	Ux count = 0;

	while(*stringptr != NULL)
	{
		count++;
		ptrInc(stringptr);
	}

	return count;
}

void copyString(U8* fromString, U8* toString) //make sure the U8 pointer is not auto increment. test by reading same *pointer twice and see
{
	while(*fromString != NULL)
	{
		*toString = *fromString;
		ptrInc(fromString);
		ptrInc(toString);
	}
	*toString = NULL;
}

void copyString_count(U8* fromString, U8* toString, Ux cnt)
{
	Ux a;

	for(a=0; a<cnt; a++)
	{
		*toString = *fromString;
		ptrInc(fromString);
		ptrInc(toString);
	}
	*toString = NULL;
}

Ux compareString(U8* string1, U8* string2)
{
	Ux a, b;
	a = countString(string1);
	b = countString(string2);
	if(a!=b)
		return NOTSAME;					//not same
	while(*string1 != NULL)
	{
		if(*string1 != *string2)
			return	NOTSAME;
		ptrInc(string1);
		ptrInc(string2);
	}

	return	SAME;
}

void fillZeroString(U8* string_ffb, Ux size)
{
	Ux count;

	for(count=0;count<size;count++)
	{
		*string_ffb = NULL;
		ptrInc(string_ffb);
	}
}

void convertUpperString(U8* string_ffb)
{
	while(*string_ffb != NULL)
	{
		if((*string_ffb>=0x61)&&(*string_ffb<=0x7A))
			*string_ffb -=0x20;
		ptrInc(string_ffb);
	}
}

Ux findString(U8* key, U8* string_ffb)	//find and return the end position of string_ffb  (can be used in pointer/array directly based on based-0)
{
	Ux count = 0;
	Ux loop;
	U8* tempPtr = string_ffb;

	if((countString(key)) > (countString(string_ffb)))
		return NOTFOUND;
	loop = countString(string_ffb) - countString(key) + 1;

	while(count<loop)
	{
		if(symbolMatchString(key, string_ffb)==MATCH)
		{
			if((count+(countString(key))+1)<=(countString(tempPtr)))
				return count+(countString(key));
		}
		ptrInc(string_ffb);
		count++;
	}

	return NOTFOUND;
}

Ux findBeginString(U8* key, U8* string_ffb)	
{
	Ux count = 0;
	Ux loop;

	loop = countString(string_ffb) - countString(key) + 1;

	while(count<loop)
	{
		if(symbolMatchString(key, string_ffb)==MATCH)
			return count;
		ptrInc(string_ffb);
		count++;
	}

	return NOTFOUND;
}

U8 symbolMatchString(U8* symbol, U8* string_ffb)	//match symbol given from beginning of string_ffb. eg. symbol=xxx, string_ffb = xxxyyyy... <-- MATCH!
{
	while(*symbol != NULL)
	{
		if(*symbol != *string_ffb)
			return NOTMATCH;
		ptrInc(symbol);
		ptrInc(string_ffb);
	}
	return MATCH;
}

Ux countNoCharString(U8* string_ffb, U8* begin, U8* end)
{
	Ux a,b;

	a = findString(begin, string_ffb);
	if(a == NOTFOUND)
		return STRING_ERROR;

	b = findBeginString(end, string_ffb);
	if(b == NOTFOUND)
		return STRING_ERROR;

	if(a>=b)
		return STRING_ERROR;

	return b-a;
}


Ux extractArrayString(U8* result[], U8* string_ffb, U8* begin, U8* end, U8* seperator)
{
	Ux start, symbolLen;
	Ux point = 0;
	Ux count = 1;
	U8* tempPtr;

	symbolLen = countString(seperator);

	if(findString(end, string_ffb)==NOTFOUND)
		return STRING_ERROR;

	start = findString(begin, string_ffb);
	if(start == NOTFOUND)
		return STRING_ERROR;
	ptrAdd(string_ffb, start);

	result[point] = MemoryString;
	tempPtr = result[point];

	while(symbolMatchString(end, string_ffb) == NOTMATCH)
	{
		if(*string_ffb == NULL)
			return STRING_ERROR;

		if(symbolMatchString(seperator, string_ffb) == NOTMATCH)
		{
			*result[point] = *string_ffb;
			ptrInc(result[point]);
			ptrInc(string_ffb);
			ptrInc(MemoryString);
		}		
		else
		{
			*result[point] = NULL;			//put NULL for end of the array
			result[point] = tempPtr;		//restore pointer to point to first element

			ptrAdd(string_ffb, symbolLen);

			point++;
			ptrInc(MemoryString);
			count ++;
			result[point] = MemoryString;
			tempPtr = result[point];

			if(symbolMatchString(end, string_ffb) == MATCH)				//error checking if the seperator is the last symbol before end symbol
				return --count;
		}
	}

	*result[point] = NULL;			//put NULL for end of the array
	result[point] = tempPtr;		//restore pointer to point to first element

	return count;
}


//void oneD2twoD(U8* memoryPool, U8* arrayPointer[], U8 totalSize, U8 sectionSize)
//{
//	U8 x;
//	U8 section = totalSize / sectionSize;
//
//	for(x=0;x<section;x++)
//	{
//		arrayPointer[x] = memoryPool;
//		ptrAdd(memoryPool, sectionSize);
//	}
//}

void replaceString(U8* string_ffb, U8* ori, U8* replace)
{
	Ux x, oriLen, replaceLen;
	U8* oriPtr = string_ffb;
	U8* opPtr = tempStorageString;
	U8* keepPtr = tempStorageString;

	oriLen = countString(ori);
	replaceLen = countString(replace);

	while(*oriPtr != NULL)
	{
		if(symbolMatchString(ori, oriPtr) == NOTMATCH)
		{
			*opPtr = *oriPtr;
			ptrInc(oriPtr);
			ptrInc(opPtr);
		}		
		else
		{
			for(x=0;x<replaceLen;x++)
			{
				*opPtr = replace[x];
				ptrInc(opPtr);
			}
			ptrAdd(oriPtr, oriLen);
		}
	}

	*opPtr = NULL;

	while(*keepPtr != NULL)
	{
		*string_ffb = *keepPtr;
		ptrInc(keepPtr);
		ptrInc(string_ffb);
	}
	*string_ffb = NULL;
}

void replaceChar(U8* string_ffb, Ux ori, Ux replace)
{
	U8* oriPtr = string_ffb;

	while(*oriPtr != NULL)
	{
		if(*oriPtr == ori)
			*oriPtr = replace;
		ptrInc(oriPtr);
	}
}

void eliminateCharsString(U8* string_ffb, U8* chars)
{
	Ux charsLen;
	U8 *oriPtr = string_ffb;
	U8 *opPtr = tempStorageString;
	U8 *keepPtr = tempStorageString;

	charsLen = countString(chars);

	while(*oriPtr != NULL)
	{
		if(symbolMatchString(chars, oriPtr) == NOTMATCH)
		{
			*opPtr = *oriPtr;
			ptrInc(oriPtr);
			ptrInc(opPtr);
		}		
		else
		{
			ptrAdd(oriPtr, charsLen);
		}
	}

	*opPtr = NULL;

	while(*keepPtr != NULL)
	{
		*string_ffb = *keepPtr;
		ptrInc(keepPtr);
		ptrInc(string_ffb);
	}
	*string_ffb = NULL;
}

void shrinkString(U8* string_ffb, Ux key)
{
	U8 *oriPtr = string_ffb;
	U8 *opPtr = tempStorageString;
	U8 *keepPtr = tempStorageString;

	while(*oriPtr != NULL)
	{
			*opPtr = *oriPtr;
			if(*opPtr == key)
			{
				while(*ptrNext(oriPtr) == key)
					ptrInc(oriPtr);
			}
			ptrInc(oriPtr);
			ptrInc(opPtr);
	}

	*opPtr = NULL;
	while(*keepPtr != NULL)
	{
		*string_ffb = *keepPtr;
		ptrInc(keepPtr);
		ptrInc(string_ffb);
	}
	*string_ffb = NULL;
}

void appendString(U8* basePtr, U8* toBeAppend)
{
	U8 cnt;

	cnt = countString(basePtr);
	while(*toBeAppend != 0)
	{
		basePtr[cnt] = *toBeAppend;
		ptrInc(toBeAppend);
		cnt++;
	}
	basePtr[cnt] = 0;
}

void getElementString(U8* begin, U8 medium, Ux which, U8* string, U8* result)
{
	U16 cnt;
	
	cnt = findString(begin, string);
	if(cnt == NOTFOUND)
		cnt = 0;

	while(which != 0)
	{		
		if(string[cnt] == medium)
			which--;
		cnt++;
	}

	while((string[cnt]!= medium)&&(string[cnt] != 0))
	{
		*result = string[cnt];
		cnt++;
		ptrInc(result);
	}

	*result = 0;
}

void getElementString_BeginEnd(U8* begin, U8* end, U8 medium, Ux which, U8* string, U8* result)
{
/*
	U16 cnt, cntEnd;
	
	cnt = findString(begin, string);
	if(cnt == NOTFOUND)
		cnt = 0;
	
	cntEnd = findString(begin, string);
	if(cnt == NOTFOUND)

	while(which != 0)
	{		
		if(string[cnt] == medium)
			which--;
		cnt++;
	}

	while((string[cnt]!= medium)&&(string[cnt] != 0))
	{
		*result = string[cnt];
		cnt++;
		ptrInc(result);
	}

	*result = 0;
	*/
}

Ux numericalCheck(U8* ptr, Ux length)		//return 1 = all numerical, return 0 if hv non-numeric
{
	Ux cnt;

	for(cnt=0;cnt<length;cnt++)
	{
		if((ptr[cnt] < 0x30)||(ptr[cnt] > 0x39))
			return 0;
	}
	return 1;
}

Ux extractString_BeginEnd_ptrSpecify(U8 begin, U8 end, Ux location, U8* string, U8* result)
{
	U8 ctrl;  //0=not find begin yet, 1=begin found and collecting
	U8* tempString;
	Ux cnt;

	cnt = location;
	tempString = &string[location];
	ctrl = 0;

	while(*tempString != 0)
	{
		if(ctrl == 0)
		{
			if(*tempString == begin)
				ctrl = 1;
			tempString++;
			cnt++;
		}
		else if(ctrl == 1)
		{
			if(*tempString == end)
			{
				*result = 0;
				return cnt;
			}
			else
				*result = *tempString;
			tempString++;
			result++;
			cnt++;
		}
	}

	*result = 0;
	return 0;
}

U8 flipU8(U8 num)
{
    U8 reverse_num = 0; 
	U8 i, temp;
 
    for (i = 0; i < 8; i++)
    {
        temp = (num & (1 << i));
        if(temp)
            reverse_num |= (1 << (7 - i));
    }
  
    return reverse_num;
}

U16 flipU16(U16 num)
{
    U16 reverse_num = 0; 
	U16 i, temp;
 
    for (i = 0; i < 16; i++)
    {
        temp = (num & (1 << i));
        if(temp)
            reverse_num |= (1 << (15 - i));
    }
  
    return reverse_num;
}

U32 flipU32(U32 num)
{
    U32 reverse_num = 0; 
	U32 i, temp;
 
    for (i = 0; i < 32; i++)
    {
        temp = (num & (1 << i));
        if(temp)
            reverse_num |= (1 << (31 - i));
    }
  
    return reverse_num;
}

U64 flipU64(U64 num)
{
    U64 reverse_num = 0; 
	U64 i, temp;
 
    for (i = 0; i < 64; i++)
    {
        temp = (num & (1 << i));
        if(temp)
            reverse_num |= (1 << (63 - i));
    }
  
    return reverse_num;
}
