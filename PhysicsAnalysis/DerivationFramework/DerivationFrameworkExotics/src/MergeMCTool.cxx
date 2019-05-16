/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkExotics/MergeMCTool.h"
#include "xAODEventInfo/EventInfo.h"

DerivationFramework::MergeMCTool::MergeMCTool(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthHistogramAlgorithm(name, pSvcLocator){

  //declareProperty( "RunNumber", m_runNumber = 0 );
}

DerivationFramework::MergeMCTool::~MergeMCTool() {
}

StatusCode DerivationFramework::MergeMCTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");
  CHECK (book (TH1F("MCCount", ";MCID;Sum Weighted Events", 50, -0.5, 49.5)));

// Set bin labels
hist( "MCCount" )->GetXaxis()->SetBinLabel(1,"All W");
hist( "MCCount" )->GetXaxis()->SetBinLabel(2,"All Raw");
hist( "MCCount" )->GetXaxis()->SetBinLabel(3,"Pass VBF");
hist( "MCCount" )->GetXaxis()->SetBinLabel(4,"Fail VBF");
hist( "MCCount" )->GetXaxis()->SetBinLabel(5,"Pass PTV");
hist( "MCCount" )->GetXaxis()->SetBinLabel(6,"Fail PTV");
hist( "MCCount" )->GetXaxis()->SetBinLabel(7,"Pass BFilt");
hist( "MCCount" )->GetXaxis()->SetBinLabel(8,"Pass CFilt");
hist( "MCCount" )->GetXaxis()->SetBinLabel(9,"Pass LFFilt");
hist( "MCCount" )->GetXaxis()->SetBinLabel(10,"Pass QCD");
hist( "MCCount" )->GetXaxis()->SetBinLabel(11,"Fail QCD");
hist( "MCCount" )->GetXaxis()->SetBinLabel(12,"Pass MG");
hist( "MCCount" )->GetXaxis()->SetBinLabel(13,"Fail MG");
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
    bool passJetFilter = passTruthFilter(jets, JetEtaFilter,JetpTFilter,
					 MjjFilter, PhijjFilter);
    
    if(passJetFilter) hist( "MCCount" )->Fill(2.0, weight);
    else              hist( "MCCount" )->Fill(3.0, weight);

    // Implement the b-filter, c filter for the Sherpa samples...maybe for MG too?
bool passBFilter = true;
bool passCFilter = false;
bool passLFFilter=false;
// https://gitlab.cern.ch/atlas/athena/blob/master/Generators/GeneratorFilters/src/HeavyFlavorHadronFilter.cxx
    if(passBFilter) hist( "MCCount" )->Fill(4.0, weight);
    if(passCFilter) hist( "MCCount" )->Fill(5.0, weight);
    if(passLFFilter) hist( "MCCount" )->Fill(6.0, weight);
    
    // implement the pTV filter
bool passPTVFilter=true;
    if(passPTVFilter) hist( "MCCount" )->Fill(7.0, weight);
    else  hist( "MCCount" )->Fill(8.0, weight);
    
    // QCD filter
bool passQCDFilter=passTruthFilter(jets, 5.0, 20e3, 1000.0e3, 2.0);
    if(passQCDFilter) hist( "MCCount" )->Fill(9.0, weight);
    else  hist( "MCCount" )->Fill(10.0, weight);
    
    // MG filtering
// for the Znn - filtSeq.MissingEtFilter.METCut = 100*GeV
// only for the Z we need  dR<0.3 and pT>35 GeV
// filtSeq.VBFMjjIntervalFilter.ElectronJetOverlapRemoval = True
// filtSeq.VBFMjjIntervalFilter.TauJetOverlapRemoval = True
// https://gitlab.cern.ch/atlas/athena/blob/master/Generators/GeneratorFilters/src/MissingEtFilter.cxx
bool passMGFilter=passTruthFilter(jets, JetEtaFilter, 35.0e3,
					 MjjFilter, PhijjFilter);

    if(passMGFilter) hist( "MCCount" )->Fill(11.0, weight);
    else  hist( "MCCount" )->Fill(12.0, weight);
}
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MergeMCTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");

  return StatusCode::SUCCESS;
}

bool DerivationFramework::MergeMCTool::passTruthFilter(const xAOD::JetContainer *truthJets, double JetEtaFilter, double JetpTFilter, double MjjFilter, double PhijjFilter){

   bool passFilter=false;
   std::vector<TLorentzVector> jets;
   TLorentzVector tmp;
   for (const auto &jet : *truthJets) {
     tmp.SetPtEtaPhiM(jet->pt(),jet->eta(),jet->phi(),jet->m() );
     if(fabs(jet->eta() )<JetEtaFilter && jet->pt()>JetpTFilter) jets.push_back(tmp);
   }

   std::sort(jets.begin(),jets.end(),DerivationFramework::MergeMCTool::GreaterPt);

   if(jets.size()>=2){
     double e_DiJetMass = (jets.at(0) + jets.at(1)).M();
//double e_JetsDEta = fabs(jets.at(0).Eta() - jets.at(1).Eta());
     double e_JetsDPhi = fabs(jets.at(0).DeltaPhi(jets.at(1)));
     //std::cout << "mjj " << e_DiJetMass << ", Etajj " << e_JetsDEta << ", Phijj " << e_JetsDPhi << std::endl;
     if (e_DiJetMass > MjjFilter && e_JetsDPhi < PhijjFilter) passFilter=true;
   }

   return passFilter;
}
