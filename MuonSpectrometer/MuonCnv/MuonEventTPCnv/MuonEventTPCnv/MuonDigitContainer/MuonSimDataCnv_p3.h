/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACNV_P3_H
#define MUONSIMDATACNV_P3_H

#include "MuonSimData/MuonSimData.h"
#include "MuonSimData_p3.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p3.h"
#include "MuonMCDataCnv_p1.h"

class MsgStream;

class MuonSimDataCnv_p3 : public T_AthenaPoolTPCnvBase<MuonSimData, Muon::MuonSimData_p3> {

  public:
    MuonSimDataCnv_p3() {};
    void persToTrans( const Muon::MuonSimData_p3 * persObj, MuonSimData * transObj, MsgStream & log);
    void transToPers( const MuonSimData * transObj, Muon::MuonSimData_p3 * persObj, MsgStream & log);

  private:
    HepMcParticleLinkCnv_p3 m_mcpartlinkCnv;
    MuonMCDataCnv_p1 m_mcdataCnv;

};

#endif
