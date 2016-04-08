/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeature_p2
 *
 * @brief persistent partner for TileMuFeature
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Apr. 16. 2008 
 **********************************************************************************/

#ifndef TRIGMUONEVENTTPCNV_TILEMUFEATURE_P2_H
#define TRIGMUONEVENTTPCNV_TILEMUFEATURE_P2_H

#include <stdint.h>
#include <string>


class TileMuFeature_p2
{

 friend class TileMuFeatureCnv_p2;

 public:

  TileMuFeature_p2() {}
  virtual ~TileMuFeature_p2(){}

  //private:
 
  float m_allTheFloats[3];
  // float m_eta;
  // float m_phi;
  std::vector<float> m_energy_deposited;
  // float m_quality_factor;

  //float m_Pt_IDTrk, m_Eta_IDTrk, m_EtaTR_IDTrk;
  //float m_Phi_IDTrk, m_PhiTR_IDTrk, m_zPos_IDTrk;
  //int    m_IDTrk;
 
};

#endif

