/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTNonCollBkg_ZDC
// PACKAGE:  DataQualityTools
//
// AUTHORS:  Anadi Canepa (acanepa@cern.ch)
//           Marco Leite  (leite@cern.ch)
//
//
// This code provides histograms about the ZDC (http://zdctwiki)
// energy and timing.
// Energy is obtained by module and the weighted sum is used for the total energy.
// Timing (for now) is calculated using only the first hadronic section.
//
// In the future, this code should use what will become the "official" ZDC reconstruction
// algorithm and calibration, and should refer to the tools in that package. For now, we just
// replicate the code here.
//
// The histograms produced at the moment are:
// m_EnergySideA   -> Total Energy Side A (weigted sum of hadronic sections 0,1,2)
// m_EnergySideC   -> Total Energy Side C (weigted sum of hadronic sections 0,1,2)
// m_TimeSideA     -> Timing Side A calculated by "preferred method" using hadronic section 0 signal
// m_TimeSideC     -> Timing Side C calculated by "preferred method" using hadronic section 0 signal
//
// The location of these histograms in the root file (and in ATLAS DQM web tool) is defined by
// the 'histopath' property declared in the interface and
// currently it points to GLOBAL/DQTNonCollBkg_ZDC.
//
// The 4 methods provided here are the standard DataQuality methods for histogram processing.
//
// What this class *IS NOT DOING* yet:
//  1) Retrieve information from the segmented channels
//  2) Automatically combine the gains and delays in order to optmally
//     extract the information of timing and energy (now we are playing safe with low gain and
//     undelayed channels)
//
//
// Notice: the ZDC configuration was changed in July 2010 due to the LHCf removal
// and instalation of new ZDC electromagnetic modules. Before this date, the ZDC
// consisted of only 3 hadronic modules per each side. Since July 2010, there is an
// electromagnetic module in front of these.
//
//
// ********************************************************************

#ifndef DQTNONCOLLBKG_ZDC_H
#define DQTNONCOLLBKG_ZDC_H

#include <set>
#include <stdint.h>
#include "TH2F.h"
#include "TProfile.h"
#include "LWHists/TH1I_LW.h"

#include "TMath.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <functional>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

/*
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
*/
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "ZdcIdentifier/ZdcID.h"

#include "xAODEventInfo/EventInfo.h"
#include "ZdcEvent/ZdcDigitsCollection.h"

#include "DataQualityTools/DataQualityFatherMonTool.h"

class DQTNonCollBkg_ZDC: public DataQualityFatherMonTool
{

 public:

  DQTNonCollBkg_ZDC(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTNonCollBkg_ZDC();

  StatusCode initialize();
  
  StatusCode bookHistograms(  );
  //StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );

  StatusCode fillHistograms();

  StatusCode procHistograms( );
  //StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );

  StatusCode checkHists(bool fromFinalize);

  bool bookDQTNonCollBkg_ZDC();

  //Methods to extract the ZDC information

  int  getEnergyAndTimeByModule(const Identifier& id,
                   const std::vector< std::vector <int> >& wfm);

private:

  std::vector<float> m_EnergySideA;
  std::vector<float> m_EnergySideC;

  std::vector<float> m_TimeSideA;
  std::vector<float> m_TimeSideC;

  //ZDC Global information histograms

  TH1F_LW * m_HistEnergySideA;
  TH1F_LW * m_HistEnergySideC;
  TH1F_LW * m_HistTimeSideA;
  TH1F_LW * m_HistTimeSideC;

  TH1F_LW * m_HistTimeDeltaSidesAC;

  //ZDC detailed information histograms
  //that can be included in the future
  /*
  TH1F_LW * m_TimeHdA0;
  TH1F_LW * m_TimeHdA1;
  TH1F_LW * m_TimeHdA2;

  TH1F_LW * m_TimeHdA0;
  TH1F_LW * m_TimeHdA1;
  TH1F_LW * m_TimeHdC2;

  TH1F_LW * m_EnergyHdA0;
  TH1F_LW * m_EnergyHdA1;
  TH1F_LW * m_EnergyHdA2;

  TH1F_LW * m_EnergyHdC0;
  TH1F_LW * m_EnergyHdC1;
  TH1F_LW * m_EnergyHdC2;
  */

  //StoreGate variables:
  //m_digitsContainerName : the container that will be used to extract the ZDC information
  //m_complainContain     : controls if a warning message will be issued in case of failure
  //m_complainRetrieve    : controls if a warning message will be issued in case of failure
  SG::ReadHandleKey<ZdcDigitsCollection> m_digitsContainerName
    { "ZdcDigitsCollection" };
  bool m_complainContain;
  bool m_complainRetrieve;

  unsigned int m_nsamples;

  TH1F_LW* m_bcid;

  std::map<unsigned, TH1F_LW*> m_map_BCID_DeltaT;
  
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  std::vector<std::string> m_L1Names;
  std::vector<double> m_L1Trigs;

  // Calibration for Energy. 
  // This will move to COOL 
  std::map<Identifier::value_type, float > m_map_LowGain;
  std::map<Identifier::value_type, float > m_map_HighGain;

  // StoreGate keys
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey
    { "EventInfo" };
  };

#endif
