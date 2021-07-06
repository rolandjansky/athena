/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// create by: joao victor da fonseca pinto, joao.victor.da.fonseca.pinto@cern.ch

// STD incluse(s)
#include <iostream>
#include <vector>
// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// local include(s)
#include "xAODTrigRinger/versions/TrigRingerRings_v2.h"

namespace xAOD{

    
    TrigRingerRings_v2::TrigRingerRings_v2():  SG::AuxElement()
    {
        // no things to be done!
    } 
   
    AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigRingerRings_v2, std::vector<float>, rings, setRings) 

    unsigned TrigRingerRings_v2::size() const{
        static const Accessor< std::vector<float> > acc( "rings" );
        return acc(*this).size();
    }

    AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigRingerRings_v2, TrigRingerRings_v2::EMClusterLink_t, emClusterLink, setEmClusterLink )


    /// This function can be used to conveniently access the EM cluster
    /// associated with the electron. It will return a null pointer if
    /// associated object can't be retrieved, but it will never fail in
    /// other way.
    /// @returns A pointer to the associated EM cluster, or a null
    /// if the object can't be found
    //
    const TrigEMCluster* TrigRingerRings_v2::emCluster() const {
       // The link accessor:
       static const Accessor< EMClusterLink_t > acc( "emClusterLink" );
   
       // Check if the link is available:
       if( ! acc.isAvailable( *this ) ) {
          return nullptr;
       }
       // Check if the link is valid:
       const EMClusterLink_t& link = acc( *this );
       if( ! link.isValid() ) {
          return nullptr;
       }
   
       // Return the pointer:
       return *link;
    }
    
}// namespace xAOD 
