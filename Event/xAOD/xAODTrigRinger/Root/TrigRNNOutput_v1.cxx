/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// local include(s)
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"
#include <vector>

namespace xAOD{

    
    TrigRNNOutput_v1::TrigRNNOutput_v1(): SG::AuxElement()
    {
    }
   
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigRNNOutput_v1, long , RoIword, setRoiWord)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigRNNOutput_v1, float , et, setEt)
    AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigRNNOutput_v1, std::vector<float>, decision, setDecision)

    void TrigRNNOutput_v1::setDecision(float d){
        static Accessor< std::vector<float> > acc( "decision" );
        acc(*this).push_back(d);
    }


}// namespace xAOD 
