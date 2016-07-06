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
#ifndef TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTS_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTS_P1_H

#include <vector>

class TrigTrackCounts_p1 
{
  friend class TrigTrackCountsCnv_p1;

 public:
  
  TrigTrackCounts_p1() {}
  virtual ~TrigTrackCounts_p1(){}
 
  protected:

  std::vector<float> m_z0pcnt;
  std::vector<float> m_phi0cnt;
  std::vector<float> m_etacnt;
  std::vector<float> m_ptcnt;
  int m_trkcnt;
  int m_pixcnt;
  int m_sctcnt;

};

#endif
