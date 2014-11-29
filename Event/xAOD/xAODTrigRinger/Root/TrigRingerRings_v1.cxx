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
    
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigRingerRings_v1, long , RoIword, setRoIword)
     
    void TrigRingerRings_v1::setRings(const std::vector<float> &r){
        static Accessor< std::vector<float> > acc( "rings" );
        acc(*this)=r;
    }
    
    //const std::vector<float>& RingerRings_v1::rings(){
    //    static Accessor< std::vector<float> > acc( "rings" );
    //    return acc(*this);
    //}
   
    std::vector<float> TrigRingerRings_v1::rings() const{ 
        static Accessor< std::vector<float> > acc( "rings" );
        return acc(*this);
    }

 
    unsigned TrigRingerRings_v1::size() const{
        static Accessor< std::vector<float> > acc( "rings" );
        return acc(*this).size();
    }
    
    
}// namespace xAOD 
