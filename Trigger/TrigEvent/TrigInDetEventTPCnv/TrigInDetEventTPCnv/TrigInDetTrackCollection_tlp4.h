/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackCollection_tlp1
 *
 * @brief "top level" persistent partner for TrigInDetTrackCollection
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTION_tlp4_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTION_tlp4_H

#include "TrigInDetTrackCollection_p1.h"
#include "TrigInDetTrack_p5.h"
#include "TrigInDetTrackFitPar_p3.h"

class TrigInDetTrackCollection_tlp4
{
 public:
  
  TrigInDetTrackCollection_tlp4() {}
  friend class TrigInDetTrackCollectionCnv_tlp4;
  
 private:
  
  std::vector< TrigInDetTrackCollection_p1 >  m_trigInDetTrackCollections;
  std::vector< TrigInDetTrackFitPar_p3 >      m_trigInDetTrackFitPars;
  std::vector< TrigInDetTrack_p5 >            m_trigInDetTracks;

};

#endif
