// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFINFOTRACKCONTAINER_H
#define TRIGMUONEVENT_TRIGMUONEFINFOTRACKCONTAINER_H
/********************************************************************

 NAME:     TrigMuonEFInfoTrackContainer.h
 Package : offline/Trigger/TrigEvent/TrigMuonEvent

 AUTHORS:  Alexander Oh
 CREATED:  January 2009

 PURPOSE:  Collection of TrigMuonEFInfoTrack objects
********************************************************************/

// INCLUDE HEADER FILES:
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "AthenaKernel/BaseInfo.h"

class TrigMuonEFInfoTrackContainer : public DataVector<TrigMuonEFInfoTrack>
{
public:
  TrigMuonEFInfoTrackContainer( SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS )
    : DataVector<TrigMuonEFInfoTrack>( ownPolicy ) {}
  ~TrigMuonEFInfoTrackContainer() {}
  void print();
};

CLASS_DEF(TrigMuonEFInfoTrackContainer, 1264424340, 1)

SG_BASE(TrigMuonEFInfoTrackContainer, DataVector<TrigMuonEFInfoTrack>);

#endif // TRIGMUONEVENT_TRIGMUONEFINFOTRACKCONTAINER_H
