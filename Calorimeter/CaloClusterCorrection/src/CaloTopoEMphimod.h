/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOTOPOEMPHIMOD_H
#define CALOCLUSTERCORRECTION_CALOTOPOEMPHIMOD_H
/********************************************************************

NAME:     CaloTopoEMphimod.h
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

class CaloTopoEMphimod : public CaloClusterCorrectionCommon
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
  // Barrel :
  // f(x) = P0 + P1*cos(8*PI*x)  + P2*cos(16*PI*x) + P3*sin(8*PI*x) + P4*sin(16*PI*x)
  // Endcap :
  // f(x) = P0 + P1*cos(6*PI*x)  + P2*cos(12*PI*x) + P3*sin(6*PI*x) + P4*sin(12*PI*x)
  // with the constraint P0=1 (no modification of the overall scale at this stage).
  // x is the phi position in the cell (0<x<1)
  // Tuned on 100 GeV electrons, no energy dependence
  Constant<CxxUtils::Array<1> > m_P1b { this, "P1b", "" };
  Constant<CxxUtils::Array<1> > m_P2b { this, "P2b", "" };
  Constant<CxxUtils::Array<1> > m_P3b { this, "P3b", "" };
  Constant<CxxUtils::Array<1> > m_P4b { this, "P4b", "" };
  Constant<CxxUtils::Array<1> > m_P1e { this, "P1e", "" };
  Constant<CxxUtils::Array<1> > m_P2e { this, "P2e", "" };
  Constant<CxxUtils::Array<1> > m_P3e { this, "P3e", "" };
  Constant<CxxUtils::Array<1> > m_P4e { this, "P4e", "" };
  Constant<CxxUtils::Array<1> > m_EtaFrontier { this, "EtaFrontier", "" };
  Constant<float> m_BarrelGranularity { this, "BarrelGranularity", "" };
  Constant<float> m_EndcapGranularity { this, "EndcapGranularity", "" };
};

#endif
