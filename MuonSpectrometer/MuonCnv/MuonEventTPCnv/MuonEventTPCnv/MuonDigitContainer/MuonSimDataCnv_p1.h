/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACNV_P1_H
#define MUONSIMDATACNV_P1_H

#include "MuonSimData/MuonSimData.h"
#include "MuonSimData_p1.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "MuonMCDataCnv_p1.h"

class MsgStream;

class MuonSimDataCnv_p1 : public T_AthenaPoolTPCnvBase<MuonSimData, Muon::MuonSimData_p1> {

  public:
    MuonSimDataCnv_p1() {};
    void persToTrans( const Muon::MuonSimData_p1 * persObj, MuonSimData * transObj, MsgStream & log);
    void transToPers( const MuonSimData * transObj, Muon::MuonSimData_p1 * persObj, MsgStream & log);

  private:
    HepMcParticleLinkCnv_p1 m_mcpartlinkCnv;
    MuonMCDataCnv_p1 m_mcdataCnv;

};

#endif
