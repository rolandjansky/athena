/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOPHIPARABOLA_H
#define CALOCLUSTERCORRECTION_CALOPHIPARABOLA_H
/********************************************************************

NAME:     CaloPhiParabola.h
PACKAGE:  offline/Calorimter/CaloClusterCorrection
 
AUTHORS:  M. Aharrouche
CREATED:  Jan 25, 2006

PURPOSE:  corrects for the Phi versus Ecluster modulations

********************************************************************/
#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"



class CaloPhiParabola
: public CaloClusterCorrectionCommon
{
public:

  CaloPhiParabola(const std::string& type,
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

  CaloRec::Array<2> m_correction;
  CaloRec::Array<1> m_energies;
  int m_degree;
};


#endif 
