/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATA_P3_H
#define CSCSIMDATA_P3_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p3.h"
#include "CscMcData_p1.h"

namespace Muon {
    class CscSimData_p3 {

      public:
	CscSimData_p3() : m_word(0) {};
	friend class CscSimDataCnv_p3;
	int m_word;
	std::vector < std::pair < HepMcParticleLink_p3, CscMcData_p1 > > m_deposits;

    };

}
#endif
