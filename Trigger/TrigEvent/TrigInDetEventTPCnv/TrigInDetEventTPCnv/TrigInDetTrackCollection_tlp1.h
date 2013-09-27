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
 * $Id: TrigInDetTrackCollection_tlp1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTION_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTION_TLP1_H

#include "TrigInDetTrackCollection_p1.h"
#include "TrigInDetTrack_p1.h"
#include "TrigInDetTrack_p2.h"
#include "TrigInDetTrackFitPar_p1.h"
#include "TrigInDetTrackFitPar_p2.h"

class TrigInDetTrackCollection_tlp1
{
 public:
  
  TrigInDetTrackCollection_tlp1() {}
  friend class TrigInDetTrackCollectionCnv_tlp1;
  
 private:
  
  std::vector< TrigInDetTrackCollection_p1 >  m_trigInDetTrackCollections;
  std::vector< TrigInDetTrack_p1 >            m_trigInDetTracks;
  std::vector< TrigInDetTrackFitPar_p1 >      m_trigInDetTrackFitPars;
  std::vector< TrigInDetTrackFitPar_p2 >      m_trigInDetTrackFitPars_p2;
  std::vector< TrigInDetTrack_p2 >            m_trigInDetTracks_p2;

};

#endif
