/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// NOTE: This is an exact copy of CscRawDataCnv_p3, since from p3 to p4 only the meaning of the m_hashId changed:
// Until p3, the m_hashId stores the identifier hash which encodes the geometrical position of the CSC
// From p4 onwards, the m_hashId stores the position of the CSC identifier in the vector of identifiers

#ifndef MUON_CSCRAWDATACNV_P4_H
#define MUON_CSCRAWDATACNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"  
#include "MuonEventAthenaPool/CscRawData_p4.h"
#include "MuonRDO/CscRawData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class MsgStream;

/**
Transient/Persistent converter for CscRawData class
*/
class CscRawDataCnv_p4  : public T_AthenaPoolTPCnvBase<CscRawData, CscRawData_p4>
{
public:
  CscRawDataCnv_p4() {}
  
  virtual void		persToTrans(const CscRawData_p4* persObj, CscRawData* transObj, MsgStream &log);
  virtual void		transToPers(const CscRawData* transObj, CscRawData_p4* persObj, MsgStream &log);
  void setMuonIdHelperSvc(Muon::IMuonIdHelperSvc* muonIdHelper);
private:
  Muon::IMuonIdHelperSvc* m_idHelp = nullptr;
};
inline void CscRawDataCnv_p4::setMuonIdHelperSvc(Muon::IMuonIdHelperSvc* muonIdHelper) {
  m_idHelp = muonIdHelper;
}

#endif
