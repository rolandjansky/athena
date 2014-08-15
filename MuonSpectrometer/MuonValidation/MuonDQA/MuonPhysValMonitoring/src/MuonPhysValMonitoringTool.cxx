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
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaBaseComps/AthCheckMacros.h"


namespace MuonPhysValMonitoring {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

MuonPhysValMonitoringTool::MuonPhysValMonitoringTool( const std::string& type, 
						      const std::string& name, 
						      const IInterface* parent ):
  ManagedMonitorToolBase( type, name, parent )
{  
  declareProperty( "MuonContainerName", m_muonsName = "Muons" );
  declareProperty( "MuonTruthParticleContainerName", m_muonsTruthName = "MuonTruthParticle" );
  declareProperty( "MuonTrackContainerName", m_muonTracksName = "MuonSpectrometerTrackParticles" );
  declareProperty( "MuonSegmentContainerName", m_muonSegmentsName = "MuonSegments" );
  declareProperty( "MuonTruthSegmentContainerName", m_muonSegmentsTruthName = "MuonTruthSegments" );
  declareProperty( "SelectMuonAuthors", m_selectMuonAuthors);
  declareProperty( "SelectMuonCategories", m_selectMuonCategories );
  declareProperty( "DoMuonTrackValidation", m_doMuonTrackValidation = true);
  declareProperty( "DoMuonSegmentValidation", m_doMuonSegmentValidation = true);
  declareProperty( "DoBinnedResolutionPlots", m_doBinnedResolutionPlots = true);
}

// Destructor
///////////////
MuonPhysValMonitoringTool::~MuonPhysValMonitoringTool()
{ 
}

// Athena algtool's Hooks
////////////////////////////
StatusCode MuonPhysValMonitoringTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  
  return StatusCode::SUCCESS;
}

StatusCode MuonPhysValMonitoringTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");    
  ATH_CHECK(ManagedMonitorToolBase::finalize());

  for (auto plots:m_muonValidationPlots) {
    delete plots;
    plots=0;
  }
  if (m_doMuonTrackValidation) {
    for (auto plots:m_muonTrackValidationPlots) {
      delete plots;
      plots = 0;
    }
    delete m_oUnmatchedRecoMuonTrackPlots;
    m_oUnmatchedRecoMuonTrackPlots=0;
  }
  if (m_doMuonSegmentValidation) {
    for (auto plots:m_muonSegmentValidationPlots) {
      delete plots;
      plots = 0;
    }
    delete m_oUnmatchedRecoMuonSegmentPlots;
    m_oUnmatchedRecoMuonSegmentPlots=0;
  }

  delete m_oUnmatchedRecoMuonPlots;
  delete m_oUnmatchedTruthMuonPlots;

  m_oUnmatchedRecoMuonPlots=0;
  m_oUnmatchedTruthMuonPlots=0;
 
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
    unsigned int defaultMuonCategories[]={ ALL, PROMPT, INFLIGHT, REST };
    for (const auto category: defaultMuonCategories) m_selectMuonCategories.push_back(category);
  }

  std::string theMuonCategories[4];
  theMuonCategories[ALL]="All";
  theMuonCategories[PROMPT]="Prompt";
  theMuonCategories[INFLIGHT]="InFlight";
  theMuonCategories[REST]="Rest";

  for (const auto category: m_selectMuonCategories) m_selectMuonCategoriesStr.push_back(theMuonCategories[category]);

  for (const auto category : m_selectMuonCategoriesStr) {
    std::string categoryPath = "Muons/"+category+"/";
    m_muonValidationPlots.push_back(new MuonValidationPlots(0, categoryPath, m_selectMuonAuthors, m_doBinnedResolutionPlots));
    if (m_doMuonTrackValidation) 
      m_muonTrackValidationPlots.push_back(new MuonTrackValidationPlots(0, categoryPath));
    if (m_doMuonSegmentValidation) 
      m_muonSegmentValidationPlots.push_back(new MuonSegmentValidationPlots(0, categoryPath));
  }
  
  m_oUnmatchedRecoMuonPlots = new RecoMuonPlots(0, "Muons/UnmatchedRecoMuons/");
  m_oUnmatchedTruthMuonPlots = new TruthMuonPlots(0, "Muons/UnmatchedTruthMuons/");

  if (m_doMuonTrackValidation) 
    m_oUnmatchedRecoMuonTrackPlots = new RecoMuonTrackPlots(0, "Muons/UnmatchedRecoMuonTracks/");
  
  if (m_doMuonSegmentValidation) 
    m_oUnmatchedRecoMuonSegmentPlots = new Muon::MuonSegmentPlots(0, "Muons/UnmatchedRecoMuonSegments/");


  for (const auto plots : m_muonValidationPlots) {
    bookValidationPlots(*plots).ignore();
  }
  if (m_doMuonTrackValidation) {
    for (const auto plots : m_muonTrackValidationPlots) {
      bookValidationPlots(*plots).ignore();
    }
    bookValidationPlots(*m_oUnmatchedRecoMuonTrackPlots).ignore();
  }
  if (m_doMuonSegmentValidation) {
    for (const auto plots : m_muonSegmentValidationPlots) {
      bookValidationPlots(*plots).ignore();
    }
    bookValidationPlots(*m_oUnmatchedRecoMuonSegmentPlots).ignore();
  }
  bookValidationPlots(*m_oUnmatchedRecoMuonPlots).ignore();
  bookValidationPlots(*m_oUnmatchedTruthMuonPlots).ignore();

  //book overview hists
  h_overview_reco_category = new TH1F("Muons_reco_category","",4,0,4); //prompt/in-flight/rest/unmatched
  for (int i=1; i<4; i++) { //skip 'All'
    h_overview_reco_category->GetXaxis()->SetBinLabel(i,theMuonCategories[i].c_str());
  } h_overview_reco_category->GetXaxis()->SetBinLabel(4,"Unmatched");
  ATH_CHECK(regHist(h_overview_reco_category,"Muons/Overview",all));

  int nAuth = xAOD::Muon::NumberOfMuonAuthors;
  for (int i=1; i<4; i++) {
    h_overview_reco_authors.push_back(new TH1F(("Muons_"+theMuonCategories[i]+"_reco_authors").c_str(),("Muons_"+theMuonCategories[i]+"_reco_authors").c_str(),nAuth+1,-0.5,nAuth+0.5));
  } h_overview_reco_authors.push_back(new TH1F("Muons_Unmatched_reco_authors","Muons_Unmatched_reco_authors",nAuth+1,-0.5,nAuth+0.5));
  for (const auto hist : h_overview_reco_authors) {
    if (hist) ATH_CHECK(regHist(hist,"Muons/Overview",all));
  }

  return StatusCode::SUCCESS;      
}

StatusCode MuonPhysValMonitoringTool::bookValidationPlots(PlotBase& valPlots)
{
  valPlots.initialize();
  std::vector<HistData> hists = valPlots.retrieveBookedHistograms();
  for (auto hist: hists){
    ATH_MSG_DEBUG ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonPhysValMonitoringTool::fillHistograms()
{
  ATH_MSG_DEBUG ("Filling hists " << name() << "...");
  m_vMatchedMuons.clear();
  m_vMatchedMuonTracks.clear();
  m_vMatchedMuonSegments.clear();
  // const xAOD::EventInfo* eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
  // if (!eventInfo){
  //   ATH_MSG_WARNING("Could not retrieve EventInfo. Returning 1");
  //   return StatusCode::SUCCESS;
  // } 
  //  
  // if (!eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION )) return StatusCode::SUCCESS; // enable once it is available

  const xAOD::TruthParticleContainer* TruthMuons = evtStore()->tryConstRetrieve< xAOD::TruthParticleContainer >(m_muonsTruthName);
  if (!TruthMuons) {
    ATH_MSG_WARNING ("Couldn't retrieve TruthMuons container with key: " << m_muonsTruthName);
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Retrieved truth muons " << TruthMuons->size());

  for (const auto truthMu: *TruthMuons) handleTruthMuon(truthMu);

  const xAOD::MuonContainer* Muons = evtStore()->retrieve< const xAOD::MuonContainer >( m_muonsName );
  if (!Muons) {
    ATH_MSG_WARNING ("Couldn't retrieve Muons container with key: " << m_muonsName);
    return StatusCode::SUCCESS;
  } 
  ATH_MSG_DEBUG("Retrieved muons " << Muons->size());

  for(const auto mu : *Muons) handleMuon(mu);

  if (m_doMuonTrackValidation) {
    const xAOD::TrackParticleContainer* MuonTracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_muonTracksName );
    if (!MuonTracks) {
      ATH_MSG_WARNING ("Couldn't retrieve Muon Tracks container with key: " << m_muonTracksName);
      return StatusCode::SUCCESS;
    } 
    ATH_MSG_DEBUG("Retrieved muon tracks " << MuonTracks->size());
    
    for(const auto muTP : *MuonTracks) handleMuonTrack(muTP);
  }

  if (m_doMuonSegmentValidation) {
    const xAOD::MuonSegmentContainer* TruthMuonSegments = evtStore()->tryConstRetrieve< xAOD::MuonSegmentContainer >(m_muonSegmentsTruthName);

    if (!TruthMuonSegments) {
      ATH_MSG_WARNING ("Couldn't retrieve Truth Muon Segments container with key: " << m_muonSegmentsTruthName);
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Retrieved truth muon segments " << TruthMuonSegments->size());
  
    for (const auto truthMuSeg: *TruthMuonSegments) handleTruthMuonSegment(truthMuSeg);
    
    const xAOD::MuonSegmentContainer* MuonSegments = evtStore()->retrieve< const xAOD::MuonSegmentContainer >( m_muonSegmentsName );
    if (!MuonSegments) {
      ATH_MSG_WARNING ("Couldn't retrieve MuonSegments container with key: " << m_muonSegmentsName);
      return StatusCode::SUCCESS;
    } 
    ATH_MSG_DEBUG("Retrieved muon segments " << MuonSegments->size());
    for(const auto muSeg : *MuonSegments) handleMuonSegment(muSeg);	
  }

  return StatusCode::SUCCESS;
}

void MuonPhysValMonitoringTool::handleTruthMuonSegment(const xAOD::MuonSegment* truthMuSeg)
{
  const xAOD::MuonSegment* muSeg = findRecoMuonSegment(truthMuSeg);
  ////if (msgLvl(MSG::DEBUG)) printTruthMuonDebug(truthMu, mu);

  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) {
      m_muonSegmentValidationPlots[i]->fill(truthMuSeg, muSeg);   // if no reco muon segment is found a protection inside MuonSegmentValidationPlots will ensure, its plots won't be filled
      break;
    }
  }

  // @@@ FIX: find origin of particle from which this segment originates

  // ElementLink< xAOD::TruthParticleContainer > truthLink;
  // if( truthMuSeg->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") ) {
  //   truthLink = truthMuSeg->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
  // }
  // else {
  //   ATH_MSG_WARNING("No truth link available for muon truth segment");
  //   return;
  // }
  // if( truthLink ) {
  //   if (truthLink.isValid()) {
  //     std::cout<< truthLink.auxdata< int > ("truthType") << std::endl;
  //   }
  // }
  //   //
}

void MuonPhysValMonitoringTool::handleMuonSegment(const xAOD::MuonSegment* muSeg)
{
  if( !muSeg ) {
    ATH_MSG_WARNING("No muon segment found");
    return;
  }
  if (std::find(std::begin(m_vMatchedMuonSegments), std::end(m_vMatchedMuonSegments), muSeg) != std::end(m_vMatchedMuonSegments)) return;
  m_oUnmatchedRecoMuonSegmentPlots->fill(*muSeg);
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) { 
      m_muonSegmentValidationPlots[i]->fill(muSeg);
      break;
    }
  }
}


void MuonPhysValMonitoringTool::handleMuon(const xAOD::Muon* mu)
{
  if (msgLvl(MSG::DEBUG)) printMuonDebug(mu);
  if (std::find(std::begin(m_vMatchedMuons), std::end(m_vMatchedMuons), mu) != std::end(m_vMatchedMuons)) return;
  //unmatched reco muons (not matched with any kind of truth particle, fakes)
  m_oUnmatchedRecoMuonPlots->fill(*mu);
  h_overview_reco_category->Fill("Unmatched",1);
  h_overview_reco_authors[3]->Fill(mu->author());
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) { 
      m_muonValidationPlots[i]->fill(*mu);
      break;
    }
  }
}


void MuonPhysValMonitoringTool::handleTruthMuon(const xAOD::TruthParticle* truthMu)
{
  const xAOD::Muon* mu = findRecoMuon(truthMu);
  if (msgLvl(MSG::DEBUG)) printTruthMuonDebug(truthMu, mu);
  if( truthMu->pt() < 5000. || fabs(truthMu->eta()) > 2.5 ) return;
  
  unsigned int thisMuonCategory = getMuonTruthCategory(truthMu);

  if (mu) {
    h_overview_reco_category->Fill(thisMuonCategory-1);
    h_overview_reco_authors[thisMuonCategory-1]->Fill(mu->author());
  }

  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory) {
      m_muonValidationPlots[i]->fill(truthMu, mu); //if no muon is found a protection inside MuonValidationPlots will ensure, its plots won't be filled
    }
  }

  if (!mu) m_oUnmatchedTruthMuonPlots->fill(*truthMu);
}

void MuonPhysValMonitoringTool::handleMuonTrack(const xAOD::TrackParticle* muTP)
{
  if( !muTP ) {
    ATH_MSG_WARNING("No track particle found");
    return;
  }
  
  ElementLink< xAOD::TruthParticleContainer > truthLink;
  if( muTP->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") ) {
    truthLink = muTP->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
  }
  else {
    ATH_MSG_WARNING("No truth link available");
    return;
  }

  if( truthLink ) {
    if (!truthLink.isValid() || muTP->auxdata< int >("truthType")==0 ){
      //unmatched - unknown
      m_oUnmatchedRecoMuonTrackPlots->fill(*muTP);
      for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
	if (m_selectMuonCategories[i]==ALL) {
	  m_muonTrackValidationPlots[i]->fill(*muTP);
	  break;
	}
      }
    }
    else {
      unsigned int thisMuonCategory = getMuonTruthCategory(muTP);
      for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
	if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory) {
	  m_muonTrackValidationPlots[i]->fill(*truthLink, muTP); //if no muon is found a protection inside MuonValidationPlots will ensure, its plots won't be filled
	}
      }
    }
  }

  return;
}

void MuonPhysValMonitoringTool::printMuonDebug(const xAOD::Muon* mu)
{
  const xAOD::TrackParticle* tp  = mu->primaryTrackParticle();
  ElementLink< xAOD::TruthParticleContainer > truthLink;
  if( tp  ){
    if( !tp->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") ) ATH_MSG_DEBUG("No truth link found");
    else truthLink = tp->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
  }
  ATH_MSG_DEBUG("Muon: pt " << mu->pt() << " eta " << mu->eta() << " link " <<truthLink.isValid() );    
}

const xAOD::Muon* MuonPhysValMonitoringTool::findRecoMuon(const xAOD::TruthParticle* truthMu)
{
  typedef ElementLink< xAOD::MuonContainer > MuonLink;
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


StatusCode MuonPhysValMonitoringTool::procHistograms() 
{
  ATH_MSG_INFO ("Finalising hists " << name() << "...");
  for (const auto plots: m_muonValidationPlots) plots->finalize();
  m_oUnmatchedRecoMuonPlots->finalize();
  m_oUnmatchedTruthMuonPlots->finalize();

  if (m_doMuonTrackValidation) {
    for (const auto plots: m_muonTrackValidationPlots) plots->finalize();
    m_oUnmatchedRecoMuonTrackPlots->finalize();
  }

  if (m_doMuonSegmentValidation) {
    for (const auto plots: m_muonSegmentValidationPlots) plots->finalize();
    m_oUnmatchedRecoMuonSegmentPlots->finalize();
  }

  return StatusCode::SUCCESS;
}

const xAOD::MuonSegment* MuonPhysValMonitoringTool::findRecoMuonSegment(const xAOD::MuonSegment* truthMuSeg)
{
  typedef ElementLink< xAOD::MuonSegmentContainer > MuonSegmentLink;
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

MuonPhysValMonitoringTool::MUCATEGORY MuonPhysValMonitoringTool::getMuonTruthCategory(const xAOD::IParticle* mu)
{
  if (mu->auxdata< int >("truthType") == 6) return PROMPT;
  else if (mu->auxdata< int >("truthType") == 20 && (mu->auxdata< int >("truthOrigin") == 34 || mu->auxdata< int >("truthOrigin") == 35)) return INFLIGHT;
  return REST;
}

} //close namespace
