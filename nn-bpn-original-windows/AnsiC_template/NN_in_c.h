typedef int           BOOL;
typedef int           INT;
typedef double        REAL;

#define FALSE         0
#define TRUE          1
#define NOT           !
#define AND           &&
#define OR            ||

#define MIN_REAL      -HUGE_VAL
#define MAX_REAL      +HUGE_VAL
#define MIN(x,y)      ((x)<(y) ? (x) : (y))
#define MAX(x,y)      ((x)>(y) ? (x) : (y))

#define LO            0.1
#define HI            0.9
#define BIAS          1

#define sqr(x)        ((x)*(x))

#include <math.h>
#include "NN_setting.h"

typedef struct {                     /* A LAYER OF A NET:                     */
	INT           Units;         /* - number of units (node) in this layer       */
	REAL*         Output;        /* - output of ith unit 					(output side - 1D array for number of node in that layer)	*/
	REAL*         Error;         /* - error term of ith unit				(output side - 1D array for error of that output)              */
	REAL**        Weight;        /* - connection weights to ith unit 		(input side  - 2D array [Nth node on current layer][weight for previous layer Nth mode])   */
	REAL**        WeightSave;    /* - saved weights for stopped training	(input side) */
	REAL**        dWeight;       /* - last weight deltas for momentum     	(input side) */
} LAYER;

typedef struct {                     /* A NET:                                */
	LAYER**       Layer;         /* - layers of this net                  */
	LAYER*        InputLayer;    /* - input layer                         */
	LAYER*        OutputLayer;   /* - output layer                        */
	REAL          Alpha;         /* - momentum factor                     */
	REAL          Eta;           /* - learning rate                       */
	REAL          Gain;          /* - gain of sigmoid function            */
	REAL          Error;         /* - total net error                     */
} NET;

/******************************************************************************
Random Number Generation related functions
******************************************************************************/
void InitializeRandoms()					//provide RNG seed
{
	srand(4711);	
}

INT RandomEqualINT(INT Low, INT High)		//generate random data within range of LOW - HIGH
{
	return rand() % (High - Low + 1) + Low;
}

REAL RandomEqualREAL(REAL Low, REAL High)	//generate random data within range of LOW - HIGH
{
	return ((REAL)rand() / RAND_MAX) * (High - Low) + Low;
}

/******************************************************************************
NN Generation
******************************************************************************/
void GenerateNetwork(NET* Net)
{
	INT l, i;

	Net->Layer = (LAYER**)calloc(NUM_LAYERS, sizeof(LAYER*));

	for (l = 0; l<NUM_LAYERS; l++) {											/* for every layer - including input layer and output layer */
		Net->Layer[l] = (LAYER*)malloc(sizeof(LAYER));

		Net->Layer[l]->Units = Units[l];										/* number of node = total number of node defined for that layer */
		Net->Layer[l]->Output = (REAL*)calloc(Units[l] + 1, sizeof(REAL));		/* allocate and zerolize output of all nodes at that layer */  /* ref -- void* calloc (number of element, size of element */
		Net->Layer[l]->Output[0] = BIAS;										/* 1st node is for bias */
		Net->Layer[l]->Error = (REAL*)calloc(Units[l] + 1, sizeof(REAL));		/* "" */
		Net->Layer[l]->Weight = (REAL**)calloc(Units[l] + 1, sizeof(REAL*));	/* allocate and zerolize input weights for all nodes at that layer - define the [][] */
		Net->Layer[l]->WeightSave = (REAL**)calloc(Units[l] + 1, sizeof(REAL*)); /* "" */
		Net->Layer[l]->dWeight = (REAL**)calloc(Units[l] + 1, sizeof(REAL*));	 /* "" */

		if (l != 0) {															/* all layer after the input layer (1st layer) */
			for (i = 1; i <= Units[l]; i++) {
				Net->Layer[l]->Weight[i] = (REAL*)calloc(Units[l - 1] + 1, sizeof(REAL));		/* allocate and zerolize the each input weights for that particular node */
				Net->Layer[l]->WeightSave[i] = (REAL*)calloc(Units[l - 1] + 1, sizeof(REAL));
				Net->Layer[l]->dWeight[i] = (REAL*)calloc(Units[l - 1] + 1, sizeof(REAL));
			}
		}
	}
	Net->InputLayer = Net->Layer[0];
	Net->OutputLayer = Net->Layer[NUM_LAYERS - 1];
	Net->Alpha = 0.9;
	Net->Eta = 0.25;
	Net->Gain = 1;
}

void RandomWeights(NET* Net)
{
	INT l, i, j;

	for (l = 1; l<NUM_LAYERS; l++) {							/* every layer (except input layer) */
		for (i = 1; i <= Net->Layer[l]->Units; i++) {			/* every node in that layer */
			for (j = 0; j <= Net->Layer[l - 1]->Units; j++) {	/* every input weight of that particular node */
				Net->Layer[l]->Weight[i][j] = RandomEqualREAL(-0.5, 0.5);
			}
		}
	}
}

/******************************************************************************
NN Training Functions
******************************************************************************/
//Step A
void SetInput(NET* Net, REAL* Input)					//for used by SimulateNet
{
	INT i;

	for (i = 1; i <= Net->InputLayer->Units; i++) {
		Net->InputLayer->Output[i] = Input[i - 1];
	}
}

void PropagateLayer(NET* Net, LAYER* Lower, LAYER* Upper)
{
	INT  i, j;
	REAL Sum;

	for (i = 1; i <= Upper->Units; i++) {
		Sum = 0;
		for (j = 0; j <= Lower->Units; j++) {
			Sum += Upper->Weight[i][j] * Lower->Output[j];
		}
		Upper->Output[i] = 1 / (1 + exp(-Net->Gain * Sum));
	}
}

void PropagateNet(NET* Net)
{
	INT l;

	for (l = 0; l<NUM_LAYERS - 1; l++) {
		PropagateLayer(Net, Net->Layer[l], Net->Layer[l + 1]);
	}
}

void GetOutput(NET* Net, REAL* Output)					// for used by SimulateNet
{
	INT i;

	for (i = 1; i <= Net->OutputLayer->Units; i++) {
		Output[i - 1] = Net->OutputLayer->Output[i];
	}
}

//Step B
void ComputeOutputError(NET* Net, REAL* Target)
{
	INT  i;
	REAL Out, Err;

	Net->Error = 0;
	for (i = 1; i <= Net->OutputLayer->Units; i++) {
		Out = Net->OutputLayer->Output[i];
		Err = Target[i - 1] - Out;
		Net->OutputLayer->Error[i] = Net->Gain * Out * (1 - Out) * Err;
		Net->Error += 0.5 * sqr(Err);
	}
}

//Step C
void BackpropagateLayer(NET* Net, LAYER* Upper, LAYER* Lower)
{
	INT  i, j;
	REAL Out, Err;

	for (i = 1; i <= Lower->Units; i++) {
		Out = Lower->Output[i];
		Err = 0;
		for (j = 1; j <= Upper->Units; j++) {
			Err += Upper->Weight[j][i] * Upper->Error[j];
		}
		Lower->Error[i] = Net->Gain * Out * (1 - Out) * Err;
	}
}

void BackpropagateNet(NET* Net)
{
	INT l;

	for (l = NUM_LAYERS - 1; l>1; l--) {
		BackpropagateLayer(Net, Net->Layer[l], Net->Layer[l - 1]);
	}
}

void AdjustWeights(NET* Net)
{
	INT  l, i, j;
	REAL Out, Err, dWeight;

	for (l = 1; l<NUM_LAYERS; l++) {
		for (i = 1; i <= Net->Layer[l]->Units; i++) {
			for (j = 0; j <= Net->Layer[l - 1]->Units; j++) {
				Out = Net->Layer[l - 1]->Output[j];
				Err = Net->Layer[l]->Error[i];
				dWeight = Net->Layer[l]->dWeight[i][j];
				Net->Layer[l]->Weight[i][j] += Net->Eta * Err * Out + Net->Alpha * dWeight;
				Net->Layer[l]->dWeight[i][j] = Net->Eta * Err * Out;
			}
		}
	}
}

void SimulateNet(NET* Net, REAL* Input, REAL* Output, REAL* Target, BOOL Training)
{
	SetInput(Net, Input);		//Step A
	PropagateNet(Net);			//""
	GetOutput(Net, Output);		//""

	ComputeOutputError(Net, Target);	//step B
	if (Training) {							//if the call is for training purpose
		BackpropagateNet(Net);			//Step C
		AdjustWeights(Net);				//""
	}
}
/******************************************************************************
NN Training Functions - supporting
******************************************************************************/
void NormalizeTrainingData(REAL* trainData, REAL* normalizeData, INT dataSize, REAL mean)
{
	INT  n;
	REAL Min, Max, MeanValue;

	Min = MAX_REAL;
	Max = MIN_REAL;
	MeanValue = mean;
	for (n = 0; n<dataSize; n++) {
		Min = MIN(Min, trainData[n]);
		Max = MAX(Max, trainData[n]);
	}
	MeanValue = 0;
	for (n = 0; n<dataSize; n++) {
		normalizeData[n] =
			trainData[n] = ((trainData[n] - Min) / (Max - Min)) * (HI - LO) + LO;
		MeanValue += trainData[n] / dataSize;
	}
}

void SaveWeights(NET* Net)
{
	INT l, i, j;

	for (l = 1; l<NUM_LAYERS; l++) {
		for (i = 1; i <= Net->Layer[l]->Units; i++) {
			for (j = 0; j <= Net->Layer[l - 1]->Units; j++) {
				Net->Layer[l]->WeightSave[i][j] = Net->Layer[l]->Weight[i][j];
			}
		}
	}
}

void RestoreWeights(NET* Net)
{
	INT l, i, j;

	for (l = 1; l<NUM_LAYERS; l++) {
		for (i = 1; i <= Net->Layer[l]->Units; i++) {
			for (j = 0; j <= Net->Layer[l - 1]->Units; j++) {
				Net->Layer[l]->Weight[i][j] = Net->Layer[l]->WeightSave[i][j];
			}
		}
	}
}
