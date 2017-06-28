#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BinBCD.h"
#include "string_ffb.h"
#include "aes_polar.h"

//#define	HAVE_HEADER

#define debugmsg		printf
#define EndofFile		0
#define AES_DATA_SIZE	10000

void data_file_setup(void);
void core_din_output(void);
void core_data_output(void);
void core_data_generation(void);
void iocsr_din_output(void);
void iocsr_data_generation(void);
void iocsr_data_output(void);
void create_aes_pof(void);

FILE *fsml;
FILE *spi_header_readptr;
FILE *spi_outputdat;
FILE *spi_comp_outputdat;
FILE *spi_aes_core_outputdat;
FILE *spi_aes_comp_core_outputdat;
FILE *sdm_header_readptr;
FILE *sdm_footer_readptr;
FILE *sdm_core_outputdat;
FILE *sdm_comp_outputdat;
FILE *sdm_aes_core_outputdat;
FILE *sdm_aes_comp_core_outputdat;
FILE *sdm128_core_outputdat;
FILE *sdm128_comp_outputdat;
FILE *sdm128_aes_core_outputdat;
FILE *sdm128_aes_comp_core_outputdat;
FILE *jtag_outputdat;
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
unsigned int start_filler_no;

char workbuf1[500];
char workbuf2[500];
char output_filename[30];
char spi_header_filename[30] = {0x0A, 0};
char sdm_header_filename[30] = {0x0A, 0};
char sdm_footer_filename[30] = {0x0A, 0};
char spi_output_comp_filename[30];
char sdm_output_comp_filename[30];
char sdm_iocsr_filename[30];
char sdm_core_filename[30];
char spi_aes_core_filename[30];
char spi_aes_comp_core_filename[30];
char sdm_aes_core_filename[30];
char sdm_aes_comp_core_filename[30];
char sdm128_core_filename[30];
char sdm128_output_comp_filename[30];
char sdm128_aes_core_filename[30];
char sdm128_aes_comp_core_filename[30];
char jtag_filename[30];
char iocsr_cff_filename[30];
char core_cff_filename[30];
char *singleSmlBuf_ptr;
char *extractPara_ptr;
char *output_filename_ptr;
char *spi_output_comp_filename_ptr;
char *sdm_output_comp_filename_ptr;
char *spi_header_filename_ptr;
char *sdm_header_filename_ptr;
char *sdm_footer_filename_ptr;
char *sdm_iocsr_filename_ptr;
char *sdm_core_filename_ptr;
char *spi_aes_core_filename_ptr;
char *spi_aes_comp_core_filename_ptr;
char *sdm_aes_core_filename_ptr;
char *sdm_aes_comp_core_filename_ptr;
char *sdm128_core_filename_ptr;
char *sdm128_output_comp_filename_ptr;
char *sdm128_aes_core_filename_ptr;
char *sdm128_aes_comp_core_filename_ptr;
char *jtag_filename_ptr;
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
char *core_aes_comp_data;
char *core_aes_comp_data_ptr;
unsigned int *core_sdm_compress_data;
unsigned int *core_sdm_compress_data_ptr;
unsigned int *core_spi_compress_data_temp;
unsigned int *core_spi_compress_data_temp_ptr;
unsigned int *core_sdm_data;
unsigned int *core_sdm_data_ptr;
unsigned short int core_crc;
unsigned int core_length, core_comp_length, core_sdm_comp_length;
unsigned int core_frame_no, core_data_hold, core_sdm_compress_data_passover;
char core_ft, core_sl, core_width, core_1st, second_dword, second_dword_sdm_comp, core_switch, core_switch_sdm_compress, switch_no;
char comp_high_nibble, comp_32_nibble, start_comp_output_odd, sdm128_out_track, sdm128_out_core_track, sdm128_out_comp_track, sdm128_out_aes_track, sdm128_out_aescomp_track;

unsigned int dword_data[10000];
unsigned char w,x,y,z;
unsigned int v;
unsigned int *dword_data_ptr;

aes_context Gctx;
unsigned char aes_key[17];
unsigned char aes_iv[17];
unsigned char aes_iv_2nd[17];
unsigned char aes_testiv[17];
unsigned char enc_counter[17];
unsigned char aes_testout[17];
unsigned char aes_testdata[17];
unsigned int *aes_core_data;
unsigned int *aes_core_data_ptr;
unsigned int *aes_encrypted_data;
unsigned int *aes_encrypted_data_ptr;
unsigned int aes_core_data_cnt;
unsigned int aes_core_data_32bit_cnt;
unsigned int *aes_comp_encrypted_data;
unsigned int *aes_comp_encrypted_data_ptr;
unsigned int *comment_list;
unsigned int *comment_list_ptr;
unsigned int *comment_list_comp;
unsigned int *comment_list_comp_ptr;
unsigned int postamble_count;

int main(void)
{
	printf("\n    ------------------------------------------------------------------------");
	printf("\n    Altera Image Generation Software Tool v5.3");
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
	spi_header_filename_ptr = &spi_header_filename[0];
	sdm_header_filename_ptr = &sdm_header_filename[0];
	sdm_footer_filename_ptr = &sdm_footer_filename[0];
	sdm_iocsr_filename_ptr = &sdm_iocsr_filename[0];
	sdm_core_filename_ptr = &sdm_core_filename[0];
	spi_aes_core_filename_ptr = &spi_aes_core_filename[0];
	spi_aes_comp_core_filename_ptr = &spi_aes_comp_core_filename[0];
	sdm_aes_core_filename_ptr = &sdm_aes_core_filename[0];
	sdm_aes_comp_core_filename_ptr = &sdm_aes_comp_core_filename[0];
	iocsr_cff_filename_ptr = &iocsr_cff_filename[0];
	core_cff_filename_ptr = &core_cff_filename[0];
	spi_output_comp_filename_ptr = &spi_output_comp_filename[0];
	sdm_output_comp_filename_ptr = &sdm_output_comp_filename[0];
	sdm128_core_filename_ptr = &sdm128_core_filename[0];
	sdm128_output_comp_filename_ptr = &sdm128_output_comp_filename[0];
	sdm128_aes_core_filename_ptr = &sdm128_aes_core_filename[0];
	sdm128_aes_comp_core_filename_ptr = &sdm128_aes_comp_core_filename[0];
	jtag_filename_ptr = &jtag_filename[0];
	
	aes_core_data = malloc(AES_DATA_SIZE);							//data malloc
	aes_core_data_ptr = aes_core_data;
	aes_core_data_cnt = 0;
	
	aes_encrypted_data = malloc(AES_DATA_SIZE);							//data malloc
	aes_encrypted_data_ptr = aes_encrypted_data;
	
	core_aes_comp_data = malloc(AES_DATA_SIZE);							//data malloc
	core_aes_comp_data_ptr = core_aes_comp_data;

	aes_comp_encrypted_data	= malloc(AES_DATA_SIZE);							//data malloc
	aes_comp_encrypted_data_ptr = aes_comp_encrypted_data;
	
	second_dword = 0;
	core_switch = 0;
	core_switch_sdm_compress = 0;
	//comp_high_nibble = 1;
	comp_32_nibble = 0;
	sdm128_out_track = 0;

	while(1)
	{
		singleSmlBuf_ptr = &workbuf1[0];
		extractPara_ptr = &workbuf2[0];
		smlIteration = extractString_BeginEnd_ptrSpecify('<','>', smlIteration, temp, singleSmlBuf_ptr);
		if(smlIteration == 0) 
			break;
		
		//****** General Setting
		if(findString("TESTING", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("TESTING=", ',', 1, singleSmlBuf_ptr, extractPara_ptr);
			if(compareString("ZB8", extractPara_ptr)==SAME)
				printf("    ZB8 device, ");
			else
				printf("    not ZB8 device, ");
			
			getElementString("MARKING=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			j = AsciiDec2Bin(extractPara_ptr);	
		}
		else if(findString("VERSION", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("VERSION=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			if(compareString("5.3", extractPara_ptr)==SAME)
				printf("\n    ... software version verified");
			else
			{
				printf("    version not match... exit..");
				break;
			}
		}
		//****** FileName Setting
		#ifdef	HAVE_HEADER
		else if(findString("SPI_HEADER", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SPI_HEADER=", ',', 0, singleSmlBuf_ptr, spi_header_filename_ptr);
			//debugmsg("\n    SDM HEADER FILENAME = %s", spi_header_filename_ptr);
		}		
		else if(findString("SDM_HEADER", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_HEADER=", ',', 0, singleSmlBuf_ptr, sdm_header_filename_ptr);
			//debugmsg("\n    SDM HEADER FILENAME = %s", sdm_header_filename_ptr);
		}		
		else if(findString("SDM_FOOTER", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_FOOTER=", ',', 0, singleSmlBuf_ptr, sdm_footer_filename_ptr);
			//debugmsg("\n    SDM FOOTER FILENAME = %s", sdm_footer_filename_ptr);
		}				
		#endif
		else if(findString("SPI_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SPI_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, output_filename_ptr);
			//debugmsg("\n    SPI OUTPUT FILENAME = %s", output_filename_ptr);
		}		
		else if(findString("SPI_OUTPUT_COMP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SPI_OUTPUT_COMP_FILE=", ',', 0, singleSmlBuf_ptr, spi_output_comp_filename_ptr);
			//debugmsg("\n    SPI COMPRESS OUTPUT FILENAME = %s", spi_output_comp_filename_ptr);
		}		
		else if(findString("SDM_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, sdm_core_filename_ptr);
			//debugmsg("\n    SDM OUTPUT FILENAME = %s", sdm_core_filename_ptr);
		}
		else if(findString("SDM_OUTPUT_COMP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_OUTPUT_COMP_FILE=", ',', 0, singleSmlBuf_ptr, sdm_output_comp_filename_ptr);
			//debugmsg("\n    SDM COMPRESS OUTPUT FILENAME = %s", sdm_output_comp_filename_ptr);
		}
		else if(findString("SPI_AES_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SPI_AES_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, spi_aes_core_filename_ptr);
			//debugmsg("\n    SPI OUTPUT FILENAME = %s", spi_aes_core_filename_ptr);
		}
		else if(findString("SPI_AES_COMP_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SPI_AES_COMP_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, spi_aes_comp_core_filename_ptr);
			//debugmsg("\n    SDM OUTPUT FILENAME = %s", spi_aes_comp_core_filename_ptr);
		}
		else if(findString("SDM_AES_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_AES_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, sdm_aes_core_filename_ptr);
			//debugmsg("\n    SDM OUTPUT FILENAME = %s", sdm_aes_core_filename_ptr);
		}
		else if(findString("SDM_AES_COMP_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM_AES_COMP_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, sdm_aes_comp_core_filename_ptr);
			//debugmsg("\n    SDM OUTPUT FILENAME = %s", sdm_aes_comp_core_filename_ptr);
		}
		else if(findString("SDM128_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM128_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, sdm128_core_filename_ptr);
		}
		else if(findString("SDM128_OUTPUT_COMP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM128_OUTPUT_COMP_FILE=", ',', 0, singleSmlBuf_ptr, sdm128_output_comp_filename_ptr);
		}
		else if(findString("SDM128_AES_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM128_AES_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, sdm128_aes_core_filename_ptr);
		}
		else if(findString("SDM128_AES_COMP_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("SDM128_AES_COMP_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, sdm128_aes_comp_core_filename_ptr);
		}
		else if(findString("JTAG_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("JTAG_OUTPUT_FILE=", ',', 0, singleSmlBuf_ptr, jtag_filename_ptr);
			//debugmsg("\n    JTAG UNCOMPRESSED OUTPUT FILENAME = %s", jtag_filename_ptr);
		}
		else if(findString("IOCSR_EXP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("IOCSR_EXP_FILE=", ',', 0, singleSmlBuf_ptr, iocsr_cff_filename_ptr);
			//debugmsg("\n    IOCSR EXPECTED OUTPUT FILENAME = %s", iocsr_cff_filename_ptr);
		}
		else if(findString("CORE_EXP_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("CORE_EXP_FILE=", ',', 0, singleSmlBuf_ptr, core_cff_filename_ptr);
			//debugmsg("\n    CORE EXPECTED OUTPUT FILENAME = %s", core_cff_filename_ptr);
		}
		else if(findString("CREATE_OUTPUT_FILE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			data_file_setup();
		}
		//***** AES
		else if(findString("AES_KEY", singleSmlBuf_ptr)!=NOTFOUND)
		{
			aes_key[16] = 0;
			getElementString("AES_KEY=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			//debugmsg("\nAES_Key ");
			for(i=0;i<16;i++)
			{
				j = ASCIIHEX2BinByte(extractPara_ptr);
				//debugmsg("%x-", j & 0xff);
				aes_key[i] = j;
				extractPara_ptr++;
				extractPara_ptr++;
				//debugmsg("%x ",aes_key[i] & 0xff);
			}
			//debugmsg("\nAES_Key(rev) ");
			for(i=0;i<8;i++)		//128-bit-block reverse in byte unit
			{
				j = aes_key[i];
				aes_key[i] = aes_key[15-i];
				aes_key[15-i] = j;
				//debugmsg("%x ",aes_key[i] & 0xff);
			}
		}
		else if(findString("AES_IV", singleSmlBuf_ptr)!=NOTFOUND)
		{
			aes_iv[8] = 0;
			getElementString("AES_IV=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			//debugmsg("\nAES IV = ");
			for(i=0;i<8;i++)
			{
				j = ASCIIHEX2BinByte(extractPara_ptr);
				aes_iv[i] = j;
				extractPara_ptr++;
				extractPara_ptr++;
				//debugmsg("%x ",aes_iv[i] & 0xff);
			}
		}
		else if(findString("AES_TESTIV", singleSmlBuf_ptr)!=NOTFOUND)
		{
			debugmsg("\n\nAES Encryption Test:\nKey = ");
			for(i=0;i<16;i++)
				debugmsg("%x",aes_key[i] & 0xff);
		
			aes_testiv[16] = 0;
			getElementString("AES_TESTIV=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			debugmsg("\nIV = ");
			for(i=0;i<16;i++)
			{
				j = ASCIIHEX2BinByte(extractPara_ptr);
				aes_testiv[i] = j;
				extractPara_ptr++;
				extractPara_ptr++;
				debugmsg("%x",aes_testiv[i] & 0xff);
			}
		}
		else if(findString("AES_TESTDATA", singleSmlBuf_ptr)!=NOTFOUND)
		{
			aes_testdata[16] = 0;
			getElementString("AES_TESTDATA=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			debugmsg("\nTESTDATA = ");
			for(i=0;i<16;i++)
			{
				j = ASCIIHEX2BinByte(extractPara_ptr);
				aes_testdata[i] = j;
				extractPara_ptr++;
				extractPara_ptr++;
				debugmsg("%x",aes_testdata[i] & 0xff);
			}

			aes_setkey_enc(&Gctx, aes_key, 128);
			aes_crypt_ecb(&Gctx, AES_ENCRYPT, aes_testiv, enc_counter);
			debugmsg("\nCOUNTER OUTPUT = ");
			for(i=0;i<16;i++)
				debugmsg("%x",enc_counter[i] & 0xff);
			
			debugmsg("\nXORED OUTPUT = ");
			for(i=0;i<16;i++)
			{
				aes_testdata[i] = aes_testdata[i] ^ enc_counter[i];
				debugmsg("%x",aes_testdata[i] & 0xff);
			}
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
			//debugmsg("\n    ...iocsr output complete");
		}
		//CORE
		else if(findString("START_FILLER", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("START_FILLER=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			start_filler_no = AsciiDec2Bin(extractPara_ptr);	
			/*for(i=0;i<start_filler_no;i++)		//for aes
			{
				*aes_core_data_ptr = 0xFFFFFFFF;
				aes_core_data_ptr++;
				aes_core_data_cnt++;
			}*/
			printf("\n    ...number of filler = %i", start_filler_no);
		}
		else if(findString("CORE_FRAME", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("CORE_FRAME=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			core_frame_no = AsciiDec2Bin(extractPara_ptr);	
			if(core_frame_no > 60)
			{
				printf("\n    error... too many core frames");
				exit(0);
			}

			comment_list = malloc(core_frame_no*4);
			comment_list_ptr = comment_list;
			
			comment_list_comp = malloc(core_frame_no*4);
			comment_list_comp_ptr = comment_list_comp;

			core_frame = core_frame_no;
			printf("\n    ...Total core frame number = %i", core_frame_no);
		}
		else if(findString("CORE_SWITCH", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("CORE_SWITCH=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			switch_no = AsciiDec2Bin(extractPara_ptr);	
			getElementString("CORE_SWITCH=", ',', 1, singleSmlBuf_ptr, extractPara_ptr);
			core_width = AsciiDec2Bin(extractPara_ptr);	
			//debugmsg("swtich no = %d", switch_no);
			core_switch = 1;
			core_switch_sdm_compress = 1;
			comp_high_nibble = 1;
			start_comp_output_odd = 0;
			//comp_32_nibble = 0;

			*comment_list_ptr = aes_core_data_cnt;				//mark frame start commenct location
			comment_list_ptr++;
			

			for(i=0;i<switch_no;i++)			//for aes
			{
				*aes_core_data_ptr = 0xFFFFFFFF;
				aes_core_data_ptr++;
				aes_core_data_cnt++;
			}

			for(i=0;i<switch_no;i++)
			{
				if(core_width==8)
				{
					fprintf(corecffdat, "11111111000");			//to compensate the RTL
					fputc(0x0A, corecffdat);
					fprintf(corecffdat, "11111111000");			//to compensate the RTL
					fputc(0x0A, corecffdat);			
					fprintf(corecffdat, "11111111000");			//to compensate the RTL
					fputc(0x0A, corecffdat);
					fprintf(corecffdat, "11111111000");			//to compensate the RTL
					fputc(0x0A, corecffdat);
				}
				else if(core_width==16)
				{
					fprintf(corecffdat, "1111111111111111000");			//to compensate the RTL
					fputc(0x0A, corecffdat);
					fprintf(corecffdat, "1111111111111111000");			//to compensate the RTL
					fputc(0x0A, corecffdat);			
				}
				else if(core_width==32)
				{
					fprintf(corecffdat, "11111111111111111111111111111111000");			//to compensate the RTL
					fputc(0x0A, corecffdat);
				}
			}
		}
		else if((findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)&&(core_frame_no != 0))
		{
			if(core_frame_no == core_frame)
			{
				core_1st = 1;
				start_comp_output_odd = 0;
				comp_high_nibble = 1;
			}
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
				core_sl = core_length + 1;
				printf("\n    Core frame length = %i", core_sl-1);
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
			//if(findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			//{
			//	getElementString("CORE_DATA=", ',', 4, singleSmlBuf_ptr, extractPara_ptr);	//SL = size of frame
			//	core_sl = ASCIIHEX2BinByte(extractPara_ptr);				
			//	printf("\n    CORE Frame Length = 0x%x", core_sl & 0xff);
			//}
			if(findString("CORE_DATA", singleSmlBuf_ptr)!=NOTFOUND)
			{
				getElementString("CORE_DATA=", ',', 4, singleSmlBuf_ptr, extractPara_ptr);	//SL = size of frame
				core_width = AsciiDec2Bin(extractPara_ptr);				
				printf("\n    CORE Segmentation = %i", core_width & 0xff);
			}
			
			core_data_generation();
			core_data_output();
			core_din_output();
			//debugmsg("\n    ...core output complete");
		}
		else if(findString("POST_AMBLE", singleSmlBuf_ptr)!=NOTFOUND)
		{
			getElementString("POST_AMBLE=", ',', 0, singleSmlBuf_ptr, extractPara_ptr);
			postamble_count = AsciiDec2Bin(extractPara_ptr);	
			
			for(i=0;i<(postamble_count*4);i++)
			{
				fprintf(spi_outputdat, "FF");		
				if(i==0)
					fprintf(spi_outputdat, "  //post-amble");
				fputc(0x0A, spi_outputdat);		
			}

			for(i=0;i<postamble_count;i++)
			{
				if(second_dword == 0)
				{
					fprintf(sdm_core_outputdat, "FFFFFFFF");	
					second_dword = 1;
				}
				else
				{
					fprintf(sdm_core_outputdat, "FFFFFFFF    //post-amble");	
					fputc(0x0A, sdm_core_outputdat);		
					second_dword = 0;
				}
			}
			if(second_dword != 0)
			{
				fprintf(sdm_core_outputdat, "FFFFFFFF");	
				fputc(0x0A, sdm_core_outputdat);		
				second_dword = 0;
			}
			
			for(i=0;i<postamble_count;i++)
			{
				fprintf(jtag_outputdat, "FFFF");		
				if(i==0)
					fprintf(jtag_outputdat, "  //post-amble");
				fputc(0x0A, jtag_outputdat);		

				fprintf(jtag_outputdat, "FFFF");		
				fputc(0x0A, jtag_outputdat);		
			}
		}
		else if(findString("CREATE_POF", singleSmlBuf_ptr)!=NOTFOUND)
		{
			create_aes_pof();
		}
	}

	printf("\n    ------------------------------------end---------------------------------\n");

	fclose(spi_outputdat);
	fclose(spi_comp_outputdat);
	fclose(sdm_core_outputdat);
	fclose(sdm_comp_outputdat);
	fclose(spi_aes_core_outputdat);
	fclose(spi_aes_comp_core_outputdat);
	fclose(sdm_aes_core_outputdat);
	fclose(sdm_aes_comp_core_outputdat);
	fclose(jtag_outputdat);
	fclose(iocsrcffdat);
	fclose(corecffdat);
	#ifdef	HAVE_HEADER
	fclose(spi_header_readptr);
	fclose(sdm_header_readptr);
	fclose(sdm_footer_readptr);
	#endif
	/*
	free(aes_core_data);
	free(aes_encrypted_data);
	free(iocsr_data);
	free(iocsr_sdm_data);
	free(core_data);
	free(core_sdm_data);
	free(core_aes_comp_data);
	free(aes_comp_encrypted_data);
	free(core_spi_compress_data_temp);
	free(core_sdm_compress_data);
	free(sml_ptr);
	free(comment_list);
	free(comment_list_comp);
	*/
	return 0;
}

void data_file_setup(void)	
{
	#ifdef	HAVE_HEADER
	spi_header_readptr = fopen(spi_header_filename_ptr, "r");	//open SPI header file
	if ( spi_header_readptr == NULL)
	{
		printf("\n    No SPI HEADER file specified\n");
		exit(0);
	}
	else
		printf("\n    SPI HEADER file created successfully");		

	sdm_header_readptr = fopen(sdm_header_filename_ptr, "r");			//open SDM header file
	if ( sdm_header_readptr == NULL)
	{
		printf("\n    No SDM HEADER file specified\n");
		exit(0);
	}
	else
		printf("\n    SDM HEADER file created successfully");		

	sdm_footer_readptr = fopen(sdm_footer_filename_ptr, "r");			//open SDM footer file
	if ( sdm_footer_readptr == NULL)
	{
		printf("\n    NO SDM footer file specified\n");
		exit(0);
	}
	else
		printf("\n    SDM footer file created successfully");		
	#endif

	spi_outputdat = fopen(output_filename_ptr, "w");			//create SPI POF Image file
	if (spi_outputdat == NULL)
	{
		printf("\n    SPI POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SPI POF image file created successfully");

	spi_comp_outputdat = fopen(spi_output_comp_filename_ptr, "w");		//create SPI compress POF Image file
	if (spi_comp_outputdat == NULL)
	{
		printf("\n    SPI Compress POF image file creation error\n");
		exit(0);
	}
	else
	{
		#ifdef	HAVE_HEADER
		while ((j=fgetc(spi_header_readptr)) != EOF)
			fputc(j, spi_comp_outputdat);
		fputc(0x0A, spi_comp_outputdat);
		#endif
		printf("\n    SPI Compress POF image file created successfully");
	}

	sdm_core_outputdat = fopen(sdm_core_filename_ptr, "w");			//create SDM POF Image file
	if (sdm_core_outputdat == NULL)
	{
		printf("\n    SDM CORE POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SDM CORE POF image file created successfully");		

	sdm_comp_outputdat = fopen(sdm_output_comp_filename_ptr, "w");			//create SDM compress POF Image file
	if (sdm_comp_outputdat == NULL)
	{
		printf("\n    SDM CORE POF image file creation error\n");
		exit(0);
	}
	else
	{
		#ifdef	HAVE_HEADER
		while ((j=fgetc(sdm_header_readptr)) != EOF)
			fputc(j, sdm_comp_outputdat);
		fputc(0x0A, sdm_comp_outputdat);
		#endif
		printf("\n    SDM CORE POF image file created successfully");		
	}

	spi_aes_core_outputdat = fopen(spi_aes_core_filename_ptr, "w");			//create SPI AES POF Image file
	if (spi_aes_core_outputdat == NULL)
	{
		printf("\n    SPI AES CORE POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SPI AES CORE POF image file created successfully");		

	spi_aes_comp_core_outputdat = fopen(spi_aes_comp_core_filename_ptr, "w");			//create SPI AES Compression POF Image file
	if (spi_aes_comp_core_outputdat == NULL)
	{
		printf("\n    SPI AES Compression CORE POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SPI AES compression CORE POF image file created successfully");		

	sdm_aes_core_outputdat = fopen(sdm_aes_core_filename_ptr, "w");			//create SDM AES POF Image file
	if (sdm_aes_core_outputdat == NULL)
	{
		printf("\n    SDM AES CORE POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SDM AES CORE POF image file created successfully");

	sdm_aes_comp_core_outputdat = fopen(sdm_aes_comp_core_filename_ptr, "w");	//create SDM AES COMP POF Image file
	if (sdm_aes_comp_core_outputdat == NULL)
	{
		printf("\n    SDM AES CORE Compression POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SDM AES CORE Compression POF image file created successfully");

	sdm128_core_outputdat = fopen(sdm128_core_filename_ptr, "w");			//create SDM 128 POF Image file
	if (sdm128_core_outputdat == NULL)
	{
		printf("\n    SDM 128 CORE POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SDM 128 CORE POF image file created successfully");		

	sdm128_comp_outputdat = fopen(sdm128_output_comp_filename_ptr, "w");			//create SDM 128 compress POF Image file
	if (sdm128_comp_outputdat == NULL)
	{
		printf("\n    SDM 128 CORE POF image file creation error\n");
		exit(0);
	}
	else
	{
		printf("\n    SDM 128 CORE POF image file created successfully");		
	}

	sdm128_aes_core_outputdat = fopen(sdm128_aes_core_filename_ptr, "w");			//create SDM 128 AES POF Image file
	if (sdm128_aes_core_outputdat == NULL)
	{
		printf("\n    SDM 128 AES CORE POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SDM 128 AES CORE POF image file created successfully");

	sdm128_aes_comp_core_outputdat = fopen(sdm128_aes_comp_core_filename_ptr, "w");	//create SDM 128 AES COMP POF Image file
	if (sdm128_aes_comp_core_outputdat == NULL)
	{
		printf("\n    SDM 128 AES CORE Compression POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    SDM 128 AES CORE Compression POF image file created successfully");

	jtag_outputdat = fopen(jtag_filename_ptr, "w");	//create SDM AES COMP POF Image file
	if (jtag_outputdat == NULL)
	{
		printf("\n    JTAG POF image file creation error\n");
		exit(0);
	}
	else
		printf("\n    JTAG POF image file created successfully");

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
	//debugmsg("\n    ... IOCSR data generation\n");		//IOCSR data generation

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
		*iocsr_data_ptr = (char)(l >> 24)&0x000000ff;
		iocsr_data_ptr++;
		*iocsr_data_ptr = (char)(l >> 16)&0x000000ff;
		iocsr_data_ptr++;
		*iocsr_data_ptr = (char)(l >> 8)&0x000000ff;
		iocsr_data_ptr++;
		*iocsr_data_ptr = (char)(l & 0x000000ff);
		iocsr_data_ptr++;
	}

	for(i=0;i<(ioscr_pad*4);i++)		//IOCSR pad generation (SPI)
	{
		*iocsr_data_ptr = 0xFF;
		iocsr_data_ptr++;
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
	iocsr_data_ptr++;
	*iocsr_data_ptr = random_num & 0x00ff;
	iocsr_data_ptr++;
	
	//debugmsg("\n    DWORD=%i\n", (ioscr_length+ioscr_pad)+1);	//************ compute CRC
	v = 0;									
	dword_data[0] = 0;
	for(i=0;i<(ioscr_length+ioscr_pad);i++)			//4*U8 -> U32(flip)
	{
		w = iocsr_data[v];
		x = iocsr_data[v+1];
		y = iocsr_data[v+2];
		z = iocsr_data[v+3];
		dword_data[i] = (unsigned int)((w<<24) | (x<<16) | (y<<8) | z);
		dword_data[i] = flipU32(dword_data[i]);
		v+=4;
		dword_data[i+1] = 0;
	}
	w = iocsr_data[v];								
	x = iocsr_data[v+1];
	r = (unsigned short int)((w<<8) | x);
	r = flipU16(r);
	dword_data[ioscr_length+ioscr_pad]   = r & 0x0000ffff;
	
	dword_data_ptr = &dword_data[0];
	iocsr_crc = calculate_crc_cnt_dword(0x0000ffff, dword_data_ptr, ((ioscr_length+ioscr_pad)*32)+16);
	
	*iocsr_sdm_data_ptr = *iocsr_sdm_data_ptr | ((unsigned int)((iocsr_crc << 16)& 0xFFFF0000));		//sdm portion
	
	iocsr_crc = flipU16(iocsr_crc);						//SPI portion
	
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
	//debugmsg("\n    ... IOCSR data output\n");		//IOCSR data generation
	
	iocsr_data_ptr = iocsr_data;			
	iocsr_sdm_data_ptr = iocsr_sdm_data;
	//debugmsg("\n    generate ");
	for(i=0;i<((ioscr_length*4)+4);i++)		
	{
		j = *iocsr_data_ptr;
		iocsr_data_ptr++;
	}
	
	//IOCSR Frame
	for(i=0;i<4;i++)					//start - SPI portion
	{
		fputc('0', spi_outputdat);
		fputc('0', spi_outputdat);
		if(i == 0)
			fprintf(spi_outputdat, "  //iocsr frame start (start dword) %i", iocsr_frame_no+1);
		fputc(0x0A, spi_outputdat);
	}

	for(i=0;i<4;i++)					//start - SPI Compress portion
	{
		fputc('0', spi_comp_outputdat);
		fputc('0', spi_comp_outputdat);
		if(i == 0)
			fprintf(spi_comp_outputdat, "  //iocsr frame start (start dword) %i", iocsr_frame_no+1);
		fputc(0x0A, spi_comp_outputdat);
	}

	for(i=0;i<2;i++)					//start - JTAG portion
	{
		fprintf(jtag_outputdat, "0000");
		if(i == 0)
			fprintf(jtag_outputdat, "  //iocsr frame start (start dword) %i", iocsr_frame_no+1);
		fputc(0x0A, jtag_outputdat);
	}
	
	fprintf(sdm_core_outputdat, "00000000");	//start - SDM portion
	fprintf(sdm_comp_outputdat, "00000000");	//start - SDM compression portion
	fprintf(sdm_aes_core_outputdat, "00000000");	//start - SDM aes portion
	fprintf(sdm_aes_comp_core_outputdat, "00000000");	//start - SDM aes compression portion

	for(i=0;i<4;i++)					//start - SPI AES portion
	{
		fputc('0', spi_aes_core_outputdat);
		fputc('0', spi_aes_core_outputdat);
		if(i == 0)
			fprintf(spi_aes_core_outputdat, "  //iocsr frame start (start dword) %i", iocsr_frame_no+1);
		fputc(0x0A, spi_aes_core_outputdat);
	}

	for(i=0;i<4;i++)					//start - SPI AES Compression portion
	{
		fputc('0', spi_aes_comp_core_outputdat);
		fputc('0', spi_aes_comp_core_outputdat);
		if(i == 0)
			fprintf(spi_aes_comp_core_outputdat, "  //iocsr frame start (start dword) %i", iocsr_frame_no+1);
		fputc(0x0A, spi_aes_comp_core_outputdat);
	}

	hextxt_ptr = &hextxt[0];					//IOCSR data - SPI portion
	for(i=0;i<(((ioscr_length+ioscr_pad)*4)+4);i++)		
	{
		j = iocsr_data[i];
		U8toASCIIHEX(j, hextxt_ptr);
		hextxt[2] = 0;
		
		j = hextxt[0];
		fputc(j, spi_outputdat);
		fputc(j, spi_comp_outputdat);		
		j = hextxt[1];
		fputc(j, spi_outputdat);
		fputc(j, spi_comp_outputdat);

		if(i == 0)
		{
			fprintf(spi_outputdat, "  //iocsr data start");			
			fprintf(spi_comp_outputdat, "  //iocsr data start");			
		}
		else if((i == (ioscr_length*4))&&(ioscr_pad > 0))
		{
			fprintf(spi_outputdat, "  //iocsr padding");
			fprintf(spi_comp_outputdat, "  //iocsr padding");
		}
		else if(i == (((ioscr_length+ioscr_pad)*4)+2))
		{
			fprintf(spi_outputdat, "  //iocsr CRC signature");
			fprintf(spi_comp_outputdat, "  //iocsr CRC signature");
		}
		fputc(0x0A, spi_outputdat);	
		fputc(0x0A, spi_comp_outputdat);	
	}
	
	hextxt_ptr = &hextxt[0];							//IOCSR data - SDM portion
	for(i=0;i<(ioscr_length+ioscr_pad+1);i++)
	{
		l = iocsr_sdm_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		fprintf(sdm_core_outputdat, "%s", hextxt_ptr);
		fprintf(sdm_comp_outputdat, "%s", hextxt_ptr);
		fprintf(sdm_aes_core_outputdat, "%s", hextxt_ptr);
		fprintf(sdm_aes_comp_core_outputdat, "%s", hextxt_ptr);

		if(i==(ioscr_length+ioscr_pad))
		{
			fprintf(sdm_core_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);
			fprintf(sdm_comp_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);
			fprintf(sdm_aes_core_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);
			fprintf(sdm_aes_comp_core_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);

			fputc(0x0A, sdm_core_outputdat);
			fputc(0x0A, sdm_comp_outputdat);
			fputc(0x0A, sdm_aes_core_outputdat);
			fputc(0x0A, sdm_aes_comp_core_outputdat);
		}
		else if(i%2==0)
		{
			fputc(0x0A, sdm_core_outputdat);
			fputc(0x0A, sdm_comp_outputdat);
			fputc(0x0A, sdm_aes_core_outputdat);
			fputc(0x0A, sdm_aes_comp_core_outputdat);
		}
	}

	fprintf(sdm128_core_outputdat, "00000000");	//start - SDM128 portion							//IOCSR data - SDM 128 portion
	fprintf(sdm128_comp_outputdat, "00000000");	//start - SDM128 compression portion
	fprintf(sdm128_aes_core_outputdat, "00000000");	//start - SDM128 aes portion
	fprintf(sdm128_aes_comp_core_outputdat, "00000000");	//start - SDM128 aes compression portion
	sdm128_out_track++;

	hextxt_ptr = &hextxt[0];
	for(i=0;i<(ioscr_length+ioscr_pad+1);i++)
	{
		l = iocsr_sdm_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		fprintf(sdm128_core_outputdat, "%s", hextxt_ptr);
		fprintf(sdm128_comp_outputdat, "%s", hextxt_ptr);
		fprintf(sdm128_aes_core_outputdat, "%s", hextxt_ptr);
		fprintf(sdm128_aes_comp_core_outputdat, "%s", hextxt_ptr);
		sdm128_out_track++;

		if(sdm128_out_track == 4)
		{
			sdm128_out_track = 0;
			
			if((i<=(ioscr_length+ioscr_pad))&&(i>=(ioscr_length+ioscr_pad-3)))
			{
				fprintf(sdm128_core_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);
				fprintf(sdm128_comp_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);
				fprintf(sdm128_aes_core_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);
				fprintf(sdm128_aes_comp_core_outputdat, "    //end of IOCSR frame %i", iocsr_frame_no+1);
			}
			
			fputc(0x0A, sdm128_core_outputdat);
			fputc(0x0A, sdm128_comp_outputdat);
			fputc(0x0A, sdm128_aes_core_outputdat);
			fputc(0x0A, sdm128_aes_comp_core_outputdat);
		}
	}
	sdm128_out_core_track = sdm128_out_track;
	sdm128_out_comp_track = sdm128_out_track;
	sdm128_out_aes_track = sdm128_out_track;
	sdm128_out_aescomp_track = sdm128_out_track;

	hextxt_ptr = &hextxt[0];							//IOCSR data - JTAG portion
	for(i=0;i<(ioscr_length+ioscr_pad+1);i++)
	{
		l = iocsr_sdm_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		
		for(j=4;j<8;j++)
			fputc(hextxt[j], jtag_outputdat);
		fputc(0x0A, jtag_outputdat);

		for(j=0;j<4;j++)
			fputc(hextxt[j], jtag_outputdat);

		if(i==(ioscr_length+ioscr_pad))
		{
			fprintf(jtag_outputdat, "  //end of IOCSR frame %i", iocsr_frame_no+1);
			fputc(0x0A, jtag_outputdat);
		}
		else
			fputc(0x0A, jtag_outputdat);
	}

	hextxt_ptr = &hextxt[0];					//IOCSR data - SPI AES portion
	for(i=0;i<(((ioscr_length+ioscr_pad)*4)+4);i++)		
	{
		j = iocsr_data[i];
		U8toASCIIHEX(j, hextxt_ptr);
		hextxt[2] = 0;
		
		j = hextxt[0];
		fputc(j, spi_aes_core_outputdat);
		fputc(j, spi_aes_comp_core_outputdat);
		j = hextxt[1];
		fputc(j, spi_aes_core_outputdat);
		fputc(j, spi_aes_comp_core_outputdat);

		if(i == 0)
		{
			fprintf(spi_aes_core_outputdat, "  //iocsr data start");			
			fprintf(spi_aes_comp_core_outputdat, "  //iocsr data start");			
		}
		else if((i == (ioscr_length*4))&&(ioscr_pad > 0))
		{
			fprintf(spi_aes_core_outputdat, "  //iocsr padding");
			fprintf(spi_aes_comp_core_outputdat, "  //iocsr padding");
		}
		else if(i == (((ioscr_length+ioscr_pad)*4)+2))
		{
			fprintf(spi_aes_core_outputdat, "  //iocsr CRC signature");
			fprintf(spi_aes_comp_core_outputdat, "  //iocsr CRC signature");
		}
		fputc(0x0A, spi_aes_core_outputdat);	
		fputc(0x0A, spi_aes_comp_core_outputdat);	
	}
}

void iocsr_din_output(void)
{
	hextxt_ptr = &hextxt[0];					//IOCSR CFF
	for(i=0;i<(((ioscr_length+ioscr_pad)*4)+0);i++)		
	{
		j = iocsr_data[i];
		j = flipU8(j);
		U8toAsciiBin(j, hextxt_ptr);
		hextxt[8] = 0;
		
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

char pack_nibble_data_32bit(unsigned int *ptr, char nibble_status, char data)
{
	*ptr = *ptr | (data << (nibble_status * 4));
	nibble_status++;
	if(nibble_status >= 8)
		nibble_status = 0;
		
	return nibble_status;
}

void core_data_generation(void)	//*** Core data generation
{
	char comp_nibble_temp, comp_nibble_ctrl;

	//debugmsg("\n    -------- Core data --------\n");		//core data generation

	core_data = malloc((core_length*4)+9);							//data malloc
	core_data_ptr = core_data;
	core_spi_compress_data_temp = malloc((core_length*4)+9);
	core_spi_compress_data_temp_ptr = core_spi_compress_data_temp;
	core_spi_compress_data = malloc((core_length*4)*2);
	core_spi_compress_data_ptr = core_spi_compress_data;
	core_sdm_data = (unsigned int *) malloc((core_length+1)*4);
	core_sdm_data_ptr = core_sdm_data;
	//core_sdm_compress_data = malloc((core_length*4)*2);
	core_sdm_compress_data = malloc(500);
	core_sdm_compress_data_ptr = core_sdm_compress_data;
	if(core_1st==1)
		comp_32_nibble = 0;
	else
		*core_sdm_compress_data_ptr = core_sdm_compress_data_passover;

	*core_data_ptr = 0;		//start zero - spi portion
	core_data_ptr++;
	*core_data_ptr = 0;
	core_data_ptr++;

	r = 0;
	r = (unsigned short int)(((core_sl << 8)& 0xff00) | ((unsigned char)(core_ft)));
	
	*core_sdm_data_ptr = (unsigned int)(r<<16);		//start - sdm portion
	core_sdm_data_ptr++;

	*core_spi_compress_data_temp_ptr = (unsigned int)(r<<16);	//start - spi compression portion
	core_spi_compress_data_temp_ptr++;

	*comment_list_ptr = aes_core_data_cnt;				//mark frame start commenct location
	comment_list_ptr++;

	*aes_core_data_ptr = (unsigned int)(r<<16);		//start - aes portion
	aes_core_data_ptr++;
	aes_core_data_cnt++;
	
	r = flipU16(r);				//start - spi portion
	*core_data_ptr = (char)(r >> 8)&0x00ff;
	core_data_ptr++;
	*core_data_ptr = (char)(r & 0x00ff);
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
		
		*aes_core_data_ptr = l;		//aes portion
		aes_core_data_ptr++;
		aes_core_data_cnt++;
		
		//spi portion
		l = flipU32(l);				
		*core_data_ptr = (char)(l >> 24)&0x000000ff;
		core_data_ptr++;
		*core_data_ptr = (char)(l >> 16)&0x000000ff;
		core_data_ptr++;
		*core_data_ptr = (char)(l >> 8)&0x000000ff;
		core_data_ptr++;
		*core_data_ptr = (char)(l & 0x000000ff);
		core_data_ptr++;		
	}

	random_num = (unsigned short int)xor128();		//************ random number
	*core_sdm_data_ptr = (unsigned int)(random_num & 0x0000FFFF);		//sdm portion
	*core_spi_compress_data_temp_ptr = *core_spi_compress_data_temp_ptr | ((unsigned int)(random_num & 0x0000FFFF));	//spi compress portion
	*aes_core_data_ptr = (unsigned int)(random_num & 0x0000FFFF);		//aes portion
	
	random_num = flipU16(random_num);					//SPI portion
	*core_data_ptr = (random_num >> 8) & 0x00ff;
	core_data_ptr++;
	*core_data_ptr = random_num & 0x00ff;
	core_data_ptr++;
	
	//debugmsg("\n    DWORD=%i\n", core_length+1);	//************ compute CRC
	v = 0;
	dword_data[0] = 0;
	for(i=0;i<(core_length+1);i++)			//4*U8 -> U32(flip)
	{
		w = core_data[v];
		x = core_data[v+1];
		y = core_data[v+2];
		z = core_data[v+3];
		dword_data[i] = (unsigned int)((w<<24) | (x<<16) | (y<<8) | z);
		dword_data[i] = flipU32(dword_data[i]);
		v+=4;
		dword_data[i+1] = 0;
	}
	w = core_data[v];								
	x = core_data[v+1];
	r = (unsigned short int)((w<<8) | x);
	r = flipU16(r);
	dword_data[core_length+1]   = r & 0x0000ffff;
	
	dword_data_ptr = &dword_data[0];
	core_crc = calculate_crc_cnt_dword(0x0000ffff, dword_data_ptr, ((core_length+1)*32)+16);

	*core_sdm_data_ptr = *core_sdm_data_ptr | ((unsigned int)((core_crc << 16)& 0xFFFF0000));		//sdm portion
	*core_spi_compress_data_temp_ptr = *core_spi_compress_data_temp_ptr | ((unsigned int)((core_crc << 16)& 0xFFFF0000));		//spi compress portion
	*aes_core_data_ptr = *aes_core_data_ptr | ((unsigned int)((core_crc << 16)& 0xFFFF0000));		//aes portion
	aes_core_data_ptr++;
	aes_core_data_cnt++;
	
	core_crc = flipU16(core_crc);	//spi portion

	j = (unsigned char)((core_crc>>8) & 0x00FF);		//record CRC data
	*core_data_ptr = j;
	core_data_ptr++;

	j = (unsigned char)(core_crc & 0x00FF);
	*core_data_ptr = j;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ spi compress portion
	//debugmsg("\nCore data = ");
	//for(i=0;i<core_length+2;i++)
	//{
	//	debugmsg("%x ",core_spi_compress_data_temp[i]);
	//}
	//debugmsg("\ncontrol = ");
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
		//debugmsg("%x ",comp_nibble_ctrl);
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
		//debugmsg("%x ",comp_nibble_ctrl);
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
	//debugmsg("core_comp_length = %d\n", core_comp_length);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~sdm compress portion
	core_sdm_comp_length = 0;

	if(core_switch_sdm_compress==1)
	{
		core_switch_sdm_compress = 0;
		for(i=0;i<(switch_no*10);i++)
		{
			comp_nibble_temp = 0x0F;
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i)", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
	}
	for(i=0;i<core_length+2;i++)
	{
		l = core_spi_compress_data_temp[i];				//1st 4-nibble
		//debugmsg("data=%x\n", l);
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
		comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_ctrl);
		//debugmsg("ctrl=%x(%i), ", comp_nibble_ctrl & 0x0f, comp_32_nibble);
		if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
		if(comp_32_nibble==0) core_sdm_comp_length++;

		if((r&0x000F)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>0)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i), ", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		if((r&0x00F0)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>4)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i), ", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		if((r&0X0F00)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>8)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i), ", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		if((r&0XF000)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>12)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i); ", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		
		l = core_spi_compress_data_temp[i];				//2nd 4-nibble
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
		comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_ctrl);
		//debugmsg("ctrl=%x(%i), ", comp_nibble_ctrl & 0x0f, comp_32_nibble);
		if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
		if(comp_32_nibble==0) core_sdm_comp_length++;

		if((r&0X000F)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>0)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i), ", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		if((r&0X00F0)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>4)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i), ", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		if((r&0X0F00)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>8)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i), ", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		if((r&0XF000)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>12)&0x000F);
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			//debugmsg("%x(%i)", comp_nibble_temp & 0x0f, comp_32_nibble);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
		}
		//debugmsg("\n");
	}
	if((comp_32_nibble!=0)&&(core_frame_no==0))	//to pad the last frame with "F"
	{
		while(1)
		{
			comp_nibble_temp = 0x0F;
			comp_32_nibble = pack_nibble_data_32bit(core_sdm_compress_data_ptr, comp_32_nibble, comp_nibble_temp);
			if(comp_32_nibble==0) core_sdm_compress_data_ptr++;
			if(comp_32_nibble==0) core_sdm_comp_length++;
			if(comp_32_nibble==0) break;
		}
	}
	core_sdm_compress_data_passover = *core_sdm_compress_data_ptr;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end
	printf("\n    random number = 0x%x (0x%x)", (flipU16(random_num) & 0xffff), random_num & 0xffff);
	printf("\n    core CRC = 0x%x (0x%x) of %i bits", (flipU16(core_crc) & 0xffff), core_crc & 0xffff, ((core_length)*32)+32+16);
	printf("\n    ... Core data generation successful");
}

void core_data_output(void)
{	
	//core Frame
	//debugmsg("\n    compare  ");
	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~core data - SPI portion
	{
		for(i=0;i<(start_filler_no*4);i++)
		{
			fprintf(spi_outputdat, "FF  // filler");		
			fputc(0x0A, spi_outputdat);		
		}
	}
	else if(core_switch == 1)
	{
		for(i=0;i<(switch_no*4);i++)
		{
			fprintf(spi_outputdat, "FF  // filler to compensate switch region state");		
			fputc(0x0A, spi_outputdat);	
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
		fputc(j, spi_outputdat);
		j = hextxt[1];
		fputc(j, spi_outputdat);

		if(i == 0)
			fprintf(spi_outputdat, "  //core frame start (start dword) %i", core_frame_no+1);
		else if(i == 4)
			fprintf(spi_outputdat, "  //core data start");			
		else if(i == ((core_length*4)+4))
			fprintf(spi_outputdat, "  //core frame Random number");		
		else if(i == ((core_length*4)+6))
			fprintf(spi_outputdat, "  //core frame CRC signature");		
		else if(i == ((core_length*4)+7))
			fprintf(spi_outputdat, "  //core frame end");		
		fputc(0x0A, spi_outputdat);		
	}

	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~ core data - SPI Compress portion
	{
		for(i=0;i<(start_filler_no*4);i++)
		{
			fprintf(spi_comp_outputdat, "FF  // filler");		
			fputc(0x0A, spi_comp_outputdat);		
		}
	}
	/*else if(core_switch == 1)
	{
		for(i=0;i<(switch_no*4);i++)		//or x5??
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
		//debugmsg("%s ", &hextxt_ptr);
		
		if(start_comp_output_odd == 0)
		{
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
		else
		{
			j = hextxt[0];
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
			j = hextxt[1];
			fputc(j, spi_comp_outputdat);
		}		
	}
	if(comp_high_nibble==1)
	{
		start_comp_output_odd = 0;
	}
	else
	{
		start_comp_output_odd = 1;

		j = core_spi_compress_data[core_comp_length];
		U8toASCIIHEX(j, hextxt_ptr);
		hextxt[2] = 0;
		j = hextxt[0];
	}*/

	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ core data - SDM portion
	{
		for(i=0;i<start_filler_no;i++)
		{
			fprintf(sdm_core_outputdat, "FFFFFFFF");	
			fprintf(sdm_aes_core_outputdat, "FFFFFFFF");		
			fprintf(sdm_aes_comp_core_outputdat, "FFFFFFFF");		
			if(i%2 == 0)		//even
			{
				second_dword = 1;
			}
			else
			{
				second_dword = 0;
				fprintf(sdm_core_outputdat, "    //filler");		
				fputc(0x0A, sdm_core_outputdat);
				fprintf(sdm_aes_core_outputdat, "    //filler");		
				fputc(0x0A, sdm_aes_core_outputdat);
				fprintf(sdm_aes_comp_core_outputdat, "    //filler");		
				fputc(0x0A, sdm_aes_comp_core_outputdat);
			}
		}
	}
	else if(core_switch == 1)
	{
		//core_switch = 0;
		
		for(i=0;i<switch_no;i++)
		{
			fprintf(sdm_core_outputdat, "FFFFFFFF");		
			if(second_dword == 0)
				second_dword = 1;
			else if(second_dword == 1)
			{
				fprintf(sdm_core_outputdat, "    //switch region");
				fputc(0x0A, sdm_core_outputdat);		
				second_dword = 0;
			}
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
			if((i==0)||(i==1))
				fprintf(sdm_core_outputdat, "    //start of CORE data frame %i", core_frame_no+1);
			fputc(0x0A, sdm_core_outputdat);		
			second_dword = 0;
		}
	}

	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ core data - SDM 128 portion
	{
		for(i=0;i<start_filler_no;i++)
		{
			fprintf(sdm128_core_outputdat, "FFFFFFFF");	
			sdm128_out_core_track++;
			if(sdm128_out_core_track == 4)
			{
				sdm128_out_core_track = 0;
				fprintf(sdm128_core_outputdat, "    //filler");						
				fputc(0x0A, sdm128_core_outputdat);
			}
		}
		
		for(i=0;i<start_filler_no;i++)
		{
			fprintf(sdm128_aes_core_outputdat, "FFFFFFFF");	
			sdm128_out_aes_track++;
			if(sdm128_out_aes_track == 4)
			{
				sdm128_out_aes_track = 0;
				fprintf(sdm128_aes_core_outputdat, "    //filler");						
				fputc(0x0A, sdm128_aes_core_outputdat);
			}
		}
		
		for(i=0;i<start_filler_no;i++)
		{
			fprintf(sdm128_aes_comp_core_outputdat, "FFFFFFFF");	
			sdm128_out_aescomp_track++;
			if(sdm128_out_aescomp_track == 4)
			{
				sdm128_out_aescomp_track = 0;
				fprintf(sdm128_aes_comp_core_outputdat, "    //filler");						
				fputc(0x0A, sdm128_aes_comp_core_outputdat);
			}
		}
	}
	else if(core_switch == 1)
	{
		//core_switch = 0;
		for(i=0;i<switch_no;i++)
		{
			fprintf(sdm128_core_outputdat, "FFFFFFFF");		
			sdm128_out_core_track++;
			if(sdm128_out_core_track == 4)
			{
				sdm128_out_core_track = 0;
				fprintf(sdm128_core_outputdat, "    //switch region");						
				fputc(0x0A, sdm128_core_outputdat);
			}
		}
	}
	
	hextxt_ptr = &hextxt[0];
	for(i=0;i<(core_length+2);i++)
	{
		l = core_sdm_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		fprintf(sdm128_core_outputdat, hextxt_ptr);
		sdm128_out_core_track++;
		if(sdm128_out_core_track == 4)
		{
			sdm128_out_core_track = 0;
			fputc(0x0A, sdm128_core_outputdat);
		}
	}

	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ core data - SDM compression portion
	{
		for(i=0;i<start_filler_no;i++)
		{
			fprintf(sdm_comp_outputdat, "FFFFFFFF");		
			if(i%2 == 0)		//even
			{
				second_dword_sdm_comp = 1;
			}
			else
			{
				second_dword_sdm_comp = 0;
				fprintf(sdm_comp_outputdat, "    //filler");		
				fputc(0x0A, sdm_comp_outputdat);
			}
		}
	}
	
	//debugmsg("\n");
	hextxt_ptr = &hextxt[0];
	for(i=0;i<core_sdm_comp_length;i++)
	{
		l = core_sdm_compress_data[i];
		//debugmsg("%i->%x\n", i, l);
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		fprintf(sdm_comp_outputdat, hextxt_ptr);
		//debugmsg("%i->%s\n", i, hextxt_ptr);

		if(second_dword_sdm_comp == 0)
			second_dword_sdm_comp = 1;
		else if(second_dword_sdm_comp == 1)
		{
			if((i==0)||(i==1))
				fprintf(sdm_comp_outputdat, "    //start of CORE data frame %i", core_frame_no+1);
			fputc(0x0A, sdm_comp_outputdat);		
			second_dword_sdm_comp = 0;
		}
	}	

	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ core data - SDM 128 compression portion
	{
		for(i=0;i<start_filler_no;i++)
		{
			fprintf(sdm128_comp_outputdat, "FFFFFFFF");		
			sdm128_out_comp_track++;
			if(sdm128_out_comp_track == 4)
			{
				sdm128_out_comp_track = 0;
				fprintf(sdm128_comp_outputdat, "    //filler");						
				fputc(0x0A, sdm128_comp_outputdat);
			}
			
		}
	}
	
	hextxt_ptr = &hextxt[0];
	for(i=0;i<core_sdm_comp_length;i++)
	{
		l = core_sdm_compress_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		fprintf(sdm128_comp_outputdat, hextxt_ptr);
		sdm128_out_comp_track++;
		if(sdm128_out_comp_track == 4)
		{
			sdm128_out_comp_track = 0;
			fputc(0x0A, sdm128_comp_outputdat);
		}
	}	
	
	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ core data - SPI AES portion
	{
		for(i=0;i<(start_filler_no*4);i++)
		{
			fprintf(spi_aes_core_outputdat, "FF  // filler");		
			fputc(0x0A, spi_aes_core_outputdat);		
		}
	}

	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ core data - SPI AES Compression portion
	{
		for(i=0;i<(start_filler_no*4);i++)
		{
			fprintf(spi_aes_comp_core_outputdat, "FF  // filler");		
			fputc(0x0A, spi_aes_comp_core_outputdat);		
		}
	}

	if(core_1st==1)									//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ core data - JTAG portion
	{
		for(i=0;i<(start_filler_no*2);i++)
		{
			fprintf(jtag_outputdat, "FFFF");
			fputc(0x0A, jtag_outputdat);
			fprintf(jtag_outputdat, "FFFF  //filler");
			fputc(0x0A, jtag_outputdat);
		}
	}
	else if(core_switch == 1)
	{
		core_switch = 0;
		
		for(i=0;i<(switch_no*2);i++)
		{
			fprintf(jtag_outputdat, "FFFF");
			fputc(0x0A, jtag_outputdat);
			fprintf(jtag_outputdat, "FFFF");
			if(i == 0)
			{
				fprintf(jtag_outputdat, "    //switch region");
				fputc(0x0A, jtag_outputdat);		
			}
			else
				fputc(0x0A, jtag_outputdat);		
		}
	}
	
	hextxt_ptr = &hextxt[0];
	for(i=0;i<(core_length+2);i++)
	{
		l = core_sdm_data[i];
		U32toASCIIHEX(l, hextxt_ptr);
		hextxt[8] = 0;
		
		for(j=4;j<8;j++)
			fputc(hextxt[j], jtag_outputdat);
		fputc(0x0A, jtag_outputdat);

		for(j=0;j<4;j++)
			fputc(hextxt[j], jtag_outputdat);

		if(i==0)
		{
			fprintf(jtag_outputdat, "  //start of IOCSR frame %i", core_frame_no+1);
			fputc(0x0A, jtag_outputdat);
		}
		else
			fputc(0x0A, jtag_outputdat);
	}
	
	#ifdef	HAVE_HEADER
	if(core_frame_no == 0)		//sdm compression footer
	{
		while ((j=fgetc(sdm_footer_readptr)) != EOF)
			fputc(j, sdm_comp_outputdat);
		fputc(0x0A, sdm_comp_outputdat);
	}
	#endif
}

void core_din_output(void)
{
	unsigned int ii;

	/*if(core_1st==1)
	{
		fprintf(corecffdat, "00000000000");		//to compensate the RTL
		fputc(0x0A, corecffdat);
	}*/

	hextxt_ptr = &hextxt[0];					//Core CFF
	if(core_width==4)
	{
		if(core_1st==1)
		{
			fprintf(corecffdat, "0000000");		//to compensate the RTL
			fputc(0x0A, corecffdat);
		}
	
		for(i=0;i<((core_length*4)+8);i++)		
		{
			j = core_data[i];
			j = flipU8(j);
			U8toAsciiBin(j, hextxt_ptr);
			hextxt[8] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			for(l=0;l<4;l++)			//1st nibble
			{
				j = hextxt[l];
				fputc(j, corecffdat);
			}

			if((i==((core_length*4)+8)-4)&&(core_1st==1))		//ARIN
				fputc('1', corecffdat);
			else
				fputc('0', corecffdat);
			
			if(i==((core_length*4)+8)-4)						//ARCE & DRBCE
				fprintf(corecffdat, "11");
			else
				fprintf(corecffdat, "00");

			fputc(0x0A, corecffdat);	
			
			for(l=4;l<8;l++)				//2nd nibble
			{
				j = hextxt[l];
				fputc(j, corecffdat);
			}

			/*if((i==((core_length*4)+8)-4)&&(core_1st==1))		//ARIN
				fputc('1', corecffdat);
			else
				fputc('0', corecffdat);
			
			if(i==((core_length*4)+8)-4)						//ARCE & DRBCE
				fprintf(corecffdat, "11");
			else
				fprintf(corecffdat, "00");*/
				
			fprintf(corecffdat, "000");
			fputc(0x0A, corecffdat);		
		}
	}
	else if(core_width==8)
	{
		if(core_1st==1)
		{
			fprintf(corecffdat, "00000000000");		//to compensate the RTL
			fputc(0x0A, corecffdat);
		}
	
		for(i=0;i<((core_length*4)+8);i++)		
		{
			j = core_data[i];
			j = flipU8(j);
			U8toAsciiBin(j, hextxt_ptr);
			hextxt[8] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			for(l=0;l<8;l++)
			{
				j = hextxt[l];
				fputc(j, corecffdat);
			}

			if((i==((core_length*4)+8)-4)&&(core_width == 8)&&(core_1st==1))		//ARIN
				fputc('1', corecffdat);
			else
				fputc('0', corecffdat);
			
			if((i==((core_length*4)+8)-4)&&(core_width == 8))			//ARCE & DRBCE
				fprintf(corecffdat, "11");
			else
				fprintf(corecffdat, "00");

			fputc(0x0A, corecffdat);		
		}
	}
	else if(core_width==16)
	{
		if(core_1st==1)
		{
			fprintf(corecffdat, "0000000000000000000");		//to compensate the RTL
			fputc(0x0A, corecffdat);
		}
	
		for(i=0;i<((core_length*4)+8);i++)		
		{
			if(i%2==0)
			{
				j = core_data[i+1];
				j = flipU8(j);
				U8toAsciiBin(j, hextxt_ptr);
				hextxt[8] = 0;
				//debugmsg("%i=%s ", i, hextxt_ptr);
				
				for(l=0;l<8;l++)
				{
					j = hextxt[l];
					fputc(j, corecffdat);
				}
			
				j = core_data[i];
				j = flipU8(j);
				U8toAsciiBin(j, hextxt_ptr);
				hextxt[8] = 0;
				//debugmsg("%i=%s ", i, hextxt_ptr);
				
				for(l=0;l<8;l++)
				{
					j = hextxt[l];
					fputc(j, corecffdat);
				}
			}

			if(i%2!=0)
			{
				if((i==((core_length*4)+8)-3)&&(core_width == 16)&&(core_1st==1))	//ARIN
					fputc('1', corecffdat);
				else
					fputc('0', corecffdat);
				
				if((i==((core_length*4)+8)-3)&&(core_width == 16))				//ARCE & DRBCE
					fprintf(corecffdat, "11");
				else
					fprintf(corecffdat, "00");
				
				fputc(0x0A, corecffdat);
			}
		}
	}
	else if(core_width==32)
	{
		if(core_1st==1)
		{
			fprintf(corecffdat, "00000000000000000000000000000000000");		//to compensate the RTL
			fputc(0x0A, corecffdat);
		}
	
		for(i=0;i<((core_length*4)+8);i++)		
		{
			if(i%4==0)
			{
				j = core_data[i+3];
				j = flipU8(j);
				U8toAsciiBin(j, hextxt_ptr);
				hextxt[8] = 0;
				//debugmsg("%i=%s ", i, hextxt_ptr);
				
				for(l=0;l<8;l++)
				{
					j = hextxt[l];
					fputc(j, corecffdat);
				}
			
				j = core_data[i+2];
				j = flipU8(j);
				U8toAsciiBin(j, hextxt_ptr);
				hextxt[8] = 0;
				//debugmsg("%i=%s ", i, hextxt_ptr);
				
				for(l=0;l<8;l++)
				{
					j = hextxt[l];
					fputc(j, corecffdat);
				}

				j = core_data[i+1];
				j = flipU8(j);
				U8toAsciiBin(j, hextxt_ptr);
				hextxt[8] = 0;
				//debugmsg("%i=%s ", i, hextxt_ptr);
				
				for(l=0;l<8;l++)
				{
					j = hextxt[l];
					fputc(j, corecffdat);
				}

				j = core_data[i];
				j = flipU8(j);
				U8toAsciiBin(j, hextxt_ptr);
				hextxt[8] = 0;
				//debugmsg("%i=%s ", i, hextxt_ptr);
				
				for(l=0;l<8;l++)
				{
					j = hextxt[l];
					fputc(j, corecffdat);
				}

			if((i==(((core_length*4)+8)-4))&&(core_width == 32)&&(core_1st==1))	//ARIN
				fputc('1', corecffdat);
			else
				fputc('0', corecffdat);
				
			if((i==(((core_length*4)+8)-4))&&(core_width == 32))		//ARCE & DRBCE
				fprintf(corecffdat, "11");
			else
				fprintf(corecffdat, "00");

			fputc(0x0A, corecffdat);
			}
		}
	}
	
	if((core_frame_no == 0)&&(core_width==16))
	{
		fprintf(corecffdat, "1111111111111111000");			//to compensate the RTL
		fputc(0x0A, corecffdat);
		fprintf(corecffdat, "1111111111111111000");			
		fputc(0x0A, corecffdat);
	}
	else if((core_frame_no == 0)&&(core_width==32))
	{
		fprintf(corecffdat, "11111111111111111111111111111111000");			//to compensate the RTL
		fputc(0x0A, corecffdat);
		fprintf(corecffdat, "11111111111111111111111111111111000");			
		fputc(0x0A, corecffdat);
	}	
}

void create_aes_pof(void)
{
	unsigned int pof_count, enc_count;
	unsigned int temp_32bit_data[5];
	unsigned int enc_counter_32bit[5];
	unsigned char tempdata[4];
	unsigned char singleChar;
	char comp_nibble_temp, comp_nibble_ctrl, temp_char;
	
	pof_count = 0;
	comp_high_nibble = 1;
	comment_list_comp_ptr = comment_list_comp;
	comment_list_ptr = comment_list;
	
	//compression aes portion
	core_comp_length = 0;
	for(i=0;i<aes_core_data_cnt;i++)
	{
		if((i>=(*comment_list_ptr))&&(i<=(*comment_list_ptr)))
		{
			*comment_list_comp_ptr = core_comp_length;
			comment_list_comp_ptr++;
			comment_list_ptr++;
		}

		l = aes_core_data[i];				//1st 4-nibble
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
		//debugmsg("%x ",comp_nibble_ctrl);
		comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_ctrl);
		if(comp_high_nibble) core_aes_comp_data_ptr++;
		if(comp_high_nibble) core_comp_length++;

		if((r&0xF000)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>12)&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x0F00)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>8)&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x00F0)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>4)&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x000F)!=0)
		{
			comp_nibble_temp = (unsigned char)(r&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		
		l = aes_core_data[i];				//2nd 4-nibble
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
		//debugmsg("%x ",comp_nibble_ctrl);
		comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_ctrl);
		if(comp_high_nibble) core_aes_comp_data_ptr++;
		if(comp_high_nibble) core_comp_length++;

		if((r&0xF000)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>12)&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x0F00)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>8)&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x00F0)!=0)
		{
			comp_nibble_temp = (unsigned char)((r>>4)&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
		if((r&0x000F)!=0)
		{
			comp_nibble_temp = (unsigned char)(r&0x000F);
			comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
			if(comp_high_nibble) core_aes_comp_data_ptr++;
			if(comp_high_nibble) core_comp_length++;
		}
	}

	//for(i=0;i<core_comp_length;i++)
	//	debugmsg("%x(%d) ",core_aes_comp_data[i] & 0xff, i);
	
	*comment_list_ptr = aes_core_data_cnt;
	for(i=0;i<postamble_count;i++)					//*** post amble
	{
		*aes_core_data_ptr = 0xFFFFFFFF;		//spi aes
		aes_core_data_ptr++;
		aes_core_data_cnt++;
	}

	/*
	*comment_list_comp_ptr = core_comp_length;
	for(i=0;i<(postamble_count*4);i++)					//*** post amble
	{
		*core_aes_comp_data_ptr = 0xFF;					//spi aes compression
		core_aes_comp_data_ptr++;
		core_comp_length++;
	}*/
	*comment_list_comp_ptr = core_comp_length;
	for(i=0;i<(postamble_count*4*2);i++)					//*** post amble
	{
		comp_nibble_temp = 0x0F;
		comp_high_nibble = pack_nibble_data(core_aes_comp_data_ptr, comp_high_nibble, comp_nibble_temp);
		if(comp_high_nibble) core_aes_comp_data_ptr++;
		if(comp_high_nibble) core_comp_length++;
	}

	if(aes_core_data_cnt > AES_DATA_SIZE)			//*** calculate AES block count
	{
		printf("\n    insufficient memory!");
		exit(0);
	}
	//debugmsg("\naes data count = %d", aes_core_data_cnt);
	//debugmsg("\nDivide = %d", aes_core_data_cnt/4);
	//debugmsg("\nremainder = %d", aes_core_data_cnt%4);
	if(aes_core_data_cnt%4 > 0)
	{
		for(i=0;i<=(aes_core_data_cnt%4);i++) //???? how is that possible??
		{
			*aes_core_data_ptr = 0xFFFFFFFF;
			aes_core_data_ptr++;
			aes_core_data_cnt++;
		}
	}
	
	if(core_comp_length%16 > 0)
	{
		for(i=0;i<=(core_comp_length%16);i++) //???? how is that possible??
		{
			*core_aes_comp_data_ptr = 0xFF;
			core_aes_comp_data_ptr++;
			core_comp_length++;
		}
	}
	
	//debugmsg("\n after fill\naes data count = %d", aes_core_data_cnt);
	//debugmsg("\nDivide = %d", aes_core_data_cnt/4);
	//debugmsg("\nremainder = %d", aes_core_data_cnt%4);
	aes_core_data_32bit_cnt = aes_core_data_cnt/4;

	for(i=0;i<16;i++)
		aes_iv_2nd[i] = aes_iv[i];

	//temp solution for SPI compression ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	comment_list_comp_ptr = comment_list_comp;
	
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<core_comp_length;i++)		
	{
		temp_char = core_aes_comp_data[i];
		//temp_char = flipU8(temp_char);

		U8toASCIIHEX(temp_char, hextxt_ptr);
		hextxt[2] = 0;
		//debugmsg("%i=%s ", i, hextxt_ptr);
			
		singleChar = hextxt[0];
		fputc(singleChar, spi_comp_outputdat);
		singleChar = hextxt[1];
		fputc(singleChar, spi_comp_outputdat);

		if((i>=(*comment_list_comp_ptr))&&(i<=(*comment_list_comp_ptr)))
		{
			comment_list_comp_ptr++;
			fprintf(spi_comp_outputdat, "  //mark for start of frame or region");
		}
	
		fputc(0x0A, spi_comp_outputdat);		
	}
	//temp solution for SDM compression post-amble~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if(second_dword_sdm_comp == 1)
	{
		fprintf(sdm_comp_outputdat, "FFFFFFFF");	
		fputc(0x0A, sdm_comp_outputdat);		
	}

	for(i=0;i<(postamble_count/2);i++)
	{
		fprintf(sdm_comp_outputdat, "FFFFFFFFFFFFFFFF");	
		
		if(i==0)
			fprintf(sdm_comp_outputdat, "    //post-amble");
		fputc(0x0A, sdm_comp_outputdat);		
	}

	//temp solution for SDM128 post-amble~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
	for(i=0;i<postamble_count;i++)
	{
		fprintf(sdm128_core_outputdat, "FFFFFFFF");	
		sdm128_out_core_track++;
		if(sdm128_out_core_track == 4)
		{
			sdm128_out_core_track = 0;
			fprintf(sdm128_core_outputdat, "    //post-amble");						
			fputc(0x0A, sdm128_core_outputdat);
		}
	}
	while(sdm128_out_core_track!=4)
	{
		fprintf(sdm128_core_outputdat, "FFFFFFFF");	
		sdm128_out_core_track++;
	}

	//temp solution for SDM128 comp post-amble~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
	for(i=0;i<postamble_count;i++)
	{
		fprintf(sdm128_comp_outputdat, "FFFFFFFF");	
		sdm128_out_comp_track++;
		if(sdm128_out_comp_track == 4)
		{
			sdm128_out_comp_track = 0;
			fprintf(sdm128_comp_outputdat, "    //post-amble");						
			fputc(0x0A, sdm128_comp_outputdat);
		}
	}
	while(sdm128_out_comp_track!=4)
	{
		fprintf(sdm128_comp_outputdat, "FFFFFFFF");	
		sdm128_out_comp_track++;
	}
	
	//spi aes  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//debugmsg("\n\nAES Encrption..");
	for(i=0;i<aes_core_data_32bit_cnt;i++)				//*** 128-bit per block (per loop) - SPI AES portion
	{
		//concatenate IV 2x64 = 128
		for(j=0;j<4;j++)		//128-bit-block reverse in byte unit (for ZB special case)
		{
			singleChar = aes_iv[j];
			aes_iv[j] = aes_iv[7-j];
			aes_iv[7-j] = singleChar;
			//debugmsg("%x ",aes_iv[j] & 0xff);
		}
	
		for(j=0;j<8;j++)
		{
			aes_iv[j+8] = aes_iv[j];
		}
		aes_iv[16] = 0;
		
		//encryption
		aes_setkey_enc(&Gctx, aes_key, 128);
		aes_crypt_ecb(&Gctx, AES_ENCRYPT, aes_iv, enc_counter);

		for(j=0;j<8;j++)		//128-bit-block reverse in byte unit (for ZB special case)
		{
			singleChar = enc_counter[j];
			enc_counter[j] = enc_counter[15-j];
			enc_counter[15-j] = singleChar;
		}
		
		//format enc_counter
		enc_count = 0;
		for(j=0;j<4;j++)
		{
			enc_counter_32bit[j] = 0;
			enc_counter_32bit[j] = enc_counter_32bit[j] | (enc_counter[enc_count] << 24);
			enc_count++;
			enc_counter_32bit[j] = enc_counter_32bit[j] | (enc_counter[enc_count] << 16);
			enc_count++;
			enc_counter_32bit[j] = enc_counter_32bit[j] | (enc_counter[enc_count] << 8);
			enc_count++;
			enc_counter_32bit[j] = enc_counter_32bit[j] | (enc_counter[enc_count] << 0);
			enc_count++;			
		}

		//XOR with Data & assign to buffer
		aes_encrypted_data[pof_count+3] = aes_core_data[pof_count+3] ^ enc_counter_32bit[0];
		//debugmsg("%i..\n",i);
		//debugmsg("enc=%x, data=%x, cnt=%x\n",aes_encrypted_data[pof_count+3],aes_core_data[pof_count+3],enc_counter_32bit[0]);
		aes_encrypted_data[pof_count+2] = aes_core_data[pof_count+2] ^ enc_counter_32bit[1];
		//debugmsg("enc=%x, data=%x, cnt=%x\n",aes_encrypted_data[pof_count+2],aes_core_data[pof_count+2],enc_counter_32bit[1]);
		aes_encrypted_data[pof_count+1] = aes_core_data[pof_count+1] ^ enc_counter_32bit[2];
		//debugmsg("enc=%x, data=%x, cnt=%x\n",aes_encrypted_data[pof_count+1],aes_core_data[pof_count+1],enc_counter_32bit[2]);
		aes_encrypted_data[pof_count+0] = aes_core_data[pof_count+0] ^ enc_counter_32bit[3];
		//debugmsg("enc=%x, data=%x, cnt=%x\n",aes_encrypted_data[pof_count+0],aes_core_data[pof_count+0],enc_counter_32bit[3]);

		pof_count += 4;

		for(j=0;j<4;j++)		//reverse back (for ZB special case)
		{
			singleChar = aes_iv[j];
			aes_iv[j] = aes_iv[7-j];
			aes_iv[7-j] = singleChar;
			//debugmsg("%x ",aes_iv[j] & 0xff);
		}
		
		//increment IV
		U8ArrayIncBy1(aes_iv, 8);	
	}

	//SPI AES POF Output
	comment_list_ptr = comment_list;
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<aes_core_data_cnt;i++)		
	{
		l = aes_encrypted_data[i];
		l = flipU32(l);				
		tempdata[0] = (char)(l >> 24)&0x000000ff;
		tempdata[1] = (char)(l >> 16)&0x000000ff;
		tempdata[2] = (char)(l >> 8)&0x000000ff;
		tempdata[3] = (char)(l & 0x000000ff);
		
		for(j=0;j<4;j++)
		{
			U8toASCIIHEX(tempdata[j], hextxt_ptr);
			hextxt[2] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			singleChar = hextxt[0];
			fputc(singleChar, spi_aes_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, spi_aes_core_outputdat);

			if((i>=(*comment_list_ptr))&&(i<=(*comment_list_ptr)))
			{
				comment_list_ptr++;
				fprintf(spi_aes_core_outputdat, "  //mark for start of frame or region");
			}

			fputc(0x0A, spi_aes_core_outputdat);		
		}
	}
	
	//SDM AES POF OUTPUT
	comment_list_ptr = comment_list;
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<aes_core_data_cnt;i+=2)		
	{
		l = aes_encrypted_data[i];
		//debugmsg("\n%x %x", l, aes_core_data[i]);
		tempdata[0] = (char)(l >> 24)&0x000000ff;
		tempdata[1] = (char)(l >> 16)&0x000000ff;
		tempdata[2] = (char)(l >> 8)&0x000000ff;
		tempdata[3] = (char)(l & 0x000000ff);
		
		for(j=0;j<4;j++)
		{
			U8toASCIIHEX(tempdata[j], hextxt_ptr);
			hextxt[2] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			singleChar = hextxt[0];
			fputc(singleChar, sdm_aes_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, sdm_aes_core_outputdat);
		}

		l = aes_encrypted_data[i+1];
		tempdata[0] = (char)(l >> 24)&0x000000ff;
		tempdata[1] = (char)(l >> 16)&0x000000ff;
		tempdata[2] = (char)(l >> 8)&0x000000ff;
		tempdata[3] = (char)(l & 0x000000ff);
		
		for(j=0;j<4;j++)
		{
			U8toASCIIHEX(tempdata[j], hextxt_ptr);
			hextxt[2] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			singleChar = hextxt[0];
			fputc(singleChar, sdm_aes_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, sdm_aes_core_outputdat);
		}

		if((i>=(*comment_list_ptr))&&(i<=(*comment_list_ptr)))
		{
			comment_list_ptr++;
			fprintf(sdm_aes_core_outputdat, "    //mark for start of frame or region");
		}
		
		fputc(0x0A, sdm_aes_core_outputdat);		
	}

	//spi aes compression ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	pof_count = 0;

	for(i=0;i<(core_comp_length/16);i++)				//*** 128-bit per block (per loop) - SPI AES Compression
	{
		//concatenate IV 2x64 = 128
		for(j=0;j<4;j++)		//128-bit-block reverse in byte unit (for ZB special case)
		{
			singleChar = aes_iv_2nd[j];
			aes_iv_2nd[j] = aes_iv_2nd[7-j];
			aes_iv_2nd[7-j] = singleChar;
		}
	
		for(j=0;j<8;j++)
		{
			aes_iv_2nd[j+8] = aes_iv_2nd[j];
		}
		aes_iv_2nd[16] = 0;
		
		//encryption
		aes_setkey_enc(&Gctx, aes_key, 128);
		aes_crypt_ecb(&Gctx, AES_ENCRYPT, aes_iv_2nd, enc_counter);

		//debugmsg("\nenc=");
		//for(j=0;j<16;j++)
		//	debugmsg("%x ",enc_counter[j] & 0xff);

		//XOR with Data & assign to buffer
		for(j=0;j<16;j++)
		{
			singleChar = flipU8(core_aes_comp_data[pof_count]);
			aes_comp_encrypted_data[pof_count] = singleChar ^ enc_counter[j];
			//debugmsg("\nenc=%x comp=%x aes=%x ", enc_counter[j] & 0xff, core_aes_comp_data[pof_count] & 0xff, aes_comp_encrypted_data[pof_count] & 0xff);
			pof_count++;
		}

		for(j=0;j<4;j++)		//reverse back (for ZB special case)
		{
			singleChar = aes_iv_2nd[j];
			aes_iv_2nd[j] = aes_iv_2nd[7-j];
			aes_iv_2nd[7-j] = singleChar;
			//debugmsg("%x ",aes_iv_2nd[j] & 0xff);
		}
		//debugmsg("\nIV(rev back) = ");
		//for(j=0;j<8;j++)
		//	debugmsg("%x ",aes_iv_2nd[j] & 0xff);
		
		//increment IV
		U8ArrayIncBy1(aes_iv_2nd, 8);	
	}

	//SPI AES COMPRESSION POF Output
	comment_list_comp_ptr = comment_list_comp;
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<core_comp_length;i++)		
	{
		temp_char = aes_comp_encrypted_data[i];
		temp_char = flipU8(temp_char);

		U8toASCIIHEX(temp_char, hextxt_ptr);
		hextxt[2] = 0;
		//debugmsg("%i=%s ", i, hextxt_ptr);
			
		singleChar = hextxt[0];
		fputc(singleChar, spi_aes_comp_core_outputdat);
		singleChar = hextxt[1];
		fputc(singleChar, spi_aes_comp_core_outputdat);

		if((i>=(*comment_list_comp_ptr))&&(i<=(*comment_list_comp_ptr)))
		{
			comment_list_comp_ptr++;
			fprintf(spi_aes_comp_core_outputdat, "  //mark for start of frame or region");
		}

		fputc(0x0A, spi_aes_comp_core_outputdat);		
	}
	
	//SDM AES compression POF output
	comment_list_comp_ptr = comment_list_comp;
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<(core_comp_length/8);i++)		
	{
		for(j=3;j>=0;j--)
		{
			temp_char = aes_comp_encrypted_data[j+(i*8)];
			//debugmsg("%d ",j+(i*8));
			
			U8toASCIIHEX(temp_char, hextxt_ptr);
			hextxt[2] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			singleChar = hextxt[0];
			fputc(singleChar, sdm_aes_comp_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, sdm_aes_comp_core_outputdat);
		}

		for(j=7;j>=4;j--)
		{
			temp_char = aes_comp_encrypted_data[j+(i*8)];
			//debugmsg("%d ",j+(i*8));
			
			U8toASCIIHEX(temp_char, hextxt_ptr);
			hextxt[2] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			singleChar = hextxt[0];
			fputc(singleChar, sdm_aes_comp_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, sdm_aes_comp_core_outputdat);
		}
		
		if((*comment_list_comp_ptr>=(i*8))&&(*comment_list_comp_ptr<=((i+1)*8)))
		{
			comment_list_comp_ptr++;
			fprintf(sdm_aes_comp_core_outputdat, "    //mark for start of frame or region");
		}

		fputc(0x0A, sdm_aes_comp_core_outputdat);		
	}

	//SDM128 AES POF OUTPUT
	comment_list_ptr = comment_list;
	hextxt_ptr = &hextxt[0];					
	for(i=0;i<aes_core_data_cnt;i++)		
	{
		l = aes_encrypted_data[i];
		//debugmsg("\n%x %x", l, aes_core_data[i]);
		tempdata[0] = (char)(l >> 24)&0x000000ff;
		tempdata[1] = (char)(l >> 16)&0x000000ff;
		tempdata[2] = (char)(l >> 8)&0x000000ff;
		tempdata[3] = (char)(l & 0x000000ff);
		
		for(j=0;j<4;j++)
		{
			U8toASCIIHEX(tempdata[j], hextxt_ptr);
			hextxt[2] = 0;
			//debugmsg("%i=%s ", i, hextxt_ptr);
			
			singleChar = hextxt[0];
			fputc(singleChar, sdm128_aes_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, sdm128_aes_core_outputdat);
		}

		/*if((i>=(*comment_list_ptr))&&(i<=(*comment_list_ptr)))
		{
			comment_list_ptr++;
			j = 1;
			//fprintf(sdm128_aes_core_outputdat, "    //mark for start of frame or region");
		}*/

		sdm128_out_aes_track++;
		if(sdm128_out_aes_track == 4)
		{
			/*if(j==1)
			{
				fprintf(sdm128_aes_core_outputdat, "    //mark for start of frame or region");
				j = 0;
			}*/
			sdm128_out_aes_track = 0;
			fputc(0x0A, sdm128_aes_core_outputdat);
		}
	}	
	while(sdm128_out_aes_track !=4)
	{
		fprintf(sdm128_aes_core_outputdat, "FFFFFFFF");	
		sdm128_out_aes_track++;
	}
	

	//SDM128 AES compression POF output
	comment_list_comp_ptr = comment_list_comp;
	hextxt_ptr = &hextxt[0];					

	for(i=0;i<(core_comp_length/8);i++)		
	{
		for(j=3;j>=0;j--)
		{
			temp_char = aes_comp_encrypted_data[j+(i*8)];
			
			U8toASCIIHEX(temp_char, hextxt_ptr);
			hextxt[2] = 0;
			
			singleChar = hextxt[0];
			fputc(singleChar, sdm128_aes_comp_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, sdm128_aes_comp_core_outputdat);
		}

		sdm128_out_aescomp_track++;
		if(sdm128_out_aescomp_track == 4)
		{
			sdm128_out_aescomp_track = 0;
			fputc(0x0A, sdm128_aes_comp_core_outputdat);
		}

		for(j=7;j>=4;j--)
		{
			temp_char = aes_comp_encrypted_data[j+(i*8)];
			
			U8toASCIIHEX(temp_char, hextxt_ptr);
			hextxt[2] = 0;
			
			singleChar = hextxt[0];
			fputc(singleChar, sdm128_aes_comp_core_outputdat);
			singleChar = hextxt[1];
			fputc(singleChar, sdm128_aes_comp_core_outputdat);
		}
		
		/*if((*comment_list_comp_ptr>=(i*8))&&(*comment_list_comp_ptr<=((i+1)*8)))
		{
			comment_list_comp_ptr++;
			fprintf(sdm128_aes_comp_core_outputdat, "    //mark for start of frame or region");
		}*/

		sdm128_out_aescomp_track++;
		if(sdm128_out_aescomp_track == 4)
		{
			sdm128_out_aescomp_track = 0;
			fputc(0x0A, sdm128_aes_comp_core_outputdat);
		}
	}
	while(sdm128_out_aescomp_track !=4)
	{
		fprintf(sdm128_aes_comp_core_outputdat, "FFFFFFFF");	
		sdm128_out_aescomp_track++;
	}
}
