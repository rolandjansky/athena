/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSDOVARIABLES_H
#define CSCSDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>

class CscSDOVariables : public ValAlgVariables
{
 public:
  CscSDOVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MuonIdHelper* idhelper,
                  TTree* tree, const std::string & containername, MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~CscSDOVariables()=default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:
  const CscIdHelper* m_CscIdHelper;

  void setHelper(const MuonIdHelper* idhelper){
    m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
    if(!m_CscIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to CscIdHelper failed");
       throw std::runtime_error("Cast failed in CscSDOVariables::setHelper");
    }
  }

  StatusCode clearVariables();
  void deleteVariables(){};

  int m_csc_nsdo{};
  std::vector<std::string> m_csc_sdo_stationName;
  std::vector<int> m_csc_sdo_stationEta;
  std::vector<int> m_csc_sdo_stationPhi;
  std::vector<int> m_csc_sdo_channel;
  std::vector<int> m_csc_sdo_chamberLayer;
  std::vector<int> m_csc_sdo_wireLayer;
  std::vector<int> m_csc_sdo_Strip;
  std::vector<int>    m_csc_sdo_word;
  std::vector<int>    m_csc_sdo_barcode;
  std::vector<double> m_csc_sdo_globalPosX;
  std::vector<double> m_csc_sdo_globalPosY;
  std::vector<double> m_csc_sdo_globalPosZ;
  std::vector<double> m_csc_sdo_charge;
  std::vector<double> m_csc_sdo_localPosX;
  std::vector<double> m_csc_sdo_localPosY;

};

#endif // CSCSDOVariables_H
