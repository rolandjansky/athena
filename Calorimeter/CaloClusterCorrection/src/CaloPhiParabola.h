/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  Constant<CxxUtils::Array<2> > m_correction { this, "correction", "" };
  Constant<CxxUtils::Array<1> > m_energies   { this, "energies",   "" };
  Constant<int>                 m_degree     { this, "degree",     "" };
};


#endif 
