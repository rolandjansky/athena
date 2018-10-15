/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_SUPERPOINTDATA_H
#define  TRIGL2MUONSA_SUPERPOINTDATA_H

#define NMEAMX 50
#define MNLINE 14
#define NLAYER 8
#define NCAND  6  //segment candidate
namespace TrigL2MuonSA {

class PBFitResult
{
 public:
  PBFitResult() :
    CHI2(0),
    PCHI2(0),
    NPOI(0),
    NDOF(0),
    ALIN(0),
    BLIN(0)
      {
      	for (int i=0; i<NMEAMX; i++) {
          IDMEA[i] = 0;
          XILIN[i] = 0;
          YILIN[i] = 0;
          IGLIN[i] = 0;
          RILIN[i] = 0;
          WILIN[i] = 0;
          DISTJ[i] = 0;
          JLINE[i] = 0;
          RESI[i]  = 0;
        }
      	//////segment candidate
      	for (int i=0; i<NCAND; i++){
          SlopeCand[i]     = 0;
          InterceptCand[i] = 0;
          Chi2Cand[i]      = 0;
        }
      	for (int i=0; i<MNLINE; i++) NLINE[i] = 0;
      	for (int i=0; i<2; i++)
          for (int j=0; j<2; j++)
            DABLIN[i][j] = 0;
     };
  ~PBFitResult() {};

 public:
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
  float SlopeCand[NCAND];     //segment candidate
  float InterceptCand[NCAND]; //
  float Chi2Cand[NCAND];      //

}; // See description at the bottom


class SuperPoint
{
 public:
  SuperPoint() :
    Npoint(0),
    Ndigi(0),
    R(0),
    Z(0),
    Phim(0),
    Alin(0),
    Blin(0),
    Xor(0),
    Yor(0),
    Chi2(0),
    PChi2(0)
  {
    for (int i=0; i<NLAYER; i++) Residual[i]=0;
    //////
    for (int i=0; i<NCAND; i++){
      SlopeCand[i]     = 0;
      InterceptCand[i] = 0;
      Chi2Cand[i]      = 0;
    }
   //////
  };

  ~SuperPoint() {};
    
 public:
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
  float Residual[NLAYER];
  float SlopeCand[NCAND];     //
  float InterceptCand[NCAND]; //
  float Chi2Cand[NCAND];      //

};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_SUPERPOINTDATA_H


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
