/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetExtensionProcessor.cxx
//   Implementation file for class InDetExtensionProcessor
///////////////////////////////////////////////////////////////////
// version 0.0 Markus Elsing
///////////////////////////////////////////////////////////////////

#include "InDetExtensionProcessor/InDetExtensionProcessor.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkParameters/TrackParameters.h"

//==================================================================================================

// Constructor with parameters:
InDet::InDetExtensionProcessor::InDetExtensionProcessor(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_TrackName("Tracks"),
  m_cosmics(false),
  m_ExtensionMapName("TrackExtensionMap"),
  m_NewTrackName("ExtendedTrack"),
  m_ITrackFitter("Trk::KalmanFitter/InDetTrackFitter"),
  m_scoringTool("Trk::TrackScoringTool"),
  m_runOutlier(true),
  m_keepFailedExtensionOnTrack(true),
  m_refitPrds(true),
  m_matEffects(3),
  m_suppressHoleSearch(false),
  m_tracks(0),
  m_trackExtensionMap(0),
  m_newtracks(0),
  m_ParticleHypothesis(Trk::undefined),
  m_Nevents(0),m_Ninput(4),m_Nrecognised(4),m_Nextended(4),m_Nrejected(4),m_Nfailed(4),
  m_NrecoveryBremFits(4),m_NbremFits(4),m_Nfits(4),m_NnotExtended(4),m_NlowScoreBremFits(4),m_NextendedBrem(4)
{  
  m_etabounds.push_back(0.8);
  m_etabounds.push_back(1.6);
  m_etabounds.push_back(2.10);

  // Get parameter values from jobOptions file
  declareProperty("TrackName",          m_TrackName, "Name of the input Trackcollection");
  declareProperty("Cosmics",            m_cosmics, "switch whether we are running on cosmics");
  declareProperty("ExtensionMap",       m_ExtensionMapName,"Name of the input extension map");
  declareProperty("NewTrackName",       m_NewTrackName,"Name of the output Trackcollection");
  declareProperty("TrackFitter",        m_ITrackFitter,"Toolhandle for the track fitter");
  declareProperty("ScoringTool",        m_scoringTool,"Toolhandle for the track scorer");
  declareProperty("runOutlier",         m_runOutlier,"switch whether to run outlier logics or not");
  declareProperty("keepFailedExtension",m_keepFailedExtensionOnTrack,"switch whether to keep failed extension as outlier hits on the new track");
  declareProperty("RefitPrds",          m_refitPrds,"switch whether to do the fit with re-calibrated clusters (true) or not");
  declareProperty("matEffects",         m_matEffects,"particle hypothesis to assume for material effects in track fit");
  declareProperty("suppressHoleSearch", m_suppressHoleSearch,"suppressing hole search for comparison");
  declareProperty("tryBremFit"        , m_tryBremFit         = false,"brem recover mode");
  declareProperty("caloSeededBrem"    , m_caloSeededBrem     = false,"calo seeded brem recovery");
  declareProperty("pTminBrem"         , m_pTminBrem          = 1000.,"min pT for trying a brem recovery");
  declareProperty("etaBounds",          m_etabounds,"eta intervals for internal monitoring");
}

//==================================================================================================

// Initialize method:
StatusCode InDet::InDetExtensionProcessor::initialize()
{

  ATH_MSG_INFO( "InDetExtensionProcessor::initialize()"  );

  // get track fitter tool
  if ( m_ITrackFitter.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_ITrackFitter  );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_ITrackFitter  );
  }
  
  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_ParticleHypothesis = particleSwitch.particle[m_matEffects];

  // get scoring tool
  if ( m_scoringTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_scoringTool  );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_scoringTool  );
  }
  
  // brem fitting enabled ?
  if (m_tryBremFit)
    ATH_MSG_INFO( "Try brem fit and recovery for electron like tracks."  );

  for (int i=0; i<4; i++) {
    m_Ninput[i]     = 0;
    m_Nrecognised[i]= 0;
    m_Nextended[i]  = 0;
    m_NextendedBrem[i]  = 0;
    m_Nrejected[i]  = 0;
    m_Nfailed[i]    = 0;
    m_NrecoveryBremFits[i] = 0;
    m_NbremFits[i]  = 0;
    m_Nfits[i]      = 0;
    m_NnotExtended[i]      = 0;
    m_NlowScoreBremFits[i] = 0;
  }

  return StatusCode::SUCCESS;
}

//==================================================================================================

// Execute method:
StatusCode InDet::InDetExtensionProcessor::execute() 
{
  ATH_MSG_DEBUG ("InDetExtensionProcessor::execute()");

  ++m_Nevents; // statistics

  StatusCode sc;

  // get tracks from Storegate
  if (m_TrackName != "") {
    sc = sgSvc()->retrieve(m_tracks, m_TrackName);
    if (sc.isFailure()){
      ATH_MSG_ERROR ("Track collection named " <<m_TrackName<< " not found, exit InDetExtensionProcessor.");
      return StatusCode::SUCCESS;
    }
    else{ 
      ATH_MSG_DEBUG ("Number of Tracks found : " << m_tracks->size());
      m_Ninput[InDet::InDetExtensionProcessor::iAll] += m_tracks->size(); // statistics
    }
  }
  else {
    ATH_MSG_ERROR ("m_TrackName not set");
  }

  // get track ExensionMap from Storegate
  if (m_ExtensionMapName != "") {
    sc = sgSvc()->retrieve(m_trackExtensionMap, m_ExtensionMapName);
    if (sc.isFailure()){
      ATH_MSG_WARNING( "TrackExtensionMap named " << m_ExtensionMapName 
                     << " not found, exit InDetExtensionProcessor." );
      return StatusCode::SUCCESS;
    }
    else{ 
      ATH_MSG_DEBUG ("TrackExtensionMap found, size : " << m_trackExtensionMap->size());
    }
  }
  else ATH_MSG_ERROR ("m_ExensionMapName not set");

  // create new collection of tracks to write in storegate
  m_newtracks = new TrackCollection;

  // process the extensions
  process();

  // record new track collection in storegate
  sc = sgSvc()->record(m_newtracks,m_NewTrackName,false);
  if (sc.isFailure()){
    ATH_MSG_FATAL ("New Track Container could not be recorded in StoreGate !");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG ("Container '" <<m_NewTrackName<< "' recorded in StoreGate, size : " << m_newtracks->size());

  if (m_newtracks->size() != m_tracks->size())
   ATH_MSG_WARNING ("Lost tracks after extension ??? This is a bug !");
  
  return StatusCode::SUCCESS;
}

//==================================================================================================

void InDet::InDetExtensionProcessor::process() {

  // loop over tracks
  for (TrackCollection::const_iterator itr  = (*m_tracks).begin(); itr < (*m_tracks).end(); itr++)
  {
    // statistics
    increment_by_eta( m_Ninput, *itr, false );

    ATH_MSG_DEBUG ("evaluate input track : " << *itr);

    // try to find this track in extension map
    TrackExtensionMap::const_iterator itEx = m_trackExtensionMap->find(*itr);

    // copy input track if this track is not found in list
    if (itEx == m_trackExtensionMap->end()) {

      ATH_MSG_DEBUG ("track not in extension map, copy original track to output");

      // copy track, set pattern info 
      Trk::Track * ntrk = new Trk::Track(**itr);
      ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
      m_newtracks->push_back(ntrk);

      // statistics
      increment_by_eta( m_NnotExtended, *itr);
	    
    } else {

      ATH_MSG_DEBUG ("track found in extension map, processing...");

      // statistics
      increment_by_eta( m_Nrecognised, *itr);

      // setup new track
      Trk::Track* newtrack=0;
      // keep old track info
      const Trk::TrackInfo old_info = (*itr)->info();
      // keep list of PRDs
      std::vector<const Trk::PrepRawData*> vecPrd;

      // --- refit PRD ?
      if (m_refitPrds) {
	
	// insert PrepRawData
	std::vector<const Trk::MeasurementBase*>::const_iterator it    = itEx->second.begin();
	std::vector<const Trk::MeasurementBase*>::const_iterator itend = itEx->second.end();
	for (;it!=itend;++it) 	{
	  const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (*it);
	  if (!rot) ATH_MSG_ERROR ("cast to ROT failed, should not happen here !");
	  else vecPrd.push_back(rot->prepRawData());
	} //end of loop over prds
	
	// we have the PRD Vector, we need to refit the track and see if it is better
	ATH_MSG_DEBUG ("fit track "<<*itr<< " with PRDs, number : " << vecPrd.size());
	
	if (!m_cosmics) {
	  // normal event processing
	  
	  if (m_tryBremFit && (*itr)->info().trackProperties(Trk::TrackInfo::BremFit)) {
	    
	    ATH_MSG_DEBUG ("brem fit of electron like track");
	    //statistics
	    increment_by_eta(m_NbremFits,*itr);
	    // try brem fit of combined track
	    newtrack = m_ITrackFitter->fit(**itr,vecPrd,m_runOutlier,Trk::electron);
	    
	  } else {
	    
	    ATH_MSG_DEBUG ("normal fit track");
	    //statistics
	    increment_by_eta(m_Nfits,*itr);
	    // normal fit of combined track
	    newtrack = m_ITrackFitter->fit(**itr,vecPrd,m_runOutlier,m_ParticleHypothesis);
	    
	    if (!newtrack && m_tryBremFit &&
		(*itr)->trackParameters()->front()->pT() > m_pTminBrem &&
		(!m_caloSeededBrem || (*itr)->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))) {
	      
	      ATH_MSG_DEBUG ("normal fit track failed, try to recover with brem fit");
	      // statistics
	      increment_by_eta(m_NrecoveryBremFits,*itr);
	      // try brem fit of combined track
	      newtrack = m_ITrackFitter->fit(**itr,vecPrd,m_runOutlier,Trk::electron);
	      
	    }
	  }
	  
	} else {
	  // ===> special case for cosmics

	  std::vector<const Trk::PrepRawData*> vecPrdComb;
	  // get track parameters
	  const Trk::Perigee* silicon_per = (*itr)->perigeeParameters();

	  if (!silicon_per) {

	    ATH_MSG_DEBUG ("no perigee found on track, copy input");
	    // statistics
	    increment_by_eta( m_Nfailed, *itr);

	    // copy track, set pattern info 
	    Trk::Track * ntrk = new Trk::Track(**itr);
	    ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
	    m_newtracks->push_back(ntrk);
	    
	  } else {

	    // loop over extension ROTs and add PRD it into vecPrdComb
	    DataVector<const Trk::MeasurementBase>::const_iterator RIOit=itEx->first->measurementsOnTrack()->begin();
	    for ( ;RIOit!=itEx->first->measurementsOnTrack()->end();RIOit++) {
	      const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (*RIOit);
	      if (!rot) ATH_MSG_ERROR ("cast to ROT failed, should not happen here !");
	      else vecPrdComb.push_back(rot->prepRawData());
	    }
	    
	    // merge lists
	    for (unsigned int i=0;i<vecPrd.size();i++) {
	      double inprod=(vecPrd[i]->detectorElement()->surface(vecPrd[i]->identify()).center()-silicon_per->position()).dot(silicon_per->momentum());
	      if (inprod<0){
		vecPrdComb.insert(vecPrdComb.begin(),vecPrd[i]);
	      } else {
		vecPrdComb.push_back(vecPrd[i]);
	      }     
	    }

	    //statistics
	    increment_by_eta(m_Nfits,*itr);
	    // normal fit of combined track
	    newtrack = m_ITrackFitter->fit(vecPrdComb,*silicon_per,m_runOutlier,m_ParticleHypothesis);
	    
	  }
	}  

      } else {
	  
	// we have an extension map with ROTs, we need to refit the track and see if it is better
	ATH_MSG_DEBUG ("fit track "<<*itr<<" with ROTs, number : " << itEx->second.size());
	
	if (!m_cosmics) {
	  
	  if (m_tryBremFit && (*itr)->info().trackProperties(Trk::TrackInfo::BremFit) ) {
	    
	    ATH_MSG_DEBUG ("brem fit of electron like track");
	    //statistics
	    increment_by_eta(m_NbremFits,*itr);
	    // try brem fit of combined track
	    newtrack = m_ITrackFitter->fit(**itr,itEx->second,m_runOutlier,Trk::electron);
	    
	  } else {
	    
	    ATH_MSG_DEBUG ("normal fit track");
	    //statistics
	    increment_by_eta(m_Nfits,*itr);
	    // fit combined track 
	    newtrack = m_ITrackFitter->fit(**itr,itEx->second,m_runOutlier,m_ParticleHypothesis);
	    
	    if (!newtrack && m_tryBremFit &&
		(*itr)->trackParameters()->front()->pT() > m_pTminBrem &&
		(!m_caloSeededBrem || (*itr)->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))) {

	      ATH_MSG_DEBUG ("normal fit track failed, try to recover with brem fit");
	      // statistics
	      increment_by_eta(m_NrecoveryBremFits,*itr);
	      // try to recover with brem fit
	      newtrack = m_ITrackFitter->fit(**itr,itEx->second,m_runOutlier,Trk::electron);
	    }
	  }
	  
	} else {
	  // special case for cosmics
	  
	  Trk::MeasurementSet rotSet;
	  // get track parameters
	  const Trk::Perigee* silicon_per = (*itr)->perigeeParameters();

	  if (!silicon_per) {

	    ATH_MSG_DEBUG ("no perigee found on track, copy input");
	    // statistics
	    increment_by_eta( m_Nfailed, *itr);

	    // copy track, set pattern info 
	    Trk::Track * ntrk = new Trk::Track(**itr);
	    ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
	    m_newtracks->push_back(ntrk);
	    
	  } else {

	    // extract ROTs from extension
	    DataVector<const Trk::MeasurementBase>::const_iterator RIOit=itEx->first->measurementsOnTrack()->begin();
	    for ( ;RIOit!=itEx->first->measurementsOnTrack()->end();RIOit++) {
	      rotSet.push_back(*RIOit);
	    }
	    
	    // merge ROT lists
	    for (unsigned int i=0;i<itEx->second.size();i++){
	      double inprod = (itEx->second[i]->associatedSurface().center()-silicon_per->position()).dot(silicon_per->momentum());
	      if (inprod<0){
		rotSet.insert(rotSet.begin(),itEx->second[i]);
	      } else {
		rotSet.push_back(itEx->second[i]);
	      }     
	    }
	    
	    ATH_MSG_DEBUG ("normal fit track");
	    //statistics
	    increment_by_eta(m_Nfits,*itr);
	    // fit combined track 
	    newtrack=m_ITrackFitter->fit(rotSet,*silicon_per,m_runOutlier,m_ParticleHypothesis);
	  }
	}
      }
	
      if(!newtrack) {
	
	ATH_MSG_DEBUG ("refit of extended track failed, copy original track to output");
	// statistics
	increment_by_eta( m_Nfailed, *itr);
	
	// push track into output, does copy, needs fixing
	if (m_keepFailedExtensionOnTrack) {
	  m_newtracks->push_back( trackPlusExtension( *itr, itEx->second ) );
	} else {
	  // copy track, set pattern info 
	  Trk::Track * ntrk = new Trk::Track(**itr);
	  ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
	  m_newtracks->push_back(ntrk);
	}
	
      } else {
	
	// score old and new tool and decide which one to push back
	Trk::TrackScore oldScore = m_scoringTool->score( **itr, m_suppressHoleSearch );
	ATH_MSG_DEBUG ("original track has score : " << oldScore);
	Trk::TrackScore newScore = m_scoringTool->score( *newtrack, m_suppressHoleSearch );
	ATH_MSG_DEBUG ("new track has score      : " << newScore);
	
	// do we need to recover with a brem fit
	if (newScore < oldScore && m_tryBremFit &&
	    !newtrack->info().trackProperties(Trk::TrackInfo::BremFit) &&
	    (*itr)->trackParameters()->front()->pT() > m_pTminBrem &&
	    (!m_caloSeededBrem || (*itr)->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))) {
	  
	  ATH_MSG_DEBUG ("new track has low score, try to recover track using brem fit");
	  // statistics
	  increment_by_eta(m_NlowScoreBremFits,*itr);
	  
	  Trk::Track* newBremTrack=0;
	  // try to recover with brem fit
	  if (m_refitPrds) {
	    newBremTrack = m_ITrackFitter->fit(**itr,vecPrd,m_runOutlier,Trk::electron);
	  } else {
	    newBremTrack = m_ITrackFitter->fit(**itr,itEx->second,m_runOutlier,Trk::electron);
	  }
	  
	  if (newBremTrack) {
	    // score again
	    newScore = m_scoringTool->score( *newtrack, m_suppressHoleSearch );
	    ATH_MSG_DEBUG ("recovered new track has score      : "<<newScore);
	    // clean up newtrack
	    delete (newtrack);
	    // copy pointer
	    newtrack = newBremTrack;
	  }
	}
	
	if (newScore >= oldScore) {
	  
	  ATH_MSG_DEBUG ("take extended track, it's better !");
	  // statistics
	  increment_by_eta( m_Nextended, newtrack );
	  if (m_tryBremFit && newtrack->info().trackProperties(Trk::TrackInfo::BremFit))
	    increment_by_eta( m_NextendedBrem, newtrack );

	  //storing the precedent info 
	  newtrack->info().addPatternReco(old_info);
	  
	  // push track into output
	  newtrack->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
	  m_newtracks->push_back( newtrack );
	  
	} else {
	  
	  ATH_MSG_DEBUG ("take original track, new one is worse !");
	  // statistics
	  increment_by_eta( m_Nrejected, newtrack );
	  
	  // clean up newtrack
	  delete (newtrack);
	  
	  // push track into output, does copy
	  Trk::Track* ntrk = NULL;
	  if (m_keepFailedExtensionOnTrack)
	    ntrk = trackPlusExtension( *itr, itEx->second );
	  else
	    ntrk = new Trk::Track(**itr);
	  ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
	  m_newtracks->push_back(ntrk);
	}
      }
    }
  }
}

//==================================================================================================

void InDet::InDetExtensionProcessor::increment_by_eta(std::vector<int>& Ntracks,const Trk::Track* track,
						      bool updateAll) {
  if (updateAll) Ntracks[InDet::InDetExtensionProcessor::iAll] += 1;
  
  // test
  if (!track) {
    ATH_MSG_ERROR ("track pointer zero, should not happen!");
    return;
  }
  
  // use first parameter 
  if (!track->trackParameters()) {
      ATH_MSG_WARNING ("No track parameters, needed for statistics code in Trk::SimpleAmbiguityProcessorTool!");
  } else {
    double eta = track->trackParameters()->front()->eta();
    if (fabs(eta) < m_etabounds[0]) ++Ntracks[InDet::InDetExtensionProcessor::iBarrel];
    else if (fabs(eta) < m_etabounds[1]) ++Ntracks[InDet::InDetExtensionProcessor::iTransi];
    else if (fabs(eta) < m_etabounds[2]) ++Ntracks[InDet::InDetExtensionProcessor::iEndcap];
  }
}

//==================================================================================================

Trk::Track* InDet::InDetExtensionProcessor::trackPlusExtension(const Trk::Track* siTrack,
							       const std::vector<const Trk::MeasurementBase*>& extension) const
{
  DataVector<const Trk::TrackStateOnSurface>* extendedTrajectory
    = new DataVector<const Trk::TrackStateOnSurface>;
  
  int nSiStates=0, nExtStates=0;
  // copy existing si track as first part to new track - including all track pars since fit does not change
  {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it    = siTrack->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itEnd = siTrack->trackStateOnSurfaces()->end();
    for ( ; it!=itEnd; ++it, ++nSiStates) {
      assert(*it!=0); // check that is defined.
      Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface((**it));
      extendedTrajectory->push_back( tsos );
    }
  }
  
  // copy proposed (but failed) extension as second part onto new track - all hits flagged as outliers.
  {
    std::vector<const Trk::MeasurementBase*>::const_iterator it    = extension.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator itEnd = extension.end();
    for (;it!=itEnd;++it, ++nExtStates) {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
      typePattern.set(Trk::TrackStateOnSurface::Outlier);
      const Trk::TrackStateOnSurface* aState = new Trk::TrackStateOnSurface((*it)->clone(),0,0,0,typePattern);
      if (!m_cosmics) extendedTrajectory->push_back( aState );
      else{
        double inprod=((**it).associatedSurface().center()-siTrack->perigeeParameters()->position()).dot(siTrack->perigeeParameters()->momentum());
        if (inprod<0) extendedTrajectory->insert(extendedTrajectory->begin(),aState);
        else extendedTrajectory->push_back( aState );
      }
    }
  }
  Trk::Track* extTrack = new Trk::Track(siTrack->info(), 
					extendedTrajectory,
					( siTrack->fitQuality() ? siTrack->fitQuality()->clone() : 0 )
					);
  
  Trk::TrackScore extScore = m_scoringTool->score( *extTrack, m_suppressHoleSearch );
  ATH_MSG_DEBUG ("rejected extension saved as Trk::Track with " << nSiStates <<
		 " fitted hits and " << nExtStates << " additional Outliers, score :" << extScore);
  return extTrack;
}

//==================================================================================================

// Finalize method:
StatusCode InDet::InDetExtensionProcessor::finalize() 
{
  ATH_MSG_INFO (name() << "::finalize() -- statistics:");
  if (msgLvl(MSG::INFO)) {
    int iw=9;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of events processed      :   "<< m_Nevents << std::endl;
    std::cout << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- " << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of tracks at input       :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    std::cout << "  Number of not extended tracks   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    std::cout << "  Number of proposed ext. roads   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of track fits            :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + brem fits for electron tracks :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
      std::cout << "  + brem fit to recover failed fit:" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
      std::cout << "  + brem fit to recover low score :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    }
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of track fit failing     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    std::cout << "  Number of rejected extensions   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    std::cout << "  Number of successful extensions :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  including brem fitted tracks    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NextendedBrem[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NextendedBrem[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NextendedBrem[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NextendedBrem[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    }
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
              << "    definition: ( 0.0 < Barrel < " << m_etabounds[0] << " < Transition < " << m_etabounds[1]
              << " < Endcap < " << m_etabounds[2] << " )" << std::setprecision(-1) << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
  }
  
  return StatusCode::SUCCESS;
}
