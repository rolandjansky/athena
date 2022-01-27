/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCSDOVARIABLES_H
#define STGCSDOVARIABLES_H

#include <vector>

#include "ValAlgVariables.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

class sTGCSDOVariables : public ValAlgVariables
{
 public:
  sTGCSDOVariables(StoreGateSvc* evtStore,
                       const MuonGM::MuonDetectorManager* detManager,
                       const MuonIdHelper* idhelper,
                       TTree* tree,
							  const std::string & containername,
						     MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_sTgcIdHelper(0),
    m_NSWsTGC_nsdo(0), 
    m_NSWsTGC_sdo_stationName(0),
    m_NSWsTGC_sdo_stationEta(0),
    m_NSWsTGC_sdo_stationPhi(0),
    m_NSWsTGC_sdo_multiplet(0),
    m_NSWsTGC_sdo_gas_gap(0),
    m_NSWsTGC_sdo_channel(0),
    m_NSWsTGC_sdo_channel_type(0),
    m_NSWsTGC_sdo_word(0),
    m_NSWsTGC_sdo_barcode(0),
    m_NSWsTGC_sdo_globalPosX(0),
    m_NSWsTGC_sdo_globalPosY(0),
    m_NSWsTGC_sdo_globalPosZ(0),
    m_NSWsTGC_sdo_globaltime(0),
    m_NSWsTGC_sdo_E(0),
    m_NSWsTGC_sdo_tof(0),
    m_NSWsTGC_sdo_localPosX(0),
    m_NSWsTGC_sdo_localPosY(0)
  {
    setHelper(idhelper);
  }

  ~sTGCSDOVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_sTgcIdHelper = dynamic_cast<const sTgcIdHelper*>(idhelper);
    if(m_sTgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to MmIdhelper failed");
       throw std::runtime_error("Casting error in sTGCSDOVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const sTgcIdHelper* m_sTgcIdHelper{};

  int m_NSWsTGC_nsdo{};
  std::vector<std::string> *m_NSWsTGC_sdo_stationName;
  std::vector<int> *m_NSWsTGC_sdo_stationEta;
  std::vector<int> *m_NSWsTGC_sdo_stationPhi;
  std::vector<int> *m_NSWsTGC_sdo_multiplet;
  std::vector<int> *m_NSWsTGC_sdo_gas_gap;
  std::vector<int> *m_NSWsTGC_sdo_channel;
  std::vector<int> *m_NSWsTGC_sdo_channel_type;

  std::vector<int>    *m_NSWsTGC_sdo_word;
  std::vector<int>    *m_NSWsTGC_sdo_barcode;
  std::vector<double> *m_NSWsTGC_sdo_globalPosX;
  std::vector<double> *m_NSWsTGC_sdo_globalPosY;
  std::vector<double> *m_NSWsTGC_sdo_globalPosZ;
  std::vector<double> *m_NSWsTGC_sdo_globaltime;

  std::vector<double> *m_NSWsTGC_sdo_E;
  std::vector<double> *m_NSWsTGC_sdo_tof;

  std::vector<double> *m_NSWsTGC_sdo_localPosX;
  std::vector<double> *m_NSWsTGC_sdo_localPosY;

};

#endif // STGCSDOVARIABLE_H
