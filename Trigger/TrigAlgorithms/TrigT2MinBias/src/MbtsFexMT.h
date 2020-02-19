/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2MINBIAS_MBTS_FEX_MT_H
#define TRIGT2MINBIAS_T2_MBTS_FEX_MT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"
#include "TileIdentifier/TileTBID.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "xAODEventInfo/EventInfo.h"

class MbtsFexMT : public AthReentrantAlgorithm {
public:
  MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MbtsFexMT() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:

Gaudi::Property<bool>  m_useCachedResult{this, "useCachedResult",true, ""};
SG::WriteHandleKey<xAOD::TrigT2MbtsBits> m_t2MbtsBits{this,"t2MbtsBits","Undefined",""};
SG::WriteHandleKey<xAOD::TrigT2MbtsBitsAuxContainer> m_t2MbtsBitsAuxKey{this,"t2MbtsBitsAuxKey","Undefined",""};
SG::ReadHandleKey<TileTBID> m_tileTBID{this,"tileTBID", "DetectorStore+TileTBID"," "};
ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  ToolHandle<ITrigDataAccess> m_dataTool{this,"dataTool","","dataTool"};

  // declareMonitoredVariable("MultiplicityEBA",           m_mult.first); // Number of counters within cuts, side A
  // declareMonitoredVariable("MultiplicityEBC",           m_mult.second); // Number of counters within cuts, side C
  // declareMonitoredVariable("TimeDiff_A_C",              m_timeDiff_A_C);  // Time difference A - C
  // declareMonitoredVariable("BCID",                      m_BCID);
  // declareMonitoredStdContainer("TriggerEnergies",       m_triggerEnergies); // Energies deposited in each counter
  // declareMonitoredStdContainer("TriggerTimes",          m_triggerTimes); // Relative times of each of the triggers.
  // declareMonitoredStdContainer("TriggerID",             m_triggerID); // ID of MBTS detectors


  Gaudi::Property<double>m_Threshold{this, "Threshold",40.0/222.0, " "};
  Gaudi::Property<double>m_TimeCut{this, "TimeCut",-1.0, " "};
  Gaudi::Property<double>m_TimeOffsetC{this, "TimeOffsetC",0.0, " "};
//WILL BE MINIMISED SOON..INEFFICIENT DEFINIITION METHOD
  Gaudi::Property<double>m_TimeOffsetA0{this, "TimeOffsetA0",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA1{this, "TimeOffsetA1",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA2{this, "TimeOffsetA2",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA3{this, "TimeOffsetA3",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA4{this, "TimeOffsetA4",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA5{this, "TimeOffsetA5",0.0, " "};

  Gaudi::Property<double>m_TimeOffsetA6{this, "TimeOffsetA6",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA7{this, "TimeOffsetA7",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA8{this, "TimeOffsetA8",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA9{this, "TimeOffsetA9",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA10{this, "TimeOffsetA10",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA11{this, "TimeOffsetA11",0.0, " "};

  Gaudi::Property<double>m_TimeOffsetA12{this, "TimeOffsetA12",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA13{this, "TimeOffsetA13",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA14{this, "TimeOffsetA14",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetA15{this, "TimeOffsetA15",0.0, " "};

  Gaudi::Property<double>m_TimeOffsetC0{this, "TimeOffsetC0",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC1{this, "TimeOffsetC1",0.0, " "};

  Gaudi::Property<double>m_TimeOffsetC2{this, "TimeOffsetC2",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC3{this, "TimeOffsetC3",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC4{this, "TimeOffsetC4",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC5{this, "TimeOffsetC5",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC6{this, "TimeOffsetC6",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC7{this, "TimeOffsetC7",0.0, " "};

  Gaudi::Property<double>m_TimeOffsetC8{this, "TimeOffsetC8",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC9{this, "TimeOffsetC9",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC10{this, "TimeOffsetC10",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC11{this, "TimeOffsetC11",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC12{this, "TimeOffsetC12",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC13{this, "TimeOffsetC13",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC14{this, "TimeOffsetC14",0.0, " "};
  Gaudi::Property<double>m_TimeOffsetC15{this, "TimeOffsetC15",0.0, " "};


};
#endif
