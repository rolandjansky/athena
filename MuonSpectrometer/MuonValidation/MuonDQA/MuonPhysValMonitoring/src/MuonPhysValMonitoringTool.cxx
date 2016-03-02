 ///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonPhysValMonitoringTool.cxx 
// Implementation file for class MuonPhysValMonitoringTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PhysVal includes
#include "MuonPhysValMonitoringTool.h"

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "MuonHistUtils/MuonEnumDefs.h"

#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"

#include "TString.h"
#include <math.h> 
typedef ElementLink< xAOD::MuonContainer > MuonLink;
typedef ElementLink< xAOD::MuonSegmentContainer > MuonSegmentLink;
typedef ElementLink< xAOD::TruthParticleContainer > TruthLink;

namespace MuonPhysValMonitoring {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

//utilities
float getMatchingProbability(const xAOD::TrackParticle & trackParticle) {
 float result(std::numeric_limits<float>::quiet_NaN());
 if (trackParticle.isAvailable<float>("truthMatchProbability")){
   result = trackParticle.auxdata<float>("truthMatchProbability" );
 }
 return result;
}

// Constructors
////////////////

MuonPhysValMonitoringTool::MuonPhysValMonitoringTool( const std::string& type, 
						      const std::string& name, 
						      const IInterface* parent ):
  ManagedMonitorToolBase( type, name, parent ),
  MSTracks(NULL),
  m_isData(false),
  m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
  m_muonPrinter("Rec::MuonPrintingTool/MuonPrintingTool"),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_trackSelector("InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedTrackSelectorTool"),
  // m_muonResonanceSelectionTool("MuonResonanceSelectionTool/MuonZSelectionTool"),
  // m_muonResonancePairingTool("MuonResonancePairingTool/ZmumuResonancePairingTool"),
  m_oUnmatchedRecoMuonPlots(NULL),
  m_oUnmatchedTruthMuonPlots(NULL),
  m_oUnmatchedRecoMuonTrackPlots(NULL),
  m_oUnmatchedRecoMuonSegmentPlots(NULL),
  h_overview_reco_category(NULL),
  h_overview_Z_mass(NULL),
  h_overview_Z_mass_ME(NULL),
  h_overview_Z_mass_ID(NULL)
{
  // default for muon chains

  declareProperty( "IsData", m_isData = false );
  declareProperty( "TrackContainerName", m_tracksName = "InDetTrackParticles" );
  declareProperty( "MuonContainerName", m_muonsName = "Muons" );
  declareProperty( "MuonTruthParticleContainerName", m_muonsTruthName = "MuonTruthParticles" );
  declareProperty( "MuonTrackContainerName", m_muonTracksName = "MuonSpectrometerTrackParticles" );
  declareProperty( "MuonExtrapolatedTrackContainerName", m_muonExtrapolatedTracksName = "ExtrapolatedMuonTrackParticles" );
  
  declareProperty( "TrackSelector", m_trackSelector);
  // declareProperty( "MuonResonanceSelectionTool",  m_muonResonanceSelectionTool);  
  // declareProperty( "MuonResonancePairingTool",    m_muonResonancePairingTool);
  
  declareProperty( "MuonSegmentContainerName", m_muonSegmentsName = "MuonSegments" );
  declareProperty( "MuonTruthSegmentContainerName", m_muonSegmentsTruthName = "MuonTruthSegments" );
  declareProperty( "SelectMuonAuthors", m_selectMuonAuthors);
  declareProperty( "SelectHLTMuonItems", m_selectHLTMuonItems);
  declareProperty( "SelectL1MuonItems", m_L1MuonItems);
  declareProperty( "SelectMuonCategories", m_selectMuonCategories );
  declareProperty( "DoMuonTrackValidation", m_doMuonTrackValidation = true);
  declareProperty( "DoMuonSegmentValidation", m_doMuonSegmentValidation = true);
  declareProperty( "DoBinnedResolutionPlots", m_doBinnedResolutionPlots = true);

  declareProperty( "DoTrigMuonValidation", m_doTrigMuonValidation = false);
  declareProperty( "DoTrigMuonL1Validation", m_doTrigMuonL1Validation = false);
  declareProperty( "DoTrigMuonL2Validation", m_doTrigMuonL2Validation = false);
  declareProperty( "DoTrigMuonEFValidation", m_doTrigMuonEFValidation = false);
  declareProperty( "L1TrigMuonContainerName", m_muonL1TrigName = "LVL1MuonRoIs");
  declareProperty( "L2SAMuonContainerName",m_muonL2SAName = "HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo");
  declareProperty( "L2CBMuonContainerName",m_muonL2CBName = "HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo");
  declareProperty( "EFCombTrigMuonContainerName", m_muonEFCombTrigName = "HLT_xAOD__MuonContainer_MuonEFInfo");
  declareProperty( "TrigDecisionKey",   m_trigDecisionKey = "TrigDecision");

  declareProperty( "DoMuonTree", m_doMuonTree = false);

  SelectedAuthor = 0;
}

// Destructor
///////////////
MuonPhysValMonitoringTool::~MuonPhysValMonitoringTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode MuonPhysValMonitoringTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  
  for (unsigned int i=0; i<m_selectHLTMuonItems.size(); i++){
    if(m_selectHLTMuonItems[i][0]=="" || m_selectHLTMuonItems[i][1]=="") continue;
    m_muonItems.push_back(m_selectHLTMuonItems[i][0]);
    m_L1Seed.push_back(m_selectHLTMuonItems[i][1]);
  }



  // setup flags
  if (m_doTrigMuonValidation==false) {
    m_doTrigMuonL1Validation = false;
    m_doTrigMuonL2Validation = false;
    m_doTrigMuonEFValidation = false;
  }
  if (m_isData) {
    m_selectMuonCategories.clear();
    m_selectMuonCategories.push_back(ALL);
    m_selectMuonCategories.push_back(PROMPT);
    m_selectMuonCategories.push_back(REST);
  }

  ATH_CHECK(m_muonSelectionTool.retrieve());
  ATH_CHECK(m_trackSelector.retrieve());
  // ATH_CHECK(m_muonResonanceSelectionTool.retrieve());
  // ATH_CHECK(m_muonResonancePairingTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MuonPhysValMonitoringTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");    
  ATH_CHECK(ManagedMonitorToolBase::finalize());

  //for (auto plots:m_muonValidationPlots) {
  //  delete plots;    plots=0;
  //}
  for (auto plots:m_TriggerMuonValidationPlots) {
    delete plots;    plots=0;
  }


  if (m_doMuonTrackValidation) {
    // for (auto plots:m_muonMSTrackValidationPlots) {
    //   delete plots;      plots = 0;
    // }
    // for (auto plots:m_muonMETrackValidationPlots) {
    //   delete plots;      plots = 0;
    // }
    // for (auto plots:m_muonIDTrackValidationPlots) {
    //   delete plots;      plots = 0;
    // }
    // for (auto plots:m_muonIDSelectedTrackValidationPlots) {
    //   delete plots;      plots = 0;
    // }

    //delete m_oUnmatchedRecoMuonTrackPlots;
    //m_oUnmatchedRecoMuonTrackPlots=0;
  }
  if (m_doMuonSegmentValidation) {
    for (auto plots:m_muonSegmentValidationPlots) {
      delete plots;      plots = 0;
    }
    if (!m_isData) {delete m_oUnmatchedRecoMuonSegmentPlots;    m_oUnmatchedRecoMuonSegmentPlots=0;}
  }

  //delete m_oUnmatchedRecoMuonPlots;  m_oUnmatchedRecoMuonPlots=0;
  //delete m_oUnmatchedTruthMuonPlots;  m_oUnmatchedTruthMuonPlots=0;
 
  return StatusCode::SUCCESS;
}

StatusCode MuonPhysValMonitoringTool::bookHistograms() 
{
  ATH_MSG_INFO ("Booking hists " << name() << "...");
  
  if (m_selectMuonAuthors.size()==0) {
    unsigned int defaultAuthorList[]={xAOD::Muon::MuidCo,xAOD::Muon::MuTagIMO,xAOD::Muon::MuidSA,xAOD::Muon::MuGirl,xAOD::Muon::CaloTag,xAOD::Muon::CaloLikelihood};
    for (const auto author : defaultAuthorList) m_selectMuonAuthors.push_back(author);
  }

  if (m_selectMuonCategories.size()==0) {
    unsigned int defaultMuonCategories[]={ ALL, PROMPT, INFLIGHT, NONISO, REST };
    for (const auto category: defaultMuonCategories) m_selectMuonCategories.push_back(category);
  }

  std::string theMuonCategories[5];
  theMuonCategories[ALL]="All";
  theMuonCategories[PROMPT]="Prompt";
  theMuonCategories[INFLIGHT]="InFlight";
  theMuonCategories[NONISO]="NonIsolated";
  theMuonCategories[REST]="Rest";

  for (const auto category: m_selectMuonCategories) m_selectMuonCategoriesStr.push_back(theMuonCategories[category]);

  for (const auto category : m_selectMuonCategoriesStr) {
    std::string categoryPath = "Muons/"+category+"/";
    m_muonValidationPlots.push_back( new MuonValidationPlots(0, categoryPath,
              m_selectMuonAuthors, m_isData,
	      (category==theMuonCategories[ALL]? false : m_doBinnedResolutionPlots),
	      m_doMuonTree));
    if (m_doTrigMuonValidation) {
       if (category=="All"){
          m_TriggerMuonValidationPlots.push_back( new TriggerMuonValidationPlots(0, categoryPath,
              m_selectMuonAuthors, m_isData, m_doTrigMuonL1Validation,m_doTrigMuonL2Validation,m_doTrigMuonEFValidation,m_selectHLTMuonItems,m_L1MuonItems));
       }
    }
    if (m_doMuonTrackValidation) {
      m_muonMSTrackValidationPlots.push_back(new MuonTrackValidationPlots(0, categoryPath, "MSTrackParticles", m_isData));
      m_muonMETrackValidationPlots.push_back(new MuonTrackValidationPlots(0, categoryPath, "METrackParticles", m_isData));

      m_muonIDTrackValidationPlots.push_back(new MuonTrackValidationPlots(0, categoryPath, "IDTrackParticles", m_isData));
      m_muonIDSelectedTrackValidationPlots.push_back(new MuonTrackValidationPlots(0, categoryPath, "IDSelectedTrackParticles", m_isData));
    }
    if (m_doMuonSegmentValidation) {
      if (category!=theMuonCategories[ALL]) continue; //cannot identify the truth origin of segments...
      m_muonSegmentValidationPlots.push_back(new MuonSegmentValidationPlots(0, categoryPath, m_isData));
    }
  }

  if (!m_isData) {
    m_oUnmatchedRecoMuonPlots = new Muon::RecoMuonPlotOrganizer(0, "Muons/UnmatchedRecoMuons/");
    m_oUnmatchedTruthMuonPlots = new Muon::TruthMuonPlotOrganizer(0, "Muons/UnmatchedTruthMuons/");
    if (m_doMuonTrackValidation) 
      m_oUnmatchedRecoMuonTrackPlots = new Muon::RecoMuonTrackPlotOrganizer(0, "Muons/UnmatchedRecoMuonTracks/");
    
    if (m_doMuonSegmentValidation) 
      m_oUnmatchedRecoMuonSegmentPlots = new Muon::MuonSegmentPlots(0, "Muons/UnmatchedRecoMuonSegments/");
  }

  for (const auto plots : m_muonValidationPlots)                   bookValidationPlots(*plots).ignore();
  for (const auto plots : m_TriggerMuonValidationPlots)            bookValidationPlots(*plots).ignore();
  if (m_doMuonTrackValidation) {
    for (const auto plots : m_muonIDTrackValidationPlots)          bookValidationPlots(*plots).ignore();
    for (const auto plots : m_muonIDSelectedTrackValidationPlots)  bookValidationPlots(*plots).ignore();
    for (const auto plots : m_muonMSTrackValidationPlots)          bookValidationPlots(*plots).ignore();
    for (const auto plots : m_muonMETrackValidationPlots)          bookValidationPlots(*plots).ignore();
    if (!m_isData) bookValidationPlots(*m_oUnmatchedRecoMuonTrackPlots).ignore();
  }
  if (m_doMuonSegmentValidation) {
    for (const auto plots : m_muonSegmentValidationPlots)          bookValidationPlots(*plots).ignore();
    if (!m_isData) bookValidationPlots(*m_oUnmatchedRecoMuonSegmentPlots).ignore();
  }

  if (!m_isData) {
    bookValidationPlots(*m_oUnmatchedRecoMuonPlots).ignore();
    bookValidationPlots(*m_oUnmatchedTruthMuonPlots).ignore();
  }
  
  //book overview hists
  h_overview_Z_mass = new TH1F("Muons_Overview_Z_mass","",20,76,106);
  ATH_CHECK(regHist(h_overview_Z_mass,"Muons/Overview",all));
  h_overview_Z_mass_ME = new TH1F("Muons_Overview_Z_mass_ME","",20,76,106);
  ATH_CHECK(regHist(h_overview_Z_mass_ME,"Muons/Overview",all));
  h_overview_Z_mass_ID = new TH1F("Muons_Overview_Z_mass_ID","",20,76,106);
  ATH_CHECK(regHist(h_overview_Z_mass_ID,"Muons/Overview",all));
  
  h_overview_nObjects.clear();
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_truth_muons", "Number of truth Muons per event", 20, -0.5, 19.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_muons", "Number of Muons per event", 20, -0.5, 19.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_tracks", "Number of Tracks per event", 50, -0.5, 49.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_truth_segments", "Number of truth Segments per event", 200, -0.5, 199.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_segments", "Number of Segments per event", 200, -0.5, 199.5));
  for (const auto hist : h_overview_nObjects) {
    if (hist) ATH_CHECK(regHist(hist,"Muons/Overview",all));
  }

  h_overview_reco_category = new TH1F("Muons_Overview_reco_category","",4,0,4); //prompt/in-flight/non-isolated/other
  for (int i=1; i<4; i++) { //skip 'All'
    h_overview_reco_category->GetXaxis()->SetBinLabel(i,theMuonCategories[i].c_str());
  } h_overview_reco_category->GetXaxis()->SetBinLabel(4,"Other"); //of some other origin or fakes
  ATH_CHECK(regHist(h_overview_reco_category,"Muons/Overview",all));

  int nAuth = xAOD::Muon::NumberOfMuonAuthors;
  for (int i=1; i<4; i++) {
    h_overview_reco_authors.push_back(new TH1F(("Muons_"+theMuonCategories[i]+"_reco_authors").c_str(),("Muons_"+theMuonCategories[i]+"_reco_authors").c_str(),nAuth+1,-0.5,nAuth+0.5));
  }
  h_overview_reco_authors.push_back(new TH1F("Muons_Overview_Other_reco_authors","Muons_Other_reco_authors",nAuth+1,-0.5,nAuth+0.5));
  
  for (const auto hist : h_overview_reco_authors) {
    if (hist) ATH_CHECK(regHist(hist,"Muons/Overview",all));
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonPhysValMonitoringTool::bookValidationPlots(PlotBase& valPlots)
{
  valPlots.initialize();
  std::vector<HistData> hists = valPlots.retrieveBookedHistograms();

  for (auto &hist: hists){
    TString sHistName = hist.first->GetName();
    ATH_MSG_DEBUG ("Initializing " << hist.first << " " << sHistName << " " << hist.second << "...");

    //check for histograms that are useless and skip regHist:
    if ( sHistName.Contains( "momentumPulls" ) )  {
      if ( sHistName.Contains( Muon::EnumDefs::toString(xAOD::Muon::MuidSA) ) ) continue; // empty for standalone muons
      if (!(sHistName.Contains( "Prompt" ) && sHistName.Contains( "AllMuons" )) ) continue; // don't need binned eloss plots for separate muon types, keep only for Prompt AllMuons
    }
    if ( (sHistName.Contains( "resolution" ) || sHistName.Contains( "pulls" ) ) && !sHistName.Contains( "Prompt" ) ) continue; //don't need resolution plots except for prompt muons
    if (sHistName.Contains( "trigger" )&&sHistName.Contains( "wrt" )&&sHistName.Contains( "Features" )) continue;
    modifyHistogram(hist.first);
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }

  // register trees
  std::vector<TreeData> trees = valPlots.retrieveBookedTrees();
  for (auto &tree: trees){
    std::string sTreeName = tree.first->GetName();
    ATH_MSG_DEBUG ("Initializing " << tree.first << " " << sTreeName << " " << tree.second << "...");
    ATH_CHECK(regTree(tree.first, tree.second,all));
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonPhysValMonitoringTool::fillHistograms()
{
  ATH_MSG_DEBUG ("Filling hists " << name() << "...");
  m_vMatchedTruthMuons.clear();
  m_vMatchedMuons.clear();
  m_vMatchedMuonTracks.clear();
  m_vMatchedMuonSegments.clear();
  m_vZmumuIDTracks.clear();
  m_vZmumuMSTracks.clear();
  m_vZmumuMETracks.clear();
  m_vZmumuMuons.clear();
  m_vEFMuons.clear();
  m_vEFMuonsSelected.clear();
  m_vL2SAMuons.clear();
  m_vL2SAMuonsSelected.clear();
  m_vL2CBMuons.clear();
  m_vL2CBMuonsSelected.clear();
  m_vRecoMuons.clear();
  m_vRecoMuons_EffDen.clear();
  m_vRecoMuons_EffDen_CB.clear();
  m_vRecoMuons_EffDen_MS.clear();

  // const xAOD::EventInfo* eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
  // if (!eventInfo){
  //   ATH_MSG_WARNING("Could not retrieve EventInfo, returning");
  //   return StatusCode::SUCCESS;
  // }
  // isData = !eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION );
  

  const xAOD::TruthParticleContainer* TruthMuons(0);
  
  if (!m_isData) {
    TruthMuons = evtStore()->tryConstRetrieve< xAOD::TruthParticleContainer >(m_muonsTruthName);
    if (!TruthMuons) {
      ATH_MSG_ERROR("Couldn't retrieve TruthMuons container with key: " << m_muonsTruthName);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Retrieved truth muons " << TruthMuons->size());
    h_overview_nObjects[0]->Fill(TruthMuons->size());
  }

  const xAOD::MuonContainer* Muons = evtStore()->retrieve< const xAOD::MuonContainer >( m_muonsName );
  if (!Muons) {
    ATH_MSG_WARNING ("Couldn't retrieve Muons container with key: " << m_muonsName);
    return StatusCode::SUCCESS;
  } 
  ATH_MSG_DEBUG("Retrieved muons " << Muons->size());
  h_overview_nObjects[1]->Fill(Muons->size());

  /////////////////////////////////////////////////////////////////////// @@@
  // @@@ Temp hack to get the MuonSpectrometerTrackParticle (@MS Entry, not extrapolated), needed for eloss plots
  // Remove when the link to the real MuonSpectrometerTrackParticle appears in the xAOD muon
  if ( evtStore()->contains<xAOD::TrackParticleContainer>("MuonSpectrometerTrackParticles") ) {
    MSTracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( "MuonSpectrometerTrackParticles" );
    if (!MSTracks) {
      ATH_MSG_WARNING ("Couldn't retrieve MS Tracks container");
      return StatusCode::SUCCESS;
    } 
    else ATH_MSG_DEBUG("Retrieved muon tracks " << MSTracks->size());
  }
  else ATH_MSG_DEBUG("Couldn't find MS Tracks container");

  /////////////////////////////////////////////////////////////////////// @@@


  //Do resonance selection
  std::vector<std::pair<const xAOD::Muon*,const xAOD::Muon*> >  pairs;
  for (const auto mu1: *Muons) {
    for (const auto mu2: *Muons) {
      if (mu1==mu2) continue;
      if (mu1->charge()*mu2->charge()>=0) continue;
      pairs.push_back(std::make_pair(mu1,mu2));
    }
  }

        
  float dMmin=1e10;
  float mZ=0;
  for (auto x : pairs){
    //select best Z
    TLorentzVector mu1,mu2;
    mu1.SetPtEtaPhiM( x.first->pt(),x.first->eta(),x.first->phi(), 105.77 );
    mu2.SetPtEtaPhiM( x.second->pt(),x.second->eta(),x.second->phi(), 105.77 );
    
    auto M = (mu1+mu2).M();
    //choose the Z candidate closest to the Z pole - if multiple exist
    float dM = fabs(M-91187.);
    if (dM>dMmin) continue;
    dMmin = dM;
    mZ=M;
    
    m_vZmumuMuons.clear();
    m_vZmumuMuons.push_back(x.first);
    m_vZmumuMuons.push_back(x.second);
  }

  if (m_vZmumuMuons.size()==2) {
    
    h_overview_Z_mass->Fill( mZ/1000. );

    const xAOD::TrackParticle* metr1(0);
    const xAOD::TrackParticle* metr2(0);
    metr1 = m_vZmumuMuons[0]->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
    metr2 = m_vZmumuMuons[1]->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
    if (metr1 && metr2) {
      TLorentzVector mu1ME,mu2ME;
      mu1ME.SetPtEtaPhiM( metr1->pt(), metr1->eta(), metr1->phi(), 105.77 );
      mu2ME.SetPtEtaPhiM( metr2->pt(), metr2->eta(), metr2->phi(), 105.77 );
      h_overview_Z_mass_ME->Fill( (mu1ME+mu2ME).M()/1000. );
    }

    
    const xAOD::TrackParticle* tr1(0);
    const xAOD::TrackParticle* tr2(0);
    tr1 = m_vZmumuMuons[0]->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    tr2 = m_vZmumuMuons[1]->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if (tr1 && tr2) {
      TLorentzVector mu1ID,mu2ID;
      mu1ID.SetPtEtaPhiM( tr1->pt(), tr1->eta(), tr1->phi(), 105.77 );
      mu2ID.SetPtEtaPhiM( tr2->pt(), tr2->eta(), tr2->phi(), 105.77 );
      h_overview_Z_mass_ID->Fill( (mu1ID+mu2ID).M()/1000. );
    }

    if (m_isData) {
      // m_vZmumuMSTracks.clear();
      // m_vZmumuMSTracks.push_back(x.first->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle));
      // m_vZmumuMSTracks.push_back(x.second->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle));
      m_vZmumuIDTracks.clear();
      m_vZmumuIDTracks.push_back(tr1);
      m_vZmumuIDTracks.push_back(tr2);
    }
  }

  if (!m_isData) {
    for (const auto truthMu: *TruthMuons) handleTruthMuon(truthMu);
  }

  for (const auto mu: *Muons) {
    handleMuon(mu);
    m_vRecoMuons.push_back(mu);
  }
  
  if (m_doMuonTrackValidation) {

    auto IDTracks = getContainer<xAOD::TrackParticleContainer>( m_tracksName );
    if (!IDTracks) return StatusCode::FAILURE;
    for (const auto tp: *IDTracks) handleMuonTrack(tp,xAOD::Muon::InnerDetectorTrackParticle);

    auto MuonTracks = getContainer<xAOD::TrackParticleContainer>( m_muonTracksName );
    if (!MuonTracks) return StatusCode::FAILURE;
    for(const auto tp : *MuonTracks) handleMuonTrack(tp,xAOD::Muon::MuonSpectrometerTrackParticle);

    auto MuonExtrapolatedTracks = getContainer<xAOD::TrackParticleContainer>( m_muonExtrapolatedTracksName );
    if (!MuonExtrapolatedTracks) return StatusCode::FAILURE;
    for(const auto tp : *MuonExtrapolatedTracks) handleMuonTrack(tp,xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);

    ATH_MSG_DEBUG("Retrieved muon tracks " << MuonTracks->size());
    h_overview_nObjects[2]->Fill(MuonTracks->size());
  }

  if (m_doMuonSegmentValidation) {
    const xAOD::MuonSegmentContainer* TruthMuonSegments(0);
    if (!m_isData) {
      TruthMuonSegments = evtStore()->tryConstRetrieve< xAOD::MuonSegmentContainer >(m_muonSegmentsTruthName);
      if (!TruthMuonSegments) {
	ATH_MSG_WARNING ("Couldn't retrieve Truth Muon Segments container with key: " << m_muonSegmentsTruthName);
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG("Retrieved truth muon segments " << TruthMuonSegments->size());
      h_overview_nObjects[3]->Fill(TruthMuonSegments->size());
      for (const auto truthMuSeg: *TruthMuonSegments) handleTruthMuonSegment(truthMuSeg,TruthMuons);
    }
      
    const xAOD::MuonSegmentContainer* MuonSegments = evtStore()->retrieve< const xAOD::MuonSegmentContainer >( m_muonSegmentsName );
    if (!MuonSegments) {
      ATH_MSG_WARNING ("Couldn't retrieve MuonSegments container with key: " << m_muonSegmentsName);
      return StatusCode::SUCCESS;
    } 
    ATH_MSG_DEBUG("Retrieved muon segments " << MuonSegments->size());
    h_overview_nObjects[4]->Fill(MuonSegments->size());
    for(const auto muSeg : *MuonSegments) handleMuonSegment(muSeg);	
  }

  if (m_doTrigMuonValidation) {
    StatusCode sc =  m_trigDec.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG("Could not retrieve Trigger configuration! " );
      return sc;
    }
    else ATH_MSG_DEBUG("TriggerDecision accessed");
    m_trigDec->ExperimentalAndExpertMethods()->enable();

    ////////////////  
    auto chainGroups = m_trigDec->getChainGroup("HLT_mu.*");
    for(auto &trig : chainGroups->getListOfTriggers()) {
      if(m_trigDec->isPassed(trig, TrigDefs::EF_passedRaw )) ATH_MSG_DEBUG("Chain "<<trig <<" is passed: YES");
      else ATH_MSG_DEBUG("Chain "<<trig <<" is passed: NO");
    }
    auto L1chainGroups = m_trigDec->getChainGroup("L1_MU.*");
    for(auto &L1trig : L1chainGroups->getListOfTriggers()) {
      if(m_trigDec->isPassed(L1trig, TrigDefs::EF_passedRaw )) ATH_MSG_DEBUG("Chain "<<L1trig <<" is passed: YES");
      else ATH_MSG_DEBUG("Chain "<<L1trig <<" is passed: NO");
    }
      ////////////////
    for (auto mu: m_vRecoMuons) {
      if (passesAcceptanceCuts(mu) && fabs(mu->eta())<2.4) {
	if (mu->author()==1) {
	    m_vRecoMuons_EffDen_CB.push_back(mu);
	    ATH_MSG_DEBUG("##### m_vRecoMuons_EffDen_CB  pt:"<< mu->pt()<<"   phi:"<<mu->phi()<<"   eta:"<< mu->eta());
	    std::cout<<"##### m_vRecoMuons_EffDen_CB  pt:"<< mu->pt()<<"   phi:"<<mu->phi()<<"   eta:"<< mu->eta()<<std::endl;
	}
	else if (mu->author()==5) {
	    m_vRecoMuons_EffDen_MS.push_back(mu);
	    ATH_MSG_DEBUG("##### m_vRecoMuons_EffDen_MS  pt:"<< mu->pt()<<"   phi:"<<mu->phi()<<"   eta:"<< mu->eta());
	    std::cout<<"##### m_vRecoMuons_EffDen_MS  pt:"<< mu->pt()<<"   phi:"<<mu->phi()<<"   eta:"<< mu->eta()<<std::endl;
	}
      }
    }

    if(m_doTrigMuonL1Validation){
       const xAOD::MuonRoIContainer* L1TrigMuons = evtStore()->retrieve< const xAOD::MuonRoIContainer >( m_muonL1TrigName );    
       if (!L1TrigMuons) {
         ATH_MSG_WARNING ("Couldn't retrieve L1 Muon Trigger container with key: " << m_muonL1TrigName);
         return StatusCode::SUCCESS;
       }      
       ATH_MSG_DEBUG("Retrieved L1 triggered muons " << L1TrigMuons->size());
       for(const auto TrigL1mu : *L1TrigMuons) handleMuonL1Trigger(TrigL1mu);
    }

    if(m_doTrigMuonL2Validation) {
       const xAOD::L2StandAloneMuonContainer* L2SAMuons = evtStore()->retrieve< const xAOD::L2StandAloneMuonContainer >( m_muonL2SAName );
       if (!L2SAMuons) {
         ATH_MSG_WARNING ("Couldn't retrieve L2 StandAlone Trigger Muon container with key: " << m_muonL2SAName);
         return StatusCode::SUCCESS;
       }      
       ATH_MSG_DEBUG("Retrieved L2 StandAlone triggered muons " << L2SAMuons->size());
       if (L2SAMuons->size()!=0){
          for(const auto L2SAmu : *L2SAMuons) {
             ATH_MSG_DEBUG("Muon L2SA Trigger: pt " << L2SAmu->pt() <<" phi " << L2SAmu->phi() <<" eta " << L2SAmu->eta()<< " roiWord " << L2SAmu->roiWord() << " sAddress " << L2SAmu->sAddress() );
             m_vL2SAMuons.push_back(L2SAmu);
          }
       
          for (unsigned int i=0; i<m_vL2SAMuons.size(); i++) { 
              if ((m_vL2SAMuons.at(i)->pt())!=0.) {
                  m_vL2SAMuonsSelected.push_back(m_vL2SAMuons.at(i));
                  break;
              }
          }
          for (unsigned int i=0; i<m_vL2SAMuons.size(); i++) {    
		unsigned int cont=0;

              for (unsigned int j=0; j< m_vL2SAMuonsSelected.size(); j++) {    
		if(    ((m_vL2SAMuons.at(i)->pt())!=0.)      &&    ((deltaR(m_vL2SAMuonsSelected.at(j), m_vL2SAMuons.at(i)))>0.1)  ) cont++;
		
		if (cont==m_vL2SAMuonsSelected.size()){  
		     m_vL2SAMuonsSelected.push_back(m_vL2SAMuons.at(i));
	             break;
		}                         
              }
	  }
       }
       for (unsigned int j=0; j< m_vL2SAMuonsSelected.size(); j++) {   	 
	 handleMuonL2Trigger(m_vL2SAMuonsSelected.at(j));
       }
       for(auto muonItem : m_muonItems) {
         std::vector<Trig::Feature<xAOD::L2StandAloneMuonContainer> > vec_muons;

         TString muonItem_str=(TString)muonItem;
         if (muonItem_str.Contains("_OR_")) {
           muonItem_str.ReplaceAll("_OR_"," ");
           TString delim=" ";
           std::vector<TString> v_subchains;
           SplitString(muonItem_str, delim, v_subchains);
           for(int i=0; i<(int)v_subchains.size(); i++){
             Trig::FeatureContainer fc1 = m_trigDec->features((std::string)v_subchains.at(i));
             std::vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > vec_muons_1 = fc1.get<xAOD::L2StandAloneMuonContainer>();
             for( auto mufeat : vec_muons_1 ) {
               vec_muons.push_back(mufeat);
             }
           }
         }
         



         else{
           Trig::FeatureContainer fc = m_trigDec->features(muonItem);
           vec_muons = fc.get<xAOD::L2StandAloneMuonContainer>();
         }
        // std::cout<<"Size of vector Trig::Feature<xAOD::L2StandAloneMuonContainer> for chain "<<muonItem<<" = " << vec_muons.size()<<std::endl;
         for( auto mufeat : vec_muons ) {
           ATH_MSG_DEBUG(muonItem<<"  vec_muons.size() = "<<vec_muons.size()<<"  mufeat.cptr()->size() = "<< mufeat.cptr()->size());
           ATH_MSG_DEBUG(muonItem<<"  vec_muons.size() = "<<vec_muons.size()<<"  mufeat.cptr()->size() = "<< mufeat.cptr()->size());
            for(unsigned int i=0; i<mufeat.cptr()->size(); i++) {
              ATH_MSG_DEBUG("##### L2SA feature pt: "<< (*mufeat.cptr())[i]->pt()<< "   eta: "<<(*mufeat.cptr())[i]->eta()<< "   phi: "<<(*mufeat.cptr())[i]->phi());
            }
         }
       }
       const xAOD::L2CombinedMuonContainer* L2CBMuons = evtStore()->retrieve< const xAOD::L2CombinedMuonContainer >( m_muonL2CBName );
       if (!L2CBMuons) {
         ATH_MSG_WARNING ("Couldn't retrieve L2 Combined Trigger Muon container with key: " << m_muonL2CBName);
         return StatusCode::SUCCESS;
       }   
       ATH_MSG_DEBUG("Retrieved L2 Combined triggered muons " << L2CBMuons->size());

       if (L2CBMuons->size()!=0){
	 for(const auto L2CBmu : *L2CBMuons) {
	   ATH_MSG_DEBUG("Muon L2CB Trigger: pt " << L2CBmu->pt() <<" phi " << L2CBmu->phi() <<" eta " << L2CBmu->eta() );
	   m_vL2CBMuons.push_back(L2CBmu);
	 }
       
	 for (unsigned int i=0; i<m_vL2CBMuons.size(); i++) { 
	   if ((m_vL2CBMuons.at(i)->pt())!=0.) {
	     m_vL2CBMuonsSelected.push_back(m_vL2CBMuons.at(i));
	     break;
	   }
	 }


	 for (unsigned int i=0; i<m_vL2CBMuons.size(); i++) {    
	   unsigned int cont=0;

	   for (unsigned int j=0; j< m_vL2CBMuonsSelected.size(); j++) {    
	     if(    ((m_vL2CBMuons.at(i)->pt())!=0.)      &&     ((deltaR(m_vL2CBMuonsSelected.at(j),m_vL2CBMuons.at(i)))>0.1)     ) cont++;
		
	     if (cont==m_vL2CBMuonsSelected.size()){  
	       m_vL2CBMuonsSelected.push_back(m_vL2CBMuons.at(i));
	       break;
	     }                         
	   }
	 }
       }
       for (unsigned int j=0; j< m_vL2CBMuonsSelected.size(); j++) {   	 
	 handleMuonL2Trigger(m_vL2CBMuonsSelected.at(j));
       }
       for(auto muonItem : m_muonItems) {
         std::vector<Trig::Feature<xAOD::L2CombinedMuonContainer> > vec_muons;

         TString muonItem_str=(TString)muonItem;
         if (muonItem_str.Contains("_OR_")) {
           muonItem_str.ReplaceAll("_OR_"," ");
           TString delim=" ";
           std::vector<TString> v_subchains;
           SplitString(muonItem_str, delim, v_subchains);
           for(int i=0; i<(int)v_subchains.size(); i++){
             Trig::FeatureContainer fc1 = m_trigDec->features((std::string)v_subchains.at(i));
             std::vector< Trig::Feature<xAOD::L2CombinedMuonContainer> > vec_muons_1 = fc1.get<xAOD::L2CombinedMuonContainer>();
             for( auto mufeat : vec_muons_1 ) {
               vec_muons.push_back(mufeat);
             }
           }
         }


         else{
           Trig::FeatureContainer fc = m_trigDec->features(muonItem);
           vec_muons = fc.get<xAOD::L2CombinedMuonContainer>();
         }
         ATH_MSG_DEBUG("Size of vector Trig::Feature<xAOD::L2CombinedMuonContainer> for chain "<<muonItem<<" = " << vec_muons.size());
         for( auto mufeat : vec_muons ) {
            for(unsigned int i=0; i<mufeat.cptr()->size(); i++) {
              ATH_MSG_DEBUG("##### L2CB feature pt: "<< (*mufeat.cptr())[i]->pt()<< "   eta: "<<(*mufeat.cptr())[i]->eta()<< "   phi: "<<(*mufeat.cptr())[i]->phi());
            }
         }
       }
    }




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ do_doTrigMuonEFValidation
    if(m_doTrigMuonEFValidation){
       const xAOD::MuonContainer* EFCombTrigMuons = evtStore()->retrieve< const xAOD::MuonContainer >( m_muonEFCombTrigName );
       const xAOD::MuonRoIContainer* L1TrigMuons = evtStore()->retrieve< const xAOD::MuonRoIContainer >( m_muonL1TrigName );    
       if (!EFCombTrigMuons) {
         ATH_MSG_WARNING ("Couldn't retrieve EF Muon Trigger container with key: " << m_muonEFCombTrigName);
         return StatusCode::SUCCESS;
       }      
       ATH_MSG_DEBUG("Retrieved EF triggered muons " << EFCombTrigMuons->size());
       if (EFCombTrigMuons->size()!=0){
          for(const auto Trigmu : *EFCombTrigMuons) {
             ATH_MSG_DEBUG("Muon EF Trigger: pt " << Trigmu->pt() <<" phi " << Trigmu->phi() <<" eta " << Trigmu->eta() << "   author" << Trigmu->author());
             m_vEFMuons.push_back(Trigmu);
          }
       
          m_vEFMuonsSelected.push_back(m_vEFMuons.at(0));


          for (unsigned int i=0; i<m_vEFMuons.size(); i++) {    
		unsigned int cont=0;


              for (unsigned int j=0; j< m_vEFMuonsSelected.size(); j++) {  
		if(  ((deltaR(m_vEFMuonsSelected.at(j),m_vEFMuons.at(i)))>0.1)   || ((m_vEFMuons.at(i)->author()-m_vEFMuonsSelected.at(j)->author())!=0)) {
                       cont++;
                }	
		if (cont==m_vEFMuonsSelected.size()){  
		     m_vEFMuonsSelected.push_back(m_vEFMuons.at(i));
	             break;
		}                       
              }
              
	  } 
          for (unsigned int i=0; i< m_vEFMuonsSelected.size(); i++) {  
             ATH_MSG_DEBUG("==> Geometrical selection of EF Trigger muons: pt " << m_vEFMuonsSelected.at(i)->pt() <<" phi " << m_vEFMuonsSelected.at(i)->phi() <<" eta " << m_vEFMuonsSelected.at(i)->eta() <<  " author " << m_vEFMuonsSelected.at(i)->author());   
          }
          handleMuonTrigger(m_vEFMuonsSelected, m_vRecoMuons);
       }

       for(auto muonItem : m_muonItems) {
         m_vRecoMuons_EffDen=m_vRecoMuons_EffDen_CB; SelectedAuthor=1;
         if ((muonItem.find("msonly") != std::string::npos)) { m_vRecoMuons_EffDen=m_vRecoMuons_EffDen_MS; SelectedAuthor=5; }
         std::vector<Trig::Feature<xAOD::MuonContainer> > vec_muons;

         TString muonItem_str=(TString)muonItem;
         if (muonItem_str.Contains("_OR_")) {
           muonItem_str.ReplaceAll("_OR_"," ");
           TString delim=" ";
           std::vector<TString> v_subchains;
           SplitString(muonItem_str, delim, v_subchains);
           for(int i=0; i<(int)v_subchains.size(); i++){
             Trig::FeatureContainer fc1 = m_trigDec->features((std::string)v_subchains.at(i));
             std::vector< Trig::Feature<xAOD::MuonContainer> > vec_muons_1 = fc1.get<xAOD::MuonContainer>();
             for( auto mufeat : vec_muons_1 ) {
               vec_muons.push_back(mufeat);
             }
           }
         }

         else{
           Trig::FeatureContainer fc = m_trigDec->features(muonItem);
           vec_muons = fc.get<xAOD::MuonContainer>();
         }
         ATH_MSG_DEBUG("Size of vector Trig::Feature<xAOD::MuonContainer> for chain "<<muonItem<<" = " << vec_muons.size());
         for( auto mufeat : vec_muons ) { 
           //ATH_MSG_DEBUG(muonItem<<"  vec_muons.size() = "<<vec_muons.size()<<"  mufeat.cptr()->size() = "<< mufeat.cptr()->size());
           for(unsigned int i=0; i<mufeat.cptr()->size(); i++) {
             ATH_MSG_DEBUG("##### EF MuidCo feature "<<muonItem <<"    pt: "<< (*mufeat.cptr())[i]->pt()<< "   eta: "<<(*mufeat.cptr())[i]->eta()<< "   phi: "<<(*mufeat.cptr())[i]->phi()<< "   author: "<<  (*mufeat.cptr())[i]->author());
std::cout<<"##### EF MuidCo feature "<<muonItem <<"    pt: "<< (*mufeat.cptr())[i]->pt()<< "   eta: "<<(*mufeat.cptr())[i]->eta()<< "   phi: "<<(*mufeat.cptr())[i]->phi()<< "   author: "<<  (*mufeat.cptr())[i]->author()<<std::endl;
             for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
               if (m_selectMuonCategories[j]==ALL) { 
                 if (((*mufeat.cptr())[i]->author())==SelectedAuthor) m_TriggerMuonValidationPlots[j]->fillFeatPlots(*(*mufeat.cptr())[i], muonItem);
               }//if categ=ALL
             }//categories
           }//mufeat.cptr
         }//mufeat
         for (unsigned int k=0; k<m_vRecoMuons_EffDen.size(); k++) {
           bool break_flag=false;
           for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
             if (m_selectMuonCategories[j]==ALL) { 
             m_TriggerMuonValidationPlots[j]->fillDenEff(*m_vRecoMuons_EffDen.at(k), muonItem);}
           }
           for( auto mufeat : vec_muons ) {
             for(unsigned int i=0; i<mufeat.cptr()->size(); i++) {
                if ((((*mufeat.cptr())[i]->author())==SelectedAuthor)&&  (deltaR((*mufeat.cptr())[i],m_vRecoMuons_EffDen.at(k))<0.1)  ) {
                  break_flag=true;
                  ATH_MSG_DEBUG("   $$$ match Reco_EffDen "<<muonItem<<"         pt: "<< m_vRecoMuons_EffDen.at(k)->pt()<< "   eta: "<<m_vRecoMuons_EffDen.at(k)->eta()<< "   phi: "<<m_vRecoMuons_EffDen.at(k)->phi()<< "   author: "<<  m_vRecoMuons_EffDen.at(k)->author());
                  ATH_MSG_DEBUG("   $$$ match EF MuidCo feature "<<muonItem<<"   pt: "<< (*mufeat.cptr())[i]->pt()<< "   eta: "<<(*mufeat.cptr())[i]->eta()<< "   phi: "<<(*mufeat.cptr())[i]->phi()<< "   author: "<<  (*mufeat.cptr())[i]->author() << "    rel_p "<< ( fabs(((*mufeat.cptr())[i]->pt()-m_vRecoMuons_EffDen.at(k)->pt())/(m_vRecoMuons_EffDen.at(k)->pt()))) );
                    for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
                      if (m_selectMuonCategories[j]==ALL) { 
                        m_TriggerMuonValidationPlots[j]->fillNumEff(*m_vRecoMuons_EffDen.at(k), muonItem);
                      }//if categ=ALL
                    }//categories
                    break;
                }//if(Delta_R)
              }//mufeat
              if(break_flag) break;
            }//vec_muons
         }//m_vRecoMuons_EffDen
       }//m_muonItems



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     

       for (const auto L1MuonItem : m_L1MuonItems) { 
         m_vRecoMuons_EffDen=m_vRecoMuons_EffDen_CB; SelectedAuthor=1;
           float treshold=0.;
           if(L1MuonItem=="L1_MU4") treshold=4000;
           if(L1MuonItem=="L1_MU6") treshold=6000;
           if(L1MuonItem=="L1_MU10") treshold=10000;
           if(L1MuonItem=="L1_MU11") treshold=11000;
           if(L1MuonItem=="L1_MU15") treshold=15000;
           if(L1MuonItem=="L1_MU20") treshold=20000;
             for(const auto TrigL1mu : *L1TrigMuons){
               for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
                 if (m_selectMuonCategories[j]==ALL) { 
                   if ((TrigL1mu->thrValue())>=treshold) m_TriggerMuonValidationPlots[j]->fillFeatPlots(*TrigL1mu, L1MuonItem);
                 }//if categ=ALL
               }//categories
             }//L1TrigMuons
             for (unsigned int k=0; k<m_vRecoMuons_EffDen.size(); k++) {
               for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
                 if (m_selectMuonCategories[j]==ALL) { 
                   m_TriggerMuonValidationPlots[j]->fillDenL1Eff(*m_vRecoMuons_EffDen.at(k), L1MuonItem);
                 }
               }
               for(const auto TrigL1mu : *L1TrigMuons){
                 if ( ((TrigL1mu->thrValue())>=treshold)  &&  (sqrt(pow(m_vRecoMuons_EffDen.at(k)->eta()-TrigL1mu->eta(),2.)+pow(m_vRecoMuons_EffDen.at(k)->phi()-TrigL1mu->phi(),2.))<0.2)) {
                    ATH_MSG_DEBUG("   $$$ match Reco_EffDen "<<L1MuonItem<<"         pt: "<< m_vRecoMuons_EffDen.at(k)->pt()<< "   eta: "<<m_vRecoMuons_EffDen.at(k)->eta()<< "   phi: "<<m_vRecoMuons_EffDen.at(k)->phi()<< "   author: "<<  m_vRecoMuons_EffDen.at(k)->author());
                    ATH_MSG_DEBUG("   $$$ L1 feature "<<L1MuonItem<<"   pt: "<< TrigL1mu->thrValue()<< "   eta: "<<TrigL1mu->eta()<< "   phi: "<<TrigL1mu->phi() );
                    for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
                      if (m_selectMuonCategories[j]==ALL) { 
                        m_TriggerMuonValidationPlots[j]->fillNumL1Eff(*m_vRecoMuons_EffDen.at(k), L1MuonItem);
                      }//if categ=ALL
                    }//categories
                    break;          
                  }//if(Delta_R)
              }//L1TrigMuons
           }//m_vRecoMuons_EffDen
       }
    
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//******************filling histos for HLTchains efficiency wrt L1seed

       for(unsigned int m=0; m<m_muonItems.size(); m++) {
         std::vector<Trig::Feature<xAOD::MuonContainer> > vec_muons;

         TString muonItem_str=(TString)m_muonItems[m];
         if (muonItem_str.Contains("_OR_")) {
           muonItem_str.ReplaceAll("_OR_"," ");
           TString delim=" ";
           std::vector<TString> v_subchains;
           SplitString(muonItem_str, delim, v_subchains);
           for(int i=0; i<(int)v_subchains.size(); i++){
             Trig::FeatureContainer fc1 = m_trigDec->features((std::string)v_subchains.at(i));
             std::vector< Trig::Feature<xAOD::MuonContainer> > vec_muons_1 = fc1.get<xAOD::MuonContainer>();
             for( auto mufeat : vec_muons_1 ) {
               vec_muons.push_back(mufeat);
             }
           }
         }



         else{
           Trig::FeatureContainer fc = m_trigDec->features(m_muonItems[m]);
           vec_muons = fc.get<xAOD::MuonContainer>();
         }
         m_vRecoMuons_EffDen=m_vRecoMuons_EffDen_CB; SelectedAuthor=1;
         if ((m_muonItems[m].find("msonly") != std::string::npos)) { m_vRecoMuons_EffDen=m_vRecoMuons_EffDen_MS; SelectedAuthor=5; }         
         float treshold=0.;
         if(m_L1Seed[m]=="L1_MU4") treshold=4000;
         if(m_L1Seed[m]=="L1_MU6") treshold=6000;
         if(m_L1Seed[m]=="L1_MU10") treshold=10000;
         if(m_L1Seed[m]=="L1_MU11") treshold=11000;
         if(m_L1Seed[m]=="L1_MU15") treshold=15000;
         if(m_L1Seed[m]=="L1_MU20") treshold=20000;
         for (unsigned int k=0; k<m_vRecoMuons_EffDen.size(); k++) {
           bool break_flag=false;
           for(const auto TrigL1mu : *L1TrigMuons){
             if ( ((TrigL1mu->thrValue())>=treshold)  &&  (sqrt(pow(m_vRecoMuons_EffDen.at(k)->eta()-TrigL1mu->eta(),2.)+pow(m_vRecoMuons_EffDen.at(k)->phi()-TrigL1mu->phi(),2.))<0.2)) {
               for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
                 if (m_selectMuonCategories[j]==ALL) { 
                   m_TriggerMuonValidationPlots[j]->fillDenRELEff(*m_vRecoMuons_EffDen.at(k), m_muonItems[m]);
                 }//if categ=ALL
               }//categories
               for( auto mufeat : vec_muons ) {
                 for(unsigned int i=0; i<mufeat.cptr()->size(); i++) {
                   if ((((*mufeat.cptr())[i]->author())==SelectedAuthor)&&  (deltaR((*mufeat.cptr())[i],m_vRecoMuons_EffDen.at(k))<0.1)  ) {
                     break_flag=true;
                     for (unsigned int j=0; j<m_selectMuonCategories.size(); j++) {
                       if (m_selectMuonCategories[j]==ALL) { 
                         m_TriggerMuonValidationPlots[j]->fillNumRELEff(*m_vRecoMuons_EffDen.at(k), m_muonItems[m]);
                       }//if categ=ALL
                     }//categories
                     break;
                   }//if(Delta_R)
                 }//mufeat
               if(break_flag) break;
               }//vec_muons
               break;          
             }//if(Delta_R)
           }//L1TrigMuons
         }//m_vRecoMuons_EffDen

       }//m_muonItems.size()

    }//m_doTrigMuonEFValidation
  }
  return StatusCode::SUCCESS;
}

void MuonPhysValMonitoringTool::handleTruthMuonSegment(const xAOD::MuonSegment* truthMuSeg,const xAOD::TruthParticleContainer* /*muonTruthContainer*/)
{
  const xAOD::MuonSegment* muSeg = findRecoMuonSegment(truthMuSeg);
  ////if (msgLvl(MSG::DEBUG)) printTruthMuonDebug(truthMu, mu);

  unsigned int thisMuonCategory = ALL; //getMuonSegmentTruthCategory(truthMuSeg, muonTruthContainer) @@@ Does not work...
;

  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {    
    if ( m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory ) {
      m_muonSegmentValidationPlots[i]->fill(truthMuSeg, muSeg);   // if no reco muon segment is found a protection inside MuonSegmentValidationPlots will ensure, its plots won't be filled
    }
  }

}

void MuonPhysValMonitoringTool::handleMuonSegment(const xAOD::MuonSegment* muSeg)
{
  if( !muSeg ) {
    ATH_MSG_WARNING("No muon segment found");
    return;
  }
  if (std::find(std::begin(m_vMatchedMuonSegments), std::end(m_vMatchedMuonSegments), muSeg) != std::end(m_vMatchedMuonSegments)) return;
  if (!m_isData) m_oUnmatchedRecoMuonSegmentPlots->fill(*muSeg);
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) { 
      m_muonSegmentValidationPlots[i]->fill(muSeg);
      break;
    }
  }
}


void MuonPhysValMonitoringTool::handleMuon(const xAOD::Muon* mu)
{
  if (!mu) return;
  if (msgLvl(MSG::DEBUG)) printMuonDebug(mu);

  if (m_isData) {
    MUCATEGORY thisMuonCategory = ALL;
    // for events with a Zmumu candidate, separate Z muons from the rest:
    if (m_vZmumuMuons.size()>0) {
      thisMuonCategory = REST;
      for (const auto zmu: m_vZmumuMuons) {
	if ( zmu->p4().DeltaR(mu->p4())<0.05 ) {
	  thisMuonCategory=PROMPT;
	  break;
	}
      }      
    }
    //fill plots
    for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
      if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory) {
	m_muonValidationPlots[i]->fill(*mu);
      }
    }    
    return;
  }
  
  //make deep copy of muon and decorate with quality
  xAOD::Muon* mu_c = 0;
  try{
    mu_c = getCorrectedMuon(*mu);
  }
  catch(SG::ExcBadAuxVar&){  
    ATH_MSG_ERROR( "Cannot retrieve aux-item - rejecting muon" );
    if (mu_c) delete mu_c;
    return;
  }
  
  if (std::find(std::begin(m_vMatchedMuons), std::end(m_vMatchedMuons), mu) != std::end(m_vMatchedMuons)) {
    if (mu_c) delete mu_c;
    return;
  }
  
  //unmatched reco muons (not matched with any kind of truth particle, fakes)
  if (!m_isData)
    m_oUnmatchedRecoMuonPlots->fill(*mu_c);
  
  h_overview_reco_category->Fill("Other",1);
  h_overview_reco_authors[3]->Fill(mu->author());
 
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) {
      m_muonValidationPlots[i]->fill(*mu_c);
      break;
    }
  }
  if (mu_c) delete mu_c;
}


void MuonPhysValMonitoringTool::handleTruthMuon(const xAOD::TruthParticle* truthMu)
{
  const xAOD::Muon* mu = findRecoMuon(truthMu);

  if (msgLvl(MSG::DEBUG)) printTruthMuonDebug(truthMu, mu);
  if ( !passesAcceptanceCuts(truthMu) ) return;
  
  xAOD::Muon* mu_c = 0;
  if (mu) {
    try{
      mu_c = getCorrectedMuon(*mu);
    }
    catch(SG::ExcBadAuxVar&){    
      ATH_MSG_ERROR( "Cannot retrieve aux-item - rejecting muon" );
      delete mu_c;
      mu_c=0;
    }
  }
  
  unsigned int thisMuonCategory = getMuonTruthCategory(truthMu);
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory) {
      m_muonValidationPlots[i]->fill(truthMu, mu_c, MSTracks); //if no muon is found a protection inside MuonValidationPlots will ensure, its plots won't be filled
    }
  }
  if (mu_c) {    
    h_overview_reco_category->Fill(thisMuonCategory-1);
    h_overview_reco_authors[thisMuonCategory-1]->Fill(mu_c->author());
  }
  else if (!m_isData) m_oUnmatchedTruthMuonPlots->fill(*truthMu);

  if (mu_c) delete mu_c;
}


void MuonPhysValMonitoringTool::handleMuonTrack(const xAOD::TrackParticle* tp, xAOD::Muon::TrackParticleType type)
{
  if( !tp ) {
    ATH_MSG_WARNING("No track particle found");    return;
  }  

  //if ID track, check that it passes standard combined mu reco selections
  bool passesMuonTrackSel = false;
  if (type==xAOD::Muon::InnerDetectorTrackParticle) {    
    passesMuonTrackSel = m_trackSelector->decision(*tp);
  }       
  
  if (m_isData) {
    if (type==xAOD::Muon::InnerDetectorTrackParticle) {
      MUCATEGORY thisTrkCategory = ALL;
      // for events with a Zmumu candidate, separate Z muon tracks from the rest:
      if (m_vZmumuIDTracks.size()>0) {
	thisTrkCategory = REST;
	for (const auto zmu: m_vZmumuIDTracks) {
	  if ( zmu->p4().DeltaR(tp->p4())<0.01 ) {
	    thisTrkCategory=PROMPT;
	    break;
	  }
	}
      }
      
      for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
	if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisTrkCategory) {
	  m_muonIDTrackValidationPlots[i]->fill(*tp);
	  if (passesMuonTrackSel) m_muonIDSelectedTrackValidationPlots[i]->fill(*tp);
	}
      }
    }
    else if (type==xAOD::Muon::MuonSpectrometerTrackParticle) m_muonMSTrackValidationPlots[ALL]->fill(*tp);
    else if (type==xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle) m_muonMETrackValidationPlots[ALL]->fill(*tp);
    return;	
  }

  TruthLink truthLink;
  if( tp->isAvailable<TruthLink>("truthParticleLink") ) {
    truthLink = tp->auxdata<TruthLink>("truthParticleLink");
  }

  //int truthType = tp->isAvailable<int>("truthType")? tp->auxdata< int >("truthType") :0;
  //float truthMatchProb = getMatchingProbability(*tp);

  
  if (!truthLink.isValid()) {
    ATH_MSG_DEBUG("No truth link available");
    if (type==xAOD::Muon::InnerDetectorTrackParticle) return;	
    if (!passesAcceptanceCuts(tp)) return;

    for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
      if (m_selectMuonCategories[i]==ALL) {
	//ID track plots for any track
	// if (type==xAOD::Muon::InnerDetectorTrackParticle) {
	//   m_muonIDTrackValidationPlots[i]->fill(*tp);
	//   if (passesMuonTrackSel) m_muonIDSelectedTrackValidationPlots[i]->fill(*tp);
	// } else
	if (type==xAOD::Muon::MuonSpectrometerTrackParticle) {
	  if (!m_isData) m_oUnmatchedRecoMuonTrackPlots->fill(*tp);
	  m_muonMSTrackValidationPlots[i]->fill(*tp);
	}	
	else if (type==xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle) {
	  m_muonMETrackValidationPlots[i]->fill(*tp);
	}
	break;
      }
    }
  }//end if no valid truth link
  else {     //here: valid truth link and truthType

    if (type==xAOD::Muon::InnerDetectorTrackParticle) { //don't fill histograms for any ID track, only for muons; buys a lot of time
      if ((*truthLink)->absPdgId()!=13 || (*truthLink)->status()!=1) return; //not a muon
      if ((*truthLink)->barcode()==0 || (*truthLink)->barcode()>=200e3) return; //must have valid barcode
    }
    
    if ( !passesAcceptanceCuts(*truthLink) ) return;
    unsigned int thisMuonCategory = getMuonTruthCategory(tp);
      
    for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
      if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory) {

	if (type==xAOD::Muon::InnerDetectorTrackParticle) {
	  m_muonIDTrackValidationPlots[i]->fill(*truthLink, tp);
	  if (passesMuonTrackSel) m_muonIDSelectedTrackValidationPlots[i]->fill(*truthLink,tp);
	}
	else if (type==xAOD::Muon::MuonSpectrometerTrackParticle) {
	  m_muonMSTrackValidationPlots[i]->fill(*truthLink, tp);
	}
	else if (type==xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle) {
	  m_muonMETrackValidationPlots[i]->fill(*truthLink, tp);
	}
	
      }
    }
    
  }
  
  return;
}

void MuonPhysValMonitoringTool::handleMuonL1Trigger(const xAOD::MuonRoI* TrigL1mu)
{
  if (msgLvl(MSG::DEBUG)) printMuonL1TriggerDebug(TrigL1mu);
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) { 
      m_TriggerMuonValidationPlots[i]->fill(*TrigL1mu);
      break;
    }
  } 
} 

  
void MuonPhysValMonitoringTool::handleMuonL2Trigger(const xAOD::L2StandAloneMuon* L2SAmu)
{
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) { 
      m_TriggerMuonValidationPlots[i]->fill(*L2SAmu);
      break;
    }
  } 
  ATH_MSG_DEBUG("   ==> Geometrical selection of Muon L2SA Trigger :  pt " << L2SAmu->pt()<< " phi " << L2SAmu->phi() << " eta " << L2SAmu->eta()  << " roiWord " << L2SAmu->roiWord() << " sAddress " << L2SAmu->sAddress() );
}



void MuonPhysValMonitoringTool::handleMuonL2Trigger(const xAOD::L2CombinedMuon* L2CBmu)
{
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) { 
      m_TriggerMuonValidationPlots[i]->fill(*L2CBmu);
      break;
    }
  } 
  ATH_MSG_DEBUG("   ==> Geometrical selection of Muon L2CB Trigger :  pt " << L2CBmu->pt() << " phi " << L2CBmu->phi() << " eta " << L2CBmu->eta()  );
}



void MuonPhysValMonitoringTool::handleMuonTrigger(  std::vector<const xAOD::Muon*> m_vEFMuonsSelected, std::vector<const xAOD::Muon*> m_vRecoMuons)
{         
  for (unsigned int k=0; k<m_vEFMuonsSelected.size(); k++){
    for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
      if (m_selectMuonCategories[i]==ALL) { 
        m_TriggerMuonValidationPlots[i]->fill(*m_vEFMuonsSelected.at(k));
        break;
      }
    }
  } 

  for (unsigned int i=0; i<m_vRecoMuons.size(); i++){   
     for (unsigned int k=0; k<m_vEFMuonsSelected.size(); k++){  
       for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
         if (m_selectMuonCategories[i]==ALL) { 
           if((deltaR(m_vEFMuonsSelected.at(k),m_vRecoMuons.at(i))<0.1)&&(m_vRecoMuons.at(i)->author()==1)&&(fabs(m_vRecoMuons.at(i)->eta())<2.4 )){
             std::cout<<"*** Reco to be matched :  pt " << m_vRecoMuons.at(i)->pt() << " phi " <<m_vRecoMuons.at(i)->phi() << " eta " << m_vRecoMuons.at(i)->eta() <<"   author"<<m_vRecoMuons.at(i)->author()<<std::endl;
             std::cout<<"******************* EF :  pt " << m_vEFMuonsSelected.at(k)->pt() << " phi " <<m_vEFMuonsSelected.at(k)->phi() << " eta " << m_vEFMuonsSelected.at(k)->eta()  <<"  author"<<m_vEFMuonsSelected.at(k)->author()<<std::endl;
             m_TriggerMuonValidationPlots[i]->fill(*m_vEFMuonsSelected.at(k),*m_vRecoMuons.at(i));    /////work in progress
           }
         }
       } 
     }
   }

}

void MuonPhysValMonitoringTool::printMuonDebug(const xAOD::Muon* mu)
{
  const xAOD::TrackParticle* tp  = mu->primaryTrackParticle();
  TruthLink truthLink;
  if( tp  ){
    if( !tp->isAvailable<TruthLink>("truthParticleLink") ) ATH_MSG_DEBUG("No truth link found");
    else truthLink = tp->auxdata<TruthLink>("truthParticleLink");
  }
  ATH_MSG_DEBUG("Muon: pt " << mu->pt() << " eta " << mu->eta() << " link " <<truthLink.isValid() );    
}

const xAOD::Muon* MuonPhysValMonitoringTool::findRecoMuon(const xAOD::TruthParticle* truthMu)
{
  if(!truthMu->isAvailable< MuonLink >("recoMuonLink") ) return nullptr;
  MuonLink link = truthMu->auxdata< MuonLink >("recoMuonLink");
  if(!link.isValid() ) return nullptr;
  m_vMatchedMuons.push_back(*link);
  return (*link);
}


void MuonPhysValMonitoringTool::printTruthMuonDebug(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu)
{
  ATH_MSG_DEBUG("Truth muon: " << truthMu->pt() << " eta " << truthMu->eta() );
  if (!mu) return;
  ATH_MSG_DEBUG("Reco muon: " << mu->pt() << " eta " << mu->eta() );
  
}


void MuonPhysValMonitoringTool::printMuonL1TriggerDebug(const xAOD::MuonRoI* TrigL1mu)
{
  ATH_MSG_DEBUG("MuonRoI L1 Trigger: ptThr " << TrigL1mu->thrValue() <<" phi " << TrigL1mu->phi() <<" eta " << TrigL1mu->eta());    
}



StatusCode MuonPhysValMonitoringTool::procHistograms() 
{
  ATH_MSG_INFO ("Finalising hists " << name() << "...");
  for (const auto plots: m_muonValidationPlots) plots->finalize();
  for (const auto plots: m_TriggerMuonValidationPlots) plots->finalize();
  if (!m_isData) {
    m_oUnmatchedRecoMuonPlots->finalize();
    m_oUnmatchedTruthMuonPlots->finalize();
  }

  if (m_doMuonTrackValidation) {
    for (const auto plots: m_muonMSTrackValidationPlots) plots->finalize();
    for (const auto plots: m_muonMETrackValidationPlots) plots->finalize();
    for (const auto plots: m_muonIDTrackValidationPlots) plots->finalize();
    for (const auto plots: m_muonIDSelectedTrackValidationPlots) plots->finalize();
    if (!m_isData) m_oUnmatchedRecoMuonTrackPlots->finalize();
  }

  if (m_doMuonSegmentValidation) {
    for (const auto plots: m_muonSegmentValidationPlots) plots->finalize();
    if (!m_isData) m_oUnmatchedRecoMuonSegmentPlots->finalize();
  }

  return StatusCode::SUCCESS;
}

const xAOD::MuonSegment* MuonPhysValMonitoringTool::findRecoMuonSegment(const xAOD::MuonSegment* truthMuSeg)
{
  if(!truthMuSeg->isAvailable< MuonSegmentLink >("recoSegmentLink") ) {
    ATH_MSG_DEBUG("recoSegmentLink not found");
    return nullptr;
  }
  MuonSegmentLink link = truthMuSeg->auxdata< MuonSegmentLink >("recoSegmentLink");
  if(!link.isValid() ) {
    ATH_MSG_DEBUG("recoSegmentLink not valid");
    return nullptr;
  }
  m_vMatchedMuonSegments.push_back(*link);
  return (*link);
}

TH1F* MuonPhysValMonitoringTool::findHistogram(std::vector<HistData> hists,std::string hnameTag,std::string hdirTag,std::string hNewName) {
  TH1F* h=0;
  for (auto hist: hists){
    if( hist.second.find(hdirTag) != std::string::npos || hdirTag.empty() ) {
      std::string histname = hist.first->GetName();
      if( histname.find(hnameTag) != std::string::npos ) {
	h = (TH1F*) hist.first->Clone(hNewName.c_str());
	return h;
      }
    }
  }
  return h;
}

MuonPhysValMonitoringTool::MUCATEGORY MuonPhysValMonitoringTool::getMuonSegmentTruthCategory(const xAOD::MuonSegment* truthMuSeg,const xAOD::TruthParticleContainer* muonTruthContainer)
{
  TruthLink truthLink;
  if( truthMuSeg->isAvailable<TruthLink>("truthParticleLink") ) {
    truthLink = truthMuSeg->auxdata<TruthLink>("truthParticleLink");
    if (truthLink.isValid()) {
      int theBarcode = (*truthLink)->barcode();
      if (abs( (*truthLink)->pdgId() ) != 13) return REST;
      	    
      for (const auto muTruthPart: *muonTruthContainer) {
      	if (muTruthPart->barcode() == theBarcode) {
      	 return getMuonTruthCategory(muTruthPart);
      	}
      }
    }
  }
  else ATH_MSG_WARNING("No truth link available for muon truth segment");

  return REST;
}

MuonPhysValMonitoringTool::MUCATEGORY MuonPhysValMonitoringTool::getMuonTruthCategory(const xAOD::IParticle* mu)
{
  int truthType = mu->auxdata< int >("truthType");
  if (truthType == 6) return PROMPT;
  else if (truthType == 8 && (mu->auxdata< int >("truthOrigin") == 34 || mu->auxdata< int >("truthOrigin") == 35)) return INFLIGHT;
  else if (truthType == 7 ) return NONISO;
  return REST;
}

xAOD::Muon* MuonPhysValMonitoringTool::getCorrectedMuon(const xAOD::Muon &mu)
{
  xAOD::Muon *mu_c = 0;
  if(mu.m()<=0) return mu_c;
  mu_c = new xAOD::Muon();
  mu_c->makePrivateStore(mu);

  m_muonSelectionTool->setQuality(*mu_c);
  m_muonSelectionTool->setPassesHighPtCuts(*mu_c);
  m_muonSelectionTool->setPassesIDCuts(*mu_c);
  return mu_c;
}  
  
void MuonPhysValMonitoringTool::modifyHistogram(TH1* hist)
{
  std::string histname = hist->GetName();

  if (histname.find("trigger_L1_pt") != std::string::npos){ //if (histname=="Muons_All_trigger_L1_pt"){ 
    hist->SetTitle("L1Trigger Muons pt treshold");
    hist->GetXaxis()->SetTitle("L1Trigger Muons pt treshold [GeV]");
    hist->GetXaxis()->Set(30,-0.5,29.5);
  }
  if (histname.find("trigger_L1_eta_pt") != std::string::npos){ // if (histname=="Muons_All_trigger_L1_eta_pt") {
    hist->SetTitle("L1Trigger Muons pt treshold vs eta");
    hist->GetYaxis()->Set(90,-0.5,29.5);
    hist->GetYaxis()->SetTitle("L1Trigger Muons pt treshold [GeV]");
  }

  if (histname.find("trigger") != std::string::npos){
    if ((histname.find("Denom_pt") != std::string::npos)||(histname.find("Numer_pt") != std::string::npos)||(histname.find("Features_pt") != std::string::npos)){
      hist->GetXaxis()->Set(200,0.,200.);
    }	
  }

  
  if (histname.find("hits") != std::string::npos)  {
    if (histname.find("etaLayer2") != std::string::npos) hist->GetXaxis()->Set(15,-0.5,14.5);
    else if (histname.find("etaLayer") != std::string::npos) hist->GetXaxis()->Set(11,-0.5,10.5);
  }
  
  if (histname.find("METrackParticles") != std::string::npos)  {
    if (histname.find("Res_eta") != std::string::npos){
      hist->GetXaxis()->Set(50,-0.025,0.025);	
    }
    else if (histname.find("Res_phi") != std::string::npos){
      hist->GetXaxis()->Set(50,-0.02,0.02);
    }
  }
  else if (histname.find("MSTrackParticles") != std::string::npos)  {
    if (histname.find("Res_eta") != std::string::npos){
      hist->GetXaxis()->Set(50,-0.025,0.025);	
    }
    else if (histname.find("Res_phi") != std::string::npos){
      hist->GetXaxis()->Set(50,-0.05,0.05);
    }
  }
  else if (histname.find("trigger") != std::string::npos)  {
    if (histname.find("Res_eta") != std::string::npos){
      hist->GetXaxis()->Set(100,-0.005,0.005);	
    }
    else if (histname.find("Res_phi") != std::string::npos){
      hist->GetXaxis()->Set(100,-0.002,0.002);
    }
    if (histname.find("MuidSA") != std::string::npos){
      if (histname.find("Res_eta") != std::string::npos){
        hist->GetXaxis()->Set(100,-0.02,0.02);	
      }
      else if (histname.find("Res_phi") != std::string::npos){
        hist->GetXaxis()->Set(100,-0.02,0.02);
      }
    }
  }
  else {
    if (histname.find("Res_eta") != std::string::npos){
      hist->GetXaxis()->Set(50,-0.005,0.005);	
    }
    else if (histname.find("Res_phi") != std::string::npos){
      hist->GetXaxis()->Set(50,-0.002,0.002);	
    }
  }

  
}

// std::vector<const xAOD::TrackParticle*> MuonPhysValMonitoringTool::getMatchedTracks(const xAOD::TrackParticleContainer* Tracks)
// {
//   std::vector<const xAOD::TrackParticle*> matchedTracks;
//   std::vector<const xAOD::TruthParticle*> truthParticles;

//   for (const auto trk: *Tracks){
//     // find if this is a duplicate / fake:
    
//     float prob = getMatchingProbability(*trk);
//     if (prob<0.8) continue; //bad matching prob, probably a fake;
    
//     const xAOD::TruthParticle* truthMu = findTruthMuon(trk);
//     if (!truthMu) continue;    
//     //loop over accumulated matched tracks
//     bool isDuplicate=false;
//     for (unsigned int i=0; i<matchedTracks.size(); i++) {
//       if (truthMu==truthParticles[i]) { //two tracks pointing to the same truth particle... resolve by truthMatchProbability
// 	isDuplicate = true;
// 	if (prob > getMatchingProbability(*matchedTracks[i])) {
// 	  matchedTracks[i] = trk;
// 	}
//       }
//     }
//     if (!isDuplicate) {
//       truthParticles.push_back(truthMu);
//       matchedTracks.push_back(trk);
//     }
//   }
//   truthParticles.clear();
//   return matchedTracks;
// }
// const xAOD::TruthParticle* MuonPhysValMonitoringTool::findTruthMuon(const xAOD::TrackParticle* trk)
// {
//   if( !trk ) return nullptr;
//   if( !trk->isAvailable<TruthLink>("truthParticleLink") ) return nullptr;  
//   TruthLink truthLink = trk->auxdata<TruthLink>("truthParticleLink");
//   if (!truthLink.isValid()) return nullptr;
  
//   int truthType = trk->auxdata< int >("truthType");
//   if (truthType<5 || truthType>8) return nullptr; //not muon

//   return (*truthLink);
// }

bool MuonPhysValMonitoringTool::passesAcceptanceCuts(const xAOD::IParticle* prt)
{
  if( prt->pt() < 2000. ) return false;
  if( fabs(prt->eta()) > 2.7 ) return false;

  return true;
}

float MuonPhysValMonitoringTool::deltaR(const xAOD::IParticle* prt1, const xAOD::IParticle* prt2)
{
  float Delta_R = (sqrt( pow((prt1->eta()-prt2->eta()),2) + pow((prt1->phi()- prt2->phi()),2) ));   
  return Delta_R;
}

void MuonPhysValMonitoringTool::SplitString(TString x, TString delim, std::vector<TString> &v){
 v.clear();
 int stringLength=x.Length();
 int delimLength=delim.Length();


 int stop=1;
 TString temp="---";
 while(stop!=-1){
  stop = x.First(delim);

  if(stop!=-1){
   temp = x(0, stop);
   TSubString newString = x( stop+delimLength, stringLength );
   x=newString;
   stringLength=x.Length();
  }
  else{
   stringLength=x.Length();
   temp = x(0, stringLength);
  }

  v.push_back(temp);
 }

}


} //close namespace
