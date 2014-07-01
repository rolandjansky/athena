/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "eformat/SourceIdentifier.h"

#include "TrigT1ResultByteStream/L1SrcIdMap.h"

using eformat::helper::SourceIdentifier;


L1SrcIdMap::L1SrcIdMap() {

}

uint32_t L1SrcIdMap::getRodID() {
  SourceIdentifier helperID( eformat::TDAQ_LVL2, 0 );
  return helperID.code();
}

uint32_t L1SrcIdMap::getRobID( uint32_t rod_id ) {
  return rod_id;
}

uint32_t L1SrcIdMap::getRosID( uint32_t ) {
  SourceIdentifier helperID2( eformat::TDAQ_LVL2, 0 );
  return helperID2.code();
}

uint32_t L1SrcIdMap::getDetID  ( uint32_t ros_id) {
  SourceIdentifier helperID1( ros_id );
  SourceIdentifier helperID2( helperID1.subdetector_id(), 0 );
  return helperID2.code();
}
