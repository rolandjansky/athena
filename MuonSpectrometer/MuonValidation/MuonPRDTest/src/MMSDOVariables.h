/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSDOVARIABLES_H
#define MMSDOVARIABLES_H

#include <vector>

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

class MMSDOVariables : public ValAlgVariables
{
 public:
  MMSDOVariables(StoreGateSvc* evtStore,
                       const MuonGM::MuonDetectorManager* detManager,
                       const MuonIdHelper* idhelper,
                       TTree* tree,
							  const  std::string & containername,
							  MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_MmIdHelper(0),
    m_NSWMM_nsdo(0), 
    m_NSWMM_sdo_stationName(0),
    m_NSWMM_sdo_stationEta(0),
    m_NSWMM_sdo_stationPhi(0),
    m_NSWMM_sdo_multiplet(0),
    m_NSWMM_sdo_gas_gap(0),
    m_NSWMM_sdo_channel(0),
    m_NSWMM_sdo_word(0),
    m_NSWMM_sdo_barcode(0),
    m_NSWMM_sdo_globalPosX(0),
    m_NSWMM_sdo_globalPosY(0),
    m_NSWMM_sdo_globalPosZ(0),
    m_NSWMM_sdo_globaltime(0),
    m_NSWMM_sdo_localPosX(0),
    m_NSWMM_sdo_localPosY(0)
  {
    setHelper(idhelper);
  }

  ~MMSDOVariables()
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
       throw std::runtime_error("Bad cast in MMSDOVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const MmIdHelper* m_MmIdHelper{};

  int m_NSWMM_nsdo{};
  std::vector<std::string> *m_NSWMM_sdo_stationName;
  std::vector<int> *m_NSWMM_sdo_stationEta;
  std::vector<int> *m_NSWMM_sdo_stationPhi;
  std::vector<int> *m_NSWMM_sdo_multiplet;
  std::vector<int> *m_NSWMM_sdo_gas_gap;
  std::vector<int> *m_NSWMM_sdo_channel;

  std::vector<int>    *m_NSWMM_sdo_word;
  std::vector<int>    *m_NSWMM_sdo_barcode;
  std::vector<double> *m_NSWMM_sdo_globalPosX;
  std::vector<double> *m_NSWMM_sdo_globalPosY;
  std::vector<double> *m_NSWMM_sdo_globalPosZ;
  std::vector<double> *m_NSWMM_sdo_globaltime;

  std::vector<double> *m_NSWMM_sdo_localPosX;
  std::vector<double> *m_NSWMM_sdo_localPosY;

};

#endif // MMSDOVARIABLE_H
