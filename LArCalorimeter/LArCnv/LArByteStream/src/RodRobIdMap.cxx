/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArByteStream/src/RodRobIdMap.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2020
 * @brief ROD->ROB mapping part of Hid2RESrcID.
 */


#include "LArByteStream/RodRobIdMap.h"
#include "eformat/SourceIdentifier.h" 


/** mapping SrcID from ROD to ROB
 */ 
uint32_t RodRobIdMap::getRobID( uint32_t rod_id) const
{
  //  Change Module Type to ROB 
  using eformat::helper::SourceIdentifier; 
  SourceIdentifier  id  = SourceIdentifier(rod_id);
  SourceIdentifier  id2 = SourceIdentifier(id.subdetector_id(), id.module_id());
  return    id2.code();
}
