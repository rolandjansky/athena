/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACOLLECTION_P3_H
#define MUONSIMDATACOLLECTION_P3_H

#include <vector>
#include "MuonSimData_p3.h"
#include "Identifier/Identifier.h"

namespace Muon {
    class MuonSimDataCollection_p3 {

      public:
	MuonSimDataCollection_p3() {};
        std::vector<std::pair<Identifier32::value_type, MuonSimData_p3> > m_muonsimdata;
    };
}
#endif
