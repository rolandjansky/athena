/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCRAWDATACNV_P1_H
#define MUON_CSCRAWDATACNV_P1_H

/*
Transient/Persistent converter for CscRawData class
Author: Marcin Nowak
        CERN, January 2006
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"  
#include "MuonEventAthenaPool/CscRawData_p1.h"
#include "MuonRDO/CscRawData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class MsgStream;

class CscRawDataCnv_p1  : public T_AthenaPoolTPCnvBase<CscRawData, CscRawData_p1>
{
public:
  CscRawDataCnv_p1() {}
  
  virtual void		persToTrans(const CscRawData_p1* persObj, CscRawData* transObj, MsgStream &log);
  virtual void		transToPers(const CscRawData* transObj, CscRawData_p1* persObj, MsgStream &log);
  void setMuonIdHelperSvc(Muon::IMuonIdHelperSvc* muonIdHelper);
private:
  Muon::IMuonIdHelperSvc* m_idHelp = nullptr;
};
inline void CscRawDataCnv_p1::setMuonIdHelperSvc(Muon::IMuonIdHelperSvc* muonIdHelper) {
  m_idHelp = muonIdHelper;
}

#endif

