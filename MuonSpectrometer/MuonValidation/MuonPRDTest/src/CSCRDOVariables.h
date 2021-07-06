/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCRDOVARIABLES_H
#define CSCRDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>

class CSCRDOVariables : public ValAlgVariables
{
 public:
 CSCRDOVariables(StoreGateSvc* evtStore,
		const MuonGM::MuonDetectorManager* detManager,
		const MuonIdHelper* idhelper,
		TTree* tree,
	 	std::string containername,
	 	MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_CscIdHelper(0),
    m_Csc_nrdo(0),
    m_Csc_rdo_stationName(0),
    m_Csc_rdo_stationEta(0),
    m_Csc_rdo_stationPhi(0),
    m_Csc_rdo_channel(0),
    m_Csc_rdo_chamberLayer(0),
    m_Csc_rdo_wireLayer(0),
    m_Csc_rdo_strip(0),
    m_Csc_rdo_measuresPhi(0),
    m_Csc_rdo_time(0),
    m_Csc_rdo_localPosX(0),
    m_Csc_rdo_localPosY(0),
    m_Csc_rdo_globalPosX(0),
    m_Csc_rdo_globalPosY(0),
    m_Csc_rdo_globalPosZ(0)
  {
    setHelper(idhelper);
  }

  ~CSCRDOVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
    if(m_CscIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to CscIdHelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const CscIdHelper* m_CscIdHelper;

  int m_Csc_nrdo;
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
