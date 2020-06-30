/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWETA2B_G3_H
#define CALOCLUSTERCORRECTION_CALOSWETA2B_G3_H
/********************************************************************

NAME:     CaloSwEta2b_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 2 as a function of eta
	  for barrel EM calorimeter. 
          (Tuned using 50 GeV Et photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qeta2b.age          

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.


********************************************************************/
#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/Array.h"

class CaloSwEta2b_g3 : public CaloClusterCorrection
{
public:
  // Inherit constructor.
  using CaloClusterCorrection::CaloClusterCorrection;


  // Virtual function in CaloClusterCorrection
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:

  CaloSwEta2b_g3() = delete;

  Constant<CxxUtils::Array<2> > m_correction { this, "correction", "" };
  Constant<CxxUtils::Array<2> > m_residuals  { this, "residuals", "" };
  Constant<CxxUtils::Array<1> > m_residual_bins { this, "residual_bins", "" };
  Constant<int>      m_correction_degree    { this, "correction_degree", "" };
  Constant<int>      m_residual_eval_degree { this, "residual_eval_degree", ""};
  Constant<int>      m_residual_degree      { this, "residual_degree", "" };
  Constant<float>    m_correction_coef      { this, "correction_coef", "" };
  Constant<float>    m_residual_coef        { this, "residual_coef",   "" };
  Constant<int>      m_region               { this, "region",          "" };

  static const float s_middle_layer_granularity;

};

#endif
