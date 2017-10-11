/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CPAnalysisExamples/ToolExamplesAlg.h"


#include "PATInterfaces/SystematicsUtil.h" //for make_systematic_vector 

#include "xAODCore/ShallowCopy.h" //for shallowCopyContainer method

ToolExamplesAlg::ToolExamplesAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  //declareProperty( "Property", m_nProperty ); //example property declaration

}


ToolExamplesAlg::~ToolExamplesAlg() {}


StatusCode ToolExamplesAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  //connect the ToolHandles to the tools configured in the joboptions
  //could also have done this by declaring the handles as properties above:
  //   declareProperty("MyTool",handle);
  //and in joboptions doing:
  //algseq.MyAlg.MyTool = ToolSvc.SomeTool
  
  
  ///General 
  m_grl.setTypeAndName("GoodRunsListSelectionTool/GRLTool");
  m_prw.setTypeAndName("CP::PileupReweightingTool/PileupReweightingTool");
  m_tdt.setTypeAndName("Trig::TrigDecisionTool/TrigDecisionTool");
  m_tmt.setTypeAndName("Trig::MatchingTool/TrigMatchingTool");
  
  ///Electrons
  m_elCalib.setTypeAndName("CP::EgammaCalibrationAndSmearingTool/EgammaCalibTool");
  m_elIsoCalib.setTypeAndName("CP::IsolationCorrectionTool/IsolationCorrectionTool");
  m_elSelection.setTypeAndName("AsgElectronLikelihoodTool/ElectronSelectionTool");
  m_elIsoSelection.setTypeAndName("CP__IsolationSelectionTool/ElectronIsolationTool");
  m_elCorrectedIsoSelection.setTypeAndName("CP__IsolationCloseByCorrectionTool/ElectronIsolationCloseByCorrectionTool");
  
  //example of using a ToolHandleArray 
  m_elSFTools.push_back( "AsgElectronEfficiencyCorrectionTool/ElectronRecoSF" );
  m_elSFTools.push_back( "AsgElectronEfficiencyCorrectionTool/ElectronIDSF" );
  m_elSFTools.push_back( "AsgElectronEfficiencyCorrectionTool/ElectronIsolSF" );
  
  
  //Muons
  
  
  //Jets
  
  
  return StatusCode::SUCCESS;
}

StatusCode ToolExamplesAlg::firstExecute() {
  ATH_MSG_INFO ("FirstExecuting " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode ToolExamplesAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode ToolExamplesAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed

  const xAOD::EventInfo* ei = 0; CHECK( evtStore()->retrieve( ei , "EventInfo" ) );
  const xAOD::ElectronContainer* els = 0; CHECK( evtStore()->retrieve( els, "Electrons" ) );
  const xAOD::MuonContainer* mus = 0; CHECK( evtStore()->retrieve( mus, "Muons" ) );
  const xAOD::JetContainer* jets = 0; CHECK( evtStore()->retrieve( jets, "AntiKt4EMTopoJets" ) );

  //GRL Example
  bool passGRL = m_grl->passRunLB(*ei);
  ATH_MSG_VERBOSE( "passGRL = " << passGRL );

  //PRW Example
  CHECK( m_prw->apply(*ei) );
  float pileupWeight = ei->auxdata<float>("PileupWeight");
  uint randomRunNumber = ei->auxdata<uint>("RandomRunNumber");
  ATH_MSG_VERBOSE( "pileupWeight = " << pileupWeight );
  ATH_MSG_VERBOSE( "randomRunNumber = " << randomRunNumber );

  //Trigger Decision Example
  bool passTrigger = m_tdt->isPassed("HLT_e24_lhmedium_L1EM20VH");
  ATH_MSG_VERBOSE( "passTrigger = " << passTrigger );


  //Electron Calibration, Iso Calibration, LH Selection, Iso Selection, Scale Factor Example
  
  CP::SystematicSet elSysts = m_elCalib->recommendedSystematics(); 
  elSysts.insert(m_elIsoCalib->recommendedSystematics());
  std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(elSysts);
  
  for(auto& syst : sysList ) {
    m_elCalib->applySystematicVariation( syst ).setChecked();
    m_elIsoCalib->applySystematicVariation( syst ).setChecked();
    auto calib_els = xAOD::shallowCopyContainer( *els );
    for(auto el : *calib_els.first) {
      m_elCalib->applyCorrection(*el).setChecked();
      m_elIsoCalib->applyCorrection(*el).setChecked();
      Root::TAccept acc = m_elSelection->accept(el); //can treat 'acc' as a bool
      Root::TAccept isoAcc = m_elIsoSelection->accept(*el);
      for(auto tool : m_elSFTools) {
        double sf(1.);
        tool->getEfficiencyScaleFactor( *el , sf ).setChecked(); //fills sf with scale factor
      }
    }
    const std::string&& systName = (syst.name()=="") ? "Nominal" : syst.name();
    CHECK( evtStore()->record(calib_els.first,"Electrons" + systName ) ); 
    CHECK( evtStore()->record(calib_els.second,"Electrons" + systName + "Aux.") ); 
    
    //CloseBy-corrected Isolation Selection example
    std::vector<const xAOD::IParticle*> all_els;
    for(auto el : *calib_els.first) all_els.push_back(el);
    
    for(auto el : *calib_els.first) {
      Root::TAccept acc = m_elCorrectedIsoSelection->acceptCorrected(*el, all_els);
    }
    
  }
  

  


  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode ToolExamplesAlg::beginInputFile() { 
  ATH_MSG_INFO ("beginInputFile...");
  return StatusCode::SUCCESS;
}


