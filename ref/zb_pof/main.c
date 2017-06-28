#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BinBCD.h"
#include "string_ffb.h"

#define debugmsg	//printf
#define EndofFile	0

void data_file_setup(void);
void core_din_output(void);
void core_data_output(void);
void core_data_generation(void);
void iocsr_din_output(void);
void iocsr_data_generation(void);
void iocsr_data_output(void);

FILE *fsml;
FILE *outputdat;
FILE *spi_comp_outputdat;
FILE *sdm_core_outputdat;
FILE *sdm_comp_outputdat;
FILE *iocsrcffdat;
FILE *corecffdat;
unsigned int sml_fsize;
char *sml_ptr;
char *temp;
unsigned int smlIteration;
unsigned char hextxt[4];
unsigned char *hextxt_ptr;
unsigned char bintxt[9];
unsigned char *bintxt_ptr;

char j;
unsigned int i, t;
char *k;
unsigned int l, m;
unsigned short int random_num, r;

char workbuf1[500];
char workbuf2[500];
char output_filename[30];
char spi_output_comp_filename[30];
char sdm_iocsr_filename[30];
char sdm_core_filename[30];
char iocsr_cff_filename[30];
char core_cff_filename[30];
char *singleSmlBuf_ptr;
char *extractPara_ptr;
char *output_filename_ptr;
char *spi_output_comp_filename_ptr;
char *sdm_iocsr_filename_ptr;
char *sdm_core_filename_ptr;
char *iocsr_cff_filename_ptr;
char *core_cff_filename_ptr;

char iocsr_data_type = 3;			//1-zero, 2-one, 3-random, 4-customize data
char *iocsr_data;
char *iocsr_data_ptr;
unsigned int *iocsr_sdm_data;
unsigned int *iocsr_sdm_data_ptr;
unsigned short int iocsr_crc;
unsigned int ioscr_length;
unsigned int ioscr_pad;
unsigned int iocsr_frame_no, core_frame;

char core_data_type = 3;			//1-zero, 2-one, 3-random
char *core_data;
char *core_data_ptr;
char *core_spi_compress_data;
char *core_spi_compress_data_ptr;
unsigned int *core_spi_compress_data_temp;
unsigned int *core_spi_compress_data_temp_ptr;
unsigned int *core_sdm_data;
unsigned int *core_sdm_data_ptr;
unsigned short int core_crc;
unsigned int core_length, core_comp_length;
unsigned int core_frame_no, core_data_hold;
char core_ft, core_sl, core_width, core_seg, core_1st, second_dword, core_switch;
char comp_high_nibble;

unsigned int dword_data[10000];
unsigned char w,x,y,z;
unsigned int v;
unsigned int *dword_data_ptr;

int main(void)
{
	printf("\n    ------------------------------------------------------------------------");
	printf("\n    Altera Image Generation Software Tool v3.3");
	printf("\n    Copyright 2013 (c) Altera Corporation.");
	//printf("\n    contact: seong@altera.com");
	printf("\n    ------------------------------------------------------------------------");
	/* open and read in sml file to working buffer*/
	fsml = fopen("zb_setting.sml", "r");	//sml - simple markup language
	if (fsml == NULL)
	{
		printf("\nzb_setting.sml open error\n");
		exit(0);
	}
	else
		printf("\n    zb_setting.sml read successfully");
	
	fseek(fsml, 0L, SEEK_END);	//seek end of file for file size
	sml_fsize = ftell(fsml);	//get file size
	fseek(fsml, 0L, SEEK_SET);	//set file pointer back to intial
	debugmsg("\n    file size is %d", sml_fsize);	

	sml_ptr = malloc(sml_fsize);
	temp = sml_ptr;
	
	for(i=0; i<sml_fsize; i++)
	{
		*temp = fgetc(fsml);
		temp++;
	}
	fclose(fsml);	

	/* process sml - one by one */
	smlIteration = 0;
	temp = sml_ptr;
	
	output_filename_ptr = &output_filename[0];
	sdm_iocsr_filename_ptr = &sdm_iocsr_filename[0];
	sdm_core_filename_ptr = &sdm_core_filename[0];
	iocsr_cff_filename_ptr = &iocsr_cff_filename[0];
	core_cff_filename_ptr = &core_cff_filename[0];
	spi_output_comp_filename_ptr = &spi_output_comp_filename[0];
	
	second_dword = 0;
	core_switch = 0;
	comp_high_nibble = 1;

	while(1)
	{
		singleSmlBuf_ptr = &workbuf1[0];
		extractPara_ptr = &workbuf2[0];
		smlIteration = extractString_BeginEnd_ptrSpecify('<','>', smlIteration, temp, singleSmlBuf_ptr);
		if(smlIteration == 0) 
			break;
		debugmsg("\n    context = \"%s\"; number = %i", singleSmlBuf_ptr, smlIteration);
		
		//****** General Setting
		if(findString("TESTING", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("TESTING=", ',', 1, singleSmlBuf_ptr, extractPara_ptr);
			if(compareString("ZB8", extractPara_ptr)==SAME)
				debugmsg("    ZB8 device, ");
			else
				debugmsg("    not ZB8 device, ");
			
			getElementString("MARKING=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			j = AsciiDec2Bin(extractPara_ptr);	
			debugmsg("\n    parameter = %i", j);
		}
		else if(findString("VERSION", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("VERSION=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			if(compareString("3.3", extractPara_ptr)==SAME)
				printf("\n    ... software version verified");
			else
			{
				printf("    version not match... exit..");
				break;
			}
		}
		//****** FileName Setting
		else if(findString("SPI_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SPI_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, output_filename_ptr);
			debugmsg("\n    SPI OUTPUT FILENAME = %s", output_filename_ptr);
		}		
		else if(findString("SPI_OUTPUT_COMP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SPI_OUTPUT_COMP_FILE=", ',', 0, singleSmlBuf_ptr, spi_output_comp_filename_ptr);
			debugmsg("\n    SPI COMPRESS OUTPUT FILENAME = %s", spi_output_comp_filename_ptr);
		}		
		else if(findString("SDM_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, sdm_core_filename_ptr);
			debugmsg("\n    SDM OUTPUT FILENAME = %s", sdm_core_filename_ptr);
		}
		else if(findString("SDM_OUTPUT_COMP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_OUTPUT_COMP_FILE=", ',', 0, singleSmlBuf_ptr, sdm_core_filename_ptr);
			debugmsg("\n    SDM COMPRESS OUTPUT FILENAME = %s", sdm_core_filename_ptr);
		}
		else if(findString("IOCSR_EXP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("IOCSR_EXP_FILE=", ',', 0, singleSmlBuf_ptr, iocsr_cff_filename_ptr);
			debugmsg("\n    IOCSR EXPECTED OUTPUT FILENAME = %s", iocsr_cff_filename_ptr);
		}
		else if(findString("CORE_EXP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("CORE_EXP_FILE=", ',', 0, singleSmlBuf_ptr, core_cff_filename_ptr);
			debugmsg("\n    CORE EXPECTED OUTPUT FILENAME = %s", core_cff_filename_ptr);
		}
		else if(findString("CREATE_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			data_file_setup();
		}
		//***** IOCSR
		else if(findString("IOCSR_FRAME", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("IOCSR_FRAME=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			iocsr_frame_no = AsciiDec2Bin(extractPara_ptr);	
			printf("\n    ...Total IOCSR frame number = %i", iocsr_frame_no);
		}
		else if((findString("IOCSR_DATA", singleSmlBuf_ptr)!=NOTFOUND)&&(iocsr_frame_no != 0))
		{
			iocsr_frame_no--;
			getElementString("IOCSR_DATA=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			i = AsciiDec2Bin(extractPara_ptr);
			printf("\n    IOCSR frame no.%i", i);
			if(findString("IOCSR_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("IOCSR_DATA=", ',', 1, singleSmlBuf_ptr, extractPara_ptr);
				ioscr_length = AsciiDec2Bin(extractPara_ptr);	
				printf("\n    IOCSR frame length = %i", ioscr_length);
			}
			if(findString("IOCSR_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("IOCSR_DATA=", ',', 2, singleSmlBuf_ptr, extractPara_ptr);
				ioscr_pad = AsciiDec2Bin(extractPara_ptr);	
				printf("\n    IOCSR pad length = %i", ioscr_pad);
			}					
			if(findString("IOCSR_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("IOCSR_DATA=", ',', 3, singleSmlBuf_ptr, extractPara_ptr);
				printf("\n    IOCSR Data type = ");
				if(compareString("random", extractPara_ptr)==SAME)
				{
					iocsr_data_type = 3;
					printf("random");
				}
				else if(compareString("one", extractPara_ptr)==SAME)
				{
					iocsr_data_type = 2;
					printf("all 1's");
				}
				else
				{
					iocsr_data_type = 1;
					printf("all 0's");
				}
			}
			
			iocsr_data_generation();
			iocsr_data_output();
			iocsr_din_output();
			debugmsg("\n    ...iocsr output complete");
		}
		//CORE
		else if(findString("CORE_FRAME", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("CORE_FRAME=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			core_frame_no = AsciiDec2Bin(extractPara_ptr);	
			core_frame = core_frame_no;
			printf("\n    ...Total core frame number = %i", core_frame_no);
		}
		else if(findString("CORE_SWITCH", singleSmlBuf_ptr)!=NOTFOUND)
		{
			core_switch = 1;
			comp_high_nibble = 1;
		}
		else if((findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)&&(core_frame_no != 0))
		{
			if(core_frame_no == core_frame)
				core_1st = 1;
			else
				core_1st = 0;
			core_frame_no--;
			getElementString("CORE_DATA=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			i = AsciiDec2Bin(extractPara_ptr);
			printf("\n    core frame no.%i", i);
			if(findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("CORE_DATA=", ',', 1, singleSmlBuf_ptr, extractPara_ptr);
				core_length = AsciiDec2Bin(extractPara_ptr);	
				printf("\n    Core frame length = %i", core_length);
			}
			if(findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("CORE_DATA=", ',', 2, singleSmlBuf_ptr, extractPara_ptr);
				printf("\n    Core Data type = ");
				if(compareString("random", extractPara_ptr)==SAME)
				{
					core_data_type = 3;
					printf("random");
				}
				else if(compareString("one", extractPara_ptr)==SAME)
				{
					core_data_type = 2;
					printf("all 1's");
				}
				else if(compareString("zero", extractPara_ptr)==SAME)
				{
					core_data_type = 1;
					printf("all 0's");
				}
				else
				{
					core_data_type = 4;
					core_data_hold = 0;

					j = ASCIIHEX2BinByte(extractPara_ptr);
					r = (unsigned int)j & 0x000000ff;
					core_data_hold = core_data_hold | (r<<24);
					extractPara_ptr++;
					extractPara_ptr++;

					j = ASCIIHEX2BinByte(extractPara_ptr);
					r = (unsigned int)j & 0x000000ff;
					core_data_hold = core_data_hold | (r<<16);
					extractPara_ptr++;
					extractPara_ptr++;

					j = ASCIIHEX2BinByte(extractPara_ptr);
					r = (unsigned int)j & 0x000000ff;
					core_data_hold = core_data_hold | (r<<8);
					extractPara_ptr++;
					extractPara_ptr++;

					j = ASCIIHEX2BinByte(extractPara_ptr);
					r = (unsigned int)j & 0x000000ff;
					core_data_hold = core_data_hold | (r<<0);
					
					printf("0x%x", core_data_hold);
				}
			}
			if(findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("CORE_DATA=", ',', 3, singleSmlBuf_ptr, extractPara_ptr);
				core_ft = ASCIIHEX2BinByte(extractPara_ptr);	
				printf("\n    CORE Frame Type = 0x%x", core_ft & 0xff);
			}
			if(findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("CORE_DATA=", ',', 4, singleSmlBuf_ptr, extractPara_ptr);	//SL = size of frame
				core_sl = ASCIIHEX2BinByte(extractPara_ptr);				
				printf("\n    CORE Frame Length = 0x%x", core_sl & 0xff);
			}
			if(findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("CORE_DATA=", ',', 5, singleSmlBuf_ptr, extractPara_ptr);	//SL = size of frame
				core_width = AsciiDec2Bin(extractPara_ptr);				
				core_seg = core_width;
				printf("\n    CORE Segmentation = %i", core_width & 0xff);
			}
			
			core_data_generation();
			core_data_output();
			core_din_output();
			debugmsg("\n    ...core output complete");
		}
	}

	printf("\n    ------------------------------------end---------------------------------\n");

	fclose(outputdat);
	fclose(spi_comp_outputdat);
	fclose(sdm_core_outputdat);
	fclose(iocsrcffdat);
	fclose(corecffdat);

	free(iocsr_data);
	free(iocsr_sdm_data);
	free(core_data);
	free(core_sdm_data);
	free(sml_ptr);
	
	return 0;
}

void data_file_setup(void)	
{
	outputdat = fopen(output_filename_ptr, "w");			//create SPI POF Image file
	if (outputdat == NULL)
	{
		printf("\n    SPI POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SPI POF image file created successfully");

	spi_comp_outputdat = fopen(spi_output_comp_filename_ptr, "w");			//create SPI POF Image file
	if (spi_comp_outputdat == NULL)
	{
		printf("\n    SPI Compress POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SPI Compress POF image file created successfully");

	sdm_core_outputdat = fopen(sdm_core_filename_ptr, "w");			//create SDM CORE POF Image file
	if (sdm_core_outputdat == NULL)
	{
		printf("\n    SDM CORE POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SDM CORE POF image file created successfully");		

	iocsrcffdat = fopen(iocsr_cff_filename_ptr, "w");		//create IOCSR CFF file
	if (iocsrcffdat == NULL)
	{
		printf("\n    IOCSR CFF file creation error\n");
		exit(0);
	}
	else
		printf("\n    IOCSR CFF file created successfully");

	corecffdat = fopen(core_cff_filename_ptr, "w");			//create CORE CFF file
	if (corecffdat == NULL)
	{
		printf("\n    Core CFF file creation error\n");
		exit(0);
	}
	else
		printf("\n    CORE CFF file created successfully");
	
	printf("\n    ... Files created");
}

void iocsr_data_generation(void)	//*** IOCSR data generation
{	
	debugmsg("\n    ... IOCSR data generation\n");		//IOCSR data generation

	xor128_seed();
	iocsr_data = malloc(((ioscr_length+ioscr_pad)*4)+9);
	iocsr_data_ptr = iocsr_data;
	iocsr_sdm_data = (unsigned int *) malloc((ioscr_length+ioscr_pad+1)*4);
	iocsr_sdm_data_ptr = iocsr_sdm_data;
	
	for(i=0;i<ioscr_length;i++)		
	{
		if(iocsr_data_type == 1)
			l = 0;
		else if(iocsr_data_type == 2)
			l = 1;
		else
			l = (unsigned int)xor128();
		
		*iocsr_sdm_data_ptr = l;		//sdm data
		iocsr_sdm_data_ptr++;
		
		l = flipU32(l);					//spi data
		//debugmsg("%i=%x->", i, l & 0xffffffff);
		*iocsr_data_ptr = (char)(l >> 24)&0x000000ff;
		//debugmsg("%x,", *iocsr_data_ptr & 0xff);
		iocsr_data_ptr++;
		*iocsr_data_ptr = (char)(l >> 16)&0x000000ff;
		//debugmsg("%x,", *iocsr_data_ptr & 0xff);
		iocsr_data_ptr++;
		*iocsr_data_ptr = (char)(l >> 8)&0x000000ff;
		//debugmsg("%x,", *iocsr_data_ptr & 0xff);
		iocsr_data_ptr++;
		*iocsr_data_ptr = (char)(l & 0x000000ff);
		//debugmsg("%x ", *iocsr_data_ptr & 0xff);
		iocsr_data_ptr++;
	}

	for(i=0;i<(ioscr_pad*4);i++)		//IOCSR pad generation (SPI)
	{
		*iocsr_data_ptr = 0xFF;
		iocsr_data_ptr++;
		//debugmsg("ff ");
	}

	for(i=0;i<ioscr_pad;i++)		//IOCSR pad generation (SDM)
	{
		*iocsr_sdm_data_ptr = 0xFF;
		iocsr_sdm_data_ptr++;
	}

	random_num = (unsigned short int)xor128();		//********* random number
	*iocsr_sdm_data_ptr = (unsigned int)(random_num & 0x0000FFFF);		//sdm portion
	random_num = flipU16(random_num);				//spi portion
	*iocsr_data_ptr = (random_num >> 8) & 0x00ff;
	debugmsg("r=%x",*iocsr_data_ptr & 0xff);
	iocsr_data_ptr++;
	*iocsr_data_ptr = random_num & 0x00ff;
	debugmsg("%x ",*iocsr_data_ptr & 0xff);
	iocsr_data_ptr++;
	
	debugmsg("\n    DWORD=%i\n", (ioscr_length+ioscr_pad)+1);	//************ compute CRC
	v = 0;									
	dword_data[0] = 0;
	for(i=0;i<(ioscr_length+ioscr_pad);i++)			//4*U8 -> U32(flip)
	{
		w = iocsr_data[v];
		x = iocsr_data[v+1];
		y = iocsr_data[v+2];
		z = iocsr_data[v+3];
		dword_data[i] = (unsigned int)((w<<24) | (x<<16) | (y<<8) | z);
		debugmsg("%i=%x",i, dword_data[i]);
		dword_data[i] = flipU32(dword_data[i]);
		debugmsg("->%x ",dword_data[i]);
		v+=4;
		dword_data[i+1] = 0;
	}
	w = iocsr_data[v];								
	x = iocsr_data[v+1];
	r = (unsigned short int)((w<<8) | x);
	r = flipU16(r);
	dword_data[ioscr_length+ioscr_pad]   = r & 0x0000ffff;
	debugmsg("%x ",dword_data[ioscr_length+ioscr_pad]);
	
	dword_data_ptr = &dword_data[0];
	iocsr_crc = calculate_crc_cnt_dword(0x0000ffff, dword_data_ptr, ((ioscr_length+ioscr_pad)*32)+16);
	debugmsg("\n    CRC=%x of %i bits", iocsr_crc & 0xffff, ((ioscr_length+ioscr_pad)*32)+16);
	
	*iocsr_sdm_data_ptr = *iocsr_sdm_data_ptr | ((unsigned int)((iocsr_crc << 16)& 0xFFFF0000));		//sdm portion
	
	iocsr_crc = flipU16(iocsr_crc);						//SPI portion
	debugmsg("\n    CRC flip=%x", iocsr_crc & 0xffff);
	
	j = (unsigned char)((iocsr_crc>>8) & 0x00FF);	//record CRC data
	*iocsr_data_ptr = j;
	iocsr_data_ptr++;

	j = (unsigned char)(iocsr_crc & 0x00FF);
	*iocsr_data_ptr = j;

	printf("\n    iocsr CRC = 0x%x (0x%x) of %i bits", (flipU16(iocsr_crc) & 0xffff), iocsr_crc & 0xffff, ((ioscr_length+ioscr_pad)*32)+16);	
	printf("\n    ... IOCSR data generation successful");
}

void iocsr_data_output(void)
{
	//debug -- iocsr data print checking
	debugmsg("\n    ... IOCSR data output\n");		//IOCSR data generation
	
	iocsr_data_ptr = iocsr_data;			
	iocsr_sdm_data_ptr = iocsr_sdm_data;
	debugmsg("\n    generate ");
	for(i=0;i<((ioscr_length*4)+4);i++)		
	{
		j = *iocsr_data_ptr;
		debugmsg("%i=%x ", i, j & 0xff);
		iocsr_data_ptr++;
	}
	
	//IOCSR Frame
	for(i=0;i<4;i++)					//start - SPI portion
	{
		fputc('0', outputdat);
		fputc('0', outputdat);
		if(i == 0)
			fprintf(outputdat, "  //iocsr frame start (start dword) %i", iocsr_frame_no+1);
		fputc(0x0A, outputdat);
	}

	for(i=0;i<4;i++)					//start - SPI Compress portion
	{
		fputc('0', spi_comp_outputdat);
		fputc('0', spi_comp_outputdat);
		if(i == 0)
			fprintf(spi_comp_outputdat, "  //iocsr frame start (start dword) %i", iocsr_frame_no+1);
		fputc(0x0A, spi_comp_outputdat);
	}

	fprintf(sdm_core_outputdat, "00000000");	//start - SDM portion
	
	debugmsg("\n    compare  ");
	hextxt_ptr = &hextxt[0];					//IOCSR data - SPI portion
	for(i=0;i<(((ioscr_length+ioscr_pad)*4)+4);i++)		
	{
		j = iocsr_data[i];
		U8toASCIIHEX(j, hextxt_ptr);
		hextxt[2] = 0;
		debugmsg("%i=%s ", i, hextxt_ptr);
		
		j = hextxt[0];
		fputc(j, outputdat);
		fputc(j, spi_comp_outputdat);		
		j = hextxt[1];
		fputc(j, outputdat);
		fputc(j, spi_comp_outputdat);

		if(i == 0)
		{
			fprintf(outputdat, "  //iocsr data start");			
			fprintf(spi_comp_outputdat, "  //iocsr data start");			
		}
		else if((i == (ioscr_length*4))&&(ioscr_pad > 0))
		{
			fprintf(outputdat, "  //iocsr padding");
			fprintf(spi_comp_outputdat, "  //iocsr padding");
		}
		else if(i == (((ioscr_length+ioscr_pad)*4)+2))
		{
			fprintf(outputdat, "  //iocsr CRC signature");
			fprintf(spi_comp_outputdat, "  //iocsr CRC signature");
		}
		fputc(0x0A, outputdat);	
		fputc(0x0A, spi_comp_outputdat);	
	}
	
	hextxt_ptr = &hextxt[0];							//IOCSR data - SDM portion
	for(i=0;i<(ioscr_length+ioscr_pad+1);i++)
	{
		debugmsg("%i=", i);
		l = iocsr_sdm_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		debugmsg("%s ", hextxt_ptr);
		hextxt[8] = 0;
		fprintf(sdm_core_outputdat, "%s", hextxt_ptr);

		if(i==(ioscr_length+ioscr_pad))
		{
			fprintf(sdm_core_outputdat, "    //end of frame %i", iocsr_frame_no+1);
			fputc(0x0A, sdm_core_outputdat);
		}
		else if(i%2==0)
			fputc(0x0A, sdm_core_outputdat);
	}
}

void iocsr_din_output(void)
{
	hextxt_ptr = &hextxt[0];					//IOCSR CFF
	for(i=0;i<(((ioscr_length+ioscr_pad)*4)+4);i++)		
	{
		j = iocsr_data[i];
		U8toAsciiBin(j, hextxt_ptr);
		hextxt[8] = 0;
		//debugmsg("%i=%s ", i, hextxt_ptr);
		
		for(l=0;l<8;l++)
		{
			j = hextxt[l];
			fputc(j, iocsrcffdat);
		}
		fputc(0x0A, iocsrcffdat);
	}
}

char pack_nibble_data(char *ptr, char nibble_status, char data)
{
	if(nibble_status == 1)
	{
		*ptr = (*ptr & 0x00) | (data << 4);
		return 0;
	}
	else
	{
		*ptr = (*ptr & 0xF0) | data;
		return 1;
	}
}

void core_data_generation(void)	//*** Core data generation
{
	char comp_nibble_temp, comp_nibble_ctrl;

	debugmsg("\n    -------- Core data --------\n");		//core data generation

	core_data = malloc((core_length*4)+9);							//data malloc
	core_data_ptr = core_data;
	core_spi_compress_data_temp = malloc((core_length*4)+9);
	core_spi_compress_data_temp_ptr = core_spi_compress_data_temp;
	core_spi_compress_data = malloc((core_length*4)*2);
	core_spi_compress_data_ptr = core_spi_compress_data;
	core_sdm_data = (unsigned int *) malloc((core_length+1)*4);
	core_sdm_data_ptr = core_sdm_data;

	*core_data_ptr = 0;		//start zero - spi portion
	core_data_ptr++;
	*core_data_ptr = 0;
	core_data_ptr++;

	r = 0;
	r = (unsigned short int)(((core_sl << 8)& 0xff00) | core_ft);
	
	*core_sdm_data_ptr = (unsigned int)(r<<16);		//start - sdm portion
	core_sdm_data_ptr++;

	*core_spi_compress_data_temp_ptr = (unsigned int)(r<<16);;	//start - spi compression portion
	core_spi_compress_data_temp_ptr++;

	r = flipU16(r);				//start - spi portion
	*core_data_ptr = (char)(r >> 8)&0x00ff;
	//debugmsg("%x", *core_data_ptr & 0xff);
	core_data_ptr++;
	*core_data_ptr = (char)(r & 0x00ff);
	//debugmsg("%x ", *core_data_ptr & 0xff);
	core_data_ptr++;		

	for(i=0;i<core_length;i++)		//core data
	{
		if(core_data_type == 1)
			l = 0;
		else if(core_data_type == 2)
			l = 1;
		else if(core_data_type == 4)
			l = core_data_hold;
		else
			l = (unsigned int)xor128();

		*core_sdm_data_ptr = l;		//sdm portion
		core_sdm_data_ptr++;

		*core_spi_compress_data_temp_ptr = l;	//spi compression portion
		core_spi_compress_data_temp_ptr++;
		
		//spi portion
		l = flipU32(l);				
		//debugmsg("%i=%x->", i, l & 0xffffffff);
		*core_data_ptr = (char)(l >> 24)&0x000000ff;
		//debugmsg("%x", *core_data_ptr & 0xff);
		core_data_ptr++;
		*core_data_ptr = (char)(l >> 16)&0x000000ff;
		//debugmsg("%x", *core_data_ptr & 0xff);
		core_data_ptr++;
		*core_data_ptr = (char)(l >> 8)&0x000000ff;
		//debugmsg("%x", *core_data_ptr & 0xff);
		core_data_ptr++;
		*core_data_ptr = (char)(l & 0x000000ff);
		//debugmsg("%x ", *core_data_ptr & 0xff);
		core_data_ptr++;		
	}

	random_num = (unsigned short int)xor128();		//************ random number
	*core_sdm_data_ptr = (unsigned int)(random_num & 0x0000FFFF);		//sdm portion
	debugmsg("    r=%x",*core_sdm_data_ptr & 0xffff);
	*core_spi_compress_data_temp_ptr = *core_spi_compress_data_temp_ptr | ((unsigned int)(random_num & 0x0000FFFF));	//spi compress portion
	random_num = flipU16(random_num);					//SPI portion
	*core_data_ptr = (random_num >> 8) & 0x00ff;
	//debugmsg("    r=%x",*core_data_ptr & 0xff);
	core_data_ptr++;
	*core_data_ptr = random_num & 0x00ff;
	//debugmsg("%x ",*core_data_ptr & 0xff);
	core_data_ptr++;
	
	debugmsg("\n    DWORD=%i\n", core_length+1);	//************ compute CRC
	v = 0;
	dword_data[0] = 0;
	for(i=0;i<(core_length+1);i++)			//4*U8 -> U32(flip)
	{
		w = core_data[v];
		x = core_data[v+1];
		y = core_data[v+2];
		z = core_data[v+3];
		dword_data[i] = (unsigned int)((w<<24) | (x<<16) | (y<<8) | z);
		debugmsg("%i=%x",i, dword_data[i]);
		dword_data[i] = flipU32(dword_data[i]);
		debugmsg("->%x ",dword_data[i]);
		v+=4;
		dword_data[i+1] = 0;
	}
	w = core_data[v];								
	x = core_data[v+1];
	r = (unsigned short int)((w<<8) | x);
	r = flipU16(r);
	dword_data[core_length+1]   = r & 0x0000ffff;
	debugmsg("%x ",dword_data[core_length]);
	
	dword_data_ptr = &dword_data[0];
	core_crc = calculate_crc_cnt_dword(0x0000ffff, dword_data_ptr, ((core_length+1)*32)+16);
	debugmsg("\n    CRC=%x of %i bits", core_crc & 0xffff, ((core_length+1)*32)+16);

	*core_sdm_data_ptr = *core_sdm_data_ptr | ((unsigned int)((core_crc << 16)& 0xFFFF0000));		//sdm portion
	*core_spi_compress_data_temp_ptr = *core_spi_compress_data_temp_ptr | ((unsigned int)((core_crc << 16)& 0xFFFF0000));		//spi compress portion
	
	core_crc = flipU16(core_crc);	//spi portion
	debugmsg("\n    CRC flip=%x", core_crc & 0xffff);

	j = (unsigned char)((core_crc>>8) & 0x00FF);		//record CRC data
	*core_data_ptr = j;
	core_data_ptr++;

	j = (unsigned char)(core_crc & 0x00FF);
	*core_data_ptr = j;

	//spi compress portion
	core_comp_length = 0;
	for(i=0;i<core_length+2;i++)
	{
		l = core_spi_compress_data_temp[i];				//1st 4-nibble
		l = flipU32(l);
		r = (unsigned short int)((l>>16) & 0x0000FFFF);
		
		comp_nibble_ctrl = 0;
		if((r&0xF000)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x08;
		if((r&0x0F00)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x04;
		if((r&0x00F0)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x02;
		if((r&0x000F)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x01;
		comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_ctrl);
		if(comp_high_nibble) core_spi_compress_data_ptr++;
		if(comp_high_nibble) core_comp_length++;

		if((r&0xF000)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>12)&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x0F00)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>8)&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x00F0)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>4)&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x000F)!=0)
		{
			comp_nibble_temp = (unsigned char)(r&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		
		l = core_spi_compress_data_temp[i];				//2nd 4-nibble
		l = flipU32(l);
		r = (unsigned short int)(l & 0x0000FFFF);

		comp_nibble_ctrl = 0;
		if((r&0xF000)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x08;
		if((r&0x0F00)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x04;
		if((r&0x00F0)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x02;
		if((r&0x000F)!=0)
			comp_nibble_ctrl = comp_nibble_ctrl | 0x01;
		comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_ctrl);
		if(comp_high_nibble) core_spi_compress_data_ptr++;
		if(comp_high_nibble) core_comp_length++;

		if((r&0xF000)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>12)&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x0F00)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>8)&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x00F0)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>4)&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x000F)!=0)
		{
			comp_nibble_temp = (unsigned char)(r&0x000F);
			comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_spi_compress_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
	}

	/*
	l = core_spi_compress_data_temp[core_length+1];				//spi compress CRC
	l = flipU32(l);
	r = (unsigned short int)(l & 0x0000FFFF);
	
	comp_nibble_ctrl = 0;
	if((r&0xF000)!=0)
		comp_nibble_ctrl = comp_nibble_ctrl | 0x08;
	if((r&0x0F00)!=0)
		comp_nibble_ctrl = comp_nibble_ctrl | 0x04;
	if((r&0x00F0)!=0)
		comp_nibble_ctrl = comp_nibble_ctrl | 0x02;
	if((r&0x000F)!=0)
		comp_nibble_ctrl = comp_nibble_ctrl | 0x01;
	comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_ctrl);
	if(comp_high_nibble) core_spi_compress_data_ptr++;
	if(comp_high_nibble) core_comp_length++;

	if((r&0xF000)!=0)
	{
		comp_nibble_temp = (unsigned char)((r>>12)&0x000F);
		comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
		if(comp_high_nibble) core_spi_compress_data_ptr++;
		if(comp_high_nibble) core_comp_length++;
	}
	if((r&0x0F00)!=0)
	{
		comp_nibble_temp = (unsigned char)((r>>8)&0x000F);
		comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
		if(comp_high_nibble) core_spi_compress_data_ptr++;
		if(comp_high_nibble) core_comp_length++;
	}
	if((r&0x00F0)!=0)
	{
		comp_nibble_temp = (unsigned char)((r>>4)&0x000F);
		comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
		if(comp_high_nibble) core_spi_compress_data_ptr++;
		if(comp_high_nibble) core_comp_length++;
	}
	if((r&0x000F)!=0)
	{
		comp_nibble_temp = (unsigned char)(r&0x000F);
		comp_high_nibble = pack_nibble_data(core_spi_compress_data_ptr, comp_high_nibble, comp_nibble_temp);
		if(comp_high_nibble) core_spi_compress_data_ptr++;
		if(comp_high_nibble) core_comp_length++;
	}
	*/

	printf("\n    core CRC = 0x%x (0x%x) of %i bits", (flipU16(core_crc) & 0xffff), core_crc & 0xffff, ((core_length)*32)+32+16);
	printf("\n    ... Core data generation successful");
	
}

void core_data_output(void)
{	
	//core Frame
	debugmsg("\n    compare  ");
	if(core_1st==1)									//core data - SPI portion
	{
		fprintf(outputdat, "FF  // filler");		
		fputc(0x0A, outputdat);		
		fprintf(outputdat, "FF  // filler");		
		fputc(0x0A, outputdat);		
		fprintf(outputdat, "FF  // filler");		
		fputc(0x0A, outputdat);		
		fprintf(outputdat, "FF  // filler");		
		fputc(0x0A, outputdat);		
	}
	else if(core_switch == 1)
	{
		for(i=0;i<8;i++)
		{
			fprintf(outputdat, "FF  // filler to compensate switch region state");		
			fputc(0x0A, outputdat);	
		}
	}
	
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<((core_length*4)+8);i++)		
	{
		j = core_data[i];
		U8toASCIIHEX(j, hextxt_ptr);
		hextxt[2] = 0;
		//debugmsg("%i=%s ", i, hextxt_ptr);
		
		j = hextxt[0];
		fputc(j, outputdat);
		j = hextxt[1];
		fputc(j, outputdat);

		if(i == 0)
			fprintf(outputdat, "  //core frame start (start dword) %i", core_frame_no+1);
		else if(i == 4)
			fprintf(outputdat, "  //core data start");			
		else if(i == ((core_length*4)+4))
			fprintf(outputdat, "  //core frame Random number");		
		else if(i == ((core_length*4)+6))
			fprintf(outputdat, "  //core frame CRC signature");		
		else if(i == ((core_length*4)+7))
			fprintf(outputdat, "  //core frame end");		
		fputc(0x0A, outputdat);		
	}

	if(core_1st==1)									//core data - SPI Compress portion
	{
		fprintf(spi_comp_outputdat, "FF  // filler");		
		fputc(0x0A, spi_comp_outputdat);		
		fprintf(spi_comp_outputdat, "FF  // filler");		
		fputc(0x0A, spi_comp_outputdat);		
		fprintf(spi_comp_outputdat, "FF  // filler");		
		fputc(0x0A, spi_comp_outputdat);		
		fprintf(spi_comp_outputdat, "FF  // filler");		
		fputc(0x0A, spi_comp_outputdat);		
	}
	else if(core_switch == 1)
	{
		for(i=0;i<8;i++)
		{
			fprintf(spi_comp_outputdat, "FF  // filler to compensate switch region state");		
			fputc(0x0A, spi_comp_outputdat);	
		}
	}
	
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<core_comp_length;i++)		
	{
		j = core_spi_compress_data[i];
		U8toASCIIHEX(j, hextxt_ptr);
		hextxt[2] = 0;
		//debugmsg("%i=%s ", i, hextxt_ptr);
		
		j = hextxt[0];
		fputc(j, spi_comp_outputdat);
		j = hextxt[1];
		fputc(j, spi_comp_outputdat);

		if(i == 0)
			fprintf(spi_comp_outputdat, "  //core frame start (start dword) %i", core_frame_no+1);
		else if(i == 3)
			fprintf(spi_comp_outputdat, "  //core data start");			
		//else if(i == ((core_length*4)+4))
		//	fprintf(spi_comp_outputdat, "  //core frame Random number");		
		//else if(i == ((core_length*4)+6))
		//	fprintf(spi_comp_outputdat, "  //core frame CRC signature");		
		else if(i == (core_comp_length-1))
			fprintf(spi_comp_outputdat, "  //core frame end");		
		fputc(0x0A, spi_comp_outputdat);		
	}

	if(core_1st==1)									//core data - SDM portion
	{
		fprintf(sdm_core_outputdat, "FFFFFFFF");		
		second_dword = 1;
	}
	else if(core_switch == 1)
	{
		core_switch = 0;
		
		fprintf(sdm_core_outputdat, "FFFFFFFF");		
		if(second_dword == 0)
			second_dword = 1;
		else if(second_dword == 1)
		{
			fputc(0x0A, sdm_core_outputdat);		
			second_dword = 0;
		}

		fprintf(sdm_core_outputdat, "FFFFFFFF");		
		if(second_dword == 0)
			second_dword = 1;
		else if(second_dword == 1)
		{
			fputc(0x0A, sdm_core_outputdat);		
			second_dword = 0;
		}
	}
	
	hextxt_ptr = &hextxt[0];
	for(i=0;i<(core_length+2);i++)
	{
		l = core_sdm_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		fprintf(sdm_core_outputdat, hextxt_ptr);

		if(second_dword == 0)
			second_dword = 1;
		else if(second_dword == 1)
		{
			fputc(0x0A, sdm_core_outputdat);		
			second_dword = 0;
		}
	}
}

void core_din_output(void)
{
	hextxt_ptr = &hextxt[0];					//Core CFF
	for(i=0;i<((core_length*4)+8);i++)		
	{
		if(core_width==8)
		{
			j = core_data[i];
			U8toAsciiBin(j, hextxt_ptr);
			hextxt[8] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			for(l=0;l<8;l++)
			{
				j = hextxt[l];
				fputc(j, corecffdat);
			}

			if((i==((core_length*4)+8)-1)&&(core_seg == 32)&&(core_1st==1))	//ARIN
				fputc('1', corecffdat);
			else if((i==((core_length*4)+8)-2)&&(core_seg == 16)&&(core_1st==1))
				fputc('1', corecffdat);
			else if((i==((core_length*4)+8)-4)&&(core_seg == 8)&&(core_1st==1))
				fputc('1', corecffdat);
			else
				fputc('0', corecffdat);
			
			if((i==((core_length*4)+8)-1)&&(core_seg == 32))		//ARCE & DRBCE
				fprintf(corecffdat, "11");
			else if((i==((core_length*4)+8)-2)&&(core_seg == 16))
				fprintf(corecffdat, "11");
			else if((i==((core_length*4)+8)-4)&&(core_seg == 8))
				fprintf(corecffdat, "11");
			else
				fprintf(corecffdat, "00");

			fputc(0x0A, corecffdat);		
		}
		else if(core_width==16)
		{
			j = core_data[i];
			U8toAsciiBin(j, hextxt_ptr);
			hextxt[8] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			for(l=0;l<8;l++)
			{
				j = hextxt[l];
				fputc(j, corecffdat);
			}

			if(i%2!=0)
			{
				if((i==((core_length*4)+8)-1)&&(core_seg == 32)&&(core_1st==1))	//ARIN
					fputc('1', corecffdat);
				else if((i==((core_length*4)+8)-3)&&(core_seg == 16)&&(core_1st==1))
					fputc('1', corecffdat);
				else if((i==((core_length*4)+8)-7)&&(core_seg == 8)&&(core_1st==1))
					fputc('1', corecffdat);
				else
					fputc('0', corecffdat);
				
				if((i==((core_length*4)+8)-1)&&(core_seg == 32))		//ARCE & DRBCE
					fprintf(corecffdat, "11");
				else if((i==((core_length*4)+8)-3)&&(core_seg == 16))
					fprintf(corecffdat, "11");
				else if((i==((core_length*4)+8)-7)&&(core_seg == 8))
					fprintf(corecffdat, "11");
				else
					fprintf(corecffdat, "00");
				
				fputc(0x0A, corecffdat);
			}
		}		
	}
}
