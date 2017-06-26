/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMFASTDIGITVARIABLES_H
#define MMFASTDIGITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MMFastDigitVariables : public ValAlgVariables
{
 public:
  MMFastDigitVariables(StoreGateSvc* evtStore,
                       const MuonGM::MuonDetectorManager* detManager,
                       const MuonIdHelper* idhelper,
                       TTree* tree,
                       std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "MMFastDigitVariables"),
    m_MmIdHelper(0),
    m_NSWMM_nFastDigits(0), 
    m_NSWMM_fdg_stationName(0),
    m_NSWMM_fdg_stationEta(0),
    m_NSWMM_fdg_stationPhi(0),
    m_NSWMM_fdg_multiplet(0),
    m_NSWMM_fdg_gas_gap(0),
    m_NSWMM_fdg_channel(0),
    m_NSWMM_fdg_globalPosX(0),
    m_NSWMM_fdg_globalPosY(0),
    m_NSWMM_fdg_globalPosZ(0),
    m_NSWMM_fdg_localPosX(0),
    m_NSWMM_fdg_localPosY(0),
    m_NSWMM_fdg_covMatrix_1_1(0),
    m_NSWMM_fdg_truth_barcode(0),
    m_NSWMM_fdg_truth_hitOnSurfaceX(0),
    m_NSWMM_fdg_truth_hitOnSurfaceY(0)
  {
    setHelper(idhelper);
  }

  ~MMFastDigitVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete MMFastDigitVariables.");
      std::terminate();
    }
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

  StatusCode deleteVariables();
  StatusCode clearVariables();

  const MmIdHelper* m_MmIdHelper;

  int m_NSWMM_nFastDigits;
  std::vector<std::string> *m_NSWMM_fdg_stationName;
  std::vector<int> *m_NSWMM_fdg_stationEta;
  std::vector<int> *m_NSWMM_fdg_stationPhi;
  std::vector<int> *m_NSWMM_fdg_multiplet;
  std::vector<int> *m_NSWMM_fdg_gas_gap;
  std::vector<int> *m_NSWMM_fdg_channel;

  std::vector<double> *m_NSWMM_fdg_globalPosX;
  std::vector<double> *m_NSWMM_fdg_globalPosY;
  std::vector<double> *m_NSWMM_fdg_globalPosZ;

  std::vector<double> *m_NSWMM_fdg_localPosX;
  std::vector<double> *m_NSWMM_fdg_localPosY;
  std::vector<double> *m_NSWMM_fdg_covMatrix_1_1;

  std::vector<int>    *m_NSWMM_fdg_truth_barcode;
  std::vector<double> *m_NSWMM_fdg_truth_hitOnSurfaceX;
  std::vector<double> *m_NSWMM_fdg_truth_hitOnSurfaceY;

};

#endif // MMDIGITVARIABLE_H
