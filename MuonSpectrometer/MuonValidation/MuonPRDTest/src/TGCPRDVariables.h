/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCPRDVARIABLES_H
#define TGCPRDVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class TGCPRDVariables : public ValAlgVariables
{
 public:
  TGCPRDVariables(StoreGateSvc* evtStore,
                       const MuonGM::MuonDetectorManager* detManager,
                       const MuonIdHelper* idhelper,
                       TTree* tree,
						  	  const std::string & containername,
						 	  MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~TGCPRDVariables() = default;
  
  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_TgcIdHelper = dynamic_cast<const TgcIdHelper*>(idhelper);
    if(!m_TgcIdHelper) {
      throw std::runtime_error("casting IdHelper to TgcIdHelper failed");
    }
  }

  void deleteVariables(){};
  StatusCode clearVariables();

  const TgcIdHelper* m_TgcIdHelper{};

  int m_TGC_PRD_nPRDs{};
  std::vector<std::string> m_TGC_PRD_stationName;
  std::vector<int> m_TGC_PRD_stationEta;
  std::vector<int> m_TGC_PRD_stationPhi;
  std::vector<int> m_TGC_PRD_channel;
  std::vector<int> m_TGC_PRD_gasGap;
  std::vector<int> m_TGC_PRD_isStrip;

  std::vector<double> m_TGC_PRD_globalPosX;
  std::vector<double> m_TGC_PRD_globalPosY;
  std::vector<double> m_TGC_PRD_globalPosZ;

  std::vector<double> m_TGC_PRD_localPosX;
  std::vector<double> m_TGC_PRD_localPosY;

};

#endif // TGCPRDVARIABLE_H
