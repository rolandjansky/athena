/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RoiDescriptorStoreAuxInfo_v1.cxx         
//   
//
//   @author M.Sutton
// 
//
//   $Id: RoiDescriptorStoreAuxInfo_v1.cxx, v0.0   Wed 11 Nov 2015 14:51:11 CET sutt $

// Local include(s):
#include "xAODTrigger/versions/RoiDescriptorStoreAuxInfo_v1.h"

namespace xAOD {

   RoiDescriptorStoreAuxInfo_v1::RoiDescriptorStoreAuxInfo_v1()
      : AuxInfoBase() {

      AUX_VARIABLE( serialised );

   }

} // namespace xAOD
