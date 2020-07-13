/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOTOPOEMSSHAPE_H
#define CALOCLUSTERCORRECTION_CALOTOPOEMSSHAPE_H
/********************************************************************

NAME:     CaloTopoEMsshape.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  M.Boonekamp, N.Kerschen
CREATED:  March 2005

PURPOSE:  s-shape corrections
          base class: CaloClusterCorrection (AlgTool)

Updated:  March 12, 2005   (MB)
          corrections for the TopoCluster 
********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
class CaloCluster;

class CaloTopoEMsshape : public CaloClusterCorrectionCommon
{
public:
  /// Inherit constructor.
  using CaloClusterCorrectionCommon::CaloClusterCorrectionCommon;


  // virtual method in CaloClusterCorrection
  virtual void makeTheCorrection(const Context& myctx,
                                 xAOD::CaloCluster* cluster,
				 const CaloDetDescrElement* elt,
				 float eta,
				 float adj_eta,
				 float phi,
                                 float adj_phi,
				 CaloSampling::CaloSample samp) const;

 private:
  // Comments on the parametrization :
  // f(x) = P0 + atan(P1*(x-0.5)) + P2(x-0.5) + P3*|x-0.5| + P4
  // x is the position on the cell (0<x<1)
  // Tuned on 100 GeV electrons, no energy dependence
  Constant<CxxUtils::Array<1> > m_P0            { this, "P0", "" };
  Constant<CxxUtils::Array<1> > m_P1            { this, "P1", "" };
  Constant<CxxUtils::Array<1> > m_P2            { this, "P2", "" };
  Constant<CxxUtils::Array<1> > m_P3            { this, "P3", "" };
  Constant<CxxUtils::Array<1> > m_P4            { this, "P4", "" };
  Constant<CxxUtils::Array<1> > m_EtaFrontier   { this, "EtaFrontier", "" };
  Constant<float> m_Granularity                 { this, "Granularity", "" };
};

#endif
