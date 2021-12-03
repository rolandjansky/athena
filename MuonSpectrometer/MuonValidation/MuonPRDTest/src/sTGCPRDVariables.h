/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCPRDVARIABLES_H
#define STGCPRDVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class sTGCPRDVariables : public ValAlgVariables
{
 public:
  sTGCPRDVariables(StoreGateSvc* evtStore,
                       const MuonGM::MuonDetectorManager* detManager,
                       const MuonIdHelper* idhelper,
                       TTree* tree,
						  	  const std::string & containername,
						 	  MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_sTgcIdHelper(0),
    m_NSWsTGC_nPRDs(0),
    m_NSWsTGC_prd_stationName(0),
    m_NSWsTGC_prd_stationEta(0),
    m_NSWsTGC_prd_stationPhi(0),
    m_NSWsTGC_prd_multiplet(0),
    m_NSWsTGC_prd_gas_gap(0),
    m_NSWsTGC_prd_channel_type(0),
    m_NSWsTGC_prd_channel(0),
    m_NSWsTGC_prd_charge(0),
    m_NSWsTGC_prd_bcTag(0),
    m_NSWsTGC_prd_globalPosX(0),
    m_NSWsTGC_prd_globalPosY(0),
    m_NSWsTGC_prd_globalPosZ(0),
    m_NSWsTGC_prd_localPosX(0),
    m_NSWsTGC_prd_localPosY(0),
    m_NSWsTGC_prd_covMatrix_1_1(0),
    m_NSWsTGC_prd_covMatrix_2_2(0)
  {
    setHelper(idhelper);
  }

  ~sTGCPRDVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_sTgcIdHelper = dynamic_cast<const sTgcIdHelper*>(idhelper);
    if(m_sTgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to sTgcIdhelper failed");
       throw std::runtime_error("Casting error in sTGCPRDVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const sTgcIdHelper* m_sTgcIdHelper{};

  int m_NSWsTGC_nPRDs{};
  std::vector<std::string> *m_NSWsTGC_prd_stationName;
  std::vector<int> *m_NSWsTGC_prd_stationEta;
  std::vector<int> *m_NSWsTGC_prd_stationPhi;
  std::vector<int> *m_NSWsTGC_prd_multiplet;
  std::vector<int> *m_NSWsTGC_prd_gas_gap;
  std::vector<int> *m_NSWsTGC_prd_channel_type;
  std::vector<int> *m_NSWsTGC_prd_channel;
  std::vector<int> *m_NSWsTGC_prd_charge;
  std::vector<uint16_t> *m_NSWsTGC_prd_bcTag;

  std::vector<double> *m_NSWsTGC_prd_globalPosX;
  std::vector<double> *m_NSWsTGC_prd_globalPosY;
  std::vector<double> *m_NSWsTGC_prd_globalPosZ;

  std::vector<double> *m_NSWsTGC_prd_localPosX;
  std::vector<double> *m_NSWsTGC_prd_localPosY;
  std::vector<double> *m_NSWsTGC_prd_covMatrix_1_1;
  std::vector<double> *m_NSWsTGC_prd_covMatrix_2_2;

};

#endif // STGCPRDVARIABLE_H
