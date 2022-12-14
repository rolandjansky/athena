/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RoiDescriptorStore_v1.cxx         
//   
//
//   @author M.Sutton
// 
//
//   $Id: RoiDescriptorStore_v1.cxx, v0.0   Wed 11 Nov 2015 14:51:11 CET sutt $

// xAOD include
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include
#include "xAODTrigger/versions/RoiDescriptorStore_v1.h"


namespace xAOD {

  RoiDescriptorStore_v1::RoiDescriptorStore_v1()
    : SG::AuxElement() {
  }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( RoiDescriptorStore_v1,
				     std::vector< std::vector<uint32_t> >,
				     serialised, setSerialised )
  
  
} // namespace xAOD
