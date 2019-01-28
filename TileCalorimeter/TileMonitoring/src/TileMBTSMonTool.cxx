/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************//
//
// NAME:     TileMBTSMonTool.cxx
// PACKAGE:  TileMonitoring  
//
// AUTHORS:   Base Structure - Luca Fiorini (Luca.Fiorini@cern.ch) 
//            Specific Alg - Brian Martin (Brian.Thomas.Martin@cern.ch)
//
// ********************************************************************//

#include "TileMonitoring/TileMBTSMonTool.h"


#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileDigitsContainer.h"

#include "TrigConfL1Data/TriggerItem.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "StoreGate/ReadHandle.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

#include <sstream>
#include <iomanip>


// Notes which counters are on the inner/outer segment
const int TileMBTSMonTool::m_isInner[32] =
{
	1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0
};
//=====================================================================
TileMBTSMonTool::TileMBTSMonTool(	const std::string & type, const std::string & name,const IInterface* parent) 
	: TileFatherMonTool(type, name, parent)
  //, m_h_sumPed(0)
  //, m_h_sumHFNoise(0)
  //, m_h_sumLFNoise(0)
  , m_h_occupancy(0)
  , m_h_sumEnergy(0)
  , m_h_sumEnergy_wTBP(0)
  , m_h_sumTime(0)
  , m_h_timeA(0)
  , m_h_timeC(0)
  , m_h_timeDiff(0)
  , m_h_timeDiffLumi(0)
  , m_h_coinEnergyHits(0)
  , m_h_bcidEnergyA(0)
  , m_h_bcidEnergyC(0)
  , m_h_ctpPIT(0)
  , m_h_ctpTBP(0)
  , m_h_ctpTAP(0)
  , m_h_ctpTAV(0)
  , m_h_ctpPITwin(0)
  , m_h_ctpTBPwin(0)
  , m_h_ctpTAPwin(0)
  , m_h_ctpTAVwin(0)
  , m_h_sumPIT(0)
  , m_h_sumTBP(0)
  , m_h_sumTAP(0)
  , m_h_sumTAV(0)
  , m_h_bcidPITsum(0)
  , m_h_bcidTBPsum(0)
  , m_h_bcidTAPsum(0)
  , m_h_bcidTAVsum(0)
  , m_h_multiMapPITA(0)
  , m_h_multiMapPITC(0)
  , m_h_multiPerSide(0)
  , m_h_coinPIT(0)
  , m_h_coinTBP(0)
  , m_h_coinTAP(0)
  , m_h_coinTAV(0)
  , m_h_bcidTriggerA(0)
  , m_h_bcidTriggerC(0)
  , m_h_tileRdOutErr(0)
  , m_lvl1ConfigSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name)
  , m_pitID(32, 0)
  , m_ctpID(32, 0)
  , m_counterExist(32, false)
  , m_old_lumiblock(-1)
  , m_nLumiblocks(3000)
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("LVL1ConfigSvc", m_lvl1ConfigSvc, "LVL1 Config Service");
  declareProperty("readTrigger", m_readTrigger = true); // Switch for CTP config
  declareProperty("doOnline", m_isOnline = false); // Switch for online running
  declareProperty("UseTrigger", m_useTrigger = true); // Switch for using trigger information
  declareProperty("FillHistogramsPerMBTS", m_fillHistogramsPerMBTS = true); // Switch for using per MBTS histograms
  declareProperty("NumberOfLumiblocks", m_nLumiblocks = 3000);
  declareProperty("TileDQstatus", m_DQstatusKey = "TileDQstatus");

  m_path = "/Tile/MBTS";
  m_numEvents = 0;

  std::ostringstream ss;
  std::string partName;
  int counter;
  for (int c = 0; c < 32; c++) {

    if (c < 16) {
      counter = c;
      partName = "A";
    } else {
      counter = c - 16;
      partName = "C";
    }

    ss.str("");
    ss << "MBTS" << partName << counter; //MBTSA1, MBTSA2,...
    m_counterLabel.push_back(ss.str());

    ss.str("");
    ss << "MBTS" << partName << std::setfill('0') << std::setw(2) << counter << std::setfill(' ');
    m_counterNames.push_back(ss.str());
  }
}

//=====================================================================
TileMBTSMonTool::~TileMBTSMonTool() {
}

//=====================================================================
StatusCode TileMBTSMonTool:: initialize(){

  ATH_MSG_INFO( "in initialize()" );

  CHECK( TileFatherMonTool::initialize() );

  memset(m_MBTSchannels, -1, sizeof(m_MBTSchannels));
  memset(m_MBTScounters, -1, sizeof(m_MBTScounters));
  for (unsigned int ros = 3; ros < TileCalibUtils::MAX_ROS ; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      HWIdentifier drawerHwid = m_tileHWID->drawer_id(ros, drawer);
      Identifier mbtsId = m_cabling->drawer2MBTS_id(drawerHwid);
      if (mbtsId.is_valid()) {
        int channel = m_tileHWID->channel(m_cabling->s2h_channel_id(mbtsId));
        m_MBTSchannels[ros - 3][drawer] = channel;

        int counter = m_tileTBID->phi(mbtsId) + 8 * m_tileTBID->eta(mbtsId);
        if (m_tileTBID->side(mbtsId) < 0) counter += 16;// EBC side
        m_MBTScounters[ros - 3][drawer] = counter;

        m_counterExist[counter] = true;

        ATH_MSG_DEBUG( "Found MBTS: drawer/channel (counter)=> " << drawer << "/" << channel << "(" << counter << ")" );

      }
    }
  }
	
  CHECK( m_TileDigitsContainerID.initialize() );
  CHECK( m_MBTSCellContainerID.initialize() );
  CHECK( m_DQstatusKey.initialize() );

  return StatusCode::SUCCESS;
}

/// BookHistogram method is called at every event block ,lumi block and run.
/// At the moment we need to book only every run
//=====================================================================
StatusCode TileMBTSMonTool::bookHistograms() {

  ATH_MSG_INFO("in bookHistograms()");

  if (m_readTrigger) {
    // Connect to the LVL1ConfigSvc for the trigger configuration:
    // This needs to be done AFTER initialize
    if (m_lvl1ConfigSvc.retrieve().isFailure()) {
      ATH_MSG_WARNING( "Can't connect to LVL1ConfigSvc using default CTP mapping" );
      m_readTrigger = false;
    }
  }

  if (!m_readTrigger) {
    int ctpOffset = 30;
    int pitOffset = 115;
    for (int c = 0; c < 32; ++c) {
      if (m_counterExist[c]) {
        m_ctpID[c] = (ctpOffset + c);
        m_pitID[c] = (pitOffset + c);
      }
    }
  } else {
    const TrigConf::CTPConfig* ctpc = m_lvl1ConfigSvc->ctpConfig();
    if (ctpc) {
#ifdef TrigConf_Menu
      for (auto it = ctpc->menu().itemVector().begin(); it != ctpc->menu().itemVector().end(); ++it)
#else
      for (std::vector<TrigConf::TriggerItem*>::const_iterator it =
            ctpc->menu()->itemVector().begin(); it != ctpc->menu()->itemVector().end(); ++it)
#endif
      {
        for (unsigned int c = 0; c < m_counterLabel.size(); c++) {
          if ((*it)->name() == ("L1_" + m_counterLabel[c])) {
            if (m_counterExist[c]) m_ctpID[c] = (*it)->ctpId();
            break;
          }
        }
      }
#ifdef TrigConf_Menu
			  for( std::vector< TrigConf::PIT* >::const_iterator it = ctpc->menu().pitVector().begin();
			       it != ctpc->menu().pitVector().end(); ++it )
#else
			  for( std::vector< TrigConf::PIT* >::const_iterator it = ctpc->menu()->pitVector().begin();
			       it != ctpc->menu()->pitVector().end(); ++it ) 
#endif
			  {
          for (unsigned int c = 0; c < m_counterLabel.size(); c++) {
            std::string pitStr = m_counterLabel[c];
            pitStr.insert(4, 1, '_'); // change MBTSA1 to MBTS_A1 for pit name matching
            if ((*it)->thresholdName() == pitStr) {
              //std::cout << "FOUND PIT item: " << (*it)->thresholdName() << " at id: " << (*it)->pitNumber() << std::endl;
              if (m_counterExist[c]) m_pitID[c] = (*it)->pitNumber();
              break;
            }
          }
        }
    } else {
      ATH_MSG_WARNING( "Retrieved CTPConfig with address: " << ctpc );
    }
  }
  // Where the MBTS multiplicty is stored
  m_pitMultBitsA[0] = 0;
  m_pitMultBitsA[1] = 1;
  m_pitMultBitsA[2] = 2;
  m_pitMultBitsC[0] = 3;
  m_pitMultBitsC[1] = 4;
  m_pitMultBitsC[2] = 5;

  // Threshold at CTP divided by pC/mV
  m_energyCut[0] = 60. / 222;
  m_energyCut[1] = 60. / 222;
  m_energyCut[2] = 60. / 222;
  m_energyCut[3] = 60. / 222;
  m_energyCut[4] = 60. / 222;
  m_energyCut[5] = 60. / 222;
  m_energyCut[6] = 60. / 222;
  m_energyCut[7] = 60. / 222;
  m_energyCut[8] = 60. / 222;
  m_energyCut[9] = 60. / 222;
  m_energyCut[10] = 60. / 222;
  m_energyCut[11] = 60. / 222;
  m_energyCut[12] = 60. / 222;
  m_energyCut[13] = 60. / 222;
  m_energyCut[14] = 60. / 222;
  m_energyCut[15] = 60. / 222;
  m_energyCut[16] = 60. / 222;
  m_energyCut[17] = 60. / 222;
  m_energyCut[18] = 60. / 222;
  m_energyCut[19] = 60. / 222;
  m_energyCut[20] = 60. / 222;
  m_energyCut[21] = 60. / 222;
  m_energyCut[22] = 60. / 222;
  m_energyCut[23] = 60. / 222;
  m_energyCut[24] = 60. / 222;
  m_energyCut[25] = 60. / 222;
  m_energyCut[26] = 60. / 222;
  m_energyCut[27] = 60. / 222;
  m_energyCut[28] = 60. / 222;
  m_energyCut[29] = 60. / 222;
  m_energyCut[30] = 60. / 222;
  m_energyCut[31] = 60. / 222;


  ATH_MSG_INFO( "Using path " << m_path );
  // Currently all levels of detail (LOD) are set to SHIFT.
  // This will be changed once multiple LODs can be written to the same ROOT file
  if (m_fillHistogramsPerMBTS) {
    for (int counter = 0; counter < 32; ++counter) {
      if (m_counterExist[counter]) {
        // Trigger
        if (m_useTrigger) {
          m_h_bcidPIT[counter] = book1S("Trigger", "DeltaBcidPIT_" + m_counterNames[counter], "MBTS BCID difference of PIT signal", 20, -9, 10);
          m_h_bcidPIT[counter]->GetXaxis()->SetTitle("Bunch crossings between PIT fire and L1Accept");

          m_h_bcidTBP[counter] = book1S("Trigger", "DeltaBcidTBP_" + m_counterNames[counter], "MBTS BCID difference of TBP signal", 20, -9, 10);
          m_h_bcidTBP[counter]->GetXaxis()->SetTitle("Bunch crossings between TBP fire and L1Accept");

          m_h_bcidTAP[counter] = book1S("Trigger", "DeltaBcidTAP_" + m_counterNames[counter], "MBTS BCID difference of TAP signal", 20, -9, 10);
          m_h_bcidTAP[counter]->GetXaxis()->SetTitle("Bunch crossings between TAP fire and L1Accept");

          m_h_bcidTAV[counter] = book1S("Trigger", "DeltaBcidTAV_" + m_counterNames[counter], "MBTS BCID difference of TAV signal", 20, -9, 10);
          m_h_bcidTAV[counter]->GetXaxis()->SetTitle("Bunch crossings between TAV fire and L1Accept");

          m_h_energy_wTBP[counter] = book1F("Cell", "Energy_wTBP_" + m_counterNames[counter], 
                                            "MBTS Energy with TBP fired of " + m_counterNames[counter], 400, -0.5, 10);
          m_h_energy_wTBP[counter]->GetXaxis()->SetTitle("Energy (with Trigger) [pC]");

          m_h_time_wTBP[counter] = book1F("Cell", "Time_wTBP_" + m_counterNames[counter], 
                                          "MBTS Time with TBP fired of " + m_counterNames[counter], 100, -100, 100);
          m_h_time_wTBP[counter]->GetXaxis()->SetTitle("Pulse Peak Time (with Trigger) [ns]");

          m_h_efficiency[counter] = bookProfile("Cell", "Efficiency_" + m_counterNames[counter], 
                                                "Efficiency of Lvl1 vs readout " + m_counterNames[counter], 400, -0.5, 10);
          m_h_efficiency[counter]->GetXaxis()->SetTitle("Energy in Counter [pC]");
          m_h_efficiency[counter]->GetYaxis()->SetTitle("Fraction of events w/ Trigger");
          
          m_h_pulseAvgTrig[counter] = bookProfile("Digit", "PulseAvgTrig_" + m_counterNames[counter], 
                                                  "Avg Pulse shape (w/ Trigger) for digitized signals of " + m_counterNames[counter], 7, 0., 7.);
          m_h_pulseAvgTrig[counter]->GetXaxis()->SetTitle("TileCal Digital Sample");
          m_h_pulseAvgTrig[counter]->GetYaxis()->SetTitle("Average number of ADC counts");
          
        }
        
        // Cell
        m_h_energy[counter] = book1F("Cell", "Energy_" + m_counterNames[counter], "MBTS Energy of " + m_counterNames[counter], 400, -0.5, 10.);
        m_h_energy[counter]->GetXaxis()->SetTitle("Energy [pC]");
        
        if (m_isOnline) {
          m_h_energy_lb[counter] = bookProfile("Cell", "EnergyLB_" + m_counterNames[counter], 
                                               "MBTS Energy of " + m_counterNames[counter] + " per lumiblock", 100, -99.5, 0.5);
          m_h_energy_lb[counter]->GetXaxis()->SetTitle("Last LumiBlocks");
          
          for (int bin = 1; bin < 100; ++bin) {
            m_h_energy_lb[counter]->SetBinContent(bin, 0);
            m_h_energy_lb[counter]->SetBinEntries(bin, 1);
            
          }
          m_h_energy_lb[counter]->SetEntries(0);
        }
        
        m_h_time[counter] = book1F("Cell", "Time_" + m_counterNames[counter], "MBTS Time of " + m_counterNames[counter], 100, -100, 100);
        m_h_time[counter]->GetXaxis()->SetTitle("Time [ns]");
        
        // Digit
        m_h_pedestal[counter] = book1F("Digit", "Ped_" + m_counterNames[counter], "MBTS Pedestal of " + m_counterNames[counter], 100, 20, 120);
        m_h_pedestal[counter]->GetXaxis()->SetTitle("Channel pedestal in ADC counts");

        m_h_hfNoise[counter] = book1F("Digit", "HFN_" + m_counterNames[counter], "MBTS High Frequency Noise of " + m_counterNames[counter], 100, 0, 5);
        m_h_hfNoise[counter]->GetXaxis()->SetTitle("Inter-sample RMS in ADC counts");
        
        m_h_pulseAvgNoTrig[counter] = bookProfile("Digit", "PulseAvgNoTrig_" + m_counterNames[counter], 
                                                  "Avg Pulse shape (no Trigger) for digitized signals of " + m_counterNames[counter], 7, 0., 7.);
        m_h_pulseAvgNoTrig[counter]->GetXaxis()->SetTitle("TileCal Digital Sample");
        m_h_pulseAvgNoTrig[counter]->GetYaxis()->SetTitle("Average number of ADC counts");
      }
    }
  }
  
  

  // Trigger
  if (m_useTrigger) {
    m_h_ctpPIT = book1S("Trigger", "PITs_Fired", "MBTS Trigger Inputs fired", 32, 0, 32);
    setBinLabels(m_h_ctpPIT, 1);

    m_h_ctpTBP = book1S("Trigger", "TBPs_Fired", "MBTS Triggers before prescale", 32, 0, 32);
    setBinLabels(m_h_ctpTBP, 1);

    m_h_ctpTAP = book1S("Trigger", "TAPs_Fired", "MBTS Triggers after prescale", 32, 0, 32);
    setBinLabels(m_h_ctpTAP, 1);

    m_h_ctpTAV = book1S("Trigger", "TAVs_Fired", "MBTS Triggers after veto", 32, 0, 32);
    setBinLabels(m_h_ctpTAV, 1);

    m_h_ctpPITwin = book1S("Trigger", "PITs_FiredWin", "MBTS Trigger Inputs fired in BC window", 32, 0, 32);
    setBinLabels(m_h_ctpPITwin, 1);

    m_h_ctpTBPwin = book1S("Trigger", "TBPs_FiredWin", "MBTS Triggers before prescale in BC window", 32, 0, 32);
    setBinLabels(m_h_ctpTBPwin, 1);

    m_h_ctpTAPwin = book1S("Trigger", "TAPs_FiredWin", "MBTS Triggers after prescale in BC window", 32, 0, 32);
    setBinLabels(m_h_ctpTAPwin, 1);

    m_h_ctpTAVwin = book1S("Trigger", "TAVs_FiredWin", "MBTS Triggers after veto in BC window", 32, 0, 32);
    setBinLabels(m_h_ctpTAVwin, 1);

    m_h_sumPIT = book1S("Trigger", "PITs_Sum", "Sum of MBTS Trigger Inputs fired", 6, 0, 6);
    setBinLabels(m_h_sumPIT, 2);

    m_h_sumTBP = book1S("Trigger", "TBPs_Sum", "Sum of MBTS Trigger before prescale", 6, 0, 6);
    setBinLabels(m_h_sumTBP, 2);

    m_h_sumTAP = book1S("Trigger", "TAPs_Sum", "Sum of MBTS Trigger after prescale", 6, 0, 6);
    setBinLabels(m_h_sumTAP, 2);

    m_h_sumTAV = book1S("Trigger", "TAVs_Sum", "Sum of MBTS Trigger after veto", 6, 0, 6);
    setBinLabels(m_h_sumTAV, 2);

    m_h_coinPIT = book2S("Trigger", "CoinPIT", "Coincident PITs fired between two MBTS counters", 32, 0, 32, 32, 0, 32);
    setBinLabels(m_h_coinPIT, 1, true);

    m_h_coinTBP = book2S("Trigger", "CoinTBP", "Coincident TBPs fired between two MBTS counters", 32, 0, 32, 32, 0, 32);
    setBinLabels(m_h_coinTBP, 1, true);

    m_h_coinTAP = book2S("Trigger", "CoinTAP", "Coincident TAPs fired between two MBTS counters", 32, 0, 32, 32, 0, 32);
    setBinLabels(m_h_coinTAP, 1, true);

    m_h_coinTAV = book2S("Trigger", "CoinTAV", "Coincident TAVs fired between two MBTS counters", 32, 0, 32, 32, 0, 32);
    setBinLabels(m_h_coinTAV, 1, true);

    m_h_bcidPITsum = bookProfile("Trigger", "DeltaBcidPITsummary", "BCID difference of PIT from Event BCID", 32, 0, 32);
    m_h_bcidPITsum->GetYaxis()->SetTitle("Avg deltaBCID");
    setBinLabels((TH1*) m_h_bcidPITsum, 1);

    m_h_bcidTBPsum = bookProfile("Trigger", "DeltaBcidTBPsummary", "BCID difference of TBP from Event BCID", 32, 0, 32);
    m_h_bcidTBPsum->GetYaxis()->SetTitle("Avg deltaBCID");
    setBinLabels((TH1*) m_h_bcidTBPsum, 1);

    m_h_bcidTAPsum = bookProfile("Trigger", "DeltaBcidTAPsummary", "BCID difference of TAP from Event BCID", 32, 0, 32);
    m_h_bcidTAPsum->GetYaxis()->SetTitle("Avg deltaBCID");
    setBinLabels((TH1*) m_h_bcidTAPsum, 1);

    m_h_bcidTAVsum = bookProfile("Trigger", "DeltaBcidTAVsummary", "BCID difference of TAV from Event BCID", 32, 0, 32);
    m_h_bcidTAVsum->GetYaxis()->SetTitle("Avg deltaBCID");
    setBinLabels((TH1*) m_h_bcidTAVsum, 1);

    m_h_multiMapPITA = book2S("Trigger", "MultiMapPITA", "MBTS A-side multiplicities", 16, 0, 16, 16, 0, 16);
    m_h_multiMapPITA->GetXaxis()->SetTitle("From summing counter bits");
    m_h_multiMapPITA->GetYaxis()->SetTitle("From CTP multi. bits");

    m_h_multiMapPITC = book2S("Trigger", "MultiMapPITC", "MBTS C-side multiplicities", 16, 0, 16, 16, 0, 16);
    m_h_multiMapPITC->GetXaxis()->SetTitle("From summing counter bits");
    m_h_multiMapPITC->GetYaxis()->SetTitle("From CTP multi. bits");

    m_h_multiPerSide = book2S("Trigger", "MultiPerSide", "MBTS multiplicities per side", 16, 0, 16, 16, 0, 16);
    m_h_multiPerSide->GetXaxis()->SetTitle("Number of A side triggers");
    m_h_multiPerSide->GetYaxis()->SetTitle("Number of C side triggers");

    m_h_bcidTriggerA = bookProfile("Trigger", "BCIDTriggerA", "Average number of MBTS triggers on A side as a function of BCID", 3565, 0, 3565);
    m_h_bcidTriggerA->GetXaxis()->SetTitle("BCID");
    m_h_bcidTriggerA->GetYaxis()->SetTitle("Average number of triggers on A side");

    m_h_bcidTriggerC = bookProfile("Trigger", "BCIDTriggerC", "Average number of MBTS triggers on C side as a function of BCID", 3565, 0, 3565);
    m_h_bcidTriggerC->GetXaxis()->SetTitle("BCID");
    m_h_bcidTriggerC->GetYaxis()->SetTitle("Average number of triggers on C side");

    m_h_sumEnergy_wTBP = bookProfile("Cell", "SummaryEnergy_wTBP", "Average MBTS Energy with Trigger", 32, 0, 32);

    m_h_bcidEnergyA = bookProfile("Cell", "BCIDEnergyA", "Average MBTS Energy of entire A side as a function of BCID", 3565, 0, 3565);
    m_h_bcidEnergyA->GetXaxis()->SetTitle("BCID");
    m_h_bcidEnergyA->GetYaxis()->SetTitle("Average Energy (sum of 16 counters) [pC]");

    m_h_bcidEnergyC = bookProfile("Cell", "BCIDEnergyC", "Average MBTS Energy of entire C side as a function of BCID", 3565, 0, 3565);
    m_h_bcidEnergyC->GetXaxis()->SetTitle("BCID");
    m_h_bcidEnergyC->GetYaxis()->SetTitle("Average Energy (sum of 16 counters) [pC]");

  }

  // Cell
  m_h_occupancy = book1F("Cell", "Occupancy", "MBTS Occupancy", 32, 0, 32);
  setBinLabels((TH1*) m_h_occupancy, 1);

  m_h_sumEnergy = bookProfile("Cell", "SummaryEnergy", "Average MBTS Energy", 32, 0, 32);
  m_h_sumEnergy->GetYaxis()->SetTitle("Average Energy [pC]");
  setBinLabels((TH1*) m_h_sumEnergy, 1);

  m_h_sumTime = bookProfile("Cell", "SummaryTime", "Average MBTS Time (Energy above threshold)", 32, 0, 32);
  m_h_sumTime->GetYaxis()->SetTitle("Average Time [ns]");
  setBinLabels((TH1*) m_h_sumTime, 1);

  m_h_timeA = book1F("Cell", "Time_A", "Time MBTS on A side", 151, -75.5, 75.5);
  m_h_timeA->GetXaxis()->SetTitle("Average Time [ns]");

  m_h_timeC = book1F("Cell", "Time_C", "Time MBTS on C side", 151, -75.5, 75.5);
  m_h_timeC->GetXaxis()->SetTitle("Average Time [ns]");

  m_h_timeDiff = book1F("Cell", "TimeDiff_A-C", "Time Difference between MBTS on A and C side", 151, -75, 75);
  m_h_timeDiff->GetXaxis()->SetTitle("Time difference, A-side - C-side [ns]");

  m_h_timeDiffLumi = book2F("Cell", "TimeDiff_A-C_LB", "Time Difference between MBTS on A and C side vs LumiBlock", 
                            m_nLumiblocks, -0.5, m_nLumiblocks - 0.5, 151, -75, 75);
  m_h_timeDiffLumi->GetXaxis()->SetTitle("Luminosity Block");
  m_h_timeDiffLumi->GetYaxis()->SetTitle("Time difference, A-side - C-side [ns]");

  m_h_coinEnergyHits = book2S("Cell", "CoinEnergyHits", "Coincident Hits(energy) between two MBTS counters", 32, 0, 32, 32, 0, 32);
  setBinLabels(m_h_coinEnergyHits, 1, true);

  //m_h_corrEnergy     = book2F("Cell","EnergyCorr","Energy Correlation between two MBTS counters",32,0,32,32,0,32);
  //setBinLabels(m_h_corrEnergy,1,true);

  //m_h_corrEnergyTot  = book2F("Cell","EnergyCorrTot","Energy Correlation between two MBTS Areas",6,0,6,6,0,6);
  //setBinLabels(m_h_corrEnergyTot,2,true);


  // Digit
  // m_h_sumPed = book2F("Digit", "SummaryPed", "Avg Pedestal for MBTS channels", 32, 0, 32, 100, 20, 120);
  // m_h_sumPed->GetYaxis()->SetTitle("Channel pedestal in ADC counts");
  // m_h_sumPed->SetMarkerStyle(5);
  // setBinLabels(m_h_sumPed, 1, false);


  // m_h_sumHFNoise = book2F("Digit", "SummaryHFNoise", "Avg High Freq Noise Level for MBTS channels", 32, 0, 32, 100, 0, 5);
  // m_h_sumHFNoise->GetYaxis()->SetTitle("Inter-sample RMS in ADC counts");
  // m_h_sumHFNoise->SetMarkerStyle(5);
  // setBinLabels(m_h_sumHFNoise, 1, false);

  // m_h_sumLFNoise = book2F("Digit", "SummaryLFNoise", "Avg Low Freq Noise Level for MBTS channels", 32, 0, 32, 100, 0, 5);
  // m_h_sumLFNoise->GetYaxis()->SetTitle("Inter-event RMS in ADC counts");
  // m_h_sumLFNoise->SetMarkerStyle(5);
  // setBinLabels(m_h_sumLFNoise, 1, false);

  // Error
  m_h_tileRdOutErr = book2S("Digit", "ReadOutErrors", "Tile Readout Errors for MBTS counters", 32, 0, 32, 10, 0, 10);
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(1, "General");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(2, "GlobCRC");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(3, "RodCRC");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(4, "FeCRC");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(5, "BCID");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(6, "HdrFormat");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(7, "HdrParity");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(8, "SmpFormat");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(9, "SmpParity");
  m_h_tileRdOutErr->GetYaxis()->SetBinLabel(10, "MemParity");
  setBinLabels(m_h_tileRdOutErr, 1, false);

  return StatusCode::SUCCESS;
}

//=====================================================================
StatusCode TileMBTSMonTool::fillHistograms() {

  ATH_MSG_DEBUG( "in fillHistograms()" );
  const EventContext& ctx = Gaudi::Hive::currentContext();

  memset(m_hasEnergyHit, false, sizeof(m_hasEnergyHit));
  memset(m_hasPIT, false, sizeof(m_hasPIT));
  memset(m_hasTBP, false, sizeof(m_hasTBP));
  memset(m_hasTAP, false, sizeof(m_hasTAP));
  memset(m_hasTAV, false, sizeof(m_hasTAV));
  memset(m_energy_ij, 0, sizeof(m_energy_ij));
  memset(m_energy_sum, 0, sizeof(m_energy_sum));
  memset(m_energy_sq, 0, sizeof(m_energy_sq));
  memset(m_energy_ij_tot, 0, sizeof(m_energy_ij_tot));
  memset(m_energy_sum_tot, 0, sizeof(m_energy_sum_tot));
  memset(m_energy_sq_tot, 0, sizeof(m_energy_sq_tot));

  fillEvtInfo();
  uint32_t LB = getLumiBlock();
  uint32_t lvl1info = getL1info();
  get_eventTrigs(lvl1info);

  if (m_eventTrigs.end() != find(m_eventTrigs.begin(), m_eventTrigs.end(), (int) Trig_b7)) {
    //std::cout << "Calibration Event found.....skip filling histograms." << std::endl;
    return StatusCode::SUCCESS;
  }

	
	
  //=======================================================================
  // CTP Information
  //=======================================================================
  
  unsigned int l1aBCID(0);

  if (m_useTrigger) {

    uint32_t numberBC(0);
    CTP_Decoder ctp;
    CTP_RDO* theCTP_RDO = evtStore()->tryRetrieve<CTP_RDO>();

    if (theCTP_RDO) {
      ctp.setRDO(theCTP_RDO);
      numberBC = theCTP_RDO->getNumberOfBunches();
    } else {
      ATH_MSG_WARNING("No CTP_RDO is available! No trigger specific information will be used!");
      m_useTrigger = false;
    }
 
    if (numberBC > 0) {
      short l1aBunch = theCTP_RDO->getL1AcceptBunchPosition();
      const std::vector<CTP_BC> &BCs = ctp.getBunchCrossings();
      const CTP_BC & bunch = BCs[l1aBunch];
      l1aBCID = bunch.getBCID();
      //std::cout << "Number of Bunches in CTP window: " << numberBC << std::endl;
      //std::cout << "Level 1 Accept Bunch: " << l1aBunch << std::endl;
      //std::cout << "Level 1 Accept BCID: "  << l1aBCID << std::endl;
      unsigned int bcid;
      int deltaBCID;
      // Loop over bunch crossings in CTP window
      for (std::vector<CTP_BC>::const_iterator it = BCs.begin(); it != BCs.end(); ++it) {

	bcid = it->getBCID();
	deltaBCID = l1aBCID - bcid;
	int PITmultiA = 0;
	int PITmultiC = 0; // for counting multiplicity from counter bits
	int TBPmultiA = 0;
	int TBPmultiC = 0;
	const std::bitset<512> currentPIT(it->getTIP());
	for (size_t p = 0; p < m_pitID.size(); ++p) {
	  if (m_pitID[p] == 0) continue;
	  if (currentPIT.test(m_pitID[p])) {
	    m_h_bcidPIT[p]->Fill(deltaBCID, 1);
	    m_h_bcidPITsum->Fill(p, deltaBCID);
	    m_h_ctpPITwin->Fill(p, 1);
	    if (bcid == l1aBCID) {
	      m_h_ctpPIT->Fill(p, 1);
	      m_hasPIT[p] = true;
	      // Fill Sum histogram
	      if (p < 16)   {//Side A
		
		m_h_sumPIT->Fill(2, 1);
		PITmultiA++;
		if (m_isInner[p] == 1) m_h_sumPIT->Fill(0., 1);
		else m_h_sumPIT->Fill(1, 1);
	      } else {//Side C
		
		m_h_sumPIT->Fill(5, 1);
		PITmultiC++;
		if (m_isInner[p] == 1) m_h_sumPIT->Fill(3, 1);
		else m_h_sumPIT->Fill(4, 1);
	      }
	    }
	  }
	  
	  if (bcid == l1aBCID) {
	    // Check MBTSmultiplicity bits (only in PIT right now)
	    int PITmultiA_ctp = 0;
	    int PITmultiC_ctp = 0;
	    if (currentPIT.test(m_pitMultBitsA[0])) PITmultiA_ctp += 1;
	    if (currentPIT.test(m_pitMultBitsA[1])) PITmultiA_ctp += 2;
	    if (currentPIT.test(m_pitMultBitsA[2])) PITmultiA_ctp += 4;
	    if (currentPIT.test(m_pitMultBitsC[0])) PITmultiC_ctp += 1;
	    if (currentPIT.test(m_pitMultBitsC[1])) PITmultiC_ctp += 2;
	    if (currentPIT.test(m_pitMultBitsC[2])) PITmultiC_ctp += 4;
	    m_h_multiMapPITA->Fill(PITmultiA, PITmultiA_ctp);
	    m_h_multiMapPITC->Fill(PITmultiA, PITmultiC_ctp);
	  }
	}
	
	const std::bitset<512> currentTBP(it->getTBP());
	for (size_t p = 0; p < m_ctpID.size(); ++p) {
	  if (m_ctpID[p] == 0) continue;
	  if (currentTBP.test(m_ctpID[p])) {
	    m_h_bcidTBP[p]->Fill(deltaBCID, 1);
	    m_h_bcidTBPsum->Fill(p, deltaBCID);
	    m_h_ctpTBPwin->Fill(p, 1);
	    if (bcid == l1aBCID) {
	      m_h_ctpTBP->Fill(p, 1);
	      m_hasTBP[p] = true;
	      // Fill Sum histogram
	      if (p < 16) {//Side A
		
		TBPmultiA++;
		m_h_sumTBP->Fill(2, 1);
		if (m_isInner[p] == 1) m_h_sumTBP->Fill(0., 1);
		else m_h_sumTBP->Fill(1, 1);
	      } else {//Side C
		
		TBPmultiC++;
		m_h_sumTBP->Fill(5, 1);
		if (m_isInner[p] == 1) m_h_sumTBP->Fill(3, 1);
		else m_h_sumTBP->Fill(4, 1);
	      }
	    }
	  }
	}
	
	if (bcid == l1aBCID) {
	  m_h_bcidTriggerA->Fill(l1aBCID, TBPmultiA);
	  m_h_bcidTriggerC->Fill(l1aBCID, TBPmultiC);
	  m_h_multiPerSide->Fill(TBPmultiA, TBPmultiC);
	}
	
	const std::bitset<512> currentTAP(it->getTAP());
	for (size_t p = 0; p < m_ctpID.size(); ++p) {
	  if (m_ctpID[p] == 0) continue;
	  if (currentTAP.test(m_ctpID[p])) {
	    m_h_bcidTAP[p]->Fill(deltaBCID, 1);
	    m_h_bcidTAPsum->Fill(p, deltaBCID);
	    m_h_ctpTAPwin->Fill(p, 1);
	    if (bcid == l1aBCID) {
	      m_h_ctpTAP->Fill(p, 1);
	      m_hasTAP[p] = true;
	      // Fill Sum histogram
	      if (p < 16) {//Side A
		
		m_h_sumTAP->Fill(2, 1);
		if (m_isInner[p] == 1) m_h_sumTAP->Fill(0., 1);
		else m_h_sumTAP->Fill(1, 1);
	      } else {//Side C
		
		m_h_sumTAP->Fill(5, 1);
		if (m_isInner[p] == 1) m_h_sumTAP->Fill(3, 1);
		else m_h_sumTAP->Fill(4, 1);
	      }
	    }
	  }
	}
	
	const std::bitset<512> currentTAV(it->getTAV());
	for (size_t p = 0; p < m_ctpID.size(); ++p) {
	  if (m_ctpID[p] == 0) continue;
	  if (currentTAV.test(m_ctpID[p])) {
	    m_h_bcidTAV[p]->Fill(deltaBCID, 1);
	    m_h_bcidTAVsum->Fill(p, deltaBCID);
	    m_h_ctpTAVwin->Fill(p, 1);
	    if (bcid == l1aBCID) {
	      m_h_ctpTAV->Fill(p, 1);
	      m_hasTAV[p] = true;
	      // Fill Sum histogram
	      if (p < 16) {//Side A
		
		m_h_sumTAV->Fill(2, 1);
		if (m_isInner[p] == 1) m_h_sumTAV->Fill(0., 1);
		else m_h_sumTAV->Fill(1, 1);
	      } else {//Side C
		
		m_h_sumTAV->Fill(5, 1);
		if (m_isInner[p] == 1) m_h_sumTAV->Fill(3, 1);
		else m_h_sumTAV->Fill(4, 1);
	      }
	    }
	  }
	}
      } // end BC loop
    }
  }

  //==============================================================================
  // CELL LEVEL INFORMATION
  //==============================================================================
  //Retrieve MBTS container collection from SG
  SG::ReadHandle<TileCellContainer> theMBTScontainer
    (m_MBTSCellContainerID, ctx);
  ATH_MSG_VERBOSE( "Retrieval of MBTS container " << m_MBTSCellContainerID << " succeeded" );

  double energy[32], time[32];
  int tHitsA = 0, tHitsC = 0;
  double timeA = 0., timeC = 0.;
  memset(energy, 0, sizeof(energy));
  memset(time, 0, sizeof(time));
  double bcidEnergyA(0.), bcidEnergyC(0.);

  TileCellContainer::const_iterator iCell = theMBTScontainer->begin();
  TileCellContainer::const_iterator lastCell = theMBTScontainer->end();

  for (; iCell != lastCell; ++iCell) {
    Identifier id;
    const TileCell* mbtsCell = *iCell;   // pointer to cell object
    if (mbtsCell != 0) {
      id = mbtsCell->ID();

      // Calculate MBTS counter from "side", "tower" and "module"
      // Counter goes 0-31.
      // EBA-inner:0-7,EBA-outer:8-15,EBC-inner:16-23,EBC-outer:24-31
      // tower is 1 for outer counter (lower eta) and 0 for inner counter (higher eta)
      // module counts from 0-7 in increasing phi
      // side is -1 for EBC 1 for EBA
      //int counter = (m_tileID->module(id)*2+1) - m_tileID->tower(id);

      int counter = m_tileTBID->phi(id) + 8 * m_tileTBID->eta(id);
      if (m_tileTBID->side(id) < 0) counter += 16;// EBC side

      energy[counter] = mbtsCell->energy();
      time[counter] = mbtsCell->time();
      double quality = mbtsCell->quality();

      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "Counter: " << counter << endmsg;
        msg(MSG::VERBOSE) << "Energy= " << energy[counter] << " pCb" << endmsg;
        msg(MSG::VERBOSE) << "Time= " << time[counter] << endmsg;
        msg(MSG::VERBOSE) << "Quality= " << quality << endmsg;
      }

      m_h_occupancy->Fill(counter, energy[counter]);
      m_h_sumEnergy->Fill(counter, energy[counter]);
      if (m_hasTBP[counter]) m_h_sumEnergy_wTBP->Fill(counter, energy[counter]);

      if (m_fillHistogramsPerMBTS) {

        m_h_energy[counter]->Fill(energy[counter], 1.0);
        
        if (m_isOnline) {
          
          int32_t current_lumiblock = getLumiBlock();
          if(m_old_lumiblock == -1) {
            m_old_lumiblock = current_lumiblock;
          }
          
          if(m_old_lumiblock < current_lumiblock) {// move bins
            int32_t delta_lumiblock = current_lumiblock - m_old_lumiblock;
            for (int counter = 0; counter < 32; ++counter) {
              if (m_counterExist[counter]) {
                
                ShiftTprofile(m_h_energy_lb[counter], delta_lumiblock);
                
                int last_bin = m_h_energy_lb[counter]->GetNbinsX();
                for(int bin = last_bin - delta_lumiblock + 1; bin < last_bin; ++bin) {
                  m_h_energy_lb[counter]->SetBinContent(bin, 0);
                  m_h_energy_lb[counter]->SetBinEntries(bin, 1);
                }
              }
            }
            m_old_lumiblock = current_lumiblock;
          }
          
          m_h_energy_lb[counter]->Fill(0.0, energy[counter]);
        }

        if (m_hasTBP[counter]) {
          m_h_energy_wTBP[counter]->Fill(energy[counter], 1.0);
          if (TMath::Abs(time[counter]) > 10e-4) m_h_time_wTBP[counter]->Fill(time[counter], 1.0);
        }
      }


      if (counter < 16) bcidEnergyA += energy[counter];
      else bcidEnergyC += energy[counter];

      if (energy[counter] > m_energyCut[counter]) {
        m_hasEnergyHit[counter] = true;
        m_h_sumTime->Fill(counter, time[counter]);

        if (m_fillHistogramsPerMBTS) {
          m_h_time[counter]->Fill(time[counter], 1.0);
          
          if (m_useTrigger) {
            if (m_hasTBP[counter]) m_h_efficiency[counter]->Fill(energy[counter], 1.);
            else m_h_efficiency[counter]->Fill(energy[counter], 0.);
          }
        }

        if (counter < 16) {
          if (TMath::Abs(time[counter]) > 10e-4) {
            timeA += time[counter];
            tHitsA++;
          }
        } else {
          if (TMath::Abs(time[counter]) > 10e-4) {
            timeC += time[counter];
            tHitsC++;
          }
        }
      } else
        m_hasEnergyHit[counter] = false;

    } // end if cell not empty
  } // end Cell loop

  if (tHitsA > 0 && tHitsC > 0) {
    double tdiff = timeA / tHitsA - timeC / tHitsC;
    m_h_timeDiff->Fill(tdiff, 1.0);
    m_h_timeDiffLumi->Fill(LB, tdiff);
  }

  if (tHitsA > 0) m_h_timeA->Fill(timeA / tHitsA);
  if (tHitsC > 0) m_h_timeC->Fill(timeC / tHitsC);

  if (m_useTrigger) {
    m_h_bcidEnergyA->Fill(l1aBCID, bcidEnergyA);
    m_h_bcidEnergyC->Fill(l1aBCID, bcidEnergyC);
  }

  //=======================================================================
  // Digits Level Information
  //=======================================================================

  //Retrieve TileDigits container collection from SG
  SG::ReadHandle<TileDigitsContainer> theDigitsContainer
    (m_TileDigitsContainerID, ctx);

  // Create instance of TileDQstatus used to check for readout errors in Tile
  const TileDQstatus * theDQstatus = SG::makeHandle (m_DQstatusKey).get();

  TileDigitsContainer::const_iterator collItr = theDigitsContainer->begin();
  TileDigitsContainer::const_iterator lastColl = theDigitsContainer->end();

  for (; collItr != lastColl; collItr++) {  // Loop over TileModules

    TileDigitsCollection::const_iterator digitsItr = (*collItr)->begin();
    TileDigitsCollection::const_iterator lastDigits = (*collItr)->end();

    if (digitsItr != lastDigits) {
      HWIdentifier adc_id = (*digitsItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      if (ros > 2) { // extended barrel
        int drawer = m_tileHWID->drawer(adc_id);
        int MBTSchannel = m_MBTSchannels[ros - 3][drawer];

        if (MBTSchannel >= 0) {
          for (; digitsItr != lastDigits; digitsItr++) {  // Loop over TileChannels
            HWIdentifier adc_id = (*digitsItr)->adc_HWID();
            int channel = m_tileHWID->channel(adc_id);
            if (channel == MBTSchannel) {  // MBTS channel found
              int gain = m_tileHWID->adc(adc_id);
              int counter = m_MBTScounters[ros - 3][drawer];

              // Fill Readout Error histogram
              int dmu = 0;
              if (!(theDQstatus->isChanDQgood(ros, drawer, channel)))
                m_h_tileRdOutErr->Fill(counter, 0., 1); // 0 = some error (any of the following)
              if (theDQstatus->checkGlobalCRCErr(ros, drawer, gain))
                m_h_tileRdOutErr->Fill(counter, 1., 1); // 1 = global CRC
              if (theDQstatus->checkROD_CRCErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 2., 1); // 2 = ROD CRC
              if (theDQstatus->checkFE_CRCErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 3., 1); // 3 = FrontEnd CRC
              if (theDQstatus->checkBCIDErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 4., 1); // 4 = BCID
              if (theDQstatus->checkHeaderFormatErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 5., 1); // 5 = Header format
              if (theDQstatus->checkHeaderParityErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 6., 1); // 6 = Header parity
              if (theDQstatus->checkSampleFormatErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 7., 1); // 7 = Sample format
              if (theDQstatus->checkSampleParityErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 8., 1); // 8 = Sample parity
              if (theDQstatus->checkMemoryParityErr(ros, drawer, dmu, gain))
                m_h_tileRdOutErr->Fill(counter, 9., 1); // 9 = Memory parity

              if (m_fillHistogramsPerMBTS) {

                double meansamp = 0;
                double rmssamp = 0;
                
                std::vector<float> vdigits = (*digitsItr)->samples();
                unsigned int dsize = vdigits.size();
                if (dsize > 0) {
                  double ped;
                  int maxSamp = 0;
                  int minSamp = 1023;
                  ped = vdigits[0];
                  for (unsigned int i = 0; i < dsize; ++i) {
                    double dig = vdigits[i];
                    meansamp += dig;
                    rmssamp += dig * dig;
                    if (static_cast<int>(dig) > maxSamp) maxSamp = static_cast<int>(dig);
                    if (static_cast<int>(dig) < minSamp) minSamp = static_cast<int>(dig);
                  }
                  
                  if ((maxSamp - minSamp) > 10) { // if pulse, plot profile
                    
                    if (m_hasTBP[counter]) {
                      for (unsigned int i = 0; i < dsize; ++i)
                        m_h_pulseAvgTrig[counter]->Fill(i, vdigits[i]);
                    } else {
                      for (unsigned int i = 0; i < dsize; ++i)
                        m_h_pulseAvgNoTrig[counter]->Fill(i, vdigits[i]);
                    }
                  }
                  
                  meansamp /= dsize;
                  rmssamp = rmssamp / dsize - meansamp * meansamp;
                  rmssamp = (rmssamp > 0.0 && dsize > 1u) ? sqrt(rmssamp * dsize / (dsize - 1u)) : 0.0;
                  
                  m_h_pedestal[counter]->Fill(ped, 1.0);
                  m_h_hfNoise[counter]->Fill(rmssamp, 1.0);
                } // dsize > 0

              }

              break;  // found MBTS in module
            }
          } // ch loop
        }
      } // if MBTSmod
    }// if digitsContainer not empty
  } // drawer loop

	//=======================================================================
	//   Fill Correlation Quantities
	//=======================================================================
  double energyArea[6];
  memset(energyArea, 0, sizeof(energyArea));
  for (int counter_i = 0; counter_i < 32; counter_i++) {

    //// Fill quantites E,E^2 for correlation calc
//    m_energy_sum[counter_i] += energy[counter_i];
//    m_energy_sq[counter_i] += energy[counter_i] * energy[counter_i];
//
//    if (counter_i < 8) { // A-inner
//      energyArea[0] += energy[counter_i];
//      energyArea[2] += energy[counter_i];
//    } else if (counter_i < 16) { // A-outer
//      energyArea[1] += energy[counter_i];
//      energyArea[2] += energy[counter_i];
//    } else if (counter_i < 24) { // C-inner
//      energyArea[3] += energy[counter_i];
//      energyArea[5] += energy[counter_i];
//    } else if (counter_i < 32) { // C-outer
//      energyArea[4] += energy[counter_i];
//      energyArea[5] += energy[counter_i];
//    }

    for (int counter_j = 0; counter_j < 32; counter_j++) {
      // Fill quanitity Eij = Ei*Ej for correlation calc
      m_energy_ij[counter_i][counter_j] += energy[counter_i] * energy[counter_j];

      if (m_hasEnergyHit[counter_i] && m_hasEnergyHit[counter_j]) {
        m_h_coinEnergyHits->Fill(counter_i, counter_j);
        //if(counter_i<16 && counter_j>15) // get one counter on each side (A and C)
        //{
        //	m_h_timeDiff->Fill(time[counter_i]-time[counter_j]);
        //	m_h_timeDiffLumi->Fill(LB,time[counter_i]-time[counter_j]);
        //}
      }

      if (m_hasPIT[counter_i] && m_hasPIT[counter_j]) {
        m_h_coinPIT->Fill(counter_i, counter_j);
      }
      
      if (m_hasTBP[counter_i] && m_hasTBP[counter_j]) {
        m_h_coinTBP->Fill(counter_i, counter_j);
      }
      
      if (m_hasTAP[counter_i] && m_hasTAP[counter_j]) {
        m_h_coinTAP->Fill(counter_i, counter_j);
      }
      
      if (m_hasTAV[counter_i] && m_hasTAV[counter_j]) {
        m_h_coinTAV->Fill(counter_i, counter_j);
      }

    }
  }
  //// Fill for Area correlation
//  for (int i = 0; i < 6; i++) {
//    m_energy_sq_tot[i] += energyArea[i] * energyArea[i];
//    m_energy_sum_tot[i] += energyArea[i] * energyArea[i];
//    for (int j = 0; j < 6; j++)
//      m_energy_ij_tot[i][j] += energyArea[i] * energyArea[j];
//  }

  m_numEvents++;

  return StatusCode::SUCCESS;
}

//=====================================================================
StatusCode TileMBTSMonTool::procHistograms() {

  if (endOfRunFlag() && m_isOnline) {
    return StatusCode::SUCCESS;
  }

  if (endOfRunFlag()) {
    ATH_MSG_INFO( "in procHistograms()" );
  }

//  double ped, hfn, lfn;
//  double corr;
//  for (int counter_i = 0; counter_i < 32; counter_i++) {
//    ped = m_h_pedestal[counter_i]->GetMean();
//    hfn = m_h_hfNoise[counter_i]->GetMean();
//    lfn = m_h_pedestal[counter_i]->GetRMS();
//
//    m_h_sumPed->Fill(counter_i, ped);
//    m_h_sumHFNoise->Fill(counter_i, hfn);
//    m_h_sumLFNoise->Fill(counter_i, lfn);
//
//    for (int counter_j = 0; counter_j < 32; counter_j++) {
//      if (TMath::Abs(
//          m_numEvents * m_energy_sq[counter_i] - m_energy_sum[counter_i] * m_energy_sum[counter_i]) > 1e-7) {
//        if (TMath::Abs(m_numEvents * m_energy_sq[counter_j] - m_energy_sum[counter_j] * m_energy_sum[counter_j]) > 1e-7) {
//          // Calculate the correlation between 2 counters and fill histogram
//          corr = (m_numEvents * m_energy_ij[counter_i][counter_j] - m_energy_sum[counter_i] * m_energy_sum[counter_j]);
//          corr /= sqrt( m_numEvents * m_energy_sq[counter_i] - m_energy_sum[counter_i] * m_energy_sum[counter_i]);
//          corr /= sqrt( m_numEvents * m_energy_sq[counter_j] - m_energy_sum[counter_j] * m_energy_sum[counter_j]);
//          m_h_corrEnergy->Fill(counter_i, counter_j, corr);
//        }
//      }
//    }        // counter_j loop
//  }        //counter_i loop
//           //  Compute summary correlation
//  for (int i = 0; i < 6; i++) {
//    for (int j = 0; j < 6; j++) {
//      if (TMath::Abs(m_numEvents * m_energy_sq_tot[i] - m_energy_sum_tot[i] * m_energy_sum_tot[i]) > 1e-7) {
//        if (TMath::Abs(m_numEvents * m_energy_sq_tot[j] - m_energy_sum_tot[j] * m_energy_sum_tot[j]) > 1e-7) {
//          corr = (m_numEvents * m_energy_ij_tot[i][j] - m_energy_sum_tot[i] * m_energy_sum_tot[j]);
//          corr /= sqrt( m_numEvents * m_energy_sq_tot[i] - m_energy_sum_tot[i] * m_energy_sum_tot[i]);
//          corr /= sqrt( m_numEvents * m_energy_sq_tot[j] - m_energy_sum_tot[j] * m_energy_sum_tot[j]);
//          m_h_corrEnergyTot->Fill(i, j, corr);
//        }
//      }
//    }
//  }

  return StatusCode::SUCCESS;
}

//=====================================================================
StatusCode TileMBTSMonTool::checkHists(bool) {
  ATH_MSG_INFO( "in checkHists()" );
  return StatusCode::SUCCESS;
}


//=====================================================================
void TileMBTSMonTool::setBinLabels(TH2 * h_theHist, int type, bool doY) {
  if (type == 1) {

    for (int i = 0; i < 32; i++) {
      h_theHist->GetXaxis()->SetBinLabel(i + 1, m_counterNames[i].c_str());
      if (doY) h_theHist->GetYaxis()->SetBinLabel(i + 1, m_counterNames[i].c_str());
    }

    h_theHist->GetXaxis()->LabelsOption("v");
  } else if (type == 2) {
    h_theHist->GetXaxis()->SetBinLabel(1, "A-Inner");
    h_theHist->GetXaxis()->SetBinLabel(2, "A-Outer");
    h_theHist->GetXaxis()->SetBinLabel(3, "A-Total");
    h_theHist->GetXaxis()->SetBinLabel(4, "C-Inner");
    h_theHist->GetXaxis()->SetBinLabel(5, "C-Outer");
    h_theHist->GetXaxis()->SetBinLabel(6, "C-Total");
    if (doY) {
      h_theHist->GetYaxis()->SetBinLabel(1, "A-Inner");
      h_theHist->GetYaxis()->SetBinLabel(2, "A-Outer");
      h_theHist->GetYaxis()->SetBinLabel(3, "A-Total");
      h_theHist->GetYaxis()->SetBinLabel(4, "C-Inner");
      h_theHist->GetYaxis()->SetBinLabel(5, "C-Outer");
      h_theHist->GetYaxis()->SetBinLabel(6, "C-Total");
    }
  }
}

//=====================================================================
void TileMBTSMonTool::setBinLabels(TH1 * h_theHist, int type) {
  if (type == 1) {

    for (int i = 0; i < 32; i++) {
      h_theHist->GetXaxis()->SetBinLabel(i + 1, m_counterNames[i].c_str());
    }

    h_theHist->GetXaxis()->LabelsOption("v");
  } else if (type == 2) {
    h_theHist->GetXaxis()->SetBinLabel(1, "A-Inner");
    h_theHist->GetXaxis()->SetBinLabel(2, "A-Outer");
    h_theHist->GetXaxis()->SetBinLabel(3, "A-Total");
    h_theHist->GetXaxis()->SetBinLabel(4, "C-Inner");
    h_theHist->GetXaxis()->SetBinLabel(5, "C-Outer");
    h_theHist->GetXaxis()->SetBinLabel(6, "C-Total");
  }
}
