/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamCnvSvcBase/SrcIdMap.h" 

#include "eformat/SourceIdentifier.h" 

// implementation of SrcIdMap
//    This is only provided as an example. Each detector should 
//    implement the specific mapping between the identifiers.
//    .i.e, how each lower level fragments should be mapped onto an 
//    high level fragment, and especially, how the Module_ID field 
//    should be defined.

//  Modified Oct 27, 2002
//   use SourceID class to make Source ID. 

SrcIdMap::SrcIdMap ()
{ 
}


uint32_t SrcIdMap::getRobID (uint32_t rod_id)
{
  return rod_id;
}


uint32_t SrcIdMap::getRosID (uint32_t rob_id)
{
  //  Set module ID = 0 
  //    ALL ROB goes to one ROS 
  eformat::helper::SourceIdentifier id  = eformat::helper::SourceIdentifier(rob_id);
  eformat::helper::SourceIdentifier id2 = eformat::helper::SourceIdentifier(id.subdetector_id(),0);
  return id2.code();
}


uint32_t SrcIdMap::getDetID  (uint32_t ros_id) 
{
  //  Change Module Type to SUBDET  and set module ID = 0 
  //    ALL ROS goes to one SUBDET 
  eformat::helper::SourceIdentifier id  = eformat::helper::SourceIdentifier(ros_id);
  eformat::helper::SourceIdentifier id2 = eformat::helper::SourceIdentifier(id.subdetector_id(),0);
  return id2.code();
}

