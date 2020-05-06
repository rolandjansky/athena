/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACOLLECTION_P3_H
#define CSCSIMDATACOLLECTION_P3_H

#include <vector>
#include "CscSimData_p3.h"
#include "Identifier/Identifier.h"

namespace Muon {
    class CscSimDataCollection_p3 {

      public:
	CscSimDataCollection_p3() {};
        std::vector<std::pair<Identifier32::value_type, CscSimData_p3> > m_cscsimdata;
    };
}
#endif
