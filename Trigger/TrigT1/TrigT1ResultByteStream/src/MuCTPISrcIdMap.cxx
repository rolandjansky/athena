/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "eformat/SourceIdentifier.h"

#include "MuCTPISrcIdMap.h"

using eformat::helper::SourceIdentifier;


uint32_t MuCTPISrcIdMap::getRodID() const {
  SourceIdentifier helpID( eformat::TDAQ_MUON_CTP_INTERFACE, 0 );
  return helpID.code();
}

uint32_t MuCTPISrcIdMap::getRobID( uint32_t /*rod_id*/ ) const {
  SourceIdentifier helpID( eformat::TDAQ_MUON_CTP_INTERFACE, 0 );
  return helpID.code();
}

uint32_t MuCTPISrcIdMap::getRosID( uint32_t rob_id ) const {
  SourceIdentifier id = SourceIdentifier( rob_id );
  SourceIdentifier id2 = SourceIdentifier( id.subdetector_id(), 0 );
  return id2.code();
}

uint32_t MuCTPISrcIdMap::getDetID( uint32_t ros_id ) const {
  SourceIdentifier id = SourceIdentifier( ros_id );
  SourceIdentifier id2 = SourceIdentifier( id.subdetector_id(), 0 );
  return id2.code();
}
