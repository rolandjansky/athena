/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCMCDATA_P1_H
#define CSCMCDATA_P1_H

namespace Muon {
    class CscMcData_p1 {

      public:
	CscMcData_p1() : m_energy(-9999.), m_ypos(-9999.), m_zpos(-9999.), m_charge(0.0) {};
	friend class MuonMCDataCnv_p1;
        float m_energy;
        float m_ypos;
        float m_zpos;
        float m_charge;
    };
}
#endif
