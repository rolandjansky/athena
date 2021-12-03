/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMPRDVARIABLES_H
#define MMPRDVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MMPRDVariables : public ValAlgVariables
{
 public:
  MMPRDVariables(StoreGateSvc* evtStore,
		 const MuonGM::MuonDetectorManager* detManager,
		 const MuonIdHelper* idhelper,
		 TTree* tree,
		 const std::string & containername,
		 MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_MmIdHelper(0),
    m_NSWMM_nPRDs(0), 
    m_NSWMM_prd_stationName(0),
    m_NSWMM_prd_stationEta(0),
    m_NSWMM_prd_stationPhi(0),
    m_NSWMM_prd_multiplet(0),
    m_NSWMM_prd_gas_gap(0),
    m_NSWMM_prd_channel(0),
    m_NSWMM_prd_time(0),
    m_NSWMM_prd_globalPosX(0),
    m_NSWMM_prd_globalPosY(0),
    m_NSWMM_prd_globalPosZ(0),
    m_NSWMM_prd_localPosX(0),
    m_NSWMM_prd_localPosY(0),
    m_NSWMM_prd_covMatrix_1_1(0),
    m_NSWMM_prd_rdos_charge(0),
    m_NSWMM_prd_rdos_time(0),
    m_NSWMM_prd_rdos_channel(0),
    m_NSWMM_prd_nRdos(0),
    m_NSWMM_prd_uTPCAngle(0),
    m_nsw_prd_uTPCChiSqProb(0)
  {
    setHelper(idhelper);
  }

  ~MMPRDVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_MmIdHelper = dynamic_cast<const MmIdHelper*>(idhelper);
    if(m_MmIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to MmIdhelper failed");
       throw std::runtime_error("Casting exception in MMPRDVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const MmIdHelper* m_MmIdHelper;

  int m_NSWMM_nPRDs;
  std::vector<std::string> *m_NSWMM_prd_stationName;
  std::vector<int> *m_NSWMM_prd_stationEta;
  std::vector<int> *m_NSWMM_prd_stationPhi;
  std::vector<int> *m_NSWMM_prd_multiplet;
  std::vector<int> *m_NSWMM_prd_gas_gap;
  std::vector<int> *m_NSWMM_prd_channel;
  std::vector<int> *m_NSWMM_prd_time;

  std::vector<double> *m_NSWMM_prd_globalPosX;
  std::vector<double> *m_NSWMM_prd_globalPosY;
  std::vector<double> *m_NSWMM_prd_globalPosZ;

  std::vector<double> *m_NSWMM_prd_localPosX;
  std::vector<double> *m_NSWMM_prd_localPosY;
  std::vector<double> *m_NSWMM_prd_covMatrix_1_1;

  std::vector<std::vector<int>> *m_NSWMM_prd_rdos_charge;
  std::vector<std::vector<int>> *m_NSWMM_prd_rdos_time;
  std::vector<std::vector<int>> *m_NSWMM_prd_rdos_channel;
  std::vector<int>              *m_NSWMM_prd_nRdos;

  std::vector<double>* m_NSWMM_prd_uTPCAngle;
  std::vector<double>* m_nsw_prd_uTPCChiSqProb;
  


};

#endif // MMPRDVARIABLE_H
