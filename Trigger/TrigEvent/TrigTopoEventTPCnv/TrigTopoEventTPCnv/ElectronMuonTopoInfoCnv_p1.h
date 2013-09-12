/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTopoEventTPCnv
 * @Class  : ElectronMuonTopoInfoCnv_p1
 *
 * @brief transient persistent converter for ElectronMuonTopoInfo
 *
 ***********************************************************************************/
#ifndef TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFOCNV_P1_H
#define TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFOCNV_P1_H

#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfo_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class ElectronMuonTopoInfoCnv_p1 : public T_AthenaPoolTPCnvBase<ElectronMuonTopoInfo, ElectronMuonTopoInfo_p1>
{
 public:

  ElectronMuonTopoInfoCnv_p1() {}

  virtual void persToTrans(const ElectronMuonTopoInfo_p1* persObj, 
			   ElectronMuonTopoInfo* transObj, 
			   MsgStream &log);
  virtual void transToPers(const ElectronMuonTopoInfo* transObj, 
			   ElectronMuonTopoInfo_p1* persObj, 
			   MsgStream &log);

};


#endif
