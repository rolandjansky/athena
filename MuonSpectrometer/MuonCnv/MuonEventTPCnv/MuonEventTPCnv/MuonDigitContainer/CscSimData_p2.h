/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATA_P2_H
#define CSCSIMDATA_P2_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"
#include "CscMcData_p1.h"

namespace Muon {
    class CscSimData_p2 {

      public:
	CscSimData_p2() : m_word(0) {};
	friend class CscSimDataCnv_p2;
	int m_word;
	std::vector < std::pair < HepMcParticleLink_p2, CscMcData_p1 > > m_deposits;

    };

}
#endif
