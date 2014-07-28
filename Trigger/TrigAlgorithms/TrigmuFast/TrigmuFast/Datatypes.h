// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_DATATYPES_H
#define PHYSICS_MU_DATATYPES_H

#include <stdint.h>
#include "TrigmuFast/MuData.h"
//#include "MDTcabling/t2mugeo.h"

#define MAXLAYER 8
#define MAXSTATION 12
#define MAXSPOINT 4

#define PIGR 3.1415926535897931160E0

class TgcRawData;


struct Persint
{
    int Jtype;
    int Jff;
    int Jzz;
    int Jobj;
    int Jlay;
    int Jtub;
    float xyz[3];
    float Drift;
    int Code;
    int Ipart;
};

struct Offline
{
    unsigned int name;
    int StationEta;
    int StationPhi;
    int Multilayer;
    int TubeLayer;
    int Tube;
    float xyz[3];
    float Drift;
    int Code;
    int Ipart;
};

struct RpcTrigPatt
{
    uint32_t eta_pivot_lay0;
    uint32_t eta_pivot_lay1;
    uint32_t eta_low_0_lay0;
    uint32_t eta_low_1_lay0;
    uint32_t eta_low_0_lay1;
    uint32_t eta_low_1_lay1;
    uint32_t eta_high_0_lay0;
    uint32_t eta_high_1_lay0;
    uint32_t eta_high_0_lay1;
    uint32_t eta_high_1_lay1;
    
    uint32_t phi_pivot_lay0;
    uint32_t phi_pivot_lay1;
    uint32_t phi_low_0_lay0;
    uint32_t phi_low_1_lay0;
    uint32_t phi_low_0_lay1;
    uint32_t phi_low_1_lay1;
    uint32_t phi_high_0_lay0;
    uint32_t phi_high_1_lay0;
    uint32_t phi_high_0_lay1;
    uint32_t phi_high_1_lay1;
};


struct TriggerData
{
    bool isEndcap;
    int PhysicsSector;
    int LargeSmall;
    int type;
    int threshold;
    int triggerNum;
    int side;
    int MDT_Sector_for_trigger;
    int MDT_Sector_for_overlap;
    float slope;
    float intercept;
    float rpc1[4];
    float rpc2[4];
    float rpc3[4];

    float beta;

    // for TGC study
    // Middle station
    float tgcMid1[4];       // Eta, Phi, R, Z at lowest Z
    float tgcMid2[4];       // Eta, Phi, R, Z at highest Z
    float tgcMidRhoChi2;    // Rho fit Chi2
    int tgcMidRhoNin;       // Rho fit N-points in input
    int tgcMidRhoN;         // Rho fit N-points
    float tgcMidPhiChi2;    // Phi fit Chi2
    int tgcMidPhiNin;       // Phi fit N-points in input
    int tgcMidPhiN;         // Phi fit N-points

    // Inner station
    float tgcInn[4];        // Eta, Phi, R, Z
    float tgcInnRhoStd;     // Rho fit standard dev
    int tgcInnRhoNin;       // Rho fit N-points in input
    int tgcInnRhoN;         // Rho fit N-points
    float tgcInnPhiStd;     // Phi fit standard dev
    int tgcInnPhiNin;       // Phi fit N-points in input
    int tgcInnPhiN;         // Phi fit N-points

    float tgcPT;            // Estimate of pT based on TGC segment

    // RoI eta/hi
    float roiEta;   
    float roiPhi;
};

struct TrackFit
{
    bool isEndcap;
    bool isPhidir;
    int   Type;
    float Xcenter;
    float Ycenter;
    float Radius;
    float is;
    float a2;
    float aq;
    float b;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    double ap;
    double aphi;
    double phi;
    double extrInnerEta;
    double extrInnerPhi;
};

struct RoadRegions
{
    int Type;
    int ChamberTypes[2][4][2];
    int pos1[2];
    float aw1[2];
    float bw1[2];
    int pos2[2];
    float aw2[2];
    float bw2[2];
    int pos3[2];
    float aw3[2];
    float bw3[2];
    int pos4[2];
    float aw4;
    float bw4;
    float zetaMin[2][4]; //comp. at Inner,Middle,Outer,Extended  for trigger+overlap
    float zetaMax[2][4]; //comp. at Inner,Middle,Outer,Extended  for trigger+overlap
    float radMin [2][4]; //comp. at Inner,Middle,Outer,Extended  for trigger+overlap
    float radMax [2][4]; //comp. at Inner,Middle,Outer,Extended  for trigger+overlap
    float etaMin [2][4]; //comp. at Inner,Middle,Outer,Extended  for trigger+overlap
    float etaMax [2][4]; //comp. at Inner,Middle,Outer,Extended  for trigger+overlap
    float phi[2][4];
    float roads[2][4];
};

struct DigitData
{
    float Z;
    float R;
    float DriftTime;
    float DriftSpace;
    float DriftSigma;
    float Residual;
    uint32_t OnlineId;
    uint16_t Adc;
    uint16_t LeadingCoarseTime;
    uint16_t LeadingFineTime;
    uint16_t TrailingCoarseTime;
    uint16_t TrailingFineTime;
    int Ipart;
    DigitData *ring;
    DigitData *next;
    Persint *PeMDT;
    Offline* IdMDT;
};

struct LayerDigits
{
    bool isEndcap;
    int ntot;
    int ndigi;
    int ndigit;
    float ResSum;
    float Ymid;
    float Xmid;
    float Amid;
    float Phi0;
    float PhiMed;
    float InCosS;
    DigitData *first;
};

struct DigiPointer
{
    LayerDigits (*digi)[MAXLAYER];
};

struct SpointData
{

    int   Npoint;
    int   Ndigi;
    float R;
    float Z;
    float Phim;
    float Alin;
    float Blin;
    float Xor;
    float Yor;
    float Chi2;
    float PChi2;
    float Residual[8];
};

struct MuFeature
{

    int ok;
    int NSpoint;
    int Saddress;
    float Spoint[MAXSPOINT][3];
    int   Sdigi [MAXSPOINT][2];
    double Sagitta;   // track sagitta
    double Radius;    // track curvature radius
    double EtaMap;
    double PhiMap;
    double phi;
    double dir_phi;
    double Pt;
    float Charge;
    int   etaBin;
    int   phiBin;
    double slope;     // slope computed from MDT fit
    double intercept; // intercept computed from MDT fit
    double alpha;     // APLHA computed with the MIDDLE and/or + OUTER
    double beta;      // BETA computed with INNER + MIDDLE and/or + OUTER
    double deltaR;    // deltaR computed with INNER + MIDDLE and/or + OUTER
};

struct tgcRawData
{
    unsigned short int bcTag,sswId,slbId,bitpos;
};

struct tgcDigitData
{
    int iDigit;
    double x, y, z, r, eta, phi, w;
    char sw, sel;
    std::string stationName;
};

struct tgcLUTDigit
{
    double eta, phi, r, z, width;
    int sta;
    bool isStrip;
    TgcRawData* rawData;
    bool inRoad;
    bool isCoincidence;
    bool inSegment;
};

typedef std::vector<TgcDigit*>    DigitVec;
typedef std::vector<tgcRawData>   RawVec;
typedef std::vector<tgcDigitData> DecodedVec;
typedef std::vector<tgcLUTDigit>  LUTDigitVec;

#endif // PHYSICS_MU_DATATYPES_H
