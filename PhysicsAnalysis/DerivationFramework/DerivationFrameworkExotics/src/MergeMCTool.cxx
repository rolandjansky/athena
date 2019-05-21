/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkExotics/MergeMCTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticle.h"
#include <iostream>
DerivationFramework::MergeMCTool::MergeMCTool(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthHistogramAlgorithm(name, pSvcLocator){

  declareProperty( "DebugPlots", m_debugplots = 0 );
}

DerivationFramework::MergeMCTool::~MergeMCTool() {
}

StatusCode DerivationFramework::MergeMCTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");
  CHECK (book (TH1F("MCCount", ";MCID;Sum Weighted Events", 50, -0.5, 49.5)));
  if(m_debugplots>0){
    CHECK (book (TH1F("ptll23", ";p_{T}^{ll};Particles", 1000, 0.0, 500.0)));
    CHECK (book (TH1F("ptv22", ";p_{T}^{v};Particles", 1000, 0.0, 500.0)));
    CHECK (book (TH1F("ptv44", ";p_{T}^{v};Particles", 1000, 0.0, 500.0)));
    CHECK (book (TH1F("ptv62", ";p_{T}^{v};Particles", 1000, 0.0, 500.0)));
    CHECK (book (TH1F("npart23", ";N;Particles", 20, -0.5, 19.5)));
    CHECK (book (TH1F("ptvFilter", ";p_{T}^{v};Particles", 1000, 0.0, 500.0)));
  }
  // Set bin labels
  hist( "MCCount" )->GetXaxis()->SetBinLabel(1,"All W");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(2,"All Raw");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(3,"Pass VBF");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(4,"Fail VBF");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(5,"Pass BFilt");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(6,"Pass CFilt");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(7,"Pass LFFilt");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(8,"Pass PTV");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(9,"Fail PTV");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(10,"Pass QCD");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(11,"Fail QCD");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(12,"Pass MG VBF");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(13,"Fail MG VBF");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(14,"Pass MG VBF OR");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(15,"Fail MG VBF OR");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(16,"Pass MG Znn,Np01 OR");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(17,"Fail MG Znn,Np01 OR");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(18,"Pass MG ZOther OR");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(19,"Fail MG ZOther OR");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(20,"Pass MG W");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(21,"Fail MG W");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(22,"Pass MG W noFlav");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(23,"Fail MG W noFlav");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(32,"Pass Photon");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(33,"Fail Photon");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(34,"Pass Photon w/Iso");
  hist( "MCCount" )->GetXaxis()->SetBinLabel(35,"Fail Photon w/Iso");

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MergeMCTool::execute()
{
  const xAOD::EventInfo* eventInfo = nullptr;
  ATH_CHECK(evtStore()->retrieve(eventInfo));
  bool  m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  double weight = 1.0;
  if(m_isMC) weight = eventInfo->mcEventWeight();

  hist( "MCCount" )->Fill(1.0);
  hist( "MCCount" )->Fill(0.0, weight);
  if (m_isMC) {
    std::string m_containerName = "AntiKt4TruthJets";
    // retrieve container
    const xAOD::JetContainer* jets = nullptr;
    ATH_CHECK(evtStore()->retrieve(jets, "AntiKt4TruthJets"));
    
    double JetEtaFilter = 5.0;
    double JetpTFilter = 20e3;
    double MjjFilter = 800.0e3;
    double PhijjFilter = 2.5;
    
    // After burner filter for the NLO sherpa samples
    bool passJetFilter = passTruthFilter(nullptr, jets, JetEtaFilter,JetpTFilter,
					 MjjFilter, PhijjFilter);

    if(passJetFilter) hist( "MCCount" )->Fill(2.0, weight); // passJetFilter
    else hist( "MCCount" )->Fill(3.0, weight); // fail
    
    // Read in the truth event
    const xAOD::TruthEventContainer* xTruthEventContainer = nullptr;
    CHECK( evtStore()->retrieve( xTruthEventContainer, "TruthEvents"));

    // Implement the b-filter, c filter for the Sherpa samples...maybe for MG too?
    // https://gitlab.cern.ch/atlas/athena/blob/master/Generators/GeneratorFilters/src/HeavyFlavorHadronFilter.cxx
    SG::AuxElement::ConstAccessor< int >  decoratorFlavourFilter("FlavourFilter");
    int flav=decoratorFlavourFilter(*eventInfo);

    if(flav==1) hist( "MCCount" )->Fill(4.0, weight); // passBFilter
    if(flav==2) hist( "MCCount" )->Fill(5.0, weight); // passCFilter
    if(flav==0) hist( "MCCount" )->Fill(6.0, weight); // passLF
    
    ATH_MSG_VERBOSE( "Flavour" << decoratorFlavourFilter(*eventInfo));

    // implement the pTV filter -->> TODO once we have the sherpa samples
    bool passPTVFilter=true;
    if(passPTVFilter) hist( "MCCount" )->Fill(7.0, weight);
    else  hist( "MCCount" )->Fill(8.0, weight);
    
    // QCD filter
    bool passQCDFilter=passTruthFilter(nullptr, jets, 5.0, 20e3, 1000.0e3, 2.0);
    if(passQCDFilter) hist( "MCCount" )->Fill(9.0, weight);
    else  hist( "MCCount" )->Fill(10.0, weight);
    
    // MG filtering
    // for the Znn - filtSeq.MissingEtFilter.METCut = 100*GeV
    // only for the Z we need  dR<0.3 and pT>35 GeV
    // filtSeq.VBFMjjIntervalFilter.ElectronJetOverlapRemoval = True
    // filtSeq.VBFMjjIntervalFilter.TauJetOverlapRemoval = True
    // https://gitlab.cern.ch/atlas/athena/blob/master/Generators/GeneratorFilters/src/MissingEtFilter.cxx
    bool passMGJetFilter=passTruthFilter(nullptr, jets, JetEtaFilter, 35.0e3,
				      MjjFilter, PhijjFilter);
    // Overlap remove electrons and taus
    bool passMGJetFilterOREleTau = passTruthFilter(xTruthEventContainer, jets, JetEtaFilter,35.0e3,
						 MjjFilter, PhijjFilter);

    if(passMGJetFilter) hist( "MCCount" )->Fill(11.0, weight);
    else  hist( "MCCount" )->Fill(12.0, weight);
    if(passMGJetFilterOREleTau) hist( "MCCount" )->Fill(13.0, weight);
    else  hist( "MCCount" )->Fill(14.0, weight);

    // -FOR W: BVCV + VBF cuts + ptll>100.0
    // -FOR Z: VBFcuts (ele+tau OR) + ptll>75.0 Np01 or Znn
    // -FOR Z: VBFcuts (ele+tau OR) + ptll>100.0 other

    SG::AuxElement::ConstAccessor< float >  decoratorMGVTruthPt("MGVTruthPt");
    float ptv = decoratorMGVTruthPt(*eventInfo);
    if(m_debugplots>0){
      ptv = getPTV(xTruthEventContainer);
    }
    if(ptv>75.0e3 && passMGJetFilterOREleTau) hist( "MCCount" )->Fill(15.0, weight); // Znn,Np01
    else hist( "MCCount" )->Fill(16.0, weight);
    if(ptv>100.0e3 && passMGJetFilterOREleTau) hist( "MCCount" )->Fill(17.0, weight); // other Z
    else hist( "MCCount" )->Fill(18.0, weight);
    if(flav==0 && ptv>100.0e3 && passMGJetFilter) hist( "MCCount" )->Fill(19.0, weight); // W
    else hist( "MCCount" )->Fill(20.0, weight);
    if(ptv>100.0e3 && passMGJetFilter) hist( "MCCount" )->Fill(21.0, weight); // W without flavour
    else hist( "MCCount" )->Fill(22.0, weight);

    // photon merging samples to remove overlap with photos?
    SG::AuxElement::ConstAccessor< bool >  decoratorin_vy_overlap("in_vy_overlap");
    bool passPhotonOR = decoratorin_vy_overlap(*eventInfo);
    if(passPhotonOR) hist( "MCCount" )->Fill(31.0, weight);
    else  hist( "MCCount" )->Fill(32.0, weight);

    // photon merging samples to remove overlap with photos? also with Frixione isolation
    SG::AuxElement::ConstAccessor< bool >  decoratorin_vy_overlap_iso("in_vy_overlap_iso");
    bool passPhotonORIso = decoratorin_vy_overlap_iso(*eventInfo);
    if(passPhotonORIso) hist( "MCCount" )->Fill(33.0, weight);
    else  hist( "MCCount" )->Fill(34.0, weight);
}
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MergeMCTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");

  return StatusCode::SUCCESS;
}

bool DerivationFramework::MergeMCTool::passTruthFilter(const xAOD::TruthEventContainer* xTruthEventContainer, const xAOD::JetContainer *truthJets, double JetEtaFilter, double JetpTFilter, double MjjFilter, double PhijjFilter){

   bool passFilter=false;
   std::vector<TLorentzVector> jets, ele_taus;
   TLorentzVector tmp;

   // load the electrona and taus if requested
   if(xTruthEventContainer){
     xAOD::TruthEventContainer::const_iterator itr;
     for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
       unsigned nPart = (*itr)->nTruthParticles();
       for(unsigned iPart=0; iPart<nPart;++iPart){
	 const xAOD::TruthParticle* particle = (*itr)->truthParticle(iPart);
	 if(particle && particle->status()==1 && (fabs(particle->pdgId())==11 || fabs(particle->pdgId())==15) && particle->pt()>20.0e3 && fabs(particle->eta())<5.0)
	   ele_taus.push_back(particle->p4());
       }
     }
   }
   // Load the truth jets
   for (const auto &jet : *truthJets) {
     tmp.SetPtEtaPhiM(jet->pt(),jet->eta(),jet->phi(),jet->m() );
     if(fabs(jet->eta() )<JetEtaFilter && jet->pt()>JetpTFilter){
       bool passOR=true;
       for(unsigned ell=0; ell<ele_taus.size(); ++ell){
	 if(ele_taus.at(ell).DeltaR(tmp)<0.3) passOR=false;
       }

       if(passOR) jets.push_back(tmp);
     }
   }
   // Compute the jets
   std::sort(jets.begin(),jets.end(),DerivationFramework::MergeMCTool::GreaterPt);

   if(jets.size()>=2){
     double e_DiJetMass = (jets.at(0) + jets.at(1)).M();
     double e_JetsDPhi = fabs(jets.at(0).DeltaPhi(jets.at(1)));
     if (e_DiJetMass > MjjFilter && e_JetsDPhi < PhijjFilter) passFilter=true;
   }

   return passFilter;
}

// Compute the ptv
float DerivationFramework::MergeMCTool::getPTV(const xAOD::TruthEventContainer* xTruthEventContainer){
  float ptv62=-1.0;
  unsigned npart23=0;
  TLorentzVector lep23;
  xAOD::TruthEventContainer::const_iterator itr;
  for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
    unsigned nPart = (*itr)->nTruthParticles();
    for(unsigned iPart=0; iPart<nPart;++iPart){
      const xAOD::TruthParticle* particle = (*itr)->truthParticle(iPart);
      if(particle && ((fabs(particle->pdgId())>=11 && fabs(particle->pdgId())<=18)) && particle->status()==23){ // these are the events without a status 62 W or Z
	lep23+=particle->p4();
	++npart23;
      }
      if(particle && ((fabs(particle->pdgId())>=11 && fabs(particle->pdgId())<=18) || fabs(particle->pdgId())==24 || fabs(particle->pdgId())==23)){// && fabs(particle->eta())<10.0){
	if(particle->status()==22 && (particle->pdgId()==23 || fabs(particle->pdgId())==24)){ hist("ptv22")->Fill(particle->pt()/1.0e3); }
	if(particle->status()==44 && (particle->pdgId()==23 || fabs(particle->pdgId())==24)){ hist("ptv44")->Fill(particle->pt()/1.0e3); }
	if(particle->status()==62 && (particle->pdgId()==23 || fabs(particle->pdgId())==24)){ hist("ptv62")->Fill(particle->pt()/1.0e3); ptv62 = particle->pt(); /*isPart62=true;*/ }
	if((particle->pdgId()==23 || fabs(particle->pdgId())==24))
	  ATH_MSG_VERBOSE("status: " << particle->status() << " pt: " << particle->pt() << " pdg: "  << particle->pdgId() );
      }
    }
  }
  
  hist("ptll23")->Fill(lep23.Pt()/1.0e3);
  hist("npart23")->Fill(npart23);
  
  float ptvFilter=ptv62;
  if(ptv62<0.0) ptvFilter = lep23.Pt();
  hist("ptvFilter")->Fill(ptvFilter/1.0e3);
  
  return ptvFilter;
}
