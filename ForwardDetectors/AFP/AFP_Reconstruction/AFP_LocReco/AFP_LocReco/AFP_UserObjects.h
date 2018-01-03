/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_USEROBJECTS_H
#define AFP_USEROBJECTS_H

#include <iostream>
#include <vector>

typedef struct _TDHIT
{
	int iEvent;
	float fADC;
	float fTDC;
	int nDetectorID;
	int nStationID;
	int nSensitiveElementID;
	
} TDHIT, *PTDHIT;



typedef struct _TDRESULT
{
	int   	nStationID;
	int 	nDetectorID;
	int 	nTrainID;
	float   fTrainTime;
	int	nTrainSize;
        int     nTrainNSat;

	void clear();
} TDRESULT, *PTDRESULT;


typedef struct _SIDHIT
{
	int iEvent;
	int fADC;
	int fTDC;
	int nDetectorID;
	int nStationID;
	int nPixelRow;
	int nPixelCol;
	
} SIDHIT, *PSIDHIT;


typedef struct _SIDHITPOS
{
	int nPlateID;
	int nStationID;
	float fPixX;
	float fPixY;
	float fPixZ;
	float fAmp;
	
} SIDHITPOS, *PSIDHITPOS;

typedef struct _SIDHITSEED
{
	int nHitID1;
	int nHitID2;
	int nLastPlate;
	int nStationID;
	float fSeedX;
	float fSeedDX;
	float fSeedY;
	float fSeedDY;
	float fSeedZ;
	
} SIDHITSEED, *PSIDHITSEED;

typedef struct _SIDRESULT
{
	int   nStationID;	
	float x_pos;
	float y_pos;
	float z_pos;
	float x_slope;
	float y_slope;
	float z_slope;
	
        std::vector<int> ListHitID;
	
	float nHits;
	float nHoles;
	float fChi2;	

	void clear();
} SIDRESULT, *PSIDRESULT;

#endif // AFP_USEROBJECTS_H
