/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//01/2009: T. Guillemin
//This algorithm predicts the relative times of the physics pulses 
//from the calibration pulses and the complete readout path
#ifndef LARTIMEPHYSPREDICTION_H
#define LARTIMEPHYSPREDICTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"

class CaloDepthTool;
class CaloDetDescrManager;
class ITHistSvc;

class LArTimePhysPrediction : public AthAlgorithm
{

 public:

  LArTimePhysPrediction(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArTimePhysPrediction();

  //standard algorithm methods
  StatusCode initialize(); 
  StatusCode execute()    { return StatusCode::SUCCESS ; } 
  StatusCode stop();
  StatusCode finalize() { return StatusCode::SUCCESS ; }
  
 private:
  
  ITHistSvc * m_thistSvc;
  std::string m_keyoutput;
  std::string m_keyinput;
  std::string m_groupingType;
  std::string m_CaloDepth;
  CaloDepthTool*  m_CaloDepthTool;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArCalibLineMapping> m_calibMapKey{this,"CalibMapKey","LArCalibLineMap","SG Key of calib line mapping object"};

  int m_nchannels_max;
  std::vector<std::vector<double> > m_vLCalib_EMB;
  std::vector<double> m_vEtaMin_EMB;
  std::vector<double> m_vEtaMax_EMB;
  double m_vLCalib_EMEC;
  double m_vLCalib_HEC;
  std::vector<std::vector<double> > m_vLSignal_EMB;
  double m_vLSignal_EMEC;
  double m_vLSignal_HEC;
  std::vector<double> m_vLSignal_FCAL;
  std::vector<std::vector<double> > m_vDeltaTTC_EMB;
  std::vector<std::vector<double> > m_vDeltaTTC_EC;
  std::vector<std::vector<double> > m_vDeltaTTC_ECC_SPEC;
  std::vector<std::vector<double> > m_vDeltaTTC_ECA_SPEC;
  double m_sCalib;
  double m_sSignal;
  double m_sPig;
  double m_sLTP;
  
  //Ntuple pointer
  NTuple::Tuple* m_nt;
  //Ntuple variables
  NTuple::Item<long> m_Chid;
  NTuple::Item<long> m_Channel;
  NTuple::Item<long> m_CalibLine;
  NTuple::Item<long> m_is_lar_em;
  NTuple::Item<long> m_is_lar_hec;
  NTuple::Item<long> m_is_lar_fcal;
  NTuple::Item<long> m_pos_neg;
  NTuple::Item<long> m_barrel_ec;
  NTuple::Item<long> m_FT;
  NTuple::Item<long> m_slot;
  NTuple::Item<long> m_FEBid;
  NTuple::Item<long> m_eta;
  NTuple::Item<long> m_phi;
  NTuple::Item<long> m_layer;
  NTuple::Item<double> m_real_eta;
  NTuple::Item<double> m_real_phi;
  NTuple::Item<double> m_t0;
  NTuple::Item<double> m_tcali; 
  NTuple::Item<double> m_tCalibPredicted;
  NTuple::Item<double> m_CalibCables;
  NTuple::Item<double> m_SignalCables;
  NTuple::Item<double> m_TOF;
  NTuple::Item<double> m_DeltaTTC;
  NTuple::Item<double> m_tPhysPredicted;
};

#endif
