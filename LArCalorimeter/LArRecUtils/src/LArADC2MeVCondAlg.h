//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/



#ifndef LARRECUTILS_LARADC2MEVCONDALG_H
#define LARRECUTILS_LARADC2MEVCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "LArRecConditions/LArFebConfig.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILArDAC2uA.h"
#include "LArElecCalib/ILAruA2MeV.h"
#include "LArElecCalib/ILArMphysOverMcal.h"
#include "LArElecCalib/ILArHVScaleCorr.h"

class LArOnlineID_Base;


class LArADC2MeVCondAlg: public AthReentrantAlgorithm {
 public:

  LArADC2MeVCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArADC2MeVCondAlg();

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey{this,"LArOnOffIdMappingKey","LArOnOffIdMap","SG key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<ILAruA2MeV>         m_lAruA2MeVKey{this,"LAruA2MeVKey","LAruA2MeV","SG key of uA2MeV object"}; //Always used
  SG::ReadCondHandleKey<ILArDAC2uA>         m_lArDAC2uAKey{this,"LArDAC2uAKey","LArDAC2uA","SG key of DAC2uA object"}; //Always used
  SG::ReadCondHandleKey<ILArRamp>           m_lArRampKey{this,"LArRampKey","LArRamp","SG key of Ramp object"}; //Always used

  
  SG::ReadCondHandleKey<ILArMphysOverMcal>  m_lArMphysOverMcalKey{this,"LArMphysOverMcalKey","LArMphysOverMcal",
      "SG key of MpysOverMcal object (or empty string if no MphysOverMcal)"}; //Not used for supercells
  SG::ReadCondHandleKey<ILArHVScaleCorr>    m_lArHVScaleCorrKey{this,"LArHVScaleCorrKey","LArHVScaleCorr",
      "SG key of HVScaleCorr object (or empty string if no HVScaleCorr)"};   //Not (yet) used for supercells and simulation 
  
  SG::WriteCondHandleKey<LArADC2MeV>  m_ADC2MeVKey{this,"LArADC2MeVKey","LArADC2MeV","SG key of the resulting LArADC2MeV object"};

  SG::ReadCondHandleKey<LArFebConfig> m_febConfigKey{this,"FebConfigKey","LArFebConfig","SG key for FEB config object"};

  Gaudi::Property<bool> m_useFEBGainThresholds{this,"UseFEBGainTresholds",true};
  Gaudi::Property<bool> m_isSuperCell{this,"isSuperCell",false,"switch to true to use the SuperCell Identfier helper"};

  size_t m_nGains = 0UL;
  const LArOnlineID_Base* m_larOnlineID=nullptr;
};



#endif
