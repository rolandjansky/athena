/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREVENTATHENAPOOL_MDTCSMCONTAINER_P1_H
#define MUONREVENTATHENAPOOL_MDTCSMCONTAINER_P1_H

// This container provides acces to the MDT RDOs
// Stefano Rosati, Mar 2003

#include <vector>
#include "MuonEventAthenaPool/MdtCsm_p1.h"

class MdtCsmContainer_p1
   : public std::vector<MdtCsm_p1> {
public:  
       MdtCsmContainer_p1() {}; 
       friend class MdtCsmContainerCnv_p1;
};

#endif     // MUONREVENTATHENAPOOL_MDTCSMCONTAINER_P1_H
