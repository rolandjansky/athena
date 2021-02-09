/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONITORING_LARDIGITMON_H
#define LARMONITORING_LARDIGITMON_H

//inheritance:
#include "AthenaMonitoring/AthMonitorAlgorithm.h"

//LAr services:
#include "LArElecCalib/ILArPedestal.h"
#include "LArRecConditions/ILArBadChannelMasker.h"

//STL:
#include <string>

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

//Events infos:
#include "xAODEventInfo/EventInfo.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <mutex>

class LArEM_ID;
class LArOnlineID;
class HWIdentifier;
class LArOnOffIdMapping;

class LArDigitMonAlg: public AthMonitorAlgorithm
{
  
  
public:
  LArDigitMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Default destructor */
  virtual ~LArDigitMonAlg();
  
  virtual StatusCode initialize() override;
  
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;
  
private:
  
  /**declaration variables used in joboptions*/
  Gaudi::Property<std::string> m_MonGroupName  {this, "LArDigitsGroupName", "LArDigitsMonGroup"};
  Gaudi::Property<std::vector<std::string> > m_SubDetNames  {this, "LArDigitsSubDetNames", {} };
  Gaudi::Property<std::vector<std::string> > m_partitions {this, "LArDigitsPartitionNames", {} };
  Gaudi::Property<std::vector<int> > m_Nslots {this, "LArDigitsNslots", {} };
  /**Range to check for the max sample. If min and max=0, the range is set dynamically	*/
  Gaudi::Property<int>         m_AskedSampleRangeLow{this, "SampleRangeLow", -1};
  Gaudi::Property<int>         m_AskedSampleRangeUp {this, "SampleRangeUp", -1};
  /**default saturation cuts*/
  Gaudi::Property<int>         m_ADCsatureCut  {this, "ADCSatureCut", 4095};
  /**default cut to select events*/
  Gaudi::Property<int>         m_SigmaCut      {this, "SigmaCut", 5};
  /** Use the SampleMax expected and the SampleNumber from DB*/
  Gaudi::Property<int>         m_ExpectedSampleMax {this, "ExpectedSampleMax", 0};
  Gaudi::Property<int>         m_SampleNumberFromDB{this, "SampleNumberFromDB", 0};
  /** Number of Bad FEBs used to declare an event noisy and exclude it from monitoring*/
  Gaudi::Property<int>         m_NumberBadFebs     {this, "NumberBadFebs", 5};
  /**bool use to mask the bad channels*/
  Gaudi::Property<bool>        m_ignoreKnownBadChannels{this, "IgnoreBadChannels", false};
  /** Switch to online/offline mode*/
  Gaudi::Property<bool>        m_IsOnline      {this, "IsOnline", false}; 
  /** Treshold to declare a bad event*/
  Gaudi::Property<double>      m_TreshOut      {this, "TreshOut", 5.};
  Gaudi::Property<double>      m_TreshNull     {this, "TreshNull", 5.};
  Gaudi::Property<double>      m_TreshSat      {this, "TreshSat", 5.};
  
  //Added for Stream aware:
  /** Give the name of the streams you want to monitor:*/
  Gaudi::Property<std::vector<std::string> >  m_streams {this, "Streams", {""}};
  //std::vector<unsigned> m_streamsThisEvent;
  
  //Histogram group names
  Gaudi::Property<std::string> m_summaryMonGroupName {this, "SummaryMonGroup", "Summary"};

  //** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "CablingKey", "LArOnOffIdMap","Cabling key"};
  
  /** Handle to bad-channel mask */
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;
  
  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};
  
  /** Handle to NoisyRO info */
  SG::ReadHandleKey<LArNoisyROSummary> m_noisyROSummaryKey{this,"NoisyROSumKey","LArNoisyROSummary","SG key of LArNoisyROSummary object"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfo","EventInfo","SG Key of EventInfo object"};

  /** Handle to digits */
  SG::ReadHandleKey<LArDigitContainer> m_digitContainerKey{this,"LArDigitContainerKey","FREE","SG key of LArDigitContainer read from Bytestream"};

  /* Id helpers */
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID* m_LArEM_IDHelper;

  /* Histograms grouping (subdet)(side) */
  std::vector<std::map<std::string,int> > m_histoGroups;

  /** Private members*/
  /* set once, guarded by mutex */
  mutable int m_Samplenbr ATLAS_THREAD_SAFE;
  mutable std::mutex m_lock;
  mutable int m_SampleRangeLow ATLAS_THREAD_SAFE; 
  mutable int m_SampleRangeUp ATLAS_THREAD_SAFE; 
  
  int WhatPartition(HWIdentifier id, int side) const; 
};

#endif
