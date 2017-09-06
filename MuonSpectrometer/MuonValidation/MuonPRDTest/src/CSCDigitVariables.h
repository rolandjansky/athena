/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCDIGITVARIABLES_H
#define CSCDIGITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class CSCDigitVariables : public ValAlgVariables
{
 public:
  CSCDigitVariables(StoreGateSvc* evtStore,
                   const MuonGM::MuonDetectorManager* detManager,
                   const MuonIdHelper* idhelper,
                   TTree* tree,
                   std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "CSCDigitVariables"),
    m_CscIdHelper(0),
    m_CSC_nDigits(0),
    m_CSC_dig_stationName(0),
    m_CSC_dig_stationEta(0),
    m_CSC_dig_stationPhi(0),
    m_CSC_dig_chlayer(0),
    m_CSC_dig_wlayer(0),
    m_CSC_dig_gas_gap(0),
    m_CSC_dig_channel(0),
    m_CSC_dig_truth_barcode(0),
    m_CSC_dig_truth_localPosX(0),
    m_CSC_dig_truth_localPosY(0),
    m_CSC_dig_truth_globalPosX(0),
    m_CSC_dig_truth_globalPosY(0),
    m_CSC_dig_truth_globalPosZ(0),
    m_CSC_dig_truth_charge(0)
  {
    setHelper(idhelper);
  }

  ~CSCDigitVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete CSCDigitVariables.");
      std::terminate();
    }
  }

  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
    if(m_CscIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to CscIdhelper failed");
       throw;
    }
  }

  StatusCode deleteVariables();
  StatusCode clearVariables();

  const CscIdHelper* m_CscIdHelper;

  int m_CSC_nDigits;
  std::vector<std::string> *m_CSC_dig_stationName;
  std::vector<int> *m_CSC_dig_stationEta;
  std::vector<int> *m_CSC_dig_stationPhi;
  std::vector<int> *m_CSC_dig_chlayer;
  std::vector<int> *m_CSC_dig_wlayer;
  std::vector<int> *m_CSC_dig_gas_gap;
  std::vector<int> *m_CSC_dig_channel;

  std::vector< int    > *m_CSC_dig_truth_barcode;
  std::vector< double > *m_CSC_dig_truth_localPosX;
  std::vector< double > *m_CSC_dig_truth_localPosY;
  std::vector< double > *m_CSC_dig_truth_globalPosX;
  std::vector< double > *m_CSC_dig_truth_globalPosY;
  std::vector< double > *m_CSC_dig_truth_globalPosZ;
  std::vector< double > *m_CSC_dig_truth_charge;

};

#endif // CSCDIGITVARIABLES_H
