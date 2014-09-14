/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERINGDICT_H
#define TRIGSTEERINGDICT_H

#include "TrigSteeringEvent/Chain.h"
#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"

void dummy_function_for_TrigSteering_which_forces_dictionaries_generation 
(   std::vector<HLT::Chain*> chain_vec,
    std::vector<const HLT::Chain*> chain_const_vec,
    std::vector<HLT::Signature*> sig_vec,
    std::vector<const HLT::Signature*> sig_const_vec,
    std::vector<HLT::Sequence*> seq_vec,
    std::vector<const HLT::Sequence*> seq_const_vec){

    std::vector<HLT::Chain*> vec1 = chain_vec;
    std::vector<const HLT::Chain*> vec_const1 = chain_const_vec;
    std::vector<HLT::Signature*> vec2 = sig_vec;
    std::vector<const HLT::Signature*> vec_const2 = sig_const_vec;
    std::vector<HLT::Sequence*> vec3 = seq_vec;
    std::vector<const HLT::Sequence*> vec_const3 = seq_const_vec;

}


#endif
