/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// local include(s)
#include "xAODTrigRinger/versions/TrigRNNOutput_v2.h"
#include <vector>

namespace xAOD{

    
    TrigRNNOutput_v2::TrigRNNOutput_v2(): SG::AuxElement()
    {
        // no things to be done!
    }
   
    AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigRNNOutput_v2, std::vector<float>, rnnDecision, setRnnDecision)

    void TrigRNNOutput_v2::setRnnDecision(float d){
        static const Accessor< std::vector<float> > acc( "rnnDecision" );
        acc(*this).push_back(d);
    }

    AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigRNNOutput_v2, TrigRNNOutput_v2::RingerLink_t, ringerLink, setRingerLink )


    /// This function can be used to conveniently access the TrigRingerRings
    /// associated with the rnn. It will return a null pointer if the
    /// associated object can't be retrieved, but it will never fail in any
    /// other way.
    ///
    /// @returns A pointer to the associated ringer, or a null pointer
    ///          if the object can't be found
    ///
    const TrigRingerRings* TrigRNNOutput_v2::ringer() const {
  
       // The link accessor:
       static const Accessor< RingerLink_t > acc( "ringerLink" );
  
       // Check if the link is available:
       if( ! acc.isAvailable( *this ) ) {
          return nullptr;
       }
  
       // Check if the link is valid:
       const RingerLink_t& link = acc( *this );
       if( ! link.isValid() ) {
          return nullptr;
       }
  
       // Return the pointer:
       return *link;
    }


}// namespace xAOD 
