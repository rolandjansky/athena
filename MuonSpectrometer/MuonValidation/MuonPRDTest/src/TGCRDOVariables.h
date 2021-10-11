/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRDOVARIABLES_H
#define TGCRDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

#include <vector>

class TGCRDOVariables : public ValAlgVariables
{
 public:
 TGCRDOVariables(StoreGateSvc* evtStore,
		const MuonGM::MuonDetectorManager* detManager,
		const MuonIdHelper* idhelper,
    const ITGCcablingSvc* cabling_svc,
		TTree* tree,
	 	const std::string& containername,
	 	MSG::Level msglvl);

  ~TGCRDOVariables() = default;
 
  
  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:
  void deleteVariables(){}
  void setHelper(const MuonIdHelper* idhelper){
    m_TgcIdHelper = dynamic_cast<const TgcIdHelper*>(idhelper);
    if(!m_TgcIdHelper) {
       throw std::runtime_error("Casting IdHelper to TgcIdHelper failed");
    }
  }

  
  StatusCode clearVariables();

  const TgcIdHelper* m_TgcIdHelper{nullptr};
  const ITGCcablingSvc* m_tgcCabling{nullptr};

  int m_TGC_nrdo{0};
  std::vector<std::string> m_TGC_rdo_stationName{};
  std::vector<int> m_TGC_rdo_stationEta{};
  std::vector<int> m_TGC_rdo_stationPhi{};
  std::vector<int> m_TGC_rdo_gas_gap{};
  std::vector<int> m_TGC_rdo_isStrip{};
  std::vector<int> m_TGC_rdo_channel{};
  std::vector<double> m_TGC_rdo_localPosX{};
  std::vector<double> m_TGC_rdo_localPosY{};
  std::vector<double> m_TGC_rdo_globalPosX{};
  std::vector<double> m_TGC_rdo_globalPosY{};
  std::vector<double> m_TGC_rdo_globalPosZ{};


};

#endif // TGCRDOVARIABLES_H
