/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandle.h"

// Tile includes
#include "TileCalibAlgs/TileExpertDump.h"
#include "TileConditions/TileInfoLoader.h"
#include "TileConditions/TileInfo.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTrigType.h"
#include "TileIdentifier/TileFragHash.h"
#include "Identifier/IdentifierHash.h"


#include <iomanip>


/////////////////////////////////////////////////////////////////////////////

TileExpertDump::TileExpertDump(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_emExpertOptions(0)
  , m_tileHWID(0)
{
  declareProperty("PrintExpertEmscale"     , m_printExpertEmscale=false      ,"Switch on expert calibration chain printout");
  declareProperty("PrintExpertEmscaleOnl"  , m_printExpertEmscaleOnl=false   ,"Switch on expert onl calibration chain printout");

  declareProperty("useOflCisFitLin" , m_OflCisFitLin  = false, "Use calibration of OflCisFitLin");
  declareProperty("useOflCisFitNln" , m_OflCisFitNln  = false, "Use calibration of OflCisFitNln");
  declareProperty("useOflLasLin"    , m_OflLasLin     = false, "Use calibration of OflLasLin");
  declareProperty("useOflLasNln"    , m_OflLasNln     = false, "Use calibration of OflLasNln");
  declareProperty("useOflLasPart"   , m_OflLasPart    = false, "Use calibration of OflLasPart");
  declareProperty("useOflCes"       , m_OflCes        = false, "Use calibration of OflCes");
  declareProperty("useOflEms"       , m_OflEms        = false, "Use calibration of OflEms");
  declareProperty("useOflLasLinPisa", m_OflLasLinPisa = false, "Use calibration of OflLasLinPisa");
  declareProperty("useOflLasNlnPisa", m_OflLasNlnPisa = false, "Use calibration of OflLasNlnPisa");
  declareProperty("useOflMinbias"   , m_OflMinbias    = false, "Use calibration of OflMinbias");
  declareProperty("useOnlCisLin"    , m_OnlCisLin     = false, "Use calibration of OnlCisLin");
  declareProperty("useOnlLasLin"    , m_OnlLasLin     = false, "Use calibration of OnlLasLin");
  declareProperty("useOnlCes"       , m_OnlCes        = false, "Use calibration of OnlCes");
  declareProperty("useOnlEms"       , m_OnlEms        = false, "Use calibration of OnlEms");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

TileExpertDump::~TileExpertDump() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TileExpertDump::initialize() {

  ATH_MSG_DEBUG( "in initialize()" );

  //=== EventInfo key
  ATH_CHECK( m_eventInfoKey.initialize() );
  
  //=== Get TileHWID
  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  //=== Get Emscale Calib Options for TileExpertToolEmscale
  CHECK( setEmExpertOptions() );

  //=== Get TileExpertToolEmscale and set the options
  CHECK( m_tileExpertToolEmscale.retrieve() );

  m_tileExpertToolEmscale->setEmOptions(m_emExpertOptions);

  return StatusCode::SUCCESS;
}

//
//___________________________________________________________________________________________
StatusCode TileExpertDump::setEmExpertOptions() {

  if(m_OflCisFitLin ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflCisFitLin, true );
  if(m_OflCisFitNln ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflCisFitNln, true );
  if(m_OflLasLin    ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflLasLin   , true );
  if(m_OflLasNln    ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflLasNln   , true );
  if(m_OflLasPart   ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflLasPart  , true );
  if(m_OflCes       ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflCes      , true );
  if(m_OflEms       ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflEms      , true );
  if(m_OflLasLinPisa) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflLasLinPisa,true );
  if(m_OflLasNlnPisa) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflLasNlnPisa,true );
  if(m_OflMinbias   ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OflMinbias  , true );
  if(m_OnlCisLin    ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OnlCisLin   , true );
  if(m_OnlLasLin    ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OnlLasLin   , true );
  if(m_OnlCes       ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OnlCes      , true );
  if(m_OnlEms       ) m_emExpertOptions.setEmscaleCalibBit(TileEmscaleCalibOptions::OnlEms      , true );

  return StatusCode::SUCCESS;
}


//
//___________________________________________________________________________________________
StatusCode TileExpertDump::execute() {

  ATH_MSG_DEBUG( " in execute()" );

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  ATH_CHECK( eventInfo.isValid() );

  ATH_MSG_DEBUG( "Event: ["
                << eventInfo->runNumber() << ", "
                << eventInfo->eventNumber() << ", "
                << eventInfo->lumiBlock() << ": "
                << eventInfo->timeStamp() << "]" );


  //=== print out stuff
  if (m_printExpertEmscale) printExpertEmscale();
  if (m_printExpertEmscaleOnl) printExpertEmscaleOnl();

  return StatusCode::SUCCESS;
}

//
//___________________________________________________________________________________________
void TileExpertDump::printExpertEmscale() {

  ATH_MSG_DEBUG( "In printExpertEmscale()" );

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
                         << m_tileExpertToolEmscale->doCalibCis(drawerIdx, channel, adc, 1.) << " (CIS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileExpertToolEmscale->doCalibLas(drawerIdx, channel, 1.) << " (LAS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileExpertToolEmscale->doCalibCes(drawerIdx, channel, 1.) << " (CES) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileExpertToolEmscale->doCalibEms(drawerIdx, channel, 1.) << " (EMS) | ";

          float chanCalMeV = m_tileExpertToolEmscale->channelCalib(drawerIdx, channel, adc, 1.
                                                                   , TileRawChannelUnit::ADCcounts
                                                                   , TileRawChannelUnit::MegaElectronVolts);

          msg(MSG::INFO) << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                          << chanCalMeV << " (total)";

          //=== "Undo" online calibration only if this functionality is available
          if (m_tileExpertToolEmscale->getOnlCacheUnit() != TileRawChannelUnit::Invalid) {
            msg(MSG::INFO) << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                           << m_tileExpertToolEmscale->channelCalib(drawerIdx, channel, adc, chanCalMeV
                                                                    , TileRawChannelUnit::OnlineMegaElectronVolts
                                                                    , TileRawChannelUnit::ADCcounts)
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
      ATH_MSG_INFO( ros << "/" << drawer << "/" << channel << "/" << adc << " : "
                   << "ADC counts = " << adcCounts
                   << "\t---> " << m_tileExpertToolEmscale->doCalibCis(20, 0, adc, energy) );

    }  //end adcCounts
  }  //end adc

}

//
//___________________________________________________________________________________________
void TileExpertDump::printExpertEmscaleOnl() {

  ATH_MSG_DEBUG( "In printExpertEmscaleOnl()" );

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
                         << m_tileExpertToolEmscale->doCalibCisOnl(drawerIdx, channel, adc, 1.) << " (ONL_CIS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileExpertToolEmscale->doCalibLasOnl(drawerIdx, channel, 1.) << " (ONL_LAS) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileExpertToolEmscale->doCalibCesOnl(drawerIdx, channel, 1.) << " (ONL_CES) | "
                         << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << m_tileExpertToolEmscale->doCalibEmsOnl(drawerIdx, channel, 1.) << " (ONL_EMS) | ";

          float chanCalMeV = m_tileExpertToolEmscale->channelCalibOnl(drawerIdx, channel, adc, 1.
                                                                      , TileRawChannelUnit::OnlineMegaElectronVolts);

          msg(MSG::INFO) << std::fixed << std::right << std::setw(9) << std::setprecision(5)
                         << chanCalMeV << " (total)";

          msg(MSG::INFO) << endmsg;

        }  //end adc
      }  //end channel
    }  //end drawer
  }  //end ros

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TileExpertDump::finalize() {

  ATH_MSG_DEBUG( "in finalize()" );

  //=== Dump the emExpertOptions for cross-check
  if (m_printExpertEmscale || m_printExpertEmscaleOnl) {
    ATH_MSG_INFO( "Dumping m_emExpertOptions = " << m_emExpertOptions.getEmscaleCalibOptions() );

    std::ostringstream sout;
    m_emExpertOptions.dumpEmscaleCalibOptions(sout);
    ATH_MSG_INFO( sout.str() );
  }

  return StatusCode::SUCCESS;
}
