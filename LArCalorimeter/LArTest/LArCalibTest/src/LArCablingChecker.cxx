/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTest/LArCablingChecker.h"
#include "LArRawEvent/LArDigit.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArRawConditions/LArCalibParams.h"
#include "xAODEventInfo/EventInfo.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

LArCablingChecker::LArCablingChecker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    //m_chan(0),
    m_onlineHelper(0),
    m_emId(0),
    m_errorcellsThisEvent(0),
    m_errorCounterThisEvent(0),
    m_errorcellsPreviousEvent(0),
    m_errorCounterPreviousEvent(0),
    m_numberOfEventsWithThisErrorState(0),
    m_channelHashMax(0)
{
  m_count = 0;
  declareProperty("DigitKey", m_key = ""); 
  declareProperty("FileName", m_outFileName = "CablingSummary.txt");
  //declareProperty("Pattern", m_vPattern);
  declareProperty("ADCThreshold", m_ADCThreshold = 1250);
  declareProperty("DACHighGainThreshold", m_DACHighGainThreshold = 100);
  declareProperty("DACMediumGainThreshold", m_DACMediumGainThreshold = 1000);
  declareProperty("DACLowGainThreshold", m_DACLowGainThreshold = 10000);
  declareProperty("PrintAllCellsAndEvents", m_PrintAllCellsAndEvents = false);
  declareProperty("PrintDisconnectedCells", m_PrintDisconnectedCells = true);
  declareProperty("PrintEventSummary", m_printEventSummary = false);
  declareProperty("UseBadChannelTool", m_useBadChannelTool = true);
}

LArCablingChecker::~LArCablingChecker() {
  //empty
}

StatusCode LArCablingChecker::initialize() {
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_CLKey.initialize() );

  if (m_useBadChannelTool) {
     ATH_CHECK( m_BCKey.initialize() );
  }

  m_outfile.open(m_outFileName.c_str(), std::ios::out);
  if (!m_outfile.is_open()) {
    ATH_MSG_ERROR ( "Unable to open output file with name " << m_outFileName );
    return StatusCode::FAILURE;
  }

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();

  m_channelHashMax = m_onlineHelper->channelHashMax();

  m_errorcellsPreviousEvent = 0;
  m_errorCounterPreviousEvent = 0;
  // Phony "previous" event so that the first event has something to compare to.
  m_errorcellsThisEvent = new std::vector<bool>(m_channelHashMax);
  m_errorCounterThisEvent = 0;
  m_numberOfEventsWithThisErrorState = 0;

  ATH_MSG_DEBUG ( "======== LArCablingChecker initialize successfully ========" );
  return StatusCode::SUCCESS;
}

StatusCode LArCablingChecker::execute() {
  const DataHandle<xAOD::EventInfo> thisEventInfo;
  ATH_CHECK( evtStore()->retrieve(thisEventInfo) );

  unsigned eventNb = thisEventInfo->eventNumber();

  ATH_MSG_INFO ( "======== executing event "<< eventNb << " ========" );
  //m_outfile << "Checking Event " << eventNb << "..." << std::endl;

  //log << MSG::INFO << "Retrieving LArDigitContainer. Key= " << m_key << std::endl; 
  const LArDigitContainer* larDigitCont = nullptr;
  ATH_CHECK( evtStore()->retrieve(larDigitCont, m_key) );
  
  //const LArCalibParams* calibParams;
  const DataHandle<LArCalibParams> calibParams;
  ATH_CHECK( detStore()->retrieve(calibParams,"LArCalibParams") );

  SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
  const LArBadChannelCont *bcCont {*readHandle};
  if(!bcCont) {
     ATH_MSG_ERROR( "Do not have Bad chan container " << m_BCKey.key() );
     return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_CLKey};
  const LArCalibLineMapping *clCont {*clHdl};
  if(!clCont) {
     ATH_MSG_ERROR( "Do not have calib line mapping !!!" );
     return StatusCode::FAILURE;
  }


  // Using vectors like this works because the hashes seem to be guaranteed to be collision-free.
  delete m_errorcellsPreviousEvent; // OK to delete 0, Stroustrup 6.2.6
  m_errorcellsPreviousEvent = m_errorcellsThisEvent;
  m_errorcellsThisEvent = new std::vector<bool>(m_channelHashMax); // All elements initialized to false, Stroustrup 16.3.4
  
  m_errorCounterPreviousEvent = m_errorCounterThisEvent;
  m_errorCounterThisEvent = 0;

  int cellsCounter = 0;
  
  int inconsistencyCounter = 0;
  int disconnectedCounter = 0;
  int badChannelCounter = 0;

  bool signalPresent;
  bool pulsed;
  bool badChannel;
  bool disconnected;

  // Put errors in ErrorList, to print if needed.
  std::ostringstream ErrorList;

  // Loop on all cells (digits).
  LArDigitContainer::const_iterator it=larDigitCont->begin();
  LArDigitContainer::const_iterator it_end=larDigitCont->end();

  for (;it != it_end; it++) {
    cellsCounter++;

    ErrorList.clear();

    // Get FEB info.
    const HWIdentifier online_id = (*it)->hardwareID();
    const HWIdentifier febid = m_onlineHelper->feb_Id(online_id);
  
    int chNb = m_onlineHelper->channel(online_id);

    // Check if channel is connected.
    if (!cabling->isOnlineConnected(online_id)) {
      disconnected = true;
      disconnectedCounter++;
    } else
      disconnected = false;

    Identifier offline_id;

    // Get offline id.
    bool noOffline_id = false;
    try {
      offline_id = cabling->cnvToIdentifier(online_id);
    }
    catch (const LArID_Exception&) {
      noOffline_id = true;
    }

    // Find out if signal over theshold is present.
    // Set signalPresent.
    const int max = *max_element((*it)->samples().begin(), (*it)->samples().end());
    if (max >= m_ADCThreshold) 
      signalPresent = true;
    else 
      signalPresent = false;
    
    // Loop on calibchannels to see which, if any, that pulsed the cell.
    // Set pulsed and DACvalue, and fill PulserList.
    pulsed = false;
    int DACvalue = -1;
    std::ostringstream PulserList;
    const std::vector<HWIdentifier>& calibChannelIDs=clCont->calibSlotLine(online_id);

    std::vector<HWIdentifier>::const_iterator csl_it=calibChannelIDs.begin();
    std::vector<HWIdentifier>::const_iterator csl_it_e=calibChannelIDs.end();

    for (;csl_it != csl_it_e; csl_it++) {
      //m_outfile << (*csl_it).get_compact() << " ";
      bool isPulsed=calibParams->isPulsed(eventNb, *csl_it);
      if (isPulsed) {
	int DACvalue_temp = calibParams->DAC(eventNb, *csl_it);
	if (DACvalue_temp > DACvalue)
	  DACvalue = DACvalue_temp;
	pulsed = true;
	const int slot = m_onlineHelper->slot(*csl_it);
	const int line = m_onlineHelper->channel(*csl_it);
	PulserList << std::hex << (*csl_it).get_compact() << std::dec << " " << slot << " " << line << " ";
      }
    }
    //m_outfile << std::endl;

    CaloGain::CaloGain gain = (*it)->gain();
    bool DACOverThreshold;
    switch (gain) {
    case CaloGain::LARLOWGAIN :
      DACOverThreshold = DACvalue >= m_DACLowGainThreshold;
      break;
    case CaloGain::LARMEDIUMGAIN :
      DACOverThreshold = DACvalue >= m_DACMediumGainThreshold;
      break;
    case CaloGain::LARHIGHGAIN :
      DACOverThreshold = DACvalue >= m_DACHighGainThreshold;
      break;
    default:
      DACOverThreshold = DACvalue >= m_DACHighGainThreshold;
    }
 
    // Find out if we have a bad cell.
    if (m_useBadChannelTool && !noOffline_id) {
      if (bcCont->offlineStatus(offline_id).good()) {
	badChannel = false;
      } else {
	badChannel = true;
	badChannelCounter++;
      }
    } else {
      badChannel = false;
    }

    // Analyze and report.

    bool inconsistency = (signalPresent != pulsed) && DACOverThreshold;
    if (inconsistency)
      inconsistencyCounter++;

    bool errorInCell = inconsistency || badChannel;
    if (errorInCell)
      m_errorCounterThisEvent++;

    std::ostringstream CellData;
    CellData << "0x" << std::hex << std::setw(8) << febid.get_compact() << std::dec
	     << std::setw(5) << m_onlineHelper->feedthrough(febid)
	     << std::setw(5) << m_onlineHelper->slot(febid)
	     << std::setw(5) << chNb;

    CellData << std::setw(5);
    if (noOffline_id)
      CellData << "-";
    else
      CellData << m_emId->sampling(offline_id);

    CellData << std::setw(5);
    if (noOffline_id)
      CellData << "-";
    else
      CellData << m_emId->eta(offline_id);

    CellData << std::setw(5);
    if (noOffline_id)
      CellData << "-";
    else
      CellData << m_emId->phi(offline_id);

    CellData << std::setw(7) << max;
    if (DACvalue >= 0) // Above have set DACvalue=-1 if got no DAC value.
      CellData << std::setw(7) << DACvalue;
    else
      CellData << std::setw(7) << "-";

    if (disconnected) {
      if (m_PrintDisconnectedCells) {
	ErrorList << "    D ";
	ErrorList << CellData.str() << std::endl;
      }
    }
    else {
      // Set relevant bit in m_errorcellsThisEvent if error in this cell.
      if (errorInCell)
	(*m_errorcellsThisEvent)[m_onlineHelper->channel_Hash(online_id)] = true;

      if (errorInCell || m_PrintAllCellsAndEvents) {
	if (inconsistency)
	  ErrorList << "I";
	else
	  ErrorList << " ";
	if (pulsed)
	  ErrorList << "P";
	else
	  ErrorList << " ";
	if (signalPresent)
	  ErrorList << "S";
	else
	  ErrorList << " ";
	if (badChannel)
	  ErrorList << "B  ";
	else
	  ErrorList << "   ";
	
	ErrorList << CellData.str();
	
	if (pulsed)
	  ErrorList << " " << PulserList.str();
	
	ErrorList << std::endl;
      }
    }
  } //End looping on all cells
    
  ErrorList << std::endl;

  if (m_printEventSummary) {
    ErrorList << "Examined " << cellsCounter << " cells in event " << eventNb << ". Found:" << std::endl;
    ErrorList << std::left << std::setw(25) << "Inconsistencies: "
	      << std::right << std::setw(4) << inconsistencyCounter << std::endl;
    ErrorList << std::left << std::setw(25) << "Bad cells: "
	      << std::right << std::setw(4) << badChannelCounter << std::endl;
    ErrorList << std::left << std::setw(25) << "Sum errors: "
	      << std::right << std::setw(4) << m_errorCounterThisEvent << std::endl;
    ErrorList << std::left << std::setw(25) << "Disconnected cells: "
	      << std::right << std::setw(4) << disconnectedCounter << std::endl;
    ErrorList << std::endl;
  }

  if (!m_PrintAllCellsAndEvents) {
    // Compare m_errorcellsThisEvent to m_errorcellsPreviousEvent
    if (*m_errorcellsThisEvent == *m_errorcellsPreviousEvent) { // Pray to God this is optimized.
      // Same status as previous event, increase counter and go to next event.
      m_numberOfEventsWithThisErrorState++;
    }
    else {
      // A new status for this event. Print number of repeats for the previous one and print new state, if an error.
      if ((m_errorCounterPreviousEvent != 0) && (m_numberOfEventsWithThisErrorState > 1))
	m_outfile << "Last error occurred in "
		  << m_numberOfEventsWithThisErrorState << " events." << std::endl << std::endl;
      
      if (m_errorCounterThisEvent != 0) {
	m_outfile << "Error in event "<< std::setw(7) << eventNb
		  << " ==========================================================================" << std::endl;
	m_outfile << "I=Inconsistency P=Pulsed S=Signal B=Bad D=Disconnected" << std::endl;
	m_outfile << "      "
		  << std::left << std::setw(10) << "FEB"
		  << std::right << std::setw(5) << "FT"
		  << std::setw(5) << "SL"
		  << std::setw(5) << "CH"
		  << std::setw(5) << "Smpl"
		  << std::setw(5) << "Eta"
		  << std::setw(5) << "Phi"
		  << std::setw(7) << "ADC"
		  << std::setw(7) << "DAC"
		  << " Pulsed by calibchannel slot line"
		  << std::endl;
	
	m_outfile << ErrorList.str();
      }
      
      m_numberOfEventsWithThisErrorState = 1;
    }
  }
  else { // m_PrintAllCellsAndEvents==true
    m_outfile << "Status of event "<< std::setw(7) << eventNb
	      << " ==========================================================================" << std::endl;
    m_outfile << "I=Inconsistency P=Pulsed S=Signal B=Bad D=Disconnected" << std::endl;
    m_outfile << "      "
	      << std::left << std::setw(10) << "FEB"
	      << std::right << std::setw(5) << "FT"
	      << std::setw(5) << "SL"
	      << std::setw(5) << "CH"
	      << std::setw(5) << "Smpl"
	      << std::setw(5) << "Eta"
	      << std::setw(5) << "Phi"
	      << std::setw(7) << "ADC"
	      << std::setw(7) << "DAC"
	      << " Pulsed by slot line"
	      << std::endl;
    
    m_outfile << ErrorList.str();
  }

  return StatusCode::SUCCESS;
}

StatusCode LArCablingChecker::finalize() {
  if ((m_errorCounterThisEvent != 0) && (m_numberOfEventsWithThisErrorState > 1))
    m_outfile << "Last error occurred in " << m_numberOfEventsWithThisErrorState << " events." << std::endl << std::endl;

  m_outfile.close();

  delete m_errorcellsThisEvent;
  delete m_errorcellsPreviousEvent;

  return StatusCode::SUCCESS;
}
