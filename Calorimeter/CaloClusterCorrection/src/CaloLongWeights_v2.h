/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOLONGWEIGHTS_V2_H
#define CALOCLUSTERCORRECTION_CALOLONGWEIGHTS_V2_H
/********************************************************************

NAME:     CaloLongWeights_v2.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  M. Aharrouche
CREATED:  Jan 25, 2006

PURPOSE:  correction for the energy lost behind the calorimeter,
          between presampler and the calorimeter and in dead material
          in front of the presampler. 

********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


class CaloLongWeights_v2 : public CaloClusterCorrectionCommon
{
public:
  // constructor 
  CaloLongWeights_v2 (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

 
  virtual void makeTheCorrection (const EventContext& ctx,
                                  xAOD::CaloCluster* cluster,
                                  const CaloDetDescrElement* elt,
                                  float eta,
                                  float adj_eta,
                                  float phi,
                                  float adj_phi,
                                  CaloSampling::CaloSample samp) const override;


 private:


  CaloRec::Array<3>     m_correction;
  CaloRec::Array<1>     m_energies;
  int m_degree;
  float m_etamax;
  float m_barrel_frontier;
  float m_endcap_frontier;
};

#endif

