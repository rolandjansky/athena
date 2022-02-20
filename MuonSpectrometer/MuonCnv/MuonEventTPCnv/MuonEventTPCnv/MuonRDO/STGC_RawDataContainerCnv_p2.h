/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACONTAINERCNV_P2_H
#define STGC_RAWDATACONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataCnv_p2.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainer_p2.h"

#include <iostream>

class sTgcIdHelper;

namespace Muon{
  class STGC_RawDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase<Muon::STGC_RawDataContainer, Muon::STGC_RawDataContainer_p2>
  {
    public:
      STGC_RawDataContainerCnv_p2() : m_idHelper(0) {}

      virtual void	persToTrans(const Muon::STGC_RawDataContainer_p2* persCont,
          Muon::STGC_RawDataContainer* transCont,
          MsgStream &log) override;
      virtual void	transToPers(const Muon::STGC_RawDataContainer* transCont,
          Muon::STGC_RawDataContainer_p2* persCont,
          MsgStream &log) override;

      virtual Muon::STGC_RawDataContainer* createTransient(const Muon::STGC_RawDataContainer_p2* persObj, MsgStream& log) override final;    
    
      void initialize(const sTgcIdHelper* idHelper);
    
    private:
      const sTgcIdHelper *m_idHelper;
      STGC_RawDataCnv_p2  m_chanCnv;
  };
}


#endif


