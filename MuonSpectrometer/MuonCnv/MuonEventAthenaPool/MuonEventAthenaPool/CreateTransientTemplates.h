#ifndef MUONEVENTATHENAPOOL_CREATETRANSIENTTEMPLATES_H
#define MUONEVENTATHENAPOOL_CREATETRANSIENTTEMPLATES_H

#include "MuonRDO/RpcPadContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TPTools/TPConverter.h"
#include "MuonEventTPCnv/CreateTransientTemplates.h"
namespace TPCnv {


  template<> class CreateTransient<RpcPadContainer> {
    public: static std::unique_ptr<RpcPadContainer> create();
  };


  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcCoinData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcCoinData> >> create();
  };

}

#endif

