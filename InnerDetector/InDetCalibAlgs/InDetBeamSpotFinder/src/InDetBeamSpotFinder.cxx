/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotFinder.h"
#include <map>
#include "InDetBeamSpotFinder/IInDetBeamSpotTool.h"
#include "InDetBeamSpotVertex.h"
#include "InDetBeamSpotRooFit.h"
#include "GaudiKernel/ITHistSvc.h"
#include "VxVertex/VxCandidate.h"
#include "BeamSpotID.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

InDet::InDetBeamSpotFinder::InDetBeamSpotFinder(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_toolSvc("ToolSvc",name),
  m_bcTool( "Trig::TrigConfBunchCrossingTool/BunchCrossingTool" ),
  m_root_vtxName("Vertices"),m_bcid(0),m_useForcedRun(false)
{
  declareProperty("ToolSvc", m_toolSvc);
  declareProperty("BeamSpotToolList",m_beamSpotToolList);
  declareProperty( "BCTool", m_bcTool );

  declareProperty("MaxCount",m_maxCount = -1);
  declareProperty("LumiRange",m_lumiRange = 3);
  declareProperty("RunRange",m_runRange   = 1);
  declareProperty("EventRange",m_eventRange = -1);
  declareProperty("WriteDb",m_writeDb = true);
  declareProperty("NearestBeamSpot", m_useNearestBS = false);
  declareProperty("UseLumiBlocks",m_useLumiBlocks = false);
  declareProperty("LumiBlockRanges",m_lumiBlockRanges);
  declareProperty("SkipEvents",m_skipNevents=-1); // skip every N number of events

  declareProperty("DoHists",m_doHists=true);
  declareProperty("BeamSpotRootName",m_root_beamspotName = "BeamSpotNt");
   
  //if using method (such as Track Chi2, that returns 0 for beam sigmas, etc)
  // should default values be inserted. Use with care, as this is guessing!
  declareProperty("UseDefaultValues",m_useDefaults = false);
  declareProperty("Default_PosX",m_def_posX = 0.0);
  declareProperty("Default_PosY",m_def_posY = 0.0);
  declareProperty("Default_PosZ",m_def_posZ = 0.0);
  declareProperty("Default_TiltX",m_def_tiltX = 0.0);
  declareProperty("Default_TiltY",m_def_tiltY = 0.0);
  declareProperty("Default_SigmaX",m_def_sigmaX = 0.1);
  declareProperty("Default_SigmaY",m_def_sigmaY = 0.1);
  declareProperty("Default_SigmaZ",m_def_sigmaZ = 60);
  declareProperty("Default_SigmaXY",m_def_sigmaXY = 0.);  

  declareProperty("ForceRunNumber",m_forcedRunNumber = 0);
  declareProperty("ForceRunRange", m_forcedRunRange = 1);

  declareProperty("LBRangeOffset",m_lbRangeOffset=1);

  declareProperty("UseBCID",m_bcid_val);

  declareProperty("UseLBFromAccepted",m_setLBwithAcceptedEvents=false);
  declareProperty("UseLBFromViewed",  m_setLBwithViewedEvents  =false);

  declareProperty("SeparateByBCID",m_separateByBCID=false);
  declareProperty("UseFilledBCIDsOnly",m_useFilledBCIDsOnly=true);

  //declareProperty("VertexContainer",m_containerName = "VxPrimaryCandidate");
  declareProperty("VertexContainer",m_containerName = "PrimaryVertices");
  // selection criteria
  declareProperty( "MinTracksPerVtx", m_minTrackNum = 5);
  declareProperty( "MaxTracksPerVtx", m_maxTrackNum = 10000000);
  declareProperty( "MinVtxNum"   ,  m_minVertexNum = 100);
  declareProperty( "MaxVtxChi2" ,   m_maxChi2Vertex = 10);
  declareProperty( "VertexTypes",  m_vertexTypeNames);
  declareProperty( "MaxTransverseErr", m_maxTransverseErr = 1000000);
  declareProperty( "MinVertexProb", m_minVtxProb=0.001); // min prob(chi2,ndf)
  declareProperty( "VertexNtuple", m_VertexNtuple );
  declareProperty( "WriteAllVertices",m_writeAllVertices=false);
}

StatusCode InDet::InDetBeamSpotFinder::initialize() {
  msg(MSG::DEBUG) << "in initialize()" << endreq;
  if ( m_runRange ==0 ) {
    msg(MSG::FATAL) <<"Can't assign a run range 0" << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_toolSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_toolSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved service " << m_toolSvc << endreq;
   
  for ( unsigned int i = 0; i < m_beamSpotToolList.size(); i++){
    if ( m_beamSpotToolList[i].retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Cannot retrieve Beamspot Tool" << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::DEBUG) << "Retreived BeamSpot Tool "
		      << m_beamSpotToolList[i]<< endreq;
    }
  }

  if (m_useFilledBCIDsOnly) {
    if (m_bcTool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve Bunch Crossing Tool" << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::DEBUG) << "Retreived Bunch Crossing Tool: "
		      << m_bcTool<< endreq;
    }
  }
 
  // Setup the histogramming
  if (m_doHists) {
    if ((service("THistSvc", m_thistSvc)).isFailure()){
      msg(MSG::FATAL) << "THistSvc service not found" << endreq;
      return StatusCode::FAILURE;
    }
    
    setupBeamspotTree();
    if (m_VertexNtuple){
      setupVertexTree();
    }
    convertVtxTypeNames();

    if (m_forcedRunNumber !=0 ) {
      msg(MSG::WARNING) << "Forcing run number to value " << m_forcedRunNumber << endreq;     
      m_useForcedRun = true;
    }
    m_eventCount = 0;
  
    if (m_useLumiBlocks || m_lumiBlockRanges.size() != 0) {
      m_useLumiBlocks = true;
      msg(MSG::INFO) << "Beamspot range determined from specified LumiBlock ranges" << endreq;
      if ( m_lumiBlockRanges.size() == 0) {
	msg(MSG::WARNING) << "JobOption \"LumiBlockRanges\" contians no sets of ranges." << endreq;
	msg(MSG::WARNING) << "Reverting to LumiRange = : " <<m_lumiRange << endreq;
	m_useLumiBlocks = false;
      }
      for (std::vector< std::pair< int, int> >::const_iterator it = m_lumiBlockRanges.begin(); 
	   it != m_lumiBlockRanges.end(); ++it ) {
	msg(MSG::INFO) << "Range: [ " <<  it->first << ", " << it->second << " ]" << endreq;
      }
    } // if using specified lumiblock ranges
    if ( m_lumiRange ==0 ) {
      msg(MSG::FATAL) <<"Can't assign a luminosty range 0" << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "================ InDetBeamSpotFinder ================" << endreq;
    msg(MSG::INFO) << "LumiBlockRanges: " << m_useLumiBlocks << ". #: " << (int)m_lumiBlockRanges.size() << endreq;
    msg(MSG::INFO) << "EventRange: " << m_eventRange <<endreq;
    msg(MSG::INFO) << "LumiRange:  " << m_lumiRange << endreq;
    msg(MSG::INFO) << "RunRange:   " << m_runRange  << endreq;
    msg(MSG::INFO) << "ForcedRunNumber: " << m_forcedRunNumber << endreq;
    msg(MSG::INFO) << "MaxCount:   " << m_maxCount << endreq;
    msg(MSG::INFO) << "SkipEvents: " << m_skipNevents << endreq; 
    msg(MSG::INFO) << "Seperate BCID: " << m_separateByBCID << endreq; 
    msg(MSG::INFO) << "BCID used : " ;
    if (m_bcid_val.size() == 0)  msg(MSG::INFO) << " All. " << endreq;
    else {
      for ( std::vector<unsigned int>::const_iterator it = m_bcid_val.begin(); it != m_bcid_val.end(); ++it) {
	msg(MSG::INFO) << *it << ", ";
      }
      msg(MSG::INFO) << endreq;
    }
    msg(MSG::INFO) << "Fit tool(s) used: "<<endreq;
    for(unsigned int i =0 ; i < m_beamSpotToolList.size(); i++){
      msg(MSG::INFO) << m_beamSpotToolList[i] <<", ";
    }
    msg(MSG::INFO)<<endreq;
    msg(MSG::INFO) << "================ InDetBeamSpotFinder ================" << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotFinder::execute(){
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "in execute()" << endreq;
  if (m_skipNevents > 0 ) { // option to skip every N set of events
    static int skipCount(0);
    ++skipCount;
    if ( skipCount % (m_skipNevents+1) != 0 ) {
      if ( msgLvl(MSG::VERBOSE)) {
	msg(MSG::VERBOSE) << "Skip mode activated: skipping event" << endreq;
      }
      return StatusCode::SUCCESS;
    } else {
      skipCount=0; // reset the counter, just to make sure it can't get too big
      if (msgLvl(MSG::VERBOSE) ){
	msg(MSG::VERBOSE) << "Skip mode activated: processing event" << endreq;
      }
    } // else
  } // if skipping
  
  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve( eventInfo ) );
  unsigned int lb = eventInfo->lumiBlock();
  unsigned int run = eventInfo->runNumber();
  unsigned int bcid = eventInfo->bcid();

  // Skip empty BCIDs
  if(m_useFilledBCIDsOnly) {
    if (!m_bcTool->isFilled( bcid ) ) {
      if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "BCID "<< bcid << " is not filled: skipping..." << endreq;     
      return StatusCode::SUCCESS;      
    }
  }

  if (m_bcid_val.size()) {
    if ( find(m_bcid_val.begin(), m_bcid_val.end(), bcid) == m_bcid_val.end()) {
      // bcid not in list
      if (msgLvl(MSG::VERBOSE) ){
	msg(MSG::VERBOSE) << "BCID of event: "<< bcid << " not in list: skipping..." << endreq;
      }
      return StatusCode::SUCCESS;
    } else { // if not in list
      if (msgLvl(MSG::VERBOSE) ){
	msg(MSG::VERBOSE) << "BCID of event: "<< bcid << " found in list: " << endreq;
      }
    }
  }

  uint64_t idnum(0);

  unsigned int runStart(0), runR(0), lumiStart(0), lumiR(0);

  m_eventCount++;

  //simple options
  if ( m_lumiRange > 0) {
    //normal case - maintain run integrity
    idnum  = run;
    idnum  = idnum << 32;
    //    idnum += lb / m_lumiRange; 
    runStart = run;
    runR     = 1;
    //    lumiStart = ( (lb - m_lbRangeOffset) / m_lumiRange) * m_lumiRange  + m_lbRangeOffset ;
    //    lumiStart = ((lb-m_lbRangeOffset)/m_lumiRange) *m_lumiRange;
    //lumiStart = lb;
    //lumiStart = (((lb-m_lbRangeOffset)/m_lumiRange) *m_lumiRange) + m_lbRangeOffset;

    lumiStart = floor( ( lb-m_lbRangeOffset*1.  ) /m_lumiRange + 0.01 )*m_lumiRange + m_lbRangeOffset  ;
    idnum += floor(( lb-m_lbRangeOffset*1.  ) /m_lumiRange + 0.01) + m_lbRangeOffset;

    if (lb==0) lumiStart =0;
    lumiR   = m_lumiRange;
  } else if ( m_runRange > 0) {
    //span multiple runs, include all events in specified runs
    idnum  = run / m_runRange;
    idnum  = idnum << 32;
    runStart = (run / m_runRange) * m_runRange;
    runR   = m_runRange;
    lumiStart = 0;
    lumiR   = 0 -1; 
  } else {
    //everything will be included
    runStart = 0;
    runR   = 1;
    lumiStart = 0;
    lumiR   = 0;
    idnum = 0;
  }
  
  if (m_useForcedRun) {
    //override run number values
    run= m_forcedRunNumber;
    runStart = run;
    runR = m_forcedRunRange;
    if ( m_lumiRange > 0) {
      idnum  = run;
      idnum  = idnum << 32;
      idnum += lb / m_lumiRange;
    }else {
      idnum = run;
    }
  }

  if (m_eventRange > 0){
    static int lumiBlockStart(0);
    static int previousIDNum(0);
    if ( lumiBlockStart == 0 || (previousIDNum != m_eventCount/m_eventRange)) {
      // implies starting a new beamspot, store and hold onto the lumiblock number
      lumiBlockStart = m_eventCount/m_eventRange;
      //      lumiBlockStart = lb;
    }

    //overrides run and lb values
    runStart = 0;
    runR   = 1;
    lumiStart = lumiBlockStart;
    lumiR   = ~0 - lumiStart; 
    idnum = m_eventCount/m_eventRange;
    previousIDNum = idnum;
  }
 if (m_eventRange > 0){
    //overrides run and lb values
    runStart = 0;
    runR   = 1;
    lumiStart = 0;
    lumiR   = 1; 
    idnum = m_eventCount/m_eventRange;
  }

 if (m_useLumiBlocks) {
   // use user-defined ranges of lumi-blocks.
   idnum  = run;
   idnum  = idnum << 32;
   unsigned int startLB(~0);
   unsigned int range(0);
   for (std::vector< std::pair< int, int> >::const_iterator it = m_lumiBlockRanges.begin(); 
	it != m_lumiBlockRanges.end(); ++it ) {
     // TBD: update to make quicker at some point
     if ( (int)lb >= it->first && (int)lb <= it->second) {
       if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Event run/lb: " << run <<" / " << lb << " in Beamspot LBrange: [ " 
						   << it->first << " , " << it->second << " ]." << endreq;
       startLB = it->first;
       range = (int) (it->second - it->first +1);
       break;
     }
   }// for loop
   runStart  = run;
   runR      = 1;
   lumiStart = startLB;
   lumiR     = range;
   idnum    += lumiStart; 
 } // lumiblocks 

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "From event determine the ID number as: " << endreq;
    msg(MSG::VERBOSE) << "ID: " << idnum << ", split as runID: " << (idnum >> 32)
	  << ", and identID: " << ( (idnum <<32) >>32) << endreq;
  }
  //Get the correct beamspot tool for the given id.
  //Set the run min and run max to [runBegin, runEnd]
  //and the lumiRange to [lumiMin,lumiMax)
  //set the bcid if m_separateByBCID is true to distinguish results
  std::vector<BeamSpot::ID> ids;
  ids.resize(m_beamSpotToolList.size());
  for(unsigned int i = 0; i < ids.size(); i ++){
    ids[i].run(runStart);
    ids[i].id(idnum);
    ids[i].lumiStart(lumiStart);
    ids[i].lumiEnd(lumiStart + lumiR);
    ids[i].setRunRange(runR);
    ids[i].bcid( (m_separateByBCID ? bcid: 0) );
    ids[i].setBSTool(i);
  }
  //const VxContainer* importedVxContainer =0;
  const xAOD::VertexContainer* importedVxContainer = 0;

  //get PV vertex container - can this go in initialise?

  ATH_CHECK( evtStore()->retrieve( importedVxContainer, m_containerName) );
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "There are " << importedVxContainer->size() 
					      << " vertices in "<< m_containerName << endreq;

  bool eventUsed(false);
  //get list of PVs

  //VxContainer::const_iterator vtxItr;
  xAOD::VertexContainer::const_iterator vtxItr;

  for(unsigned int i = 0; i < ids.size(); i++){
    
    if( m_nEvents.find(ids[i]) == m_nEvents.end() ) m_nEvents[ids[i]] = 0;
    m_nEvents[ids[i]]++;
  }
  for(vtxItr=importedVxContainer->begin(); 
    vtxItr!=importedVxContainer->end(); ++vtxItr) {
    if ((*vtxItr)->vertexType() == xAOD::VxType::NoVtx) continue; // ignore dummy vertex

    for(unsigned int i = 0; i < ids.size(); i++){
      if ( m_vertexCountAll.find( ids[i] ) == m_vertexCountAll.end() ) m_vertexCountAll[ids[i]] = 0;
      if ( m_vertexCountPrim.find( ids[i] ) == m_vertexCountPrim.end() ) m_vertexCountPrim[ids[i]] = 0;
      if ( *vtxItr == NULL ) continue;

      ++m_vertexCountAll[ ids[i] ];
      if((*vtxItr)->vertexType() == xAOD::VxType::PriVtx) ++m_vertexCountPrim[ ids[i] ];
    }

    if ( applySelection(*vtxItr) ) {
      for(unsigned int i = 0; i < ids.size(); i++){
	if ( addVertex( *vtxItr, ids[i] ) ) { 
	  eventUsed = true;
	  m_root_par.passed = true;
	}
      }
    }
    else m_root_par.passed = false;
    
    addToVertexTree(*vtxItr, *eventInfo );


    if (m_VertexNtuple && (m_root_par.passed || m_writeAllVertices)) {
      m_root_vrt->Fill();
    }
  }

  for( unsigned int i = 0; i < m_beamSpotToolList.size(); i ++){
    IInDetBeamSpotTool * bs(0);
    std::pair< std::map<BeamSpot::ID, IInDetBeamSpotTool *>::iterator, bool> ret 
      = m_beamspots.insert(std::pair<BeamSpot::ID,IInDetBeamSpotTool*>(ids[i],bs));

    //insert returns a pair, with pair::first an interator pointing to either
    //the newly inserted element or the element that already had an equivalent key
    //pair::second = (new element inserted ? true : false)
    
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "BS id " << ids[i] << ". BS " <<bs << endreq;
    
    //If a new element is inserted into the beamspots map (an element with that key didn't exist before)
    //Create a new beamspot object
    if (ret.second == true) {
      msg(MSG::INFO)<<"Creating new beamspot with Tool number "<<ids[i].getBSTool()<<endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Creating new beamspot" << endreq;
      bs = cloneTool(i);
      if (!bs) {
	if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not create beamspot clone" 
						<< endreq;
	return StatusCode::FAILURE;
      }
      //  m_beamspots[id] = bs; //NB This Algorithm 'owns' the beamspot. Must delete later.
      ret.first->second = bs;
    } 

    //if an element with that key already existed, just set bs equal to that tool
    else {
      bs = ret.first->second;
    }

    addExtraBranches(bs);
  
    
    // 'merge' additional info to the ID, such as the last event number
    unsigned int evt = eventInfo->eventNumber();
    unsigned int time = eventInfo->timeStamp();
    BeamSpot::ID * myId = const_cast<BeamSpot::ID *>(&ret.first->first);
    if (myId->firstEvent() == 0){
      myId->firstEvent(evt);
      myId->firstTimeStamp(time);
      myId->timeStampOffset(eventInfo->timeStampNSOffset());
      myId->firstLB(lb);
    }
    
    if ( evt > myId->lastEvent() ) {
      myId->lastEvent(evt);
      myId->lastTimeStamp(time);
      myId->lastLB(lb);
    }
    if ( evt < myId->firstEvent() ) {
      myId->firstEvent(evt);
      myId->lastTimeStamp(time);
      myId->timeStampOffset(eventInfo->timeStampNSOffset());
      myId->firstLB(lb);
    }
 
    // set the events used lb range correctly
    if ( eventUsed ) { 
      if (myId->firstAcceptedLB() == 0) {
	myId->firstAcceptedLB(lb);
	myId->lastAcceptedLB(lb);
      }
      if ( lb < myId->firstAcceptedLB())
	myId->firstAcceptedLB(lb);
      if ( lb > myId->lastAcceptedLB())
	myId->lastAcceptedLB(lb);
    }// event used
  }
  return StatusCode::SUCCESS;
}


//Here we determine the needed parameters and cov matrix elements from the tools being used
void InDet::InDetBeamSpotFinder::addExtraBranches(const IInDetBeamSpotTool * tool){
  std::map<std::string,double> paramMap = tool->getParamMap();
  std::map<std::string,double> covMap   = tool->getCovMap();

  std::string slashD = "/D";
  std::string keySlashD; 
  
  //Loop over the parameters for a given fit tool, and create a branch for each if it doesn't exist
  for( std::map<std::string,double>::iterator iter = paramMap.begin(); iter != paramMap.end(); ++iter){
    std::string key =  iter->first;
    //double val =  iter->second;
    if( !(m_root_bs->GetBranch(key.c_str())) ){
      m_beamSpotNtuple.paramMap[key] = 0;
      keySlashD = key + slashD;
      m_root_bs->Branch(key.c_str(), &m_beamSpotNtuple.paramMap[key], keySlashD.c_str());
    }
  }
  //Loop over the covariance matrix for a given fit tool and create a branch for each element, if it doesn't already exist.
  for( std::map<std::string,double>::iterator iter = covMap.begin(); iter != covMap.end(); ++iter){
    std::string key =  iter->first;
    //double val  =  iter->second;
    if( !(m_root_bs->GetBranch(key.c_str())) ){
      m_beamSpotNtuple.covMap[key] = 0;
      keySlashD = key + slashD;
      m_root_bs->Branch( key.c_str(), &m_beamSpotNtuple.covMap[key], keySlashD.c_str());
    }
  }
}

//Sets the values to be added to the TTree
void InDet::InDetBeamSpotFinder::setRootValues(const IInDetBeamSpotTool * tool) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Storing Beamspot info in root" << endreq;
  if ( !tool ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not Valid" << endreq;
    return;
  }
  std::map<std::string,double> paramMap = tool->getParamMap();
  std::map<std::string,double> covMap   = tool->getCovMap();

  //Loop over the parameter and covariance matrix maps and set the values
  //If that partcular tool doesn't use that paramter or cov matrix element, then set the value to zero

  for( std::map<std::string,double>::iterator iter = m_beamSpotNtuple.paramMap.begin(); 
       iter != m_beamSpotNtuple.paramMap.end(); ++iter){    
    std::string key = iter->first;
    iter->second = ( paramMap.find(key) == paramMap.end() ) ? 0 : paramMap[key];
  }

  for( std::map<std::string,double>::iterator iter = m_beamSpotNtuple.covMap.begin(); 
       iter != m_beamSpotNtuple.covMap.end(); ++iter){    
    std::string key = iter->first;
    iter->second = ( covMap.find(key) == covMap.end() ) ? 0 : covMap[key];
  }

  //These values are not determined now, and shall be set to zero.
  m_beamSpotNtuple.defectWord = 0;
  m_beamSpotNtuple.fill = 0;
  m_beamSpotNtuple.separation = 0;
  m_beamSpotNtuple.timeEnd = 0;
  m_beamSpotNtuple.timeStart = 0;

}

StatusCode InDet::InDetBeamSpotFinder::finalize() {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in finalize()" << endreq;
  //std::map<BeamSpot::ID, BeamSpot::BeamSpotDB > dbSpots;
  msg(MSG::INFO) << m_beamspots.size() << " fits will be performed."<<endreq;
  
  for (std::map<BeamSpot::ID, IInDetBeamSpotTool *>::iterator it =  m_beamspots.begin();
       it != m_beamspots.end(); ++it) {
    //unsigned int bcidKey = it->first.bcid();
    msg(MSG::INFO) << "Attempt solution of Beamspot " << it->first << endreq;
    msg(MSG::INFO) << "With: " << m_vertexDataMap[it->first].size() <<" vertices."<< endreq;
    msg(MSG::INFO) << "Beamspot for run range: ["<< it->first.run()
		   << ", " <<it->first.runEnd() <<  "]" << endreq;
    msg(MSG::INFO) << "Beamspot for lumi range: ["<< it->first.lumiStart()
		   << ", " <<it->first.lumiEnd() <<  ")" << endreq;
    if (m_separateByBCID) {
      msg(MSG::INFO) << "Beamspot with BCID: " << it->first.bcid() << endreq;
    }
    
    IInDetBeamSpotTool::FitStatus bsFitStatus;
    if(m_vertexDataMap[ it->first ].size() >= m_minVertexNum)
      bsFitStatus = it->second->fit(m_vertexDataMap[ it->first ]);

    else bsFitStatus = IInDetBeamSpotTool::failed;

    msg(MSG::INFO) << "Beamspot run/lb: " 
		   << "["<< it->first.run() << ", " <<it->first.runEnd() <<  "] / " 
		   << "["<< it->first.lumiStart() << ", " <<it->first.lumiEnd() <<  ") " ;
    if (m_separateByBCID) {
      msg(MSG::INFO) << ", BCID: " << it->first.bcid() << " ";
    }
    msg(MSG::INFO) << ( (bsFitStatus == IInDetBeamSpotTool::successful) ? " solved. " : " had problems. ") << "\n"<<endreq;
    
    printBeamspot( &(it->first), it->second);
    
    //set the conditions status
    //current design :
    /*
      bit 0-1:  fit status
      bit 2:    online (0) / offline (1)  beam spot
      bit 3-7:  algorithm number
      bit 8: width determination mode (0 => default, 1 => from fit)
      //fit status 
      0 = values are defaults (fit did not converge)
      1 = fit had problems, values are fit output
      2 = reserved for later use
      3 = fit converged, good beam spot	
      //algorithm number
      0 = default beam spot
      1 = track based algorithm with default width
      2 = track based algorithm with width from fit
      3 = vertex based algorithm, full
      4 = vertex based algorithm, chi2, default width
      6 = RooFit based maximum log likelihood with full beamspot parameters from fit
    */
    m_BeamStatusCode.clearWord();
    m_BeamStatusCode.setOnlineStatus( false );
    int fitStat;
    int onOffline(1);
    int algNumber;
    msg(MSG::DEBUG) << "Word Status setup: " << onOffline << "  " 
		    << it->second->getFitID() << "  " << bsFitStatus << endreq; 

    algNumber = it->second->getFitID();
    m_BeamStatusCode.setAlgType( algNumber );
    
    if ( bsFitStatus == IInDetBeamSpotTool::successful)
      fitStat = 3;
    else if ( bsFitStatus == IInDetBeamSpotTool::problems)
      fitStat = 1;
    else if ( bsFitStatus == IInDetBeamSpotTool::failed || 
	      bsFitStatus == IInDetBeamSpotTool::unsolved)
      fitStat = 0;	
    else 
      fitStat = 0;
    m_BeamStatusCode.setFitStatus(fitStat);
    
    //determine if the width was set from algorithm or a default value

    if (it->second->getParamMap()["sigmaX"] == 0 && it->second->getParamMap()["sigmaY"] ==0 ) {
      msg(MSG::DEBUG) << " Width set from Defaults" << endreq;
      m_BeamStatusCode.setFitWidth( false);
    } else {
      msg(MSG::DEBUG) << " Width set from fitted values" << endreq;
      m_BeamStatusCode.setFitWidth(true);
    } 
    msg(MSG::DEBUG) << " Status word: "<< m_BeamStatusCode.getWord() << ", Online: "
		    << m_BeamStatusCode.isOnline() << ", AlgNum: " << m_BeamStatusCode.algType() 
		    << ", fitStatus:  " 
		    << m_BeamStatusCode.fitStatus() << " FittedWidth: " << m_BeamStatusCode.fitWidth() << endreq;
    msg(MSG::DEBUG) << " Status: "<<getAlgorithmName(m_BeamStatusCode.algType())
		    << ",  " <<getFitStatusName(m_BeamStatusCode.fitStatus())
		    << (m_BeamStatusCode.isOnline()? ", Online":", Offline") <<", " 
		    << (m_BeamStatusCode.fitWidth()?" Fitted Width":" Default Width") << endreq;

    if (m_doHists) {
      //set root to look at the correct variables
      m_beamSpotNtuple.run = it->first.run();
      m_beamSpotNtuple.bcid = it->first.bcid();
      m_beamSpotNtuple.runEnd = it->first.runEnd();
      //m_beamSpotNtuple.Word = m_BeamStatusCode.getWord();

      if (m_setLBwithAcceptedEvents ) {
	m_beamSpotNtuple.lbStart = it->first.firstAcceptedLB();
	m_beamSpotNtuple.lbEnd   = it->first.lastAcceptedLB();
	
      } else if (m_setLBwithViewedEvents) {
	m_beamSpotNtuple.lbStart = it->first.firstLB();
	m_beamSpotNtuple.lbEnd   = it->first.lastLB();

      } else {
	m_beamSpotNtuple.lbStart = it->first.lumiStart();
	m_beamSpotNtuple.lbEnd   = it->first.lumiEnd();
      }

      m_beamSpotNtuple.status = m_BeamStatusCode.getWord();// bsFitStatus;
      m_beamSpotNtuple.nEvents = m_nEvents[it->first];
      m_beamSpotNtuple.nValid = m_vertexDataMap[it->first].size();
      m_beamSpotNtuple.nVtxAll = m_vertexCountAll[it->first];
      m_beamSpotNtuple.nVtxPrim = m_vertexCountPrim[it->first];


      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "run:  " << m_beamSpotNtuple.run << endreq;
	msg(MSG::DEBUG) << "bcid: " << m_beamSpotNtuple.bcid << endreq;
        msg(MSG::DEBUG) << "lbStart:  " << m_beamSpotNtuple.lbStart << endreq;
        msg(MSG::DEBUG) << "lbRange:  " << m_beamSpotNtuple.lbEnd - m_beamSpotNtuple.lbStart + 1 << endreq;
        msg(MSG::DEBUG) << "nVertices:  " << m_beamSpotNtuple.nEvents << endreq;
      }
      
      setRootValues(it->second);
      m_root_bs->Fill(); // fill the tree
    } //root store
    
    if (!(bsFitStatus == IInDetBeamSpotTool::successful)) {
      delete it->second;
      it->second = 0;
    }
  } //for loop 
  
  // delete all remaining beamspots
  for (std::map<BeamSpot::ID, IInDetBeamSpotTool *>::iterator it =  m_beamspots.begin();
       it != m_beamspots.end(); ++it) {
    if ((it->second)) {
      delete it->second;
      it->second = 0;
    }
  }
  
  return StatusCode::SUCCESS;
}

InDet::IInDetBeamSpotTool* InDet::InDetBeamSpotFinder::cloneTool(int i) {
  IInDetBeamSpotTool  * orig = &(*m_beamSpotToolList[i]);
  IInDetBeamSpotTool * temp = orig->Clone();
  temp = orig->Clone();
  return temp;
}

std::string InDet::InDetBeamSpotFinder::getAlgorithmName(int status) {
  switch ( status ) {
  case 0:
    return "unknown";
  case 1:
    return "trackChi2";
  case 2:
    return "trackChi2";
  case 3:
    return "vertexLL";
  case 4:
    return "vertexChi2";
  default:
    return "unknown";
  }
  return "unknown";
}

std::string InDet::InDetBeamSpotFinder::getFitStatusName(int status) {
  switch ( status ) {
  case 1:
    return "problems";
  case 2:
    return "Reserved - not in use";
  case 3:
    return "Successful";
  default:
    return "Defaults used";
    
  }

}

void InDet::InDetBeamSpotFinder::printBeamspot(const BeamSpot::ID* id , const IInDetBeamSpotTool* bs){
  //Print a nice summary of the beamspot information
  if (!id || !bs) {
    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "No beamspot information available"<<endreq;
    }
    return;
  }

  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "\n" 
		   << "\n*   " << "Run:       " << id->run() 
		   << "\n*   " << "ID:        " << id->id()
		   << "\n*   " << "lumStart:  " << id->lumiStart()
		   << "\n*   " << "lumEnd:    " << id->lumiEnd();
    if (m_separateByBCID)
      msg(MSG::INFO)  << "\n*   " << "BCID:   " << id->bcid();
    msg(MSG::INFO) << endreq;
    
    
  } // if
}

void InDet::InDetBeamSpotFinder::setupBeamspotTree(){
  const std::string inRootID = "/INDETBEAMSPOTFINDER/";
  const std::string sbs       = m_root_beamspotName;
  
  m_root_bs = new TTree(sbs.data(),"Beamspot Solutions");
  m_root_bs->Branch("bcid",&m_beamSpotNtuple.bcid,"bcid/I");
  m_root_bs->Branch("defectWord",    &m_beamSpotNtuple.defectWord, "defectWord/I");
  m_root_bs->Branch("fill",          &m_beamSpotNtuple.fill,       "fill/I");
  m_root_bs->Branch("lbEnd",         &m_beamSpotNtuple.lbEnd,      "lbEnd/I");
  m_root_bs->Branch("lbStart",       &m_beamSpotNtuple.lbStart,    "lbStart/I");
  m_root_bs->Branch("nEvents",       &m_beamSpotNtuple.nEvents,    "nEvents/I");
  m_root_bs->Branch("nValid",        &m_beamSpotNtuple.nValid,     "nValid/I");
  m_root_bs->Branch("nVtxAll",       &m_beamSpotNtuple.nVtxAll,    "nVtxAll/I");
  m_root_bs->Branch("nVtxPrim",      &m_beamSpotNtuple.nVtxPrim,   "nVtxPrim/I");
  m_root_bs->Branch("separation",    &m_beamSpotNtuple.separation, "separation/I");
  m_root_bs->Branch("status",        &m_beamSpotNtuple.status,        "status/I");
  m_root_bs->Branch("timeEnd",       &m_beamSpotNtuple.timeEnd,       "timeEnd/I");
  m_root_bs->Branch("timeStart",     &m_beamSpotNtuple.timeStart,     "timeStart/I");
  m_root_bs->Branch("run",           &m_beamSpotNtuple.run,           "run/I");
  m_root_bs->Branch("runEnd",        &m_beamSpotNtuple.runEnd,        "runEnd/I");
  if (StatusCode::SUCCESS==m_thistSvc->regTree(inRootID+sbs,m_root_bs)) {
    msg(MSG::INFO) << "Booked module ntuple " << inRootID <<sbs << endreq;
  } else {
    msg(MSG::ERROR) << "Unable to register module ntuple " <<inRootID <<sbs<<
      endreq;
  }
}


void InDet::InDetBeamSpotFinder::setupVertexTree(){
  const std::string inRootID = "/INDETBEAMSPOTFINDER/";
  const std::string svrts       = m_root_vtxName;
  m_root_vrt = new TTree(svrts.data(),"Primary Vertices");
  //m_root_vrt->Branch("event",m_root_event,"run/i:event:lb:bcid");
  m_root_vrt->Branch("bcid",&m_bcid,"bcid/I");
  m_root_vrt->Branch("vrt",&m_root_vtx,"x/D:y:z:vxx:vxy:vyy:vxz:vyz:vzz:chi2:ndf:vType/I:valid:run:evt:lb:bcid");
  m_root_vrt->Branch("qual",&m_root_par,"nTracks/I:passed");

  if (StatusCode::SUCCESS==m_thistSvc->regTree(inRootID+svrts,m_root_vrt)) {
    msg(MSG::INFO) << "Booked module ntuple " << inRootID <<svrts << endreq;
  } else {
    msg(MSG::ERROR) << "Unable to register module ntuple " <<inRootID <<svrts<<
      endreq;
    }
}

void InDet::InDetBeamSpotFinder::addToVertexTree(const xAOD::Vertex *vtx, const xAOD::EventInfo &eventInfo){
  m_root_vtx.x    = vtx->x();
  m_root_vtx.y    = vtx->y();
  m_root_vtx.z    = vtx->z();

  m_root_vtx.vxx  = vtx->covariancePosition()(0,0);
  m_root_vtx.vxy  = vtx->covariancePosition()(0,1);
  m_root_vtx.vyy  = vtx->covariancePosition()(1,1);
  m_root_vtx.vxz  = vtx->covariancePosition()(0,2);
  m_root_vtx.vyz  = vtx->covariancePosition()(1,2);
  m_root_vtx.vzz  = vtx->covariancePosition()(2,2);
    
  m_root_vtx.chi2 =  vtx->chiSquared();
  m_root_vtx.ndf  =  vtx->numberDoF();
  m_root_vtx.vertexType = vtx->vertexType();

  m_root_vtx.run = eventInfo.runNumber();
  m_root_vtx.evt = eventInfo.eventNumber();
  m_root_vtx.lb  = eventInfo.lumiBlock();
  m_root_vtx.tck = eventInfo.bcid();

  m_root_par.nTracks = vtx->nTrackParticles();
}

//This is the function returns true if the vertex passes all cuts, and false otherwise
bool InDet::InDetBeamSpotFinder::applySelection(const xAOD::Vertex * vtx ) {
  if (!vtx) return false;
  //  return false;
  bool passed = true;

  //ignore vertices with Chi2 < m_maxChi2Vertex
  if (  vtx->chiSquared()/vtx->numberDoF() > m_maxChi2Vertex ) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Vertex not included in fit; failed chi2 cut: " 
						<< endreq;
    passed = false;
  }
  //ignore vertex if made with less than m_minTrackNum tracks
  if (  static_cast<int>(vtx->nTrackParticles()) < m_minTrackNum ) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Vertex not included; failed min. track number:"
						<< " has " << vtx->nTrackParticles() << " tracks" << endreq;
    passed = false;
  }
  //ignore vertex if made with greater than m_maxTrackNum tracks
  if (  static_cast<int>(vtx->nTrackParticles()) > m_maxTrackNum ) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Vertex not included; failed max. track number:"
						<< " has " << vtx->nTrackParticles() << " tracks" << endreq;
    passed = false;
  }
  //ignore vertex if the Prob(chi2,ndf) is lower than some min value
  if ( TMath::Prob(vtx->chiSquared(), vtx->numberDoF()) < m_minVtxProb ) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Vertex not included in fit; failed chi2/ndf Prob cut: "  
						<< TMath::Prob(vtx->chiSquared(), vtx->numberDoF()) 
						<< " < " << m_minVtxProb  << endreq;
    passed = false;
  }

  // add checks bassed on validity of vertex
  if ( vtx->covariancePosition()(0,0) <= 0 || 
       vtx->covariancePosition()(1,1) <= 0 ||
       vtx->covariancePosition()(2,2) <= 0 ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Vertex not correctly defined. " 
						<< vtx->position() 
						<< vtx->covariancePosition() << endreq;
    passed = false;
  }
  else if(sqrt(vtx->covariancePosition()(0,0)) > m_maxTransverseErr || sqrt(vtx->covariancePosition()(1,1)) > m_maxTransverseErr){
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Vertex not included; failed max. transverse error:" << " sqrt(vxx) =  " << sqrt(vtx->covariancePosition()(0,0)) << " and sqrt(vyy) = " << (vtx->covariancePosition()(1,1)) << endreq;
    passed = false;
  }

  //if Vertex type specified, apply selection
  if ( m_vertexTypes.size() ) {
    if ( m_vertexTypes.end() == find(  m_vertexTypes.begin(),  
				       m_vertexTypes.end(),
				       vtx->vertexType() ) ){
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Vertex not included; not correct VertexType." << endreq;
      passed = false;
    }
  }
  return passed;
}

//Convert the vector of strings to vector of vertex types.
void InDet::InDetBeamSpotFinder::convertVtxTypeNames(){
  if ( m_vertexTypeNames.size() ) {
    for ( std::vector<std::string>::const_iterator it = m_vertexTypeNames.begin();
	  it != m_vertexTypeNames.end(); ++it) {
      if ((*it) == "NoVtx") ;
      else if ((*it) == "PriVtx") m_vertexTypes.push_back(xAOD::VxType::PriVtx);
      else if ((*it) == "SecVtx") m_vertexTypes.push_back(xAOD::VxType::SecVtx);
      else if ((*it) == "PileUp") m_vertexTypes.push_back(xAOD::VxType::PileUp);
      else if ((*it) == "ConvVtx") m_vertexTypes.push_back(xAOD::VxType::ConvVtx);
      else if ((*it) == "V0Vtx") m_vertexTypes.push_back(xAOD::VxType::V0Vtx);
      else if ((*it) == "KinkVtx") m_vertexTypes.push_back(xAOD::VxType::KinkVtx);
      else if ((*it) =="NotSpecified")m_vertexTypes.push_back(xAOD::VxType::NotSpecified) ;
    }
    msg(MSG::INFO) << "Allowing " << m_vertexTypes.size()
	  << " Vertex types" << endreq;
  }else {
    msg(MSG::DEBUG) << "No selection based on vertexType will be done" 
	  << endreq;
  }
}

//This function adds the selected vertex to the list of vertices
bool InDet::InDetBeamSpotFinder::addVertex(const xAOD::Vertex * vxc, BeamSpot::ID id ) {
  // update the beamspot values with the vertex info.

  if ( !vxc ) return false;

  double x,y,z;
  double vxx, vxy,vyy, vxz,vyz,vzz;

  x = vxc->x();
  y = vxc->y();
  z = vxc->z();

  vxx  = vxc->covariancePosition()(0,0);
  vxy  = vxc->covariancePosition()(0,1);
  vyy  = vxc->covariancePosition()(1,1);
  vxz  = vxc->covariancePosition()(0,2);
  vyz  = vxc->covariancePosition()(1,2);
  vzz  = vxc->covariancePosition()(2,2);

  BeamSpot::VrtHolder vtx;
  vtx.x = x;
  vtx.y = y;
  vtx.z = z;
  vtx.vxx = vxx;
  vtx.vxy = vxy;
  vtx.vyy = vyy;
  vtx.vxz = vxz;
  vtx.vyz = vyz;
  vtx.vzz = vzz;

  vtx.chi2  = vxc->chiSquared();
  vtx.ndf   = vxc->numberDoF();

  vtx.vertexType = vxc->vertexType();

  m_vertexDataMap[id].push_back(vtx);
  return true;
}
