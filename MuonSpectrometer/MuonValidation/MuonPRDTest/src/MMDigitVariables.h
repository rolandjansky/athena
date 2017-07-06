/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDIGITVARIABLES_H
#define MMDIGITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MMDigitVariables : public ValAlgVariables
{
 public:
  MMDigitVariables(StoreGateSvc* evtStore,
                   const MuonGM::MuonDetectorManager* detManager,
                   const MuonIdHelper* idhelper,
                   TTree* tree,
                   std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "MMDigitVariables"),
    m_MmIdHelper(0),
    m_NSWMM_nDigits(0),
    m_NSWMM_dig_stationName(0),
    m_NSWMM_dig_stationEta(0),
    m_NSWMM_dig_stationPhi(0),
    m_NSWMM_dig_multiplet(0),
    m_NSWMM_dig_gas_gap(0),
    m_NSWMM_dig_channel(0),
    m_NSWMM_dig_time(0),
    m_NSWMM_dig_charge(0),
    m_NSWMM_dig_stripPosition(0),
    m_NSWMM_dig_stripLposX(0),
    m_NSWMM_dig_stripLposY(0),
    m_NSWMM_dig_stripGposX(0),
    m_NSWMM_dig_stripGposY(0),
    m_NSWMM_dig_stripGposZ(0),
    m_NSWMM_dig_sr_time(0),
    m_NSWMM_dig_sr_charge(0),
    m_NSWMM_dig_sr_stripPosition(0),
    m_NSWMM_dig_sr_stripLposX(0),
    m_NSWMM_dig_sr_stripLposY(0),
    m_NSWMM_dig_sr_stripGposX(0),
    m_NSWMM_dig_sr_stripGposY(0),
    m_NSWMM_dig_sr_stripGposZ(0),
    m_NSWMM_dig_truth_barcode(0),
    m_NSWMM_dig_truth_localPosX(0),
    m_NSWMM_dig_truth_localPosY(0),
    m_NSWMM_dig_truth_globalPosX(0),
    m_NSWMM_dig_truth_globalPosY(0),
    m_NSWMM_dig_truth_globalPosZ(0),
    m_NSWMM_dig_truth_XZ_angle(0),
    m_NSWMM_dig_time_trigger(0),
    m_NSWMM_dig_charge_trigger(0),
    m_NSWMM_dig_position_trigger(0),
    m_NSWMM_dig_MMFE_VMM_id_trigger(0),
    m_NSWMM_dig_VMM_id_trigger(0)

  {
    setHelper(idhelper);
  }

  ~MMDigitVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete MMDigitVariables.");
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

  int m_NSWMM_nDigits;
  std::vector<std::string> *m_NSWMM_dig_stationName;
  std::vector<int> *m_NSWMM_dig_stationEta;
  std::vector<int> *m_NSWMM_dig_stationPhi;
  std::vector<int> *m_NSWMM_dig_multiplet;
  std::vector<int> *m_NSWMM_dig_gas_gap;
  std::vector<int> *m_NSWMM_dig_channel;

  std::vector< std::vector<float> >  *m_NSWMM_dig_time;
  std::vector< std::vector<float> >  *m_NSWMM_dig_charge;
  std::vector< std::vector<int> >    *m_NSWMM_dig_stripPosition;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripLposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripLposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripGposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripGposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripGposZ;
  std::vector< std::vector<float> >  *m_NSWMM_dig_sr_time;
  std::vector< std::vector<float> >  *m_NSWMM_dig_sr_charge;
  std::vector< std::vector<int> >    *m_NSWMM_dig_sr_stripPosition;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposZ;

  std::vector< int    > *m_NSWMM_dig_truth_barcode;
  std::vector< double > *m_NSWMM_dig_truth_localPosX;
  std::vector< double > *m_NSWMM_dig_truth_localPosY;
  std::vector< double > *m_NSWMM_dig_truth_globalPosX;
  std::vector< double > *m_NSWMM_dig_truth_globalPosY;
  std::vector< double > *m_NSWMM_dig_truth_globalPosZ;
  std::vector< float  > *m_NSWMM_dig_truth_XZ_angle;

  std::vector< std::vector<float> >  *m_NSWMM_dig_time_trigger;
  std::vector< std::vector<float> >  *m_NSWMM_dig_charge_trigger;
  std::vector< std::vector<int> >    *m_NSWMM_dig_position_trigger;
  std::vector< std::vector<int> >    *m_NSWMM_dig_MMFE_VMM_id_trigger;
  std::vector< std::vector<int> >    *m_NSWMM_dig_VMM_id_trigger;

};

#endif // MMDIGITVARIABLES_H
