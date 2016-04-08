/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeature_p1
 *
 * @brief persistent partner for TileMuFeature
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Apr. 16. 2008 
 **********************************************************************************/

#ifndef TRIGMUONEVENTTPCNV_TILEMUFEATURE_P1_H
#define TRIGMUONEVENTTPCNV_TILEMUFEATURE_P1_H

#include <stdint.h>
#include <string>


class TileMuFeature_p1
{

 friend class TileMuFeatureCnv;

 public:

  TileMuFeature_p1() {}
  virtual ~TileMuFeature_p1(){}

  ///private:
 
  float m_eta;
  float m_phi;
  std::vector<float> m_energy_deposited;
  float m_quality_factor;

  //float m_Pt_IDTrk, m_Eta_IDTrk, m_EtaTR_IDTrk;
  //float m_Phi_IDTrk, m_PhiTR_IDTrk, m_zPos_IDTrk;
  //int    m_IDTrk;
 
};

#endif

