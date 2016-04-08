/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeature_p1
 *
 * @brief persistent partner for MuonFeature
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeature_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURE_P1_H
#define TRIGMUONEVENTTPCNV_MUONFEATURE_P1_H

#include <stdint.h>
#include <string>
#include "TrigMuonEvent/MuonFeature.h"

class MuonFeature_p1 
{
  friend class MuonFeatureCnv;

 public:
  
  MuonFeature_p1() {}
  virtual ~MuonFeature_p1(){}
  
  //private:

  int   m_RoIId;
  int   m_saddress;
  float m_pt;
  float m_radius;
  float m_eta;
  float m_phi;
  float m_dir_phi;
  float m_zeta;
  float m_dir_zeta;
  float m_beta;

};

#endif
