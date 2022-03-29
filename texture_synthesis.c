#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "texture_synthesis.h"
#include <float.h>
#include "image.h"

//A function that computes the GaussianWeight of a pixel on a window from the central pixel of that window
double GaussianWeight(int relativeIndex_x, int relativeIndex_y, unsigned int windowRadius) 
{
	double Sigma = (2*windowRadius+1) / 6.4;
	double result = exp(-1 * ((relativeIndex_x*relativeIndex_x) + (relativeIndex_y*relativeIndex_y)) / (2*(Sigma * Sigma)));
	return result;
}

// compares tbs pixels 
int CompareTBSPixels( const void *v1 , const void *v2 )
{
	const TBSPixel *tp1 = (const TBSPixel *)v1;
	const TBSPixel *tp2 = (const TBSPixel *)v2;
	int d = tp1->neighborCount - tp2->neighborCount;
	if( d ) return -d;
	d = tp1->idx.y - tp2->idx.y;
	if( d ) return d;
	d = tp1->idx.x - tp2->idx.x;
	if( d ) return d;
	return tp2->r - tp1->r;
}

// sorts tbs pixels, returns zero if succeeded
int SortTBSPixels( TBSPixel *tbsPixels , unsigned int sz )
{
	unsigned int *permutation = (unsigned int*)malloc( sizeof(unsigned int)*sz );
	if( !permutation )
	{
		fprintf( stderr , "[ERROR] Failed to allocate memory for permutation: %d\n" , sz );
		return 1;
		exit(1);
	}
	for( unsigned int i=0 ; i<sz ; i++ ) permutation[i] = i;
	for( unsigned int i=0 ; i<sz ; i++ )
	{
		unsigned int i1 = rand() % sz;
		unsigned int i2 = rand() % sz;
		unsigned int tmp = permutation[i1];
		permutation[i1] = permutation[i2];
		permutation[i2] = tmp;
	}

	for( unsigned int i=0 ; i<sz ; i++ ) tbsPixels[i].r = permutation[i];
	free( permutation );

	qsort( tbsPixels , sz , sizeof( TBSPixel ) , CompareTBSPixels );

	return 0;
}

//A function checking if a pixel is TBS and return the number of neighboring set pixels
int IsTBSPixel(const Image *synthesized, PixelIndex index) {
	int neighborCount = 0;
	PixelIndex IndexToCheck = {index.x, index.y};
	const Pixel * PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
	//check if the pixel itself is unset
	if(PixelToCheck->a == 255) {
		return 0;
	}

	//check right neighbor
	IndexToCheck.x = index.x+1;
	IndexToCheck.y = index.y;

	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	//check left neighbor
	IndexToCheck.x = index.x-1;
	IndexToCheck.y = index.y;
	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	//check top neighbor
	IndexToCheck.x = index.x;
	IndexToCheck.y = index.y-1;
	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	//check bottom neighbor
	IndexToCheck.x = index.x;
	IndexToCheck.y = index.y+1;
	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	//check upperright neighbor
	IndexToCheck.x = index.x+1;
	IndexToCheck.y = index.y-1;
	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	//check upperleft neighbor
	IndexToCheck.x = index.x-1;
	IndexToCheck.y = index.y-1;
	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	//check bottomleft neighbor
	IndexToCheck.x = index.x-1;
	IndexToCheck.y = index.y+1;
	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	//check bottomright neighbor
	IndexToCheck.x = index.x+1;
	IndexToCheck.y = index.y+1;
	if(InBounds(synthesized, IndexToCheck)) {
		PixelToCheck = GetConstPixel(synthesized, IndexToCheck);
		if (PixelToCheck->a == 255) {//if the neighboring pixel is set
			neighborCount++;
		}
	}
	return neighborCount;
}

//Return the number of TBSPixels in the synthesized image.
int IsImageSet(const Image *synthesized){
	int NumTBSPixels = 0;
	for (unsigned int i = 0; i < (synthesized -> width); i++) {
		for (unsigned int j = 0; j < (synthesized -> height); j++){
			PixelIndex index = {i, j};
			if (IsTBSPixel(synthesized, index)) {
				NumTBSPixels++;
			}
		}
	}
	return NumTBSPixels;
}

//returns the list of indices of TBSPixels on the synthesized image
TBSPixel *GetTBSPixelList(const Image *synthesized, const int NumTBSPixels) {
	if (!NumTBSPixels) {//if NumTBSPixels = 0, meaning the image is all set!
		return NULL;
	}
	TBSPixel * TBSPixelList= (TBSPixel *)malloc(sizeof(TBSPixel) * NumTBSPixels); 
	int k = 0;//The index of TBSPixelList
	for (unsigned int i = 0; i < (synthesized -> width); i++) {
		for (unsigned int j = 0; j < (synthesized -> height); j++){
			PixelIndex index = {i, j};
			unsigned int neighborCount = IsTBSPixel(synthesized, index);
			if (neighborCount) {//if neighborCount != 0, meaning it's a TBSPixel
				TBSPixel pixel = {index, neighborCount, 0};
				TBSPixelList[k++] = pixel; 
			}
		}
	}
	return TBSPixelList;
}

//takes a pixelindex as input and returns a list of its neighboring set pixels
int ** GetWindowList(const Image *image, PixelIndex index, unsigned int windowRadius) {
	int ** SetPixels = malloc(sizeof(int *) * (2*windowRadius + 1));
	const Pixel * PixelToCheck;
	for (unsigned int i = 0; i<2*windowRadius+1; i++) {
		SetPixels[i] = calloc(sizeof(int), 2*windowRadius+1);
	}
	//{i, j} is the relative index of a pixel on the image
	//{windowRadius, windowRadius} is the relative index of the center of the window
	//For example, windowRadius = 5, the center's relative index is at {5,5}
	//{index.x+i - windowRadius, index.y+i - windowRadius} is the relative index of a pixel away from the center of that window
	for (unsigned int i = 0; i < 2*windowRadius+1; i++) {
		for (unsigned int j = 0; j <2*windowRadius+1; j++) {
			PixelIndex IndexToCheck = GetAbsolutePixelIndex(image, index, i, j, windowRadius);
			if(InBounds(image, IndexToCheck)) {
				PixelToCheck = GetConstPixel(image, IndexToCheck);
				if (PixelToCheck->a == 255) {//if the neighboring pixel is set
					//we store coordinate (i, j) in 2D array[j][i] because of how the pixel index is defined
					SetPixels[j][i] = 1;//1 means that the neighbor within the window is set. SetPixels[0][0] = 1 means that the top left corner is set.
				}   
			}
		}
	}
	
	return SetPixels;
}

//given the relative index ({m, n}) of a pixel on the window, return the absolute index ({x,y}) of a pixel on the image
PixelIndex GetAbsolutePixelIndex(const Image * image, PixelIndex centerIndex, int m, int n, int windowRadius) {
	//centerIndex is the absolute index of the center pixel of the window
	//m = n = 0, 1, 2,...,2*windowRadius; {windowRadius,windowRadius} is the relative index of the center pixel of the window.
	if ((centerIndex.x + m) < windowRadius || (centerIndex.y + n )< windowRadius) {
		PixelIndex index = {-1,-1};
		return index;//meaning an invalid index is returned
	}
	//windowRadius - m = centerIndex.x - absoluteIndex.x; windowRadius - n = centerIndex.y - absoluteIndex.y
	//absoluteIndex.x = centerIndex.x - windowRadius + m;
	PixelIndex index = {centerIndex.x - windowRadius + m, centerIndex.y - windowRadius + n};
	if (InBounds(image, index)) {
		return index;
	} 
	
	//meaning an invalid index is returned
	index.x = -1;
	index.y = -1;
	return index;
}
//takes a TBSPixelIndex as input returns a list of its matching pixels on the exemplar image
Pixel ** FindMatches (const Image *exemplar, Image * synthesized, PixelIndex TBSPixelIndex, int ** SetPixels, unsigned int windowRadius,int* sizeofBestMatch) {
	//record the number of set pixels in the window of SetPixels
	unsigned int pixelsInWindow = 0;
	for(unsigned int i = 0;i<2*windowRadius+1;i++){
		for(unsigned int j = 0;j<2*windowRadius+1;j++){
			if(SetPixels[i][j]==1){
				pixelsInWindow++;
			}
		}
	}

	const int MAXNUM = (exemplar->width) * (exemplar->height) - 1; //TODO: TOBEMODIFIED

	Pixel ** BestMatches = malloc(sizeof(Pixel *) * MAXNUM); //the list of matching pixels on the exemplar image that have difference less than the threshold
	int BestMatchesIndex = 0;//number of BestMatches
	
	int width = exemplar->width;
	int height = exemplar->height;
	double diffList[height][width]; //the list storing each exemplar pixel's difference with the TBS pixel
	
	//initializing diffList with -1.0; if diffList[i][j] is still -1.0 after executing the function, it means that pixel {i, j} on the exemplar image is not a match
	for (int i = 0;i<height; i++) {
		for (int j = 0; j<width; j++) {
			diffList[i][j] = -1.0;
		}
	}
	//initialize minDiff to the maximum double to be replaced in future comparison
	double minDiff = DBL_MAX; 


	//the following section finds each exemplar pixel's neighboring set pixels and compare them with the TBSPixel 
	//i, j are the absolute index of the pixel on the exemplar image to be checked
	for (unsigned int i = 0; i<exemplar->width; i++) {
		for (unsigned int j = 0; j<exemplar->height; j++){
			double diff = 0; //the difference of an exemplar pixel with the TBS pixel;
			unsigned int exitState = 0;//if exitState == 1, this pixel with (i, j) does not need to be checked;
			unsigned int matchCount = 0; //if the pixel from the exemplar image has the same set neighbors as the TBSPixel's neighbors, this pixel is considered a match and matchCount++
			PixelIndex ExemplarAbsoluteIndex= {i, j}; //i, j are the coordinates of the exemplar pixel
			int ** ExemplarSetPixels = GetWindowList(exemplar, ExemplarAbsoluteIndex, windowRadius); //the 2D array of 1s and 0s around the exemplar pixel
			
			//compare every neighboring set pixels inside the TBSPixel's 
			//m, n are the relative index of the exemplar/TBSPixel's neighbor on the window
			for (unsigned int m = 0; m < 2*windowRadius+1; m++) {
				for(unsigned int n = 0; n < 2*windowRadius+1; n++){
					PixelIndex ExemplarNeighborAbsoluteIndex = GetAbsolutePixelIndex(exemplar, ExemplarAbsoluteIndex, m, n, windowRadius);
					//GetAbsolutePixelIndex will initialize the index to {-1,-1} if it is invalid
					if (!InBounds(exemplar, ExemplarNeighborAbsoluteIndex)) { //skip comparing invalid index
						continue;
					}
					if (ExemplarNeighborAbsoluteIndex.x==(int)i && ExemplarNeighborAbsoluteIndex.y==(int)j) {//skip comparing the exemplar pixel itself
						continue;
					}
					//SetPixels[n][m] corresponds to the relative index (m, n)
					if(SetPixels[n][m] == 1) {//meaning that the TBSPixel's neighbor pixel is set
						if (ExemplarSetPixels[n][m] == 1) {//meaning that the Exemplar's neighbor pixel is also set
						//!!!
							const Pixel * ExemplarNeighbor = GetConstPixel(exemplar, ExemplarNeighborAbsoluteIndex);
							PixelIndex TBSNeighborAbsoluteIndex = GetAbsolutePixelIndex(synthesized, TBSPixelIndex, m, n, windowRadius);
							if (!InBounds(synthesized, TBSNeighborAbsoluteIndex)) {
								continue;
							}
							Pixel * TBSNeighbor = GetPixel(synthesized, TBSNeighborAbsoluteIndex);
							double d = PixelSquaredDifference((Pixel*)ExemplarNeighbor, TBSNeighbor);
							double s = GaussianWeight(TBSPixelIndex.x - TBSNeighborAbsoluteIndex.x, TBSPixelIndex.y - TBSNeighborAbsoluteIndex.y, windowRadius);
							diff += d*s;
							matchCount++;
						} 
						//if the following gets executed, meaning ExemplarSetPixels[m][n] != 1, this exemplar pixel {i, j} is not valid for further comparison
						//therefore, we will do two breaks (skip m loop and n loop) consecutively to jump to the next {i,j}
						//diff can never equal to negative under normal circumstances because d,s > 0
						else {
							exitState = 1;
							break;
						}		
					}
				}
				if (exitState) {
					break;
				}
			}
			if (matchCount < pixelsInWindow) {
				exitState = 1;
			}

			for (unsigned int k = 0; k < 2*windowRadius+1; k++){
				free(ExemplarSetPixels[k]);
			}
			free(ExemplarSetPixels);

			if (exitState) {//skip this iteration if the pixel is not a match
				diffList[j][i] = -1; 
				continue;
			} 
			
			//storing the diff of each exemplar pixel and the minDiff; -1 if invalid exemplar pixel;
			diffList[j][i] = diff;
			if(diff<minDiff) {
				minDiff = diff;
			}
		}
	}

	
	

	//calculate the error threshold and Select the BestMatches
	double threshold = minDiff * 1.1;
	int capacity = 20;
	for (unsigned int i = 0; i < exemplar->width; i++){
		for (unsigned int j = 0; j<exemplar->height; j++){
			//realloc memory
			if (BestMatchesIndex == capacity-1) {
				capacity *= 2;
				BestMatches = realloc(BestMatches, sizeof(Pixel *) * capacity);
			}
			if (diffList[j][i] >= 0 && diffList[j][i] <= threshold) {
				PixelIndex index = {i, j};
				BestMatches[BestMatchesIndex++] = GetPixel((Image*)exemplar, index);
			}
		}
	}
	

	*sizeofBestMatch = BestMatchesIndex;
	return BestMatches;
	
}
//choose a BestMatch from a list of BestMatches
Pixel* chooseRandom(Pixel** BestMatches,int size){
	int r = rand() % size;
	return BestMatches[r];
}

Image *SynthesizeFromExemplar(const Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius)
{
	//check if width and height is large enough
	if(exemplar->width>outWidth || exemplar->height>outHeight){
		printf("ERROR: Width or height too small, unable to contain input image!\n");
		exit(5);
	}

	//allocate image
	Image *synthesized = AllocateImage(outWidth,outHeight);

	// initializing the image
	for(unsigned int i=0;i<outHeight;i++){
		for(unsigned int j=0;j<outWidth;j++){
			((synthesized->pixels)+i*outWidth+j)->r = 0;
			((synthesized->pixels)+i*outWidth+j)->g = 0;
			((synthesized->pixels)+i*outWidth+j)->b = 0;
			((synthesized->pixels)+i*outWidth+j)->a = 0;
		}
	}

	//set the top left parts to be the same as the exemplar + set the alpha track
	for(unsigned int i=0;i<exemplar->height;i++){
		for(unsigned int j=0;j<exemplar->width;j++){
			((synthesized->pixels)+i*outWidth+j)->r = ((exemplar->pixels)+i*exemplar->width+j)->r;
			((synthesized->pixels)+i*outWidth+j)->g = ((exemplar->pixels)+i*exemplar->width+j)->g;
			((synthesized->pixels)+i*outWidth+j)->b = ((exemplar->pixels)+i*exemplar->width+j)->b;
			((synthesized->pixels)+i*outWidth+j)->a = 255;
		}
	}

	int NumTBSPixels = IsImageSet(synthesized);
	TBSPixel *TBSPixelList = GetTBSPixelList(synthesized, NumTBSPixels);
	
	//This loop will keep executing as long as there are more than zero TBS Pixels
	while (NumTBSPixels) {
		SortTBSPixels(TBSPixelList, NumTBSPixels);
		for (int i = 0; i < NumTBSPixels; i++){
			//keep track of the size of the best match
			int sizeofBestMatch;
			//we obtain the list of Set Pixels on the window of the TBS Pixel
			int ** SetPixels = GetWindowList(synthesized, TBSPixelList[i].idx, windowRadius);
			Pixel ** BestMatches = FindMatches(exemplar, synthesized,TBSPixelList[i].idx, SetPixels, windowRadius,&sizeofBestMatch);
			//pick a random pixel from the best matches
			Pixel* random = chooseRandom(BestMatches,sizeofBestMatch);
			//get the pixel that we want to change and change its values
			Pixel* changed = GetPixel(synthesized, TBSPixelList[i].idx);
			changed->r = random->r;
			changed->g = random->g;
			changed->b = random->b;
			changed->a = 255;

			//free SetPixels
			for (unsigned int i = 0; i < 2*windowRadius + 1; i++) {
				free(SetPixels[i]);
			}
			free(SetPixels);
			//free BestMatches
			free(BestMatches);
			
		}
		//printf("Total [%d] TBSpixels\n", NumTBSPixels); used for showing progress
		NumTBSPixels = IsImageSet(synthesized);
		free(TBSPixelList);
		//update the TBS Pixel List
		TBSPixelList = GetTBSPixelList(synthesized, NumTBSPixels);
	}
	free(TBSPixelList);
	return synthesized;

}
