/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "eformat/SourceIdentifier.h"

#include "TrigT1ResultByteStream/CTPSrcIdMap.h"

using eformat::helper::SourceIdentifier;


CTPSrcIdMap::CTPSrcIdMap() {

}

uint32_t CTPSrcIdMap::getRodID() {
  SourceIdentifier helpID( eformat::TDAQ_CTP, 0 );
  return helpID.code();
}

uint32_t CTPSrcIdMap::getRobID( uint32_t /*rod_id*/ ) {
  SourceIdentifier helpID( eformat::TDAQ_CTP, 0 );
  return helpID.code();
}

uint32_t CTPSrcIdMap::getRosID( uint32_t rob_id ) {
  SourceIdentifier id = SourceIdentifier( rob_id );
  SourceIdentifier id2 = SourceIdentifier( id.subdetector_id(), 0 );
  return id2.code();
}

uint32_t CTPSrcIdMap::getDetID( uint32_t ros_id ) {
  SourceIdentifier id = SourceIdentifier( ros_id );
  SourceIdentifier id2 = SourceIdentifier( id.subdetector_id(), 0 );
  return id2.code();
}
