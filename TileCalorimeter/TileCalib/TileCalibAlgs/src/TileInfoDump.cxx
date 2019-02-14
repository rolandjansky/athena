/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/ITHistSvc.h"

// Athena includes
#include "xAODEventInfo/EventInfo.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

// Tile includes
#include "TileCalibAlgs/TileInfoDump.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTrigType.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolIntegrator.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolNoiseRawChn.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileConditions/TileCondToolMuID.h"
#include "TileConditions/TileCondToolAutoCr.h"

#include "TH2.h"

#include <iomanip>
#include <sstream>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////

TileInfoDump::TileInfoDump(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
    , m_tileBadChanTool("TileBadChanTool")
    , m_tileToolEmscale("TileCondToolEmscale")
    , m_tileToolIntegrator("TileCondToolIntegrator")
    , m_tileToolTiming("TileCondToolTiming")
    , m_tileToolPulseShape("TileCondToolPulseShape")
    , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_tileToolAutoCr("TileCondToolAutoCr")
    , m_tileToolNoiseRawChn("TileCondToolNoiseRawChn", this)
    , m_tileToolOfcCool("TileCondToolOfcCool")
    , m_tileToolMuID("TileCondToolMuID")
    , m_thistSvc(0)
    , m_tileHWID(0)
    , m_tileID(0)
    , m_caloMgr(0)
    , m_h_badCellA(0)
    , m_h_badCellBC(0)
    , m_h_badCellD(0)
    , m_h_badCellGap(0)
    , m_h_badCell(0)

{
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("TileCondToolIntegrator", m_tileToolIntegrator);
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolPulseShape", m_tileToolPulseShape);
  declareProperty("TileCondToolOfcCool", m_tileToolOfcCool);
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("TileCondToolAutoCr", m_tileToolAutoCr);
  declareProperty("TileCondToolNoiseRawChn", m_tileToolNoiseRawChn);
  declareProperty("AthenaIsOnline", m_isOnline = false, "Availability of COOL folders depends on Athena mode");
  declareProperty("PrintEmscale", m_printEmscale = false, "Switch on calibration chain printout");
  declareProperty("PrintEmscaleOnl", m_printEmscaleOnl = false, "Switch on online calibration chain printout");
  declareProperty("PrintLaser", m_printLaser = false, "Switch on all laser constants printout");
  declareProperty("PrintIntegrator", m_printIntegrator = false, "Switch on integrator printout");
  declareProperty("PrintPedestals", m_printPedestals = false, "Switch on Pedstals print out");
  declareProperty("PrintTimingCorrections", m_printTimingCorrections = false, "Switch on Timing Corrections print out");
  declareProperty("PrintPulseShapes", m_printPulseShapes = false, "Switch on Pulse Shape print out");
  declareProperty("PrintOfcsCool", m_printOfcsCool = false, "Switch on OFC print out");
  declareProperty("PrintPulseShapesStep", m_printPulseShapesStep = 0.5, "Time step (ns) of PulseShapes print out");
  declareProperty("PrintBadChannels", m_printBadChannels = false, "Switch on Bad Channels print out");
  declareProperty("PrintBadCells", m_printBadCells = false, "Switch on Bad Cells print out");
  declareProperty("PrintMuID", m_printMuID = false, "Switch on MuID threshold print out");
  declareProperty("Print1gNoise", m_print1gNoise = false, "Switch on 1g noise print out");
  declareProperty("PrintOfcRos", m_printOfcRos = 0, "Print OFC for this ros (0 by default)");
  declareProperty("PrintOfcDrawer", m_printOfcDrawer = 0, "Print OFC for this drawer (0 by default)");
  declareProperty("PrintOfcChannel", m_printOfcChannel = 0, "Print OFC for this channel (0 by default)");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

TileInfoDump::~TileInfoDump() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TileInfoDump::initialize() {

  ATH_MSG_DEBUG( "in initialize()" );

  //=== EventInfo key
  ATH_CHECK( m_eventInfoKey.initialize() );

  //=== Get TileHWID
  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  //=== CaloDetDescrManager
  CHECK( detStore()->retrieve(m_caloMgr) );

  //=== Get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  //=== Get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  //=== Get TileCondToolIntegrator
  CHECK( m_tileToolIntegrator.retrieve() );

  //=== Get TileCondToolTiming
  CHECK( m_tileToolTiming.retrieve() );

  //=== Get TileCondToolPulseShape
  CHECK( m_tileToolPulseShape.retrieve() );

  //=== Get TileCondToolOfcCool
  CHECK( m_tileToolOfcCool.retrieve() );

  //=== Get TileCondToolNoiseSample
  CHECK( m_tileToolNoiseSample.retrieve() );

  //=== Get TileCondToolAutoCr
  CHECK( m_tileToolAutoCr.retrieve() );

  //=== Get TileCondToolMuID
  CHECK( m_tileToolMuID.retrieve() );

  //=== Get TileCondToolNoiseRawChn
  if (m_printPedestals || m_print1gNoise) {
    CHECK( m_tileToolNoiseRawChn.retrieve() );
  }

  //=== Retrieve TileID helpers from det store
  CHECK( detStore()->retrieve(m_tileID, "TileID") );

  //--- Get a handle on the NTuple and histogramming service
  CHECK( service("THistSvc", m_thistSvc) );

  //--- Histogram initialization
  m_h_badCellA = new TH2F("m_h_badCellA", "Etaphi_maskedCell A", 34, -1.7, 1.7, 64, -3.2, 3.2);
  m_h_badCellBC = new TH2F("m_h_badCellBC", "Etaphi_maskedCell BC", 34, -1.7, 1.7, 64, -3.2, 3.2);
  m_h_badCellD = new TH2F("m_h_badCellD", "Etaphi_maskedCell D", 34, -1.7, 1.7, 64, -3.2, 3.2);
  m_h_badCellGap = new TH2F("m_h_badCellGap", "Etaphi_maskedCell Gap", 34, -1.7, 1.7, 64, -3.2, 3.2);
  m_h_badCell = new TH2F("m_h_badCell", "Etaphi_maskedCell All", 34, -1.7, 1.7, 64, -3.2, 3.2);

  CHECK( m_thistSvc->regHist("/AANT/m_h_badCellA", m_h_badCellA) );
  CHECK( m_thistSvc->regHist("/AANT/m_h_badCellBC", m_h_badCellBC) );
  CHECK( m_thistSvc->regHist("/AANT/m_h_badCellD", m_h_badCellD) );
  CHECK( m_thistSvc->regHist("/AANT/m_h_badCellGap", m_h_badCellGap) );
  CHECK( m_thistSvc->regHist("/AANT/m_h_badCell", m_h_badCell) );

  return StatusCode::SUCCESS;

}
//
//___________________________________________________________________________________________
StatusCode TileInfoDump::execute() {

  ATH_MSG_DEBUG( " in execute()" );

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  ATH_CHECK( eventInfo.isValid() );

  ATH_MSG_DEBUG( "Event: ["
                << eventInfo->runNumber() << ", "
                << eventInfo->eventNumber() << ", "
                << eventInfo->lumiBlock() << ": "
                << eventInfo->timeStamp() << "]" );




  m_h_badCell->Fill(1., 1.5, 1.);

  //=== Print out stuff
  if (m_printEmscale) printEmscale();
  if (m_printEmscaleOnl) printEmscaleOnl();
  if (m_printLaser) printLaser();
  if (m_printIntegrator) printIntegrator();
  if (m_printPedestals) printPedestals();
  if (m_printTimingCorrections) printTimingCorrections();
  if (m_printPulseShapes) printPulseShapes();
  if (m_printOfcsCool) printOfcs();
  if (m_printMuID) printMuID();
  if (m_print1gNoise) print1gNoise();

  //=== Print Bad Channels
  if (m_printBadChannels) printBadChannels();
  if (m_printBadCells) printBadCells();

  return StatusCode::SUCCESS;
}

//
//___________________________________________________________________________________________
void TileInfoDump::printEmscale() {
  ATH_MSG_DEBUG( "In printEmscale()" );

  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
    for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {

          msg(MSG::INFO) << ros << "/" << std::setw(2)
                         << drawer << "/" << std::setw(2)
                         << channel << "/"
                         << adc << " : " << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibCis(drawerIdx, channel, adc, 1.) << " (CIS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibLas(drawerIdx, channel, 1.) << " (LAS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibCes(drawerIdx, channel, 1.) << " (CES) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibEms(drawerIdx, channel, 1.) << " (EMS) | ";

          float chanCalMeV = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, 1.
                                                             , TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);

          msg(MSG::INFO) << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << chanCalMeV << " (total)";

          //=== "undo" online calibration only if this functionality is available
          if (m_tileToolEmscale->getOnlCacheUnit() != TileRawChannelUnit::Invalid) {
            msg(MSG::INFO) << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                           << m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, chanCalMeV
                                                              , TileRawChannelUnit::OnlineMegaElectronVolts, TileRawChannelUnit::ADCcounts)
                           << " (onlMev->oflADC)";
          }

          msg(MSG::INFO) << endmsg;

        } //end adc
      } //end channel
    } //end drawer
  } //end ros

  //===================================================================
  //=== Perform a CIS energy scan to test CIS LUT
  //===================================================================
  ATH_MSG_INFO( "Performing an energy scan to test CIS LUT" );

  unsigned int ros = 1;
  unsigned int drawer = 0;
  unsigned int channel = 0;
  for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {
    for (unsigned int adcCounts = 0; adcCounts < 1024; ++adcCounts) {
      double energy = static_cast<float>(adcCounts);
      ATH_MSG_INFO( ros << "/"  << drawer << "/" << channel << "/" << adc << " : "
                   << "ADC counts = " << adcCounts
                   << "\t---> " << m_tileToolEmscale->doCalibCis(20, 0, adc, energy) );

    }  //end adcCounts
  }  //end adc

}

//
//___________________________________________________________________________________________
void TileInfoDump::printEmscaleOnl() {

  ATH_MSG_DEBUG( "In printEmscaleOnl()" );

  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
    for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {

          msg(MSG::INFO) << ros << "/" << std::setw(2) << drawer << "/" << std::setw(2) << channel << "/" << adc << " : "
                         << std::fixed << std::right << std::setw(9)  << std::setprecision(5)
                         << m_tileToolEmscale->doCalibCisOnl(drawerIdx, channel, adc, 1.) << " (ONL_CIS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibLasOnl(drawerIdx, channel, 1.) << " (ONL_LAS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibCesOnl(drawerIdx, channel, 1.) << " (ONL_CES) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibEmsOnl(drawerIdx, channel, 1.) << " (ONL_EMS) | ";

          float chanCalMeV = m_tileToolEmscale->channelCalibOnl(drawerIdx, channel, adc, 1., TileRawChannelUnit::OnlineMegaElectronVolts);

          msg(MSG::INFO) << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << chanCalMeV << " (total)";

          msg(MSG::INFO) << endmsg;

        }  //end adc
      }  //end channel
    }  //end drawer
  }  //end ros

}

//
//___________________________________________________________________________________________
void TileInfoDump::printLaser() {
 ATH_MSG_DEBUG( "In printLaser()" );

 for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
   unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
    for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {

          msg(MSG::INFO) << ros << "/" << std::setw(2)
                         << drawer << "/" << std::setw(2)
                         << channel << "/"
                         << adc << " : "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->getCesRefLas(drawerIdx, channel, adc) << " (absolute) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->doCalibLas(drawerIdx, channel, 1.) << " (channel) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->getLasFiber(drawerIdx, channel) << " (fiber) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileToolEmscale->getLasPartition(drawerIdx) << " (partition)";

          msg(MSG::INFO) << endmsg;

        }  //end adc
      }  //end channel
    }  //end drawer
  }  //end ros

}

//
//___________________________________________________________________________________________
void TileInfoDump::printIntegrator() {

  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
    for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        for (unsigned int adc = 0; adc < 6; ++adc) {

          //=== Output from the tool
          ATH_MSG_INFO( ros << "/"
                        << drawer << "/"
                        << channel<< "/"
                        << adc << " : "
                        << "Gain=" << m_tileToolIntegrator->getGain(drawerIdx, channel, adc) << "\t"
                        << m_tileToolIntegrator->getGainError(drawerIdx, channel, adc) << "\t"
                        << m_tileToolIntegrator->getChi2(drawerIdx, channel, adc) << "\t"
                        << "Ped=" << m_tileToolIntegrator->getPedestal(drawerIdx, channel, adc) << "\t"
                        << m_tileToolIntegrator->getDACForPed(drawerIdx, channel, adc) << "\t"
                        << m_tileToolIntegrator->getSigmaOfPed(drawerIdx, channel, adc) << "\t"
                        << m_tileToolIntegrator->getRMSOfPed(drawerIdx, channel, adc) << "\t"
                        << m_tileToolIntegrator->getSigmaOfRMS(drawerIdx, channel, adc) << "\t" );

        }  //end adc
      }  //end channel
    }  //end drawer
  }  //end ros

}

//
//___________________________________________________________________________________________
void TileInfoDump::printPedestals() {

  std::vector<float> vecAutoCr;

  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
    for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {

          //=== Retrieve autocorrelations
          m_tileToolAutoCr->getAutoCorr(drawerIdx, channel, adc, vecAutoCr);

          //=== Output from the tool
          msg(MSG::INFO) << ros << "/"
                         << drawer << "/"
                         << channel << "/"
                         << adc << " : "
                         << "Pedestal level=" << m_tileToolNoiseSample->getPed(drawerIdx, channel, adc) << "\t"
                         << "HFN=" << m_tileToolNoiseSample->getHfn(drawerIdx, channel, adc) << "\t"
                         << "LFN=" << m_tileToolNoiseSample->getLfn(drawerIdx, channel, adc) << "\t"
                         << "OF=" << m_tileToolNoiseRawChn->getNoise(drawerIdx, channel, adc) << "\t"
                         << "ACR = ";
          for (unsigned int i = 0; i < vecAutoCr.size(); ++i) {
            msg(MSG::INFO) << vecAutoCr[i] << " ";
          }

          msg(MSG::INFO) << endmsg;

        }  //end adc
      }  //end channel
    }  //end drawer
  }  //end ros

}

//
//___________________________________________________________________________________________
void TileInfoDump::print1gNoise() {

  std::vector<float> vecAutoCr;

  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
    for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {

          //=== Output from the tool
          ATH_MSG_INFO( "1g Noise (ros/dr/ch/gain) "
                        << ros << "/"
                        << drawer << "/"
                        << channel << "/"
                        << adc << " : "
			<< "Electronic (ADC counts) " << m_tileToolNoiseRawChn->getElectronicNoise(drawerIdx, channel, adc) << "\t"
                        << "PileUp (MeV) " << m_tileToolNoiseRawChn->getPileUpNoise(drawerIdx, channel, adc) );

        }  //end adc
      }  //end channel
    }  //end drawer
  }  //end ros

}

//
//___________________________________________________________________________________________
void TileInfoDump::printTimingCorrections() {

  //=== In Athena online mode only channel offset is available
  //=== In offline mode everything is available
  if (m_isOnline) {
    ATH_MSG_INFO( "Athena Online mode detected: only channel offset available." );
    for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
      unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
      for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
          for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {

            ATH_MSG_INFO( ros << "/"
                         << drawer << "/"
                         << channel << "/"
                         << adc << " : "
                         << "Time correction: " << m_tileToolTiming->getSignalPhase(drawerIdx, channel, adc)
                         << "\tchannel: " << m_tileToolTiming->getChannelOffset(drawerIdx, channel, adc) );

          }  //end adc
        }  //end channel
      }  //end drawer
    }  //end ros
  } else { // offline mode
    for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
      unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
      for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
          for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {

            //.. GL 130429: remove references to obsolete condDB folders.
            ATH_MSG_INFO( ros << "/"
                         << drawer << "/"
                         << channel << "/"
                         << adc << " : "
                         << "Time correction: " << m_tileToolTiming->getSignalPhase(drawerIdx, channel, adc)
                         << "\tchannel: " << m_tileToolTiming->getChannelOffset(drawerIdx, channel, adc) );

          } //end adc
        } //end channel
      } //end drawer
    } //end ros
  } //offline mode
}

//
//___________________________________________________________________________________________
void TileInfoDump::printPulseShapes() {

  //=== dump default channel only
  unsigned int ros = 0;
  unsigned int drawer = 0;
  unsigned int channel = 0;
  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

  float y = 0;
  float dy = 0;
  float time = 0;

  float startpoint = -100.0;
  float endpoint = 150.0;

  //=== force m_printPulseShapesStep to have positive value
  if (m_printPulseShapesStep <= 0) m_printPulseShapesStep = 0.5;
  unsigned int npoint = static_cast<unsigned int>((endpoint - startpoint) / m_printPulseShapesStep);

  for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {
    for (unsigned int point = 0; point < npoint; ++point) {
      time = startpoint + point * m_printPulseShapesStep;
      //if(m_tileToolPulseShape->getPulseShapeYDY(drawerIdx,channel,adc,time,y,dy)) {
      m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, channel, adc, time, y, dy);
      ATH_MSG_INFO( ros << "/" << std::setw(2)
                   << drawer << "/" << std::setw(2)
                   << channel << "/"
                   << adc << " : "
                   << "Time, Y, DY: " << std::fixed << std::right << std::setw(9) << std::setprecision(6) << time
                   << "  " << std::fixed << std::right << std::setw(11) << std::setprecision(8) << y
                   << "  " << std::fixed << std::right << std::setw(11) << std::setprecision(8) << dy );

      //}
    } // end point    
  } //end adc

}

//
//___________________________________________________________________________________________
void TileInfoDump::printBadChannels() {

  ATH_MSG_DEBUG( "in printBadChannels()" );

  std::vector<std::string> vmod;

  std::ostringstream sSum;
  std::ostringstream sDet;

  //=== Tile partitions counters
  const unsigned int npar = 4;
  unsigned int bchn[npar];
  unsigned int badc[npar];
  for (unsigned int ros = 0; ros < npar; ++ros) {
    bchn[ros] = 0;
    badc[ros] = 0;
  }

  sDet << std::endl;
  sDet << ">>>>>>>>>>>>>>>   Detailed list of BAD Channels/ADCs <<<<<<<<<<<<<<<<" << std::endl;
  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    unsigned int drawerMax = TileCalibUtils::getMaxDrawer(ros);
    for (unsigned int drawer = 0; drawer < drawerMax; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

      unsigned int badadc = 0; //... counter of bad ADCs per drawer
      unsigned int badchn = 0; //... counter of bad channels per drawer

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

        //=== check channel status
        TileBchStatus status = m_tileBadChanTool->getChannelStatus(drawerIdx, channel);
        if (status.isAffected()) {
          if (status.isBad()) {
            sDet << TileCalibUtils::getDrawerString(ros, drawer)
                 << "/ch" << channel << " :B: "
                 << "has at least one problem: " << status.getString() << std::endl;

            if (ros > 0) {
              badchn++;
              bchn[ros - 1]++;
            }
          } else
            sDet << TileCalibUtils::getDrawerString(ros, drawer)
                 << "/ch" << channel << " :-: "
                 << "has at least one problem: " << status.getString() << std::endl;

          //==== check connected adc status
          for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {
            status = m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc);
            if (status.isBad()) {
              sDet << "-B-> " << TileCalibUtils::getDrawerString(ros, drawer)
                   << "/ch" << channel << "/" << adc << " : " << status.getString() << std::endl;

              if (ros > 0) {
                badadc++;
                badc[ros - 1]++;
              }
            } else
              sDet << "---> " << TileCalibUtils::getDrawerString(ros, drawer)
                   << "/ch" << channel << "/" << adc << " : " << status.getString() << std::endl;

          } //end adc
        } //end status!=0

      } //end channel

      if (badadc) {
        std::ostringstream myStream;
        myStream << TileCalibUtils::getDrawerString(ros, drawer) << ": " << badchn << "/" << badadc;
        vmod.push_back(myStream.str());
      }

    } //end drawer
  } //end ros

  ATH_MSG_INFO( sDet.str() );

  //=== Print summary
  sSum << std::endl;
  sSum << ">>> Summary of BAD channels/ADCs per drawer (in LBA, LBC, EBA, EBC) <<<<" << std::endl;
  if (vmod.size() == 0)
    sSum << "There are no bad ADCs in LBA, LBC, EBA and EBC" << std::endl;
  else {
    unsigned int totchn = 0;
    unsigned int totadc = 0;

    for (unsigned int ros = 0; ros < npar; ++ros) {
      totchn += bchn[ros];
      totadc += badc[ros];
    }

    sSum << "Total number of bad channels/ADC: " << totchn << "/" << totadc << std::endl;
    sSum << "Bad channels/ADC in LBA         : " << bchn[0] << "/" << badc[0] << std::endl;
    sSum << "Bad channels/ADC in LBC         : " << bchn[1] << "/" << badc[1] << std::endl;
    sSum << "Bad channels/ADC in EBA         : " << bchn[2] << "/" << badc[2] << std::endl;
    sSum << "Bad channels/ADC in EBC         : " << bchn[3] << "/" << badc[3] << std::endl;
    sSum << std::endl;

    std::vector<std::string>::iterator iter;
    for (iter = vmod.begin(); iter != vmod.end(); iter++)
      sSum << (*iter) << std::endl;
  }

  ATH_MSG_INFO( sSum.str() );

  //--- Output file: dump first the summary, then the details
  std::ofstream fcout("tileBadChannelsAdcs.txt");
  fcout << sSum.str() << std::endl;
  fcout << sDet.str() << std::endl;
  fcout.close();

}

//
//____________________________________________________________________________
void FillCell(TH2F *hist, const CaloDetDescrElement* elem) {
  //--- auxiliary function for filling of larger cells into histograms in printBadCells
  float step = 0.1;
  int nstep = elem->deta() / step;
  float seta = elem->eta() - 0.5 * (elem->deta() - step);
  for (int e = 0; e < nstep; e++)
    hist->Fill(seta + e * step, elem->phi(), 1.);
}

//
//____________________________________________________________________________
void TileInfoDump::printBadCells() {

  ATH_MSG_DEBUG( "in printBadCells()" );

  Identifier cell_id;
  IdContext cell_context = m_tileID->cell_context();
  int ncells = m_tileID->cell_hash_max();

  TileCablingService* cabling = TileCablingService::getInstance();
  bool run2 = cabling->isRun2Cabling();

  std::ostringstream sSum;
  std::ostringstream sDet;

  sDet << std::endl;
  sDet << ">>>>>>>>>>>>>>>   Detailed list of BAD Cells   <<<<<<<<<<<<<<<<" << std::endl;
  sDet << "tower 0-15: numbered by pseudorapidity from 0 with 0.1 increment in eta" << std::endl;

  int badcell_N = (run2) ? 0 : 4;
  int badcell_EBA = 0;
  int badcell_EBC = 0;
  int badcell_LBA = 0;
  int badcell_LBC = 0;
  int badcell_GapA = 0;
  int badcell_GapC = (run2) ? 0 : 4;

  HWIdentifier ch1_id, ch2_id;
  TileBchStatus ch1_status, ch2_status;
  for (int i = 0; i < ncells; ++i) {
    m_tileID->get_id((IdentifierHash) i, cell_id, &cell_context);

    //=== get calo detector description to obtain cell coordinates
    const CaloDetDescrElement* elem = m_caloMgr->get_element(cell_id);
    if (!elem) {
      //=== this should never happen
      ATH_MSG_ERROR( "CaloMgr returns NULL CaloDetDescrElement" );
      std::abort();
    }

    ch1_id = cabling->s2h_channel_id(m_tileID->pmt_id(cell_id, 0));
    ch1_status = m_tileBadChanTool->getChannelStatus(ch1_id);

    if (m_tileID->is_tile_gapscin(cell_id)) ch2_id = ch1_id;
    else ch2_id = cabling->s2h_channel_id(m_tileID->pmt_id(cell_id, 1));

    ch2_status = m_tileBadChanTool->getChannelStatus(ch2_id);

    //--- get channel
    int ich1 = m_tileHWID->channel(ch1_id);
    int ich2 = m_tileHWID->channel(ch2_id);

    if (ch1_status.isBad() && ch2_status.isBad()) {
      badcell_N++;
      if (m_tileID->section(cell_id) == 1) {
        if (m_tileID->is_positive(cell_id) || m_tileID->is_twosides(cell_id)) {
          badcell_LBA++;
          sDet << "LBA";
        }

        if (m_tileID->is_negative(cell_id)) {
          badcell_LBC++;
          sDet << "LBC";
        }
      }
      if (m_tileID->section(cell_id) == 2) {
        if (m_tileID->is_positive(cell_id)) {
          badcell_EBA++;
          sDet << "EBA";
        }

        if (m_tileID->is_negative(cell_id)) {
          badcell_EBC++;
          sDet << "EBC";
        }
      }
      if (m_tileID->section(cell_id) == 3) {
        if (m_tileID->is_positive(cell_id)) {
          badcell_GapA++;
          sDet << "GapA";
        }

        if (m_tileID->is_negative(cell_id)) {
          badcell_GapC++;
          sDet << "GapC";
        }
      }

      if (m_tileID->module(cell_id) < 9)
        sDet << "0" << m_tileID->module(cell_id) + 1;
      else
        sDet << m_tileID->module(cell_id) + 1;

      sDet << ", tower " << m_tileID->tower(cell_id) << ", ";

      switch (m_tileID->sample(cell_id)) {
        case 0: {
            sDet << "sampling A";
            FillCell(m_h_badCellA, elem);
          };
          break;
        case 1: {
            if (m_tileID->section(cell_id) == 1)
              sDet << "sampling BC";
            else
              sDet << "sampling B";
            FillCell(m_h_badCellBC, elem);
          };
          break;
        case 2: {
            sDet << "sampling D";
            FillCell(m_h_badCellD, elem);
          };
          break;
        case 3: {
            sDet << "gap and crack scintillators";
            FillCell(m_h_badCellGap, elem);
          };
          break;
        default:
          sDet << "individual tiles, used for Cesium calibration";
          break;
      }

      FillCell(m_h_badCell, elem);

      sDet << ", eta x phi = " << elem->eta()
           << " x " << elem->phi()
           << ", channels: " << ich1 << ", " << ich2 << std::endl;

    } // bad cell

  } // i, ncells

  if (!run2)
      sDet << "!!! Four cells in GapC are also bad but not listed above: EBC04 ch 1, EBC35 ch 0, EBC54 ch 1, and EBC61 ch 1 " << std::endl;

  ATH_MSG_INFO( sDet.str() );

  //=== bad cells summary
  float fncells = static_cast<float>(ncells);
  if (run2)
      fncells += 16 - 2; // adding 16 MBTS connected to PMT5 of special C10  and removing D4 in EBA15/EBC18
  else
      fncells = fncells - 2; // removing D4 in EBA15/EBC18

  sSum << std::endl;

  sSum << ">>>>>>>>>>>>>>>   Tilecal BAD Cells Summary <<<<<<<<<<<<<<<<" << std::endl;
  sSum << "Total number of cells including MBTS: " << fncells << std::endl;
  sSum << "Total number of bad cells:    " << badcell_N
       << " (" << std::setprecision(4) << 100 * static_cast<float>(badcell_N) / fncells << " %)" << std::endl;

  sSum << "Bad cells in LBA(+cell D0):   " << badcell_LBA
       << " (" << std::setprecision(4) << 100 * static_cast<float>(badcell_LBA) / fncells << " %)" << std::endl;

  sSum << "Bad cells in LBC:             " << badcell_LBC
       << " (" << std::setprecision(4) << 100 * static_cast<float>(badcell_LBC) / fncells << " %)" << std::endl;

  sSum << "Bad cells in EBA:             " << badcell_EBA
       << " (" << std::setprecision(4) << 100 * static_cast<float>(badcell_EBA) / fncells << " %)" << std::endl;

  sSum << "Bad cells in EBC:             " << badcell_EBC
       << " (" << std::setprecision(4) << 100 * static_cast<float>(badcell_EBC) / fncells << " %)" << std::endl;

  sSum << "Bad cells in Gap A(ITC+E1-4): " << badcell_GapA
       << " (" << std::setprecision(4) << 100 * static_cast<float>(badcell_GapA) / fncells << " %)" << std::endl;

  sSum << "Bad cells in Gap C(ITC+E1-4): " << badcell_GapC
       << " (" << std::setprecision(4) << 100 * static_cast<float>(badcell_GapC) / fncells << " %)" << std::endl;

  ATH_MSG_INFO( sSum.str() );

  //--- Output file: dump first the summary, then the details
  std::ofstream fcout("tileBadCells.txt");
  fcout << sSum.str() << std::endl;
  fcout << sDet.str() << std::endl;
  fcout.close();

}

//
//___________________________________________________________________________________________
void TileInfoDump::printMuID() {

  float thr_low_value = -999.;
  float thr_high_value = -999.;
  Identifier id;
  // loop over all TileCal cells
  IdContext cell_context = m_tileID->cell_context();
  int ncells = m_tileID->cell_hash_max();

  ATH_MSG_INFO( " printMuID: ncells " << ncells );
  ATH_MSG_INFO( " Cell_ID, Threshold detector(TileCal=5)/section/side/module/tower/sampling" );

  for (int i = 0; i < ncells; ++i) {
    {
      m_tileID->get_id((IdentifierHash) i, id, &cell_context);

      thr_low_value = m_tileToolMuID->getLowThresholdValue(id);
      thr_high_value = m_tileToolMuID->getHighThresholdValue(id);

      //=== Output from the tool
      ATH_MSG_INFO( " Cell_ID " << id
                   << " Threshold low/high " << thr_low_value << " " << thr_high_value
                   << ", " << m_tileID->to_string(id, -2) );
    }
  }
}

//
//____________________________________________________________________________
void TileInfoDump::printOfcs() {
  MsgStream log(msgSvc(), name());

  ATH_MSG_INFO("OFC will be printed for ros " << m_printOfcRos << ", drawer " << m_printOfcDrawer << ", channel " << m_printOfcChannel);

  //=== dump default channel only
  unsigned int gain = 0;
  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(m_printOfcRos, m_printOfcDrawer);
  int NPhases;
  int NFields;
  int Phamin;
  int Phamax;
  int NSamples;

  m_tileToolOfcCool->getOfcParams(drawerIdx, NPhases, NFields, Phamin, Phamax, NSamples);

  int phase_step = round(static_cast<double>(Phamax - Phamin) / (std::abs(NPhases) - 1));

  ATH_MSG_INFO(  "-------- OFC parameters ----->"
                 << " nPhases " << NPhases
                 << " nFields " << NFields
                 << " minimum phase " << Phamin * PHASE_PRECISION << " ns "
                 << " maximum phase " << Phamax * PHASE_PRECISION << " ns "
                 << " nSamples " << NSamples 
                 << " phase step " << phase_step * PHASE_PRECISION << " ns");



  for (gain = 0; gain < 2; gain++) {
    ATH_MSG_INFO(  "----------------- Gain " << gain << "-----------------" );

    for (int phase = Phamin; phase <= Phamax; phase += phase_step ) {
      float real_phase = float(phase) * PHASE_PRECISION;
      TileOfcWeightsStruct weights;
      m_tileToolOfcCool->getOfcWeights(drawerIdx, m_printOfcChannel, gain, real_phase, true, weights).ignore();

      ATH_MSG_INFO( "OFC phase " << real_phase << " ns");

      msg(MSG::INFO) << "OFC A";
      for (int i = 0; i < weights.n_samples; i++)
        msg(MSG::INFO) << " " << weights.w_a[i];
      msg(MSG::INFO) << endmsg;

      msg(MSG::INFO) << "OFC B";
      for (int i = 0; i < weights.n_samples; i++)
        msg(MSG::INFO) << " " << weights.w_b[i];
      msg(MSG::INFO) << endmsg;

      msg(MSG::INFO) << "OFC C";
      for (int i = 0; i < weights.n_samples; i++)
        msg(MSG::INFO) << " " << weights.w_c[i];
      msg(MSG::INFO) << endmsg;

      msg(MSG::INFO) << "OFC G";
      for (int i = 0; i < weights.n_samples; i++)
        msg(MSG::INFO) << " " << weights.g[i];
      msg(MSG::INFO) << endmsg;

      if (NFields >= 5) {
        msg(MSG::INFO) << "OFC DG";
        for (int i = 0; i < weights.n_samples; i++)
          msg(MSG::INFO) << " " << weights.dg[i];
        msg(MSG::INFO) << endmsg;
      }

    }
  }

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TileInfoDump::finalize() {

  ATH_MSG_DEBUG(  "in finalize()" );

  return StatusCode::SUCCESS;
}
