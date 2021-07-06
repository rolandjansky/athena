/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONITORING_LARDIGITALTRIGGMON_H
#define LARMONITORING_LARDIGITALTRIGGMON_H

//inheritance:
#include "AthenaMonitoring/AthMonitorAlgorithm.h"

//STL:
#include <string>

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

//Events infos:
#include "xAODEventInfo/EventInfo.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawSCContainer.h"

#include <mutex>

class LArEM_ID;
class LArOnlineID;
class HWIdentifier;
class LArOnlineIDStrHelper;
class LArOnOffIdMapping;

class LArDigitalTriggMonAlg: public AthMonitorAlgorithm
{
  
  
public:
  LArDigitalTriggMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Default destructor */
  virtual ~LArDigitalTriggMonAlg();
  
  virtual StatusCode initialize() override;
  
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;
  
private:
  
  /**declaration variables used in joboptions*/
  Gaudi::Property<std::string> m_MonGroupName  {this, "LArDigitTriggMonGroupName", "LArDigitTriggMonGroup"};
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
  /** Switch to online/offline mode*/
  Gaudi::Property<bool>        m_IsOnline      {this, "IsOnline", false}; 
  Gaudi::Property<int>         m_NLatomeBins{this, "NLatomeBins", 117};


  //Added for Stream aware:
  /** Give the name of the streams you want to monitor:*/
  Gaudi::Property<std::vector<std::string> >  m_streams {this, "Streams", {""}};
  
  //Histogram group names
  Gaudi::Property<std::string> m_scMonGroupName {this, "SCMonGroup", "SC"};

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfo","EventInfo","SG Key of EventInfo object"};

  /** Handle to digits */
  SG::ReadHandleKey<LArDigitContainer> m_digitContainerKey{this,"LArDigitContainerKey","SC","SG key of LArDigitContainer read from Bytestream"};
  SG::ReadHandleKey<LArDigitContainer> m_digitContainerKey1{this,"LArDigitContainerKey1","SC_ADC_BAS","SG key of LArDigitContainer read from Bytestream"};
  SG::ReadHandleKey<LArRawSCContainer> m_rawSCContainerKey{this,"LArRawSCContainerKey","SC_ET","SG key of LArRawSCContainer read from Bytestream"};
  SG::ReadHandleKey<LArRawSCContainer> m_rawSCContainerKey1{this,"LArRawSCContainerKey1","SC_ET_ID","SG key of LArRawSCContainer read from Bytestream"};

  /* Id helpers */
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID* m_LArEM_IDHelper;


  int WhatPartition(HWIdentifier id, int side) const; 
  int getXbinFromSourceID(int sourceID) const;
};

#endif
