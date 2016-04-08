/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// create by: joao victor da fonseca pinto, joao.victor.da.fonseca.pinto@cern.ch

// STD incluse(s)
#include <iostream>
#include <vector>
// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// local include(s)
#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"

namespace xAOD{

    
    TrigRingerRings_v1::TrigRingerRings_v1():  SG::AuxElement()
    {
        // no things to be done!
    } 
   
    AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigRingerRings_v1, std::vector<float>, rings, setRings) 
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigRingerRings_v1, long , RoIword, setRoiWord)
    
 
    unsigned TrigRingerRings_v1::size() const{
        static Accessor< std::vector<float> > acc( "rings" );
        return acc(*this).size();
    }
    
    
}// namespace xAOD 
