/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthRivetTools includes
#include "TruthRivetTools/HiggsTruthCategoryTool.h"

HiggsTruthCategoryTool::HiggsTruthCategoryTool( const std::string& name) 
: asg::AsgTool( name ),
  rivetAnaHandler(nullptr),
  higgsTemplateCrossSections(nullptr)
{
  // cannot be set to true until the issue with the beam protons in the truth event record is fixed..
  // see JIRA ticket: https://its.cern.ch/jira/browse/ATLASRECTS-3072?filter=-2
  m_outHistos = false; 
  m_isInitialized = false;
}


StatusCode HiggsTruthCategoryTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  // Rivet analysis :: Higgs truth event classifier class
  higgsTemplateCrossSections = new Rivet::HiggsTemplateCrossSections();
  // crreate an instance of the Rivet analysis handler
  rivetAnaHandler = new Rivet::AnalysisHandler();
  // Add the Higgs truth classifier class to the handler
  rivetAnaHandler->addAnalysis(&(*higgsTemplateCrossSections));
  return StatusCode::SUCCESS;
}

StatusCode HiggsTruthCategoryTool :: finalize () {
  ATH_MSG_INFO (" ====================================================== ");
  ATH_MSG_INFO (" ---- Finalizing" << name() << "...");
  ATH_MSG_INFO (" ====================================================== ");
  if ( !m_outHistos )
    higgsTemplateCrossSections->printClassificationSummary( );
  else{
    // TODO:: update the tool properly deal with output files/paths
    rivetAnaHandler->finalize();
    rivetAnaHandler->writeData("HiggsTruthCategoryTool.yoda");
  }
  ATH_MSG_INFO (" ====================================================== ");
  return StatusCode::SUCCESS;  
}

HTXS::HiggsClassification* HiggsTruthCategoryTool :: getHiggsTruthCategoryObject (const HepMC::GenEvent& HepMCEvent, const HTXS::HiggsProdMode prodMode){
  if ( !m_isInitialized ) {
    higgsTemplateCrossSections->setHiggsProdMode(prodMode); 
    rivetAnaHandler->init(HepMCEvent); 
    m_isInitialized=true;
  }
  // fill histos if flag is specified
  if ( m_outHistos ) rivetAnaHandler->analyze(HepMCEvent);
  // get the category output object containing the template cross section category,
  // and Higgs, V-boson, jets 4-vectors
  const Rivet::HiggsClassification htxs_cat_rivet = higgsTemplateCrossSections->classifyEvent(HepMCEvent,prodMode);  
  return HTXS::Rivet2Root(htxs_cat_rivet);
}

