/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACNV_P2_H
#define MUONSIMDATACNV_P2_H

#include "MuonSimData/MuonSimData.h"
#include "MuonSimData_p2.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"
#include "MuonMCDataCnv_p1.h"

class MsgStream;

class MuonSimDataCnv_p2 : public T_AthenaPoolTPCnvBase<MuonSimData, Muon::MuonSimData_p2> {

  public:
    MuonSimDataCnv_p2() {};
    void persToTrans( const Muon::MuonSimData_p2 * persObj, MuonSimData * transObj, MsgStream & log);
    void transToPers( const MuonSimData * transObj, Muon::MuonSimData_p2 * persObj, MsgStream & log);

  private:
    HepMcParticleLinkCnv_p2 m_mcpartlinkCnv;
    MuonMCDataCnv_p1 m_mcdataCnv;

};

#endif
