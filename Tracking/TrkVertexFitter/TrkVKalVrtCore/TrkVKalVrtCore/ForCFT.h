/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

//
// For several(up to 8) mass constraints
    int nmcnst;
    double wm[vkalNTrkM];
    double wmfit[vkalMaxNMassCnst];
    double localbmag;
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
    int irob;
    double RobustScale;
    double robres[vkalNTrkM];
    int indtrkmc[vkalMaxNMassCnst][vkalNTrkM];
    int IterationNumber;
    double IterationPrecision;
 
    void prcfit( long int ntrk, double  *wm, double  *wmfit, double  bmag, double  *vrt, double  *vrte) noexcept;

    void vksetIterationNum(long int Iter) noexcept;
    void vksetIterationPrec(double Prec) noexcept;
    void vksetRobustScale(double Scale) noexcept;
    void vksetRobustness(long int Rob) noexcept;
    void vksetUseMassCnst() noexcept;
    void vksetUsePhiCnst() noexcept;
    void vksetUsePlaneCnst(double a, double b, double c, double d) noexcept;
    void vksetUseThetaCnst() noexcept { useThetaCnst = 1;}
    void vksetUseAprioriVrt() noexcept { useAprioriVrt = 1;}
    void vksetUsePointingCnst(int iType = 1 ) noexcept { usePointingCnst = iType<2 ? 1 : 2 ;}
    void vksetUsePassNear(int iType = 1 )  noexcept { usePassNear = iType<2 ? 1 : 2 ;}
    void setmasscnst_(long int ncnsttrk, long int *indextrk, double  wmcnst) noexcept;

    ForCFT() noexcept;//Body is in PrCFit.cxx
    ~ForCFT() = default;

  };

}
#endif
