/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  //constructor 
  CaloSwEta1e_g3(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);
  // destructor 
  virtual ~CaloSwEta1e_g3() override;
  
  // virtual method of CaloClusterCorrection
  virtual void makeCorrection(const EventContext& ctx,
                              xAOD::CaloCluster* cluster) const override;

 private:

  CaloSwEta1e_g3() = delete;

  CaloRec::Array<3> m_correction;
  float             m_correction_coef;
  int               m_correction_degree;
  int               m_region;

  static const float s_middle_layer_granularity;
};


#endif
