
#ifndef MUONEVENTTPCNV_CREATETRANSIENT_H
#define MUONEVENTTPCNV_CREATETRANSIENT_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TgcCoinDataContainer_tlp3.h"
#include "TgcCoinDataContainer_tlp2.h"
#include "TgcCoinDataContainer_tlp1.h"
#include "MuonTrigCoinData/MuonCoinDataContainer.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonTrigCoinData/RpcCoinData.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

//These are to fix a problem with compiling TPConverter using identifiable containers, if the design is changed in the future these can be removed

namespace TPCnv {


  template<> class CreateTransient<Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::TgcCoinData> >> {
    public: static std::unique_ptr<Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::TgcCoinData> >> create();
  };

  template<> class CreateTransient<Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::RpcCoinData> >> {
    public: static std::unique_ptr<Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::RpcCoinData> >> create();
  };

  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcPrepData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcPrepData> >> create(); };


  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscPrepData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscPrepData> >> create(); };

  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscStripPrepData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscStripPrepData> >> create(); };

  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >> create(); };

  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcPrepData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcPrepData> >> create(); };

  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MMPrepData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MMPrepData> >> create(); };

  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcCoinData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcCoinData> >> create(); };

  template<> class CreateTransient<MmDigitContainer> {
    public: static std::unique_ptr<MmDigitContainer> create(); };

  template<> class CreateTransient<sTgcDigitContainer> {
    public: static std::unique_ptr<sTgcDigitContainer> create(); };

  template<> class CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::sTgcPrepData> >> {
    public: static std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::sTgcPrepData> >> create(); };
}




#endif

