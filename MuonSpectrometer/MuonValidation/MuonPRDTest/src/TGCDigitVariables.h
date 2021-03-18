/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDIGITVARIABLES_H
#define TGCDIGITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class TGCDigitVariables : public ValAlgVariables
{
 public:
  TGCDigitVariables(StoreGateSvc* evtStore,
                   const MuonGM::MuonDetectorManager* detManager,
                   const MuonIdHelper* idhelper,
                   TTree* tree,
						 std::string containername,
						 MSG::Level msglvl) :
ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_TgcIdHelper(0),
    m_TGC_nDigits(0),
    m_TGC_dig_stationName(0),
    m_TGC_dig_stationEta(0),
    m_TGC_dig_stationPhi(0),
    m_TGC_dig_gas_gap(0),
    m_TGC_dig_channel(0),
    m_TGC_dig_isStrip(0)
  {
    setHelper(idhelper);
  }

  ~TGCDigitVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_TgcIdHelper = dynamic_cast<const TgcIdHelper*>(idhelper);
    if(m_TgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to TgcIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const TgcIdHelper* m_TgcIdHelper;

  int m_TGC_nDigits;
  std::vector<std::string> m_TGC_dig_stationName;
  std::vector<int> m_TGC_dig_stationEta;
  std::vector<int> m_TGC_dig_stationPhi;
  std::vector<int> m_TGC_dig_gas_gap;
  std::vector<int> m_TGC_dig_channel;
  std::vector<int> m_TGC_dig_isStrip;

};

#endif // TGCDIGITVARIABLES_H
