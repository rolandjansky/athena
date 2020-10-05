/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotFinder.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "InDetBeamSpotFinder/IInDetBeamSpotTool.h"
#include "InDetBeamSpotVertex.h"
#include "InDetBeamSpotRooFit.h"
#include "GaudiKernel/ITHistSvc.h"
#include "VxVertex/VxCandidate.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include <algorithm>

namespace{
  //comparison of events for lumiBlock
  auto lesserLumiBlock = [](const BeamSpot::Event & e1, const BeamSpot::Event & e2)->bool{
    return (e1.lumiBlock<e2.lumiBlock);
  };
  
  //comparison of events for run number
  auto lesserRunNumber = [](const BeamSpot::Event & e1, const BeamSpot::Event & e2)->bool{
    return (e1.runNumber<e2.runNumber);
  };

}

InDet::InDetBeamSpotFinder::InDetBeamSpotFinder(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_toolSvc("ToolSvc",name),m_bcTool("Trig::TrigConfBunchCrossingTool/BunchCrossingTool")
{
  declareProperty( "ToolSvc", m_toolSvc );
  declareProperty( "BCTool", m_bcTool );
  declareProperty( "BeamSpotToolList"  , m_beamSpotToolList );
  declareProperty( "RunRange"     , m_maxRunsPerFit  = 0 );
  declareProperty( "LumiRange"      , m_maxLBsPerFit = 0 );
  declareProperty( "EventRange"   , m_maxEventsPerFit = 0);
  declareProperty( "UseBCID"     , m_BCIDsToAccept );
  declareProperty( "UseFilledBCIDsOnly", m_useFilledBCIDsOnly = true );
  declareProperty( "MinTracksPerVtx", m_minTrackNum = 5);
  declareProperty( "MaxTracksPerVtx", m_maxTrackNum = 1000000);
  declareProperty( "MinVtxNum"      , m_minVertexNum = 100);
  declareProperty( "MaxVtxChi2"     , m_maxChi2Vertex = 10);
  declareProperty( "MaxTransverseErr", m_maxTransverseError=1000000);
  declareProperty( "VertexTypes"       , m_vertexTypeNames);
  declareProperty( "MinVtxProb"     , m_minVtxProb=0.001);
  declareProperty( "GroupFitsBy"       , m_fitSortingKey = "none");
  declareProperty( "VertexNtuple" , m_writeVertexNtuple = true);
  declareProperty( "WriteAllVertices"  , m_writeAllVertices=false);
  declareProperty( "VertexTreeName"    , m_vertexTreeName);
  declareProperty( "SecondsPerFit",  m_secondsPerFit = 1);
}

StatusCode InDet::InDetBeamSpotFinder::initialize() {
  ATH_MSG_DEBUG( "in initialize()");
  if ( m_beamSpotToolList.empty() ){
    ATH_MSG_FATAL("FATAL ERROR: must provide at least one beamspot tool in beamSpotToolList");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( service("THistSvc",m_thistSvc) );
  ATH_CHECK( m_toolSvc.retrieve() );
  if( m_useFilledBCIDsOnly ) ATH_CHECK( m_bcTool.retrieve() );
  ATH_CHECK( m_eventInfo.initialize() );
  ATH_CHECK( m_vertexContainer.initialize() );

  for ( unsigned int i = 0; i < m_beamSpotToolList.size(); i++){ ATH_CHECK( m_beamSpotToolList[i].retrieve() );}
  if( m_writeVertexNtuple ){ ATH_CHECK(setupVertexTree()); }
  ATH_CHECK( setupBeamSpotTree() );
  convertVtxTypeNames();
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotFinder::execute(){
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);
  SG::ReadHandle<xAOD::VertexContainer> vertexContainer(m_vertexContainer);

  if ( !passEventSelection( *eventInfo ) ) return StatusCode::SUCCESS;
  BeamSpot::Event currentEvent = readEvent(*eventInfo, *vertexContainer);
  m_eventList.push_back( currentEvent );
  if( m_writeVertexNtuple ){
    for( auto & thisVertex: currentEvent.vertices){
      if( thisVertex.passed || m_writeAllVertices ){
        writeToVertexTree( currentEvent, thisVertex );
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotFinder::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  sortEvents();
  ATH_CHECK(performFits());
  return StatusCode::SUCCESS;
}

BeamSpot::Event InDet::InDetBeamSpotFinder::readEvent(const xAOD::EventInfo & eventInfo, const xAOD::VertexContainer & vertexContainer ){
  BeamSpot::Event event;
  BeamSpot::VrtHolder vertex;
  event.pileup = ceil(eventInfo.actualInteractionsPerCrossing());
  event.runNumber = eventInfo.runNumber();
  event.lumiBlock  = eventInfo.lumiBlock();
  event.bcid  = eventInfo.bcid();
  event.eventTime    = eventInfo.timeStamp();
  event.eventTime_NS = eventInfo.timeStampNSOffset();
  event.eventNumber  = eventInfo.eventNumber();
  const EventInfo* BSeventInfo; 
  //This is required for pseudo lumiblocks 
  if( evtStore()->retrieve(BSeventInfo) != StatusCode::SUCCESS){
    ATH_MSG_ERROR("Cannot get event info.");
    return event;
  }
  if (event.lumiBlock != BSeventInfo->event_ID()->lumi_block())
  {
    //ATH_MSG_INFO("Updating Event info " <<  BSeventInfo->event_ID()->lumi_block() );   
    event.lumiBlock = BSeventInfo->event_ID()->lumi_block(); 
  }
  
  for(const auto & vtx:vertexContainer) {
    if (vtx->vertexType() == xAOD::VxType::NoVtx) continue; 
    vertex.x          = vtx->x();
    vertex.y          = vtx->y();
    vertex.z          = vtx->z();
    vertex.vxx        = vtx->covariancePosition()(0,0);
    vertex.vxy        = vtx->covariancePosition()(0,1);
    vertex.vyy        = vtx->covariancePosition()(1,1);
    vertex.vzz        = vtx->covariancePosition()(2,2);
    vertex.vertexType = vtx->vertexType();
    vertex.nTracks    = vtx->nTrackParticles();
    vertex.passed     = passVertexSelection( vtx );
    vertex.valid      = vertex.passed;    
    event.vertices.push_back( vertex );
  }
  return event;
}

void InDet::InDetBeamSpotFinder::sortEvents(){
  for( const auto & thisEvent: m_eventList){
    BeamSpot::ID id;
    id.runNumber( (m_maxRunsPerFit > 0) ? thisEvent.runNumber : 0 );
    id.lumiBlock( (m_maxLBsPerFit  > 0) ? thisEvent.lumiBlock : 0 );
    id.pileup   ( iequals(m_fitSortingKey,"pileup") ? thisEvent.pileup : 0 );
    id.bcid     ( iequals(m_fitSortingKey,"bcid"  ) ? thisEvent.bcid   : 0 );
    //std::cout << "time " << iequals(m_fitSortingKey,"time"  )<< "  "  <<  m_eventList[i].eventTime/m_secondsPerFit  << std::endl; 
    id.timeStamp( iequals(m_fitSortingKey,"time"  ) ? thisEvent.eventTime/m_secondsPerFit  : 0 );
    m_eventMap[id].push_back( thisEvent );
  }
  auto iter = m_eventMap.begin();
  BeamSpot::ID lastID = iter->first;
  BeamSpot::ID currentID = iter->first;
  unsigned int nRuns = 0;
  unsigned int nLBs = 0;
  unsigned int nFits = 1;
  m_sortedEventList.resize( nFits );
  for( iter = m_eventMap.begin(); iter != m_eventMap.end(); ++iter){
    currentID = iter->first;
    if( iter == m_eventMap.begin() || currentID.runNumber() != lastID.runNumber() ){ nRuns++; }
    if( iter == m_eventMap.begin() || currentID.lumiBlock() != lastID.lumiBlock() ){ nLBs++;  }
    if( currentID.timeStamp() != lastID.timeStamp() || 
      currentID.pileup() != lastID.pileup() || currentID.bcid() != lastID.bcid() 
  || ( m_maxRunsPerFit > 0 && nRuns > m_maxRunsPerFit )
  || ( m_maxLBsPerFit  > 0 && nLBs  > m_maxLBsPerFit )){
      nFits++;
      m_sortedEventList.resize(nFits);
      nRuns = 1; nLBs = 1;
    }
    for( unsigned int i = 0; i < iter->second.size(); i++){
      if( m_sortedEventList.at(nFits-1).size() == m_maxEventsPerFit && m_maxEventsPerFit > 0 ){
  nFits++;
  m_sortedEventList.resize(nFits);
  nRuns = 1; nLBs = 1;
      }
      m_sortedEventList.at(nFits-1).push_back( iter->second.at(i) );
    }
    lastID = iter->first;
  }
}

//Convert the vector of strings to vector of vertex types.
void InDet::InDetBeamSpotFinder::convertVtxTypeNames(){
  if ( not m_vertexTypeNames.empty() ) {
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
    ATH_MSG_INFO("Allowing " << m_vertexTypes.size() << " Vertex types" );
  }
  else {
    ATH_MSG_DEBUG( "No selection based on vertexType will be done" );
  }
}

bool InDet::InDetBeamSpotFinder::passEventSelection(const xAOD::EventInfo & eventInfo){
  int bcid = eventInfo.bcid();
  if (m_useFilledBCIDsOnly && !m_bcTool->isFilled(bcid)) { return false; }
  if( m_BCIDsToAccept.begin() !=  m_BCIDsToAccept.end() )
    return ( std::find(m_BCIDsToAccept.begin(), m_BCIDsToAccept.end(), bcid) != m_BCIDsToAccept.end());
  else 
    return true;
}

bool InDet::InDetBeamSpotFinder::passVertexSelection(const xAOD::Vertex * vtx ) {
  if(!vtx)                                                     { return false; }
  if(vtx->chiSquared()/vtx->numberDoF() > m_maxChi2Vertex)     { return false; }
  if(static_cast<int>(vtx->nTrackParticles()) < m_minTrackNum ){ return false; }
  if(static_cast<int>(vtx->nTrackParticles()) > m_maxTrackNum ){ return false; }
  if(TMath::Prob(vtx->chiSquared(), vtx->numberDoF()) < m_minVtxProb ) { return false; }
  if(vtx->covariancePosition()(0,0) <= 0 || vtx->covariancePosition()(1,1) <= 0 || vtx->covariancePosition()(2,2) <= 0 ) { return false; }
  if(m_vertexTypes.end() == std::find(  m_vertexTypes.begin(), m_vertexTypes.end(), vtx->vertexType() ) ) { return false; }
  if(std::sqrt(vtx->covariancePosition()(0,0)) > m_maxTransverseError || std::sqrt(vtx->covariancePosition()(1,1)) > m_maxTransverseError) {return false;}
  return true;
}

StatusCode InDet::InDetBeamSpotFinder::setupVertexTree(){
  const std::string inRootID = "/INDETBEAMSPOTFINDER/";
  const std::string svrts       = m_vertexTreeName;
  m_root_vrt = new TTree(svrts.data(),"Vertices");
  m_root_vrt->Branch("vrt",&m_root_vtx,"x/D:y:z:vxx:vxy:vyy:vzz:vType/i:run:lb:bcid:pileup:nTracks:eventNumber/l:eventTime:eventTime_NS:passed/O:valid");
  return m_thistSvc->regTree(inRootID+svrts,m_root_vrt);
}

StatusCode InDet::InDetBeamSpotFinder::performFits(){
  IInDetBeamSpotTool::FitStatus bsFitStatus;
  std::vector<BeamSpot::VrtHolder> verticesToFit;

  for( auto & eventList: m_sortedEventList){
    verticesToFit.clear();
    for( const auto & thisEvent: eventList){
      for( const auto & thisVertex: thisEvent.vertices){
        if( thisVertex.passed ) { verticesToFit.push_back( thisVertex ); }
      }
    }
    for( unsigned int j = 0; j < m_beamSpotToolList.size(); j++){
      IInDetBeamSpotTool * bs(0);
      bs = cloneTool(j);
      if(!bs){ return StatusCode::FAILURE; }
      if(not verticesToFit.empty()) { bsFitStatus = bs->fit(verticesToFit); }
      else { bsFitStatus = IInDetBeamSpotTool::unsolved; }
      
      m_BeamStatusCode.clearWord();
      m_BeamStatusCode.setOnlineStatus(false);
      m_BeamStatusCode.setAlgType( bs->getFitID() );
      int fitStat;
      if ( bsFitStatus == IInDetBeamSpotTool::successful)
        fitStat = 3;
      else if ( bsFitStatus == IInDetBeamSpotTool::problems)
        fitStat = 1;
      else if ( bsFitStatus == IInDetBeamSpotTool::failed || bsFitStatus == IInDetBeamSpotTool::unsolved)
        fitStat = 0;  
      else 
        fitStat = 0;
      m_BeamStatusCode.setFitStatus(fitStat);
      if (bs->getParamMap()["sigmaX"] == 0 && bs->getParamMap()["sigmaY"] ==0 ) { m_BeamStatusCode.setFitWidth( false); }
      else { m_BeamStatusCode.setFitWidth(true); } 

      if(not eventList.empty()) writeToBeamSpotTree( bs, eventList, verticesToFit );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotFinder::setupBeamSpotTree(){
  const std::string inRootID = "/INDETBEAMSPOTFINDER/";
  const std::string sbs       = "BeamSpotNt";//m_root_beamspotName;
  m_root_bs = new TTree(sbs.data(),"Beamspot Solutions");
  m_root_bs->Branch("bcid",          &m_beamSpotNtuple.bcid,"bcid/I");
  m_root_bs->Branch("pileup",        &m_beamSpotNtuple.pileup,"pileup/I");
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
  
  for( auto &tool : m_beamSpotToolList ){
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
      const std::string & key =  iter->first;
      //double val  =  iter->second;
      if( !(m_root_bs->GetBranch(key.c_str())) ){
        m_beamSpotNtuple.covMap[key] = 0;
        keySlashD = key + slashD;
        m_root_bs->Branch( key.c_str(), &m_beamSpotNtuple.covMap[key], keySlashD.c_str());
      }
    }
  }
  return m_thistSvc->regTree(inRootID+sbs,m_root_bs);
}

void InDet::InDetBeamSpotFinder::writeToVertexTree(BeamSpot::Event &evt, BeamSpot::VrtHolder &vtx ){
  m_root_vtx.x    = vtx.x;
  m_root_vtx.y    = vtx.y;
  m_root_vtx.z    = vtx.z;
  m_root_vtx.vxx  = vtx.vxx;
  m_root_vtx.vxy  = vtx.vxy;
  m_root_vtx.vyy  = vtx.vyy;
  m_root_vtx.vzz  = vtx.vzz;
  m_root_vtx.vType = vtx.vertexType;
  m_root_vtx.run = evt.runNumber;
  m_root_vtx.lb  = evt.lumiBlock;
  m_root_vtx.bcid = evt.bcid;
  m_root_vtx.pileup = evt.pileup;
  m_root_vtx.nTracks = vtx.nTracks;
  m_root_vtx.passed = vtx.passed;
  m_root_vtx.valid        = vtx.valid;
  m_root_vtx.eventNumber  = evt.eventNumber;
  m_root_vtx.eventTime    = evt.eventTime;
  m_root_vtx.eventTime_NS = evt.eventTime_NS;
  m_root_vrt->Fill();
}

bool InDet::InDetBeamSpotFinder::iequals(const std::string& a, const std::string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

void InDet::InDetBeamSpotFinder::writeToBeamSpotTree(const IInDetBeamSpotTool *bs, std::vector<BeamSpot::Event> &eventList, std::vector<BeamSpot::VrtHolder> &vertexList){
  m_beamSpotNtuple.pileup = iequals(m_fitSortingKey,"pileup") ? eventList.at(0).pileup : 0;
  m_beamSpotNtuple.bcid   = iequals(m_fitSortingKey,"bcid")   ? eventList.at(0).bcid   : 0;
  m_beamSpotNtuple.defectWord = 0;
  m_beamSpotNtuple.fill = 0;
  m_beamSpotNtuple.lbEnd   = max_lb( eventList );
  m_beamSpotNtuple.lbStart = min_lb( eventList );
  m_beamSpotNtuple.nEvents = eventList.size();  
  m_beamSpotNtuple.nValid = vertexList.size();
  unsigned int nVtxAll = 0;
  unsigned int nVtxPrim = 0; 
  auto isPrimaryVertex=[](const BeamSpot::VrtHolder & vertex){ return (vertex.vertexType == xAOD::VxType::PriVtx); };
  for( const auto & thisEvent : eventList){
    const auto & theseVertices=thisEvent.vertices;
    nVtxAll += theseVertices.size();
    nVtxPrim+= std::count_if(theseVertices.begin(), theseVertices.end(), isPrimaryVertex);
  }
  m_beamSpotNtuple.nVtxAll = nVtxAll;
  m_beamSpotNtuple.nVtxPrim = nVtxPrim;
  m_beamSpotNtuple.run = min_run( eventList );
  m_beamSpotNtuple.status = m_BeamStatusCode.getWord();  
  m_beamSpotNtuple.separation = 0;
  m_beamSpotNtuple.timeEnd = iequals(m_fitSortingKey,"time")   ? eventList.back().eventTime   : 0;
  m_beamSpotNtuple.timeStart = iequals(m_fitSortingKey,"time")   ? eventList.front().eventTime   : 0;
  m_beamSpotNtuple.runEnd = max_run( eventList );
  const auto & bsToolCovMap= bs->getCovMap();
  for (auto & param:m_beamSpotNtuple.paramMap){
    const std::string & key = param.first;
    const auto & bsToolEquivalent = bsToolCovMap.find(key);
    param.second = ( bsToolEquivalent == bsToolCovMap.end() ) ? 0 : bsToolEquivalent->second;
  }
  /** leave these here; illustrates coverity defect 29456
  for( std::map<std::string,double>::iterator iter = m_beamSpotNtuple.paramMap.begin(); 
       iter != m_beamSpotNtuple.paramMap.end(); ++iter){    
    std::string key = iter->first;
    iter->second = ( bs->getParamMap().find(key) == bs->getParamMap().end() ) ? 0 : bs->getParamMap()[key];
  }
  **/
  for (auto & covariance:m_beamSpotNtuple.covMap){
    const std::string & key = covariance.first;
    const auto & bsToolEquivalent = bsToolCovMap.find(key);
    covariance.second = ( bsToolEquivalent == bsToolCovMap.end() ) ? 0 : bsToolEquivalent->second;
  }
  /** leave these here; illustrates coverity defect 29456
  for( std::map<std::string,double>::iterator iter = m_beamSpotNtuple.covMap.begin(); 
       iter != m_beamSpotNtuple.covMap.end(); ++iter){    
    std::string key = iter->first;
    iter->second = ( bs->getCovMap().find(key) == bs->getCovMap().end() ) ? 0 : bs->getCovMap()[key];
  }
  **/
  m_root_bs->Fill();
}

InDet::IInDetBeamSpotTool* InDet::InDetBeamSpotFinder::cloneTool(int i) {
  IInDetBeamSpotTool  * orig = &(*m_beamSpotToolList[i]);
  IInDetBeamSpotTool * temp = orig->Clone();
  return temp;
}

int InDet::InDetBeamSpotFinder::min_lb( std::vector<BeamSpot::Event> & eventList ){
  const auto smallestLbEvent=std::min_element(eventList.begin(),eventList.end(), lesserLumiBlock);
  return smallestLbEvent->lumiBlock;
}

int InDet::InDetBeamSpotFinder::max_lb( std::vector<BeamSpot::Event> & eventList ){
  const auto largestLbEvent=std::max_element(eventList.begin(),eventList.end(), lesserLumiBlock);
  return largestLbEvent->lumiBlock;
}

int InDet::InDetBeamSpotFinder::min_run( std::vector<BeamSpot::Event> & eventList ){
  const auto smallestRunEvent=std::min_element(eventList.begin(),eventList.end(), lesserRunNumber);
  return smallestRunEvent->runNumber;
}

int InDet::InDetBeamSpotFinder::max_run( std::vector<BeamSpot::Event> & eventList ){
  const auto largestRunEvent=std::max_element(eventList.begin(),eventList.end(), lesserRunNumber);
  return largestRunEvent->runNumber;
}

