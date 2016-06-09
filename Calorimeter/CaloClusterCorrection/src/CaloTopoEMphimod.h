/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  // constructor 
  CaloTopoEMphimod(const std::string& type, const std::string& name, const IInterface* parent);
  // destructor 
  ~CaloTopoEMphimod();
  // initialization
  //  virtual StatusCode initialize();
  // virtual method in CaloClusterCorrection
  virtual void makeTheCorrection(const EventContext& ctx,
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
  CaloRec::Array<1> m_P1b;
  CaloRec::Array<1> m_P2b;
  CaloRec::Array<1> m_P3b;
  CaloRec::Array<1> m_P4b;
  CaloRec::Array<1> m_P1e;
  CaloRec::Array<1> m_P2e;
  CaloRec::Array<1> m_P3e;
  CaloRec::Array<1> m_P4e;
  CaloRec::Array<1> m_EtaFrontier;
  float m_BarrelGranularity;
  float m_EndcapGranularity;

};

#endif
