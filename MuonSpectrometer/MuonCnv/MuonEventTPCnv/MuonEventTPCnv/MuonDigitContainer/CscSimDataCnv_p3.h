/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACNV_P3_H
#define CSCSIMDATACNV_P3_H

#include "MuonSimData/CscSimData.h"
#include "CscSimData_p3.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p3.h"
#include "CscMcDataCnv_p1.h"

class MsgStream;

class CscSimDataCnv_p3 : public T_AthenaPoolTPCnvBase<CscSimData, Muon::CscSimData_p3> {

  public:
    CscSimDataCnv_p3() {};
    void persToTrans( const Muon::CscSimData_p3 * persObj, CscSimData * transObj, MsgStream & log);
    void transToPers( const CscSimData * transObj, Muon::CscSimData_p3 * persObj, MsgStream & log);

  private:
    HepMcParticleLinkCnv_p3 m_mcpartlinkCnv;
    CscMcDataCnv_p1 m_mcdataCnv;

};

#endif
