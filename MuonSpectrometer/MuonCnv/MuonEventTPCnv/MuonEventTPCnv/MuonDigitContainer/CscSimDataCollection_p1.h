/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACOLLECTION_P1_H
#define CSCSIMDATACOLLECTION_P1_H

#include <vector>
#include "CscSimData_p1.h"
#include "Identifier/Identifier.h"

namespace Muon {
    class CscSimDataCollection_p1 {

      public:
	CscSimDataCollection_p1() {};
        std::vector<std::pair<Identifier32::value_type, CscSimData_p1> > m_cscsimdata;
    };
}
#endif
