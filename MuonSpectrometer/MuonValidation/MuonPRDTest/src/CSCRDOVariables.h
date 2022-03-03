/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCRDOVARIABLES_H
#define CSCRDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include <vector>

class CSCRDOVariables : public ValAlgVariables
{
 public:
 CSCRDOVariables(StoreGateSvc* evtStore,
		            const MuonGM::MuonDetectorManager* detManager,
		            const MuonIdHelper* idhelper,
                TTree* tree,
                const std::string& containername,
                MSG::Level msglvl,     
                const Muon::ICSC_RDO_Decoder* rdo_decoder);
  
  ~CSCRDOVariables() =default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
    if(!m_CscIdHelper) {
       throw std::runtime_error("casting IdHelper to CscIdHelper failed");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const CscIdHelper* m_CscIdHelper{nullptr};
  const Muon::ICSC_RDO_Decoder*  m_rdo_decoder{nullptr};


  int m_Csc_nrdo{0};
  std::vector<std::string> m_Csc_rdo_stationName;
  std::vector<int> m_Csc_rdo_stationEta;
  std::vector<int> m_Csc_rdo_stationPhi;
  std::vector<int> m_Csc_rdo_channel;
  std::vector<int> m_Csc_rdo_chamberLayer;
  std::vector<int> m_Csc_rdo_wireLayer;
  std::vector<int> m_Csc_rdo_strip;
  std::vector<bool> m_Csc_rdo_measuresPhi;
  std::vector<int> m_Csc_rdo_time;
  std::vector<double> m_Csc_rdo_localPosX;
  std::vector<double> m_Csc_rdo_localPosY;
  std::vector<double> m_Csc_rdo_globalPosX;
  std::vector<double> m_Csc_rdo_globalPosY;
  std::vector<double> m_Csc_rdo_globalPosZ;

};

#endif // CSCRDOVARIABLES_H
