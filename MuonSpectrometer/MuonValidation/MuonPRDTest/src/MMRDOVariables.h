/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMRDOVARIABLES_H
#define MMRDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MMRDOVariables : public ValAlgVariables
{
 public:
 MMRDOVariables(StoreGateSvc* evtStore,
		const MuonGM::MuonDetectorManager* detManager,
		const MuonIdHelper* idhelper,
		TTree* tree,
	 	std::string containername,
	 	MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_MmIdHelper(0),
    m_NSWMM_nrdo(0),
    m_NSWMM_rdo_stationName(0),
    m_NSWMM_rdo_stationEta(0),
    m_NSWMM_rdo_stationPhi(0),
    m_NSWMM_rdo_multiplet(0),
    m_NSWMM_rdo_gas_gap(0),
    m_NSWMM_rdo_channel(0),
    m_NSWMM_rdo_time(0),
    m_NSWMM_rdo_charge(0),
    m_NSWMM_rdo_localPosX(0),
    m_NSWMM_rdo_localPosY(0),
    m_NSWMM_rdo_globalPosX(0),
    m_NSWMM_rdo_globalPosY(0),
    m_NSWMM_rdo_globalPosZ(0)
  {
    setHelper(idhelper);
  }

  ~MMRDOVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_MmIdHelper = dynamic_cast<const MmIdHelper*>(idhelper);
    if(m_MmIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to MmIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const MmIdHelper* m_MmIdHelper;

  int m_NSWMM_nrdo;
  std::vector<std::string> *m_NSWMM_rdo_stationName;
  std::vector<int> *m_NSWMM_rdo_stationEta;
  std::vector<int> *m_NSWMM_rdo_stationPhi;
  std::vector<int> *m_NSWMM_rdo_multiplet;
  std::vector<int> *m_NSWMM_rdo_gas_gap;
  std::vector<int> *m_NSWMM_rdo_channel;
  std::vector<int> *m_NSWMM_rdo_time;
  std::vector<int> *m_NSWMM_rdo_charge;

  std::vector<double> *m_NSWMM_rdo_localPosX;
  std::vector<double> *m_NSWMM_rdo_localPosY;
  std::vector<double> *m_NSWMM_rdo_globalPosX;
  std::vector<double> *m_NSWMM_rdo_globalPosY;
  std::vector<double> *m_NSWMM_rdo_globalPosZ;
};

#endif // MMRDOVARIABLES_H
