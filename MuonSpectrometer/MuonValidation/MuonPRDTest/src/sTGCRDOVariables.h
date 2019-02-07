/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRDOVARIABLES_H
#define STGCRDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class sTGCRDOVariables : public ValAlgVariables
{
 public:
 sTGCRDOVariables(StoreGateSvc* evtStore,
		const MuonGM::MuonDetectorManager* detManager,
		const MuonIdHelper* idhelper,
		TTree* tree,
		std::string containername,
		MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_sTgcIdHelper(0),
    m_NSWsTGC_nrdo(0),
    m_NSWsTGC_rdo_stationName(0),
    m_NSWsTGC_rdo_stationEta(0),
    m_NSWsTGC_rdo_stationPhi(0),
    m_NSWsTGC_rdo_multiplet(0),
    m_NSWsTGC_rdo_gas_gap(0),
    m_NSWsTGC_rdo_channel(0),
    m_NSWsTGC_rdo_channel_type(0),
    m_NSWsTGC_rdo_time(0),
    m_NSWsTGC_rdo_charge(0),
    m_NSWsTGC_rdo_bcTag(0),
    m_NSWsTGC_rdo_isDead(0),
    m_NSWsTGC_rdo_localPosX(0),
    m_NSWsTGC_rdo_localPosY(0),
    m_NSWsTGC_rdo_globalPosX(0),
    m_NSWsTGC_rdo_globalPosY(0),
    m_NSWsTGC_rdo_globalPosZ(0)
  {
    setHelper(idhelper);
  }

  ~sTGCRDOVariables() override final { deleteVariables(); }

  StatusCode initializeVariables() override final;
  StatusCode fillVariables() override final;

 private:

  void setHelper(const MuonIdHelper* idhelper) override final{
    m_sTgcIdHelper = dynamic_cast<const sTgcIdHelper*>(idhelper);
    if(m_sTgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to sTgcIdHelper failed");
       throw;
    }
  }

  void deleteVariables() override final;
  StatusCode clearVariables() override final;

  const sTgcIdHelper* m_sTgcIdHelper;

  int m_NSWsTGC_nrdo;
  std::vector<std::string> *m_NSWsTGC_rdo_stationName;
  std::vector<int> *m_NSWsTGC_rdo_stationEta;
  std::vector<int> *m_NSWsTGC_rdo_stationPhi;
  std::vector<int> *m_NSWsTGC_rdo_multiplet;
  std::vector<int> *m_NSWsTGC_rdo_gas_gap;
  std::vector<int> *m_NSWsTGC_rdo_channel;
  std::vector<int> *m_NSWsTGC_rdo_channel_type;
  std::vector<double> *m_NSWsTGC_rdo_time;
  std::vector<double> *m_NSWsTGC_rdo_charge;
  std::vector<uint16_t> *m_NSWsTGC_rdo_bcTag;
  std::vector<bool> *m_NSWsTGC_rdo_isDead;

  std::vector<double> *m_NSWsTGC_rdo_localPosX;
  std::vector<double> *m_NSWsTGC_rdo_localPosY;

  std::vector<double> *m_NSWsTGC_rdo_globalPosX;
  std::vector<double> *m_NSWsTGC_rdo_globalPosY;
  std::vector<double> *m_NSWsTGC_rdo_globalPosZ;
};

#endif // STGCRDOVARIABLES_H
