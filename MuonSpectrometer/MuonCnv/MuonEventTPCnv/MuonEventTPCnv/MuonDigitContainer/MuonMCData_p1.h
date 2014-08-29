/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMCDATA_P1_H
#define MUONMCDATA_P1_H

namespace Muon {
    class MuonMCData_p1 {

      public:
	MuonMCData_p1():m_first(0.), m_second(0.) {};
	friend class MuonMCDataCnv_p1;
	double m_first;
	float m_second;
    };
}
#endif
