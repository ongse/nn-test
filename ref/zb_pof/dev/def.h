//******************************** DEFINITION FILE ************************************//
//
//THIS FILE SHOULD CONTAIN PURELY #DEFINE ONLY!!!
//
//*************************************************************************************//


//************************* Processor and Compiler Dependant (typedef.h) ***************//
#define U1	bit
#define U8	unsigned char
#define U16	unsigned short int
#define U32	unsigned int
#define U64	unsigned long int
#define S16	short int
#define S32	int
//#define FP32 float;     	/* Single precision floating point   */
//#define FP64 double;     	/* Double precision floating point   */
//Info: 16-bit -> 65 x 10e3(bits)  = 65 KiloBytes  BCD = 1 x 10e4  (4 digit)
//      32-bit -> 4  x 10e9(bits)  = 4  GigaBytes  BCD = 1 x 10e8  (8 digit)
//      64-bit -> 16 x 10e18(bits) = 16 ExaBytes   BCD = 1 x 10e16 (16 digit)

//************************* Common definition for all functional blocks ***************//
//Bit operation
//x = register; y = defined bit(s) / mask
#define setBit(x, y)		x |= y
#define clearBit(x, y)		x &= (~y)
#define invertBit(x, y)		x ^= y
#define checkBit(x,y)		(x & y)			// return >0 if checking bit is "1" ; 0 if checking bit is "0" ;
#define putBits(x, i, y, z)	x = ((i << z) & y) | (x & (~y))	//x=ori_to_change, i=data_to_put, y=mask(mark 1 for wanted bit, eg, 0111 1000), z=position(start from 0)
#define compareBits(x, y)	(x^y)			// return >0 if DIFFERENT; 0 if checking bit is SAME

//#define checkBit(x, y)	(x&y)>=1 ?1 :0
//#define checkBit(x, y)	if((x&y)>=1) return 1; else return 0;
#define bit0 0x01
#define bit1 0x02
#define bit2 0x04
#define bit3 0x08
#define bit4 0x10
#define bit5 0x20
#define bit6 0x40
#define bit7 0x80

//Port I/O
#define outputPort(x,y)	x = y				//eg. outputPort(Port1, 0x80)
#define inputPort(x)	(x)

//Pointer operation
#define ptrInc(x)			x++
#define ptrAdd(x, y)		x += y
#define ptrReverse(x, y)	x -= y
#define ptrNext(x)			(x+1)

//Commmon Status
#define TRUE            1
#define FALSE           0

//Switch-Case Simplification
//note: switch -- must have default: break;
//      case   -- must have break;
#define Switch(x)	switch(x){
#define EndSwitch	default: break;}
#define Case(x)		case x: {
#define EndCase		break;}

//error system
#define ERROR           0xFE
#define ERRORTRAP(x)    if(x==ERROR) while{1}        //only can use on emulation mode


//************************************ Simple Template ***********************************//
//****** template for simple timer
//declare: (1) sTimer_variable
//#define XXX_STIMER_RELOAD_VALUE
#define start_stimer(sTimer_variable, stimer_reload_value)	sTimer_variable = stimer_reload_value
#define STIMER_STOP	0xFF
#define stop_stimer(sTimer_variable)						sTimer_variable = STIMER_STOP
//put this check routine in timer
/*
	if(count != -1)
	{
		count--;
		if(count == 0)
		{
			count = -1;
			//action
		}
	}
*/
//****** template for binary input
//declare:	(1) PreInputBit
//#define INPUT_ON
//#define INPUT_OFF
//#define INPUT_ON_EVENT
//#define INPUT_OFF_EVENT
/*
	if(checkInput() == INPUT_ON)		//input ON
	{
		if(PreInputBit == INPUT_OFF)	//just ON
		{
			PreInputBit = INPUT_ON;		
			event = INPUT_ON_EVENT;		//report ON event
		}
	}
	else								//input OFF
	{
		if(PreInputBit == INPUT_ON)		//just OFF
		{
			PreInputBit = INPUT_OFF;
			event = INPUT_OFF_EVENT;	//report OFF event
		}
	}
*/
//****** template for binary input with jitter filter
//declare:	(1) InputCount
//			(2) PreInputBit
//#define INPUT_ON
//#define INPUT_OFF
//#define INPUT_JITTER
//#define INPUT_ON_EVENT
//#define INPUT_OFF_EVENT
/*
	if(checkInput() == INPUT_ON)					//input ON
	{
		if(PreInputBit == INPUT_ON)					//already ON
		{
			if(InputCount < INPUT_JITTER)
			{
				InputCount++;
				if(InputCount >= INPUT_JITTER)
					event = INPUT_ON_EVENT;			//report ON event
			}
		}
		else											//just ON
		{
			InputCount = NULL;
			PreInputBit = INPUT_ON;
		}
	}
	else											//input OFF
	{
		if(PreInputBit == INPUT_OFF)				//already OFF
		{
			if(InputCount < INPUT_JITTER)
			{
				InputCount++;
				if(InputCount >= INPUT_JITTER)
					event = INPUT_OFF_EVENT;		//report OFF event
			}
		}
		else										//just OFF
		{
			InputCount = NULL;
			PreInputBit = INPUT_OFF;
		}
	}
*/


