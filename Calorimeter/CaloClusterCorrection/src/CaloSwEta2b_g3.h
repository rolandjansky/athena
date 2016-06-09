/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  // Constructor 
  CaloSwEta2b_g3(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);
  // Destructor 
  virtual ~CaloSwEta2b_g3() override;

  // Virtual function in CaloClusterCorrection
  virtual void makeCorrection(const EventContext& ctx,
                              xAOD::CaloCluster* cluster) const override;

 private:

  CaloSwEta2b_g3() = delete;
  CaloRec::Array<2> m_correction;
  CaloRec::Array<2> m_residuals;
  CaloRec::Array<1> m_residual_bins;
  int               m_correction_degree;
  int               m_residual_eval_degree;
  int               m_residual_degree;
  float             m_correction_coef;
  float             m_residual_coef;
  int               m_region;
  static const float s_middle_layer_granularity;

};

#endif
