// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_LINKFIT_H
#define PHYSICS_MU_LINKFIT_H

#include "TrigmuFast/StationFitStd.h"
#include "TrigmuFast/CalibUti.h"

#include <string>

#define NMEAMX 50
#define MNLINE 14

class TrackFit;
class DigiPointer;
class MuFeature;

class StationFitPB: public StationFitStd {

private:
    typedef struct {
	int   IDMEA[NMEAMX];
	float XILIN[NMEAMX];
	float YILIN[NMEAMX];
	int   IGLIN[NMEAMX];
	float RILIN[NMEAMX];
	float WILIN[NMEAMX];
	float DISTJ[NMEAMX];
	int   JLINE[NMEAMX];
	float RESI[NMEAMX];
	int   NLINE[MNLINE];
	float CHI2;
	float PCHI2;
	int   NPOI;
	int   NDOF;
	float ALIN;
	float BLIN;
	float DABLIN[2][2];
    } PBfit;                    // see below

public:
    StationFitPB(MuMessageInterface& msg,
                 const TrackFit& track_fit,
                 LayerDigits (*digits)[MAXLAYER],
                 SpointData super_points[]);
    virtual ~StationFitPB(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

private:
    float SetDriftSpace(float tdr,float rad,float zeta,float phim);
    float SetDriftSigma(float ddr);
    void Xline(float *,float *,float *,int *,int ,
               float *,float *,float *,float *,float *,float *);
    void Circfit (int,float *,float *,float *,float *,int *,
                  float *,float *,float DAB[2][2],float *);
    void Circles (int,float *,float *,float *,float *,int *,
                  float *,float *,float DAB[2][2],float *,float *);
    int Evlfit (int);

private:
    PBfit m_fitres;

private:
    static const std::string s_name;

};

inline std::string
StationFitPB::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_LINKSTD_H

/*
      IDMEA(j)   : 10,000 * detector + 10 * channel + time
                   (ex. first hit of TRK 3 is 40031)
      XILIN(j)   : x of point j (for tubes, x of tube center)
      YILIN(j)   : y of point j (for tubes, y of tube center)
      IGLIN(j)   : flag of point j (see CIRCLES);
                   if point not to be used, flag is -ve
                   if point has been discarded, flag is .lt.-100;
      RILIN(j)   : for tubes, drift distance;
      WILIN(j)   : 1/sigma**2 (i.e. the weight of point j);
      DISTJ(j)   : signed distance line <--> (x,y)
                   for tubes, this is NOT the residual;
      JLINE(j)   : to which line the channels belongs
                   (e.g. tubes at the same x have the same JLINE);
      RESI(j)    : signed residual (meas - line);
      CHI2       : chi square of the fit;
      PCHI2      : prob (chi2, degrees of freedom);
      NPOI       : totsl number of meas (including not used);
      NDOF       : number of degrees of freedom (i.e. used meas - 2);
      ALIN       : line coefficient 1 --> y = ALIN * x + BLIN
      BLIN       : line coefficient 2 --> y = ALIN * x + BLIN
      DABLIN(2,2): error matrix
*/
