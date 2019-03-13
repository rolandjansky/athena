//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/



#ifndef LARRECUTILS_LARADC2MEVCONDALG_H
#define LARRECUTILS_LARADC2MEVCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "LArRecConditions/LArFebConfig.h"

class LArADC2MeV;
class ILAruA2MeV;
class ILArDAC2uA;
class ILArRamp;
class ILArMphysOverMcal;
class ILArHVScaleCorr;
class LArOnOffIdMapping;

class LArADC2MeVCondAlg: public AthAlgorithm {
 public:

  LArADC2MeVCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArADC2MeVCondAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey;   
  SG::ReadCondHandleKey<ILAruA2MeV>         m_lAruA2MeVKey; //Always used
  SG::ReadCondHandleKey<ILArDAC2uA>         m_lArDAC2uAKey; //Always used
  SG::ReadCondHandleKey<ILArRamp>           m_lArRampKey;   //Always used

  
  SG::ReadCondHandleKey<ILArMphysOverMcal>  m_lArMphysOverMcalKey; //Not used for supercells
  SG::ReadCondHandleKey<ILArHVScaleCorr>    m_lArHVScaleCorrKey;   //Not (yet) used for supercells and simulation 
  
  SG::WriteCondHandleKey<LArADC2MeV>  m_ADC2MeVKey;

  ServiceHandle<ICondSvc> m_condSvc;


  SG::ReadCondHandleKey<LArFebConfig> m_configKey;

  bool m_useFEBGainThresholds;
  size_t m_nGains;
  bool m_isSuperCell;

};



#endif
