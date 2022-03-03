/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysMonitorAlgorithm.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "FourMomUtils/xAODP4Helpers.h"
#include "xAODBPhys/BPhysHelper.h"
#include "TrigInDetToolInterfaces/TrigParticleTable.h"

using namespace Gaudi::Units;

TrigBphysMonitorAlgorithm::TrigBphysMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigBphysMonitorAlgorithm::~TrigBphysMonitorAlgorithm() {}


StatusCode TrigBphysMonitorAlgorithm::initialize() {
  
  for(auto MonitoredContainerName : m_ContainerNames) {
    SG::ReadHandleKey<xAOD::TrigBphysContainer> BphysContainerKey(MonitoredContainerName);
    ATH_CHECK( BphysContainerKey.initialize() );
    m_TrigBphysContainerKeys.push_back(BphysContainerKey);
  }
  
  ATH_CHECK( m_offlineMuonCollectionKey.initialize() );
  ATH_CHECK( m_offlineIDTrackCollectionKey.initialize() );
  
  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  
  return StatusCode::SUCCESS;
  
}


StatusCode TrigBphysMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    
  // fill the container hists
  ATH_MSG_DEBUG ("fill... Containers");
  if (fillContainers(ctx).isFailure()) {ATH_MSG_ERROR("Problems filling Container histograms");}
  
  // fill the chain hists
  ATH_MSG_DEBUG ("fill... Chains");
  if (fillChains(ctx).isFailure()) {ATH_MSG_ERROR("Problems filling Chain histograms");}
  
  // build offline dimuons and fill offline histograms
  std::vector<std::unique_ptr<xAOD::Vertex>> offlineDimuons;
  if (buildDimuons(ctx, offlineDimuons).isFailure()) {
    ATH_MSG_ERROR("Problems building offline dimuons, won't fill corresponding histograms");
  }
  else {
    if (fillOfflineDimuons(ctx, offlineDimuons).isFailure()) {ATH_MSG_ERROR("Problems filling Offline dimuon histograms");}
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillContainers(const EventContext& ctx) const {
  
  for(const SG::ReadHandleKey<xAOD::TrigBphysContainer>& monitoredContainerKey : m_TrigBphysContainerKeys) {
    ATH_MSG_DEBUG("Process container " << monitoredContainerKey.key().c_str());
    if(fillContainerHists(ctx, monitoredContainerKey).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling %s container histograms", monitoredContainerKey.key().c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillContainerHists(const EventContext& ctx, const SG::ReadHandleKey<xAOD::TrigBphysContainer>& trigBphysContainerKey) const {
  SG::ReadHandle<xAOD::TrigBphysContainer> trigBphysContainer(trigBphysContainerKey, ctx);
  if( !trigBphysContainer.isValid() ) {
    ATH_MSG_ERROR("No valid TrigBphysContainer with tag: " << trigBphysContainerKey);
  }
  else {
    // Fill container-wise histograms
    std::string monGroupName = std::string("Container_")+trigBphysContainerKey.key();
    auto monGroup = getGroup(monGroupName);
    
    auto ncandidates = Monitored::Scalar<int>("ncandidates",-999);
    ncandidates = trigBphysContainer->size();
    
    auto bphys_mass = Monitored::Collection("bphys_mass", (*trigBphysContainer),  [](const xAOD::TrigBphys* bphys){ return bphys->fitmass() / GeV;});
    
    fill(monGroup, ncandidates, bphys_mass);
    
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigBphysMonitorAlgorithm::fillChains(const EventContext& ctx) const {
  
  for(const auto& monitoredChain : m_ChainNames_MuMu) {
    ATH_MSG_DEBUG("Process chain " << monitoredChain);
    if( !getTrigDecisionTool()->isPassed(monitoredChain) ) {
      ATH_MSG_DEBUG("Chain " << monitoredChain << " is not passed");
      continue;
    }
    if(fillDimuonChainHists(ctx, monitoredChain).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling %s chain histograms", monitoredChain.c_str()));
    }
  }
  
  for(const auto& monitoredChain : m_ChainNames_MuMuX) {
    ATH_MSG_DEBUG("Process chain " << monitoredChain);
    if( !getTrigDecisionTool()->isPassed(monitoredChain) ) {
      ATH_MSG_DEBUG("Chain " << monitoredChain << " is not passed");
      continue;
    }
    if(fillBmumuxChainHists(ctx, monitoredChain).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling %s chain histograms", monitoredChain.c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillDimuonChainHists(const EventContext& ctx, const std::string& chainName) const {
  ATH_MSG_DEBUG("Filling  " << chainName << " chain histograms");
  
  std::string monGroupName = std::string("Chain_")+chainName;
  auto monGroup = getGroup(monGroupName);
    
  if (fillChainGenericHists(ctx, monGroup, chainName).isFailure()) {
    ATH_MSG_ERROR(Form("Problems filling generic histograms for %s chain",chainName.c_str()));
  }
  
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(chainName, TrigDefs::Physics);
  for (const auto& featLinkInfo: chainFeatureContainer){
    ATH_CHECK(featLinkInfo.isValid());
    const auto featLink = featLinkInfo.link;
    if (fillBphysObjectHists(monGroup, featLink, "dimu").isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling bphys object histograms for %s chain",chainName.c_str()));
    }
    if (fillMuonHists(monGroup, featLink).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling muon histograms for %s chain",chainName.c_str()));
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigBphysMonitorAlgorithm::fillBmumuxChainHists(const EventContext& ctx, const std::string& chainName) const {
  ATH_MSG_DEBUG("Filling  " << chainName << " chain histograms");
  
  std::string monGroupName = std::string("Chain_")+chainName;
  auto monGroup = getGroup(monGroupName);
    
  if (fillChainGenericHists(ctx, monGroup, chainName).isFailure()) {
    ATH_MSG_ERROR(Form("Problems filling generic histograms for %s chain",chainName.c_str()));
  }
  
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(chainName, TrigDefs::Physics);
  for (const auto& featLinkInfo: chainFeatureContainer){
    ATH_CHECK(featLinkInfo.isValid());
    const auto featLink = featLinkInfo.link;
    if (fillBphysObjectHists(monGroup, featLink, "B").isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling bphys object histograms for %s chain",chainName.c_str()));
    }
    if (fillTrkHists(monGroup, featLink).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling muon histograms for %s chain",chainName.c_str()));
    }
    
    auto dimuonLink = (*featLink)->lowerChainLink();
    ATH_CHECK(dimuonLink.isValid());
    if (fillBphysObjectHists(monGroup, dimuonLink, "dimu").isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling dimuon object histograms for %s chain",chainName.c_str()));
    }
    if (fillMuonHists(monGroup, dimuonLink).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling muon histograms for %s chain",chainName.c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
}


// Function to fill general per-chain hists, independent on the type of chains
StatusCode TrigBphysMonitorAlgorithm::fillChainGenericHists(const EventContext& /*ctx*/, const ToolHandle<GenericMonitoringTool>& currentMonGroup, const std::string& chainName) const {
  
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(chainName, TrigDefs::Physics);
  auto ncandidates = Monitored::Scalar<int>("ncandidates",-999);
  ncandidates = chainFeatureContainer.size();
  
  fill(currentMonGroup, ncandidates);
  
  return StatusCode::SUCCESS;
}


// Function to fill per-object hists (e.g. for dimuon, B from Bmumux, or X from Bmumux)
StatusCode TrigBphysMonitorAlgorithm::fillBphysObjectHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer> bphysLink, const std::string& objStr) const {
  
  auto dimu_mass = Monitored::Scalar<float>(objStr+"_mass",-999.);
  auto dimu_fitmass = Monitored::Scalar<float>(objStr+"_fitmass",-999.);
  auto dimu_pt = Monitored::Scalar<float>(objStr+"_pt",-999.);
  auto dimu_y = Monitored::Scalar<float>(objStr+"_y",-999.);
  auto dimu_chi2 = Monitored::Scalar<float>(objStr+"_chi2",-999.);
  
  dimu_mass = (*bphysLink)->mass() / GeV;
  dimu_fitmass = (*bphysLink)->fitmass() / GeV;
  dimu_pt   = (*bphysLink)->pt() / GeV;
  dimu_chi2 = (*bphysLink)->fitchi2();
  dimu_y    = (*bphysLink)->eta();
  
  fill(currentMonGroup, dimu_mass, dimu_fitmass, dimu_pt, dimu_y, dimu_chi2);
  
  return StatusCode::SUCCESS;
}


// Function to fill per-muon hists, assuming that the passed object is a dimuon
StatusCode TrigBphysMonitorAlgorithm::fillMuonHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer> bphysLink) const {
  
  const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*bphysLink)->trackParticleLinks();
  if (trackVector.size() <2) {
      ATH_MSG_ERROR("Unexpected number of tracks in a dimuon: " << trackVector.size());
      return StatusCode::FAILURE;
  }
  
  const xAOD::TrackParticle *trk1(nullptr),*trk2(nullptr);
  ATH_CHECK(trackVector.at(0).isValid());
  ATH_CHECK(trackVector.at(1).isValid());
  trk1 = *trackVector.at(0);
  trk2 = *trackVector.at(1);
  if (!trk1 || !trk2) {
      ATH_MSG_ERROR("Null pointer for track in a dimuon!");
      return StatusCode::FAILURE;
  }
  
  auto mu1_pt  = Monitored::Scalar<float>("mu1_pt",-999.);
  auto mu1_eta = Monitored::Scalar<float>("mu1_eta",-999.);
  auto mu1_d0  = Monitored::Scalar<float>("mu1_d0",-999.);
  auto mu2_pt  = Monitored::Scalar<float>("mu2_pt",-999.);
  auto mu2_eta = Monitored::Scalar<float>("mu2_eta",-999.);
  auto mu2_d0  = Monitored::Scalar<float>("mu2_d0",-999.);
  
  mu1_pt  = trk1->pt() / GeV;
  mu1_eta = trk1->eta();
  mu1_d0  = trk1->d0();
  mu2_pt  = trk2->pt() / GeV;
  mu2_eta = trk2->eta();
  mu2_d0  = trk2->d0();
  
  fill(currentMonGroup, mu1_pt, mu1_eta, mu1_d0, mu2_pt, mu2_eta, mu2_d0);
  
  return StatusCode::SUCCESS;
}

// Function to fill per-track hists for Bmumux candidates
StatusCode TrigBphysMonitorAlgorithm::fillTrkHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer>& bphysLink, UInt_t tracksStartFrom) const {
  
  const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*bphysLink)->trackParticleLinks();
  if (trackVector.size() < tracksStartFrom) {
      ATH_MSG_WARNING("Unexpected number of tracks: "<< trackVector.size() << " found, while expected at least " << tracksStartFrom);
      return StatusCode::SUCCESS;
  }
  
  for(UInt_t i = tracksStartFrom; i < trackVector.size(); ++i) {
    ATH_CHECK(trackVector.at(i).isValid());
    const xAOD::TrackParticle* trk = *trackVector.at(i);
    if (!trk) {
      ATH_MSG_ERROR("Null pointer for a track in a BMuMux candidate");
      return StatusCode::FAILURE;
    }
    auto trk_pt  = Monitored::Scalar<float>("trk_pt",-999.);
    auto trk_eta = Monitored::Scalar<float>("trk_eta",-999.);
    auto trk_d0  = Monitored::Scalar<float>("trk_d0",-999.);
    
    trk_pt  = trk->pt() / GeV;
    trk_eta = trk->eta();
    trk_d0  = trk->d0();
    
    fill(currentMonGroup, trk_pt, trk_eta, trk_d0);
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillOfflineDimuons(const EventContext& ctx, const std::vector<std::unique_ptr<xAOD::Vertex>>& dimuonContainer) const {
  
  std::vector<std::string> dimuonMonGroupNames = {"Any"};
  dimuonMonGroupNames.insert( dimuonMonGroupNames.end(), m_ChainNames_MuMu.begin(), m_ChainNames_MuMu.end() );
  dimuonMonGroupNames.insert( dimuonMonGroupNames.end(), m_ChainNames_MuMuX.begin(), m_ChainNames_MuMuX.end() );
  
  for(const auto& dimuonMonGroupName : dimuonMonGroupNames) {
    ATH_MSG_DEBUG("Process dimuons for " << dimuonMonGroupName);
    if(dimuonMonGroupName != "Any") {
      auto& monitoredChain = dimuonMonGroupName;
      if( !(dimuonMonGroupName == "Any") && !getTrigDecisionTool()->isPassed(monitoredChain) ) {
        ATH_MSG_DEBUG("Chain " << monitoredChain << " is not passed");
        continue;
      }
    }
    if(fillOfflineDimuonHists(ctx, dimuonMonGroupName, dimuonContainer).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling offline dimuon histograms for %s", dimuonMonGroupName.c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillOfflineDimuonHists(const EventContext& /*ctx*/, const std::string& dimuonMonGroupName, const std::vector<std::unique_ptr<xAOD::Vertex>>& dimuonContainer) const {
  ATH_MSG_DEBUG("Filling  " << dimuonMonGroupName << " offline dimuons histograms");
  
  // Do matching
  std::vector<const xAOD::Vertex*> matchedDimuons;
  for(auto& offlineDimuon : dimuonContainer) {
    if( dimuonMonGroupName == "Any" || matchDimuon(offlineDimuon.get(), dimuonMonGroupName) ) 
      matchedDimuons.push_back(offlineDimuon.get());
  }
  
  std::string monGroupName = std::string("OfflineDimu_")+dimuonMonGroupName;
  auto monGroup = getGroup(monGroupName);
  
  auto ncandidates = Monitored::Scalar<int>("ncandidates",-999);
  ncandidates = matchedDimuons.size();
  
  fill(monGroup, ncandidates);
  
  for(auto matchedDimuon : matchedDimuons) {
    if (fillVertexHists(monGroup, matchedDimuon, "dimu").isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling histogram for an offline dimuon vertex in %s", dimuonMonGroupName.c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
  
}

StatusCode TrigBphysMonitorAlgorithm::fillVertexHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const xAOD::Vertex* vertex, const std::string& objStr) const {
  
  xAOD::BPhysHelper dimuonVertexHelper(vertex);
  if(dimuonVertexHelper.nRefTrks() != 2) {
    ATH_MSG_WARNING("Unexpected number of refitted tracks at vertex is " << dimuonVertexHelper.nRefTrks());
    return StatusCode::SUCCESS;
  }
  
  using namespace TrigVtx;
  TLorentzVector dimuonMom = dimuonVertexHelper.totalP({TrigParticleMasses().mass[TrigParticleName::muon], TrigParticleMasses().mass[TrigParticleName::muon]});
  
  auto dimu_mass = Monitored::Scalar<float>(objStr+"_mass",-999.);
  auto dimu_pt = Monitored::Scalar<float>(objStr+"_pt",-999.);
  auto dimu_y = Monitored::Scalar<float>(objStr+"_y",-999.);
  auto dimu_chi2 = Monitored::Scalar<float>(objStr+"_chi2",-999.);
  auto dimu_Lxy = Monitored::Scalar<float>(objStr+"_Lxy",-999.);
  auto dimu_LxySig = Monitored::Scalar<float>(objStr+"_LxySig",-999.);
  
  dimu_mass = dimuonMom.M() / GeV;
  dimu_pt   = dimuonMom.Pt() / GeV;
  dimu_chi2 = vertex->chiSquared();
  dimu_y    = dimuonMom.Rapidity();
  dimu_Lxy  = dimuonVertexHelper.lxy();
  dimu_Lxy  = dimuonVertexHelper.lxy()/dimuonVertexHelper.lxyErr();
  
  fill(currentMonGroup, dimu_mass, dimu_pt, dimu_y, dimu_chi2, dimu_Lxy, dimu_LxySig);
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::buildDimuons(const EventContext& ctx, std::vector<std::unique_ptr<xAOD::Vertex>>& vxContainer) const {
  ATH_MSG_DEBUG( "TrigBphysMonitorAlgorithm::buildDimuons" );
  
  // Get the muons
  SG::ReadHandle<xAOD::MuonContainer> muonHandle(m_offlineMuonCollectionKey,ctx);
  ATH_CHECK(muonHandle.isValid());
  const xAOD::MuonContainer* muonContainer = muonHandle.cptr();
  ATH_MSG_DEBUG("Muon container size " << muonContainer->size());
  
  // Muon preselection
  std::vector<const xAOD::Muon*> selectedMuons;
  for (const auto& mu : *muonContainer) {
    if ( mu == nullptr ) continue;
    if ( mu->muonType() != xAOD::Muon::Combined ) continue; // require combined muons
    if (!mu->inDetTrackParticleLink()) continue; // No muons without ID tracks
    if (!mu->inDetTrackParticleLink().isValid()) continue; // No muons without ID tracks
    selectedMuons.push_back(mu);
  }
  if(selectedMuons.size() < 2) {
    ATH_MSG_DEBUG("Only " << selectedMuons.size() << "muons pass preselection");
    return StatusCode::SUCCESS;
  }
  
  // Build dimuons
  for(auto outerItr=selectedMuons.begin(); outerItr<selectedMuons.end(); ++outerItr){
    for(auto innerItr=(outerItr+1); innerItr!=selectedMuons.end(); ++innerItr){
      const auto muon1 = *innerItr;
      const auto muon2 = *outerItr;
      const auto trackParticle1 = muon1->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      const auto trackParticle2 = muon2->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      // Charge selection
      if(trackParticle1->qOverP() * trackParticle2->qOverP() > 0.) 
        continue;
      // Mass selection
      double dimu_momentum_prefit = dimuonMass(trackParticle1, trackParticle2);
      if( !(dimu_momentum_prefit > m_dimuMassLower_prefit && dimu_momentum_prefit < m_dimuMassUpper_prefit) ) 
        continue;
      // Fit
      std::unique_ptr<xAOD::Vertex> dimuon = dimuonFit(trackParticle1, trackParticle2);
      if(!dimuon) continue;
      if(dimuon->chiSquared() > m_dimuChi2Cut) continue;
      xAOD::BPhysHelper jpsiHelper(dimuon.get());
      jpsiHelper.setRefTrks();
      vxContainer.push_back(std::move(dimuon));
    }
  }

  return StatusCode::SUCCESS;
}

std::unique_ptr<xAOD::Vertex> TrigBphysMonitorAlgorithm::dimuonFit(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2) const {
  
  const Trk::Perigee& mu1Perigee = mu1->perigeeParameters();
  const Trk::Perigee& mu2Perigee = mu2->perigeeParameters();
  int sflag = 0; int errorcode = 0;
  Amg::Vector3D startingPoint = m_vertexPointEstimator->getCirclesIntersectionPoint(&mu1Perigee,&mu2Perigee,sflag,errorcode);
  if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
  const std::vector<const xAOD::TrackParticle*> trackPair = {mu1, mu2};
  std::unique_ptr<xAOD::Vertex> myVxCandidate(m_vertexFitter->fit(trackPair, startingPoint)); 

  return myVxCandidate;
}

bool TrigBphysMonitorAlgorithm::matchDimuon(const xAOD::Vertex* dimuonVertex, const std::string& chainName) const {
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(chainName, TrigDefs::Physics);
  for (const auto& featLinkInfo: chainFeatureContainer){
    if(!featLinkInfo.isValid())
      return false;
    const auto featLink = featLinkInfo.link;
    if( matchDimuon(dimuonVertex, featLink) )
      return true;
  }
  return false;
}

bool TrigBphysMonitorAlgorithm::matchDimuon(const xAOD::Vertex* dimuonVertex, const ElementLink<xAOD::TrigBphysContainer>& bphysLink) const {
  auto offlineTrk1 = dimuonVertex->trackParticle(0);
  auto offlineTrk2 = dimuonVertex->trackParticle(1);
  if(!offlineTrk1 || !offlineTrk2) {
    ATH_MSG_DEBUG("TrackParticle from dimuon is null, won't match");
    return false;
  }
  
  const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*bphysLink)->trackParticleLinks();
  if (trackVector.size() < 2) {
      ATH_MSG_ERROR("Unexpected number of tracks in a dimuon: " << trackVector.size() << ", won't match");
      return false;
  }
  
  const xAOD::TrackParticle *hltTrk1(nullptr),*hltTrk2(nullptr);
  // Assume the first two tracks are always muons'
  if( !trackVector.at(0).isValid() || !trackVector.at(1).isValid() )
    return false;
  hltTrk1 = *trackVector.at(0);
  hltTrk2 = *trackVector.at(1);
  if (!hltTrk1 || !hltTrk2) {
      ATH_MSG_ERROR("Null pointer for track in a dimuon!");
      return false;
  }
  if( ( matchTrackParticles(offlineTrk1, hltTrk1) && matchTrackParticles(offlineTrk2, hltTrk2) ) || 
      ( matchTrackParticles(offlineTrk1, hltTrk2) && matchTrackParticles(offlineTrk2, hltTrk1) ) ) 
    return true;
  else
    return false;
}

bool TrigBphysMonitorAlgorithm::matchTrackParticles(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2) const {
  double deltaR = xAOD::P4Helpers::deltaR(*trk1, *trk2);
  if( deltaR < m_deltaRMatchingThreshold)
    return true;
  else
    return false;
}

double TrigBphysMonitorAlgorithm::dimuonMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2) const {
  using namespace TrigVtx;
  xAOD::TrackParticle::GenVecFourMom_t mom1 = mu1->genvecP4();
  xAOD::TrackParticle::GenVecFourMom_t mom2 = mu2->genvecP4();
  mom1.SetM(TrigParticleMasses().mass[TrigParticleName::muon]);
  mom2.SetM(TrigParticleMasses().mass[TrigParticleName::muon]);
  xAOD::TrackParticle::GenVecFourMom_t dimu_mom = mom1 + mom2;
  return dimu_mom.M();
}
