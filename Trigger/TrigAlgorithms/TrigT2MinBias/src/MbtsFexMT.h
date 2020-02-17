/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2MINBIAS_MBTS_FEX_MT_H
#define TRIGT2MINBIAS_T2_MBTS_FEX_MT_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "TileEvent/TileCellCollection.h"
#include "T2MbtsUtils.h"
#include <vector>
#include "MbtsFexMT.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"
#include "TileIdentifier/TileTBID.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include <cstdlib>
#include <bitset>
#include <string>
#include "GaudiKernel/ToolHandle.h"
class TileTBID;

// namespace xAOD{
//   class TrigT2MbtsBits;
// }

/** @class MbtsFexMT

@author W. H. Bell <W.Bell@cern.ch>

A feature extraction algorithm to retrieve the MBTS TileCells from the DSP
*/
class MbtsFexMT: public AthReentrantAlgorithm {
  public:
     MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~ MbtsFexMT() override;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& context) const override;
    virtual StatusCode finalize() override;

   private:
   /** iterators to TileCells. To be used when on/offline
       TileCellCont/IDC (fast/slow ByteStream Decoder) are to be used.
       Used as input for TrigDataAccess::LoadCollections */
   // TileCellCollection::const_iterator m_itBegin;
   // TileCellCollection::const_iterator m_itEnd;
   // TileCellCollection::const_iterator m_itt;

   //   /** DataHandle to TrigTimerSvc */
   //   ServiceHandle<ITrigTimerSvc> m_timersvc;

   //   /** Timing measures.
   //       m_timer[0] is the complete exec timer. m_timer[1] is the
   //       LoadCollection timer (Data Preparation), m_timer[2] is the
   //       real algorithm timer and m_timer[3] is the saving data
   //       timer. */
   //   TrigTimer* m_timer[4];

   TrigTimer *m_timerLoadColl;
   TrigTimer *m_timerAlg;
   TrigTimer *m_timerSave;
   /** A pointer for access to the Tile Test Beam Identifier helper. */
   const TileTBID* m_tileTBID;
   /** Object  that provides data access in a Region of
       Interest. See TrigDataAccess for more details. */
   // ToolHandle<ITrigDataAccess> m_data;
ToolHandleArray<MbtsFex>  m_data{this, "data", {},"??"};

   /** Energy deposited in each counter (pC) */
   std::vector<float> m_triggerEnergies;
   /** Relative trigger times with repect to collision timing */
   std::vector<float> m_triggerTimes;

   /** ID of MBTS ID  */
   //std::vector<int> m_triggerID;
  // HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution.
   SG::WriteHandleKey<xAOD::TrigT2MbtsBits> m_t2MbtsBits{this,"t2MbtsBits","Undefined",""};//!< internal caching: MBTS feature from the first execution.
   Gaudi::Property<bool> m_useCachedResult{this, "useCachedResult",true, "??"};        //!< internal caching: true when the hltExecute will run in cached mode.
   Gaudi::Property<unsigned int> m_BCID{this, "BCID", 100, "????"};
ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
declareProperty("TrigDataAccess",m_data,"Data Access for LVL2 Calo Algorithms");

// These properties are used for monitoring only.
Gaudi::Property<double> m_threshold{this, "threshold", 40.0/222.0, "????"};
Gaudi::Property<double> m_timeCut{this, "timeCut", 0.0, "????"};
Gaudi::Property<double>  m_globalTimeOffset{this, " globalTimeOffset",0.0, "????"};

Gaudi::Property<double>  m_timeOffsets[0]{this, "TimeOffsetA0",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[1]{this, "TimeOffsetA1",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[2]{this, "TimeOffsetA2",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[3]{this, "TimeOffsetA3",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[4]{this, "TimeOffsetA4",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[5]{this, "TimeOffsetA5",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[6]{this, "TimeOffsetA6",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[7]{this, "TimeOffsetA7",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[8]{this, "TimeOffsetA8",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[9]{this, "TimeOffsetA9",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[10]{this, "TimeOffsetA10",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[11]{this, "TimeOffsetA11",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[12]{this, "TimeOffsetA12",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[13]{this, "TimeOffsetA13",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[14]{this, "TimeOffsetA14",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[15]{this, "TimeOffsetA15",0.0, "????"};

Gaudi::Property<double>  m_timeOffsets[16]{this, "TimeOffsetA16",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[17]{this, "TimeOffsetA17",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[18]{this, "TimeOffsetA18",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[19]{this, "TimeOffsetA19",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[20]{this, "TimeOffsetA20",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[21]{this, "TimeOffsetA21",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[22]{this, "TimeOffsetA22",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[23]{this, "TimeOffsetA23",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[24]{this, "TimeOffsetA24",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[25]{this, "TimeOffsetA25",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[26]{this, "TimeOffsetA26",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[27]{this, "TimeOffsetA27",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[28]{this, "TimeOffsetA28",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[29]{this, "TimeOffsetA29",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[30]{this, "TimeOffsetA30",0.0, "????"};
Gaudi::Property<double>  m_timeOffsets[31]{this, "TimeOffsetA31",0.0, "????"};

// declareMonitoredVariable("MultiplicityEBA",           m_mult.first); // Number of counters within cuts, side A
// declareMonitoredVariable("MultiplicityEBC",           m_mult.second); // Number of counters within cuts, side C
// declareMonitoredVariable("TimeDiff_A_C",              m_timeDiff_A_C);  // Time difference A - C
// declareMonitoredVariable("BCID",                      m_BCID);
// declareMonitoredStdContainer("TriggerEnergies",       m_triggerEnergies); // Energies deposited in each counter
// declareMonitoredStdContainer("TriggerTimes",          m_triggerTimes); // Relative times of each of the triggers.
// declareMonitoredStdContainer("TriggerID",             m_triggerID); // ID of MBTS detectors

auto mon_triggerEnergies = Monitored::Scalar<int>("TriggerEnergies",triggerEnergies);
auto mon_triggerTimes = Monitored::Scalar<int>("TriggerTimes",TriggerTimes);
auto mon_triggerID = Monitored::Scalar<int>("TriggerID",TriggerID);
auto mon_timeDiff_A_C = Monitored::Scalar<int>("TimeDiff_A_C",TimeDiff_A_C);
auto mon_multiplicityEBC = Monitored::Scalar<int>("MultiplicityEBC",MultiplicityEBC);
auto mon_multiplicityEBA = Monitored::Scalar<int>("MultiplicityEBA",MultiplicityEBA);

Monitored::Group(m_monTool,mon_triggerEnergies,mon_triggerTimes,mon_triggerID,mon_timeDiff_A_C,mon_multiplicityEBC,mon_multiplicityEBA);


};

#endif
