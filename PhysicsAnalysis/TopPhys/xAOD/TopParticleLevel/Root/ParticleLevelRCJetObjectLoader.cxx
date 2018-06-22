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

#include "fastjet/ClusterSequence.hh"
#include <fastjet/contrib/Nsubjettiness.hh>
#include <fastjet/contrib/EnergyCorrelator.hh>
#include "JetSubStructureUtils/Qw.h"
#include "JetSubStructureUtils/KtSplittingScale.h"

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
  m_OutputJetContainer("AntiKt10RCTrim"),
  m_jet_def_rebuild(nullptr),
  m_nSub1_beta1(nullptr),
  m_nSub2_beta1(nullptr),
  m_nSub3_beta1(nullptr),
  m_ECF1(nullptr),
  m_ECF2(nullptr),
  m_ECF3(nullptr),
  m_split12(nullptr),
  m_split23(nullptr),
  m_qw(nullptr){}

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
    
    if (m_config->useRCJetSubstructure()){
      m_useJSS = true;
      ATH_MSG_INFO("Calculating PL RCJet Substructure");
      //clean up the null ptrs
      delete m_jet_def_rebuild;
      delete m_nSub1_beta1;
      delete m_nSub2_beta1;
      delete m_nSub3_beta1;
      delete m_ECF1;
      delete m_ECF2;
      delete m_ECF3;
      delete m_split12;
      delete m_split23;
      delete m_qw;

      // Setup a bunch of FastJet stuff
      //define the type of jets you will build (http://fastjet.fr/repo/doxygen-3.0.3/classfastjet_1_1JetDefinition.html)
      m_jet_def_rebuild = new fastjet::JetDefinition(fastjet::antikt_algorithm, 1.0, fastjet::E_scheme, fastjet::Best);
	  
      //Substructure tool definitions
      m_nSub1_beta1 = new fastjet::contrib::Nsubjettiness(1, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
      m_nSub2_beta1 = new fastjet::contrib::Nsubjettiness(2, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
      m_nSub3_beta1 = new fastjet::contrib::Nsubjettiness(3, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
      m_ECF1 = new fastjet::contrib::EnergyCorrelator(1, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
      m_ECF2 = new fastjet::contrib::EnergyCorrelator(2, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
      m_ECF3 = new fastjet::contrib::EnergyCorrelator(3, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
      m_split12 = new JetSubStructureUtils::KtSplittingScale(1);
      m_split23 = new JetSubStructureUtils::KtSplittingScale(2);
      m_qw = new JetSubStructureUtils::Qw();
    }
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
    }

    if (m_useJSS){     
      xAOD::JetContainer* myJets(nullptr);
      top::check(evtStore()->retrieve(myJets,m_OutputJetContainer),"Failed to retrieve Particle Level RC JetContainer");
      for (auto rcjet : *myJets){
    	//	std::cout <<"retrieved PL rcjets" << std::endl;
    	std::vector<fastjet::PseudoJet> clusters;
    	clusters.clear();
	
    	for (auto subjet : rcjet->getConstituents() ){
    	  //std::cout <<"retrieved PL subjets" << std::endl;
    	  const xAOD::Jet* subjet_raw = static_cast<const xAOD::Jet*>(subjet->rawConstituent());
    	  //std::cout <<"retrieved PL rawjets" << std::endl;

    	  for (auto clus_itr : subjet_raw->getConstituents() ){
    	    //	  std::cout <<"retrieved PL clusters" << std::endl;
    	    TLorentzVector temp_p4;
    	    temp_p4.SetPtEtaPhiM(clus_itr->pt(), clus_itr->eta(), clus_itr->phi(), clus_itr->m());
    	    clusters.push_back(fastjet::PseudoJet(temp_p4.Px(),temp_p4.Py(),temp_p4.Pz(),temp_p4.E()));
    	  }
    	}

    	fastjet::ClusterSequence clust_seq_rebuild = fastjet::ClusterSequence(clusters, *m_jet_def_rebuild);
    	 std::vector<fastjet::PseudoJet> my_pjets =  fastjet::sorted_by_pt(clust_seq_rebuild.inclusive_jets(0.0) );
 
    	 fastjet::PseudoJet correctedJet;
    	 correctedJet = my_pjets[0];
    	 //Sometimes fastjet splits the jet into two, so need to correct for that!!
    	if(my_pjets.size() > 1)
    	  correctedJet += my_pjets[1]; 
 
    	// Now finally we can calculate some substructure!
    	double tau32 = -1, tau21 = -1, D2 = -1;
     
    	double tau1 = m_nSub1_beta1->result(correctedJet);
    	double tau2 = m_nSub2_beta1->result(correctedJet);
    	double tau3 = m_nSub3_beta1->result(correctedJet);
 
    	if(fabs(tau1) > 1e-8)
    	  tau21 = tau2/tau1;
    	else
    	  tau21 = -999.0;
    	if(fabs(tau2) > 1e-8)
    	  tau32 = tau3/tau2;
    	else
    	  tau32 = -999.0;

 	double vECF1 = m_ECF1->result(correctedJet);
 	double vECF2 = m_ECF2->result(correctedJet);
 	double vECF3 = m_ECF3->result(correctedJet);
 	if(fabs(vECF2) > 1e-8)
 	  D2 = vECF3 * pow(vECF1,3) / pow(vECF2,3);
 	else
 	  D2 = -999.0;
       
 	double split12 = m_split12->result(correctedJet);
 	double split23 = m_split23->result(correctedJet);
 	double qw = m_qw->result(correctedJet);
       
    	// now attach the results to the original jet
    	rcjet->auxdecor<float>("Tau32_clstr") = tau32;
    	rcjet->auxdecor<float>("Tau21_clstr") = tau21;
 	// lets also write out the components so we can play with them later 
 	rcjet->auxdecor<float>("Tau3_clstr") = tau3;
 	rcjet->auxdecor<float>("Tau2_clstr") = tau2;
 	rcjet->auxdecor<float>("Tau1_clstr") = tau1;
 
 	rcjet->auxdecor<float>("D2_clstr") = D2;
 	rcjet->auxdecor<float>("ECF1_clstr") = vECF1;
 	rcjet->auxdecor<float>("ECF2_clstr") = vECF2;
 	rcjet->auxdecor<float>("ECF3_clstr") = vECF3;
 
 	rcjet->auxdecor<float>("d12_clstr") = split12;
 	rcjet->auxdecor<float>("d23_clstr") = split23;
 	rcjet->auxdecor<float>("Qw_clstr") = qw;
       
 	// lets also store the rebuilt jet incase we need it later
 	rcjet->auxdecor<float>("RRCJet_pt") = correctedJet.pt();
 	rcjet->auxdecor<float>("RRCJet_eta") = correctedJet.eta();
 	rcjet->auxdecor<float>("RRCJet_phi") = correctedJet.phi();
 	rcjet->auxdecor<float>("RRCJet_e") = correctedJet.e();


	// std::cout << "RCJet pT = " <<  rcjet->p4().Pt() << std::endl;
	// std::cout << "RRCJet pT= " <<  correctedJet.pt() << std::endl;

	
       } // end rcjet loop
    } // end if useJSS
    //   // }

    return StatusCode::SUCCESS;
} // end execute()



StatusCode ParticleLevelRCJetObjectLoader::finalize() {
    /*Delete the re-clustering tool*/
    delete m_jetReclusteringTool;
    // clean up the JSS stuff
    delete m_jet_def_rebuild;
    delete m_nSub1_beta1;
    delete m_nSub2_beta1;
    delete m_nSub3_beta1;
    delete m_ECF1;
    delete m_ECF2;
    delete m_ECF3;
    delete m_split12;
    delete m_split23;
    delete m_qw;
    
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
