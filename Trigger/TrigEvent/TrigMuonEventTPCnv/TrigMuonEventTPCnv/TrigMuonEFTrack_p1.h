/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFTrack_p1
 *
 * @brief persistent partner for TrigMuonEFTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P1_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFTrack_p1 
{
  friend class TrigMuonEFTrackCnv;

 public:
  
  TrigMuonEFTrack_p1() {}
  virtual ~TrigMuonEFTrack_p1(){}
  
  //private:
  double m_charge;

  TPObjRef m_P4IPtCotThPhiM;
};

#endif
