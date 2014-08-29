/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACOLLECTION_P1_H
#define MUONSIMDATACOLLECTION_P1_H

#include <vector>
#include "MuonSimData_p1.h"
#include "Identifier/Identifier.h"

namespace Muon {
    class MuonSimDataCollection_p1 {

      public:
	MuonSimDataCollection_p1() {};
        std::vector<std::pair<Identifier32::value_type, MuonSimData_p1> > m_muonsimdata;
    };
}
#endif
