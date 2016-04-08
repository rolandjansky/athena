// emacs: this is -*- c++ -*-
//
//   @file    IRoiDecsriptorStorage_v1.h        
//
//            Interface to the raw serialised IRoiDescriptor information
//            This interface is used to store the RoiDescriptor information
//            in the xAOD and should only be used directly by experts, 
//            to deserialise to a collection of RoiDescriptors
//            regular users should only ever access the IRoiDescriptors 
//            through the TrigDecisionTool
//
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiDescriptorStore_v1.h v0.0  Wed 11 Nov 2015 14:36:15 CET sutt $ 

#ifndef XAODTRIGGER_VERSIONS_ROIDESCRIPTORSTORE_V1_H
#define XAODTRIGGER_VERSIONS_ROIDESCRIPTORSTORE_V1_H

#include <stdint.h>
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   class RoiDescriptorStore_v1 : public SG::AuxElement {

   public:
     /// Default constructor
     RoiDescriptorStore_v1();
     
     /// expose the navigation information (in serialised form)
     const std::vector< std::vector<uint32_t> >& serialised() const;

     /// sets the serialized form of the navigation from the argument
     void setSerialised( const std::vector< std::vector<uint32_t> >& value );
     
   }; // class RoiDescriptorStore_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::RoiDescriptorStore_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_ROIDESCRIPTORSTORE_V1_H
