/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACNV_P1_H
#define CSCSIMDATACNV_P1_H

#include "MuonSimData/CscSimData.h"
#include "CscSimData_p1.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "CscMcDataCnv_p1.h"

class MsgStream;

class CscSimDataCnv_p1 : public T_AthenaPoolTPCnvBase<CscSimData, Muon::CscSimData_p1> {

  public:
    CscSimDataCnv_p1() {};
    void persToTrans( const Muon::CscSimData_p1 * persObj, CscSimData * transObj, MsgStream & log);
    void transToPers( const CscSimData * transObj, Muon::CscSimData_p1 * persObj, MsgStream & log);

  private:
    HepMcParticleLinkCnv_p1 m_mcpartlinkCnv;
    CscMcDataCnv_p1 m_mcdataCnv;

};

#endif
