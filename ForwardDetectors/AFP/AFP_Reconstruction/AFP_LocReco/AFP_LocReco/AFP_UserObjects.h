/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_USEROBJECTS_H
#define AFP_USEROBJECTS_H

#include <iostream>
#include <vector>

using namespace std;

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

#endif // AFP_USEROBJECTS_H
