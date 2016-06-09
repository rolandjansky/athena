/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOTOPOEMPHIOFF_H
#define CALOCLUSTERCORRECTION_CALOTOPOEMPHIOFF_H
/********************************************************************

NAME:     CaloTopoEMphioff.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  M.Boonekamp, N.Kerschen
CREATED:  March 2005

PURPOSE:  correction for the phi offset due to accordion structure
          base class: CaloClusterCorrection (AlgTool)

Updated:  March 12, 2005   (MB)
          corrections for the TopoCluster 
********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"
class CaloCluster;

class CaloTopoEMphioff : public CaloClusterCorrectionCommon
{

 public: 
  // constructor 
  CaloTopoEMphioff(const std::string& type, const std::string& name, const IInterface* parent);
  // destructor 
  ~CaloTopoEMphioff();
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
  // The energy dependence has been fitted in eta bins of 0.1. The function that have been found to best describe this dependence are:
  // f(x) = A*(sqrt(1/x)) + B   for   (0 < eta < 0.8) where x is the energy in GeV
  // f(x) = A*(1/x) + B  for  (0.8 < eta < 2.3) 
  // f(x) = A*x + B
  // For the fits, abs(offset) has been taken, so that the adequate sign should be given to the correction (-1) for (0.8 < eta < 2.3)
  // For the G4 data, the sign should be flipped for eta < 0
  // So : 
  // EdepA is A in 25 eta bins
  // EdepB is B in 25 eta bins
  // Granularity is the Granularity
  // EtaFrontier[i] is 0.8, 2.3, 2.5 for i=0,1,2
  CaloRec::Array<1> m_EdepA;
  CaloRec::Array<1> m_EdepB;
  CaloRec::Array<1> m_EtaFrontier;

  //std::vector<float> m_EdepA;
  //std::vector<float> m_EdepB;
  //std::vector<float> m_EtaFrontier;
  float m_Granularity;
  int m_FlipPhi;
  float m_EndcapOffset;

};

#endif
