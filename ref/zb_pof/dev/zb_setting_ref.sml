<VERSION=5.2>											//version number

<SPI_OUTPUT_FILE=spi_uncompressed.dat>					//SPI POF Image File Name
<SPI_OUTPUT_COMP_FILE=spi_compressed.dat>				//SPI Compressed POF Image File Name
<SPI_AES_OUTPUT_FILE=spi_aes_uncompressed.dat>			//SPI AES POF Image File Name
<SPI_AES_COMP_OUTPUT_FILE=spi_aes_compressed.dat>		//SPI AES Compressed POF Image File Name
<SDM_OUTPUT_FILE=sdm_uncompressed.dat>					//SDM IOCSR POF Image File Name
<SDM_OUTPUT_COMP_FILE=sdm_compressed.dat>				//SDM Compressed POF Image File Name
<SDM_AES_OUTPUT_FILE=sdm_aes_uncompressed.dat>			//SDM AES POF Image Filename
<SDM_AES_COMP_OUTPUT_FILE=sdm_aes_compressed.dat>		//SDM AES Compressed POF Image Filename
<SDM128_OUTPUT_FILE=sdm128_uncompressed.dat>			//SDM IOCSR POF Image File Name
<SDM128_OUTPUT_COMP_FILE=sdm128_compressed.dat>			//SDM Compressed POF Image File Name
<SDM128_AES_OUTPUT_FILE=sdm128_aes_uncompressed.dat>	//SDM AES POF Image Filename
<SDM128_AES_COMP_OUTPUT_FILE=sdm128_aes_compressed.dat>	//SDM AES Compressed POF Image Filename
<JTAG_OUTPUT_FILE=jtag_uncompressed.dat>				//JTAG Uncompressed POF Image Filename
<IOCSR_EXP_FILE=din_iocsr.exp>							//IOCSR expected output file name
<CORE_EXP_FILE=din_core.exp>							//CORE expected output file name
<CREATE_OUTPUT_FILE=TRUE>								//marking for end of files creation
<AES_KEY=00000000012345670000000001234567>				//Hex in capital letter
<AES_IV=F0FFAFBFCDFFF9F0>								//Hex in capital letter

<IOCSR_FRAME=3>						//total number of iocsr frame
<IOCSR_DATA=1,64,0,random>				//no, data count(dword), pad count in dword, data type(random/zero/one)
<IOCSR_DATA=2,64,0,random>		
<IOCSR_DATA=3,64,0,random>	

<START_FILLER=16>					//number of filler (dword count)

<CORE_FRAME=6>						//total number of core frame
<CORE_DATA=1,10,66666666,99,4>		//no, data count(dword), data type(32-bit Hex/random/zero/one), FT(hex), segmentation(8/16/32)
<CORE_DATA=2,10,99999999,80,4>
<CORE_DATA=3,10,55555555,17,4>
<CORE_SWITCH=16,16>					//switch region (dword count), segmentation(8/16/32)
<CORE_DATA=4,10,11111111,31,16>
<CORE_DATA=5,10,22222222,40,16>
<CORE_DATA=6,10,33333333,27,16>

<POST_AMBLE=70>						//number of post amble (dword count)
<CREATE_POF=1>						//signal for POF creation

 
