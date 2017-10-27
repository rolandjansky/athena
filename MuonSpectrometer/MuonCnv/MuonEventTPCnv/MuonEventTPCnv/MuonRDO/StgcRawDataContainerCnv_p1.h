/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRAWDATACONTAINERCNV_p1_H
#define STGCRAWDATACONTAINERCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonRDO/sTGC_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/StgcRawDataCnv_p1.h"
#include "MuonEventTPCnv/MuonRDO/StgcRawDataContainer_p1.h"

#include <iostream>

class sTgcIdHelper;

namespace TPCnv {
  template<> class CreateTransient< Muon::sTGC_RawDataContainer > {
    public: static std::unique_ptr< Muon::sTGC_RawDataContainer > create() { return nullptr;}
  };
}

namespace Muon{
    
  /** */
  class StgcRawDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<Muon::sTGC_RawDataContainer, Muon::StgcRawDataContainer_p1>
  {
    public:
      StgcRawDataContainerCnv_p1() {}

      virtual void	persToTrans(const Muon::StgcRawDataContainer_p1* persCont,
          Muon::sTGC_RawDataContainer* transCont,
          MsgStream &log) ;
      virtual void	transToPers(const Muon::sTGC_RawDataContainer* transCont,
          Muon::StgcRawDataContainer_p1* persCont,
          MsgStream &log) ;

      virtual Muon::sTGC_RawDataContainer* createTransient(const Muon::StgcRawDataContainer_p1* persObj, MsgStream& log) override final;    
    
      void initialize(const sTgcIdHelper* idHelper) {m_sTgcId = idHelper;}
    
    private:
      const sTgcIdHelper *m_sTgcId;
    
  };
}


#endif


