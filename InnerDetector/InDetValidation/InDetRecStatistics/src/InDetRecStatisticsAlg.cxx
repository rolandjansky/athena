/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  file:   InDetRecStatisticsAlg.cxx
//  author: Sven Vahsen (sevahsen AT lbl DOT gov), with contributions from Andrei Gaponenko and Laurent Vacavant
//
//  to do-list:
//     o write out percentage of tracks with bad tracksummary
//     o add energy of mctracks for Michael 
//     o don't save intermediate newTracking track's to ntuple
//     o statistics prints hit association purity, (holes, shared hits in sct/pixels/b-layer, outliers, etc...)
//     o navigation between hits and tracks       
//     X check Propagator defaults (check with Andrei regarding Tool)    
//     o count tracks with without associated hits, without truth, truth without beginvertex
//     o improved navigation between truth and reconstructed tracks
//     o follow atlas naming conventions for all variable and method names

#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleData.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>
#include <memory>
#include <iostream>

// Private Helpers

// Trk

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
//#include "TrkTruthToTrack/TruthToTrack.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "TrkParameters/TrackParameters.h"       //vv
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

// Other
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "GeneratorObjects/McEventCollection.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include "InDetRecStatistics/InDetRecStatisticsAlg.h"
#include "InDetRecStatistics/TrackStatHelper.h"
#include "HepMC/GenParticle.h"
#include "TruthHelper/PileUpType.h"

#include "IdDictDetDescr/IdDictManager.h" 


//static const int s_n_maxPrimVertex= 1500;
//static const int s_n_maxtracksRec = 100000;
//static const int s_n_maxtracksMC  = 100000;
//static const int s_n_maxHits      = 500000;
//static const int s_n_maxEventHits = 1000000;
//static const int s_n_maxMatches   = 100;

using namespace std;
static const char *s_linestr  = "----------------------------------------------------------------------------------------------------------------------------------------------";
static const char *s_linestr2 = "..............................................................................................................................................";
//static const int s_ERRORVALUE = -999999;

InDet::InDetRecStatisticsAlg::InDetRecStatisticsAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_particleDataTable          (nullptr),
  m_trtID                      (nullptr),
  m_idDictMgr                  (nullptr),
  m_truthToTrack               ("Trk::TruthToTrack"),
  m_trkSummaryTool             ("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
  m_assoTool                   ("Trk::PRD_AssociationTool"),
  m_updatorHandle              ("Trk::KalmanUpdator/TrkKalmanUpdator"),
  m_updator                    (nullptr),
  m_residualPullCalculator     ("Trk::ResidualPullCalculator/ResidualPullCalculator"),
  m_McTrackCollection_key      ("TruthEvent"),
  m_trackSelectorTool          ("InDet::InDetDetailedTrackSelectorTool"),
  m_doSharedHits               (true),
  m_UseTrackSummary            (true),
  m_printSecondary             (false),
  m_minPt                      (1000),
  m_maxEta                     (2.7),
  m_maxEtaBarrel               (0.8),
  m_maxEtaTransition           (1.6),
  m_maxEtaEndcap               (2.5),
  m_fakeTrackCut               (0.9),
  m_fakeTrackCut2              (0.7),
  m_matchTrackCut              (0.5),
  m_maxRStartPrimary           (  25.0*CLHEP::mm),
  m_maxRStartSecondary         ( 360.0*CLHEP::mm),
  m_maxZStartPrimary           ( 200.0*CLHEP::mm),
  m_maxZStartSecondary         (2000.0*CLHEP::mm),
  m_minREndPrimary             ( 400.0*CLHEP::mm),
  m_minREndSecondary           (1000.0*CLHEP::mm), 
  m_minZEndPrimary             (2300.0*CLHEP::mm),
  //m_maxZIndet                (),
  m_minZEndSecondary           (3200.0*CLHEP::mm),
  m_useTrackSelection          (false),
  m_doTruth                    (true),
  m_minEtaDBM                  (2.5),
  m_maxEtaDBM                  (10.0),
  m_isUnbiased                 (0),
  m_rec_tracks_without_perigee (0),
  m_unknown_hits               (0),
  m_events_processed           (0),
  m_rec_tracks_processed       (0),
  m_gen_tracks_processed       (0),
  m_spacepoints_processed      (0)
{  
  // m_RecTrackCollection_keys.push_back(std::string("Tracks"));
  // m_TrackTruthCollection_keys.push_back(std::string("TrackTruthCollection"));

  // Algorithm properties
  declareProperty("SummaryTool",                m_trkSummaryTool);
  declareProperty("PRDAssociationTool",         m_assoTool); //lt 11.13
  declareProperty("TruthToTrackTool",           m_truthToTrack);
  declareProperty("UpdatorTool",                m_updatorHandle,
		  "Measurement updator to calculate unbiased track states");
  declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator,
		  "Tool to calculate residuals and pulls");
  declareProperty("TrackCollectionKeys",        m_RecTrackCollection_keys);
  declareProperty("McTrackCollectionKey",       m_McTrackCollection_key);
  declareProperty("TrackTruthCollectionKeys",   m_TrackTruthCollection_keys);
  declareProperty("UseTrackSelection"       ,   m_useTrackSelection);
  declareProperty("DoTruth"                 ,   m_doTruth);
  declareProperty("TrackSelectorTool"       ,   m_trackSelectorTool);
  declareProperty("DoSharedHits",               m_doSharedHits);
  declareProperty("UseTrackSummary",            m_UseTrackSummary);
  declareProperty("PrintSecondary",             m_printSecondary);
  declareProperty("minPt",		        m_minPt);
  declareProperty("maxEta",		        m_maxEta);
  declareProperty("maxEtaBarrel",               m_maxEtaBarrel  );
  declareProperty("maxEtaTransition",           m_maxEtaTransition);
  declareProperty("maxEtaEndcap",               m_maxEtaEndcap);
  declareProperty("maxEtaDBM", m_maxEtaDBM);
  declareProperty("minEtaDBM", m_minEtaDBM);
  declareProperty("fakeTrackCut",               m_fakeTrackCut);
  declareProperty("fakeTrackCut2",              m_fakeTrackCut2);
  declareProperty("matchTrackCut",              m_matchTrackCut);
  declareProperty("maxRStartPrimary",	        m_maxRStartPrimary);
  declareProperty("maxRStartSecondary",	        m_maxRStartSecondary);
  declareProperty("maxZStartPrimary",	        m_maxZStartPrimary);
  declareProperty("maxZStartSecondary",         m_maxZStartSecondary);
  declareProperty("minREndPrimary",		m_minREndPrimary);
  declareProperty("minREndSecondary",		m_minREndSecondary);
  declareProperty("minZEndPrimary",		m_minZEndPrimary);
  declareProperty("minZEndSecondary",	        m_minZEndSecondary);   
  m_idHelper  = NULL;
  m_pixelID   = NULL;
  m_sctID     = NULL;
  m_UpdatorWarning = false;
  m_pullWarning = false;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode InDet::InDetRecStatisticsAlg::initialize(){

  // Part 1: Get the messaging service, print where you are
  ATH_MSG_DEBUG("initialize()");


  //locate the StoreGateSvc and initialize our local ptr
  StatusCode sc1 = evtStore().retrieve();
  if (!sc1.isSuccess()) {
    ATH_MSG_ERROR ("Could not find StoreGateSvc");
    return StatusCode::FAILURE;
  }
  
  sc1 = getServices();           // retrieve store gate service etc
  if (sc1.isFailure()) {
    ATH_MSG_FATAL("Error retrieving services !");
    return StatusCode::FAILURE;
  }   

  if (m_RecTrackCollection_keys.size()<=0) {
    ATH_MSG_ERROR("No reco track collection specified! Aborting.");
    return StatusCode::FAILURE;
  }

  if (m_doTruth && m_RecTrackCollection_keys.size() != m_TrackTruthCollection_keys.size()) {
    ATH_MSG_ERROR("You have specified "
		  << m_RecTrackCollection_keys.size() 
		  << " TrackCollection keys, and " <<  m_TrackTruthCollection_keys.size() 
		  << " TrackTruthCollection keys."
		  << " You have to specify one TrackTruthCollection for each" 
		  << " TrackCollection! Exiting."
		  );
    return StatusCode::FAILURE;
  }

  // ----------------------------------
  // use updator to get unbiased states
  if ( ! m_updatorHandle.empty() ) {
    if (m_updatorHandle.retrieve().isFailure()) {
      ATH_MSG_FATAL("Could not retrieve measurement updator tool: "
	  << m_updatorHandle);
      return StatusCode::FAILURE;
    }
    m_updator = &(*m_updatorHandle);
  } else {
    ATH_MSG_DEBUG(
      "No Updator for unbiased track states given, use normal states!");
    m_updator = 0;
  }
  
  
  //get residual and pull calculator
  if (m_residualPullCalculator.empty()) {
    ATH_MSG_INFO(
      "No residual/pull calculator for general hit residuals configured."
	);
    ATH_MSG_INFO(
      "It is recommended to give R/P calculators to the det-specific tool"
	<< " handle lists then.");
  } else if (m_residualPullCalculator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve "<< m_residualPullCalculator 
	<<" (to calculate residuals and pulls) ");
    
  } else {
    ATH_MSG_INFO("Generic hit residuals&pulls will be calculated in one or both "
		 << "available local coordinates");
  }

  // create one TrackStatHelper object of each trackCollection --- this is used to accumulate track and hit statistics

  struct cuts ct;
  ct.maxEtaBarrel=  m_maxEtaBarrel;
  ct.maxEtaTransition= m_maxEtaTransition;
  ct.maxEtaEndcap=        m_maxEtaEndcap;
  ct.fakeTrackCut=	  m_fakeTrackCut;
  ct.fakeTrackCut2=	  m_fakeTrackCut2;
  ct.matchTrackCut	=  m_matchTrackCut;
  ct.maxRStartPrimary	 = m_maxRStartPrimary;
  ct.maxRStartSecondary	 = m_maxRStartSecondary;
  ct.maxZStartPrimary	 = m_maxZStartPrimary;
  ct.maxZStartSecondary	 = m_maxZStartSecondary;
  ct.minREndPrimary	 = m_minREndPrimary;
  ct.minREndSecondary	 = m_minREndSecondary;
  ct.minZEndPrimary	 = m_minZEndPrimary;
  ct.minZEndSecondary  	 = m_minZEndSecondary;  
  ct.minPt               = m_minPt;
  ct.minEtaDBM = m_minEtaDBM;
  ct.maxEtaDBM = m_maxEtaDBM;

  unsigned int nCollections = 0;
  for (SG::ReadHandleKeyArray<TrackCollection>::const_iterator 
       it = m_RecTrackCollection_keys.begin(); 
       it < m_RecTrackCollection_keys.end(); ++ it) {
    InDet::TrackStatHelper * collection = 
      new TrackStatHelper(it->key(),(m_doTruth ? m_TrackTruthCollection_keys[nCollections].key() : ""), m_doTruth);  
    nCollections ++;
    collection->SetCuts(ct);        
    m_SignalCounters.push_back(collection); 
  }
  
  StatusCode sc3 = resetStatistics();     // reset all statistic counters
  if (sc3.isFailure()) {
    ATH_MSG_FATAL("Error in resetStatistics !");
    return StatusCode::FAILURE;
  }     

  ATH_CHECK( m_RecTrackCollection_keys.initialize() );
  if (m_doTruth) {
    if (!m_McTrackCollection_key.key().empty()) {
      ATH_CHECK( m_McTrackCollection_key.initialize() );
    }
    if (!m_TrackTruthCollection_keys.empty()) {
      ATH_CHECK( m_TrackTruthCollection_keys.initialize() );
    }
  }

  return StatusCode :: SUCCESS;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode InDet::InDetRecStatisticsAlg::execute() {

    ATH_MSG_DEBUG("entering execute()");

    // Get reconstructed tracks , generated tracks, and truth from storegate

    SG::ReadHandle<McEventCollection> SimTracks;

    if (m_doTruth) {
      SimTracks=SG::ReadHandle<McEventCollection>(m_McTrackCollection_key);
      if (!SimTracks.isValid()) {
        // @TODO warning ?
        ATH_MSG_WARNING("Error retrieving collections !");
        return StatusCode::SUCCESS;
      }
    }

    // Doesn't take account of pileup:
    //m_gen_tracks_processed += (*(SimTracks->begin()))->particles_size();
    m_events_processed ++;


    // select charged and stable generated tracks
    // apply pt, eta etc cuts to generated tracks
    // devide generated tracks into primary, truncated, secondary

    std::vector <std::pair<HepMC::GenParticle *,int> > GenSignal;
    //     GenSignalPrimary, GenSignalTruncated, GenSignalSecondary;   
    unsigned int inTimeStart = 0;
    unsigned int inTimeEnd   = 0;
    if (m_doTruth) selectGenSignal ((SimTracks.isValid() ? &(*SimTracks) : nullptr), GenSignal, inTimeStart, inTimeEnd);

    // step through the various reconstructed TrackCollections and 
    // corresponding TrackTruthCollections and produce statistics for each

    if (m_SignalCounters.size()<=0) {
      ATH_MSG_ERROR("No reco track collection specified! Aborting." 
	);
      return StatusCode::FAILURE;
    }

    std::vector< SG::ReadHandle<TrackCollection> > rec_track_collections = m_RecTrackCollection_keys.makeHandles();
    std::vector< SG::ReadHandle<TrackTruthCollection> > truth_track_collections;
    if (m_doTruth && !m_TrackTruthCollection_keys.empty()) {
      truth_track_collections = m_TrackTruthCollection_keys.makeHandles();
      if (truth_track_collections.size() != rec_track_collections.size()) {
        ATH_MSG_ERROR("Different number of reco and truth track collections (" << rec_track_collections.size() << "!=" << truth_track_collections.size() << ")" );
      }
    }
    if (m_SignalCounters.size() != rec_track_collections.size()) {
        ATH_MSG_ERROR("Number expected reco track collections does not match the actual number of such collections (" 
                      << m_SignalCounters.size() << "!=" << rec_track_collections.size() << ")" );
    }

    std::vector< SG::ReadHandle<TrackCollection> >::iterator rec_track_collections_iter = rec_track_collections.begin();
    std::vector< SG::ReadHandle<TrackTruthCollection> >::iterator truth_track_collections_iter = truth_track_collections.begin();
    for (std::vector <class TrackStatHelper *>::const_iterator statHelper 
	 =  m_SignalCounters.begin();
	 statHelper !=  m_SignalCounters.end();
         ++statHelper, ++rec_track_collections_iter) {
      assert( rec_track_collections_iter != rec_track_collections.end());

      ATH_MSG_DEBUG("Acessing TrackCollection " <<  m_RecTrackCollection_keys.at(rec_track_collections_iter - rec_track_collections.begin()).key());
      const TrackCollection       * RecCollection = &(**rec_track_collections_iter);
      const TrackTruthCollection  * TruthMap  = NULL;

      if (RecCollection)  ATH_MSG_DEBUG("Retrieved "
			      << RecCollection->size()
			      << " reconstructed tracks from storegate"
			);

      if (m_doTruth) {
        ATH_MSG_DEBUG("Acessing TrackTruthCollection " <<  m_TrackTruthCollection_keys.at(truth_track_collections_iter - truth_track_collections.begin()).key());
        assert( truth_track_collections_iter != truth_track_collections.end());
        TruthMap = &(**truth_track_collections_iter);
        if (TruthMap)   ATH_MSG_DEBUG("Retrieved " << TruthMap->size() 
			    << " TrackTruth elements from storegate");
        ++truth_track_collections_iter;
      }

      //start process of getting correct track summary

      if (m_doSharedHits) {
	// clear prdAssociationTool (this may be altered)
	m_assoTool->reset();
	// loop over tracks and add PRD 
	TrackCollection::const_iterator trackIt    = RecCollection->begin();
	TrackCollection::const_iterator trackItEnd = RecCollection->end();
	for ( ; trackIt != trackItEnd ; ++trackIt) {
	  if ( (m_assoTool->addPRDs(**trackIt)).isFailure() ) {
	    ATH_MSG_ERROR("could not add PRDs to association tool" 
		);
	  }
	}
      }
      
      std::vector <const Trk::Track *>          RecTracks, RecSignal;    
      selectRecSignal                     (RecCollection, RecTracks,RecSignal);

      ATH_MSG_DEBUG( 
		    "  RecTracks.size()="          << RecTracks.size()        
		    << ", GenSignal.size()="          << GenSignal.size());

      ATH_MSG_DEBUG("Accumulating Statistics...");
      (*statHelper)->addEvent    (RecCollection, 
				  RecTracks, 
				  GenSignal, 
				  TruthMap, 
				  m_idHelper, 
				  m_pixelID, 
				  m_sctID, 
				  m_trkSummaryTool.operator->(), 
				  m_UseTrackSummary,
				  &inTimeStart,
				  &inTimeEnd);

      m_rec_tracks_processed += RecCollection->size();

      for (  TrackCollection::const_iterator it = RecCollection->begin() ; 
	     it < RecCollection->end(); ++ it){
	std::vector<const Trk::RIO_OnTrack*> rioOnTracks;
	Trk::RoT_Extractor::extract( rioOnTracks, 
				     (*it)->measurementsOnTrack()->stdcont() );
	m_spacepoints_processed += rioOnTracks.size();     
      }

    }

    ATH_MSG_DEBUG("leaving execute()");
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode InDet :: InDetRecStatisticsAlg :: finalize() {

  // Part 1: Get the messaging service, print where you are
  ATH_MSG_DEBUG("finalize()");

  printStatistics();

  for (std::vector <class TrackStatHelper *>::const_iterator collection =  
       m_SignalCounters.begin(); collection !=  m_SignalCounters.end(); 
       collection++) {
    ATH_MSG_DEBUG(s_linestr2);
    delete (*collection);
  } 
  m_SignalCounters.clear();
  return StatusCode::SUCCESS;
}
 

StatusCode InDet :: InDetRecStatisticsAlg :: getServices ()
{
    // get the Particle Properties Service
    IPartPropSvc* partPropSvc = 0;
    StatusCode sc = evtStore()->service("PartPropSvc", partPropSvc, true);

    if (sc.isFailure()) {
        ATH_MSG_FATAL(" Could not initialize Particle Properties Service" 
	);
        return StatusCode::FAILURE;
    }
      
    m_particleDataTable = partPropSvc->PDT();

    //Set up ATLAS ID helper to be able to identify the RIO's det-subsystem.
  
    // Get the dictionary manager from the detector store
    const IdDictManager*  idDictMgr = 0;
    sc = detStore()->retrieve(idDictMgr, "IdDict");
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not get IdDictManager !");
      return StatusCode::FAILURE;
    }
    
    // Initialize the helper with the dictionary information.
    sc = detStore()->retrieve(m_idHelper, "AtlasID");
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not get AtlasDetectorID helper.");
      return StatusCode::FAILURE;
    }
   
   //get Pixel, SCT, TRT managers and helpers
 
   if (detStore()->retrieve(m_pixelID, "PixelID").isFailure()) {
     msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
     return StatusCode::FAILURE;
   }
   if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) {
     msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg;
     return StatusCode::FAILURE;
   }

   //retrieve the TRT helper only if not-SLHC layout used 
   sc = detStore()->retrieve(m_idDictMgr, "IdDict");
   if (sc.isFailure()) {
     std::cout << "Could not get IdDictManager !" << std::endl;
     return StatusCode::FAILURE;
   } 
   const IdDictDictionary* dict = m_idDictMgr->manager()->find_dictionary("InnerDetector");
   if(!dict) {
     std::cout << " Cannot access InnerDetector dictionary "<< std::endl;
     return StatusCode::FAILURE;
   }

   bool isSLHC = false;
   if (dict->file_name().find("SLHC")!=std::string::npos) isSLHC=true;

   if(!isSLHC){
     if (detStore()->retrieve(m_trtID, "TRT_ID").isFailure()) {
       msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
       return StatusCode::FAILURE;
     }
   }
   //
  
   if (m_UseTrackSummary) {
     if (m_trkSummaryTool.retrieve().isFailure() ) {
       ATH_MSG_FATAL("Failed to retrieve tool " 
	   << m_trkSummaryTool);
       return StatusCode::FAILURE;
     } else {
       ATH_MSG_INFO("Retrieved tool " << m_trkSummaryTool);
     }
   }

   if (m_doSharedHits) {
     if ( m_assoTool.retrieve().isFailure() ) {
       ATH_MSG_FATAL("Failed to retrieve tool " << m_assoTool);
       return StatusCode::FAILURE;
     } else {
       ATH_MSG_INFO("Retrieved tool " << m_assoTool);
     }
   }

   // AG: init truthToTrack
   if (m_doTruth) {
     if (m_truthToTrack.retrieve().isFailure() ) {
       ATH_MSG_FATAL("Failed to retrieve tool " << m_truthToTrack);
       return StatusCode::FAILURE;
     } else {
       ATH_MSG_INFO("Retrieved tool " << m_truthToTrack);
     }
   }
   
   //adding track selector tool
   if(m_useTrackSelection){
     if ( m_trackSelectorTool.retrieve().isFailure() ) {
       ATH_MSG_FATAL("Failed to retrieve tool " << m_trackSelectorTool);
       return StatusCode::FAILURE;   
     } else {
       ATH_MSG_INFO("Retrieved tool " << m_trackSelectorTool);
     }
   }
   return StatusCode :: SUCCESS;
}

StatusCode InDet :: InDetRecStatisticsAlg :: resetStatistics() {
    m_rec_tracks_without_perigee = 0;
    m_unknown_hits               = 0;
    m_events_processed           = 0;
    m_rec_tracks_processed       = 0;
    m_gen_tracks_processed       = 0;
    m_spacepoints_processed      = 0;
  
    for (std::vector<InDet::TrackStatHelper *>::const_iterator counter = 
	   m_SignalCounters.begin(); 
	 counter != m_SignalCounters.end(); ++ counter) {
      (*counter)->reset();
    }
    return StatusCode :: SUCCESS;
}

void InDet::InDetRecStatisticsAlg::selectRecSignal(const TrackCollection* RecCollection, 
						   std::vector <const Trk::Track *> & RecTracks ,
						   std::vector <const Trk::Track *> & RecSignal){
  
  int trkInd = 0;
  int nSigtrks = 0;
  
  for (  TrackCollection::const_iterator it = RecCollection->begin() ;
	 it != RecCollection->end(); ++ it){
    RecTracks.push_back(*it);
    const DataVector<const Trk::TrackParameters>* trackpara = 
      (*it)->trackParameters();
    
    if(trackpara->size() > 0){
      const Trk::TrackParameters* para = trackpara->front();
      if (para){
	if (para->pT() >  m_minPt && fabs(para->eta()) < m_maxEta) {
	  RecSignal.push_back(*it);
	  nSigtrks++;
	}
      }
    }
    else {
      m_rec_tracks_without_perigee ++;
    }
    trkInd++;
  }
 return;
}

// select charged, stable particles in allowed pt and eta range
void InDet :: InDetRecStatisticsAlg ::
selectGenSignal  (const McEventCollection* SimTracks, 
		  std::vector <std::pair<HepMC::GenParticle *,int> > & GenSignal,
		  unsigned int /*inTimeStart*/, unsigned int /*inTimeEnd*/) //'unused' compiler warning
{
  if (! SimTracks) return;

  unsigned int nb_mc_event = SimTracks->size();  
  PileUpType * put = new PileUpType(SimTracks);

  McEventCollection::const_iterator inTimeMBend;
  McEventCollection::const_iterator inTimeMBbegin;

  if (put)
    {
      inTimeMBbegin = put->in_time_minimum_bias_event_begin();
      inTimeMBend = put->in_time_minimum_bias_event_end();
    }

  for(unsigned int ievt=0; ievt<nb_mc_event; ++ievt)
    {
      const HepMC::GenEvent* genEvent = SimTracks->at(ievt);
      m_gen_tracks_processed += ((SimTracks->at(ievt)))->particles_size();
      if (put && inTimeMBbegin != inTimeMBend) // if not, inTimeStart and End are untouched
	{
	  //if (genEvent == *inTimeMBbegin) inTimeStart = ievt;
	  //if (genEvent == *inTimeMBend)   inTimeEnd   = ievt;
	}
      HepMC::GenParticle * particle = NULL;
      for (HepMC::GenEvent::particle_const_iterator it = genEvent->particles_begin();
	   it != genEvent->particles_end(); ++it)
	{
	  particle = *it;
	  // require stable particle from generation or simulation\	  s
	  if ((particle->status()%1000) != 1 )
	    continue;
	  int   pdgCode = particle->pdg_id();
	  const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgCode));
	  if (!pd) {
	    ATH_MSG_DEBUG("Could not get particle data for particle with "
			 <<"pdgCode="<<pdgCode<< ", status=" << particle->status() 
			 << ", barcode=" << particle->barcode());
	    ATH_MSG_DEBUG("GenParticle= " << particle);
	    continue;
	  }
	  float charge = pd->charge();
	  if (fabs(charge)<0.5)
	      continue;
	  if (fabs(particle->momentum().perp()) >  m_minPt  &&  
	      fabs(particle->momentum().pseudoRapidity()) < m_maxEta ) { 
	    std::pair<HepMC::GenParticle *,int> thisPair(particle,ievt);
	    GenSignal.push_back(thisPair);
	  }
	} // End of a particle iteration
    } // End of one GenEvent iteration
  delete put;
  return;
}

namespace {
    class StreamState
    {
    public:
      StreamState(std::ostream& out)
           : m_out(out), m_fmt(out.flags())
       {
       }

       ~StreamState()
       {
	   m_out.flags(m_fmt);
       }

    private:
       std::ostream& m_out;
       std::ios_base::fmtflags m_fmt;
    };
}


void InDet :: InDetRecStatisticsAlg :: printStatistics() {
  ATH_MSG_INFO(" ********** Beginning InDetRecStatistics Statistics Table ***********");
  ATH_MSG_INFO("For documentation see https://twiki.cern.ch/twiki/bin/view/Atlas/InDetRecStatistics");
  ATH_MSG_INFO("(or for guaranteed latest version: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/InnerDetector/InDetValidation/InDetRecStatistics/doc/mainpage.h?&view=markup )");
  ATH_MSG_INFO(" ********************************************************************");
  //  if(msgSvc.outputLevel() >= MSG::INFO){
  StreamState restore_precision (std::cout);
  std::cout << MSG::INFO 
	    << std::setiosflags(std::ios::fixed | std::ios::showpoint)  
	    << std::setw(7) << std::setprecision(2)
	    << s_linestr << std::endl
	    << "Summary" << std::endl 
	    << "\tProcessed              : " << m_events_processed            
	    << " events, "  << m_rec_tracks_processed        
	    << " reconstructed tracks with " << m_spacepoints_processed 
	    << " hits, and "  << m_gen_tracks_processed        
	    << " truth particles"            << std::endl
	    << "\tProblem objects        : " <<  m_rec_tracks_without_perigee 
	    << " tracks without perigee, " 
	    << m_unknown_hits << " unknown hits" <<  std::endl 
	    << "\t" << "Reco  TrackCollections : ";
  bool first = true;
  for (std::vector <class TrackStatHelper *>::const_iterator collection =  
	 m_SignalCounters.begin();
       collection !=  m_SignalCounters.end(); collection++)
    {
      if (first) { 
	first = false;
      } 
      else {
	std::cout << ", ";
      }
      std::cout << "\"" << (*collection)->key() << "\"";
    }


  if (m_doTruth)
    {
      std::cout << std::endl
                << "\t" << "TrackTruthCollections  : ";
      first = true;
      for (std::vector <class TrackStatHelper *>::const_iterator collection =  m_SignalCounters.begin();
	   collection !=  m_SignalCounters.end(); collection++)
	{
	  if (first) { 
	    first = false;
	  } 
	  else {
	    std::cout << ", ";
	  }
	  std::cout << "\"" << (*collection)->Truthkey() << "\"";
	}
    }
  
  std::cout << std::endl 
	    << s_linestr2 << std::endl
	    << "Cuts and Settings for Statistics Table" << std::endl 
	    << "\t" << "TrackSummary Statistics" << "\t" 
	    << (m_UseTrackSummary     ? "YES" : "NO") << std::endl 
	    << "\t" << "Signal                \t" << "pT > " 
	    << m_minPt/1000 << " GeV/c, |eta| < " << m_maxEta << "\t\t"
	    << "\t" << "Primary track start   \t" << "R < " 
	    << m_maxRStartPrimary << "mm and |z| < " 
	    << m_maxZStartPrimary << "mm" << std::endl
	    << "\t" << "Barrel                \t" << 0.0                
	    << "< |eta| < " << m_maxEtaBarrel     << "\t\t\t"
	    << "\t" << "Primary track end     \t" << "R > " 
	    << m_minREndPrimary   << "mm or |z| > " << m_minZEndPrimary   
	    << "mm" << std::endl
	    << "\t" << "Transition Region     \t" << m_maxEtaBarrel     
	    << "< |eta| < " << m_maxEtaTransition << "\t\t\t"
	    << "\t" << "Secondary (non-Primary) start \t" 
	    << " R < "    << m_maxRStartSecondary << "mm and" 
	    << " |z| < "  << m_maxZStartSecondary << " mm" << std::endl
	    << "\t" << "Endcap                \t" << m_maxEtaTransition 
	    << "< |eta| < " << m_maxEtaEndcap     << "\t\t\t"
	    << "\t" << "Secondary (non-primary) end   \t"
	    << " R > "    << m_minREndSecondary   << "mm or"
	    << " |z| > "  << m_minREndSecondary   << "mm" << std::endl
            << "\t" << "DBM                \t"
            << "|eta| > " << m_minEtaDBM     << std::endl
	    << "\t" << "Low prob tracks #1    \t" << "< "               
	    << m_fakeTrackCut  << " of hits from single Truth Track " 
	    << std::endl  
	    << "\t" << "Low prob tracks #2    \t" << "< "               
	    << m_fakeTrackCut2 << " of hits from single Truth Track " 
	    << std::endl  
	    << "\t" << "No link tracks        \t  Track has no link associated to an HepMC Particle" << std::endl
	    << "\t" << "Good reco tracks      \t" << "> "               
	    << m_matchTrackCut << " of hits from single Truth Track + a link !" 
	    << std::endl;
  
  for (std::vector <class TrackStatHelper *>::const_iterator collection =  
	 m_SignalCounters.begin();
       collection !=  m_SignalCounters.end(); collection++) {
    std::cout << s_linestr2 << std::endl;  
    (*collection)->print();   
  } 

  if (m_UseTrackSummary) {
    std::cout << s_linestr2 << std::endl;  
    std::cout << "Detailed Statistics for Hits on Reconstructed tracks, using TrackSummary: (Preselection of tracks as described above.)" << std::endl;
    std::cout << s_linestr2 << std::endl;  
    
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Reco Tracks                           .........................................hits/track.......................................................  " << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  in BARREL                tracks/event  blay  shrd  outl   pix  shrd  hole  gang   SCT  shrd  hole DHole   TRT  outl  TRHi  outl  alloutl   DBM    " << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    
    printTrackSummary (ETA_BARREL);
    
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  in TRANSITION region     tracks/event  blay  shrd  outl   pix  shrd  hole  gang   SCT  shrd  hole DHole   TRT  outl  TRHi  outl  alloutl   DBM    " << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    printTrackSummary (ETA_TRANSITION);

    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  in ENDCAP                tracks/event  blay  shrd  outl   pix  shrd  hole  gang   SCT  shrd  hole DHole   TRT  outl  TRHi  outl  alloutl   DBM    " << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    printTrackSummary (ETA_ENDCAP);

    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  in DBM                   tracks/event  blay  shrd  outl   pix  shrd  hole  gang   SCT  shrd  hole DHole   TRT  outl  TRHi  outl  alloutl   DBM    " << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    printTrackSummary (ETA_DBM);

    
  }

  if(m_printSecondary){
    
    std::cout<<s_linestr<<std::endl;
    std::cout<<"Statistics for Secondaries (non-Primaries)"<<std::endl;
    std::cout<< "\t" << "Secondary track start \t" 
	     << " R < "   << m_maxRStartSecondary << "mm and" 
	     << " |z| < " << m_maxZStartSecondary << " mm" << std::endl;
    std::cout<< "\t" << "Secondary track end   \t"
	     << " R > "    << m_minREndSecondary << "mm or"
             << " |z| > "  << m_minZEndSecondary << "mm" << std::endl;
    for (std::vector <class TrackStatHelper *>::const_iterator collection =  
	   m_SignalCounters.begin();
	 collection !=  m_SignalCounters.end(); collection++) {
      std::cout << s_linestr2 << std::endl;  
      (*collection)->printSecondary();   
    } 

  }
  ATH_MSG_INFO(" ********** Ending InDetRecStatistics Statistics Table ***********");
  std::cout << s_linestr << std::endl; 
  
}


void InDet :: InDetRecStatisticsAlg ::printTrackSummary (enum eta_region eta_reg) 
{
  bool printed = false;
  for (std::vector <class TrackStatHelper *>::const_iterator collection =  
	 m_SignalCounters.begin();
       collection !=  m_SignalCounters.end(); collection++) {
    printed = (*collection)->printTrackSummaryRegion(TRACK_ALL, eta_reg) || printed;
  }   
  if (printed) std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
  printed = false;
  for (std::vector <class TrackStatHelper *>::const_iterator collection =  
	 m_SignalCounters.begin();
       collection !=  m_SignalCounters.end(); collection++) {
    printed = (*collection)->printTrackSummaryRegion(TRACK_LOWTRUTHPROB, eta_reg) || printed;   
    
  }
  if (printed) std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
  for (std::vector <class TrackStatHelper *>::const_iterator collection =  
	 m_SignalCounters.begin();
       collection !=  m_SignalCounters.end(); collection++) {
    (*collection)->printTrackSummaryRegion(TRACK_LOWTRUTHPROB2, eta_reg);
  }
}

// =================================================================================================================
// calculatePull 
// =================================================================================================================
float InDet :: InDetRecStatisticsAlg :: calculatePull(const float residual, 
						      const float trkErr, 
						      const float hitErr){
  double ErrorSum;
  ErrorSum = sqrt(pow(trkErr, 2) + pow(hitErr, 2));
  if (ErrorSum != 0) { return residual/ErrorSum; }
  else { return 0; }
}

const Trk::TrackParameters *  InDet::InDetRecStatisticsAlg::getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement ){
    

  const Trk::TrackParameters *unbiasedTrkParameters = 0;
  
 // -----------------------------------------
  // use unbiased track states or normal ones?
  // unbiased track parameters are tried to retrieve if the updator tool
  //    is available and if unbiased track states could be produced before
  //    for the current track (ie. if one trial to get unbiased track states
  //    fail						      

  if (m_updator && (m_isUnbiased==1) ) {
    if ( trkParameters->covariance() ) {
      // Get unbiased state
      ATH_MSG_VERBOSE(" getting unbiased params");
      unbiasedTrkParameters =
	m_updator->removeFromState( *trkParameters,
				    measurement->localParameters(),
				    measurement->localCovariance());
      
      if (!unbiasedTrkParameters) {
	ATH_MSG_WARNING("Could not get unbiased track parameters, "
	    <<"use normal parameters");
	m_isUnbiased = 0;
      }
    } else if(!m_UpdatorWarning) {
      // warn only once!
      ATH_MSG_WARNING("TrackParameters contain no covariance: "
	  <<"Unbiased track states can not be calculated "
	  <<"(ie. pulls and residuals will be too small)");
      m_UpdatorWarning = true;
      m_isUnbiased = 0;
    } else {
      m_isUnbiased = 0;
    }
  } // end if no measured track parameter
  return unbiasedTrkParameters;  
}


Identifier  InDet::InDetRecStatisticsAlg::getIdentifier(const Trk::MeasurementBase* measurement ){

  
  Identifier id;
  const Trk::CompetingRIOsOnTrack *comprot = 0;
  // identify by ROT:
  const Trk::RIO_OnTrack *rot = 
    dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
  if (rot) {
    id = rot->identify();
  } else {
    // identify by CompetingROT:
    comprot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
    if (comprot) {
      rot = &comprot->rioOnTrack(comprot->indexOfMaxAssignProb());
      id = rot->identify();
    } else {
      ATH_MSG_DEBUG("measurement is neither ROT nor competingROT:"
	  <<" can not determine detector type");
      id.clear();
    }
  }
  delete comprot;
  return id; 
}
