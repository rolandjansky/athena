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
						 const std::string & containername,
						 MSG::Level msglvl) :
ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~TGCDigitVariables() = default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_TgcIdHelper = dynamic_cast<const TgcIdHelper*>(idhelper);
    if(m_TgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to TgcIdhelper failed");
       throw std::runtime_error("Casting failed in TGCDigitVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const TgcIdHelper* m_TgcIdHelper{};

  int m_TGC_nDigits{};
  std::vector<std::string> m_TGC_dig_stationName;
  std::vector<int> m_TGC_dig_stationEta;
  std::vector<int> m_TGC_dig_stationPhi;
  std::vector<int> m_TGC_dig_gas_gap;
  std::vector<int> m_TGC_dig_channel;
  std::vector<int> m_TGC_dig_isStrip;
  std::vector<double> m_TGC_dig_localPosX;
  std::vector<double> m_TGC_dig_localPosY;
  std::vector<double> m_TGC_dig_globalPosX;
  std::vector<double> m_TGC_dig_globalPosY;
  std::vector<double> m_TGC_dig_globalPosZ;
};

#endif // TGCDIGITVARIABLES_H
