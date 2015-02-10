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
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
//#include "AthenaBaseComps/AthCheckMacros.h"
#include "MuonHistUtils/MuonEnumDefs.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"

typedef ElementLink< xAOD::MuonContainer > MuonLink;
typedef ElementLink< xAOD::MuonSegmentContainer > MuonSegmentLink;
typedef ElementLink< xAOD::TruthParticleContainer > TruthLink;

namespace MuonPhysValMonitoring {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

MuonPhysValMonitoringTool::MuonPhysValMonitoringTool( const std::string& type, 
						      const std::string& name, 
						      const IInterface* parent ):
  ManagedMonitorToolBase( type, name, parent ),
  m_isData(false),
  m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
  m_oUnmatchedRecoMuonPlots(NULL),
  m_oUnmatchedTruthMuonPlots(NULL),
  m_oUnmatchedRecoMuonTrackPlots(NULL),
  m_oUnmatchedRecoMuonSegmentPlots(NULL),
  h_overview_reco_category(NULL)
{
  declareProperty( "IsData", m_isData = false );
  declareProperty( "MuonContainerName", m_muonsName = "Muons" );
  declareProperty( "MuonTruthParticleContainerName", m_muonsTruthName = "MuonTruthParticles" );
  declareProperty( "MuonTrackContainerName", m_muonTracksName = "MuonSpectrometerTrackParticles" );
  declareProperty( "MuonSegmentContainerName", m_muonSegmentsName = "MuonSegments" );
  declareProperty( "MuonTruthSegmentContainerName", m_muonSegmentsTruthName = "MuonTruthSegments" );
  declareProperty( "SelectMuonAuthors", m_selectMuonAuthors);
  declareProperty( "SelectMuonCategories", m_selectMuonCategories );
  declareProperty( "DoMuonTrackValidation", m_doMuonTrackValidation = true);
  declareProperty( "DoMuonSegmentValidation", m_doMuonSegmentValidation = true);
  declareProperty( "DoBinnedResolutionPlots", m_doBinnedResolutionPlots = true);

  declareProperty( "DoTrigMuonValidation", m_doTrigMuonValidation = false);
  declareProperty( "DoTrigMuonL1Validation", m_doTrigMuonL1Validation = false);
  declareProperty( "DoTrigMuonL2Validation", m_doTrigMuonL2Validation = false);
  declareProperty( "DoTrigMuonEFValidation", m_doTrigMuonEFValidation = false);
  declareProperty( "L1TrigMuonContainerName", m_muonL1TrigName = "LVL1MuonRoIs");
  declareProperty( "EFCombTrigMuonContainerName", m_muonEFCombTrigName = "HLT_xAOD__MuonContainer_MuonEFInfo");

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

  // setup flags
  if (m_doTrigMuonValidation==false) {
    m_doTrigMuonL1Validation = false;
    m_doTrigMuonL2Validation = false;
    m_doTrigMuonEFValidation = false;
  }
  if (m_isData) {
    m_selectMuonCategories.clear();
    m_selectMuonCategories.push_back(ALL); //cannot identigy origin of muon in data
  }

  ATH_CHECK(m_muonSelectionTool.retrieve());

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
              m_doTrigMuonL1Validation,m_doTrigMuonL2Validation,m_doTrigMuonEFValidation ));
    
    if (m_doMuonTrackValidation) 
      m_muonTrackValidationPlots.push_back(new MuonTrackValidationPlots(0, categoryPath, m_isData));
    if (m_doMuonSegmentValidation) {
      if (category!=theMuonCategories[ALL]) continue; //cannot identify the truth origin of segments...
      m_muonSegmentValidationPlots.push_back(new MuonSegmentValidationPlots(0, categoryPath, m_isData));
    }
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
  h_overview_nObjects.clear();
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_truth_muons", "Number of truth Muons per event", 20, -0.5, 19.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_muons", "Number of Muons per event", 20, -0.5, 19.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_tracks", "Number of Tracks per event", 50, -0.5, 49.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_truth_segments", "Number of truth Segments per event", 200, -0.5, 199.5));
  h_overview_nObjects.push_back(new TH1F("Muons_Overview_N_perevent_segments", "Number of Segments per event", 200, -0.5, 199.5));
  for (const auto hist : h_overview_nObjects) {
    if (hist) ATH_CHECK(regHist(hist,"Muons/Overview",all));
  }

  h_overview_reco_category = new TH1F("Muons_reco_category","",4,0,4); //prompt/in-flight/non-isolated/other
  for (int i=1; i<4; i++) { //skip 'All'
    h_overview_reco_category->GetXaxis()->SetBinLabel(i,theMuonCategories[i].c_str());
  } h_overview_reco_category->GetXaxis()->SetBinLabel(4,"Other"); //of some other origin or fakes
  ATH_CHECK(regHist(h_overview_reco_category,"Muons/Overview",all));

  int nAuth = xAOD::Muon::NumberOfMuonAuthors;
  for (int i=1; i<4; i++) {
    h_overview_reco_authors.push_back(new TH1F(("Muons_"+theMuonCategories[i]+"_reco_authors").c_str(),("Muons_"+theMuonCategories[i]+"_reco_authors").c_str(),nAuth+1,-0.5,nAuth+0.5));
  }
  h_overview_reco_authors.push_back(new TH1F("Muons_Other_reco_authors","Muons_Other_reco_authors",nAuth+1,-0.5,nAuth+0.5));
  
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
    std::string sHistName = hist.first->GetName();
    ATH_MSG_DEBUG ("Initializing " << hist.first << " " << sHistName << " " << hist.second << "...");

    //check for histograms that are useless
    if ( sHistName.find( Muon::EnumDefs::toString(xAOD::Muon::MuidSA) ) != std::string::npos &&
	 ( sHistName.find( "ID" ) != std::string::npos || sHistName.find( "MS" ) != std::string::npos ) )
      continue;
    if ( sHistName.find( "_bin_" ) != std::string::npos && ( sHistName.find( "Res_eta" ) != std::string::npos || sHistName.find( "Res_phi" ) != std::string::npos) )
      continue;
    
    modifyHistogram(hist.first);
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
  m_vEFMuons.clear();
  m_vEFMuonsSelected.clear();
  
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
      ATH_MSG_WARNING ("Couldn't retrieve TruthMuons container with key: " << m_muonsTruthName);
      return StatusCode::SUCCESS;
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

  // std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > NewMuons;
  // NewMuons = xAOD::shallowCopyContainer( *Muons );
  // if(!setOriginalObjectLink( *Muons, *NewMuons.first )) { 
  //   ATH_MSG_ERROR("Failed to add original object links to shallow copy of Muons");
  //   return StatusCode::FAILURE;
  // }
  // for(auto mu : *NewMuons.first) {
  //   //decorateMuon(*mu); //@@@
  //   xAOD::Muon::Quality q = mu->quality();
  //   m_muonSelectionTool->setQuality(*mu);
  //   if (mu->quality()>xAOD::Muon::VeryLoose || mu->quality()<xAOD::Muon::Tight) ATH_MSG_WARNING( "Muon quality is wrong");    
  //   //m_muonSelectionTool->setPassesHighPtCuts(*mu);
  //   //m_muonSelectionTool->setPassesIDCuts(*mu);
  //   ATH_MSG_ERROR("@@@ before " << q << ", after " << mu->quality() << ", tool " << m_muonSelectionTool->getQuality(*mu)  );
  //   //if (q!=mu->quality()) ATH_MSG_ERROR("@@@ ok, changed !!");
  //   ///if (mu->quality()!=m_muonSelectionTool->getQuality(*mu)) ATH_MSG_ERROR("@@@  changed 2 !!");
  // }

  /////////////////////////////////////////////////////////////////////// @@@
  // @@@ Temp hack to get the MuonSpectrometerTrackParticle (@MS Entry, not extrapolated), needed for eloss plots
  // Remove when the link to MuonSpectrometerTrackParticle appears in the xAOD muon
  MSTracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( "MuonSpectrometerTrackParticles" );
  if (!MSTracks) {
    ATH_MSG_WARNING ("Couldn't retrieve MS Tracks container");
    return StatusCode::SUCCESS;
  } 
  else ATH_MSG_DEBUG("Retrieved muon tracks " << MSTracks->size());
  /////////////////////////////////////////////////////////////////////// @@@
  
  if (!m_isData) for (const auto truthMu: *TruthMuons) handleTruthMuon(truthMu);
  for (const auto mu: *Muons) handleMuon(mu); //for (auto mu: *NewMuons.first) handleMuon(mu);

  if (m_doMuonTrackValidation) {
    const xAOD::TrackParticleContainer* MuonTracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_muonTracksName );
    if (!MuonTracks) {
      ATH_MSG_WARNING ("Couldn't retrieve Muon Tracks container with key: " << m_muonTracksName);
      return StatusCode::SUCCESS;
    } 
    ATH_MSG_DEBUG("Retrieved muon tracks " << MuonTracks->size());
    h_overview_nObjects[2]->Fill(MuonTracks->size());
    for(const auto muTP : *MuonTracks) handleMuonTrack(muTP);
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
    if(m_doTrigMuonL1Validation){
       const xAOD::MuonRoIContainer* L1TrigMuons = evtStore()->retrieve< const xAOD::MuonRoIContainer >( m_muonL1TrigName );    
       if (!L1TrigMuons) {
         ATH_MSG_WARNING ("Couldn't retrieve L1 Muon Trigger container with key: " << m_muonL1TrigName);
         return StatusCode::SUCCESS;
       }      
       ATH_MSG_DEBUG("Retrieved L1 triggered muons " << L1TrigMuons->size());
       for(const auto TrigL1mu : *L1TrigMuons) handleMuonL1Trigger(TrigL1mu);
    }
    if(m_doTrigMuonL2Validation){
      ATH_MSG_DEBUG("TrigMuonL2Validation not defined");
    }
    if(m_doTrigMuonEFValidation){
       const xAOD::MuonContainer* EFCombTrigMuons = evtStore()->retrieve< const xAOD::MuonContainer >( m_muonEFCombTrigName );    
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
		if(
		   ( (fabs(m_vEFMuons.at(i)->eta()- m_vEFMuonsSelected.at(j)->eta())>0.05) ||
		     (fabs(m_vEFMuons.at(i)->phi()- m_vEFMuonsSelected.at(j)->phi())>0.05)   ) ||
		   ((m_vEFMuons.at(i)->author()-m_vEFMuonsSelected.at(j)->author())!=0)
		   ) cont++;
		
		if (cont==m_vEFMuonsSelected.size()){  
		     m_vEFMuonsSelected.push_back(m_vEFMuons.at(i));
	             break;
		}                         
              }
	  }
       }
       for (unsigned int j=0; j< m_vEFMuonsSelected.size(); j++) {   	 
	 handleMuon(m_vEFMuonsSelected.at(j)); //handleMuonTrigger(m_vEFMuonsSelected.at(j));
       } 
    }
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
  if (!mu) return;
  if (msgLvl(MSG::DEBUG)) printMuonDebug(mu);

  //make deep copy of muon to decorate with quality
  //xAOD::Muon* newmu(0);// = new xAOD::Muon;
  //correctedMuon(*mu,newmu);
  //newmu->makePrivateStore(*mu); 
  // m_muonSelectionTool->setQuality(*newmu);
  // m_muonSelectionTool->setPassesHighPtCuts(*newmu);
  // m_muonSelectionTool->setPassesIDCuts(*newmu);

  if (std::find(std::begin(m_vMatchedMuons), std::end(m_vMatchedMuons), mu) != std::end(m_vMatchedMuons)) return;
  
  //unmatched reco muons (not matched with any kind of truth particle, fakes)
  m_oUnmatchedRecoMuonPlots->fill(*mu);
  h_overview_reco_category->Fill("Other",1);
  h_overview_reco_authors[3]->Fill(mu->author());
 
  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL) {
      m_muonValidationPlots[i]->fill(*mu);
      break;
    }
  }
  //if (newmu) delete newmu;
}


void MuonPhysValMonitoringTool::handleTruthMuon(const xAOD::TruthParticle* truthMu)
{
  const xAOD::Muon* mu = findRecoMuon(truthMu);
  
  if (msgLvl(MSG::DEBUG)) printTruthMuonDebug(truthMu, mu);
  if( truthMu->pt() < 5000. || fabs(truthMu->eta()) > 2.5 ) return;
  
  // xAOD::Muon* newmu(0);
  // if (mu) {
  //   //make deep copy of muon to decorate with quality
  //   newmu = new xAOD::Muon();
  //   std::cout << "@@@>>" << std::endl;
  //   correctedMuon(*mu,newmu);
  //   std::cout << "<<@@@" << std::endl;
  // }
  unsigned int thisMuonCategory = getMuonTruthCategory(truthMu);

  for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
    if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory) {
      m_muonValidationPlots[i]->fill(truthMu, mu, MSTracks); //if no muon is found a protection inside MuonValidationPlots will ensure, its plots won't be filled
    }
  }
  if (mu) {    
    h_overview_reco_category->Fill(thisMuonCategory-1);
    h_overview_reco_authors[thisMuonCategory-1]->Fill(mu->author());
  }
  else m_oUnmatchedTruthMuonPlots->fill(*truthMu);

  //if (newmu) delete newmu;
}

void MuonPhysValMonitoringTool::handleMuonTrack(const xAOD::TrackParticle* muTP)
{
  if( !muTP ) {
    ATH_MSG_WARNING("No track particle found");
    return;
  }
  
  if (m_isData) {
    m_muonTrackValidationPlots.back()->fill(*muTP);
    return;	
  }

  TruthLink truthLink;
  if( muTP->isAvailable<TruthLink>("truthParticleLink") ) {
    truthLink = muTP->auxdata<TruthLink>("truthParticleLink");
  }
  else {
    ATH_MSG_WARNING("No truth link available");
    return;
  }
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
  else { //valid truth link and truthType
    unsigned int thisMuonCategory = getMuonTruthCategory(muTP);
    for (unsigned int i=0; i<m_selectMuonCategories.size(); i++) {
      if (m_selectMuonCategories[i]==ALL || m_selectMuonCategories[i]==thisMuonCategory) {
	m_muonTrackValidationPlots[i]->fill(*truthLink, muTP); //if no muon is found a protection inside MuonValidationPlots will ensure, its plots won't be filled
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
      m_muonValidationPlots[i]->fill(*TrigL1mu);
      break;
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

// void MuonPhysValMonitoringTool::printMuonEFTriggerDebug(const xAOD::Muon* mu)
// {
//   ATH_MSG_DEBUG("   ==> Geometrical selection of EF Trigger muons: pt " << mu->pt() <<" phi " << mu->phi() <<" eta " << mu->eta() <<  " author " << mu->author());    
// }

void MuonPhysValMonitoringTool::printMuonL1TriggerDebug(const xAOD::MuonRoI* TrigL1mu)
{
  ATH_MSG_DEBUG("MuonRoI L1 Trigger: ptThr " << TrigL1mu->thrValue() <<" phi " << TrigL1mu->phi() <<" eta " << TrigL1mu->eta());    
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
  if (mu->auxdata< int >("truthType") == 6) return PROMPT;
  else if (mu->auxdata< int >("truthType") == 8 && (mu->auxdata< int >("truthOrigin") == 34 || mu->auxdata< int >("truthOrigin") == 35)) return INFLIGHT;
  else if ( mu->auxdata< int >("truthType") == 7 ) return NONISO;
  return REST;
}

void MuonPhysValMonitoringTool::decorateMuon(xAOD::Muon*& mu)
{
  //if( outputMuon ) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );
  m_muonSelectionTool->setQuality(*mu);
  if (mu->quality()>xAOD::Muon::VeryLoose || mu->quality()<xAOD::Muon::Tight) ATH_MSG_WARNING( "Muon quality is wrong");
  
  m_muonSelectionTool->setPassesHighPtCuts(*mu);
  m_muonSelectionTool->setPassesIDCuts(*mu);
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

  if (histname.find("MS_Res_phi") != std::string::npos){
    hist->GetXaxis()->Set(50,-0.025,0.025);	
  }
}
  
} //close namespace
