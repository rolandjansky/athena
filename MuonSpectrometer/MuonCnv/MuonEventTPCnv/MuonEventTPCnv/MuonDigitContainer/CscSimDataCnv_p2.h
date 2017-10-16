/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACNV_P2_H
#define CSCSIMDATACNV_P2_H

#include "MuonSimData/CscSimData.h"
#include "CscSimData_p2.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"
#include "CscMcDataCnv_p1.h"

class MsgStream;

class CscSimDataCnv_p2 : public T_AthenaPoolTPCnvBase<CscSimData, Muon::CscSimData_p2> {

  public:
    CscSimDataCnv_p2() {};
    void persToTrans( const Muon::CscSimData_p2 * persObj, CscSimData * transObj, MsgStream & log);
    void transToPers( const CscSimData * transObj, Muon::CscSimData_p2 * persObj, MsgStream & log);

  private:
    HepMcParticleLinkCnv_p2 m_mcpartlinkCnv;
    CscMcDataCnv_p1 m_mcdataCnv;

};

#endif
