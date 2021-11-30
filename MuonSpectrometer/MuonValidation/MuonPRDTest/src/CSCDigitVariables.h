/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCDIGITVARIABLES_H
#define CSCDIGITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class CSCDigitVariables : public ValAlgVariables
{
 public:
  CSCDigitVariables(StoreGateSvc* evtStore,
                   const MuonGM::MuonDetectorManager* detManager,
                   const MuonIdHelper* idhelper,
                   TTree* tree,
						 const std::string & containername,
						 MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~CSCDigitVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
    if(!m_CscIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to CscIdhelper failed");
       throw std::runtime_error("Bad cast in CSCDigitVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const CscIdHelper* m_CscIdHelper{};

  int m_CSC_nDigits{};
  std::vector<std::string> m_CSC_dig_stationName;
  std::vector<int> m_CSC_dig_stationEta;
  std::vector<int> m_CSC_dig_stationPhi;
  std::vector<int> m_CSC_dig_chlayer;
  std::vector<int> m_CSC_dig_wlayer;
  std::vector<int> m_CSC_dig_gas_gap;
  std::vector<int> m_CSC_dig_channel;
  std::vector< double > m_CSC_dig_localPosX;
  std::vector< double > m_CSC_dig_localPosY;
  std::vector< double > m_CSC_dig_globalPosX;
  std::vector< double > m_CSC_dig_globalPosY;
  std::vector< double > m_CSC_dig_globalPosZ;
};

#endif // CSCDIGITVARIABLES_H
