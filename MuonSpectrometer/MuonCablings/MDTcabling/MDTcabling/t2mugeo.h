// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_T2MUGEO_H
#define PHYSICS_MU_T2MUGEO_H

//#include "roi.h"

#define PI 3.141592653
#define MaxJFF 8
#define MaxJZZ 15
#define MaxTYPE 60
#define TDCch 24
#define MaxTDC 18
#define MaxMul 2
#define MaxLay 4
#define MaxTub 72
#define MaxCham 6


struct TDCgeo{
        unsigned short int Id;
	float Rmin;
	float Rmax;
	float Zmin;
	float Zmax;
	float RZ[TDCch][2];
	unsigned short int Lay[TDCch];
        unsigned short int Tub[TDCch];
};

//struct RPCgeo{
//	float Radplane[2];
//};

struct Multilayer{
        char Type[2];
	int TubeLayers;
	int TubesNumber;
	float Rmin;
	float Rmax;
	float Zmin;
	float Zmax;
	float Ymid;
	TDCgeo *TDC[MaxTDC/2];
};

struct Chamber{
	char Type[4];
	int NumTDC;
	float Rmin;
	float Rmax;
	float Zmin;
	float Zmax;
	float Ymid;    // radius/zeta (B/E) position of the multichamber 
	float Xmid;    // zeta/radius (B/E) position of the multichamber
	float Amid;    // angular coeff. of the middle position
	float Phipos;
        float DeltaPhi;
	float OrtoRadialPos;
	Multilayer *multi[2];
	TDCgeo *TDC[MaxTDC];
};

typedef Chamber* (*Chambers)[8*(2*MaxJZZ+1)];
typedef int (*GeomIndex)[8*(2*MaxJZZ+1)];


struct ChamType{
	char type[60][3];
	int maxType;
	int maxJzz;
};
struct interstruc{
        int   MLayer;
	int   MDTlay[2];
	int   RPCcha;
	float ZposCha;
	float Zlength;
	float RposCha;
	float RmidCha;
	float OrtRadp;
	float PposCha;
	float Alpha;
	float Beta;
	float Gamma;
        float Lwidth;
        float Swidth;
	float Zwidth[2];
	float Rwidth[2];
	float ZposMul[2];
	float YposMul[2];
	float YposTub[10];
	float XposTub[10];
	float TubePitch[2];
//	float RPCrad[4];
};

//#endif

typedef struct head {
	int JJ;
	int JFF;
	int JZZ;
}Header;

struct TDCremap {
        int Multilayer;
	int Layer;
	int Tube;
};

//struct RPCrad {
//	float r1[3];
//	float r2[3];
//	float r3[3];
//	float LRoffset;
//	float LZlim;
//};



//struct LUT {
//	int NbinEta[4];
//	int NbinPhi[4];
//	float EtaMin[4];
//	float EtaMax[4];
//	float EtaStep[4];
//	float PhiMin[4];
//	float PhiMax[4];
//	float PhiStep[4];
//	float table[4][2][60][60][2];
//};


#endif // PHYSICS_MU_T2MUGEO_H

