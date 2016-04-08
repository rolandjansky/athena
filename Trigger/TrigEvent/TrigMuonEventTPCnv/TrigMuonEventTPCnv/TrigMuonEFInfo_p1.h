/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfo_p1
 *
 * @brief persistent partner for TrigMuonEFInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFO_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFO_P1_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFInfo_p1 
{
  friend class TrigMuonEFInfoCnv;

 public:

  TrigMuonEFInfo_p1() {}
  virtual ~TrigMuonEFInfo_p1(){}

  //private:
  unsigned short int m_roi;

  TPObjRef m_spectrometerTrack;
  TPObjRef m_extrapolatedTrack;
  TPObjRef m_combinedTrack;

};

#endif
