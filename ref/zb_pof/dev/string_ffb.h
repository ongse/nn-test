#include "def.h"

#define Ux				U32
#define NOTSAME			0x01
#define SAME			0x02
#define NOTFOUND		0xFC
#define FOUND			0xFD
#define STRING_ERROR	0xFE

#define TempStringSize			3000		//should hv enough size for every single string manipulation
#define StringMemoryPoolSize	3000		//should hv enough size for sum of all layer array

//******** Functions Prototyping **************
//U8 countString_array(U8* string_ffb);							//experiment for countString only
//U8 countString_pointer(U8* string_ffb);						//experiment for countString only

void copyString(U8* fromString, U8* toString);		
void copyString_count(U8* fromString, U8* toString, Ux cnt);		
Ux compareString(U8* string1, U8* string2);						//return MATCH or NOTMATCH
void convertUpperString(U8* string_ffb);						//convert all lower case char(only) to upper
void fillZeroString(U8* string_ffb, Ux size);					//fill in the string_ffb all NULL
Ux findBeginString(U8* key, U8* string_ffb);					//eg find aaa -> xxaaayy0 return 2 (value can be directly used by based-0)
Ux countNoCharString(U8* string_ffb, U8* begin, U8* end);		//eg count aa, bb for xxaayyyybbzzz return 4
Ux countString(U8* string_ffb);									//eg. given abcxyz -> 6  //actually is countString_array //eg. xxxx0 return 4
Ux findString(U8* key, U8* string_ffb);						//eg.find aaa -> xxaaayy0 return 5 (value can be directly used by based-0) //find and return the end position of string_ffb  (can be used in pointer/array directly based on based-0)
U8 symbolMatchString(U8* symbol, U8* string_ffb);				//eg. match aaa with aaabbbccc return MATCH //match symbol given from beginning of string_ffb. eg. symbol=xxx, string_ffb = xxxyyyy... <-- MATCH!
Ux extractArrayString(U8* result[], U8* string_ffb, U8* begin, U8* end, U8* seperator);
void replaceString(U8* string_ffb, U8* ori, U8* replace);		//eg. given ori xxx, replace y, string_ffb aaaxxxbbb -> aaaybbb
void replaceChar(U8* string_ffb, Ux ori, Ux replace);			//eg. given ori x, replace y, string_ffb aaaybbb -> aaaybbb
void eliminateCharsString(U8* string_ffb, U8* chars);			//eg. given chars xxx, string_ffb aaaxxxbbb -> aaabbb
void shrinkString(U8* string_ffb, Ux key);						//eg. key x, string aaxxxbbxx -> aaxbbx
void oneD2twoD(U8* memoryPool, U8* arrayPointer[], U8 totalSize, U8 sectionSize);	//eg. x[20] -> y[4][5] // no need now, maybe can be used in memory management
void appendString(U8* basePtr, U8* toBeAppend);					//eg. base=xxx, append=yyy => xxxyyy
void getElementString(U8* begin, U8 medium, Ux which, U8* string, U8* result);		//eg. string aa xx,yyy,z if(which==0) result =xx, if(which==2) result = z
void getElementString_BeginEnd(U8* begin, U8* end, U8 medium, Ux which, U8* string, U8* result);
Ux numericalCheck(U8* ptr, Ux length);							//return 1 = all numerical, return 0 if hv non-numeric
Ux extractString_BeginEnd_ptrSpecify(U8 begin, U8 end, Ux location, U8* string, U8* result);  //eg. location=5 begin='<' end='>' string=<xyz><aaabbbccc> result=aaabbbccc return=16
U8 flipU8(U8 num);
U16 flipU16(U16 num);
U32 flipU32(U32 num);
U64 flipU64(U64 num);


//*************test if the *pointer has auto increment
//U8 test1, test2, test3;  			
//test1 = *string3;
//test2 = *string3;
//string3++;  //for pointer only
//test3 = *string3;
//*************
