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
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackCollection_tlp3.h,v 1.3 demelian Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTION_TLP3_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTION_TLP3_H

#include "TrigInDetTrackCollection_p1.h"
#include "TrigInDetTrack_p4.h"
#include "TrigInDetTrackFitPar_p3.h"

class TrigInDetTrackCollection_tlp3
{
 public:
  
  TrigInDetTrackCollection_tlp3() {}
  friend class TrigInDetTrackCollectionCnv_tlp3;
  
 private:
  
  std::vector< TrigInDetTrackCollection_p1 >  m_trigInDetTrackCollections;
  std::vector< TrigInDetTrackFitPar_p3 >      m_trigInDetTrackFitPars;
  std::vector< TrigInDetTrack_p4 >            m_trigInDetTracks;

};

#endif
