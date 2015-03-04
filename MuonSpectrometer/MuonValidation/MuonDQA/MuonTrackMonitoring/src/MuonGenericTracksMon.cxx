/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================================================
// MuonGenericTracksMon.cxx
// ------------------------------
// AUTHORS:     N. Benekos, E. Christidi,  A. Cortes, A. Eppig, I. Nomidis, Tony Liss
// created:     November 2007
// modified June 2008: for Trigger Aware monitoring :  A. Cortes (UIUC)
// description: Implementation code for the MuonGenericTracksMon
// ============================================================================== 

#include "MuonGenericTracksMon.h"

#include "MuonIdHelpers/MuonStationIndex.h"

//#include "xAODEventInfo/EventInfo.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

// #include "TrkEventPrimitives/FitQualityOnSurface.h"
// #include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ResidualPull.h"

#include "TrkMeasurementBase/MeasurementBase.h" 
//#include "TrkParameters/TrackParameters.h"
 
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"
 
//#include "MuonDQAUtils/MuonChamberNameConverter.h"
//#include "MuonDQAUtils/MuonDQAFitFunc.h"

#include <TH1F.h>
#include <TH2F.h> 
#include <TMath.h>

// *********************************************************************
// Public Methods
// *********************************************************************

MuonGenericTracksMon::MuonGenericTracksMon( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )

  ,m_hNSegments(NULL)
  ,m_hNMuonTracks(NULL)
  ,m_hNExtrapMuonTracks(NULL)
  ,m_hNMuons(NULL)
   
  ,m_oRecoMuonSegmentPlots(0)
  ,m_oRecoMuonTrackPlots(0)
  ,m_oRecoMuonExtrapTrackPlots(0)
  ,m_oRecoMuonPlots(0)
  ,m_oRecoMuonForwPlots(0)
  ,m_oRecoMuonCaloPlots(0)

  ,m_trigDecTool("")
  ,m_useTrigger(false)
   
  ,m_storeGate(0)
   // m_activeStore(NULL),
   // m_muonMgr(NULL),
   // m_idHelper(NULL),
   // m_mdtIdHelper(NULL),
   // m_rpcIdHelper(NULL),
   // m_tgcIdHelper(NULL),
   // m_muondqafitfunc("Muon::MuonDQAFitFunc/MuonDQAFitFunc")
{
  m_pullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");
    
  //--> flags
  declareProperty( "MuonTriggerChainName", 	m_MuonTriggerChainName);
  declareProperty( "Muon_Trigger_Items", 	m_muon_triggers);
  declareProperty( "UseTriggerVector", 		m_useTrigger); 

  //--> tools
  declareProperty( "TriggerDecisionTool", 	m_trigDecTool);
  
  //--> container names
  declareProperty( "MuonCollection",            m_muonsName = "Muons");
  declareProperty( "MuonSegmentCollection",     m_muonSegmentsName = "MuonSegments");
  declareProperty( "MuonTrackCollection",       m_muonTracksName = "MuonSpectrometerTrackParticles");
  declareProperty( "MuonExtrapolatedTrackCollection", m_muonExtrapTracksName = "ExtrapolatedMuonTrackParticles");
}

//======================================================================================//
StatusCode MuonGenericTracksMon:: initialize()
//======================================================================================//
{ 
  StatusCode sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;
 
  sc = setupTools();
   
  return sc;
}


//======================================================================================//
StatusCode MuonGenericTracksMon::bookHistograms()
//======================================================================================//
{
  
  ATH_MSG_DEBUG("In bookHistograms()");
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {
 
    // if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // // book histograms that are only made in the online environment...
    // }

    // if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // // book histograms that are only relevant for cosmics data...
    // } 


      MgmtAttr_t attr = ATTRIB_MANAGED;

      std::vector<std::string> sRegion(0);
      for (int i=0; i<Muon::EnumDefs::nDetRegions; i++) {
	sRegion.push_back( toString( (Muon::EnumDefs::DetRegion) i ) );
      }
	       
      std::string sPathRoot = "Muon/MuonTrackMonitoring/NoTrigger";

      
      // //MonGroup mongroup_segments_shift(this, sPathSegments,  run, attr );	    
      // MonGroup mongroup_msTracks_shift(this, sPathMSTracks,  run, attr );
      // MonGroup mongroup_meTracks_shift(this, sPathMETracks,  run, attr );
      // MonGroup mongroup_muons_shift(this, sPathMuons,  run, attr );
      
      if(newEventsBlock){}
      if(newLumiBlock){}
      if(newRun) {
	std::string sPathOverview = sPathRoot+"/Overview";

	m_hNSegments = new TH1F(pathToHistName(sPathOverview+"_nSegments").c_str(),"Number of Muon Segments;#segments;Entries", 100,-0.5, 99.5);
	
	m_hNMuonTracks = new TH1F(pathToHistName(sPathOverview+"_nMuonTracks").c_str(),"Number of Muon Tracks;#tracks;Entries",21,-0.5,20.5);
	m_hNExtrapMuonTracks = new TH1F(pathToHistName(sPathOverview+"_nExtrapMuonTracks").c_str(),"Number of Extrapolated Muon Tracks;#tracks;Entries",21,-0.5,20.5);
	m_hNMuons = new TH1F(pathToHistName(sPathOverview+"_nMuons").c_str(),"Number of Muons;#muons;Entries",21,-0.5,20.5);

	MonGroup mongroup_overview(this, sPathOverview,  run, attr );
	bookInMongroup(m_hNSegments, mongroup_overview).ignore();
	bookInMongroup(m_hNMuonTracks, mongroup_overview).ignore();
	bookInMongroup(m_hNExtrapMuonTracks, mongroup_overview).ignore();
	bookInMongroup(m_hNMuons, mongroup_overview).ignore();	

	for (int i=0; i<Muon::EnumDefs::nDetRegions; i++) {
	  std::string sPath;
	  
	  sPath = sPathRoot+"/Segments/"+sRegion[i];
	  MonGroup mongroup_segments(this, sPath, run, attr );
	  m_oRecoMuonSegmentPlots.push_back( new RecoMuonSegmentPlots(0,sPath) );
	  bookInMongroup(*m_oRecoMuonSegmentPlots[i], mongroup_segments).ignore();

	  if ( (Muon::EnumDefs::DetRegion)i == Muon::EnumDefs::GLOBAL ) {
	    sPath = sPathRoot+"/MSOnlyTracks/"+sRegion[i];
	    m_oRecoMuonTrackPlots.push_back( new RecoMuonTrackPlots(0,sPath) );
	    MonGroup mongroup_mstracks(this, sPath, run, attr );
	    bookInMongroup(*m_oRecoMuonTrackPlots[i], mongroup_mstracks ).ignore();

	    sPath = sPathRoot+"/MSExtrapolatedTracks/"+sRegion[i];		    
	    m_oRecoMuonExtrapTrackPlots.push_back( new RecoMuonTrackPlots(0,sPath) );
	    MonGroup mongroup_metracks(this, sPath, run, attr );
	    bookInMongroup(*m_oRecoMuonExtrapTrackPlots[i], mongroup_metracks).ignore();

	    sPath = sPathRoot+"/Muons/"+sRegion[i];
	    m_oRecoMuonPlots.push_back( new RecoMuonPlots(0,sPath) );
	    MonGroup mongroup_mutracks(this, sPath, run, attr );
	    bookInMongroup(*m_oRecoMuonPlots[i], mongroup_mutracks).ignore();

	    sPath = sPathRoot+"/MuonsForward/"+sRegion[i];
	    m_oRecoMuonForwPlots.push_back( new RecoMuonPlots(0,sPath) );
	    MonGroup mongroup_mufwtracks(this, sPath, run, attr );       	    
	    bookInMongroup(*m_oRecoMuonForwPlots[i], mongroup_mufwtracks).ignore();

	    sPath = sPathRoot+"/MuonsCalo/"+sRegion[i];
	    m_oRecoMuonCaloPlots.push_back( new RecoMuonPlots(0,sPath) );
	    MonGroup mongroup_mucalotracks(this, sPath, run, attr );       	   
	    bookInMongroup(*m_oRecoMuonCaloPlots[i], mongroup_mucalotracks).ignore();

	  }
	}	
      }

    //  } // loop over Muon Triggers
  
  } //Environment
  
  return StatusCode::SUCCESS;  
}

StatusCode MuonGenericTracksMon::bookInMongroup(TH1* hist, MonGroup& mongroup)
{
  ATH_MSG_DEBUG ("Initializing " << hist << " " << hist->GetName() << "...");
  return (mongroup.regHist(hist));
  //return bookInMongroup( HistData(hist,hist->GetName()), mongroup );
}

StatusCode MuonGenericTracksMon::bookInMongroup(HistData& hist, MonGroup& mongroup)
{
  ATH_MSG_DEBUG ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
  ATH_CHECK(mongroup.regHist(hist.first));
  return StatusCode::SUCCESS;
}

StatusCode MuonGenericTracksMon::bookInMongroup(PlotBase& valPlots, MonGroup& mongroup)
{
  valPlots.initialize();
   std::vector<HistData> hists = valPlots.retrieveBookedHistograms(); // HistData -> std::pair<TH1*, std::string> 
  for (auto hist: hists) bookInMongroup(hist,mongroup).ignore();
  return StatusCode::SUCCESS;
}

//======================================================================================//
StatusCode MuonGenericTracksMon::fillHistograms()
//======================================================================================//
{
  ATH_MSG_DEBUG("In fillHistograms()");

  // const xAOD::EventInfo* eventInfo = nullptr;
  // if (evtStore()->retrieve(eventInfo).isFailure()) {
  //   ATH_MSG_ERROR ( " Cannot access to event info " );
  //   return StatusCode::SUCCESS;
  // }		  
  //ATH_MSG_DEBUG("LumiBlock: " << eventInfo->lumiBlock() << ", Event: " << eventInfo->eventNumber());

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {

    //MS track particles
    const xAOD::TrackParticleContainer* MSTracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_muonTracksName );
    if (!MSTracks) {
      ATH_MSG_WARNING ("Couldn't retrieve muon track container: " << m_muonTracksName);
      return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("Retrieved " << m_muonTracksName << ": "<< MSTracks->size());

    m_hNMuonTracks->Fill(MSTracks->size());

    for(const auto trk : *MSTracks) {
      m_oRecoMuonTrackPlots[Muon::EnumDefs::GLOBAL]->fill(*trk);
      FillPullResid(m_oRecoMuonTrackPlots[Muon::EnumDefs::GLOBAL],trk);
    }

    //Extrapolated muon track particles
    const xAOD::TrackParticleContainer* METracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_muonExtrapTracksName );
    if (!METracks) {
      ATH_MSG_WARNING ("Couldn't retrieve muon track container: " << m_muonExtrapTracksName);
      return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("Retrieved " << m_muonExtrapTracksName << ": "<< METracks->size());

    m_hNExtrapMuonTracks->Fill(METracks->size());
    for(const auto trk : *METracks) {
      m_oRecoMuonExtrapTrackPlots[Muon::EnumDefs::GLOBAL]->fill(*trk);
    }

    //Muons
    const xAOD::MuonContainer* Muons = evtStore()->retrieve< const xAOD::MuonContainer >( m_muonsName );
    if (!Muons) {
      ATH_MSG_WARNING ("Couldn't retrieve Muons container with key: " << m_muonsName);
      return StatusCode::SUCCESS;
    }

    m_hNMuons->Fill(Muons->size());
    ATH_MSG_DEBUG("Retrieved muons " << Muons->size());

    for(const auto mu : *Muons) {
      //Calo Muons
      if (mu->author()==xAOD::Muon::CaloTag || mu->author()==xAOD::Muon::CaloLikelihood) {
	m_oRecoMuonCaloPlots[Muon::EnumDefs::GLOBAL]->fill(*mu);
      }
      else {
	//Forward muons (not in ID acceptance)
	if (fabs(mu->eta())>2.5) {
	  m_oRecoMuonForwPlots[Muon::EnumDefs::GLOBAL]->fill(*mu);
	} else {
	  //Standard muons
	  m_oRecoMuonPlots[Muon::EnumDefs::GLOBAL]->fill(*mu);
	}
      }
    }

    //Segments
    const xAOD::MuonSegmentContainer* MuonSegments = evtStore()->retrieve< const xAOD::MuonSegmentContainer >( m_muonSegmentsName );
    if (!MuonSegments) {
      ATH_MSG_WARNING ("Couldn't retrieve MuonSegments container with key: " << m_muonSegmentsName);
      return StatusCode::SUCCESS;
    } 
    ATH_MSG_DEBUG("Retrieved muon segments " << MuonSegments->size());

    m_hNSegments->Fill(MuonSegments->size());
    std::vector<xAOD::MuonSegment*> goodSegments(0);
    
    for(const auto muSeg : *MuonSegments) {
      if (isGoodSegment(muSeg)) goodSegments.push_back(muSeg);

      Muon::EnumDefs::DetRegion detReg = getDetRegion(muSeg); //identify the detector region in which the segment is located
      m_oRecoMuonSegmentPlots[detReg]->fill(*muSeg);
      m_oRecoMuonSegmentPlots[Muon::EnumDefs::GLOBAL]->fill(*muSeg);
    }
    //for trk-seg match plots
    m_oRecoMuonSegmentPlots[Muon::EnumDefs::GLOBAL]->fill(goodSegments,*MSTracks); 

  } //environment condition

  return StatusCode::SUCCESS;
}

//======================================================================================//
StatusCode MuonGenericTracksMon::procHistograms()
//======================================================================================//
{ 
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {
  
    ATH_MSG_DEBUG("MuonTrackMonitoring procHistograms()");
    
    if(endOfEventsBlock){}
    if(endOfLumiBlock){}
    if(endOfRun) {

      for (auto plots : m_oRecoMuonSegmentPlots) delete plots;
      for (auto plots : m_oRecoMuonTrackPlots) delete plots;
      for (auto plots : m_oRecoMuonExtrapTrackPlots) delete plots;
      for (auto plots : m_oRecoMuonPlots) delete plots;
      for (auto plots : m_oRecoMuonForwPlots) delete plots;
      for (auto plots : m_oRecoMuonCaloPlots) delete plots;
      
    }// isEndOfRun

  } //environment

  return StatusCode::SUCCESS;   
}


//======================================================================================//
StatusCode MuonGenericTracksMon::finalize()
//======================================================================================//
{ 

  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;
  
  return StatusCode::SUCCESS;
}


//======================================================================================//
StatusCode MuonGenericTracksMon::setupTools()
//======================================================================================//
{
  //initializing tools
  // Retrieve the StoreGate service
  StatusCode sc = StatusCode::SUCCESS;
  sc = service( "StoreGateSvc", m_storeGate );

  if ( sc.isFailure() ){
    ATH_MSG_FATAL( "Unable to retrieve the StoreGate service... Exiting!" );
    return sc;
  }
  ATH_MSG_DEBUG( "Defined detector service" );

  // // retrieve the active store
  // sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  // if (sc != StatusCode::SUCCESS ) {
  //   ATH_MSG_ERROR( " Cannot get ActiveStoreSvc " );
  //   return sc ;
  // }

  // // Initialize the IdHelper
  // StoreGateSvc* detStore = 0;
  // sc = service("DetectorStore", detStore);
  // if (sc.isFailure()) {
  //   ATH_MSG_FATAL( "DetectorStore service not found !" );
  //   return sc;
  // }   

  // // Retrieve the MuonDetectorManager  
  // sc = detStore->retrieve(m_muonMgr);
  // if (sc.isFailure()) {
  //   ATH_MSG_FATAL( "Cannot get MuonDetectorManager from detector store" );
  //   return sc;
  // }  
  // ATH_MSG_DEBUG( " Found the MuonDetectorManager from detector store. " );

  // sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  // if (sc.isFailure()) {
  //   ATH_MSG_FATAL( "Cannot get MdtIdHelper" );
  //   return sc;
  // }  
  // ATH_MSG_DEBUG( " Found the MdtIdHelper " );

  // sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  // if (sc.isFailure()) {
  //   ATH_MSG_ERROR(  "Can't retrieve RpcIdHelper" );
  //   return sc;
  // }	 
  // ATH_MSG_DEBUG( " Found the RpcIdHelper " );

  // sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  // if (sc.isFailure()) {
  //   ATH_MSG_ERROR( "Can't retrieve TgcIdHelper" );
  //   return sc;
  // }	   
  // ATH_MSG_DEBUG( " Found the TgcIdHelper " );

  sc = m_pullCalculator.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Can't retrieve " << m_pullCalculator );
    return sc;
  }
  ATH_MSG_DEBUG( " Found the " << m_pullCalculator );

  sc = m_helperTool.retrieve();
  if (!sc.isSuccess()){
    ATH_MSG_FATAL( "Could not get " << m_helperTool );
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_helperTool );

  // sc = m_printer.retrieve();
  // if (!sc.isSuccess()){
  //   ATH_MSG_FATAL( "Could not get " << m_printer ); 
  //   return sc;
  // }       
  // ATH_MSG_DEBUG( "Retrieved " << m_printer );

  // sc = m_idHelperTool.retrieve();
  // if (!sc.isSuccess()){
  //   ATH_MSG_FATAL( "Could not get " << m_idHelperTool ); 
  //   return sc;
  // }
  // ATH_MSG_DEBUG( "Retrieved " << m_idHelperTool );


  // sc = m_muondqafitfunc.retrieve();
  // if (!sc.isSuccess()){
  // ATH_MSG_FATAL( "Could not get " << m_muondqafitfunc ); 
  // return sc;
  // }
  // ATH_MSG_DEBUG( "Retrieved " << m_muondqafitfunc );

  return StatusCode::SUCCESS;
}  


bool MuonGenericTracksMon::isGoodSegment(const xAOD::MuonSegment* muSeg)
{
  const int station = Muon::MuonStationIndex::toStationIndex(muSeg->chamberIndex());
  const int TechIndex = muSeg->technology();
  const float chi2 = muSeg->chiSquared();
  const float nhits = muSeg->numberDoF() + 2;//always add 2 here

  // MDT segemnt constraint
  if ( TechIndex == 0 ) {//This is just for MDT

    int mdtNhitsMin   = m_bMdtnhitsmin;
    float mdtChi2Max  = m_bMdtchi2max;

    if (station >= 4 ) {
      mdtNhitsMin = m_eMdtnhitsmin;
      mdtChi2Max  = m_eMdtchi2max;
    }
    // BI and EI station has 8 layers instead of 6.
    // Adjust accordingly, add one more hit/hole
    if ( station == 4 || station == 0) {
      mdtNhitsMin += 1;   
    }
    // Cuts for case where dealing with MDT
    // Otherwise, dealing with CSC
    if ( chi2   > mdtChi2Max )  return false;
    if ( nhits  < mdtNhitsMin ) return false;
  }
  // CSC segment constraint
  else if (TechIndex == 1){
    // This is relatively more straightforward
    if ( chi2   > m_eCscchi2max )  return false;
    if ( nhits  < m_eCscnhitsmin ) return false;
  }
  // maybe in the future add other technologies

  // if get to here, a good segment!
  return true;
}

void MuonGenericTracksMon::FillPullResid(RecoMuonTrackPlots* plotSet, const xAOD::TrackParticle* trk){

  //get the Trk::Track from the TrackParticle
  const Trk::Track* track = trk->track(); //@@@ NUMEROUS FPE WARNINGS!!!
  if (!track){
    ATH_MSG_DEBUG("couldn't get Trk::Track");
    return; 
  }
  //Get the TSOS from the Trk::Track
  const DataVector< const Trk::TrackStateOnSurface>* trackSoS=track->trackStateOnSurfaces();
  if (!trackSoS){
    ATH_MSG_DEBUG("coudln't get DataVector<TrackStateOnSurface");
    return; 
  }
  //Loop over the TSOS
  for (const auto stateOnSurface: *trackSoS) {

    if (!stateOnSurface) {
      ATH_MSG_DEBUG("problem on iteration over TSOS");
      continue; 
    }

    if (stateOnSurface->type(Trk::TrackStateOnSurface::Outlier)){
      ATH_MSG_VERBOSE("measurement flagged as outlier.. skipping!");
      continue;
    }

    //Get the measurement
    const Trk::MeasurementBase* meas = stateOnSurface->measurementOnTrack();
    if (!meas){
      ATH_MSG_DEBUG("couldn't get Trk::MeasurementBase");
      continue; 
    }

    Identifier id = m_helperTool->getIdentifier(*meas);
    if (!id.is_valid()) {
      ATH_MSG_DEBUG("not a valid id. Possibly a pseudo-measurement");
      continue;
    }
    
    //Get the track Parameters
    const Trk::TrackParameters* trackParameters = stateOnSurface->trackParameters();
    if (!trackParameters){
      ATH_MSG_DEBUG("couldn't get Trk::TrackParameters");
      continue; 
    }

    //---------------------------------------------
    //Use tool to find residuals and pulls
    const Trk::ResidualPull* resPull = m_pullCalculator->residualPull(meas, trackParameters, Trk::ResidualPull::Unbiased);
    //---------------------------------------------
    if (!resPull){
      ATH_MSG_DEBUG("couldn't get resPull");
      continue; 
    }

    const Muon::MuonStationIndex::TechnologyIndex tech = m_idHelperTool->technologyIndex(id);
    const int sector = m_idHelperTool->sector(id); //1-16, odd=large, even=small
    const bool measuresPhi = m_idHelperTool->measuresPhi(id);
   
    plotSet->fill(*resPull, sector, tech, measuresPhi);
  }
}


Muon::EnumDefs::DetRegion MuonGenericTracksMon::getDetRegion(const xAOD::MuonSegment* muSeg)
{
  Muon::MuonStationIndex::ChIndex chid = muSeg->chamberIndex();
  if (chid==Muon::MuonStationIndex::ChUnknown) {
    ATH_MSG_WARNING("Uknown chamber index for muon segment");
    return Muon::EnumDefs::GLOBAL;
  }
  if (chid>=Muon::MuonStationIndex::BIS && chid<=Muon::MuonStationIndex::BEE) {//isBarrel
    return ( (muSeg->etaIndex()>0)? Muon::EnumDefs::BA : Muon::EnumDefs::BC );
  }
  else { //isEndcap
    return ( (muSeg->etaIndex()>0)? Muon::EnumDefs::EA : Muon::EnumDefs::EC );
  }

  return Muon::EnumDefs::GLOBAL;
}
