/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    // constructor 
  CaloTopoEMsshape(const std::string& type, const std::string& name, const IInterface* parent);
  // destructor 
  ~CaloTopoEMsshape();
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
  // f(x) = P0 + atan(P1*(x-0.5)) + P2(x-0.5) + P3*|x-0.5| + P4
  // x is the position on the cell (0<x<1)
  // Tuned on 100 GeV electrons, no energy dependence
 CaloRec::Array<1> m_P0;
 CaloRec::Array<1> m_P1;
 CaloRec::Array<1> m_P2;
 CaloRec::Array<1> m_P3;
 CaloRec::Array<1> m_P4;
 CaloRec::Array<1> m_EtaFrontier;
 float m_Granularity;

};

#endif
