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
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h"
#include "JetSubStructureUtils/EnergyCorrelator.h"

ParticleLevelRCJetObjectLoader::ParticleLevelRCJetObjectLoader( const std::shared_ptr<top::TopConfig> & cfg ): asg::AsgTool( "ParticleLevelRCJetObjectLoader" ),m_config(cfg) 
{
    m_ptcut = 0.;
    m_etamax=0.;
    m_trim=0.;
    m_radius=0.;
    m_treeName = "particleLevel";
    m_InJetContainerBase = "AntiKt4TruthWZJets_RC";
    m_OutJetContainerBase = "AntiKt10RCTrim";
    m_InputJetContainer = "AntiKt4TruthWZJets_RC";
    m_OutputJetContainer = "AntiKt10RCTrim";
    m_name = "";
    m_useJSS = false;
    m_useAdditionalJSS = false;
    m_jet_def_rebuild = nullptr;
    m_nSub1_beta1 = nullptr;
    m_nSub2_beta1 = nullptr;
    m_nSub3_beta1 = nullptr;
    m_ECF1 = nullptr;
    m_ECF2 = nullptr;
    m_ECF3 = nullptr;
    m_split12 = nullptr;
    m_split23 = nullptr;
    m_qw = nullptr;
    m_gECF332 = nullptr;
    m_gECF461 = nullptr;
    m_gECF322 = nullptr;
    m_gECF331 = nullptr;
    m_gECF422 = nullptr;
    m_gECF441 = nullptr;
    m_gECF212 = nullptr;
    m_gECF321 = nullptr;
    m_gECF311 = nullptr;
    
    
    declareProperty( "VarRCjets", m_VarRCjets=false);
    declareProperty( "VarRCjets_rho",       m_VarRCjets_rho="");
    declareProperty( "VarRCjets_mass_scale",m_VarRCjets_mass_scale="");
}

ParticleLevelRCJetObjectLoader::~ParticleLevelRCJetObjectLoader() {}


StatusCode ParticleLevelRCJetObjectLoader::initialize(){
    /* Initialize the re-clustered jets */
    ATH_MSG_INFO(" Initializing particle level Re-clustered jets ");
    
    m_name = m_VarRCjets_rho+m_VarRCjets_mass_scale;
    
    
    if(m_VarRCjets){
      m_ptcut  = m_config->VarRCJetPtcut();        // 100 GeV
      m_etamax = m_config->VarRCJetEtacut();       // 2.5
      m_trim   = m_config->VarRCJetTrimcut();      // 0.05 (5% jet pT)
      m_radius = m_config->VarRCJetMaxRadius(); // 1.2  (min=0.4)
      m_minradius   = 0.4;                                // 0.4 default (until we have smaller jets!)
      std::string original_rho(m_VarRCjets_rho);
      std::replace( original_rho.begin(), original_rho.end(), '_', '.');
      float rho     = std::stof(original_rho);
      float m_scale = mass_scales.at(m_VarRCjets_mass_scale);
      m_massscale   = rho*m_scale*1e-3;
      
      m_useJSS=m_config->useVarRCJetSubstructure();
      m_useAdditionalJSS=m_config->useVarRCJetAdditionalSubstructure();        
    }
    else {
      m_ptcut  =  m_config->RCJetPtcut() ;     // for initialize [GeV] & passSelection
      m_etamax =  m_config->RCJetEtacut() ;    // for passSelection
      m_trim   =  m_config->RCJetTrimcut() ;   // for initialize
      m_radius =  m_config->RCJetRadius() ; // for initialize    
      m_minradius = -1.0;
      m_massscale = -1.0;
      
      m_useJSS=m_config->useRCJetSubstructure();
      m_useAdditionalJSS=m_config->useRCJetAdditionalSubstructure();
    }
    
    
    
    

    m_InputJetContainer  = m_InJetContainerBase;
    m_OutputJetContainer = m_OutJetContainerBase + m_name;

    // build a jet re-clustering tool for each case
    m_jetReclusteringTool = std::make_shared<JetReclusteringTool>(m_treeName+m_name);
    top::check(m_jetReclusteringTool->setProperty("InputJetContainer",  m_InputJetContainer),"Failed inputjetcontainer initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("OutputJetContainer", m_OutputJetContainer),"Failed outputjetcontainer initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("ReclusterRadius",    m_radius),"Failed re-clustering radius initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("RCJetPtMin",         m_ptcut*1e-3),"Failed ptmin [GeV] initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("RCJetPtFrac",        m_trim),"Failed pT fraction initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("VariableRMinRadius", m_minradius),"Failed VarRC min radius initialize reclustering tool");
    top::check(m_jetReclusteringTool->setProperty("VariableRMassScale", m_massscale),"Failed VarRC mass scale initialize reclustering tool");
    top::check(m_jetReclusteringTool->initialize(),"Failed to initialize reclustering tool");
    
    if (m_useJSS || m_useAdditionalJSS){
       ATH_MSG_INFO("Calculating RCJet Substructure");

      // Setup a bunch of FastJet stuff
      //define the type of jets you will build (http://fastjet.fr/repo/doxygen-3.0.3/classfastjet_1_1JetDefinition.html)
      m_jet_def_rebuild = std::make_shared<fastjet::JetDefinition>(fastjet::antikt_algorithm, 1.0, fastjet::E_scheme, fastjet::Best);
	  
    }
    if (m_useJSS){
	 
      //Substructure tool definitions
      m_nSub1_beta1 = std::make_shared<fastjet::contrib::Nsubjettiness>(1, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
      m_nSub2_beta1 = std::make_shared<fastjet::contrib::Nsubjettiness>(2, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
      m_nSub3_beta1 = std::make_shared<fastjet::contrib::Nsubjettiness>(3, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
      
      
      m_split12 = std::make_shared<JetSubStructureUtils::KtSplittingScale>(1);
      m_split23 = std::make_shared<JetSubStructureUtils::KtSplittingScale>(2);

      m_qw = std::make_shared<JetSubStructureUtils::Qw>();
      
      
    }
    if (m_useAdditionalJSS){
      
      m_ECF1 = std::make_shared<fastjet::contrib::EnergyCorrelator>(1, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
      m_ECF2 = std::make_shared<fastjet::contrib::EnergyCorrelator>(2, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
      m_ECF3 = std::make_shared<fastjet::contrib::EnergyCorrelator>(3, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
      
      m_gECF332 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(3,3,2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF461 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(6,4,1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF322 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(2,3,2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF331 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(3,3,1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF422 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(2,4,2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF441 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(4,4,1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF212 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(1,2,2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF321 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(2,3,1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      m_gECF311 = std::make_shared<JetSubStructureUtils::EnergyCorrelatorGeneralized>(1,3,1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      
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
	
	const xAOD::JetContainer* myJets(nullptr);
	top::check(evtStore()->retrieve(myJets,m_OutputJetContainer),"Failed to retrieve particle RC JetContainer");
	
	for( auto rcjet : *myJets ){
	  rcjet->auxdecor<bool>("PassedSelection") = passSelection(*rcjet);
	}
	
	if (m_useJSS || m_useAdditionalJSS){     
	  
	  static const SG::AuxElement::ConstAccessor<bool> passedSelection("PassedSelection");
	  
	  for (auto rcjet : *myJets){
	    
	    if(!passedSelection(*rcjet))continue; // Calculate JSS only if passed object selection
	    
	    //	std::cout <<"retrieved PL rcjets" << std::endl;
	    std::vector<fastjet::PseudoJet> clusters;
	    clusters.clear();
	    
	  for (auto subjet : rcjet->getConstituents() ){
	      //std::cout <<"retrieved PL subjets" << std::endl;
	      const xAOD::Jet* subjet_raw = static_cast<const xAOD::Jet*>(subjet->rawConstituent());
	      //std::cout <<"retrieved PL rawjets" << std::endl;
	      
	      const xAOD::TruthParticleContainer* myTruthParticle(nullptr);
        // The truth collection is needed to determine the charge of the particles     
	      if (m_config->sgKeyJetsTDS(m_config->nominalHashValue(),false).find("AntiKt4EMPFlowJets")!=std::string::npos){	
			   top::check(evtStore()->retrieve(myTruthParticle,"TruthParticles"),"Failed to retrieve truth particle JetContainer");	
		
   		}
    
	  for (auto clus_itr : subjet_raw->getConstituents() ){
		//	  std::cout <<"retrieved PL clusters" << std::endl;
		
		TLorentzVector temp_p4;
		temp_p4.SetPtEtaPhiM(clus_itr->pt(), clus_itr->eta(), clus_itr->phi(), clus_itr->m());
		
   
      // Only in case AntiKt4EMPFlowJets are used include in the substructure only the charged component of the substructure
		  if (m_config->sgKeyJetsTDS(m_config->nominalHashValue(),false).find("AntiKt4EMPFlowJets")!=std::string::npos){
        bool isCharged=false;
		    bool found=false;
		
		    for (auto truthParticle: *myTruthParticle ){
		       double epsilon=0.00001;
		       if(fabs(truthParticle->pt() - clus_itr->pt()) < epsilon && fabs(truthParticle->eta() - clus_itr->eta()) < epsilon && fabs(truthParticle->phi() - clus_itr->phi()) < epsilon && fabs(truthParticle->m() - clus_itr->m()) < epsilon ){
		          isCharged = truthParticle->isCharged();
		          found=true;
		          break;
		         }
		   }
		   if(!found) ATH_MSG_WARNING("Substructure element not in the TruthParticle collection");
		   if(!isCharged) continue;
		  }
		
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
     
	    if (m_useJSS){     
	   
	   
     
	      // Now finally we can calculate some substructure!
	      double tau32 = -1, tau21 = -1;
	   
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
       
	      rcjet->auxdecor<float>("d12_clstr") = split12;
	      rcjet->auxdecor<float>("d23_clstr") = split23;
	      rcjet->auxdecor<float>("Qw_clstr") = qw;
	      
	      
	      rcjet->auxdecor<float>("nconstituent_clstr") = clusters.size();
    
	    
	   } // end of if useJSS
	 
	  if (m_useAdditionalJSS){
	    
	    double D2 = -1;
	    
	    double vECF1 = m_ECF1->result(correctedJet);
	    double vECF2 = m_ECF2->result(correctedJet);
	    double vECF3 = m_ECF3->result(correctedJet);
	    if(fabs(vECF2) > 1e-8)
	      D2 = vECF3 * pow(vECF1,3) / pow(vECF2,3);
	    else
	      D2 = -999.0;
	    
	    // MlB's t/H discriminators
	    // E = (a*n) / (b*m)
	    // for an ECFG_X_Y_Z, a=Y, n=Z -> dimenionless variable
	    double gECF332 = m_gECF332->result(correctedJet);
	    double gECF461 = m_gECF461->result(correctedJet);
	    double gECF322 = m_gECF322->result(correctedJet);
	    double gECF331 = m_gECF331->result(correctedJet);
	    double gECF422 = m_gECF422->result(correctedJet);
	    double gECF441 = m_gECF441->result(correctedJet);
	    double gECF212 = m_gECF212->result(correctedJet);
	    double gECF321 = m_gECF321->result(correctedJet);
	    double gECF311 = m_gECF311->result(correctedJet);
    
	    double L1=-999.0, L2=-999.0, L3=-999.0, L4=-999.0, L5=-999.0;
	    if (fabs(gECF212) > 1e-12){
	      L1 = gECF321 / (pow(gECF212,(1.0)));
	      L2 = gECF331 / (pow(gECF212,(3.0/2.0)));
	    }
	    if (fabs(gECF331) > 1e-12){
	      L3 = gECF311 / (pow(gECF331,(1.0/3.0) ));
	      L4 = gECF322 / (pow(gECF331,(4.0/3.0) ));
	    }
	    if (fabs(gECF441) > 1e-12){
	      L5 = gECF422 / (pow(gECF441, (1.0)));
	    }	
	    
	    
	    rcjet->auxdecor<float>("ECF1_clstr") = vECF1;
	    rcjet->auxdecor<float>("ECF2_clstr") = vECF2;
	    rcjet->auxdecor<float>("ECF3_clstr") = vECF3;
	    rcjet->auxdecor<float>("D2_clstr") = D2;
	    
	    rcjet->auxdecor<float>("gECF332_clstr") = gECF332;
	    rcjet->auxdecor<float>("gECF461_clstr") = gECF461;
	    rcjet->auxdecor<float>("gECF322_clstr") = gECF322;
	    rcjet->auxdecor<float>("gECF331_clstr") = gECF331;
	    rcjet->auxdecor<float>("gECF422_clstr") = gECF422;
	    rcjet->auxdecor<float>("gECF441_clstr") = gECF441;
	    rcjet->auxdecor<float>("gECF212_clstr") = gECF212;
	    rcjet->auxdecor<float>("gECF321_clstr") = gECF321;
	    rcjet->auxdecor<float>("gECF311_clstr") = gECF311;
	    rcjet->auxdecor<float>("L1_clstr") = L1;
	    rcjet->auxdecor<float>("L2_clstr") = L2;
	    rcjet->auxdecor<float>("L3_clstr") = L3;
	    rcjet->auxdecor<float>("L4_clstr") = L4;
	    rcjet->auxdecor<float>("L5_clstr") = L5;
	  }

          // lets also store the rebuilt jet in case we need it later
          if (m_useJSS || m_useAdditionalJSS){

	    rcjet->auxdecor<float>("RRCJet_pt") = correctedJet.pt();
	    rcjet->auxdecor<float>("RRCJet_eta") = correctedJet.eta();
	    rcjet->auxdecor<float>("RRCJet_phi") = correctedJet.phi();
	    rcjet->auxdecor<float>("RRCJet_e") = correctedJet.e();
	    
	    
	  }// end of if useAdditional JSS
    
	    
	} // end rcjet loop
      } // //m_useJSS || m_useAdditionalJSS
	
	
    } //if (!evtStore()->contains<xAOD::JetContainer>(m_OutputJetContainer)) 

    
    //   // }

    return StatusCode::SUCCESS;
} // end execute()



StatusCode ParticleLevelRCJetObjectLoader::finalize() {
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
