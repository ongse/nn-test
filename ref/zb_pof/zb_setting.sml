<VERSION=3.3>									//version number

<SPI_OUTPUT_FILE=spi_output.dat>				//SPI POF Image File Name
<SPI_OUTPUT_COMP_FILE=spi_compress_output.dat>	//SPI Compressed POF Image File Name
<SDM_OUTPUT_FILE=sdm_output.dat>				//SDM IOCSR POF Image File Name
<SDM_OUTPUT_COMP_FILE=sdm_compress_output.dat>	//SDM Compressed POF Image File Name
<IOCSR_EXP_FILE=iocsr.exp>						//IOCSR expected output file name
<CORE_EXP_FILE=core.exp>						//CORE expected output file name
<CREATE_OUTPUT_FILE=TRUE>						//marking for end of files creation

<IOCSR_FRAME=3>						//total number of iocsr frame
<IOCSR_DATA=1,64,0,zero>			//no, data count(dword), pad count in dword, data type(random/zero/one)
<IOCSR_DATA=2,64,0,random>		
<IOCSR_DATA=3,64,0,one>	

<CORE_FRAME=6>						//total number of core frame
<CORE_DATA=1,100,ABCD1234,28,65,8>	//no, data count(dword), data type(32-bit Hex/random/zero/one), FT(hex), SL(hex), segmentation(8/16)
<CORE_DATA=2,100,one,28,65,8>
<CORE_DATA=3,100,one,28,65,8>
<CORE_SWITCH=TRUE>
<CORE_DATA=4,100,one,28,65,16>
<CORE_DATA=5,100,random,28,65,16>
<CORE_DATA=6,100,one,28,65,16>
