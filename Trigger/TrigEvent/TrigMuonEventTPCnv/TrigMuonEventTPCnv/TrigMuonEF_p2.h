/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEF_p2
 *
 * @brief persistent partner for TrigMuonEF
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEF_p2.h 723205 2016-02-10 18:16:13Z ssnyder $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEF_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEF_P2_H

#include <stdint.h>
#include <string>
//#include "FourMom/P4IPtCotThPhiM.h"
#include "TrigMuonEvent/TrigMuonEF.h"
#include "AthenaPoolUtilities/TPObjRef.h"
#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"


class TrigMuonEF_p2
{
  friend class TrigMuonEFCnv_p2;

 public:
  
  TrigMuonEF_p2() : m_muonCode(0), m_charge(0), m_P4IPtCotThPhiM(0) {}
  virtual ~TrigMuonEF_p2(){}
  
  // private:
  int m_muonCode;
  std::string m_roi;
  float m_charge;

  P4IPtCotThPhiM_p1 m_P4IPtCotThPhiM;
};

#endif

