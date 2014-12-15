/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATA_P1_H
#define CSCSIMDATA_P1_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"
#include "CscMcData_p1.h"

namespace Muon {
    class CscSimData_p1 {

      public:
	CscSimData_p1() : m_word(0) {};
	friend class CscSimDataCnv_p1;
	int m_word;
	std::vector < std::pair < HepMcParticleLink_p1, CscMcData_p1 > > m_deposits;

    };

}
#endif
