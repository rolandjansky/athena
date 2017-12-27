/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_IN_DET_TRACK_COLLECTION_H
#define TRIG_IN_DET_TRACK_COLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "AthenaKernel/BaseInfo.h"

class TrigInDetTrackCollection : public DataVector<TrigInDetTrack> {

public:
  // Constructors: POOL needs default constructor
TrigInDetTrackCollection(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS)
  : DataVector<TrigInDetTrack> (ownPolicy),
    m_RoI_ID(-1) {};

  void RoI_ID ( const int roi) { m_RoI_ID = roi; }

  int RoI_ID() const { return m_RoI_ID;} 

private:
  int     m_RoI_ID;  
};

CLASS_DEF( TrigInDetTrackCollection , 1299522495 , 1 )

CLASS_DEF( DataVector<TrigInDetTrackCollection> , 1306764742 , 1 )

SG_BASE(TrigInDetTrackCollection, DataVector<TrigInDetTrack>);

#endif 
