/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "AthContainers/ConstDataVector.h"


#include "TopParticleLevel/ParticleLevelRCJetObjectLoader.h"
#include "TopEvent/EventTools.h"
#include "TopParticleLevel/ParticleLevelEvent.h"
#include "TopConfiguration/TopConfig.h"
    

ParticleLevelRCJetObjectLoader::ParticleLevelRCJetObjectLoader( const std::shared_ptr<top::TopConfig> & cfg ): asg::AsgTool( "ParticleLevelRCJetObjectLoader" ), 
    m_config(cfg),
  m_ptcut(0.),
  m_etamax(0.),
  m_trim(0.),
  m_radius(0.),
  m_treeName("particleLevel"),
  m_InJetContainerBase( "AntiKt4TruthWZJets_RC"),
  m_OutJetContainerBase("AntiKt10RCTrim"),
  m_InputJetContainer("AntiKt4TruthWZJets_RC"),
  m_OutputJetContainer("AntiKt10RCTrim"){}

ParticleLevelRCJetObjectLoader::~ParticleLevelRCJetObjectLoader() {}


StatusCode ParticleLevelRCJetObjectLoader::initialize(){
    /* Initialize the re-clustered jets */
    ATH_MSG_INFO(" Initializing particle level Re-clustered jets ");
    
    m_ptcut  =  m_config->RCJetPtcut() ;     // for initialize [GeV] & passSelection
    m_etamax =  m_config->RCJetEtacut() ;    // for passSelection
    m_trim   =  m_config->RCJetTrimcut() ;   // for initialize
    m_radius =  m_config->RCJetRadius() ; // for initialize    
    

    m_InputJetContainer  = m_InJetContainerBase;
    m_OutputJetContainer = m_OutJetContainerBase;

    // build a jet re-clustering tool for each case
    m_jetReclusteringTool = new JetReclusteringTool(m_treeName);
    top::check(m_jetReclusteringTool->setProperty("InputJetContainer",  m_InputJetContainer),"Failed inputjetcontainer initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("OutputJetContainer", m_OutputJetContainer),"Failed outputjetcontainer initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("ReclusterRadius",    m_radius),"Failed re-clustering radius initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("RCJetPtMin",         m_ptcut*1e-3),"Failed ptmin [GeV] initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("RCJetPtFrac",        m_trim),"Failed pT fraction initialize reclustering tool");
    top::check(m_jetReclusteringTool->initialize(),"Failed to initialize reclustering tool");
    

    ATH_MSG_INFO(" Re-clustered jets initialized ");

    return StatusCode::SUCCESS;
} // end initialize()



StatusCode ParticleLevelRCJetObjectLoader::execute(const top::ParticleLevelEvent& plEvent) {
    /*
      Make the jet container (if necessary) and execute the re-clustering tool
        https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TopPhys/xAOD/TopEvent/trunk/Root/TopEventMaker.cxx#L31
    */

    // -- Save the jet container to the TStore (only if it doesn't already exist!)
    // -- Then, we can access it with the re-clustering tool further down
    if (!evtStore()->contains<xAOD::JetContainer>(m_InputJetContainer)) {
        typedef ConstDataVector< xAOD::JetContainer > CJets;
        std::unique_ptr< CJets > rcjets( new CJets( plEvent.m_jets->begin(), plEvent.m_jets->end(), SG::VIEW_ELEMENTS ) );
        top::check( evtStore()->tds()->record( std::move( rcjets ), m_InputJetContainer ), "Failed to put jets in TStore for re-clustering" );
    } // end if jet container exists


    // --- EXECUTE --- //
    // only execute if the jet container doesn't exist 
    // (do not re-make the 'nominal' jet container over & over again!)
    if (!evtStore()->contains<xAOD::JetContainer>(m_OutputJetContainer)) {
        m_jetReclusteringTool->execute();
	
	const xAOD::JetContainer* rc_jets(nullptr);
	top::check(evtStore()->retrieve(rc_jets,m_OutputJetContainer),"Failed to retrieve particle RC JetContainer");
	
	for( auto rcjet : *rc_jets ){
	  rcjet->auxdecor<bool>("PassedSelection") = passSelection(*rcjet);
	}
    }

    return StatusCode::SUCCESS;
} // end execute()



StatusCode ParticleLevelRCJetObjectLoader::finalize() {
    /*Delete the re-clustering tool*/
    delete m_jetReclusteringTool;

    return StatusCode::SUCCESS;
}



bool ParticleLevelRCJetObjectLoader::passSelection(const xAOD::Jet& jet) const {
    /*
      Check if the re-clustered jet passes selection.
      Right now, this only does something for |eta| because
      pT is taken care of in the re-clustering tool.  When
      small-r jet mass is available (calibrated+uncertainties), 
      we can cut on that.
    */  
    // [pT] calibrated to >~ 22 GeV (23 Jan 2016)
    if (jet.pt() < m_ptcut)
        return false;

    // [|eta|] calibrated < 2.5
    if (std::fabs(jet.eta()) > m_etamax)
        return false;
        
    // small-r jet mass not calibrated and no uncertainties

  return true;
}
