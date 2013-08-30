/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_WorkArrays_H
#define _TrkVKalVrtCore_WorkArrays_H

#include "TrkVKalVrtCore/CommonPars.h"

namespace Trk {
 
  class vkalDynamicArrays;

  struct WorkArray{
    int existFullCov; 
    double vcov[6], swa[6];
    double   wb[9*NTrkM];	  /* was [9][300] */
    double   wc[6*NTrkM];	  /* was [6][300] */ 
    double  wci[6*NTrkM];	  /* was [6][300] */
    double wbci[9*NTrkM];	  /* was [9][300] */ 
    double ader[(3*NTrkM+3)*(3*NTrkM+3)];  /* was [903][903] */
    double e[5*NTrkM];	                   /* was [5][300] */
    double v[5*5*NTrkM]	                   /* was [5][5][300] */;
    vkalDynamicArrays *myWorkArrays;    
  };


 
  class vkalDynamicArrays {
  
    public:
       vkalDynamicArrays(long int NTrkMax);
      ~vkalDynamicArrays();
      double *get_tt() const;
      double *get_part() const;
      double *get_parf0() const;
      double *get_scale() const;
      double *get_dphi() const;
      double *get_eps() const;
      double *get_deps() const;
      double *get_drho() const;
      double *get_dtet() const;
      double *get_phip() const;
      double *get_phiv() const;
      double *get_zp() const;
      double *get_dzp() const;
      double *get_drdp() const;

    private:
      double *tt;
      double *part;
      double *parf0;
      double *scale;
      double *dphi;
      double *eps;
      double *deps;
      double *drho;
      double *dtet;
      double *phip;
      double *phiv;
      double *zp;
      double *dzp;
      double *drdp;
  };

}
#endif
