/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWETA1B_G3_H
#define CALOCLUSTERCORRECTION_CALOSWETA1B_G3_H
/********************************************************************

NAME:     CaloSwEta1b_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 1 as a function of eta
          (Tuned using 50 GeV Et photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qeta1b.age          

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.

********************************************************************/

// INCLUDE HEADER FILES:
#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/Array.h"

class CaloSwEta1b_g3 : public CaloClusterCorrection
{

 public:
  // Inherit constructor.
  using CaloClusterCorrection::CaloClusterCorrection;

  // virtual method in CaloClusterCorrection
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:

  CaloSwEta1b_g3() = delete;
  // parameters 
  Constant<CxxUtils::Array<2> > m_correction { this, "correction", "" };
  Constant<CxxUtils::Array<1> > m_correction_bins { this, "correction_bins",""};
  Constant<int>    m_correction_degree {this, "correction_degree", ""};
  Constant<int>    m_interp_degree     { this, "interp_degree",    "" };
  Constant<float>  m_correction_coef   { this, "correction_coef",  "" };
  Constant<int>    m_region            { this, "region",           "" };

  static const float s_strip_granularity;

};


#endif
