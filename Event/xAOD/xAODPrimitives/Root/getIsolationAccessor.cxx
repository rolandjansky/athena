/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <vector>
#include <memory>
#include<string>
// Local include(s):
#include "xAODPrimitives/tools/getIsolationAccessor.h"

///Helper macro to convert TYPE->NAME
#define DEFINE_ISO_ACCESSOR_STR(TYPE)                                 \
   #TYPE

typedef std::unique_ptr< SG::AuxElement::Accessor< float > > SmartPointer_t;

std::vector<SmartPointer_t> IsoCreateAccessorHelper(std::vector<std::string>&& input ){
  std::vector<SmartPointer_t> tmp;
  for (const std::string& str : input){
     tmp.emplace_back( std::move( std::make_unique<SG::AuxElement::Accessor<float>>(str) ) );
  }
  return tmp;
}
namespace xAOD {

   SG::AuxElement::Accessor< float >*
   getIsolationAccessor( Iso::IsolationType type ) {
     /// static const in block scope
     static const std::vector< SmartPointer_t > sAccessors=IsoCreateAccessorHelper({
        //EtCone variables      
        DEFINE_ISO_ACCESSOR_STR( etcone20 ),
        DEFINE_ISO_ACCESSOR_STR( etcone30 ),
        DEFINE_ISO_ACCESSOR_STR( etcone40 ),
        // Ptcone variables: 
        DEFINE_ISO_ACCESSOR_STR( ptcone20 ),
        DEFINE_ISO_ACCESSOR_STR( ptcone30 ),
        DEFINE_ISO_ACCESSOR_STR( ptcone40 ),
        // TopoEtcone variables:
        DEFINE_ISO_ACCESSOR_STR( topoetcone20 ),
        DEFINE_ISO_ACCESSOR_STR( topoetcone30 ),
        DEFINE_ISO_ACCESSOR_STR( topoetcone40 ),
        // Mini-Isolation (http://arxiv.org/abs/1007.2221) variables:
        DEFINE_ISO_ACCESSOR_STR(ptvarcone20),
        DEFINE_ISO_ACCESSOR_STR(ptvarcone30),
        DEFINE_ISO_ACCESSOR_STR(ptvarcone40),
        // neutral eflow
        DEFINE_ISO_ACCESSOR_STR(neflowisol20),
        DEFINE_ISO_ACCESSOR_STR(neflowisol30),
        DEFINE_ISO_ACCESSOR_STR(neflowisol40),
      });
     //End static const init      
     // Look up the element, throwing an exception for too large
     // enum values:
     return sAccessors.at( static_cast< size_t >( type ) ).get();
   }
} // namespace xAOD
