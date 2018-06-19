//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef LARADC2MEVCONDALG_H
#define LARADC2MEVCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ToolHandle.h"

class LArADC2MeV;
class ILAruA2MeV;
class ILArDAC2uA;
class ILArRamp;
class ILArMphysOverMcal;
class ILArHVScaleCorr;
class LArOnOffIdMapping;

class ILArFEBConfigReader;

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


  //FIXME:
  //The FEB configurations (gain thresholds) are also conditions
  //Its stored in 18 COOL folders, so waiting for ReadCondHandleArray for migrating to a cond-algo
  ToolHandle<ILArFEBConfigReader> m_febCfgReader;

  size_t m_nGains;
  bool m_isSuperCell;

};



#endif
