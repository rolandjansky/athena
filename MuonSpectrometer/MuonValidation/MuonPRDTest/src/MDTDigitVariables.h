/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_MDTDigitVARIABLES_H
#define MuonPRDTEST_MDTDigitVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MdtDigitVariables : public ValAlgVariables
{
 public:
  MdtDigitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
							std::string containername,
							MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~MdtDigitVariables() = default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_MdtIdHelper = dynamic_cast<const MdtIdHelper*>(idhelper);
    if (!m_MdtIdHelper){
     throw std::runtime_error("Could not cast the MDTIdHelper");
    }
  }
  
  void deleteVariables(){};
  StatusCode clearVariables();
  
  const MdtIdHelper* m_MdtIdHelper;

  int m_MDT_nDigits;
  std::vector<std::string> m_MDT_dig_stationName;
  std::vector<int> m_MDT_dig_stationEta;
  std::vector<int> m_MDT_dig_stationPhi;
  std::vector<int> m_MDT_dig_multiplet;
  std::vector<int> m_MDT_dig_channel;
  std::vector<int> m_MDT_dig_stationEtaMin;
  std::vector<int> m_MDT_dig_stationEtaMax;
  std::vector<int> m_MDT_dig_stationPhiMin;
  std::vector<int> m_MDT_dig_stationPhiMax;
  std::vector<int> m_MDT_dig_numberOfMultilayers;
  std::vector<int> m_MDT_dig_multilayerMin;
  std::vector<int> m_MDT_dig_multilayerMax;
  std::vector<int> m_MDT_dig_tubeLayerMin;
  std::vector<int> m_MDT_dig_tubeLayerMax;
  std::vector<int> m_MDT_dig_tubeMin;
  std::vector<int> m_MDT_dig_tubeMax; 
  std::vector<int> m_MDT_dig_channelNumber;
  
  std::vector< double > m_MDT_dig_channelPosX;
  std::vector< double > m_MDT_dig_channelPosY;
  std::vector< double > m_MDT_dig_localPosX;
  std::vector< double > m_MDT_dig_localPosY;
  std::vector< double > m_MDT_dig_globalPosX;
  std::vector< double > m_MDT_dig_globalPosY;
  std::vector< double > m_MDT_dig_globalPosZ;
};

#endif // MuonPRDTEST_MDTDigitVARIABLES_H
