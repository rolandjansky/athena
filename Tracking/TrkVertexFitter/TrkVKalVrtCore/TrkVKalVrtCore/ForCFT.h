/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_ForCFT_H
#define _TrkVKalVrtCore_ForCFT_H

#include "TrkVKalVrtCore/CommonPars.h"

namespace Trk {

  struct ForCFT 
  {
//
    int useMassCnst;
    int usePhiCnst;
    int useThetaCnst;
    int usePointingCnst;
    int useAprioriVrt;
    int usePassNear;
    int usePlaneCnst;

// Summary charge at vertex
    long int icht;
//
// For several(up to 8) mass constraints
    int nmcnst;
    double wm[NTrkM];
    double wmfit[8], localbmag;
// Since 20/09/2009 - Vertex in plane constraint 
    double Ap,Bp,Cp,Dp;

//
// Vertex, covariance and it's weight (inverse covariance)
// Used for pointing and a priori vertex constraints
// Since 15/03/2009: 
//    For "passing near" vrt+covvrt+charge from ForVrtClose structure is used. 
//    For pointing cnst vertex from constraint object itself is used
//---------------------------------------------------------------------------
    double vrt[3], covvrt[6], wgtvrt[6];
//
// temporary vertex in global ref.frame
    double vrtstp[3];        
    long int irob;
    double RobustScale;
    double robres[NTrkM];
    short int indtrkmc[NTrkM*8];	/* was [300][8] */
    short int IterationNumber;
    double IterationPrecision;
 
    ForCFT(){
      nmcnst=0;
      useMassCnst=0; usePhiCnst=0; useThetaCnst=0; usePointingCnst=0; usePlaneCnst=0;
      useAprioriVrt=0; usePassNear=0; icht=0; Ap=Bp=Dp=Cp=0.;
      IterationNumber = 100; IterationPrecision=1.e-3; RobustScale = 1.; irob=0;
      localbmag=2.0;   // Safety: standard magnetic field in ID 
    };
    ~ForCFT(){};
  };

}
#endif
