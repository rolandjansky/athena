/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACONTAINERCNV_P1_H
#define STGC_RAWDATACONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataCnv_p1.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainer_p1.h"

#include <iostream>

class sTgcIdHelper;

namespace TPCnv {
  template<> class CreateTransient< Muon::STGC_RawDataContainer > {
    public: static std::unique_ptr< Muon::STGC_RawDataContainer > create() { return nullptr;}
  };
}

namespace Muon{
    
  /** */
  class STGC_RawDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<Muon::STGC_RawDataContainer, Muon::STGC_RawDataContainer_p1>
  {
    public:
      STGC_RawDataContainerCnv_p1() : m_idHelper(0) {}

      virtual void	persToTrans(const Muon::STGC_RawDataContainer_p1* persCont,
          Muon::STGC_RawDataContainer* transCont,
          MsgStream &log) ;
      virtual void	transToPers(const Muon::STGC_RawDataContainer* transCont,
          Muon::STGC_RawDataContainer_p1* persCont,
          MsgStream &log) ;

      virtual Muon::STGC_RawDataContainer* createTransient(const Muon::STGC_RawDataContainer_p1* persObj, MsgStream& log) override final;    
    
      void initialize(const sTgcIdHelper* idHelper);
    
    private:
      const sTgcIdHelper *m_idHelper;
      STGC_RawDataCnv_p1  m_chanCnv;
  };
}


#endif


