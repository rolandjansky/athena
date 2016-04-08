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
       vkalDynamicArrays(const vkalDynamicArrays& );             //copy
       vkalDynamicArrays& operator=(const vkalDynamicArrays& );  //assign
      ~vkalDynamicArrays();
      double *get_dphi() const;
      double *get_deps() const;
      double *get_drho() const;
      double *get_dtet() const;
      double *get_dzp() const;

    private:
      double *m_dphi;
      double *m_deps;
      double *m_drho;
      double *m_dtet;
      double *m_dzp;
      long int m_NSIZE;
  };

}
#endif
