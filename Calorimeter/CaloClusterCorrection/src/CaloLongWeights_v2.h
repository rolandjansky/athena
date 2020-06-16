/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  /// Inherit constructor.
  using CaloClusterCorrectionCommon::CaloClusterCorrectionCommon;

 
  virtual void makeTheCorrection (const Context& myctx,
                                  xAOD::CaloCluster* cluster,
                                  const CaloDetDescrElement* elt,
                                  float eta,
                                  float adj_eta,
                                  float phi,
                                  float adj_phi,
                                  CaloSampling::CaloSample samp) const override;


 private:


  Constant<CxxUtils::Array<3> > m_correction      { this, "correction", "" };
  Constant<CxxUtils::Array<1> > m_energies        { this, "energies",   "" };
  Constant<int>                 m_degree          { this, "degree",     "" };
  Constant<float>               m_etamax          { this, "EtaMax",     "" };
  Constant<float>               m_barrel_frontier { this, "EtaBarrel",  "" };
  Constant<float>               m_endcap_frontier { this, "EtaEndCap",  "" };
};

#endif

