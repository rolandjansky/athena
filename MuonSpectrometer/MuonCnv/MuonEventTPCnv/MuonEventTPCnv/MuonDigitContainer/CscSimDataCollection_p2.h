/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACOLLECTION_P2_H
#define CSCSIMDATACOLLECTION_P2_H

#include <vector>
#include "CscSimData_p2.h"
#include "Identifier/Identifier.h"

namespace Muon {
    class CscSimDataCollection_p2 {

      public:
	CscSimDataCollection_p2() {};
        std::vector<std::pair<Identifier32::value_type, CscSimData_p2> > m_cscsimdata;
    };
}
#endif
