/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCPRDVARIABLES_H
#define CSCPRDVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class CSCPRDVariables : public ValAlgVariables
{
 public:
  CSCPRDVariables(StoreGateSvc* evtStore,
                       const MuonGM::MuonDetectorManager* detManager,
                       const MuonIdHelper* idhelper,
                       TTree* tree,
						  	  const std::string & containername,
						 	  MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~CSCPRDVariables() = default;
  
  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
    if(!m_CscIdHelper) {
      throw std::runtime_error("casting IdHelper to CscIdHelper failed");
    }
  }

  void deleteVariables(){};
  StatusCode clearVariables();

  const CscIdHelper* m_CscIdHelper{};

  int m_CSC_PRD_nPRDs{};
  std::vector<std::string> m_CSC_PRD_stationName;
  std::vector<int> m_CSC_PRD_stationEta;
  std::vector<int> m_CSC_PRD_stationPhi;
  std::vector<int> m_CSC_PRD_channel;
  std::vector<int> m_CSC_PRD_chlayer;
  std::vector<int> m_CSC_PRD_wlayer;
  std::vector<int> m_CSC_PRD_measuresPhi;
  std::vector<int> m_CSC_PRD_strip;
  std::vector<int> m_CSC_PRD_charge;
  std::vector<int> m_CSC_PRD_time;

  std::vector<double> m_CSC_PRD_globalPosX;
  std::vector<double> m_CSC_PRD_globalPosY;
  std::vector<double> m_CSC_PRD_globalPosZ;

  std::vector<double> m_CSC_PRD_localPosX;
  std::vector<double> m_CSC_PRD_localPosY;

};

#endif // CSCPRDVARIABLE_H
