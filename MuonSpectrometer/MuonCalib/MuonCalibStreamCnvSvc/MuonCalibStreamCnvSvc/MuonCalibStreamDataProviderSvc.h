/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMCNVSVCBASE_MUONCALIBSTREAMDATAPROVIDERSVC_H
#define MUONCALIBSTREAMCNVSVCBASE_MUONCALIBSTREAMDATAPROVIDERSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCalibStreamCnvSvc/IMuonCalibStreamDataProviderSvc.h"
#include "MuCalDecode/CalibEvent.h"
#include <vector>
#include <map>

class MuonCalibRunLumiBlockCoolSvc;

class MuonCalibStreamDataProviderSvc :  public AthService, virtual public IMuonCalibStreamDataProviderSvc {

public:
  MuonCalibStreamDataProviderSvc(const std::string& name, ISvcLocator* svcloc);
  virtual ~MuonCalibStreamDataProviderSvc(void);
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  virtual void setNextEvent(const LVL2_MUON_CALIBRATION::CalibEvent* re);
  virtual const LVL2_MUON_CALIBRATION::CalibEvent* getEvent() ;

  int fakeEventN();
  int fakeRunN();
  int fakeLumiBlock();
    
private:
  ServiceHandle<MuonCalibRunLumiBlockCoolSvc> m_lumiBlockCoolSvc;
  bool m_run_number_from_cool;
  bool m_lumi_block_number_from_cool;
  const LVL2_MUON_CALIBRATION::CalibEvent* m_event; 
  int m_evtN;
  int m_runN;
  int m_fake_evtN;
  int m_fake_runN;
  int m_fake_lumiB;
};

#endif
