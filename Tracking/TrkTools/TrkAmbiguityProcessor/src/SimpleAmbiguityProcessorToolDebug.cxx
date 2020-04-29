#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
#include "SimpleAmbiguityProcessorToolDebug.h"
#ifdef SIMPLEAMBIGPROCNTUPLECODE
#include "xAODEventInfo/EventInfo.h"
#include "HepPDT/ParticleDataTable.hh"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GaudiKernel/ITHistSvc.h" 
#include "TTree.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#endif
#ifdef SIMPLEAMBIGPROCDEBUGCODE
#include "TrkTruthData/TrackTruthCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "AtlasHepMC/GenVertex.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "GeneratorObjects/McEventCollection.h"
#endif

Trk::SimpleAmbiguityProcessorToolDebug::SimpleAmbiguityProcessorToolDebug(const std::string& t,
                                                                          const std::string& n,
                                                                          const IInterface*  p )
   : Trk::SimpleAmbiguityProcessorTool(t,n,p)
  ,m_particlePropSvc("PartPropSvc",n),
  m_particleDataTable(0),
  m_validationTreeName("AmbiguityProcessorTool"+n),
  m_validationTreeDescription("Validation of the AmbiguityProcessor "+n),
  m_validationTreeFolder("/val/AmbiProcessor"+n),
  m_validationTree(0),
  m_event(0),
  m_trackTrackMap(0)
  ,m_truthToTrack(0)//the comma in front of m_truthToTrack is necessary  
{
#ifdef SIMPLEAMBIGPROCNTUPLECODE
  declareProperty("TrackSeedMap", m_trackSeedMapLocation = "TrackSeedMap");
  declare(m_eventInfo_key = "EventInfo");
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  declareProperty("ResolvedTrackConnection", m_resolvedTrackConnection="SiSPS_ResolvedTrackConnection");
  declareProperty("TruthCollection", m_truthCollection="SiSPSeededTracksTruthCollection");
  //to get brem truth
  declareProperty("GeneratedEventCollection", m_generatedEventCollectionName="TruthEvent");
  declare(m_write_resolvedTrackConnection);
#endif

#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
  declareProperty("IsBackTracking", m_isBackTracking=false);
  declareProperty("TruthLocationPixel", m_truth_locationPixel="PRD_MultiTruthPixel");
  declareProperty("TruthLocationSCT", m_truth_locationSCT="PRD_MultiTruthSCT");
  declareProperty("TruthLocationTRT", m_truth_locationTRT="PRD_MultiTruthTRT");
#endif


}

StatusCode Trk::SimpleAmbiguityProcessorToolDebug::initialize()
{
   ATH_CHECK(Trk::SimpleAmbiguityProcessorTool::initialize());

#ifdef SIMPLEAMBIGPROCNTUPLECODE

  ATH_CHECK(m_eventInfo_key.initialize());
  m_has_trackSeedMap = m_trackSeedMapLocation.initialize().isSuccess();
  if (!m_has_trackSeedMap)
    ATH_MSG_DEBUG("Could not find TrackSeedMap, "
		  "Seed validation needs to be run as well to have full output!");
  
  // retrieve the ParticleProperties handle
  if( m_particlePropSvc.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "[ ---- ] Can not retrieve " << m_particlePropSvc << " ! Abort. " );
      return StatusCode::FAILURE;
    }
  
  // and the particle data table 
  m_particleDataTable = m_particlePropSvc->PDT();
  if (m_particleDataTable==0)
    {
      ATH_MSG_FATAL( " [ ---- ] Could not get ParticleDataTable! Cannot associate pdg code with charge! Abort. " );
      return StatusCode::FAILURE;
    }
  
  ITHistSvc* tHistSvc = 0;
  // now register the Tree
  if (service("THistSvc",tHistSvc).isFailure()) 
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !" ); 
  ATH_MSG_VERBOSE(  "Booking the validation TTree ... " );
  // create the new Tree
  m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());
  // create the branches
  
  if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
    ATH_MSG_ERROR("Could not register the  validation Tree -> Switching  ValidationMode Off !" );
    delete m_validationTree; m_validationTree = 0;
  } else {
    ATH_MSG_INFO( "TTree for Validation booked." );
    m_validationTree->Branch( "Event",                  &m_event      , "event/I");
    m_validationTree->Branch( "Track",                  &m_track      , "track/I");    
    m_validationTree->Branch( "Pt",                     &m_pt         , "pt/F");
    m_validationTree->Branch( "Eta",                    &m_eta        , "eta/F");
    m_validationTree->Branch( "Phi",                    &m_phi        , "phi/F");
    m_validationTree->Branch( "Score",                  &m_score      , "score/I");
    m_validationTree->Branch( "Accepted",               &m_accepted   , "accepted/I");
    m_validationTree->Branch( "Duplicate",              &m_duplicate  , "duplicate/I");
    m_validationTree->Branch( "Perigee",                &m_perigeeInfo, "perigeeInfo/I");        
    m_validationTree->Branch( "Author",                 &m_author,      "authorChange/I");
    m_validationTree->Branch( "McPt",                   &m_pT_mc         , "ptmc/F");
    m_validationTree->Branch( "McCharge",               &m_charge_mc     , "chargemc/F");        
    m_validationTree->Branch( "NumberOfHits",           &m_numhits      , "hitsontrack/I");
    m_validationTree->Branch( "NumberBarcodes",         &m_numbarcodes  , "barcodesontrack/I");
    m_validationTree->Branch( "NumberOfHitsTruthLost",  &m_numtruthlost , "hitstruthlost/I");
    m_validationTree->Branch( "LeadingBarcode",         &m_leadingbarcode, "leadingbc/I");
    m_validationTree->Branch( "NumberOfHitsLBC",        &m_leadingnumhits, "numberofhitslbc/I");
    m_validationTree->Branch( "BarcodeDuplicates",      &m_barcodeDuplicates, "barcodedupes/I");      
    m_validationTree->Branch( "NumberOfSeeds",          &m_nseeds, "numberofseeds/I");
    m_validationTree->Branch( "SeedIndices",            m_seeds, "seeds[numberofseeds]/I");      
    
  }
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  ATH_CHECK(m_generatedEventCollectionName.initialize());
  ATH_CHECK(m_truthCollection.initialize());
  m_has_resolvedTrackConnection = m_resolvedTrackConnection.initialize().isSuccess();
  if (!m_has_resolvedTrackConnection) {
    m_write_resolvedTrackConnection = m_resolvedTrackConnection.key();
    ATH_CHECK(m_write_resolvedTrackConnection.initialize());
  }

  // to get the brem truth
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL) << "Toll service not found !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = toolSvc->retrieveTool("Trk::TruthToTrack", m_truthToTrack);
  if(sc.isFailure()) {
     msg(MSG::FATAL) << "Cannot retrieve the TruthToTrack tool... Exiting" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not get PixelID helper !" << endmsg;
      return StatusCode::FAILURE;
    }
#endif

#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
  ATH_CHECK(m_truth_locationPixel.initialize());
  ATH_CHECK(m_truth_locationSCT.initialize());
  ATH_CHECK(m_truth_locationTRT.initialize());
#endif

  return sc;
}

void Trk::SimpleAmbiguityProcessorToolDebug::ntupleReset() {
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  numOutliersDiff  = 0;
  numFirstFitLost  = 0;
  numSecondFitLost = 0;
  truthBefore      = 0;
  truthAfter       = 0;
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::resetTrackOutliers() {
#ifdef SIMPLEAMBIGPROCDEBUGCODE
      numOutliersBefore = 0;
      numOutliersAfter  = 0;
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::setBarcodeStats(const Trk::Track *a_track, TrackScore score)
{
   (void) a_track;
   (void) score;
#ifdef SIMPLEAMBIGPROCNTUPLECODE   
      // get the object stored from the track map
      m_duplicate        = 0; 
      m_numbarcodes      = 0;
      m_numtruthlost     = 0;
      m_leadingbarcode   = 0;
      m_leadingnumhits   = 0;        
      std::map<const Trk::Track*, TrackBarcodeStats>::iterator tbcIt = m_trackBarcodeMap.find(a_track);
      
      if ( tbcIt !=  m_trackBarcodeMap.end() ){ 
	m_numhits          = (*tbcIt).second.numhits;
	m_numbarcodes      = (*tbcIt).second.numbarcodes;
	m_numtruthlost     = (*tbcIt).second.numtruthlost;
	m_leadingbarcode   = (*tbcIt).second.leadingbarcode;
	m_leadingnumhits   = (*tbcIt).second.leadingnumhits;
	// get the number of track having the same barcode
	m_barcodeDuplicates = m_barcodeTrackMap.count(m_leadingbarcode);
      }
      
      // reset for later filling
      m_score            = int(score);
      m_eta              = 0.;
      m_phi              = 0.;
      m_pt               = 0.;
      m_track            = long(a_track);
      m_author           = 0;
      m_accepted         = 0;
      m_perigeeInfo      = 0;
      
      // only if seed map has been found
      m_nseeds = 0;
      
      if ( m_has_trackSeedMap ){
	pair< TrackSeedMap::const_iterator,TrackSeedMap::const_iterator > seeds = m_trackSeedMap->equal_range((*trackIt));
	TrackSeedMap::const_iterator tsmIter = seeds.first;
	TrackSeedMap::const_iterator tsmIterEnd = seeds.second;
        
	for (int i=0; i<MAXSEEDSPERTRACK;++i) m_seeds[i]=0;
	for(; tsmIter!=tsmIterEnd;++tsmIter){
	  if(m_nseeds <  MAXSEEDSPERTRACK) m_seeds[m_nseeds] = (*tsmIter).second;
	  m_nseeds++;
	}
      }
#endif      
}

void Trk::SimpleAmbiguityProcessorToolDebug::fillBadTrack(const Trk::Track *a_track, const Trk::PRDtoTrackMap &prd_to_track_map) {
   (void) a_track;
   (void) prd_to_track_map;
#ifdef SIMPLEAMBIGPROCNTUPLECODE 
	// (1) tracks can be rejected in the addNewTracks() method due to 0 score
	fillValidationTree(a_track);
	m_validationTree->Fill();
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
	if( isTrueTrack(a_track)){
	  msg(MSG::INFO)<< "the # of Outliers before is: " << numOutliersBefore << " and after is: " << numOutliersAfter<<endmsg;
	  if(numOutliersBefore != numOutliersAfter){
	    msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(a_track) << " Because of Outlier change" <<endmsg; 
	    numOutliersDiff++;
	  } else {
	    msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(a_track) << " Because of Zero Score" <<endmsg; 
	  }
	  prdTruth(a_track, prd_to_track_map);
	  tsosTruth(a_track);
	}
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::fillDuplicateTrack(const Trk::Track *a_track) {
#ifdef SIMPLEAMBIGPROCNTUPLECODE   
	    // (2) tracks can be rejected in the addNewTracks() method due to being a duplicate
	    m_duplicate = 1;
	    fillValidationTree(*trackIt);  	  
	    m_validationTree->Fill();
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::associateToOrig(const Trk::Track*new_track, const Trk::Track* orig_track) {
#ifdef SIMPLEAMBIGPROCNTUPLECODE
	m_trackTrackMap->insert( std::pair< const Trk::Track* , const Trk::Track*> (new_track,  orig_track));
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::countTrueTracksInMap(const TrackScoreMap &trackScoreTrackMap) {
   (void) trackScoreTrackMap;
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  int numTrueMap = 0;
  TrackScoreMap::iterator itnext = trackScoreTrackMap.begin();
  TrackScoreMap::iterator itend = trackScoreTrackMap.end();
  for (; itnext != itend; ++itnext){
     if(isTrueTrack(itnext->second.track()))
        numTrueMap++;
  }
  msg(MSG::INFO)<<"Number of TRUE tracks in map:"<< numTrueMap <<endmsg;
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::rejectedTrack(const Trk::Track*track, const Trk::PRDtoTrackMap &prd_to_track_map)
{
#ifdef SIMPLEAMBIGPROCNTUPLECODE
      m_accepted = 0;
      m_score    = 0;
      m_validationTree->Fill();
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
      if(isTrueTrack(track)){
	msg(MSG::INFO)<< "the # of Outliers before is: " << numOutliersBefore << " and after is: " << numOutliersAfter<<endmsg;
	if(numOutliersBefore != numOutliersAfter){
	  msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(track) << " Because of Outlier change" <<endmsg; 
	  numOutliersDiff++;
	}else {
	  msg(MSG::INFO)<<"Rejecting True Track:"<< origTrack(track) << " Because of Zero Score" <<endmsg; 
	}
	prdTruth(track, prd_to_track_map);
	tsosTruth(track);
      }
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::fitStatReset() {
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  n_trueFitFails = 0;
  n_fitFails     = 0;
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::acceptedTrack(const Trk::Track*track) {
   (void) track;
#ifdef SIMPLEAMBIGPROCNTUPLECODE 
   // this track goes into the final selection, record it
   m_accepted = 1;
   fillValidationTree(track);
   m_validationTree->Fill();
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::memoriseOutliers(const Trk::Track*track) {
   (void) track;
#ifdef SIMPLEAMBIGPROCDEBUGCODE
	  numOutliersBefore = track->outliersOnTrack()->size();
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::newCleanedTrack(const Trk::Track*new_track, const Trk::Track* orig_track) {
   (void) new_track;
   (void) orig_track;
#ifdef SIMPLEAMBIGPROCNTUPLECODE
   m_trackTrackMap->insert( std::make_pair (new_track, orig_track));
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
   keepTrackOfTracks(new_track, orig_track);
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::eventSummary(const TrackCollection *final_tracks) {
   (void) final_tracks
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  TrackCollection::const_iterator  m_itFinal = final_tracks->begin();
  TrackCollection::const_iterator m_endFinal = final_tracks->end();
  for ( ; m_itFinal != m_endFinal ; ++m_itFinal) {
    if ( isTrueTrack(*m_itFinal) )
      truthAfter++;
  }
  msg(MSG::INFO)<<"Where, the number of true track on output is: "<<truthAfter <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE track failed fits is:  "<< n_trueFitFails <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE tracks rejected due to outliers is: "<< numOutliersDiff <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE tracks rejected after the first fit is: "<< numFirstFitLost <<endmsg;
  msg(MSG::INFO)<<"And the number of TRUE tracks rejected after the second fit is: "<< numSecondFitLost <<endmsg;
  
  if(truthBefore != truthAfter)
    msg(MSG::INFO)<<"The number of tracks lost this events is:  "<< truthBefore-truthAfter << endmsg;
  
  if (n_trueFitFails >0 && m_isBackTracking){
    msg(MSG::INFO) << "DOING THE BREM TRUTH" << endmsg;
    getBremTruth();//problem with statuscode
  }
  
  produceInputOutputConnection(*final_tracks);
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::keepFittedInputTrack(const Trk::Track *a_track, TrackScore ascore)
{
   (void) a_track;
   (void) ascore;
#ifdef SIMPLEAMBIGPROCNTUPLECODE   
	  m_score       = int(-(ascore));
	  m_eta         = 0.;
	  m_phi         = 0.;
	  m_pt          = 0.;
	  m_track       = long(a_track);
	  m_author      = 0;
	  m_accepted    = 0;
	  m_perigeeInfo = 0;
	  m_duplicate   = 0;    
	  
	  m_nseeds = 0;
	  
	  if (m_has_trackSeedMap) {
	    // find the original track
	    std::map<const Trk::Track* , const Trk::Track*>::iterator iter;
	    const Trk::Track * tmpTrack = a_track;
	    while((iter = m_trackTrackMap->find(tmpTrack)) != m_trackTrackMap->end()){
	      tmpTrack = iter->second;
	    }
	    pair< TrackSeedMap::const_iterator,TrackSeedMap::const_iterator > seeds = m_trackSeedMap->equal_range((tmpTrack));
            
	    TrackSeedMap::const_iterator tsmIter = seeds.first;
	    TrackSeedMap::const_iterator tsmIterEnd = seeds.second;
            
	    for(int i=0; i<MAXSEEDSPERTRACK;++i) m_seeds[i]=0;
	    for(; tsmIter!=tsmIterEnd;++tsmIter){
	      if(m_nseeds <  MAXSEEDSPERTRACK) m_seeds[m_nseeds] = (*tsmIter).second;
	      m_nseeds++;
	    }
	  }
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::fillFailedFit(const Trk::Track *a_track, const Trk::PRDtoTrackMap &prd_to_track_map)
{
#ifdef SIMPLEAMBIGPROCNTUPLECODE
    // this is a track that did not survive the refit
    m_accepted = -2;
    m_validationTree->Fill();
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
    msg(MSG::INFO) << "The Track: " << origTrack(track) << " failed to fit" << endmsg;
    n_fitFails++;
    msg(MSG::INFO) << "The total number of failed fits is now" << n_fitFails <<endmsg;
    if ( isTrueTrack(a_track)) {
      n_trueFitFails++;
      msg(MSG::INFO)<< "The total number of TRUE failed fits is now" << n_trueFitFails <<endmsg;
      prdTruth(a_track, prd_to_track_map);
      tsosTruth(a_track);
    }
#endif
}

void Trk::SimpleAmbiguityProcessorToolDebug::fillEventData(std::vector<const Track*> &tracks)
{
   (void) tracks;
  using namespace std;
#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
  m_truthPIX = SG::makeHandle(m_truth_locationPixel);
  m_truthSCT = SG::makeHandle(m_truth_locationSCT);
  m_truthTRT = SG::makeHandle(m_truth_locationTRT);
#endif

#ifdef SIMPLEAMBIGPROCNTUPLECODE
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo_key);
  m_event = (int)eventInfo->eventNumber();
  
  m_trackBarcodeMap.clear();
  m_barcodeTrackMap.clear();
  // fill the truth maps
    
  TrackCollection::const_iterator trackIt    = tracks.begin();
  TrackCollection::const_iterator trackItEnd = tracks.end();
    
  for ( ; trackIt != trackItEnd ; ++trackIt) {
    std::map<int,int> barcodeOccurence;
    std::map<int,const HepMC::GenParticle*> barcodeGenParticle;
    int numhits         = 0;
    int numbarcodes     = 0;
    int numtruthlost    = 0;
    int leadingbarcode  = 0;
    int leadingnumhits  = 0;  
    // prd iteration
    const DataVector<const MeasurementBase>* measurements = (*trackIt)->measurementsOnTrack();
    if (!measurements) continue;
    DataVector<const MeasurementBase>::const_iterator rotIter    = measurements->begin();
    DataVector<const MeasurementBase>::const_iterator rotIterEnd = measurements->end();
        
        
    for ( ; rotIter != rotIterEnd; ++rotIter ){
      // get the prd from the ROT    
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*rotIter);
      if (!rot) continue;
      const Trk::PrepRawData* prd = (*rot).prepRawData();
      if (!prd) continue;
          
      // PIXEL / SCT
      const InDet::PixelCluster* pixCluster = dynamic_cast<const InDet::PixelCluster*>(prd);
      const InDet::SCT_Cluster*  sctCluster  = pixCluster ? 0 : dynamic_cast<const InDet::SCT_Cluster*>(prd);
      const InDet::SiCluster* siCluster = pixCluster;
      siCluster = siCluster ? siCluster : sctCluster;
      // get the right 
      SG::ReadHandle<PRD_MultiTruthCollection> truthColll = pixCluster ? m_truthPIX : m_truthSCT;
      // one of the two worked
          
      if (siCluster){
	    
	++numhits;            
	// get the rdo list and loop over it
	const std::vector<Identifier>& rdoList = siCluster->rdoList();
	std::vector<Identifier>::const_iterator rdoIter    = rdoList.begin();
	std::vector<Identifier>::const_iterator rdoIterEnd = rdoList.end();
            
	for ( ; rdoIter != rdoIterEnd; ++rdoIter ){
	  // find the GenParticle link : Pixels                                
	  PRD_MultiTruthCollection::const_iterator hmcpIt      = m_truthPIX->end();
	  std::pair<PRD_MultiTruthCollection::const_iterator, PRD_MultiTruthCollection::const_iterator> itpixRange = truthColll->equal_range(*rdoIter);
	  for ( hmcpIt = itpixRange.first; hmcpIt != itpixRange.second; ++hmcpIt){
	    // get the HepMCParticleLink
	    HepMcParticleLink pLink = hmcpIt->second;
	    // get vertex and direction
	    const HepMC::GenParticle* particle = pLink.cptr();
	    if (particle){
	      // get the particle barcode
	      int barcode = particle->barcode();
	      std::map<int,int>::iterator bcuIter = barcodeOccurence.find(barcode);
	      if (barcode && bcuIter == barcodeOccurence.end()) {
		barcodeOccurence.insert(std::make_pair(barcode,1));
		barcodeGenParticle.insert(std::make_pair(barcode,particle));
	      }
	      else if (barcode) ++barcodeOccurence[barcode];
	      else ++numtruthlost; 
	    } else ++numtruthlost;
                
	  } // loop over GenParticles
	} // loop over rdos
      } // siCluster check 
    } // prd Loop
    numbarcodes = barcodeOccurence.size();
    std::map<int,int>::iterator bcIter    = barcodeOccurence.begin();
    std::map<int,int>::iterator bcIterEnd = barcodeOccurence.end();
    for ( ; bcIter != bcIterEnd; ++bcIter){
      if ( (*bcIter).second > leadingnumhits ){
	leadingnumhits = (*bcIter).second;
	leadingbarcode = (*bcIter).first;
      }
    }
    if (leadingbarcode){
      // get the charge and the truth pt
      const HepMC::GenParticle* particle = barcodeGenParticle[leadingbarcode];
      m_pT_mc = particle ?  particle->momentum().perp() : 0.;
          
      int pdgCode = particle->pdg_id();
      int absPdgCode = abs(pdgCode);
          
      // get the charge: ap->charge() is used later
      const HepPDT::ParticleData* ap =  m_particleDataTable->particle( absPdgCode);
      if( ap) m_charge_mc = ap->charge();
      // since the PDT table only has abs(PID) values for the charge
      m_charge_mc *= (pdgCode > 0.) ?  1. : -1.;
    }
    // create the stat object
    TrackBarcodeStats tbcStats;
    tbcStats.numhits        =  numhits       ;
    tbcStats.numbarcodes    =  numbarcodes   ;
    tbcStats.numtruthlost   =  numtruthlost  ;
    tbcStats.leadingbarcode =  leadingbarcode;
    tbcStats.leadingnumhits =  leadingnumhits;
    // create the map entries
    m_trackBarcodeMap.insert(std::make_pair(*trackIt,tbcStats));
    m_barcodeTrackMap.insert(std::make_pair(leadingbarcode,*trackIt));
        
  } // track loop
#endif
}

//==================================================================================================
//
//
//   FROM HERE EVERYTHING IS DEBUGGING CODE !!!
//
//
// Part I : Ntuple writing
//==================================================================================================

#ifdef SIMPLEAMBIGPROCNTUPLECODE
void Trk::SimpleAmbiguityProcessorTool::fillValidationTree(const Trk::Track* track) const
{     
   // keep track of the track pointer
    m_track = long(track);
   // the good guess : we perigee 
    const Trk::TrackParameters* tp = track->perigeeParameters();
    m_perigeeInfo = tp ? 1 : 0;
    if (!tp){
     // take the first track parameter estimate for the validation
        const DataVector<const Trk::TrackParameters>* tps = track->trackParameters();    
        if (tps && tps->size()) tp = (*tps)[0];
    }
   // fill pt / eta / phi
    m_pt  = tp ? float(tp->momentum().perp()) : m_pt;
    m_eta = tp ? float(tp->momentum().eta())  : m_eta;
    m_phi = tp ? float(tp->momentum().phi())  : m_phi;   
}
#endif


#ifdef SIMPLEAMBIGPROCNTUPLECODE          
void 
  if (m_trackTrackMap) delete m_trackTrackMap;
  m_trackTrackMap = new  std::map<const Trk::Track* , const Trk::Track*> ;
#endif

//==================================================================================================
// Part II : Truth association
//==================================================================================================
#ifdef SIMPLEAMBIGPROCDEBUGCODE

void Trk::SimpleAmbiguityProcessorToolDebug::findTrueTracks(const TrackCollection* recTracks)
{

  numSharedTruth = 0;
  m_trueTracks.clear();
  m_trackHistory.clear();
  m_tracksShared.clear();

  msg(MSG::DEBUG) << "Acessing TrackTruthCollection " << endmsg;
  SG::ReadHandle<TrackTruthCollection> truthMap(m_truthCollection);
  
  std::map<int,std::pair<float,const Trk::Track*> > barcodeMap;
  float minProb =0.95;
  TrackCollection::const_iterator trackIt    = recTracks->begin();
  TrackCollection::const_iterator trackItEnd = recTracks->end();

  for (;trackIt!=trackItEnd;++trackIt)
    {
      msg(MSG::DEBUG) << "The Track is now " << *trackIt << endmsg;
	
      // initialise history tracing
      m_trackHistory.insert(std::make_pair(*trackIt,*trackIt));
	
      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(*trackIt));
      tracklink.setStorableObject(*recTracks);
      const ElementLink<TrackCollection> tracklink2=tracklink;
	
      // check if the track has a valid agreement in TrackToTruth
      TrackTruthCollection::const_iterator found = truthMap->find(tracklink2);
      if (found != truthMap->end() &&  
	  found->second.particleLink().isValid() &&
	  found->second.probability() > minProb    )
	{
	  if (!isSharedTrack(*trackIt)) addTrackToMap(*trackIt);//add track and pdrs to map
	  else numSharedTruth++;
	  int barcode=found->second.particleLink().barcode();
	  msg(MSG::DEBUG) << "The Barcode is: " << barcode << endmsg;
	  std::pair<int , std::pair<float , const Trk::Track*> >
	    insertion = std::make_pair(barcode,std::make_pair(found->second.probability(),*trackIt));
	  std::pair<std::map<int,std::pair<float,const Trk::Track*> >::iterator, bool > barcodeMapInsert =
	    barcodeMap.insert(insertion);
	  if (!(barcodeMapInsert.second) && insertion.second.first > barcodeMapInsert.first->second.first)
	    {
	      // if the barcode is already known and the new entry would be better, erase old and insert new
	      msg(MSG::DEBUG) << "Erasing Track:" << origTrack(barcodeMapInsert.first->second.second) <<endmsg; 		
	      barcodeMap.erase(barcodeMapInsert.first);
	      msg(MSG::DEBUG) << "Inserting Track:"<<  origTrack(insertion.second.second)<< endmsg;
	      barcodeMap.insert(insertion);
	    }
	  msg(MSG::DEBUG) << "The probability of " << origTrack(barcodeMapInsert.first->second.second) << "is "<< barcodeMapInsert.first->second.first <<endmsg; 
	}
    }
  
  // copy the true tracks in the TruthHistorySet:
  std::map<int,std::pair<float,const Trk::Track*> >::const_iterator  it = barcodeMap.begin();
  std::map<int,std::pair<float,const Trk::Track*> >::const_iterator end = barcodeMap.end();
  for (; it!=end; ++it) m_trueTracks.insert(it->second.second);
  truthBefore = m_trueTracks.size();
  msg(MSG::INFO) << "True Track set up with " << truthBefore << " true tracks." << endmsg;
  msg(MSG::INFO)<< "OF THE TRUE TRACKS " << numSharedTruth << " ARE SHARED" << endmsg;
  
  
}
//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::keepTrackOfTracks(const Trk::Track* oldTrack, const Trk::Track* newTrack)
{
  m_trackHistory.insert(std::make_pair(newTrack,oldTrack));
  if (m_trueTracks.find(oldTrack) != m_trueTracks.end() ) m_trueTracks.insert(newTrack);
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::produceInputOutputConnection(TrackCollection &final_tracks)
{
  if (!m_has_resolvedTrackConnection) {
    // output map: SiSpSeededTrack, ResolvedTrack  
    TrackCollectionConnection siSP_ResolvedConnection;
      
    TrackCollection::const_iterator  itFinal = final_tracks.begin();
    TrackCollection::const_iterator endFinal = final_tracks.end();
    for ( ; itFinal != endFinal ; ++itFinal)
    {
      std::map<const Trk::Track*, const Trk::Track*>::iterator pos = m_trackHistory.find(*itFinal);
      while (pos->first != pos->second && pos != m_trackHistory.end())
	pos = m_trackHistory.find(pos->second);
	  
      if (pos == m_trackHistory.end())
	msg(MSG::ERROR) << "Track not found in history" << endmsg;
      else
	siSP_ResolvedConnection.insert(std::make_pair(pos->second,*itFinal));
	  
    }

    SG::WriteHandle<TrackCollectionConnection> h_write(m_write_resolvedTrackConnection);
    h_write.record(std::make_unique<TrackCollectionConnection>(siSP_ResolvedConnection));
  }
}
//============================================================================================

const Trk::Track* Trk::SimpleAmbiguityProcessorTool::origTrack( const Trk::Track* track){
  
  std::map<const Trk::Track*, const Trk::Track*>::iterator m_pos = m_trackHistory.find(track);
  while (m_pos->first != m_pos->second && m_pos != m_trackHistory.end())
    m_pos = m_trackHistory.find(m_pos->second);
  
  if(m_pos == m_trackHistory.end())
    return 0;
  
  return m_pos->first;
}

//==================================================================================================
bool Trk::SimpleAmbiguityProcessorTool::isSharedTrack(const Trk::Track* Tr){

  int numSharedPRD = 0;
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();
  
  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if( m_tracksShared.find(prd) != m_tracksShared.end() ) numSharedPRD++;
  }
  if(numSharedPRD>=3) return true;
  else return false;
}
//===================================================================================						     

void Trk::SimpleAmbiguityProcessorTool::addTrackToMap(Trk::Track* Tr)
{
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();
  
  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if(prd) m_tracksShared.insert(std::make_pair(prd,Tr));
  }
}

//=================================================================================================
bool Trk::SimpleAmbiguityProcessorTool::isTrueTrack(const Trk::Track* track){
  std::set<const Trk::Track*>::const_iterator m_iter = m_trueTracks.find(track);
  if(m_iter != m_trueTracks.end())
    return true;
  else 
    return false;

}

//================================================================================
void Trk::SimpleAmbiguityProcessorTool::prdTruth(const Trk::Track* track, Trk::PRDtoTrackMap &prd_to_track_map){
  
  
  //geting the truth info about th prds
  std::vector<const Trk::PrepRawData*> prds = prd_to_track_map.getPrdsOnTrack(track);
  std::vector<const Trk::PrepRawData*>::const_iterator prdit = prds.begin();
  std::vector<const Trk::PrepRawData*>::const_iterator prdite = prds.end();
  double m_eta = (*(track)->trackParameters()->begin())->eta();
  msg(MSG::INFO)<< "The eta of this track is " << m_eta << endmsg;   
  
  for( ; prdit != prdite; ++prdit){
    const InDet::SiCluster      * si = dynamic_cast<const InDet::SiCluster*>      (*prdit);
    const InDet::PixelCluster   * px = dynamic_cast<const InDet::PixelCluster*>   (*prdit);
    const InDet::TRT_DriftCircle* tr = dynamic_cast<const InDet::TRT_DriftCircle*>(*prdit);
    PRD_MultiTruthCollection::const_iterator mce;
    PRD_MultiTruthCollection::const_iterator mc;
    if     (px) {
      mc=m_truthPIX->find((*prdit)->identify()); 
      mce=m_truthPIX->end();
      if(mc==mce) {
	msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was pixel noise" << endmsg;
	continue;
      }
      msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was a pixel hit" << endmsg;
    }
    else if(si) {
      mc=m_truthSCT->find((*prdit)->identify()); 
      mce=m_truthSCT->end();
      if(mc==mce) {
	msg(MSG::INFO)<< "the hit " << *prdit << "On the track " <<origTrack(track) << "was si noise" << endmsg;	
	continue;
      }	
      msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was a si hit" << endmsg;
    }
    else if(tr) {
      mc=m_truthTRT->find((*prdit)->identify()); 
      mce=m_truthTRT->end();
      if(mc==mce) {
	msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was trt noise" << endmsg;	
	continue;
      }
      msg(MSG::INFO)<< "the hit " << *prdit << "On the track " << origTrack(track) << "was a trt hit" << endmsg;
    }
    else {
      continue;
    }
    
    const HepMC::GenParticle* pa = mc->second.cptr(); 	
    int partid = pa->pdg_id();
    int partBc = pa->barcode();
    
    //HepLorentzVector m  = pa->momentum();
    double m_theta = pa->momentum().theta();
    msg(MSG::INFO)<< "the theta of the hit was " << m_theta << endmsg;
    msg(MSG::INFO)<< "The hit " << *prdit << "came from " << partid << " With a barcode of " << partBc << endmsg;
    
    
    
  }
}  

//====================================================================================
void Trk::SimpleAmbiguityProcessorTool::tsosTruth(const Trk::Track* track){

  
  const DataVector<const TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
  // loop over TSOS, copy TSOS and push into vector
  DataVector<const TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
  DataVector<const TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();   
  for(; iTsos != iTsosEnd; ++iTsos){
    msg(MSG::INFO)<< "the type of " << *iTsos << " is "<< (*iTsos)->dumpType() << endmsg;
    const FitQualityOnSurface* fq = (*iTsos)->fitQualityOnSurface();
    if (fq)
      msg(MSG::INFO)<< "the chi2 of " << *iTsos << " is "<< fq->chiSquared() << endmsg;
    const MeasurementBase* mb = (*iTsos)->measurementOnTrack();
    if (mb){
      Identifier Id =  mb->associatedSurface().associatedDetectorElementIdentifier();
      msg(MSG::INFO)<< "the global r of the hit is " << mb->associatedSurface().center().perp() << endmsg;
      std::string detType = " unknown ";
      if (m_pixelId->is_pixel(Id))
	detType = " Pixel ";
      else if (m_pixelId->is_sct(Id))
	detType = " SCT ";
      else if (m_pixelId->is_trt(Id))
	detType = " TRT ";
      msg(MSG::INFO)<< "the surface is " << detType << endmsg; 
    }
  } 
}

//=======================================================================================
StatusCode Trk::SimpleAmbiguityProcessorTool::getBremTruth(){

  StatusCode sc;
  
  // Retrieve McEventCollection from StoreGate
  SG::ReadHandle<McEventCollection> mcEventCollection(m_generatedEventCollectionName);

  // Loop over all events in StoreGate
  McEventCollection::const_iterator event = mcEventCollection->begin();
  
  for ( ; event != mcEventCollection->end(); ++event ){
    
    // Synchronise event number with reconstruction event number
    int eventNumber = (*event)->event_number();    
    msg(MSG::INFO) << "Event number: " << eventNumber - 1 << endmsg;

    // Determine the momentum of the original particle
    const double initialMomentum = originalMomentum( *event );
    msg(MSG::INFO) << "Initial momentum: " << initialMomentum << endmsg;

   

    // Calculate the total momentum loss as a result of brem
    double pLostByBrem = momentumLostByBrem( *event );
    msg(MSG::INFO) << "Total momentum lost by original particle due to Brem: " << pLostByBrem << " MeV" << endmsg;
   
    
    // Calculate the fraction of incident energy lost per vertex on track
    const std::vector<double> fractionOfIncidentEnergyLost = fractionOfIncidentMomentumLostPerVertex( *event );
    int countBrem = 0; //Si brem counter
    std::vector<double>::const_iterator fractionLostInLayer = fractionOfIncidentEnergyLost.begin();
    for ( ; fractionLostInLayer != fractionOfIncidentEnergyLost.end(); ++fractionLostInLayer ){
      msg(MSG::INFO) << "Fraction of incident energy lost at vertex: " << (*fractionLostInLayer) << endmsg;
      countBrem++;
    }
    
    // Determine the positions of the brem vertices
    const std::vector<HepPoint3D> positionsOfVertices = positionsOfBremVertices( *event );
    countBrem = 0; //Reinitialize brem counter
    std::vector<HepPoint3D>::const_iterator positionOfVertex = positionsOfVertices.begin();
    for ( ; positionOfVertex != positionsOfVertices.end(); ++positionOfVertex ){
      msg(MSG::INFO) << "(x, y, z) of vertex: " << positionOfVertex->perp() << endmsg;
      countBrem++;
    }
    
  }

  return StatusCode::SUCCESS;
}
//======================================================================================================
double Trk::SimpleAmbiguityProcessorTool::originalMomentum( const HepMC::GenEvent* genEvent )
{

  // Loop over all particles in the event (info on this from GenEvent documentation)
  HepMC::GenEvent::particle_const_iterator particle = genEvent->particles_begin();

  // Calculate initial energy of electron. Should be the first particle
  HepMC::GenParticle* initialParticle = *( genEvent->particles_begin() );

  // Double check - should come from vertex barcode = -1 & have id 10001. Particle must also be defined
  //if ( !initialParticle || initialParticle->production_vertex()->barcode() != -1 || initialParticle->barcode() != 10001 )
  //  msg(MSG::WARNING) << "Inconsistency between initial particle and initial vertex" << endmsg;

  //Hep3Vector& initial3Momentum = initialParticle->momentum();

  double initialMomentum = initialParticle->momentum().perp();

  //  const Trk::TrackParameters* initialPerigeeParameters = m_truthToTrack->makePerigeeParameters(initialParticle);

  return initialMomentum;

}
//==================================================================================================
double Trk::SimpleAmbiguityProcessorTool::momentumLostByBrem( const HepMC::GenEvent* genEvent ) const
{

 
  double bremPhotonEnergy(0.);
 
  // Loop over all verticies and determine which are associated to the original particle
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();
 
  for ( ; vertex != genEvent->vertices_end(); ++vertex ){
 
    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;
 
    // Loop over all particles in vertex. Determine if it is a vertex of the original particle
    HepMC::GenVertex::particle_iterator particleAtVertex = (*vertex)->particles_begin(HepMC::family);
 
    for ( ; particleAtVertex != (*vertex)->particles_end(HepMC::family); ++particleAtVertex ){
 
      if ( (*particleAtVertex)->pdg_id() == 22 )
	bremPhotonEnergy += (*particleAtVertex)->momentum().e();
 
    }
 
  }  

  return bremPhotonEnergy;

}
//=================================================================================================
const std::vector<double> Trk::SimpleAmbiguityProcessorTool::fractionOfIncidentMomentumLostPerVertex( const HepMC::GenEvent* genEvent ) const
{
  
  // Don't forget: Some of the 4-momentum of incident electron always transfered to nucleus. That's why no brem in vacuum.

  std::vector<double> fractionOfIncidentMomentumLostPerVertex;

  // Loop over all verticies and find those associated to the original track
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();

  for ( ; vertex != genEvent->vertices_end(); ++vertex ){

    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;

    // One incident particle only. THIS IS HARDWIRED TO ASSUME FIRST PARTICLE IS INCIDENT
    HepMC::GenVertex::particle_iterator incidentParticle = (*vertex)->particles_begin(HepMC::family);
    double incidentEnergy = (*incidentParticle)->momentum().e();

    // Loop over outgoing particles and extract the photon
    double photonEnergy(0.);

    HepMC::GenVertex::particles_out_const_iterator outgoingParticle = (*vertex)->particles_out_const_begin();

    for ( ; outgoingParticle != (*vertex)->particles_out_const_end(); ++outgoingParticle ){

      if ( (*outgoingParticle)->pdg_id() == 22 )
	photonEnergy = (*outgoingParticle)->momentum().e();

    }

    // Define the fractional energy loss
    double fractionalEnergyLoss = photonEnergy / incidentEnergy;

    if ( fractionalEnergyLoss != 0. )
      fractionOfIncidentMomentumLostPerVertex.push_back( fractionalEnergyLoss );

  }

  return fractionOfIncidentMomentumLostPerVertex;

}
//=================================================================================================
const std::vector<HepPoint3D> Trk::SimpleAmbiguityProcessorTool::positionsOfBremVertices( const HepMC::GenEvent* genEvent ) const
{

  std::vector<HepPoint3D> positionsOfVertices;

  // Loop over all verticies and find those associated to the original track
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();

  for ( ; vertex != genEvent->vertices_end(); ++vertex ){

    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;

    // Loop over vertices and find ones with photons radiated
    HepMC::GenVertex::particles_out_const_iterator outgoingParticle = (*vertex)->particles_out_const_begin();

    for ( ; outgoingParticle != (*vertex)->particles_out_const_end(); ++outgoingParticle ){

      if ( (*outgoingParticle)->pdg_id() == 22 ){
	HepPoint3D photonPoint3d((*vertex)->position().x(),(*vertex)->position().y(),(*vertex)->position().z());
	positionsOfVertices.push_back( photonPoint3d );
      }
    }

  }

  return positionsOfVertices;

}
//============================================================================================================
bool Trk::SimpleAmbiguityProcessorTool::vertexAssociatedWithOriginalTrack( HepMC::GenVertex* genVertex) const
{

  bool originalParticleFlag = false;

  // Loop over all particles in vertex. Determine if it is a vertex of the original particle
  HepMC::GenVertex::particle_iterator particleAtVertex = genVertex->particles_begin(HepMC::family);

  for ( ; particleAtVertex != genVertex->particles_end(HepMC::family); ++particleAtVertex ){
      
    int originalParticle = ( (*particleAtVertex)->barcode() - 10001 ) % 1000000;

    if (originalParticle == 0 )
      originalParticleFlag = true;

  }

  return originalParticleFlag;

}
#endif


#endif
