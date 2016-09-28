/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCaliWaveBuilderXtalk.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/ToolHandle.h"

#include "LArRawConditions/LArWaveHelper.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "Identifier/IdentifierHash.h"

#include "TString.h"

#include <fstream>
#include <algorithm>


using CLHEP::megahertz;
using CLHEP::ns;


//=========================================================================================================
LArCaliWaveBuilderXtalk::LArCaliWaveBuilderXtalk(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_groupingType("ExtendedFeedThrough"), // SubDetector, Single, FeedThrough
    m_onlineHelper(NULL),
    m_larCablingSvc("LArCablingService"),
    m_posOrNeg(0),
    m_barrelEndcap(0),
    m_isInnerWheel(0),
    m_event_counter(0),
    m_dt(0)
//=========================================================================================================
{
  declareProperty("KeyList",		m_keylistproperty);
  declareProperty("KeyOutput",		m_keyoutput="LArCaliWave");
  declareProperty("SamplingPeriod",     m_SamplingPeriod=1./(40.08*megahertz));
  declareProperty("NSteps",		m_NStep=24);				//Number of DelaySteps. FIXME: Try to get this information for somewhere else  
  declareProperty("ADCsaturation",	m_ADCsatur=0); 
  declareProperty("GroupingType",       m_groupingType); 

  // Description of the required pulsed channel to have the caliwaves reconstructed
  // PartitionPulsed = "BarrelA", "EndcapA", "BarrelC", "EndcapC"
  declareProperty("PartitionPulsed",	m_partition); 

  // For EMEC special crates
  declareProperty("SpecialCrate",	m_isSpecialCrate=false); 
  declareProperty("SpecialRegion",	m_emecSpecialRegion="OuterWheel");	// OuterWheel / InnerWheel

  // FeedthroughPul = [1-32] (Barrel) [1-24] (Endcap)
  // used to detect the right calibration pattern
  // please ensure that this FT has no bad channels / disconnected FEB
  declareProperty("FeedthroughPul",	m_feedthroughNumber=1);

  // CalibPattern = "StripSinglexx", "MiddleSinglexx", "BackSinglexx", 
  // EMB  : xx = [1-4] (Strip) xx = [1-8] (Middle) xx = [1-4] (Back) 
  // EMEC : xx = [1-4] (Strip) xx = [1-4] (Middle) xx = [1-4] (Back) 
  //                       and xx = [1-4] (Middle) xx = [1-4] (Back) for inner wheel
  declareProperty("CalibPattern",	m_calibPattern="StripSingle1");
  
  // Allow to cut waves
  // consider only the N=CutOnSample first samples to reduce needed memory (0 = no cut)
  declareProperty("CutOnSample",	m_cutOnSample=0);
}

//=========================================================================================================
LArCaliWaveBuilderXtalk::~LArCaliWaveBuilderXtalk() 
//=========================================================================================================
{}

//=========================================================================================================
StatusCode LArCaliWaveBuilderXtalk::initialize()
//=========================================================================================================
{ 

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize LArCaliWaveBuilderXtalk" << endmsg;

  // Set barrel_ec and pos_neg from PartitionPulsed
  m_barrelEndcap = 0; m_posOrNeg = 0;
  if      (m_partition == "BarrelC") {m_barrelEndcap = 0; m_posOrNeg = 0;}
  else if (m_partition == "BarrelA") {m_barrelEndcap = 0; m_posOrNeg = 1;}
  else if (m_partition == "EndcapC") {m_barrelEndcap = 1; m_posOrNeg = 0;}
  else if (m_partition == "EndcapA") {m_barrelEndcap = 1; m_posOrNeg = 1;}
  else {
    ATH_MSG_ERROR ( "Can't find partition " << m_partition << " ! Use BarrelC instead." );
  }
  ATH_MSG_INFO ( "Partition : " << m_partition << " (barrel_ec=" << m_barrelEndcap << " ; pos_neg=" << m_posOrNeg << ")" );
    
  // Test m_isSpecialCrate, set m_isInnerWheel, and check FT number for EMEC
  if (m_barrelEndcap==1) {

    // EMEC special crates
    if (m_isSpecialCrate){

      TString emecSpecialRegion_test = TString(m_emecSpecialRegion.c_str());
      emecSpecialRegion_test.ToLower();
      if (emecSpecialRegion_test.Contains("inner"))
	m_isInnerWheel = true;
      else {
	m_isInnerWheel = false;
	if (! emecSpecialRegion_test.Contains("outer"))
	  ATH_MSG_WARNING ( "Can't read EMEC special region (Inner/Outer Wheel) - set SpecialRegion to OuterWheel." );
      }

      if (m_isInnerWheel == false){	// EMEC outer wheel special crates
	if ( !(m_feedthroughNumber==2 || m_feedthroughNumber==9 || m_feedthroughNumber==15 || m_feedthroughNumber==21) ){
	  ATH_MSG_WARNING ( "FeedthroughPul=" << m_feedthroughNumber << ". For EMEC outer wheel special crates studies one should use FT = 2, 9, 15 or 21." );
	  ATH_MSG_WARNING ( "Set FeedthroughPul to 2.");
	  m_feedthroughNumber = 2;
	}
	ATH_MSG_INFO ( "Look at EM end-cap outer wheel special crates. Check calibration patterns with FT number " << m_feedthroughNumber );
      }
      else {				// EMEC inner wheel (special crates)
	if ( !(m_feedthroughNumber==3 || m_feedthroughNumber==10 || m_feedthroughNumber==16 || m_feedthroughNumber==22) ){
	  ATH_MSG_WARNING ( "FeedthroughPul=" << m_feedthroughNumber << ". For EMEC inner wheel studies one should use FT = 3, 10, 16 or 22." );
	  ATH_MSG_WARNING ( "Set FeedthroughPul to 3.");
	  m_feedthroughNumber = 3;
	}
	ATH_MSG_INFO ( "Look at EM end-cap inner wheel (special crates). Check calibration patterns with FT number " << m_feedthroughNumber );
      }

    }

    // EMEC standard crates
    else {
      if (m_feedthroughNumber==6
	  || m_feedthroughNumber==2 || m_feedthroughNumber==9  || m_feedthroughNumber==15 || m_feedthroughNumber==21
	  || m_feedthroughNumber==3 || m_feedthroughNumber==10 || m_feedthroughNumber==16 || m_feedthroughNumber==22
	  || m_feedthroughNumber>24) {
	ATH_MSG_WARNING ( "FeedthroughPul=" << m_feedthroughNumber << " : not in EMEC standard crates." );
	ATH_MSG_WARNING ( "Set FeedthroughPul to 1.");
	m_feedthroughNumber = 1;
      }
      ATH_MSG_INFO ( "Look at EM end-cap standard crates. Check calibration patterns with FT number " << m_feedthroughNumber );
    }
  } else {
    // EM Barrel
    ATH_MSG_INFO ( "Look at EM barrel. Check calibration patterns with FT number " << m_feedthroughNumber );
  }
  

  if (!m_keylistproperty.size()) // Not key list given
    {
      m_keylistproperty.push_back("HIGH");
      m_keylistproperty.push_back("MEDIUM");
      m_keylistproperty.push_back("LOW");
    }
  m_keylist= m_keylistproperty;

  //FIXME probably useless because m_wave isn't written anywhere
  ATH_CHECK( m_waves.setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( m_waves.initialize() );

  m_event_counter=0; 

  //m_dt=25*ns/m_NStep;
  m_dt = m_SamplingPeriod/m_NStep;

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_CHECK( m_larCablingSvc.retrieve() );
  
  return StatusCode::SUCCESS;
}

//=========================================================================================================
StatusCode LArCaliWaveBuilderXtalk::initializeCabling()
//=========================================================================================================
{ 
  // bug in LArTools, on-off map doesn't work at initialization stage

  ATH_MSG_INFO ( "Consider only partition " <<  m_partition );

  // Temporary solution to create onoff map as long as it is not done in calibSlotLine()
  // WAITING FOR A FIX in LArTools
 
  Identifier cellId = m_larCablingSvc->cnvToIdentifier(m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,1,4));
  ATH_MSG_INFO ( "Dummy ids - TEMPORARY " << m_onlineHelper->show_to_string(cellId) );


  // Fill the m_CalibLineHW vector,containing all the calib line HW id 
  // that are required to be pulsed
  // The m_calibPattern string is given in the job option  


  //--------------------------------------------------
  //	EM BARREL
  
  if (m_partition == "BarrelA"  || m_partition == "BarrelC") {  

    // StripSingle pattern  
    if( m_calibPattern.find("StripSingle",0) != std::string::npos ){
      HWIdentifier theConsidChannel;
      int pulsedSlot = 2;
      if (m_calibPattern.find("1",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,1);
      if (m_calibPattern.find("2",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,2);
      if (m_calibPattern.find("3",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,3);
      if (m_calibPattern.find("4",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,4);
      
      const std::vector<HWIdentifier>& calibLine = m_larCablingSvc->calibSlotLine(theConsidChannel);
      
      ATH_MSG_INFO ( "Considered Online Channel " << m_onlineHelper->show_to_string(theConsidChannel) );
      ATH_MSG_DEBUG ( "Number of associated calib line " << calibLine.size() );
      if (calibLine.size() != 0) {
	m_CalibLineHW.push_back(calibLine[0]);
	ATH_MSG_INFO ( "Calib line" << m_onlineHelper->show_to_string(calibLine[0]) );
      }
    }
    
    // MiddleSingle pattern
    if( m_calibPattern.find("MiddleSingle",0) != std::string::npos ){
      HWIdentifier theConsidChannel;
      int pulsedSlot = 11;
      if (m_calibPattern.find("1",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,3);
      if (m_calibPattern.find("2",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,2);
      if (m_calibPattern.find("3",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,7);
      if (m_calibPattern.find("4",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,6);
      if (m_calibPattern.find("5",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,67);
      if (m_calibPattern.find("6",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,66);
      if (m_calibPattern.find("7",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,71);
      if (m_calibPattern.find("8",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,70);
      
      const std::vector<HWIdentifier>& calibLine = m_larCablingSvc->calibSlotLine(theConsidChannel);
      ATH_MSG_INFO ( "Considered Channel " << m_onlineHelper->show_to_string(theConsidChannel) );
      if (calibLine.size() != 0) {
	m_CalibLineHW.push_back(calibLine[0]);
	ATH_MSG_INFO ( "Calib Line " << m_onlineHelper->show_to_string(calibLine[0]) );
      }
    }
    
    // BackSingle pattern
    if ( m_calibPattern.find("BackSingle",0) != std::string::npos){
      HWIdentifier theConsidChannel;
      int pulsedSlot = 9;
      if (m_calibPattern.find("1",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,3);
      if (m_calibPattern.find("2",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,2);
      if (m_calibPattern.find("3",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,7);
      if (m_calibPattern.find("4",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,6);
      
      const std::vector<HWIdentifier>& calibLine = m_larCablingSvc->calibSlotLine(theConsidChannel);
      ATH_MSG_INFO ( "Considered Channel " << m_onlineHelper->show_to_string(theConsidChannel) );
      if (calibLine.size() != 0) {
	m_CalibLineHW.push_back(calibLine[0]);
	ATH_MSG_INFO ( "Calib Line " << m_onlineHelper->show_to_string(calibLine[0]) );
      }
    }

  }				// End of barrel treatment


  //--------------------------------------------------
  //	EM ENDCAP


  else{

    // StripSingle pattern  
    if (m_isInnerWheel && m_isSpecialCrate && m_calibPattern.find("StripSingle",0) != std::string::npos )
      ATH_MSG_WARNING ( "There is no StripSingle pattern for EMEC inner wheel. You should try another pattern (MiddleSingle[1-4] or BackSingle[1-4])." );
    else if( m_calibPattern.find("StripSingle",0) != std::string::npos ){
      HWIdentifier theConsidChannel;

      int pulsedSlot = 2;			// EMEC outer wheel standard crates
      if (m_isSpecialCrate) pulsedSlot = 5;	// EMEC outer wheel special crates
      
      if (m_calibPattern.find("1",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,1);
      if (m_calibPattern.find("2",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,2);
      if (m_calibPattern.find("3",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,3);
      if (m_calibPattern.find("4",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,4);

      const std::vector<HWIdentifier>& calibLine = m_larCablingSvc->calibSlotLine(theConsidChannel);
      
      ATH_MSG_INFO ( "Considered Online Channel " << m_onlineHelper->show_to_string(theConsidChannel) );
      ATH_MSG_DEBUG ( "Number of associated calib line " << calibLine.size() );
      if (calibLine.size() != 0) {
	m_CalibLineHW.push_back(calibLine[0]);
	ATH_MSG_INFO ( "Calib line" << m_onlineHelper->show_to_string(calibLine[0]) );
      }
    }

    // MiddleSingle pattern
    if( m_calibPattern.find("MiddleSingle",0) != std::string::npos ){
      HWIdentifier theConsidChannel;

      int pulsedSlot = 10;			// EMEC outer wheel (standard and special crates)
      int pulsedCell1 = 1;
      int pulsedCell2 = 2;
      int pulsedCell3 = 7;
      int pulsedCell4 = 6;
      if (m_isInnerWheel && m_isSpecialCrate){	// EMEC inner wheel
	pulsedSlot = 1;
	pulsedCell1 = 1;
	pulsedCell2 = 0;
	pulsedCell3 = 2;
	pulsedCell4 = 3;
      }

      if (m_calibPattern.find("1",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell1);
      if (m_calibPattern.find("2",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell2);
      if (m_calibPattern.find("3",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell3);
      if (m_calibPattern.find("4",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell4);
      
      const std::vector<HWIdentifier>& calibLine = m_larCablingSvc->calibSlotLine(theConsidChannel);
      
      ATH_MSG_INFO ( "Considered Channel " << m_onlineHelper->show_to_string(theConsidChannel) );
      ATH_MSG_DEBUG ( "Number of associated calib line " << calibLine.size() );
      if (calibLine.size() != 0) {
	m_CalibLineHW.push_back(calibLine[0]);
	ATH_MSG_INFO ( "Calib Line " << m_onlineHelper->show_to_string(calibLine[0]) );
      }
    }// End of MiddleSingle pattern

    // BackSingle pattern
    if ( m_calibPattern.find("BackSingle",0) != std::string::npos){
      HWIdentifier theConsidChannel;

      int pulsedSlot = 8;			// EMEC standard crates
      int pulsedCell1 = 1;
      int pulsedCell2 = 2;
      int pulsedCell3 = 33;
      int pulsedCell4 = 34;
      if (m_isInnerWheel && m_isSpecialCrate){	// EMEC inner wheel
	pulsedSlot = 2;
	pulsedCell1 = 1;
	pulsedCell2 = 0;
	pulsedCell3 = 2;
	pulsedCell4 = 3;
      }
      else if (m_isSpecialCrate) {		// EMEC outer wheel special crates
        pulsedSlot = 9;
        pulsedCell1 = 2;
        pulsedCell2 = 1;
        pulsedCell3 = 17;
        pulsedCell4 = 18;
      }

      if (m_calibPattern.find("1",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell1);
      if (m_calibPattern.find("2",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell2);
      if (m_calibPattern.find("3",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell3);
      if (m_calibPattern.find("4",0) != std::string::npos )theConsidChannel = m_onlineHelper->channel_Id(m_barrelEndcap,m_posOrNeg,m_feedthroughNumber,pulsedSlot,pulsedCell4);
      
      const std::vector<HWIdentifier>& calibLine = m_larCablingSvc->calibSlotLine(theConsidChannel);
      ATH_MSG_INFO ( "Considered Channel " << m_onlineHelper->show_to_string(theConsidChannel) );
      if (calibLine.size() != 0) {
	m_CalibLineHW.push_back(calibLine[0]);
	ATH_MSG_INFO ( "Calib Line " << m_onlineHelper->show_to_string(calibLine[0]) );
      }
    }// End of BackSingle pattern	  

  };// End of endcap treatment

  return StatusCode::SUCCESS;
}

//=========================================================================================================
StatusCode LArCaliWaveBuilderXtalk::execute()
//=========================================================================================================
{
  if (m_event_counter==0) initializeCabling();

  // Print progression
  if ( m_event_counter < 100 || ( m_event_counter < 1000 && m_event_counter%100==0 ) || m_event_counter%1000==0 ) 
    ATH_MSG_INFO ( "Processing event " << m_event_counter );
  m_event_counter++ ;
 
  if (m_keylist.size()==0) {
    ATH_MSG_ERROR ( "Key list is empty! No containers to process!" );
    return StatusCode::FAILURE;
  } 

  const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;
 
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();

  for (;key_it!=key_it_e; ++key_it) { //Loop over all containers that are to be processed (e.g. different gains)
 
    StatusCode sc = evtStore()->retrieve(larAccumulatedCalibDigitContainer,*key_it);
    if (sc.isFailure()) {
      ATH_MSG_WARNING ( "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it );
      continue; // Try next container
    }

    LArAccumulatedCalibDigitContainer::const_iterator it=larAccumulatedCalibDigitContainer->begin();
    LArAccumulatedCalibDigitContainer::const_iterator it_end=larAccumulatedCalibDigitContainer->end();

    if (it == it_end) {
      ATH_MSG_DEBUG ( "LArAccumulatedCalibDigitContainer with key=" << *key_it << " is empty " );
      continue; // at this event LArAccumulatedCalibDigitContainer is empty, do not even try to loop on it...
    }
   
    const float delayScale = larAccumulatedCalibDigitContainer->getDelayScale();
    const float deltaDelay = 25*ns/(delayScale*m_NStep);

    // Check whether this "event" (LArAccumulatedDigit) is relevant for our studies
    bool relevantForXtalk = false;

    // for print purpose
    std::vector<int> nbOfEventSlot;		// Number of 'events' per slot
    nbOfEventSlot.resize(15);

    std::vector<int> nbOfEventPart;		// Number of 'events' per Subdetector/layer
    nbOfEventPart.resize(8);
    int sideA=0;				// Number of 'events' in A side
    int sideC=0;				// Number of 'events' in C side


    // Loop over all cells as long as no interesting pulsed cell is found
    do{

      const std::vector<HWIdentifier>& calibLine = m_larCablingSvc->calibSlotLine((*it)->hardwareID());
      if (calibLine.size() != 0) {
	std::vector<HWIdentifier>::iterator found=find(m_CalibLineHW.begin(),m_CalibLineHW.end(),calibLine[0]);

	if (((*it)->isPulsed()) && (found != m_CalibLineHW.end())) 
	  relevantForXtalk = true;

//  	if (m_onlineHelper->slot((*it)->hardwareID()) == 2 && m_onlineHelper->channel((*it)->hardwareID())<10)
// 	  log << MSG::DEBUG << "Relevant ? HW id ->" << m_onlineHelper->show_to_string((*it)->hardwareID()) << "Calib -> " << m_onlineHelper->show_to_string(calibLine[0]) << "Pulsed ?" << (*it)->isPulsed() << endmsg;

      } 

      if ((*it)->isPulsed()){
	int iSlot = m_onlineHelper->slot((*it)->hardwareID());
	int ibarrel_ec = m_onlineHelper->barrel_ec((*it)->hardwareID());
	int ipos_neg = m_onlineHelper->pos_neg((*it)->hardwareID());

	nbOfEventSlot[iSlot-1]++;

	if (ipos_neg==1) sideA++;
	else		 sideC++;

	if (!m_onlineHelper->isEmEndcapSpecialOnline((*it)->hardwareID())){ // do not count EMEC Special entries
	  if	(iSlot==1)			nbOfEventPart[4*ibarrel_ec]++;		// PS
	  else if (iSlot <  9 - ibarrel_ec)	nbOfEventPart[4*ibarrel_ec + 1]++;	// Front
	  else if (iSlot > 10 - ibarrel_ec)	nbOfEventPart[4*ibarrel_ec + 2]++;	// Middle
	  else					nbOfEventPart[4*ibarrel_ec + 3]++;	// Back
	}
      }

      ++it;
    }
    while ((!relevantForXtalk) && (it != it_end));// End of loop on cell to determine whether this event is relevant for xtalk study

    // Debug : print number of events per slot
    /*
      log << MSG::DEBUG << "[Slot-Nevents]: ";
      for (int jj=0; jj<15; jj++){
      log << MSG::DEBUG << "[" << jj << "-" << nbOfEventSlot[jj] << "] ";
      }
      log << MSG::DEBUG << " - Shall I keep this? " << relevantForXtalk << endmsg;
    */

    // Debug : print number of events per detector region
    msg() << MSG::DEBUG << "Event nb " << m_event_counter - 1<< " - " ;
    msg() << MSG::DEBUG << "Entries : EMB PS/F/M/B=" << nbOfEventPart[0] << "/" << nbOfEventPart[1] << "/" << nbOfEventPart[2] << "/" << nbOfEventPart[3];
    msg() << MSG::DEBUG << "  -  EMEC Std PS/F/M/B=" << nbOfEventPart[4] << "/" << nbOfEventPart[5] << "/" << nbOfEventPart[6] << "/" << nbOfEventPart[7];
    msg() << MSG::DEBUG << " - A/(A+C)=" << (sideA+sideC==0 ? -1 : float(sideA)/float(sideA+sideC));
    if (!relevantForXtalk) 
      msg() << MSG::DEBUG << " - don't keep (partition/pattern)" << endmsg;
    else 
      msg() << MSG::DEBUG << " - KEEP THIS EVENT" << endmsg;


    // Process relevant events
    if (relevantForXtalk){
      bool hasbeenprinted = false;
      for (it=larAccumulatedCalibDigitContainer->begin();it!=it_end; ++it) { // Loop over all cells
     
	HWIdentifier chid=(*it)->hardwareID(); 

	// FT selection for EMEC
        if (m_barrelEndcap==1) {
	  int iFT = m_onlineHelper->feedthrough(chid);
	  if (m_isSpecialCrate)
	    if (m_isInnerWheel)	{if (iFT!=3 && iFT!=10 && iFT!=16 && iFT!=22) continue;}
	    else		{if (iFT!=2 && iFT!=9  && iFT!=15 && iFT!=21) continue;}
	  else if (iFT==2 || iFT==9 || iFT==15 || iFT==21 || iFT==3 || iFT==10 || iFT==16 || iFT==22 || iFT==6)
	    continue;
	}

	// Process only channels that are in the studied partition
	if ( (m_barrelEndcap == m_onlineHelper->barrel_ec(chid)) && (m_posOrNeg == m_onlineHelper->pos_neg(chid)) ){ 

	  if (!hasbeenprinted){
	    ATH_MSG_DEBUG ( "This event is relevant for the studied partition" );
	    hasbeenprinted = true;
	  }

	  CaloGain::CaloGain gain=(*it)->gain();
	 
	  if (gain<0 || gain>CaloGain::LARNGAIN) {
	    ATH_MSG_ERROR ( "Found not-matching gain number ("<< (int)gain <<")" );
	    return StatusCode::FAILURE;
	  }
	 
	  // transform sampleSum vector from uint32_t to double
	  // Allow to cut waves :  consider only first samples to reduce needed memory (0 = no cut)
	  std::vector<double> samplesum;     
	  std::vector < uint32_t >::const_iterator samplesum_it=(*it)->sampleSum().begin();
	  std::vector < uint32_t >::const_iterator samplesum_it_e=(*it)->sampleSum().end();
	  int nSample=m_cutOnSample;
	  do {
	    samplesum.push_back((double)(*samplesum_it));     
	    samplesum_it++;
	    nSample--;
	  }
	  while ( (samplesum_it!=samplesum_it_e) && (nSample!=0) );

	  // transform sample2Sum vector from uint32_t to double
	  // Allow to cut waves :  consider only first samples to reduce needed memory (0 = no cut)
	  std::vector<double> sample2sum;     
	  std::vector < uint32_t >::const_iterator sample2sum_it=(*it)->sample2Sum().begin();
	  std::vector < uint32_t >::const_iterator sample2sum_it_e=(*it)->sample2Sum().end();
	  nSample=m_cutOnSample;
	  do {
	    sample2sum.push_back((double)(*sample2sum_it));     
	    sample2sum_it++;
	    nSample--;
	  }
	  while ( (sample2sum_it!=sample2sum_it_e) && (nSample!=0) );
	 
	  WaveMap& waveMap = m_waves.get(chid,gain);
	  WaveMap::iterator itm = waveMap.find((*it)->DAC());

	  if ( itm == waveMap.end() ) { // A new LArCaliWave is booked
	    LArCaliWave wave(samplesum.size()*m_NStep, m_dt, (*it)->DAC());
	    wave.setFlag( LArWave::meas ) ;

	    // Be careful! Don't call addAccumulatedEvent() twice!!!
	    // cf. LArCaliwaveBuilder ref. 1.38
	    //wave.addAccumulatedEvent((int)roundf((*it)->delay()/deltaDelay), m_NStep, 
	    //			     samplesum, sample2sum, (*it)->nTriggers());

	    itm = (waveMap.insert(WaveMap::value_type((*it)->DAC(), wave))).first;
	  }//else { // A LArCaliWave for this channel/DAC is already existing

	  (*itm).second.addAccumulatedEvent((int)roundf((*it)->delay()/deltaDelay), m_NStep, 
					    samplesum, sample2sum, (*it)->nTriggers());
	} // End of test on studied partition
      } //End loop over all cells
    } // End of relevantForXtalk test
  } //End loop over all containers
   
  return StatusCode::SUCCESS;
}

//=========================================================================================================
StatusCode LArCaliWaveBuilderXtalk::stop()
//=========================================================================================================
{
  // Create wave container using feedthru grouping and initialize
  LArCaliWaveContainer* caliWaveContainer = new LArCaliWaveContainer();
  
  ATH_CHECK( caliWaveContainer->setGroupingType(m_groupingType,msg()) );
  ATH_MSG_DEBUG ( "Set groupingType for LArCaliWaveContainer object" );

  ATH_CHECK( caliWaveContainer->initialize() );
  ATH_MSG_DEBUG ( "Initialized LArCaliWaveContainer object" );

  // Note : sometimes a Segmentation fault happen at this stage
  // not yet solved
  // 
  // Extract of the thread :
  //  > Caught signal 11(Segmentation fault). Details: 
  //  >   errno = 0, code = 1 (address not mapped to object)
  //  > #10 <signal handler called>                                                                                                                                
  //  > #11 cpp_alloc (size=52, nothrow=false) at src/tcmalloc.cc:354                                                                                              
  //  > #12 0xf7fd7a69 in operator new (size=52) at src/tcmalloc.cc:3104                                                                                           
  //  > #13 0xf775a306 in std::vector<unsigned int, std::allocator<unsigned int> >::operator= ()                                                                   
  //  > #14 0xee8b63d7 in LArCondFEBIdChanMap::addFEBIdVector ()                                                                                                   
  //  > #15 0xee8b03c4 in LArConditionsContainerBase::initGrouping ()                                                                                              
  //  > #16 0xee8b2c82 in LArConditionsContainerBase::initializeBase ()                                                                                            
  //  > #17 0xeed5f872 in LArConditionsContainer<LArCaliWaveVec>::initialize ()                                                                                    


  LArWaveHelper wHelper;
  int NCaliWave=0;

  // Loop over all gains
  for (unsigned k=0;k<(int)CaloGain::LARNGAIN;k++) {
   
    CaloGain::CaloGain gain=(CaloGain::CaloGain)k;

    /// get iterator for all channels for a gain
    WaveContainer::ConstConditionsMapIterator cell_it   = m_waves.begin(gain); 
    WaveContainer::ConstConditionsMapIterator cell_it_e = m_waves.end(gain);

    HWIdentifier lastId;
	
    for (; cell_it!=cell_it_e; ++cell_it) {
   
      // Get id of this cell - use id from first cell in map
      //
      //   Some accumulations may be empty and we must skip this
      //   cell. WaveContainer has all 128 channels for each FEB
      //   with at least ONE being read out.
      //

      const HWIdentifier hwId = cell_it.channelId();
      if (!m_larCablingSvc->isOnlineConnected(hwId)) continue; //Ignore disconnected channels
	      
      const WaveMap& waveMap = (*cell_it);
      if (waveMap.size()==0) {
	ATH_MSG_INFO ( "Empty accumulated wave. Last id: " << MSG::hex 
                       << lastId << MSG::dec );
	continue;
      }

      lastId = hwId; // save previous id for debug printout

      // FT selection for EMEC
      if (m_barrelEndcap==1) {
	int iFT = m_onlineHelper->feedthrough(hwId);
	if (m_isSpecialCrate)
	  if (m_isInnerWheel)	{if (iFT!=3 && iFT!=10 && iFT!=16 && iFT!=22) continue;}
	  else			{if (iFT!=2 && iFT!=9  && iFT!=15 && iFT!=21) continue;}
	else if (iFT==2 || iFT==9 || iFT==15 || iFT==21 || iFT==3 || iFT==10 || iFT==16 || iFT==22 || iFT==6)
	  continue;
      }

      // Get the vector of waves for this chid,gain
      LArCaliWaveContainer::LArCaliWaves& dacWaves = caliWaveContainer->get(hwId, gain);

      std::map<int, LArCaliWave>::const_iterator dac_it   = cell_it->begin();
      std::map<int, LArCaliWave>::const_iterator dac_it_e = cell_it->end();
     
      for (; dac_it != dac_it_e; ++dac_it) {
       
	const LArCaliWave& thisWave = dac_it->second ;
	float pedAve = 0.;
       	   
	double waveMax = thisWave.getSample( wHelper.getMax(thisWave) ) ;
	if ( m_ADCsatur>0 && waveMax>m_ADCsatur ) { 
	  ATH_MSG_INFO ( "Absolute ADC saturation at DAC = " << thisWave.getDAC() << " ... skip!" );
	  continue ;
	} else {

	  LArCaliWave newWave( ((thisWave)+(-pedAve)).getWave() ,
			       thisWave.getErrors(),
			       thisWave.getTriggers(),
			       thisWave.getDt(), 
			       (thisWave.getDAC() + (thisWave.getIsPulsedInt()<<16)), 
			       thisWave.getFlag() );
       
	  dacWaves.push_back(newWave);
	  NCaliWave++;
	}
      } // end of loop DACs     

      // intermediate map cleanup (save memory)
      const WaveMap* cmap = &(*cell_it);
      WaveMap* map = const_cast<WaveMap*>(cmap);
      map->clear();

    } //end loop cells

  } //end loop over m_keyList
 
  ATH_MSG_INFO ( "Summary : Number of cells with a CaliWave  reconstructed : " << NCaliWave  );
  ATH_MSG_INFO ( "Size of LAr calibration waves container : " << caliWaveContainer->size()  );
 
  // Record in detector store with key (m_keyoutput)
  ATH_CHECK( detStore()->record(caliWaveContainer, m_keyoutput) );

  ATH_MSG_INFO ( "LArCaliWaveBuilderXtalk has finished." );
  return StatusCode::SUCCESS;
}
