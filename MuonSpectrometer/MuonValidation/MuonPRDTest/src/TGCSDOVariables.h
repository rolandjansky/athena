/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSDOVARIABLES_H
#define TGCSDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>

class TgcSDOVariables : public ValAlgVariables
{
 public:
  TgcSDOVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MuonIdHelper* idhelper,
                  TTree* tree, const std::string & containername, MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~TgcSDOVariables()=default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:
  const TgcIdHelper* m_tgcIdHelper;

  void setHelper(const MuonIdHelper* idhelper){
    m_tgcIdHelper = dynamic_cast<const TgcIdHelper*>(idhelper);
    if(!m_tgcIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to TgcIdHelper failed");
       throw std::runtime_error("Cast failed in TgcSDOVariables::setHelper");
    }
  }

  StatusCode clearVariables();
  void deleteVariables(){};

  int m_tgc_nsdo{};
  std::vector<std::string> m_tgc_sdo_stationName;
  std::vector<int> m_tgc_sdo_stationEta;
  std::vector<int> m_tgc_sdo_stationPhi;
  std::vector<int> m_tgc_sdo_gas_gap;
  std::vector<int> m_tgc_sdo_channel;
  std::vector<int> m_tgc_sdo_isStrip;

  std::vector<int>    m_tgc_sdo_word;
  std::vector<int>    m_tgc_sdo_barcode;
  std::vector<double> m_tgc_sdo_globalPosX;
  std::vector<double> m_tgc_sdo_globalPosY;
  std::vector<double> m_tgc_sdo_globalPosZ;
  std::vector<double> m_tgc_sdo_globaltime;

  std::vector<double> m_tgc_sdo_localPosX;
  std::vector<double> m_tgc_sdo_localPosY;

};

#endif // TGCSDOVariables_H
