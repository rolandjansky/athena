// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    IRoiDecsriptorStorage.h        
//
//
//   $Id: RoiDescriptorStoreAuxInfo.h v0.0  Wed 11 Nov 2015 14:36:15 CET sutt $ 
#ifndef XAODTRIGGER_ROIDESCRIPTORSTORE_H
#define XAODTRIGGER_ROIDESCRIPTORSTORE_H

// Local include(s):
#include "xAODTrigger/versions/RoiDescriptorStore_v1.h"

namespace xAOD {
   /// Define the latest version of the serialised RoiDescriptor storage
   typedef RoiDescriptorStore_v1 RoiDescriptorStore;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"

CLASS_DEF( xAOD::RoiDescriptorStore , 165659280 , 1 )

#endif // XAODTRIGGER_ROIDESCRIPTORSTORE_H
