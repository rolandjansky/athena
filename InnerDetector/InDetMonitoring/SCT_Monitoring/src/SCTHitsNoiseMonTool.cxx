/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTHitsNoiseMonTool.cxx
*
*     (merging SCTHitsMonTool.cxx and SCTNoiseMonTool.cxx in one Tool)
*    
*                          
*    @author Sofia Chouridou based on code from Martin White, Luca Fiorini, Shaun Roe, Manuel Diaz & Rob McPherson 
*    'The end of all of our exploring...' (Shaun, March 2008)
*/

#include "SCT_NameFormatter.h"

#include <sstream> //?
#include <iostream>
#include <fstream> //for writing an xml summary (debugging)
#include <algorithm>


#include "GaudiKernel/IJobOptionsSvc.h"
#include "SCT_Monitoring/SCTHitsNoiseMonTool.h"
#include "SCT_Monitoring/SCT_ClusterStruct.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/PropertyMgr.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TProfile2D.h"
#include "TProfile.h"

#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/SCT_TB03_RawData.h" //?
#include "InDetRawData/InDetRawDataContainer.h" //?
#include "InDetRawData/InDetRawDataCLASS_DEF.h" //?
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" //?

#include "PathResolver/PathResolver.h" //?
#include "InDetPrepRawData/SCT_ClusterContainer.h" //?
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
//for TriggerType
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h" //?
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h" //?
#include "cArrayUtilities.h"

typedef Trk::SpacePoint SpacePoint;
using namespace std;
using namespace SCT_Monitoring;
/** 
*  Utility functions to use in this file only
**/
namespace { //use anonymous namespace to restrict scope to this file, equivalent to 'static' keyword here
  //is a variable in the specified range?
  template <typename T>
  bool
  inRange(const T & variable, const T & lo, const T & hi){
    return not ((variable < lo) or (variable > hi)); //assumes both less_than and greater_than exist for this type
  }

    //Possible timing patterns, IXX is 1XX etc.
  enum 
    Pattern{ IXX=0, XIX, XXI };

    //possible coordinate species
  enum
    Coordinate{ ETA=0, PHI};
    //... and the corresponding names
  const std::string coordinateNames[]={"ETA", "PHI"};
    //possible histogram species

    //string names for the detector parts
  const std::string names[]={"Endcap C","Barrel","Endcap A"};
   //abbreviations for detector parts
   const std::string abbreviations[]={"ECm", "","ECp"};
   const std::string streamDelimiter("_");
   const std::string pathDelimiter("/");
   

    //is the timebin in the desired pattern?
  bool
  timeBinInPattern(const int tbin, const Pattern xxx){
    switch (xxx) {
      case IXX: 
      return (tbin>3);
      break;
      case XIX:
      return (tbin==2||tbin==3||tbin==6||tbin==7);
      break;
      case XXI:
      return (tbin==1||tbin==3||tbin==5||tbin==7);
      break;
      default:
          //std::cout<<"A strange timepattern was found; this should never happen"<<std::endl;
      return false;
      break;
    }
  }
}//end of anonymous namespace

//====================================================================================================
/** Constructor, calls base class constructor with parameters
*
*  several properties are "declared" here, allowing selection
*  of the filepath for histograms, the first and second plane
*  numbers to be used, and the timebin.
*/
//====================================================================================================
SCTHitsNoiseMonTool::SCTHitsNoiseMonTool(const std::string & type, 
const std::string & name,
const IInterface* parent)
  :SCTMotherTrigMonTool(type, name, parent),
  m_nSP(nullptr),
  nSP_buf(nullptr),
  nSP_pos(0),
  m_nHits(nullptr),
  nHits_buf(nullptr),
  nHits_pos(0),
  m_nmaxHits(nullptr),
  nmaxHits_buf(nullptr),
  nmaxModule_buf(nullptr),
  m_nminHits(nullptr),
  nminHits_buf(nullptr),
  nminModule_buf(nullptr),
  m_numBarrelHitsPerLumiBlock(nullptr),
  m_numHitsPerLumiBlockECp(nullptr),
  m_numHitsPerLumiBlockECm(nullptr),
  m_numBarrelSPPerLumiBlock(nullptr),
  m_numSPPerLumiBlockECp(nullptr),
  m_numSPPerLumiBlockECm(nullptr),
  rioMap(nullptr),
  m_barrelNOdistribution(nullptr),
  m_barrelNOdistributionTrigger(nullptr),
  m_ECmNOdistribution(nullptr),
  m_ECmNOdistributionTrigger(nullptr),
  m_ECpNOdistribution(nullptr),
  m_ECpNOdistributionTrigger(nullptr),
  m_SCTNOdistribution(nullptr),
  m_SCTNOdistributionTrigger(nullptr),
  m_MaxOccupancyStripHist(nullptr),
  m_MinOccupancyStripHist(nullptr),
  m_clusize(nullptr),
  m_clusizeRecent(nullptr),
  m_hitsvstrigger(nullptr),
  m_hitsvsL1ID(nullptr),
  m_ncluHisto(nullptr),
  coincidenceHist(nullptr),
  m_numberOfEvents(0),
  m_numberOfEventsTrigger(0),
  m_numberOfEventsRecent(0),
  m_skipEvents(0),
  m_booltxscan(false),
  m_current_lb(0),
  m_last_reset_lb(0),
  m_tracks(nullptr),
  m_tbinHisto(nullptr),
  m_tbinHistoECp(nullptr),
  m_tbinHistoECm(nullptr),
  m_tbinHistoRecent(nullptr),
  m_tbinHistoRecentECp(nullptr),
  m_tbinHistoRecentECm(nullptr),
  m_tbinfracall(nullptr),
  m_tbinfracVsLB(nullptr),
  m_tbinfracVsLBECp(nullptr),
  m_tbinfracVsLBECm(nullptr),
  m_initialize(false),
  m_pSCTHelper(nullptr),
  m_ConfigurationSvc("InDetSCT_ConfigurationConditionsSvc", name)
{
  /** sroe 3 Sept 2015:
  histoPathBase is declared as a property in the base class, assigned to m_path
  with default as empty string.
	Declaring it here as well gives rise to compilation warning
	WARNING duplicated property name 'histoPathBase', see https://its.cern.ch/jira/browse/GAUDI-1023

  declareProperty("histoPathBase", m_stream = "/stat"); **/
  m_stream="/stat";
  declareProperty("localSummary",m_localSummary = 0);
  declareProperty("doHitmapHistos",m_boolhitmaps=true);
  declareProperty("doTXScan",m_booltxscan=false);
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("CheckRecent",m_checkrecent=30);
  declareProperty("tracksName",m_tracksName="Combined_Tracks");
  declareProperty("NOTrigger",m_NOTrigger="L1_RD0_EMPTY");
  declareProperty("numberOfSigma",m_numSigma=3);
  declareProperty("EvtsBins",m_evtsbins=5000);
  declareProperty("doPositiveEndcap",m_doPositiveEndcap=true);
  declareProperty("doNegativeEndcap",m_doNegativeEndcap=true);
  declareProperty("doTrackBasedNoise",m_doTrackBasedNoise=false);
  declareProperty("doSpacePointBasedNoise",m_doSpacePointBasedNoise=true);
  declareProperty("doTimeBinFilteringForNoise",m_doTimeBinFilteringForNoise=true);
  declareProperty("doTrackHits",m_doTrackHits=true);
  declareProperty("MaxTracks", m_maxTracks=250);
  declareProperty("doLogXNoise", m_doLogXNoise=true);
  declareProperty("conditionsService", m_ConfigurationSvc);

  clear1D(m_tbinfrac);
  clear1D(m_tbinfracECp);
  clear1D(m_tbinfracECm);
  clear1D(m_clusizedist);
  clear1D(m_clusizedistECp);
  clear1D(m_clusizedistECm);
}

//====================================================================================================
//====================================================================================================
SCTHitsNoiseMonTool::~SCTHitsNoiseMonTool(){
  free(nSP_buf);
  free(nmaxHits_buf);
  free(nminHits_buf);
  free(nmaxModule_buf);
  free(nminModule_buf);
  free(nHits_buf);
}

//====================================================================================================
//                           SCTHitsNoiseMonTool :: bookHistograms
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::bookHistograms(){
  ATH_MSG_DEBUG( "initialize being called" );
  if (newRunFlag()) m_numberOfEvents=0;
  m_dataObjectName = "SCT_RDOs";
  // Get the helper:
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper,"SCT_ID"));
  ATH_CHECK(m_ConfigurationSvc.retrieve());
  m_initialize=true;
  
  const bool doSystem[]={m_doNegativeEndcap, true, m_doPositiveEndcap};
  for (unsigned int det(0);det != N_REGIONS;++det){
    if (doSystem[det]){
      if (m_doTrackHits and bookGeneralTrackTimeHistos(det).isFailure()) msg(MSG::WARNING) << "Error in bookGeneralTrackTimeHistos()" << endreq;
      if (m_doTrackHits and bookGeneralTrackHits(det).isFailure()) msg(MSG::WARNING) << "Error in bookGeneralTrackHits()" << endreq;
      if ( bookGeneralNoiseOccupancyMaps(det).isFailure() ) msg(MSG::WARNING) << "Error in bookGeneralNoiseOccupancyMaps()" << endreq;
      if ( bookGeneralHits(det).isFailure() )  msg(MSG::WARNING) << "Error in bookGeneralHits()" << endreq;
      //offline only?
      if ( bookGeneralCluSize(det).isFailure() ) msg(MSG::WARNING) << "Error in bookGeneralSize()" << endreq;
      //
    }
  }

  if ( bookSPvsEventNumber().isFailure() )     msg(MSG::WARNING) << "Error in bookSPvsEventNumber()" << endreq;
  if ( bookNoiseDistributions().isFailure() )  msg(MSG::WARNING) << "Error in bookNoiseDistributions()"  << endreq;
  
  if (m_boolhitmaps) {
    if ( book1DHitHists().isFailure() ) msg(MSG::WARNING) << "Error in barrel book1DHitHists()" << endreq;
    //Only want these histograms when running the code offline
  }
  
  if ( bookClusterSize().isFailure() ) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in bookClusterSize()" << endreq;

  if(newRunFlag()){
    std::string stem=m_stream+"/SCT/GENERAL/hits/summary";
    //book hits histogram
    MonGroup clu(this,"SCT/GENERAL/hits/summary",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED);
    m_ncluHisto = new TH1F("sct_hits","Total SCT Hits",N_HIT_BINS*4,FIRST_HIT_BIN,LAST_HIT_BIN*150);
    m_ncluHisto->GetXaxis()->SetTitle("Total SCT Hits");
    m_ncluHisto->GetYaxis()->SetTitle("Entries");
    if ( clu.regHist(m_ncluHisto).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << stem+"sct_hits" << endreq;
    if (m_booltxscan) {
      if( newEventsBlockFlag() ) {
	//book histograms for TX scans
	m_hitsvstrigger = new TH1F("hits_vs_trigger","Hits vs Trigger Type",32,-0.5,31.5);
	if ( clu.regHist(m_hitsvstrigger).isFailure() )  msg(MSG::WARNING) << "Cannot book Histogram:" << stem+"hitsvstrigger" << endreq;
	m_hitsvsL1ID = new TH1F("hits_vs_L1ID","Hits vs L1ID",1000,0.,10000.);
	if ( clu.regHist(m_hitsvsL1ID).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << stem+"hitsvsL1ID" << endreq;
      }
    }
    if ( initializeNoiseMaps().isFailure() ) msg(MSG::WARNING) << "Failed to initalize noise occupancy maps!" << endreq;
  }  

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                           SCTHitsNoiseMonTool :: bookHistograms
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::bookHistogramsRecurrent(){
  ATH_MSG_DEBUG( "bookHistogramsRecurrent being called" );
  m_numberOfEvents=0;
  m_dataObjectName = "SCT_RDOs";
  // Get the helper:
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper,"SCT_ID"));
  ATH_CHECK(m_ConfigurationSvc.retrieve());
  m_initialize=true;
  
  const bool doSystem[]={m_doNegativeEndcap, true, m_doPositiveEndcap};
  for (unsigned int det(0);det != N_REGIONS;++det){
    if (doSystem[det]){
      if (m_doTrackHits and bookGeneralTrackTimeHistos(det).isFailure()) msg(MSG::WARNING) << "Error in bookGeneralTrackTimeHistos()" << endreq;
      if (m_doTrackHits and bookGeneralTrackHits(det).isFailure()) msg(MSG::WARNING) << "Error in bookGeneralTrackHits()" << endreq;
      if ( bookGeneralNoiseOccupancyMaps(det).isFailure() ) msg(MSG::WARNING) << "Error in bookGeneralNoiseOccupancyMaps()" << endreq;
      if ( bookGeneralHits(det).isFailure() )  msg(MSG::WARNING) << "Error in bookGeneralHits()" << endreq;
      //offline only?
      if ( bookGeneralCluSize(det).isFailure() ) msg(MSG::WARNING) << "Error in bookGeneralSize()" << endreq;
      //
    }
  }

  if ( bookSPvsEventNumber().isFailure() )     msg(MSG::WARNING) << "Error in bookSPvsEventNumber()" << endreq;
  if ( bookNoiseDistributions().isFailure() )  msg(MSG::WARNING) << "Error in bookNoiseDistributions()"  << endreq;
  
  if (m_boolhitmaps) {
    if ( book1DHitHists().isFailure() ) msg(MSG::WARNING) << "Error in barrel book1DHitHists()" << endreq;
    //Only want these histograms when running the code offline
  }
  
  if ( bookClusterSize().isFailure() ) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in bookClusterSize()" << endreq;

  //  if(newRunFlag()){
    std::string stem=m_stream+"/SCT/GENERAL/hits/summary";
    //book hits histogram
    MonGroup clu(this,"SCT/GENERAL/hits/summary",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED);
    m_ncluHisto = new TH1F("sct_hits","Total SCT Hits",N_HIT_BINS*4,FIRST_HIT_BIN,LAST_HIT_BIN*150);
    m_ncluHisto->GetXaxis()->SetTitle("Event Number");
    m_ncluHisto->GetYaxis()->SetTitle("Num of SCT Hits");
    if ( clu.regHist(m_ncluHisto).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << stem+"sct_hits" << endreq;
    if (m_booltxscan) {
      //      if( newEventsBlockFlag() ) {
	//book histograms for TX scans
	m_hitsvstrigger = new TH1F("hits_vs_trigger","Hits vs Trigger Type",32,-0.5,31.5);
	if ( clu.regHist(m_hitsvstrigger).isFailure() )  msg(MSG::WARNING) << "Cannot book Histogram:" << stem+"hitsvstrigger" << endreq;
	m_hitsvsL1ID = new TH1F("hits_vs_L1ID","Hits vs L1ID",1000,0.,10000.);
	if ( clu.regHist(m_hitsvsL1ID).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << stem+"hitsvsL1ID" << endreq;
	//      }
    }
    if ( initializeNoiseMaps().isFailure() ) msg(MSG::WARNING) << "Failed to initalize noise occupancy maps!" << endreq;
    //  }  

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                       SCTHitsNoiseMonTool :: fillHistograms
// This is the real workhorse, called for each event. It retrieves the data each time 
//====================================================================================================
  StatusCode SCTHitsNoiseMonTool::fillHistograms(){
  ++m_numberOfEvents;
  ++m_numberOfEventsRecent;
  const EventInfo* pEvent(0);
  if( evtStore()->retrieve(pEvent).isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve event info!" <<endreq;
    return StatusCode::RECOVERABLE; 
  }
  m_current_lb = pEvent->event_ID()->lumi_block();
  //If track hits are selected, make the vector of track rdo identifiers
  if(m_doTrackHits){
    if(makeVectorOfTrackRDOIdentifiers().isFailure() and msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Couldn't make vector of track RDO identifiers" << endreq ;
  }
  if (generalHistsandNoise().isFailure() and msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in generalHists" << endreq ;
  if (makeSPvsEventNumber().isFailure() and msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in makeSPvsEventNumber" << endreq ;

  if(m_environment==AthenaMonManager::online){
    if ( (m_numberOfEvents % m_checkrate) == 0 ) {
      if (checkNoiseMaps(true).isFailure()) {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in checkNoiseMaps(true)" << endreq ;
      }
    }
    if ( (m_current_lb % m_checkrecent == 0) && (m_current_lb>m_last_reset_lb) ) {
      if (resetNoiseMapsRecent().isFailure()) {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in resetNoiseMapsRecent" << endreq ;
      }
      if (resetHitMapHists().isFailure()) {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in resetHitMapsRecent" << endreq ;
      }
      m_last_reset_lb=m_current_lb;
      m_numberOfEventsRecent=0;
    }
  }

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                      SCTHitsNoiseMonTool :: procHistograms
//PJB 08/08/2005
//Here we have the statistics by plane which can be written to the
//overall statistics object (the map) and then sent to the DB
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::procHistograms(){
  //SCTHitsNoiseMonTool::procHistograms(bool isEndOfEventsBlock, bool /*isEndOfLumiBlock*/, bool /*isEndOfRun*/){
  //  if(isEndOfEventsBlock){
  bool endOfEventsBlock(endOfLumiBlockFlag());  // hidetoshi 14.01.22 NEED TO BE REPLACED
  if(endOfEventsBlock){
    if (checkNoiseMaps(true).isFailure()) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in checkNoiseMaps(true)" << endreq ;
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                                   SCTHitsNoiseMonTool :: checkHists
// DD 08/10/2010
// Inherited method from base class that gets called by monitoring finalize after procHists - not used 
// at this time. Previously was and unnecessary middle function between procHistograms and 
// checkNoiseMaps which caused checkNoiseMaps to be called twice double-filling NO histograms
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::checkHists(bool /*fromFinalize*/){


  return StatusCode::SUCCESS;
}

//====================================================================================================
//                           SCTHitsNoiseMonTool :: generalHistsandNoise
//
// This method adds the spBasedNoise algorithm to the main RDO loop in an attempt to speed things up
// Once this works, it'll be the default- the old methods will be left as backups because we know they definitely work!
// This code was added by Martin White in June 2008
// Note that this is the only function with the online IPC functionality implemented- if you revert to the old methods you'll lose it
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::generalHistsandNoise(){  
  typedef SCT_RDORawData SCTRawDataType;
  const SCT_RDO_Container* p_rdocontainer;
  //Modified shirabe
  const EventInfo* pEvent(0);
  if( evtStore()->retrieve(pEvent).isFailure() ){
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve event info!" <<endreq;
    return StatusCode::FAILURE;
  }
  unsigned int current_lb = pEvent->event_ID()->lumi_block();
  //END Modified
  if(evtStore()->retrieve(p_rdocontainer,m_dataObjectName).isFailure()) return StatusCode::FAILURE;
  //Get the space point container
  const SpacePointContainer* sctContainer;
  m_SCTSPContainerName= "SCT_SpacePoints";
  if(evtStore()->retrieve(sctContainer,m_SCTSPContainerName).isFailure()) return StatusCode::FAILURE;
  Identifier SCT_Identifier;
  //Use new IDC
  SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin();
  SCT_RDO_Container::const_iterator lastCol  = p_rdocontainer->end();
  // Modified JEGN
  int local_tothits=0; int totalmodules = 0; int meanhits = 0; int maxrodhits = 0; int maxhits = 0;  Identifier maxhits_Id; int minhits = 99999;  Identifier minhits_Id;
  // END Modified

  bool isSelectedTrigger = false;

// EDAVIES - have now changed back to using L1_RD0_EMPTY 
  if(m_doTrigger){
     if  (AthenaMonManager::dataType() !=  AthenaMonManager::cosmics) {
	if (SCTMotherTrigMonTool::isCalibrationNoise(m_NOTrigger)) {
	isSelectedTrigger = true;
	++m_numberOfEventsTrigger;
      }
    }
  }

  //int local_tothits=0;
  std::vector<int> barrel_local_nhitslayer(N_BARRELSx2,0);
  std::vector<int> ECp_local_nhitslayer(N_DISKSx2,0);
  std::vector<int> ECm_local_nhitslayer(N_DISKSx2,0);
  //
  std::vector<H2_t> * hitHistogramVectors[3]={&m_phitsHistoVectorECm, &m_phitsHistoVector, &m_phitsHistoVectorECp };
  std::vector<H2_t> * hitHistogramVectorsRecent[3]={&m_phitsHistoVectorRecentECm, &m_phitsHistoVectorRecent, &m_phitsHistoVectorRecentECp };
  TH1F * hitPerLumiBlockHists[3]={m_numHitsPerLumiBlockECm, m_numBarrelHitsPerLumiBlock, m_numHitsPerLumiBlockECp };
  std::vector<int> * hitsInLayer[3] = { &ECm_local_nhitslayer, &barrel_local_nhitslayer, &ECp_local_nhitslayer};
  const bool doThisSubsystem[3] = {m_doNegativeEndcap, true, m_doPositiveEndcap};
  //
    //Define some iterators over the SP container
  SpacePointContainer::const_iterator spContainerIterator=sctContainer->begin();
  SpacePointContainer::const_iterator spContainerIteratorEnd=sctContainer->end();
  //Outer Loop on RDO Collection
  for( ; col_it!= lastCol ; ++col_it){
    const InDetRawDataCollection<SCTRawDataType>* SCT_Collection(*col_it);
    if (!SCT_Collection) continue;  // select only SCT RDOs
    //MJW new code- try getting the ID of the collection using the identify() method
    Identifier tempID = SCT_Collection->identify();
    Identifier theWaferIdentifierOfTheRDOCollection = tempID;    
    Identifier theModuleIdentifierOfTheRDOCollection = m_pSCTHelper->module_id(tempID);
    int numberOfHitsFromSPs=0;
    int numberOfHitsFromAllRDOs=0;
    //Now we want the space point container for this module
    //We have to compare module IDs- the SP collection is defined for the 'normal' (i.e. no stereo) module side
    //Define a set of spIDs
    set<Identifier>mySetOfSPIds;    
    for(;spContainerIterator!=spContainerIteratorEnd;++spContainerIterator) {
      Identifier tempSPID = (*spContainerIterator)->identify();
      Identifier theModuleIdentifierOfTheSPCollection=m_pSCTHelper->module_id(tempSPID);
      if(theModuleIdentifierOfTheSPCollection>theModuleIdentifierOfTheRDOCollection)break;
      if(theModuleIdentifierOfTheRDOCollection==theModuleIdentifierOfTheSPCollection){
        DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint=(*spContainerIterator)->begin();
        DataVector<Trk::SpacePoint>::const_iterator nextSpacePointEnd=(*spContainerIterator)->end();
        for(; nextSpacePoint!=nextSpacePointEnd ; ++nextSpacePoint) {
          const Trk::SpacePoint& sp = **nextSpacePoint;
          //the following is nasty; the 'normal' sides (where the sp is defined) swap from layer to layer. To be safe, we get both sides
          const VecId_t & rdoList0=sp.clusterList().first->rdoList();
          const VecId_t & rdoList1=sp.clusterList().second->rdoList();
          //copy to mySetOfSPIds. Use inserter(set, iterator_hint) for a set, or back_inserter(vec) for vector...
          copy(rdoList0.begin(), rdoList0.end(),inserter(mySetOfSPIds, mySetOfSPIds.end()));
          copy(rdoList1.begin(), rdoList1.end(),inserter(mySetOfSPIds, mySetOfSPIds.end()));
        }
        break;
      }
    }//
    totalmodules++; maxrodhits = 0;
    //Now we loop over the RDOs in the RDO collection, and add to the NO vector any that are in the mySetOfSPIds
    DataVector<SCTRawDataType>::const_iterator p_rdo=SCT_Collection->begin() ;
    DataVector<SCTRawDataType>::const_iterator end_rdo = SCT_Collection->end();
    for(; p_rdo!=end_rdo; ++p_rdo){
      SCT3_RawData* rdo3 = dynamic_cast<SCT3_RawData*>(*p_rdo);
      int tbin;// = (*p_rdo)->getTimeBin();
      if (rdo3!=0){
        tbin = (rdo3)->getTimeBin();
      }else{
        tbin=3;
      }
      SCT_Identifier = (*p_rdo)->identify();
      const unsigned int firstStrip (m_pSCTHelper->strip(SCT_Identifier));
      const unsigned int numberOfStrips ((*p_rdo)->getGroupSize());
      int thisBec       = m_pSCTHelper->barrel_ec(SCT_Identifier);
      int thisLayerDisk = m_pSCTHelper->layer_disk(SCT_Identifier);
      int thisPhi       = m_pSCTHelper->phi_module(SCT_Identifier);
      int thisEta       = m_pSCTHelper->eta_module(SCT_Identifier);
      int thisSide      = m_pSCTHelper->side(SCT_Identifier);
      int thisElement   =(2 * thisLayerDisk) + thisSide;
      //CAM adds mod id needed for map
      Identifier wafer_id =  m_pSCTHelper->wafer_id(SCT_Identifier);      
      unsigned int chan(firstStrip), limit (chan + numberOfStrips);
      unsigned int systemIndex=bec2Index(thisBec);
      ///CAM wafer id was here
      unsigned int module_hash = m_pSCTHelper->wafer_hash(wafer_id);       
      //
      if (doThisSubsystem[systemIndex]){

        H2_t histogram=(*hitHistogramVectors[systemIndex])[thisElement];
	H2_t histogram_recent=0;
	if(m_environment==AthenaMonManager::online) histogram_recent=(*hitHistogramVectorsRecent[systemIndex])[thisElement];

	if (find(m_RDOsOnTracks.begin(),m_RDOsOnTracks.end(),SCT_Identifier) != m_RDOsOnTracks.end()){
	  for (unsigned int ichan=chan; ichan!=limit; ++ichan){
	    //Fill the track hits histogram
            if(thisBec==0){
              m_tbinHisto->Fill(tbin,1.);
              m_tbinHistoVector[ thisLayerDisk ]->Fill(tbin,1.);
              m_ptrackhitsHistoVector[thisElement]->Fill(thisEta,thisPhi);
	      int layersidenum = thisLayerDisk*2 + thisSide;
	      if(tbin==2 || tbin==3){
		m_tbinfrac[layersidenum]->Fill(thisEta,thisPhi,1.);
		m_tbinfracVsLB->Fill(current_lb,1.0);
		m_tbinfracall->Fill(1.5,1.0);
	      }else{
		m_tbinfrac[layersidenum]->Fill(thisEta,thisPhi,0.);
		m_tbinfracVsLB->Fill(current_lb,0.0);
		m_tbinfracall->Fill(1.5,0.0);
	      }
	      if(m_environment==AthenaMonManager::online) {
		m_tbinHistoRecent->Fill(tbin,1.);
		m_tbinHistoVectorRecent[ thisLayerDisk ]->Fill(tbin,1.);
		m_ptrackhitsHistoVectorRecent[thisElement]->Fill(thisEta,thisPhi);
	      }
            }
            if(thisBec==2){
              m_tbinHistoECp->Fill(tbin,1.);
              m_tbinHistoVectorECp[ thisLayerDisk ]->Fill(tbin,1.);
              m_ptrackhitsHistoVectorECp[thisElement]->Fill(thisEta,thisPhi);
	      int layersidenum = thisLayerDisk*2 + thisSide;
	      if(tbin==2 || tbin==3){
		m_tbinfracECp[layersidenum]->Fill(thisEta,thisPhi,1.);
		m_tbinfracVsLBECp->Fill(current_lb,1.0);
		m_tbinfracall->Fill(2.5,1.0);
	      }else{
		m_tbinfracECp[layersidenum]->Fill(thisEta,thisPhi,0.);
		m_tbinfracVsLBECp->Fill(current_lb,0.0);
		m_tbinfracall->Fill(2.5,0.0);
	      }
	      if(m_environment==AthenaMonManager::online) {
		m_tbinHistoRecentECp->Fill(tbin,1.);
		m_tbinHistoVectorRecentECp[ thisLayerDisk ]->Fill(tbin,1.);
		m_ptrackhitsHistoVectorRecentECp[thisElement]->Fill(thisEta,thisPhi);
	      }
	    }
            if(thisBec==-2){
	      m_tbinHistoECm->Fill(tbin,1.);
              m_tbinHistoVectorECm[ thisLayerDisk ]->Fill(tbin,1.);
              m_ptrackhitsHistoVectorECm[thisElement]->Fill(thisEta,thisPhi);
	      int layersidenum = thisLayerDisk*2 + thisSide;
	      if(tbin==2 || tbin==3){
		m_tbinfracECm[layersidenum]->Fill(thisEta,thisPhi,1.);
		m_tbinfracVsLBECm->Fill(current_lb,1.0);
		m_tbinfracall->Fill(0.5,1.0);
	      }else{
		m_tbinfracECm[layersidenum]->Fill(thisEta,thisPhi,0.);
		m_tbinfracVsLBECm->Fill(current_lb,0.0);
		m_tbinfracall->Fill(0.5,0.0);
	      }
	      if(m_environment==AthenaMonManager::online) {
		m_tbinHistoRecentECm->Fill(tbin,1.);
		m_tbinHistoVectorRecentECm[ thisLayerDisk ]->Fill(tbin,1.);
		m_ptrackhitsHistoVectorRecentECm[thisElement]->Fill(thisEta,thisPhi);
	      }
	    }
          }
        }
        histogram->Fill(thisEta, thisPhi,numberOfStrips);
	if(m_environment==AthenaMonManager::online) histogram_recent->Fill(thisEta, thisPhi,numberOfStrips);
        numberOfHitsFromAllRDOs+=numberOfStrips;
  //Fill hits per lumi block histogram
        hitPerLumiBlockHists[systemIndex]->Fill(thisLayerDisk,numberOfStrips);
	// Record number of hits in space points if timebin filtering is on hits not in bin X1X are counted as in space points
        if(mySetOfSPIds.find(SCT_Identifier)!=mySetOfSPIds.end()) numberOfHitsFromSPs+=numberOfStrips;
	else if(m_doTimeBinFilteringForNoise && !timeBinInPattern(tbin,XIX)) numberOfHitsFromSPs+=numberOfStrips;
      }
      (*hitsInLayer[systemIndex])[thisElement]+=numberOfStrips;
      // Modified JEGN
      local_tothits+=numberOfStrips;
      maxrodhits+=numberOfStrips;
      // END Modified

      if (m_boolhitmaps) {
        for (unsigned int ichan=chan; ichan!=limit; ++ichan){ 
          (m_phitmapHistoVector[module_hash])->Fill(ichan, 1.0);  //increment channel hit for this plane
        } 
      }	

    } // End of Loop on SCT_Collection, so end of loop over the RDOs in the RDO container
    //We can now do the NO calculation for this wafer
    // Modified JEGN
    // For the Time Dependent plots
    if (maxhits < maxrodhits) { 
      maxhits_Id = theWaferIdentifierOfTheRDOCollection;
      maxhits = maxrodhits;
    }
    if (minhits > maxrodhits) {
      minhits_Id = theWaferIdentifierOfTheRDOCollection;
      minhits = maxrodhits;
    }
    meanhits = local_tothits/totalmodules;
    // END Modified

    if(numberOfHitsFromAllRDOs>0){
      int diff = numberOfHitsFromAllRDOs -  numberOfHitsFromSPs;
      int num(diff);
      int den( N_STRIPS - numberOfHitsFromSPs);
      double sumocc(0.);
      if (diff<0) {
        num = 0 ;
        m_skipEvents++;
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Too many reconstructed space points for number of real hits"<< endreq ;    
      }
      if(den>0){
        sumocc =  num / static_cast<double> (den);
	m_occSumUnbiased[theWaferIdentifierOfTheRDOCollection] += sumocc;
	if(m_environment==AthenaMonManager::online) m_occSumUnbiasedRecent[theWaferIdentifierOfTheRDOCollection] += sumocc;
	if (isSelectedTrigger) m_occSumUnbiasedTrigger[theWaferIdentifierOfTheRDOCollection] += sumocc;
      }
    }
  }// End of Loop on RDO container


  m_ncluHisto->Fill(local_tothits,1);

  if (m_boolhitmaps) {
    for (int i(0); i!=N_BARRELSx2; ++i) {
      m_ncluHistoVector[i]->Fill(barrel_local_nhitslayer[i],1.);
    }
    for (int i(0); i!=N_DISKSx2; ++i) {
      if(m_doPositiveEndcap) m_ncluHistoVectorECp[i]->Fill(ECp_local_nhitslayer[i],1.);
      if(m_doNegativeEndcap) m_ncluHistoVectorECm[i]->Fill(ECm_local_nhitslayer[i],1.);
    }
  }
  // Modified JEGN

  if(m_environment==AthenaMonManager::online){
    // Time Dependent plots only online
    bool badmodule = false;
    nHits_buf[nHits_pos] = meanhits;
    nmaxHits_buf[nHits_pos] = maxhits;
    nmaxModule_buf[nHits_pos] = maxhits_Id;
    nminHits_buf[nHits_pos] = minhits;
    nminModule_buf[nHits_pos] = minhits_Id;
    nHits_pos++;
    if (nHits_pos == m_evtsbins) nHits_pos = 0;
    
    if (m_numberOfEvents % m_checkrate == 0) {
      m_nHits->Reset(); m_nmaxHits->Reset(); Int_t latest_nHits_pos = nHits_pos;m_nminHits->Reset();
      // Check if the same module is the noisiest one for at least 10 events
      //Need to add a similar check for modules with the minimum number of hits										  
      if (nHits_pos > 10 ) {
	for ( int s = 10; s > 0; --s ) {
	  int npos = nHits_pos - s;
	  if (nmaxModule_buf[npos] == nmaxModule_buf[npos - 1]) badmodule = true;
	  else badmodule = false;
	}
      } else if ( nHits_pos < 10 && m_numberOfEvents > m_evtsbins) {
	for ( int s = 10; s > 0; --s ) {
	  int npos  = nHits_pos - s; 
	  int nppos = nHits_pos - s - 1; 
	  if (npos < 0) npos = m_evtsbins + npos;
	  //copy-and-paste error? coverity id 29910
	  if (nppos < 0) nppos = m_evtsbins + nppos;
	  if (nmaxModule_buf[npos] == nmaxModule_buf[nppos]) badmodule = true;
	  else badmodule = false;
	}
      }
      // Fill the histograms with the values from the buffer
      for (Int_t i=1; i != m_evtsbins; ++i) {
	if (latest_nHits_pos == m_evtsbins) latest_nHits_pos = 0;
	
	if (m_numberOfEvents < m_evtsbins){
	  if (i < nHits_pos) {
	    m_nHits->SetBinContent(i,nHits_buf[i]);
	    m_nmaxHits->SetBinContent(i,nmaxHits_buf[i]);
	    m_nminHits->SetBinContent(i,nminHits_buf[i]);
	  }
	  else {
	    m_nHits->SetBinContent(i,0);
	    m_nmaxHits->SetBinContent(i,0);
	    m_nminHits->SetBinContent(i,0);
	  }
	} else {
	  // Fill average histo
	  m_nHits->SetBinContent(i,nHits_buf[latest_nHits_pos]);
	  m_nHits->GetXaxis()->Set(m_evtsbins,m_numberOfEvents-m_evtsbins,m_numberOfEvents);
	  // Fill max histo
	  m_nmaxHits->SetBinContent(i,nmaxHits_buf[latest_nHits_pos]);
	  m_nmaxHits->GetXaxis()->Set(m_evtsbins,m_numberOfEvents-m_evtsbins,m_numberOfEvents);
	  // Fill min histo
	  m_nminHits->SetBinContent(i,nminHits_buf[latest_nHits_pos]);
	  m_nminHits->GetXaxis()->Set(m_evtsbins,m_numberOfEvents-m_evtsbins,m_numberOfEvents);
	  // Change color of the Line if found a noisy module
	  // Need to add similar system for minimum histogram
	  if (badmodule)  m_nmaxHits->SetFillColor(2);
	  else  m_nmaxHits->SetLineColor(0);
	}
	latest_nHits_pos++; if (latest_nHits_pos == m_evtsbins) latest_nHits_pos = 0;
      }
    }
  }
  

  //if(m_environment!=AthenaMonManager::online){ // Uncomment this line to turn off cluster hists in online
  //Fill Cluster size histogram
  const InDet::SCT_ClusterContainer* p_clucontainer;
  StatusCode sc= evtStore()->retrieve(p_clucontainer,"SCT_Clusters");
  if(sc.isFailure())if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Couldn't retrieve clusters"<< endreq ; 
  InDet::SCT_ClusterContainer::const_iterator clucol_it  = p_clucontainer->begin();
  InDet::SCT_ClusterContainer::const_iterator lastcluCol  = p_clucontainer->end();
  for( ; clucol_it!= lastcluCol ; ++clucol_it){
    const InDet::SCT_ClusterCollection* SCT_Collection(*clucol_it);
    if (!SCT_Collection) continue; // select only SCT RDOs
    //Middle Loop on SCT Clusters only
    DataVector<InDet::SCT_Cluster>::const_iterator p_clu(SCT_Collection->begin());
    DataVector<InDet::SCT_Cluster>::const_iterator clus_end = SCT_Collection->end();
    for(; p_clu!=clus_end ; ++p_clu){
      Identifier cluId = (*p_clu)->identify();
      const InDet::SCT_Cluster& cluster = **p_clu;
      int GroupSize = cluster.rdoList().size();
      // Fill  Cluster Size histogram
      int elementIndex=2*m_pSCTHelper->layer_disk(cluId)+m_pSCTHelper->side(cluId);
      int thisPhi       = m_pSCTHelper->phi_module(cluId);
      int thisEta       = m_pSCTHelper->eta_module(cluId);
      m_clusize->Fill( GroupSize,1.);
      if(m_environment==AthenaMonManager::online) m_clusizeRecent->Fill( GroupSize,1.);
      if (m_pSCTHelper->barrel_ec(cluId)==BARREL) {
	m_clusizeHistoVector[elementIndex]->Fill(GroupSize,1.);
	m_clusizedist[elementIndex]->Fill(thisEta,thisPhi,GroupSize);
	if(m_environment==AthenaMonManager::online) m_clusizeHistoVectorRecent[elementIndex]->Fill(GroupSize,1.);
      }
      // JAF ADDED
      else if (m_doPositiveEndcap && m_pSCTHelper->barrel_ec(cluId)==ENDCAP_A) { 
	m_clusizeHistoVectorECp[elementIndex]->Fill(GroupSize,1.);
	m_clusizedistECp[elementIndex]->Fill(thisEta,thisPhi,GroupSize);
	if(m_environment==AthenaMonManager::online) m_clusizeHistoVectorRecentECp[elementIndex]->Fill(GroupSize,1.);
      } else if (m_doNegativeEndcap && m_pSCTHelper->barrel_ec(cluId)==ENDCAP_C) { 
	m_clusizeHistoVectorECm[elementIndex]->Fill(GroupSize,1.);
	m_clusizedistECm[elementIndex]->Fill(thisEta,thisPhi,GroupSize);
	if(m_environment==AthenaMonManager::online) m_clusizeHistoVectorRecentECm[elementIndex]->Fill(GroupSize,1.);
      }
    }
  }

  //} // Uncomment this line as well to turn off cluster hists in online
  
  if ( (m_numberOfEvents% m_checkrate) == 0 ) {
    if (checkHists(false).isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in checkHists(false)" << endreq ;
    }
  }

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                          SCTHitsNoiseMonTool :: book1DHitHists
// Book HitMaps histograms for all present SCT modules
// using ToolRootHistSvc()
// The histograms go to the subdirectory ../hitmaps/
// to improve readability
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::book1DHitHists(){ 
//SCTHitsNoiseMonTool::book1DHitHists(bool isNewRun){ 
  if( newRunFlag() ) {
    // Pointers are deleted by regHist() method
    m_phitmapHistoVector.clear();
    std::string stem;
    MonGroup BarrelHitHists(this, "SCT/SCTB/hitmaps",ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    MonGroup PlusECHitHists(this, "SCT/SCTEA/hitmaps",ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    MonGroup MinusECHitHists(this, "SCT/SCTEC/hitmaps",ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    //
    SCT_ID::const_id_iterator planeIterator(m_pSCTHelper->wafer_begin());
    SCT_ID::const_id_iterator planeEnd = m_pSCTHelper->wafer_end();
    for(;planeIterator not_eq planeEnd;++planeIterator){
      Identifier  planeId(*planeIterator);
      const int bec(m_pSCTHelper->barrel_ec(planeId));
      const string formattedPosition( positionString(*planeIterator) );
      std::string histotitle = string("SCT ") + names[bec2Index(bec)] +string(" Hitmap: plane ") + formattedPosition;
      if (bec==BARREL){
        h1Factory(formattedPosition, histotitle,BarrelHitHists, m_phitmapHistoVector,FIRST_STRIP-0.5,LAST_STRIP+0.5,N_BINS);
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Have registered the barrel hists"<<endreq;
      } else if(m_doPositiveEndcap && bec==ENDCAP_A){
        h1Factory(formattedPosition, histotitle,PlusECHitHists, m_phitmapHistoVector,FIRST_STRIP-0.5,LAST_STRIP+0.5,N_BINS);
      }else if(m_doNegativeEndcap && bec==ENDCAP_C){
        h1Factory(formattedPosition, histotitle,MinusECHitHists, m_phitmapHistoVector,FIRST_STRIP-0.5,LAST_STRIP+0.5,N_BINS);
      }
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Have registered the hists"<<endreq;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode 
SCTHitsNoiseMonTool::bookGeneralHits( const unsigned int systemIndex) {
  //SCTHitsNoiseMonTool::bookGeneralHits(bool isNewRun,bool isNewLumiBlock, const unsigned int systemIndex) {
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));
  if (bec==INVALID_SYSTEM){
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endreq;
    return StatusCode::FAILURE;
  }
  const string paths[]={"SCT/SCTEC/hits","SCT/SCTB/hits", "SCT/SCTEA/hits"};
  const unsigned int limits[]={N_DISKSx2, N_BARRELSx2, N_DISKSx2};
  //
  //  if( isNewLumiBlock) {
  if( newLumiBlockFlag()) {
    MonGroup lumiHits( this,paths[systemIndex],lumiBlock,ATTRIB_UNMANAGED);
    switch (bec){
     case ENDCAP_C:{
       m_numHitsPerLumiBlockECm = h1Factory("numHitsPerLBECm","Number of Endcap C hits in a luminosity block",lumiHits, 0., N_DISKS, N_DISKS);
       m_numHitsPerLumiBlockECm -> GetXaxis()->SetTitle("Disk");
       m_numHitsPerLumiBlockECm -> GetYaxis()->SetTitle("Num of Hits Per LumiBlock");
       m_numSPPerLumiBlockECm = h1Factory("numSPPerLBECm","Number of Endcap C spacepoints in a luminosity block",lumiHits,0., N_DISKS, N_DISKS);
       m_numSPPerLumiBlockECm -> GetXaxis()->SetTitle("Disk");
       m_numSPPerLumiBlockECm -> GetYaxis()->SetTitle("Num of SpacePoint Per LumiBlock");
       break;
     }
     case BARREL:{
       m_numBarrelHitsPerLumiBlock=h1Factory("numBarrelHitsPerLB","Number of barrel hits in a luminosity block", lumiHits, 0., N_BARRELS, N_BARRELS);
       m_numBarrelHitsPerLumiBlock -> GetXaxis()->SetTitle("Layer");
       m_numBarrelHitsPerLumiBlock -> GetYaxis()->SetTitle("Num of Hits Per LumiBlock");
       m_numBarrelSPPerLumiBlock=h1Factory("numBarrelSPPerLB","Number of barrel spacepoints in a luminosity block", lumiHits, 0., N_BARRELS, N_BARRELS);
       m_numBarrelSPPerLumiBlock -> GetXaxis()->SetTitle("Layer");
       m_numBarrelSPPerLumiBlock -> GetYaxis()->SetTitle("Num of SpacePoint Per LumiBlock");
       break;
     }
     case ENDCAP_A:{
       m_numHitsPerLumiBlockECp = h1Factory("numHitsPerLBECp","Number of Endcap A hits in a luminosity block",lumiHits,0., N_DISKS, N_DISKS);
       m_numHitsPerLumiBlockECp -> GetXaxis()->SetTitle("Disk");
       m_numHitsPerLumiBlockECp -> GetYaxis()->SetTitle("Num of Hits Per LumiBlock");
       m_numSPPerLumiBlockECp = h1Factory("numSPPerLBECp","Number of Endcap A spacepoints in a luminosity block",lumiHits,0., N_DISKS, N_DISKS);
       m_numSPPerLumiBlockECp -> GetXaxis()->SetTitle("Disk");
       m_numSPPerLumiBlockECp -> GetYaxis()->SetTitle("Num of SpacePoint Per LumiBlock");

       break;
     }
     default:{
       msg(MSG::FATAL) << "Invalid barrel/endcap indicator, should be -2, 0 or 2, was " << bec << endreq;
       return StatusCode::FAILURE;
       break; 
     }
    }
  }
  //  if (isNewRun){
  if (newRunFlag()){
    VecH2_t * hitsArray[]={&m_phitsHistoVectorECm, &m_phitsHistoVector, &m_phitsHistoVectorECp};
    VecH2_t * hitsArrayRecent[]={&m_phitsHistoVectorRecentECm, &m_phitsHistoVectorRecent, &m_phitsHistoVectorRecentECp};
    VecH1D_t * nClustersArray[]={&m_ncluHistoVectorECm, &m_ncluHistoVector, &m_ncluHistoVectorECp};
    (hitsArray[systemIndex])->clear();
    (hitsArrayRecent[systemIndex])->clear();
    (nClustersArray[systemIndex])->clear();
    MonGroup hitHists( this,paths[systemIndex],run,ATTRIB_UNMANAGED);
    for (unsigned int i(0); i!=limits[systemIndex];++i) {
      //      LayerSideFormatter layerSide(i);//30.11.2014
      LayerSideFormatter layerSide(i,systemIndex);//30.11.2014
        string streamhitmap ="hitsmap"+abbreviations[systemIndex]+"_"+ layerSide.name() ;
	string streamhitmaprecent ="hitsmaprecent"+abbreviations[systemIndex]+"_"+ layerSide.name() ;
        string streamhits ="hits"+abbreviations[systemIndex]+"_"+ layerSide.name() ;
        std::string histotitle="SCT Hitmap for "+names[systemIndex]+": "+layerSide.title();
	std::string histotitlerecent="SCT Hitmap from recent events for "+names[systemIndex]+": "+layerSide.title();
        h2Factory(streamhitmap, histotitle, bec, hitHists, *(hitsArray[systemIndex]));
	if(m_environment==AthenaMonManager::online) h2Factory(streamhitmaprecent, histotitlerecent, bec, hitHists, *(hitsArrayRecent[systemIndex]));
        histotitle="SCT Hits for "+names[systemIndex]+": "+layerSide.title();
        if (m_boolhitmaps) h1DFactory(streamhits,histotitle,hitHists, *(nClustersArray[systemIndex]),FIRST_HIT_BIN,LAST_HIT_BIN,N_HIT_BINS);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode 
SCTHitsNoiseMonTool::bookClusterSize(){
//SCTHitsNoiseMonTool::bookClusterSize(bool isNewRun){
  MonGroup clusizebGroup(this,"SCT/SCTB/hits",run,ATTRIB_UNMANAGED);
  MonGroup clusizeeaGroup(this,"SCT/SCTEA/hits",run,ATTRIB_UNMANAGED);
  MonGroup clusizeecGroup(this,"SCT/SCTEC/hits",run,ATTRIB_UNMANAGED);
  std::string disksidenameB[] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1"};
  std::string disksidenameECp[] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
  std::string disksidenameECm[] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
  for(int i=0;i<8;i++){
    m_clusizedist[i]=prof2DFactory("clusize_dist_B_"+disksidenameB[i],"cluster size in Barrel_"+disksidenameB[i],clusizebGroup,13,-6.5,6.5,56,-0.5,55.5);
  }
  for(int i=0;i<18;i++){
    m_clusizedistECp[i]=prof2DFactory("clusize_dist_EA_"+disksidenameECp[i],"cluster size in EndcapA_"+disksidenameECp[i],clusizeeaGroup,3,-0.5,2.5,52,-0.5,51.5);
    m_clusizedistECm[i]=prof2DFactory("clusize_dist_EC_"+disksidenameECm[i],"cluster size in EndcapC_"+disksidenameECm[i],clusizeecGroup,3,-0.5,2.5,52,-0.5,51.5);
  }
  //  if (isNewRun){
  if (newRunFlag()){
    MonGroup BarrelCluSize(this,"SCT/GENERAL/hits",run,ATTRIB_UNMANAGED );
    //book Cluster width histogram for all SCT Detector
    m_clusize=h1DFactory("clu_size","SCT Cluster size",BarrelCluSize,0.,200.,200);
    m_clusize->GetXaxis()->SetTitle("Cluster Size");
    m_clusize->GetYaxis()->SetTitle("Num of Events");
    for(int i=0;i<N_BARRELSx2;i++){
      m_clusizeHistoVector[i]->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeHistoVector[i]->GetYaxis()->SetTitle("Num of Events");
    }
    for(int m=0;m<N_DISKSx2;m++){
      m_clusizeHistoVectorECp[m]->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeHistoVectorECp[m]->GetYaxis()->SetTitle("Num of Events");
      m_clusizeHistoVectorECm[m]->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeHistoVectorECm[m]->GetYaxis()->SetTitle("Num of Events");

    }

    if(m_environment==AthenaMonManager::online) {
      m_clusizeRecent=h1DFactory("clu_size_recent","SCT Cluster size from recent events",BarrelCluSize,0.,200.,200);
      m_clusizeRecent->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeRecent->GetYaxis()->SetTitle("Num of Events");
      for(int ii=0;ii<N_BARRELSx2;ii++){
	m_clusizeHistoVectorRecent[ii]->GetXaxis()->SetTitle("Cluster Size");
	m_clusizeHistoVectorRecent[ii]->GetYaxis()->SetTitle("Num of Events");
      }
      for(int mm=0;mm<N_DISKSx2;mm++){
	m_clusizeHistoVectorRecentECp[mm]->GetXaxis()->SetTitle("Cluster Size");
	m_clusizeHistoVectorRecentECp[mm]->GetYaxis()->SetTitle("Num of Events");
	m_clusizeHistoVectorRecentECm[mm]->GetXaxis()->SetTitle("Cluster Size");
	m_clusizeHistoVectorRecentECm[mm]->GetYaxis()->SetTitle("Num of Events");
      }
    }
  }
  return m_clusize?(StatusCode::SUCCESS):(StatusCode::FAILURE);
}

StatusCode 
SCTHitsNoiseMonTool::bookGeneralCluSize(const unsigned int systemIndex){
  //SCTHitsNoiseMonTool::bookGeneralCluSize(bool isNewRun, const unsigned int systemIndex){
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));
  VecH1D_t * clusterSizeArray[]={&m_clusizeHistoVectorECm, &m_clusizeHistoVector, &m_clusizeHistoVectorECp};
  VecH1D_t * clusterSizeArrayRecent[]={&m_clusizeHistoVectorRecentECm, &m_clusizeHistoVectorRecent, &m_clusizeHistoVectorRecentECp};

  


  const string paths[]={"SCT/SCTEC/hits","SCT/SCTB/hits", "SCT/SCTEA/hits"};
  const unsigned int limits[]={N_DISKSx2, N_BARRELSx2, N_DISKSx2};
  if (bec==INVALID_SYSTEM){
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endreq;
    return StatusCode::FAILURE;
  }
  VecH1D_t & clusterSizeVector=*(clusterSizeArray[systemIndex]);
  VecH1D_t & clusterSizeVectorRecent=*(clusterSizeArrayRecent[systemIndex]);
  //  if (isNewRun){
  if (newRunFlag()){
    clusterSizeVector.clear();
    clusterSizeVectorRecent.clear();
    MonGroup clusterSize(this, paths[systemIndex], run,ATTRIB_UNMANAGED);
    for (unsigned int i(0); i!=limits[systemIndex];++i) {
      LayerSideFormatter layerSide(i,systemIndex);
      const string streamclusize ="clusize"+abbreviations[systemIndex]+"_"+layerSide.name();
      std::string histotitle="SCT "+names[systemIndex]+" Cluster size: "+layerSide.title();
      h1DFactory(streamclusize, histotitle, clusterSize, clusterSizeVector, 0., 200., 200);
      //h1Factory(streamclusize,histotitle);
      //clusterSizeVector[systemIndex]->GetXaxis()->SetTitle("Cluster Size");
      //clusterSizeVector[systemIndex]->GetYaxis()->SetTitle("Num of Events");
      if(m_environment==AthenaMonManager::online) {
	const string streamclusizerecent ="clusize_recent"+abbreviations[systemIndex]+"_"+layerSide.name();
	std::string histotitlerecent="SCT "+names[systemIndex]+" Cluster size from recent events: "+layerSide.title();
	h1DFactory(streamclusizerecent, histotitlerecent, clusterSize, clusterSizeVectorRecent, 0., 200., 200);
	//clusterSizeVectorRecent[systemIndex]->GetXaxis()->SetTitle("Cluster Size");
	//clusterSizeVectorRecent[systemIndex]->GetYaxis()->SetTitle("Num of Events");
      }
    }

    //m_clusizeHistoVector[1]->GetXaxis()->SetTitle("Cluster Size");
  }
  return StatusCode::SUCCESS;
}

StatusCode  
SCTHitsNoiseMonTool::bookGeneralNoiseOccupancyMaps(const unsigned int systemIndex){
  //SCTHitsNoiseMonTool::bookGeneralNoiseOccupancyMaps(bool isNewRun, const unsigned int systemIndex){
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));
  if (bec==INVALID_SYSTEM){
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endreq;
    return StatusCode::FAILURE;
  }
  //  if(isNewRun){
  if(newRunFlag()){
    const string paths[]={"SCT/SCTEC/Noise", "SCT/SCTB/Noise", "SCT/SCTEA/Noise"};
    const unsigned int limits[]={N_DISKSx2, N_BARRELSx2, N_DISKSx2};
    VecProf2_t * storageVectors[]={&m_pnoiseoccupancymapHistoVectorECm, &m_pnoiseoccupancymapHistoVector, &m_pnoiseoccupancymapHistoVectorECp};
    VecProf2_t * storageVectorsTrigger[]={&m_pnoiseoccupancymapHistoVectorECmTrigger, &m_pnoiseoccupancymapHistoVectorTrigger, &m_pnoiseoccupancymapHistoVectorECpTrigger};
    VecProf2_t * storageVectorsRecent[]={&m_pnoiseoccupancymapHistoVectorECmRecent, &m_pnoiseoccupancymapHistoVectorRecent, &m_pnoiseoccupancymapHistoVectorECpRecent};

    MonGroup noiseOccMaps(this,paths[systemIndex],run,ATTRIB_UNMANAGED);
    (storageVectors[systemIndex])->clear();
    (storageVectorsTrigger[systemIndex])->clear();
    (storageVectorsRecent[systemIndex])->clear();
    //book 2D "noise" maps, containing hits that aren't associated to tracks
    for (unsigned int i=0; i!=limits[systemIndex];++i) {
      LayerSideFormatter layerSide(i,systemIndex);
      const string streamhitmap ="noiseoccupancymap"+abbreviations[systemIndex]+"_"+layerSide.name();
      const string streamhitmaptrigger ="noiseoccupancymaptrigger"+abbreviations[systemIndex]+"_"+layerSide.name();
      const string streamhitmaprecent ="noiseoccupancymaprecent"+abbreviations[systemIndex]+"_"+layerSide.name();
      std::string histotitle="SCT Noise Occupancy map for "+names[systemIndex]+": "+layerSide.title();
      std::string histotitletrigger="SCT Noise Occupancy map for "+m_NOTrigger+" Trigger and "+names[systemIndex]+": "+layerSide.title();
      std::string histotitlerecent="SCT Noise Occupancy map in recent events for "+names[systemIndex]+": "+layerSide.title();
      prof2Factory(streamhitmap,histotitle, bec, noiseOccMaps,*(storageVectors[systemIndex]));
      prof2Factory(streamhitmaptrigger,histotitletrigger, bec, noiseOccMaps,*(storageVectorsTrigger[systemIndex]));
      if(m_environment==AthenaMonManager::online) prof2Factory(streamhitmaprecent,histotitlerecent, bec, noiseOccMaps,*(storageVectorsRecent[systemIndex]));
    }
  }
  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: checkNoiseMaps
//LF 13/01/2006
//Here we have the check of the histograms and we can also build
//efficiency, occupancy, rms histograms, etc from the basic ones
//====================================================================================================
StatusCode  
SCTHitsNoiseMonTool::checkNoiseMaps(bool final){
  IdentifierHash next;
  std::vector<float> vectorOfOccupancies;
  if(final) {
    if(m_doSpacePointBasedNoise){
      //CAM Added code for track and SP algorithms
      ////////////////////////////////////////////////////////
      if(m_environment==AthenaMonManager::online){ // Reset histos for online so there is proper weighting of events
	if ( resetNoiseMapHists().isFailure() ) msg(MSG::WARNING) << "Failed to reset noise occupancy maps!" << endreq;
      }
      if(m_occSumUnbiased.size() && m_numberOfEvents){
        for (std::map<Identifier, double>::iterator it = m_occSumUnbiased.begin(); it != m_occSumUnbiased.end(); it++){
          Identifier wafer_id = it->first;
          int element=2*m_pSCTHelper->layer_disk(wafer_id)+m_pSCTHelper->side(wafer_id);
          double occ = (m_numberOfEvents > 0) ? it->second/(m_numberOfEvents) :  it->second;	  
          int eta = m_pSCTHelper->eta_module(wafer_id);
          int phi = m_pSCTHelper->phi_module(wafer_id);
          int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);

          if(barrel_ec==BARREL){
            m_pnoiseoccupancymapHistoVector[element]->Fill(eta,phi,occ*1E5);
	    m_barrelNOdistribution->Fill(occ*1E5);
            m_SCTNOdistribution->Fill(occ*1E5);
          }
          if(m_doPositiveEndcap && barrel_ec==ENDCAP_A){
            m_pnoiseoccupancymapHistoVectorECp[element]->Fill(eta,phi,occ*1E5);
	    m_ECpNOdistribution->Fill(occ*1E5);
            m_SCTNOdistribution->Fill(occ*1E5);
          }
          if(m_doNegativeEndcap && barrel_ec==ENDCAP_C){
            m_pnoiseoccupancymapHistoVectorECm[element]->Fill(eta,phi,occ*1E5);
	    m_ECmNOdistribution->Fill(occ*1E5);
            m_SCTNOdistribution->Fill(occ*1E5);
          }
        }
      }
      if(m_environment==AthenaMonManager::online) {
	if(m_occSumUnbiasedRecent.size() && m_numberOfEventsRecent){
	  for (std::map<Identifier, double>::iterator it = m_occSumUnbiasedRecent.begin(); it != m_occSumUnbiasedRecent.end(); it++){
	    Identifier wafer_id = it->first;
	    int element=2*m_pSCTHelper->layer_disk(wafer_id)+m_pSCTHelper->side(wafer_id);
	    double occ = (m_numberOfEventsRecent > 0) ? it->second/(m_numberOfEventsRecent) :  it->second;	  
	    int eta = m_pSCTHelper->eta_module(wafer_id);
	    int phi = m_pSCTHelper->phi_module(wafer_id);
	    int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);
	    
	    if(barrel_ec==BARREL){
	      m_pnoiseoccupancymapHistoVectorRecent[element]->Fill(eta,phi,occ*1E5);
	    }
	    if(m_doPositiveEndcap && barrel_ec==ENDCAP_A){
	      m_pnoiseoccupancymapHistoVectorECpRecent[element]->Fill(eta,phi,occ*1E5);
	    }
	    if(m_doNegativeEndcap && barrel_ec==ENDCAP_C){
	      m_pnoiseoccupancymapHistoVectorECmRecent[element]->Fill(eta,phi,occ*1E5);
	    }
	  }
	}
      }
      if(m_occSumUnbiasedTrigger.size() && m_numberOfEventsTrigger){ 

	for (std::map<Identifier, double>::iterator it = m_occSumUnbiasedTrigger.begin(); it != m_occSumUnbiasedTrigger.end(); it++){	
	  Identifier wafer_id = it->first;
          int element=2*m_pSCTHelper->layer_disk(wafer_id)+m_pSCTHelper->side(wafer_id);
          double occ = (m_numberOfEventsTrigger > 0) ? it->second/(m_numberOfEventsTrigger) :  it->second;	  
          int eta = m_pSCTHelper->eta_module(wafer_id);
          int phi = m_pSCTHelper->phi_module(wafer_id);
          int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);	  

	  if(barrel_ec==BARREL) {
            m_pnoiseoccupancymapHistoVectorTrigger[element]->Fill(eta,phi,occ*1E5);
	    m_barrelNOdistributionTrigger->Fill(occ*1E5);
            m_SCTNOdistributionTrigger->Fill(occ*1E5);
	  }
          if(m_doPositiveEndcap && barrel_ec==ENDCAP_A) {
            m_pnoiseoccupancymapHistoVectorECpTrigger[element]->Fill(eta,phi,occ*1E5);
	    m_ECpNOdistributionTrigger->Fill(occ*1E5);
            m_SCTNOdistributionTrigger->Fill(occ*1E5);
	  }
          if(m_doNegativeEndcap && barrel_ec==ENDCAP_C) {
            m_pnoiseoccupancymapHistoVectorECmTrigger[element]->Fill(eta,phi,occ*1E5);
	    m_ECmNOdistributionTrigger->Fill(occ*1E5);
            m_SCTNOdistributionTrigger->Fill(occ*1E5);
	  }
	}
      }    
    }
  }
  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: initializeNoiseMaps
//DD 12/06/2011
//Here we initialize the noise occupancy maps to avoid a bug that is present
//when adding TProfiles when no noise hit has occured in a module side
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::initializeNoiseMaps() {

  for(SCT_ID::const_id_iterator planeIterator=m_pSCTHelper->wafer_begin();planeIterator!=m_pSCTHelper->wafer_end();++planeIterator){
    // Don't initialize a value for disabled  modules
    if( ! m_ConfigurationSvc->isGood(*planeIterator, InDetConditions::SCT_SIDE) ) continue;
    m_occSumUnbiased[*planeIterator] = 0.;
    m_occSumUnbiasedTrigger[*planeIterator] = 0.;
    m_occSumUnbiasedRecent[*planeIterator] = 0.;
  }

  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: resetNoiseMapHists
//DD 12/06/2011
//Resets entries for all the noise occupancy histograms
//
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::resetNoiseMapHists() {

  if ( resetVecProf2(m_pnoiseoccupancymapHistoVector).isFailure() ) 
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVector!" << endreq;
  if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorECp).isFailure() ) 
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECp!" << endreq;
  if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorECm).isFailure() ) 
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECm!" << endreq;
  if(m_environment==AthenaMonManager::online) {
    if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorRecent).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorRecent!" << endreq;
    if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorECpRecent).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECpRecent!" << endreq;
    if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorECmRecent).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECmRecent!" << endreq;
  }
  if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorTrigger).isFailure() ) 
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorTrigger!" << endreq;
  if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorECpTrigger).isFailure() ) 
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECpTrigger!" << endreq;
  if ( resetVecProf2(m_pnoiseoccupancymapHistoVectorECmTrigger).isFailure() ) 
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECmTrigger!" << endreq;
  m_barrelNOdistribution->Reset();         m_barrelNOdistributionTrigger->Reset();
  m_ECmNOdistribution->Reset();            m_ECmNOdistributionTrigger->Reset();
  m_ECpNOdistribution->Reset();            m_ECpNOdistributionTrigger->Reset();
  m_SCTNOdistribution->Reset();            m_SCTNOdistributionTrigger->Reset();

  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: resetNoiseMapsRecent
//DD 19/07/2011
//Resets all the values in the recent noise occ maps to zero
//
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::resetNoiseMapsRecent() {
  
  for (std::map<Identifier, double>::iterator it = m_occSumUnbiasedRecent.begin(); it != m_occSumUnbiasedRecent.end(); ++it){
    it->second=0.0;
  }
  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: resetHitMapHists
//DD 23/07/2011
//Resets all the values in the recent hit maps to zero
//
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::resetHitMapHists() {
  
  if(m_environment==AthenaMonManager::online) {
    if ( resetVecH2(m_ptrackhitsHistoVectorRecent).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset ptrackhitsHistoVectorRecent!" << endreq;
    if ( resetVecH2(m_ptrackhitsHistoVectorRecentECp).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset ptrackhitsHistoVectorRecentECp!" << endreq;
    if ( resetVecH2(m_ptrackhitsHistoVectorRecentECm).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset ptrackhitsHistoVectorRecentECm!" << endreq;
    if ( resetVecH1D(m_tbinHistoVectorRecent).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset m_tbinHistoVectorRecent!" << endreq;
    if ( resetVecH1D(m_tbinHistoVectorRecentECp).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset m_tbinHistoVectorRecentECp!" << endreq;
    if ( resetVecH1D(m_tbinHistoVectorRecentECm).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset m_tbinHistoVectorRecentECm!" << endreq;
    if ( resetVecH2(m_phitsHistoVectorRecent).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset m_phitsHistoVectorRecent!" << endreq;
    if ( resetVecH2(m_phitsHistoVectorRecentECp).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset m_phitsHistoVectorRecentECp!" << endreq;
    if ( resetVecH2(m_phitsHistoVectorRecentECm).isFailure() ) 
      msg(MSG::WARNING) << "Failed to reset m_phitsHistoVectorRecentECm!" << endreq;
    if ( resetVecH1D(m_clusizeHistoVectorRecent).isFailure() )
      msg(MSG::WARNING) << "Failed to reset m_clusizeHistoVectorRecent!" << endreq;
    if ( resetVecH1D(m_clusizeHistoVectorRecentECp).isFailure() )
      msg(MSG::WARNING) << "Failed to reset m_clusizeHistoVectorRecentECp!" << endreq;
    if ( resetVecH1D(m_clusizeHistoVectorRecentECm).isFailure() )
      msg(MSG::WARNING) << "Failed to reset m_clusizeHistoVectorRecentECm!" << endreq;
    m_tbinHistoRecent->Reset();
    m_tbinHistoRecentECp->Reset();
    m_tbinHistoRecentECm->Reset();
    m_clusizeRecent->Reset();
  }

  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: resetVecProf2
//DD 12/06/2011
//Resets entries for all histograms in a vector of TProfile2Ds
//
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::resetVecProf2(VecProf2_t hists) {

  for(unsigned int i=0;i<hists.size();++i) {
    if( ! hists[i] ) continue;
    hists[i]->Reset();
  }
  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: resetVecProf2
//DD 23/07/2011
//Resets entries for all histograms in a vector of TH2s
//
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::resetVecH2(VecH2_t hists) {

  for(unsigned int i=0;i<hists.size();++i) {
    if( ! hists[i] ) continue;
    hists[i]->Reset();
  }
  return StatusCode::SUCCESS;
}


//====================================================================================================
//                                SCTHitsNoiseMonTool :: resetVecH1D
//DD 23/07/2011
//Resets entries for all histograms in a vector of TH1Ds
//
//====================================================================================================
StatusCode SCTHitsNoiseMonTool::resetVecH1D(VecH1D_t hists) {

  for(unsigned int i=0;i<hists.size();++i) {
    if( ! hists[i] ) continue;
    hists[i]->Reset();
  }
  return StatusCode::SUCCESS;
}


//====================================================================================================
//                           SCTHitsNoiseMonTool :: book1DNoiseHists
// Book noise occupancy histograms for all present SCT modules
// using ToolRootHistSvc()
// The histograms go to the subdirectory ../Modulemaps/
// to improve readability
//
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::bookNoiseDistributions(){
//SCTHitsNoiseMonTool::bookNoiseDistributions(bool isNewRun){

  //  if(isNewRun){
  if(newRunFlag()){
    MonGroup NoiseDistributions(this,"SCT/GENERAL/noise",ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    const Int_t bins = 200; Double_t xmin = 1e-1; Double_t xmax = 500; Double_t logxmin = log10(xmin);
    Double_t logxmax = log10(xmax); Double_t binwidth = (logxmax-logxmin)/bins;
    Double_t xbins[bins+1]; xbins[0] = xmin; 
    if (m_doLogXNoise){
      for (Int_t i=1;i<=bins;++i) { 
        xbins[i] = xmin + TMath::Power(10,logxmin+i*binwidth);
      }
    }

    m_barrelNOdistribution = new TH1F("barrelNOdistribution","NO Distribution for the Barrel",bins,xmin,xmax);
    m_barrelNOdistribution->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_barrelNOdistribution->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_barrelNOdistribution->SetBins(bins,xbins); 
    if(NoiseDistributions.regHist(m_barrelNOdistribution).isFailure()) msg(MSG::WARNING) << "Couldn't book barrelNOdistribution" << endreq;

    std::string barrelNO_title = "NO Distribution for the Barrel for "+m_NOTrigger+" trigger";
    m_barrelNOdistributionTrigger = new TH1F("barrelNOdistributionTrigger",TString(barrelNO_title),bins,xmin,xmax);
    m_barrelNOdistributionTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_barrelNOdistributionTrigger->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_barrelNOdistributionTrigger->SetBins(bins,xbins);
    if(NoiseDistributions.regHist(m_barrelNOdistributionTrigger).isFailure()) msg(MSG::WARNING) << "Couldn't book barrelNOdistributionTrigger" << endreq;

    m_ECmNOdistribution = new TH1F("ECCNOdistribution","NO Distribution for the EndCap C",bins,xmin, xmax);
    m_ECmNOdistribution->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECmNOdistribution->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_ECmNOdistribution->SetBins(bins,xbins);
    if(NoiseDistributions.regHist(m_ECmNOdistribution).isFailure()) msg(MSG::WARNING) << "Couldn't book ECmNOdistribution" << endreq;

    std::string ECmNO_title = "NO Distribution for the EndCap C for "+m_NOTrigger+" trigger";
    m_ECmNOdistributionTrigger = new TH1F("ECCNOdistributionTrigger",TString(ECmNO_title),bins,xmin, xmax);
    m_ECmNOdistributionTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECmNOdistributionTrigger->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_ECmNOdistributionTrigger->SetBins(bins,xbins);
    if(NoiseDistributions.regHist(m_ECmNOdistributionTrigger).isFailure()) msg(MSG::WARNING) << "Couldn't book ECmNOdistributionTrigger" << endreq;

    m_ECpNOdistribution = new TH1F("ECANOdistribution","NO Distribution for the EndCap A",bins,xmin,xmax);
    m_ECpNOdistribution->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECpNOdistribution->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_ECpNOdistribution->SetBins(bins,xbins); 
    if(NoiseDistributions.regHist(m_ECpNOdistribution).isFailure()) msg(MSG::WARNING) << "Couldn't book ECpNOdistribution" << endreq;

    std::string ECpNO_title = "NO Distribution for the EndCap A for "+m_NOTrigger+" trigger";
    m_ECpNOdistributionTrigger = new TH1F("ECANOdistributionTrigger",TString(ECpNO_title),bins,xmin,xmax);
    m_ECpNOdistributionTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECpNOdistributionTrigger->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_ECpNOdistributionTrigger->SetBins(bins,xbins);
    if(NoiseDistributions.regHist(m_ECpNOdistributionTrigger).isFailure()) msg(MSG::WARNING) << "Couldn't book ECpNOdistributionTrigger" << endreq;

    m_SCTNOdistribution = new TH1F("SCTNOdistribution","NO Distribution for the SCT",bins,xmin,xmax);
    m_SCTNOdistribution->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_SCTNOdistribution->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_SCTNOdistribution->SetBins(bins,xbins); 
    if(NoiseDistributions.regHist(m_SCTNOdistribution).isFailure()) msg(MSG::WARNING) << "Couldn't book SCTNOdistribution" << endreq;

    std::string SCTNO_title = "NO Distribution for the SCT for "+m_NOTrigger+" trigger";
    m_SCTNOdistributionTrigger = new TH1F("SCTNOdistributionTrigger",TString(SCTNO_title),bins,xmin,xmax);
    m_SCTNOdistributionTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_SCTNOdistributionTrigger->GetYaxis()->SetTitle("Modules");
    if(m_doLogXNoise) m_SCTNOdistributionTrigger->SetBins(bins,xbins);
    if(NoiseDistributions.regHist(m_SCTNOdistributionTrigger).isFailure()) msg(MSG::WARNING) << "Couldn't book SCTNOdistributionTrigger" << endreq;
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                      SCTHitsNoiseMonTool :: bookSPvsEventNumber
// Book 1D histogram of number of spacepoints vs event number
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::bookSPvsEventNumber(){
  //SCTHitsNoiseMonTool::bookSPvsEventNumber(bool isNewRun){
  // Modified JEGN
  //  if(isNewRun){
  if(newRunFlag()){
    free(nSP_buf);
    free(nHits_buf);
    free(nmaxHits_buf);
    free(nminHits_buf);
    free(nmaxModule_buf);
    free(nminModule_buf);
    MonGroup BarrelSPHist(this,"SCT/GENERAL/hits",ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    //Book a histogram
    m_nSP = h1DFactory("sct_sp_vs_en","Number of Spacepoints vs Event Number",BarrelSPHist,1,m_evtsbins+1,m_evtsbins);
    m_nSP->GetXaxis()->SetTitle("Event Number");
    m_nSP->GetYaxis()->SetTitle("Num of Spacepoints");
    size_t nSP_buf_size;
    nSP_buf_size = m_evtsbins * sizeof (int);
    nSP_buf = (int *) malloc (nSP_buf_size);
    nSP_pos = 0;
   
    m_nHits = h1DFactory("sct_av_hits_vs_en","Number of Average Hits vs Event Number",BarrelSPHist,1,m_evtsbins+1,m_evtsbins);
    m_nHits->GetXaxis()->SetTitle("Event Number");
    m_nHits->GetYaxis()->SetTitle("Num of Average Hits");
    size_t nHits_buf_size;
    nHits_buf_size = m_evtsbins * sizeof (int);
    nHits_buf = (int *) malloc (nHits_buf_size);
    nHits_pos = 0;
    
    m_nmaxHits = h1DFactory("sct_max_hits_vs_en","Max Number of Hits vs Event Number",BarrelSPHist,1,m_evtsbins+1,m_evtsbins);
    m_nmaxHits->GetXaxis()->SetTitle("Event Number");
    m_nmaxHits->GetYaxis()->SetTitle("Num of Max Hits");
    size_t nmaxHits_buf_size;
    nmaxHits_buf_size = m_evtsbins * sizeof (int);
    nmaxHits_buf = (int *) malloc (nmaxHits_buf_size);
    size_t nmaxModule_buf_size;
    nmaxModule_buf_size = m_evtsbins * sizeof (Identifier);
    nmaxModule_buf = (Identifier *) malloc (nmaxModule_buf_size);
    
    m_nminHits = h1DFactory("sct_min_hits_vs_en","Min Number of Hits vs Event Number",BarrelSPHist,1,m_evtsbins+1,m_evtsbins);
    m_nminHits->GetXaxis()->SetTitle("Event Number");
    m_nminHits->GetYaxis()->SetTitle("Num of Min Hits");
    size_t nminHits_buf_size;
    nminHits_buf_size = m_evtsbins * sizeof (int);
    nminHits_buf = (int *) malloc (nminHits_buf_size);
    size_t nminModule_buf_size;
    nminModule_buf_size = m_evtsbins * sizeof (Identifier);
    nminModule_buf = (Identifier *) malloc (nminModule_buf_size);
  }
  // END Modified
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                         SCTHitsNoiseMonTool :: makeSPvsEventNumber
//====================================================================================================
StatusCode 
SCTHitsNoiseMonTool::makeSPvsEventNumber(){
  //Retrieve the spacepoint collection
  StatusCode sc;
  const SpacePointContainer* m_SCT_spcontainer;
  // get space points for SCT from TDS
  sc = evtStore()->retrieve(m_SCT_spcontainer, "SCT_SpacePoints");
  if (sc.isFailure() || !m_SCT_spcontainer){
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Si SpacePoint container for SCT not found"<< endreq;
    return StatusCode::FAILURE;
  }
  int m_sct_nspacepoints(0);
  //loop over SCT space points collections
  SpacePointContainer::const_iterator it=m_SCT_spcontainer->begin();
  SpacePointContainer::const_iterator endit = m_SCT_spcontainer->end();
  for(;it!=endit; ++it) {
    const SpacePointCollection *colNext=&(**it);
    if(!colNext) continue;
    //Identify the spacepoint collection and gets its barrel_ec to update the num SP histogram
    int thisBec = m_pSCTHelper->barrel_ec(colNext->identify());
    int thisLayerDisk = m_pSCTHelper->layer_disk(colNext->identify());
    if(thisBec==0)m_numBarrelSPPerLumiBlock->Fill(thisLayerDisk,colNext->size());
    if(thisBec==-2)m_numSPPerLumiBlockECm->Fill(thisLayerDisk,colNext->size());
    if(thisBec==2)m_numSPPerLumiBlockECp->Fill(thisLayerDisk,colNext->size());
    m_sct_nspacepoints += (int)colNext->size();
  }
  // Modified JEGN


  if(m_environment==AthenaMonManager::online){
    // Time Dependent SP plots only online
    nSP_buf[nSP_pos] = m_sct_nspacepoints;
    nSP_pos++;
    if (nSP_pos == m_evtsbins) nSP_pos = 0;
    if (m_numberOfEvents % m_checkrate == 0) {
      m_nSP->Reset(); Int_t latest_nSP_pos = nSP_pos;
      for (Int_t i=1; i < m_evtsbins; i++) {
	if (latest_nSP_pos == m_evtsbins) latest_nSP_pos = 0;
	if (m_numberOfEvents < m_evtsbins){
	  if (i < nSP_pos)
	    m_nSP->SetBinContent(i,nSP_buf[i]);
	  else
	    m_nSP->SetBinContent(i,0);
	} else {
	  m_nSP->SetBinContent(i,nSP_buf[latest_nSP_pos]);
	  m_nSP->GetXaxis()->Set(m_evtsbins,m_numberOfEvents-m_evtsbins,m_numberOfEvents);
	}
	latest_nSP_pos++; if (latest_nSP_pos == m_evtsbins) latest_nSP_pos = 0;
      }
    }
  }

// END Modified
  return StatusCode::SUCCESS;
}

std::string
SCTHitsNoiseMonTool::positionString(const Identifier & plane) const{
  ostringstream position_txt;
  position_txt << m_pSCTHelper->barrel_ec(plane) <<"_"<<m_pSCTHelper->layer_disk(plane)<<"_"<< m_pSCTHelper->phi_module(plane)<<"_"<<m_pSCTHelper->eta_module(plane)<<"_"<<m_pSCTHelper->side(plane);
  return position_txt.str();
}

StatusCode SCTHitsNoiseMonTool::makeVectorOfTrackRDOIdentifiers(){
  StatusCode sc;
  //Clear the RDOsOnTracks vector
  m_RDOsOnTracks.clear();
  const SCT_RDO_Container* p_rdocontainer;
  sc = evtStore()->retrieve(p_rdocontainer,m_dataObjectName);
  if (sc.isFailure() || !p_rdocontainer) {
    msg(MSG::FATAL) << "Could not find the data object " << m_dataObjectName << " !" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Data object " << m_dataObjectName << " found" << endreq;
  }

  sc = evtStore()->retrieve(m_tracks,m_tracksName);
  if (sc.isFailure() ) {
    msg(MSG::FATAL) <<"No tracks for you!"<<endreq;
    return sc;
  }
  // Only do for events with less than some number of tracks
  if(m_tracks->size() > m_maxTracks) {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "The event has more than " << m_maxTracks
		      << " tracks. Don't do hits-on-track-hists" << endreq;
    }
    return StatusCode::SUCCESS;
  }
  //assemble list of rdo ids associated with tracks
  for (int i=0; i<(int)m_tracks->size(); i++) { 
    const Trk::Track *track=(*m_tracks)[i];
    if (track == 0) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "no pointer to track!!!" << endreq;
      break;
    }
    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();
    if (trackStates == 0){if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "for current track is TrackStateOnSurfaces == Null, no data will be written for this track" << endreq;}
    else {// Loop over all track states on surfaces
      for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin(); it!=trackStates->end(); it++) {
        // Get pointer to RIO of right type
        const InDet::SiClusterOnTrack *clus =dynamic_cast<const InDet::SiClusterOnTrack*>((*it)->measurementOnTrack());
        if (clus){
          // Get Pointer to prepRawDataObject
          const InDet::SiCluster *RawDataClus= dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
          if (RawDataClus==0){
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "SiCluster WITHOUT prepRawData!!!!" << endreq;
            break;
          }
          // if Cluster is in SCT ...
          if (RawDataClus->detectorElement()->isSCT()) {
            const std::vector<Identifier>& rdoList = RawDataClus->rdoList();
            m_RDOsOnTracks.insert(m_RDOsOnTracks.end(),rdoList.begin(),rdoList.end());
          }
        }
      }
    }
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"JEGN!! num RDOs on tracks is "<<m_RDOsOnTracks.size()<<endreq;
  return StatusCode::SUCCESS;
}

//StatusCode SCTHitsNoiseMonTool::bookGeneralTrackHits(bool isNewRun, const unsigned int systemIndex){
StatusCode SCTHitsNoiseMonTool::bookGeneralTrackHits(const unsigned int systemIndex){
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));
  if (bec==INVALID_SYSTEM){
     msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endreq;
     return StatusCode::FAILURE;
  }
  const string paths[]={"SCT/SCTEC/hits/", "SCT/SCTB/hits/", "SCT/SCTEA/hits/"};
  const unsigned int limits[]={N_DISKSx2, N_BARRELSx2, N_DISKSx2};
  VecH2_t * histoVec[]={&m_ptrackhitsHistoVectorECm, &m_ptrackhitsHistoVector, &m_ptrackhitsHistoVectorECp};
  VecH2_t * histoVecRecent[]={&m_ptrackhitsHistoVectorRecentECm, &m_ptrackhitsHistoVectorRecent, &m_ptrackhitsHistoVectorRecentECp};
  string stem=m_stream+"/"+paths[systemIndex]+"mapsOfHitsOnTracks";
  MonGroup tracksMon(this,paths[systemIndex]+"mapsOfHitsOnTracks", run, ATTRIB_UNMANAGED);
  //  if (isNewRun){
  if (newRunFlag()){
    (histoVec[systemIndex])->clear();
    (histoVecRecent[systemIndex])->clear();
    //book Hitmaps and hits per layer histograms
    for (unsigned int i(0); i!=limits[systemIndex];++i) {
      LayerSideFormatter layerSide(i,systemIndex);
      const string streamhitmap ="mapsOfHitsOnTracks"+abbreviations[systemIndex]+streamDelimiter+"trackhitsmap_"+layerSide.name() ;
      const string streamhitmaprecent ="mapsOfHitsOnTracksRecent"+abbreviations[systemIndex]+streamDelimiter+"trackhitsmap_"+layerSide.name() ;
      std::string histoName=stem+streamhitmap;
      std::string histotitle="SCT hits on tracks for "+names[systemIndex]+" "+layerSide.title();
      std::string histotitlerecent="SCT hits on tracks from recent events for "+names[systemIndex]+" "+layerSide.title();
      h2Factory(streamhitmap, histotitle, bec, tracksMon,*(histoVec[systemIndex]));
      if(m_environment==AthenaMonManager::online) h2Factory(streamhitmaprecent, histotitlerecent, bec, tracksMon,*(histoVecRecent[systemIndex]));
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode 
SCTHitsNoiseMonTool::bookGeneralTrackTimeHistos(const unsigned int systemIndex){
  //SCTHitsNoiseMonTool::bookGeneralTrackTimeHistos(bool isNewRun , const unsigned int systemIndex){
  //  if(isNewRun){
  if(newRunFlag()){
    const string path[]={"SCT/SCTEC/tbin/tracks/", "SCT/SCTB/tbin/tracks/", "SCT/SCTEA/tbin/tracks/"};
    const unsigned int limits[N_REGIONS]={N_DISKS, N_BARRELS, N_DISKS};
    std::vector<TH1D *> * tbinHistoVectorArray[]={&m_tbinHistoVectorECm ,&m_tbinHistoVector ,&m_tbinHistoVectorECp };
    std::vector<TH1D *> * tbinHistoVectorArrayRecent[]={&m_tbinHistoVectorRecentECm ,&m_tbinHistoVectorRecent ,&m_tbinHistoVectorRecentECp };
    //
    std::vector<TH1D *> & tbinHistoVector=*(tbinHistoVectorArray[systemIndex]);
    std::vector<TH1D *> & tbinHistoVectorRecent=*(tbinHistoVectorArrayRecent[systemIndex]);
    tbinHistoVector.clear();
    tbinHistoVectorRecent.clear();
    MonGroup timeGroup(this,path[systemIndex],run,ATTRIB_UNMANAGED);
    MonGroup tbinGroup(this,"SCT/GENERAL/tbin",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED);    
    std::string stem=m_stream+pathDelimiter+path[systemIndex] ;
    const unsigned int nBins=8;
    std::string m_tbinsNames[] = {"000","001","010","011","100","101","110","111"};
    std::string histoName="TrackTimeBin"+abbreviations[systemIndex];
    std::string histoTitle="RDO Track TimeBin for "+names[systemIndex];
    std::string histoNameRecent="TrackTimeBinRecent"+abbreviations[systemIndex];
    std::string histoTitleRecent="RDO Track TimeBin from recent events for "+names[systemIndex];
    m_tbinfracall=profFactory("TBinFracAll","fraction of 01X for each region",tbinGroup);
    switch(systemIndex){
      case 0:{
        m_tbinHistoECm=h1DFactory(histoName, histoTitle, timeGroup, -0.5,7.5, nBins);
	std::string disksidenameECm[] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
	for(int i=0; i<18; i++){
	  std::string nameECmfrac = "TBinFracEC_"+disksidenameECm[i];
	  std::string titleECmfrac = "fraction of 01X in EndcapC"+disksidenameECm[i];
	  m_tbinfracECm[i]=prof2DFactory(nameECmfrac,titleECmfrac,timeGroup,3,-0.5,2.5,52,-0.5,51.5);
	}
	m_tbinfracVsLBECm = profFactory("TBinFrac01XVsLBEC","fraction of 01X vs LumiBlock in EndcapC",timeGroup,2000,0,2000);
	for (unsigned int bin(0); bin<nBins; bin++) {
	  m_tbinHistoECm->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
	}
	m_tbinHistoECm->GetXaxis()->SetTitle("TimeBin");
	if(m_environment==AthenaMonManager::online) {
	  m_tbinHistoRecentECm=h1DFactory(histoNameRecent, histoTitleRecent, timeGroup, -0.5,7.5, nBins);
	  for (unsigned int bin(0); bin<nBins; bin++) {
	    m_tbinHistoRecentECm->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
	  }
	  m_tbinHistoRecentECm->GetXaxis()->SetTitle("TimeBin");
	}
        break; 
      }
      case 1:{
        m_tbinHisto=h1DFactory(histoName, histoTitle, timeGroup, -0.5,7.5, nBins);
	std::string layersidenameB[] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1"};
	for(int i=0; i<8; i++){
	  std::string nameBfrac = "TBinFrac_"+layersidenameB[i];
	  std::string titleBfrac = "fraction of 01X in Barrel"+layersidenameB[i];
	  m_tbinfrac[i]=prof2DFactory(nameBfrac,titleBfrac,timeGroup,13,-6.5,6.5,56,-0.5,55.5); 
	}
	m_tbinfracVsLB = profFactory("TBinFrac01XVsLB","fraction of 01X vs LumiBlock in Barrel",timeGroup,2000,0,2000);
	for (unsigned int bin(0); bin<nBins; bin++) {
	  m_tbinHisto->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
	}
	m_tbinHisto->GetXaxis()->SetTitle("TimeBin");
	if(m_environment==AthenaMonManager::online) {
	  m_tbinHistoRecent=h1DFactory(histoNameRecent, histoTitleRecent, timeGroup, -0.5,7.5, nBins);
	  for (unsigned int bin(0); bin<nBins; bin++) {
	    m_tbinHistoRecent->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
	  }
	  m_tbinHistoRecent->GetXaxis()->SetTitle("TimeBin");
	}
        break; 
      }
      case 2:{
        m_tbinHistoECp=h1DFactory(histoName, histoTitle, timeGroup, -0.5,7.5, nBins);
	std::string disksidenameECp[] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
	for(int i=0; i<18; i++){
	  std::string nameECpfrac = "TBinFracEA_"+disksidenameECp[i];
	  std::string titleECpfrac = "fraction of 01X in EndcapA"+disksidenameECp[i];
	  m_tbinfracECp[i]=prof2DFactory(nameECpfrac,titleECpfrac,timeGroup,3,-0.5,2.5,52,-0.5,51.5); 
	}
	m_tbinfracVsLBECp = profFactory("TBinFrac01XVsLBEA","fraction of 01X vs LumiBlock in EndcapA",timeGroup,2000,0,2000);
	for (unsigned int bin(0); bin<nBins; bin++) {
	  m_tbinHistoECp->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
	}
	m_tbinHistoECp->GetXaxis()->SetTitle("TimeBin");
	if(m_environment==AthenaMonManager::online) {
	  m_tbinHistoRecentECp=h1DFactory(histoNameRecent, histoTitleRecent, timeGroup, -0.5,7.5, nBins);
	  for (unsigned int bin(0); bin<nBins; bin++) {
	    m_tbinHistoRecentECp->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
	  }
	  m_tbinHistoRecentECp->GetXaxis()->SetTitle("TimeBin");
	}
        break; 
      }
    }
    for (unsigned int i(0); i!= limits[systemIndex];++i){
      const string streamlayer(std::to_string(i));
      const string streamhitmap ="TrackTimeBin"+abbreviations[systemIndex]+streamDelimiter+streamlayer;
      const string streamhitmaprecent ="TrackTimeBinRecent"+abbreviations[systemIndex]+streamDelimiter+streamlayer;
      histoName=stem+streamhitmap;
      histoNameRecent=stem+streamhitmaprecent;
      histoTitle="RDO Track TimeBin: layer " +streamlayer;
      histoTitleRecent="RDO Track TimeBin form recent events: layer " +streamlayer;
      if(systemIndex!=1){
	histoTitle="RDO Track TimeBin: disk " +streamlayer;
	histoTitleRecent="RDO Track TimeBin form recent events: disk " +streamlayer;
      }//16.12.2014
      h1DFactory(streamhitmap, histoTitle,timeGroup, *(tbinHistoVectorArray[systemIndex]),-0.5, 7.5, nBins);
      for (unsigned int bin(0); bin<nBins; bin++) tbinHistoVector[i]->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
      tbinHistoVector[i]->GetXaxis()->SetTitle("TimeBin");
      if(m_environment==AthenaMonManager::online) {
	h1DFactory(streamhitmaprecent, histoTitleRecent,timeGroup, *(tbinHistoVectorArrayRecent[systemIndex]),-0.5, 7.5, nBins);
	for (unsigned int bin(0); bin<nBins; bin++) tbinHistoVectorRecent[i]->GetXaxis()->SetBinLabel(bin+1,m_tbinsNames[bin].c_str());
	tbinHistoVectorRecent[i]->GetXaxis()->SetTitle("TimeBin");
      }
    }
  }
  return StatusCode::SUCCESS;
}

SCTHitsNoiseMonTool::H1D_t
SCTHitsNoiseMonTool::h1DFactory(const std::string & name, const std::string & title, MonGroup & registry, VecH1D_t & storageVector, const float lo, const float hi, const unsigned int nbins){
  H1D_t tmp = new TH1D(TString(name), TString(title), nbins, lo, hi);
  //  tmp->SetXTitle("Cluster Size");
  //  tmp->SetYTitle("Num of Events");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  storageVector.push_back(tmp);
  return success?tmp:NULL;
}

SCTHitsNoiseMonTool::H1D_t
SCTHitsNoiseMonTool::h1DFactory(const std::string & name, const std::string & title, MonGroup & registry, const float lo, const float hi, const unsigned int nbins){
  H1D_t tmp = new TH1D(TString(name), TString(title), nbins, lo, hi);
  bool success( registry.regHist(tmp).isSuccess());
  if (not success) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;
}

SCTHitsNoiseMonTool::H2_t
SCTHitsNoiseMonTool::h2Factory(const std::string & name, const std::string & title, const SCT_Monitoring::Bec bec, MonGroup & registry, VecH2_t & storageVector){
  int firstEta(FIRST_ETA_BIN), lastEta(LAST_ETA_BIN), firstPhi(FIRST_PHI_BIN), lastPhi(LAST_PHI_BIN), nEta(N_ETA_BINS), nPhi(N_PHI_BINS);
  if (bec != BARREL){
    firstEta=FIRST_ETA_BIN_EC;
    lastEta=LAST_ETA_BIN_EC;
    firstPhi=FIRST_PHI_BIN_EC;
    lastPhi=LAST_PHI_BIN_EC;
    nEta=N_ETA_BINS_EC;
    nPhi=N_PHI_BINS_EC;
  }
  H2_t tmp = new TH2F(TString(name), TString(title), nEta, firstEta-0.5, lastEta+0.5, nPhi, firstPhi-0.5, lastPhi+0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success)  msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  storageVector.push_back(tmp);
  return success?tmp:NULL;
}

//modify shirabe
SCTHitsNoiseMonTool::H2I_t
SCTHitsNoiseMonTool::h2IFactory(const std::string & name, const std::string & title, MonGroup & registry, int nbinx, double xlo, double xhi, int nbiny, double ylo, double yhi){

  H2I_t tmp = new TH2I(TString(name), TString(title),nbinx,xlo,xhi,nbiny,ylo,yhi);
  tmp->SetXTitle("module #");
  tmp->SetYTitle("Time bin");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success)  msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;
}
SCTHitsNoiseMonTool::Prof2_t
SCTHitsNoiseMonTool::prof2DFactory(const std::string & name, const std::string & title, MonGroup & registry, int nbinx, int xlo, int xhi, int nbiny, int ylo, int yhi){
  Prof2_t tmp = new TProfile2D(TString(name), TString(title),nbinx,xlo-0.5,xhi+0.5,nbiny,ylo-0.5,yhi+0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success)  msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;
}

SCTHitsNoiseMonTool::Prof_t
SCTHitsNoiseMonTool::profFactory(const std::string & name, const std::string & title, MonGroup & registry, int nbin, int lo, int hi){
  Prof_t tmp = new TProfile(TString(name), TString(title),nbin,lo,hi);
  tmp->SetXTitle("LumiBlock");
  tmp->SetYTitle("Fraction of 01X");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success)  msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;
}

SCTHitsNoiseMonTool::Prof_t
SCTHitsNoiseMonTool::profFactory(const std::string & name, const std::string & title, MonGroup & registry){
  Prof_t tmp = new TProfile(TString(name), TString(title),3,0,3);
  tmp->SetYTitle("Fraction of 01X");
  tmp->GetXaxis()->SetBinLabel(1.5,"Endcap C");
  tmp->GetXaxis()->SetBinLabel(2.5,"Barrel");
  tmp->GetXaxis()->SetBinLabel(3.5,"Endcap A");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success)  msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;
}

//end modify

SCTHitsNoiseMonTool::Prof2_t
SCTHitsNoiseMonTool::prof2Factory(const std::string & name, const std::string & title, const SCT_Monitoring::Bec bec, MonGroup & registry, VecProf2_t & storageVector){
  int firstEta(FIRST_ETA_BIN), lastEta(LAST_ETA_BIN), firstPhi(FIRST_PHI_BIN), lastPhi(LAST_PHI_BIN), nEta(N_ETA_BINS), nPhi(N_PHI_BINS);
  if (bec != BARREL){
    firstEta=FIRST_ETA_BIN_EC;
    lastEta=LAST_ETA_BIN_EC;
    firstPhi=FIRST_PHI_BIN_EC;
    lastPhi=LAST_PHI_BIN_EC;
    nEta=N_ETA_BINS_EC;
    nPhi=N_PHI_BINS_EC;
  }
  Prof2_t tmp = new TProfile2D(TString(name), TString(title), nEta, firstEta-0.5, lastEta+0.5, nPhi, firstPhi-0.5, lastPhi+0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success)  msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  storageVector.push_back(tmp);
  return success?tmp:NULL;
}

SCTHitsNoiseMonTool::H1_t
SCTHitsNoiseMonTool::h1Factory(const std::string & name, const std::string & title, MonGroup & registry, const float lo, const float hi, const unsigned int nbins){
  H1_t tmp = new TH1F(TString(name),TString(title),nbins,lo,hi);
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;      
}

SCTHitsNoiseMonTool::H1_t
SCTHitsNoiseMonTool::h1Factory(const std::string & name, const std::string & title, MonGroup & registry, VecH1_t & storageVector, const float lo, const float hi, const unsigned int nbins){
  H1_t tmp = new TH1F(TString(name),TString(title),nbins,lo,hi);
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  storageVector.push_back(tmp);
  return success?tmp:NULL;      
}

