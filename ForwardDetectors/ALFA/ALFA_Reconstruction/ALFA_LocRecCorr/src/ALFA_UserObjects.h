/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_USEROBJECTS_H
#define ALFA_USEROBJECTS_H

#include <iostream>
#include <list>

using namespace std;

typedef struct _ODHIT
{
	int iEvent;
	int iRPot;
	int iPlate;
	int iSide;
	int iFiber;
} ODHIT, *PODHIT;

typedef struct _MDHIT
{
	int iEvent;
	int iRPot;
	int iPlate;
	int iFiber;
} MDHIT, *PMDHIT;

typedef struct _FIBERS
{
	list<int> ListFibers;
} FIBERS, *PFIBERS;

typedef struct _ODRESULT
{
	int iSide;

	double fReconstructedPosition;
} ODRESULT, *PODRESULT;

typedef struct _ODRES
{
	int iRPot;
	int iSide;

	double fReconstructedPosition;
} ODRES, *PODRES;

#endif // ALFA_USEROBJECTS_H
