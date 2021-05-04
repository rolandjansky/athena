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
		TTree* tree,
	 	std::string containername,
	 	MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_TgcIdHelper(0),
    m_TGC_nrdo(0),
    m_TGC_rdo_stationName(0),
    m_TGC_rdo_stationEta(0),
    m_TGC_rdo_stationPhi(0),
    m_TGC_rdo_gas_gap(0),
    m_TGC_rdo_isStrip(0),
    m_TGC_rdo_channel(0),
    m_TGC_rdo_localPosX(0),
    m_TGC_rdo_localPosY(0),
    m_TGC_rdo_globalPosX(0),
    m_TGC_rdo_globalPosY(0),
    m_TGC_rdo_globalPosZ(0)
  {
    setHelper(idhelper);
  }

  ~TGCRDOVariables()
  {
    deleteVariables();
  }
  void setTgcCabling(const ITGCcablingSvc* cabling){
    m_tgcCabling=cabling;
  }
  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_TgcIdHelper = dynamic_cast<const TgcIdHelper*>(idhelper);
    if(m_TgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to TgcIdHelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const TgcIdHelper* m_TgcIdHelper;
  const ITGCcablingSvc* m_tgcCabling;

  int m_TGC_nrdo;
  std::vector<std::string> m_TGC_rdo_stationName;
  std::vector<int> m_TGC_rdo_stationEta;
  std::vector<int> m_TGC_rdo_stationPhi;
  std::vector<int> m_TGC_rdo_gas_gap;
  std::vector<int> m_TGC_rdo_isStrip;
  std::vector<int> m_TGC_rdo_channel;
  std::vector<double> m_TGC_rdo_localPosX;
  std::vector<double> m_TGC_rdo_localPosY;
  std::vector<double> m_TGC_rdo_globalPosX;
  std::vector<double> m_TGC_rdo_globalPosY;
  std::vector<double> m_TGC_rdo_globalPosZ;


};

#endif // TGCRDOVARIABLES_H
