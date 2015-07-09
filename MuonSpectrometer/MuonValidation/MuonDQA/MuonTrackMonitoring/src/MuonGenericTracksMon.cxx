/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================================================
// MuonGenericTracksMon.cxx
// ------------------------------
// AUTHORS:     N. Benekos, E. Christidi,  A. Cortes, A. Eppig, I. Nomidis, T. Liss
//              B. Tong, Y. Liu, G. Cree
// created:     November 2007
// modified June 2008: for Trigger Aware monitoring :  A. Cortes (UIUC)
// modified March 2015: XAOD migration and combination of track / segment / muon
// description: Implementation code for the MuonGenericTracksMon
// ============================================================================== 

#include "MuonTrackMonitoring/MuonGenericTracksMon.h"

#include "MuonIdHelpers/MuonStationIndex.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkMeasurementBase/MeasurementBase.h" 
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"
 
#include <TH1F.h>
#include <TH2F.h> 
#include <TMath.h>

// *********************************************************************
// Public Methods
// *********************************************************************

MuonGenericTracksMon::MuonGenericTracksMon( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
  m_inst_lumi_bcid(0.0),
  m_inst_lumi_lb(0.0),
  m_current_lb(-1),

  m_hNEvent(NULL),

  m_oRecoLumiPlots(0),
  m_oRecoMuonSegmentPlots(0),
  m_oRecoMuonMSTrackPlots(0),
  m_oRecoMuonMETrackPlots(0),
  m_oRecoMuonIDTrackPlots(0),
  m_oRecoMuonPlots(0),
  //m_oRecoMuonForwPlots(0),
  //m_oRecoMuonCaloPlots(0),
  m_oRecoPhysPlots(0),

  m_storeGate(0),
  m_useTrigger(true),
  m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
  m_ZmumuResonanceSelectionTool("MuonResonanceSelectionTool/ZmumuResonanceSelectionTool"),
  m_ZmumuResonancePairingTool("MuonResonancePairingTool/ZmumuResonancePairingTool"),
  m_JpsimumuResonanceSelectionTool("MuonResonanceSelectionTool/JpsimumuResonanceSelectionTool"),
  m_JpsimumuResonancePairingTool("MuonResonancePairingTool/JpsimumuResonancePairingTool"),
  m_isMC(false)
{

  m_pullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");

  declareProperty("UseTriggerVector",     m_useTrigger); 
  declareProperty("MuonTriggerChainName", m_MuonTriggerChainName);
  declareProperty("Muon_Trigger_Items",   m_muon_triggers);
  declareProperty("TriggerDecisionTool",  m_trigDecTool);
  
  declareProperty("MuonCollection",                  m_muonsName            = "Muons");
  declareProperty("MuonSegmentCollection",           m_muonSegmentsName     = "MuonSegments");
  declareProperty("MuonTrackCollection",             m_muonTracksName       = "MuonSpectrometerTrackParticles");
  declareProperty("MuonExtrapolatedTrackCollection", m_muonExtrapTracksName = "ExtrapolatedMuonTrackParticles");
  declareProperty("InDetTrackParticles",             m_innerTracksName      = "InDetTrackParticles");
  declareProperty("MuonHistSumTool",                 m_muonHitSummaryTool   = std::string("Muon::MuonHitSummaryTool/MuonHitSummaryTool"));
  declareProperty("MuonSelectorTool",                m_muonSelectionTool);
  declareProperty("ZmumuResonanceSelectionTool",     m_ZmumuResonanceSelectionTool);
  declareProperty("JpsimumuResonanceSelectionTool",  m_JpsimumuResonanceSelectionTool);
}

//======================================================================================//
StatusCode MuonGenericTracksMon::initialize()
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
  
  if(!(m_environment == AthenaMonManager::tier0    || 
       m_environment == AthenaMonManager::tier0ESD || 
       m_environment == AthenaMonManager::online   ||
       m_environment == AthenaMonManager::AOD)){

    return StatusCode::SUCCESS;
  }
    
  MgmtAttr_t attr = ATTRIB_MANAGED;

  //set the path with trigger chain name
  std::string rootpath = "MuonPhysics/" + m_MuonTriggerChainName;
  
  if(newEventsBlock){}
  if(newLumiBlock){}
  if(newRun) {

    //example of how to register a new histogram
    m_hNEvent = new TH1F("Overview_nEvent", "Number of Events;LumiBlock;Nevents", 2000, -0.5, 1999.5);
	   
    std::string dirpath;
    dirpath = rootpath + "/Overview";

    MonGroup mongroup_gen_overview(this, dirpath, run, attr ); 
    bookInMongroup(m_hNEvent, mongroup_gen_overview).ignore();

    for (int i = 0; i < SOURCE::N_SOURCE; i++) {

		dirpath = "Overview/" + sources[i];//redefine for hist name
		MonGroup mongroup_overview(this, rootpath + dirpath, run, attr);
		m_oRecoLumiPlots.push_back(new RecoLumiPlots(0, dirpath, sources[i]));
		bookInMongroup(*m_oRecoLumiPlots[i], mongroup_overview, sources[i]).ignore();

		dirpath = "Segments/" + sources[i];
		MonGroup mongroup_segments(this, rootpath + dirpath, run, attr);
		m_oRecoMuonSegmentPlots.push_back(new RecoMuonSegmentPlots(0, dirpath));
		bookInMongroup(*m_oRecoMuonSegmentPlots[i], mongroup_segments, sources[i]).ignore();

		if(i > SOURCE::ALLMUONS){//for MS tracks, only do not CB muons, since it overlaps with ME
		    dirpath = "TracksMS/" + sources[i];
		    MonGroup mongroup_mstracks(this, rootpath + dirpath, run, attr);
		    m_oRecoMuonMSTrackPlots.push_back(new RecoMuonTrackPlots(0, dirpath));
		    bookInMongroup(*m_oRecoMuonMSTrackPlots[i - 3], mongroup_mstracks, sources[i], "MS").ignore();
		}

		if(i != SOURCE::CONTAINER){//for IDME tracks and Muon, do not do container
			dirpath = "Muons/" + sources[i];
			MonGroup mongroup_mutracks(this, rootpath + dirpath, run, attr);
			m_oRecoMuonPlots.push_back(new RecoMuonPlots(0, dirpath));
			bookInMongroup(*m_oRecoMuonPlots[i], mongroup_mutracks, sources[i]).ignore();

		    dirpath = "TracksME/" + sources[i];
		    MonGroup mongroup_metracks(this, rootpath + dirpath, run, attr);
		    m_oRecoMuonMETrackPlots.push_back(new RecoMuonTrackPlots(0, dirpath));
		    bookInMongroup(*m_oRecoMuonMETrackPlots[i], mongroup_metracks, sources[i], "ME").ignore();

			dirpath = "TracksID/" + sources[i];
			MonGroup mongroup_idtracks(this, rootpath + dirpath, run, attr);
			m_oRecoMuonIDTrackPlots.push_back(new RecoMuonIDTrackPlots(0, dirpath));
			bookInMongroup(*m_oRecoMuonIDTrackPlots[i], mongroup_idtracks, sources[i], "ID").ignore();
		}

		if(i == SOURCE::Z){
		    dirpath = rootpath + "/MuonTrkPhys/" + sources[i];
		    MonGroup mongroup_Zsignal(this, dirpath, run, attr);
		    m_oRecoPhysPlots.push_back(new RecoPhysPlots(0, sources[i]+"/", sources[i]));
		    bookInMongroup(*m_oRecoPhysPlots[i], mongroup_Zsignal, sources[i]).ignore();
		}
		if(i == SOURCE::JPSI){
		    dirpath = rootpath + "/MuonTrkPhys/" + sources[i];
		    MonGroup mongroup_Jpsisignal(this, dirpath, run, attr);
		    m_oRecoPhysPlots.push_back(new RecoPhysPlots(0, sources[i]+"/", sources[i]));
		    bookInMongroup(*m_oRecoPhysPlots[i], mongroup_Jpsisignal, sources[i]).ignore();
		}
    }//end of loopoing over different sources

  }//end of new run condition

  return StatusCode::SUCCESS;  
}

StatusCode MuonGenericTracksMon::bookInMongroup(TH1* hist, MonGroup& mongroup)
{
  ATH_MSG_DEBUG ("Initializing " << hist << " " << hist->GetName() << "...");
  return (mongroup.regHist(hist));
  //return bookInMongroup( HistData(hist,hist->GetName()), mongroup);
}

StatusCode MuonGenericTracksMon::bookInMongroup(HistData& hist, MonGroup& mongroup, std::string source)
{
  ATH_MSG_DEBUG ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
  //change hist title
  TString sHistTitle = hist.first->GetTitle();
  sHistTitle = sHistTitle.Insert(0, (source + ": ").c_str());
  hist.first->SetTitle(sHistTitle);

  ATH_CHECK(mongroup.regHist(hist.first));
  return StatusCode::SUCCESS;
}

StatusCode MuonGenericTracksMon::bookInMongroup(PlotBase& valPlots, MonGroup& mongroup, std::string source)
{
  valPlots.initialize();
  std::vector<HistData> hists = valPlots.retrieveBookedHistograms(); // HistData -> std::pair<TH1*, std::string>
  for (auto hist: hists){
    bookInMongroup(hist, mongroup, source).ignore();
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonGenericTracksMon::bookInMongroup(PlotBase& valPlots, MonGroup& mongroup, std::string source, TString Montype)
{
  valPlots.initialize();
   std::vector<HistData> hists = valPlots.retrieveBookedHistograms(); // HistData -> std::pair<TH1*, std::string> 
  for (auto hist: hists) {

    TString sHistName = hist.first->GetName();
    TString sHistTitle = hist.first->GetTitle();
    //change the axis range label
    if (sHistName.Contains(source + "_pt")){
      hist.first->GetXaxis()->SetTitle("Track Transverse Momentum [GeV]");
      hist.first->GetXaxis()->SetRangeUser(0, 100);
    }
    if (sHistName.Contains(source + "_eta_phi")){
      hist.first->GetXaxis()->SetTitle("#eta");
      hist.first->GetYaxis()->SetTitle("#phi");
    }
    if (sHistName.Contains(source + "_z0")){
        hist.first->GetXaxis()->SetRangeUser(-50, 50);
    }
    if (sHistName.Contains(source + "_d0")){                
        hist.first->GetXaxis()->SetRangeUser(-0.2, 0.2);
    }
    if (sHistName.Contains("HitContent")){
        hist.first->SetName("Tracks" + Montype + "_" + source + "_" + sHistName);
    }
    //change histogram title
    if (sHistTitle.Contains("Track")) {
        sHistTitle = sHistTitle.Replace(0, 6, Montype + " Track");
        hist.first->SetTitle(sHistTitle);
    }
    else if (sHistTitle.Contains("Reco Muon")) {
         sHistTitle = sHistTitle.Replace(0, 9, Montype + " Track");
         hist.first->SetTitle(sHistTitle);
    }
    else hist.first->SetTitle(Montype + " Track" + sHistTitle);

    bookInMongroup(hist, mongroup, source).ignore();
  }
  return StatusCode::SUCCESS;
}

//======================================================================================//
StatusCode MuonGenericTracksMon::fillHistograms()
{
    ATH_MSG_DEBUG("In fillHistograms()");

    const xAOD::EventInfo* eventInfo;
    if (evtStore()->retrieve(eventInfo).isFailure()){
        ATH_MSG_ERROR ("Cannot access to event info");
        return StatusCode::SUCCESS;
    }

    if(!(m_environment == AthenaMonManager::tier0    || 
         m_environment == AthenaMonManager::tier0ESD || 
         m_environment == AthenaMonManager::online   ||
         m_environment == AthenaMonManager::AOD)){
      
        return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("LB " << eventInfo->lumiBlock() <<
                  " instant " << ManagedMonitorToolBase::lbLuminosityPerBCID() <<
                  " average " << ManagedMonitorToolBase::lbAverageLuminosity() <<
                  " duration " <<  ManagedMonitorToolBase::lbDuration() << 
                  " lbint " << ManagedMonitorToolBase::lbInteractionsPerCrossing()
                  );
    m_current_lb = eventInfo->lumiBlock();
    m_hNEvent->Fill(m_current_lb, 1);

    m_inst_lumi_bcid = ManagedMonitorToolBase::lbLuminosityPerBCID();
    if(m_inst_lumi_bcid < 0){
        ATH_MSG_DEBUG("Weird instantaneous luminosity per bcid. Setting to 0.");
        m_inst_lumi_bcid = 0;
    }
    m_inst_lumi_lb = ManagedMonitorToolBase::lbAverageLuminosity();
    if(m_inst_lumi_lb < 0){
        ATH_MSG_DEBUG("Weird instantaneous luminosity per lb. Setting to 0.");
        m_inst_lumi_lb = 0;
    }

    // retrieve containers
    const xAOD::MuonSegmentContainer* MuonSegments = evtStore()->retrieve< const xAOD::MuonSegmentContainer >(m_muonSegmentsName);
    const xAOD::TrackParticleContainer*   tracksMS = evtStore()->retrieve< const xAOD::TrackParticleContainer >(m_muonTracksName);
    const xAOD::MuonContainer*               Muons = evtStore()->retrieve< const xAOD::MuonContainer >(m_muonsName);
    //const xAOD::TrackParticleContainer* METracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_muonExtrapTracksName );
    //const xAOD::TrackParticleContainer* IDTracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_innerTracksName );
    // check validity
    if (!MuonSegments){
        ATH_MSG_WARNING ("Couldn't retrieve MuonSegments container: " << m_muonSegmentsName);
        return StatusCode::SUCCESS;
    }
    if (!tracksMS) {
        ATH_MSG_WARNING ("Couldn't retrieve MS tracks container: " << m_muonTracksName);
        return StatusCode::SUCCESS;
    }
    if (!Muons){
        ATH_MSG_WARNING ("Couldn't retrieve Muons container: " << m_muonsName);
        return StatusCode::SUCCESS;
    }
    // if (!METracks) {
    //   ATH_MSG_WARNING ("Couldn't retrieve muon track container: " << m_muonExtrapTracksName);
    //   return StatusCode::RECOVERABLE;
    // }
    // if (!IDTracks) {
    //   ATH_MSG_WARNING ("Couldn't retrieve muon track container: " << m_innerTracksName);
    //   return StatusCode::RECOVERABLE;
    // }
    // ATH_MSG_WARNING ("MS container size: " << tracksMS->size() << " ME container size: " << METracks->size());
    // select muons from Jpsi & Z
    auto muons_jpsi = m_JpsimumuResonanceSelectionTool->selectMuons(Muons, m_isMC, CP::SystematicSet());
    auto muons_Z = m_ZmumuResonanceSelectionTool->selectMuons(Muons, m_isMC, CP::SystematicSet());

    // find J/psi & Z candidates
    auto resonances_jpsi = m_JpsimumuResonancePairingTool->buildPairs(muons_jpsi);
    auto resonances_Z = m_ZmumuResonancePairingTool->buildPairs(muons_Z);

    // plot luminosity related plots
    plot_lumi(resonances_Z, resonances_jpsi, Muons, tracksMS, MuonSegments);

    // plot muons, Z
    if (m_dataType != AthenaMonManager::cosmics){
		for (auto resonance: resonances_Z)
		{
		  plot_muon(*resonance.first,  SOURCE::Z);
		  plot_muon(*resonance.second, SOURCE::Z);
		}
		plot_resonances(resonances_Z, SOURCE::Z);

		// plot muons, J/Psi
		for (auto resonance: resonances_jpsi)
		{
		  plot_muon(*resonance.first,  SOURCE::JPSI);
		  plot_muon(*resonance.second, SOURCE::JPSI);
		}
		plot_resonances(resonances_jpsi, SOURCE::JPSI);
    }

    // plot muons, all
    for (const xAOD::Muon* muon: *Muons)
    {
        if(muon->muonType() == xAOD::Muon::Combined) {
          plot_muon(*muon, SOURCE::ALLMUONS);
        }
        else {
          plot_muon(*muon, SOURCE::NONCBMUONS);
        }
    }

    // plot segments container
    for (const xAOD::MuonSegment* segment : *MuonSegments)
    {
        plot_segment(*segment, SOURCE::CONTAINER);
    }

    // plot tracks (MS) container
    for (const xAOD::TrackParticle* track: *tracksMS)
    {
        plot_track(*track, SOURCE::CONTAINER - 3);
    }
    
    //finish all the plotting
    for (const xAOD::Muon* muon : muons_jpsi.first) delete muon;
    for (const xAOD::Muon* muon : muons_jpsi.second) delete muon;
    for (const xAOD::Muon* muon : muons_Z.first) delete muon;
    for (const xAOD::Muon* muon : muons_Z.second) delete muon;
    
    return StatusCode::SUCCESS;
}

//======================================================================================//
void MuonGenericTracksMon::plot_lumi(
  std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances_Z, 
  std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances_jpsi,
  const xAOD::MuonContainer* Muons, 
  const xAOD::TrackParticleContainer*   tracksMS, 
  const xAOD::MuonSegmentContainer* MuonSegments)
//======================================================================================//
{   
    //fill all the luminoisty related plot
    m_oRecoLumiPlots[SOURCE::Z]->fill(resonances_Z, m_current_lb, m_inst_lumi_bcid, m_inst_lumi_lb);
    m_oRecoLumiPlots[SOURCE::JPSI]->fill(resonances_jpsi, m_current_lb, m_inst_lumi_bcid, m_inst_lumi_lb);
    m_oRecoLumiPlots[SOURCE::ALLMUONS]->fill_CB(Muons, m_current_lb, m_inst_lumi_bcid, m_inst_lumi_lb);
    m_oRecoLumiPlots[SOURCE::NONCBMUONS]->fill_Other(Muons, m_current_lb, m_inst_lumi_bcid, m_inst_lumi_lb);
    m_oRecoLumiPlots[SOURCE::CONTAINER]->fill(tracksMS, m_current_lb, m_inst_lumi_bcid, m_inst_lumi_lb);
    m_oRecoLumiPlots[SOURCE::CONTAINER]->fill(MuonSegments, m_current_lb, m_inst_lumi_bcid, m_inst_lumi_lb);
}


//======================================================================================//
void MuonGenericTracksMon::plot_muon(const xAOD::Muon& muon, int source)
//======================================================================================//
{   


    xAOD::Muon::Quality my_quality = m_muonSelectionTool->getQuality(muon);
    ATH_MSG_DEBUG ("Muon quality value: " << my_quality << " default value " << muon.quality());

    m_oRecoMuonPlots[source]->fill(muon);
    if (source == SOURCE::NONCBMUONS) m_oRecoMuonMSTrackPlots[source - 3]->fill(muon, MUON_COMPONENT::TRACK_MS);
    m_oRecoMuonMETrackPlots[source]->fill(muon, MUON_COMPONENT::TRACK_ME);
    m_oRecoMuonIDTrackPlots[source]->fill(muon, MUON_COMPONENT::TRACK_ID);
    m_oRecoMuonSegmentPlots[source]->fill(muon);
    m_oRecoMuonPlots[source]->fill(muon, my_quality);
}

//======================================================================================//
void MuonGenericTracksMon::plot_segment(const xAOD::MuonSegment& segment, int source)
//======================================================================================//
{

    m_oRecoMuonSegmentPlots[source]->fill(segment);
}

//======================================================================================//
void MuonGenericTracksMon::plot_track(const xAOD::TrackParticle& track, int source)
//======================================================================================//
{

    m_oRecoMuonMSTrackPlots[source]->fill(track);
}

//======================================================================================//
void MuonGenericTracksMon::plot_resonances(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int source)
//======================================================================================//
{
    m_oRecoPhysPlots[source]->fill(resonances);
 
}

//======================================================================================//
StatusCode MuonGenericTracksMon::procHistograms()
//======================================================================================//
{ 
  if(!(m_environment == AthenaMonManager::tier0    || 
       m_environment == AthenaMonManager::tier0ESD || 
       m_environment == AthenaMonManager::online   || 
       m_environment == AthenaMonManager::AOD)){

    return StatusCode::SUCCESS;
  }
  
    ATH_MSG_DEBUG("MuonTrackMonitoring procHistograms()");
    
    if(endOfEventsBlock){}
    if(endOfLumiBlock){}
    if(endOfRun) {

      //finish the post processing
      for (auto plots : m_oRecoPhysPlots)            plots->finalizeRecoPlots();

      //remove all the pointers
      for (auto plots : m_oRecoLumiPlots)            delete plots;
      for (auto plots : m_oRecoMuonSegmentPlots)     delete plots;
      for (auto plots : m_oRecoMuonMSTrackPlots)     delete plots;
      for (auto plots : m_oRecoMuonMETrackPlots)     delete plots;
      for (auto plots : m_oRecoMuonIDTrackPlots)     delete plots;
      for (auto plots : m_oRecoMuonPlots)            delete plots;
      //for (auto plots : m_oRecoMuonForwPlots)        delete plots;
      //for (auto plots : m_oRecoMuonCaloPlots)        delete plots;
      for (auto plots : m_oRecoPhysPlots)            delete plots;
      
    }

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
  StatusCode sc = StatusCode::SUCCESS;
  sc = service("StoreGateSvc", m_storeGate);

  if (sc.isFailure()){
    ATH_MSG_FATAL( "Unable to retrieve the StoreGate service... Exiting!" );
    return sc;
  }
  ATH_MSG_DEBUG( "Defined detector service" );


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
 
  sc = m_muonHitSummaryTool.retrieve();
  if (!sc.isSuccess()){
    ATH_MSG_FATAL( "Could not get " << m_muonHitSummaryTool ); 
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_muonHitSummaryTool );
 
  sc = m_ZmumuResonanceSelectionTool.retrieve();
  if (!sc.isSuccess()){
    ATH_MSG_FATAL( "Could not get " << m_ZmumuResonanceSelectionTool ); 
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_ZmumuResonanceSelectionTool );
 
  sc = m_ZmumuResonancePairingTool.retrieve();
  if (!sc.isSuccess()){
    ATH_MSG_FATAL( "Could not get " << m_ZmumuResonancePairingTool ); 
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_ZmumuResonancePairingTool );
 
  sc = m_JpsimumuResonanceSelectionTool.retrieve();
  if (!sc.isSuccess()){
    ATH_MSG_FATAL( "Could not get " << m_JpsimumuResonanceSelectionTool ); 
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_JpsimumuResonanceSelectionTool );
 
  sc = m_JpsimumuResonancePairingTool.retrieve();
  if (!sc.isSuccess()){
    ATH_MSG_FATAL( "Could not get " << m_JpsimumuResonancePairingTool ); 
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_JpsimumuResonancePairingTool );
 
  sc = m_muonSelectionTool.retrieve();
  if (!sc.isSuccess()){
    ATH_MSG_FATAL( "Could not get " << m_muonSelectionTool ); 
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_muonSelectionTool );


  return StatusCode::SUCCESS;
}  
