/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUCALSTREAMER_H
#define  TRIGL2MUONSA_MUCALSTREAMER_H

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class IRegSelSvc;

class MuCalStreamer : public HLT::FexAlgo
{
 public:


  enum {
    ITIMER_DATA_PREPARATOR=0,
    ITIMER_PATTERN_FINDER,
    ITIMER_STATION_FITTER,
    ITIMER_TRACK_FITTER,
    ITIMER_TRACK_EXTRAPOLATOR,
    ITIMER_TOTAL_PROCESSING
  };
  
 public:
  
  /** Constructor */
  MuCalStreamer(const std::string& name, ISvcLocator* svc);
  ~MuCalStreamer();
  
  /** hltBeginRun() */
  HLT::ErrorCode hltBeginRun();
  /** hltEndRun() */
  HLT::ErrorCode hltEndRun();
  
  /** hltInitialize() */
  HLT::ErrorCode hltInitialize();
  /** hltFinalize() */
  HLT::ErrorCode hltFinalize();
  /** hltExecute(), main code of the algorithm */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, 
			    HLT::TriggerElement* outputTE);
  
 protected:
  
  // Services
  /** A service handle to StoreGate */
  ServiceHandle<StoreGateSvc> m_storeGate;
  
  /** Timers */
  ITrigTimerSvc* m_timerSvc;
  std::vector<TrigTimer*> m_timers;
  
 private:

  // use the timer service
  bool m_use_timer;

  //Monitored variables
  float m_inner_mdt_hits;
  float m_middle_mdt_hits;
  float m_outer_mdt_hits;  
  std::vector<float> m_fit_residuals;
  std::vector<float> m_res_inner;
  std::vector<float> m_res_middle;
  std::vector<float> m_res_outer;
  float m_efficiency;
  float m_sag_inverse;
  float m_sagitta;
  float m_address;
  float m_absolute_pt;
  float m_track_pt;
  std::vector<float> m_track_eta;
  std::vector<float> m_track_phi;
  std::vector<float> m_failed_eta;
  std::vector<float> m_failed_phi;

};

#endif // MUCALSTREAMER_H
