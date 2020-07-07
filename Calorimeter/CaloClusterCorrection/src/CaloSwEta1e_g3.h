/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWETA1E_G3_H
#define CALOCLUSTERCORRECTION_CALOSWETA1E_G3_H
/********************************************************************

NAME:     CaloSwEta1e_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 1 as a function of eta for endcap
          (Tuned using 100 GeV E photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qeta1e.age          

Updated:  Feb 11, 2003   (MW)
          new correction for DC1 data

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/


#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/Array.h"

class CaloSwEta1e_g3 : public CaloClusterCorrection
{

 public:
  // Inherit construtor.
  using CaloClusterCorrection::CaloClusterCorrection;
  
  // virtual method of CaloClusterCorrection
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:

  CaloSwEta1e_g3() = delete;

  Constant<CxxUtils::Array<3> > m_correction  { this, "correction", "" };
  Constant<float>         m_correction_coef   { this, "correction_coef", "" };
  Constant<int>           m_correction_degree { this, "correction_degree", "" };
  Constant<int>           m_region            { this, "region", "" };

  static const float s_middle_layer_granularity;
};


#endif
