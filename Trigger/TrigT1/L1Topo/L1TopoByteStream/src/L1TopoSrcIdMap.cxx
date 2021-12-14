/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "eformat/SourceIdentifier.h"

#include "L1TopoSrcIdMap.h"

using eformat::helper::SourceIdentifier;


L1TopoSrcIdMap::L1TopoSrcIdMap() {

}

uint32_t L1TopoSrcIdMap::getRodID(int moduleId) const {
  SourceIdentifier helpID( eformat::TDAQ_CALO_TOPO_PROC, moduleId );
  return helpID.code();
}

uint32_t L1TopoSrcIdMap::getRodID() const {
  SourceIdentifier helpID( eformat::TDAQ_CALO_TOPO_PROC, 0 );
  return helpID.code();
}

uint32_t L1TopoSrcIdMap::getRobID( uint32_t rod_id ) const {
  //SourceIdentifier helpID( eformat::TDAQ_CALO_TOPO_PROC, 0 );
  //return helpID.code();
  return rod_id;
}

uint32_t L1TopoSrcIdMap::getRosID( uint32_t rob_id ) const {
  SourceIdentifier id = SourceIdentifier( rob_id );
  SourceIdentifier id2 = SourceIdentifier( id.subdetector_id(), 0 );
  return id2.code();
}

uint32_t L1TopoSrcIdMap::getDetID( uint32_t ros_id ) const {
  SourceIdentifier id = SourceIdentifier( ros_id );
  SourceIdentifier id2 = SourceIdentifier( id.subdetector_id(), 0 );
  return id2.code();
}
