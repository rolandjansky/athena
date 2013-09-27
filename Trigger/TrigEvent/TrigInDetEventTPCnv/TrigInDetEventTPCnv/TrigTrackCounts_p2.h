/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrackCounts_p1
 *
 * @brief persistent partner for TrigTrackCounts
 *
 * @author Regina Kwee      <Regina.Kwee@cern.ch>      - CERN 
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 **********************************************************************************/ 
#ifndef TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTS_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTS_P2_H 

#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"

class TrigTrackCounts_p2 
{
  friend class TrigTrackCountsCnv_p2;
  
 public:
  
  TrigTrackCounts_p2() {}
  virtual ~TrigTrackCounts_p2(){}
  
 private:
  
  TrigHisto2D_p1 m_z0_pt;
  TrigHisto2D_p1 m_eta_phi;

};

#endif
