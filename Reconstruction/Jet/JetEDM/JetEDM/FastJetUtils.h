// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEDM_FASTJETUTILS_H
#define JETEDM_FASTJETUTILS_H

#include "fastjet/JetDefinition.hh"
#include "xAODJet/JetContainerInfo.h"

namespace xAOD {
  
  namespace JetAlgorithmType {

    // conversion from fastjet
    const std::string& algName(fastjet::JetAlgorithm a);    
    ID algId(fastjet::JetAlgorithm a);

    // conversion to fastjet
    fastjet::JetAlgorithm fastJetDef(ID id);
    fastjet::JetAlgorithm fastJetDef(const std::string& name);


  }
}
#endif
