/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCFASTDIGITVARIABLES_H
#define STGCFASTDIGITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class sTGCFastDigitVariables : public ValAlgVariables
{
 public:
  sTGCFastDigitVariables(StoreGateSvc* evtStore,
                       const MuonGM::MuonDetectorManager* detManager,
                       const MuonIdHelper* idhelper,
                       TTree* tree,
                       std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "sTGCFastDigitVariables"),
    m_sTgcIdHelper(0),
    m_NSWsTGC_nFastDigits(0),
    m_NSWsTGC_fdg_stationName(0),
    m_NSWsTGC_fdg_stationEta(0),
    m_NSWsTGC_fdg_stationPhi(0),
    m_NSWsTGC_fdg_multiplet(0),
    m_NSWsTGC_fdg_gas_gap(0),
    m_NSWsTGC_fdg_channel_type(0),
    m_NSWsTGC_fdg_channel(0),
    m_NSWsTGC_fdg_globalPosX(0),
    m_NSWsTGC_fdg_globalPosY(0),
    m_NSWsTGC_fdg_globalPosZ(0),
    m_NSWsTGC_fdg_localPosX(0),
    m_NSWsTGC_fdg_localPosY(0),
    m_NSWsTGC_fdg_covMatrix_1_1(0),
    m_NSWsTGC_fdg_covMatrix_2_2(0),
    m_NSWsTGC_fdg_truth_barcode(0),
    m_NSWsTGC_fdg_truth_hitOnSurfaceX(0),
    m_NSWsTGC_fdg_truth_hitOnSurfaceY(0)
  {
    setHelper(idhelper);
  }

  ~sTGCFastDigitVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete sTGCFastDigitVariables.");
      std::terminate();
    }
  }

  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_sTgcIdHelper = dynamic_cast<const sTgcIdHelper*>(idhelper);
    if(m_sTgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to sTgcIdhelper failed");
       throw;
    }
  }

  StatusCode deleteVariables();
  StatusCode clearVariables();

  const sTgcIdHelper* m_sTgcIdHelper;

  int m_NSWsTGC_nFastDigits;
  std::vector<std::string> *m_NSWsTGC_fdg_stationName;
  std::vector<int> *m_NSWsTGC_fdg_stationEta;
  std::vector<int> *m_NSWsTGC_fdg_stationPhi;
  std::vector<int> *m_NSWsTGC_fdg_multiplet;
  std::vector<int> *m_NSWsTGC_fdg_gas_gap;
  std::vector<int> *m_NSWsTGC_fdg_channel_type;
  std::vector<int> *m_NSWsTGC_fdg_channel;

  std::vector<double> *m_NSWsTGC_fdg_globalPosX;
  std::vector<double> *m_NSWsTGC_fdg_globalPosY;
  std::vector<double> *m_NSWsTGC_fdg_globalPosZ;

  std::vector<double> *m_NSWsTGC_fdg_localPosX;
  std::vector<double> *m_NSWsTGC_fdg_localPosY;
  std::vector<double> *m_NSWsTGC_fdg_covMatrix_1_1;
  std::vector<double> *m_NSWsTGC_fdg_covMatrix_2_2;


  std::vector<int>    *m_NSWsTGC_fdg_truth_barcode;
  std::vector<double> *m_NSWsTGC_fdg_truth_hitOnSurfaceX;
  std::vector<double> *m_NSWsTGC_fdg_truth_hitOnSurfaceY;  


};

#endif // STGCFASTDIGITVARIABLE_H
