/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEF_p1
 *
 * @brief persistent partner for TrigMuonEF
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEF_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEF_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEF_P1_H

#include <stdint.h>
#include <string>
#include "FourMom/P4IPtCotThPhiM.h"
#include "TrigMuonEvent/TrigMuonEF.h"
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEF_p1 
{
  friend class TrigMuonEFCnv;

 public:
  
  TrigMuonEF_p1() {}
  virtual ~TrigMuonEF_p1(){}
  
  //private:
  int m_muonCode;
  std::string m_roi;
  float m_charge;

  TPObjRef m_P4IPtCotThPhiM;
};

#endif
