/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACOLLECTION_P2_H
#define MUONSIMDATACOLLECTION_P2_H

#include <vector>
#include "MuonSimData_p2.h"
#include "Identifier/Identifier.h"

namespace Muon {
    class MuonSimDataCollection_p2 {

      public:
	MuonSimDataCollection_p2() {};
        std::vector<std::pair<Identifier32::value_type, MuonSimData_p2> > m_muonsimdata;
    };
}
#endif
