/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RAWDATACONTAINERCNV_P3_H
#define MM_RAWDATACONTAINERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/MM_RawDataCnv_p3.h"
#include "MuonEventTPCnv/MuonRDO/MM_RawDataContainer_p3.h"

#include <iostream>

class MmIdHelper;

namespace Muon{
  class MM_RawDataContainerCnv_p3 : public T_AthenaPoolTPCnvBase<Muon::MM_RawDataContainer, Muon::MM_RawDataContainer_p3>
  {
    public:
      MM_RawDataContainerCnv_p3() : m_idHelper(0) {}

      virtual void	persToTrans(const Muon::MM_RawDataContainer_p3* persCont,
          Muon::MM_RawDataContainer* transCont,
          MsgStream &log) override;
      virtual void	transToPers(const Muon::MM_RawDataContainer* transCont,
          Muon::MM_RawDataContainer_p3* persCont,
          MsgStream &log) override;

      virtual Muon::MM_RawDataContainer* createTransient(const Muon::MM_RawDataContainer_p3* persObj, MsgStream& log) override final;    
    
      void initialize(const MmIdHelper* idHelper);
    
    private:
      const MmIdHelper *m_idHelper;
      MM_RawDataCnv_p3  m_chanCnv;
  };
}


#endif


