// emacs: this is -*- c++ -*-
//
//   @file    IRoiDecsriptorStorageAuxInfo_v1.h        
//
//            Class holding the data handled by RoiDescriptorStore_v1
//            This is the auxiliary class holding the serialised RoiDescriptor 
//            information in xAOD files.
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiDescriptorStoreAuxInfo_v1.h v0.0  Wed 11 Nov 2015 14:36:15 CET sutt $ 

#ifndef XAODTRIGGER_VERSIONS_ROIDESCRIPTORSTOREAUXINFO_V1_H
#define XAODTRIGGER_VERSIONS_ROIDESCRIPTORSTOREAUXINFO_V1_H

#include <stdint.h>
#include <vector>

// xAOD include
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   class RoiDescriptorStoreAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constructor
      RoiDescriptorStoreAuxInfo_v1();

   private:

     std::vector< std::vector<uint32_t> > serialised;

   }; // class RoiDescriptorStoreAuxInfo_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::RoiDescriptorStoreAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODTRIGGER_VERSIONS_ROIDESCRIPTORSTOREAUXINFO_V1_H
