// emacs: this is -*- c++ -*-
//
//   @file    IRoiDecsriptorStorageAuxInfo.h        
//
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiDescriptorStoreAuxInfo.h v0.0  Wed 11 Nov 2015 14:36:15 CET sutt $ 

#ifndef XAODTRIGGER_ROIDESCRIPTORSTOREAUXINFO_H
#define XAODTRIGGER_ROIDESCRIPTORSTOREAUXINFO_H

#include "xAODTrigger/versions/RoiDescriptorStoreAuxInfo_v1.h"

namespace xAOD {
   /// Define the latest version of the serialised RoiDescriptor collection 
   /// auxiliary store
   typedef RoiDescriptorStoreAuxInfo_v1 RoiDescriptorStoreAuxInfo;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::RoiDescriptorStoreAuxInfo , 72816317 , 1 )

#endif // XAODTRIGGER_ROIDESCRIPTORSTOREAUXINFO_H
