/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************
//
// Created:       19 January   2016
// Last Updated:  22 February  2016
//
// Daniel Marley
// demarley@umich.edu
// University of Michigan, Ann Arbor, MI
//
// File for initializing and making re-clustered jets.
//
***************************************************************/
#include "TopObjectSelectionTools/RCJetMC15.h"

#include "TopConfiguration/TopConfig.h"
#include "AsgTools/AsgTool.h"
#include "AthContainers/ConstDataVector.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "fastjet/ClusterSequence.hh"
#include <fastjet/contrib/EnergyCorrelator.hh>
#include <fastjet/contrib/Nsubjettiness.hh>
#include "JetSubStructureUtils/Qw.h"
#include "JetSubStructureUtils/KtSplittingScale.h"

RCJetMC15::RCJetMC15( const std::string& name ) :
  asg::AsgTool( name ),
  m_name(name),
  m_config(nullptr),
  m_ptcut(0.),
  m_etamax(0.),
  m_trim(0.),
  m_radius(0.),
  m_minradius(0.),
  m_massscale(0.),
  m_useJSS(false),
  m_egamma("EG_"),
  m_jetsyst("JET_"),
  m_muonsyst("MUONS_"),
  m_InJetContainerBase( "AntiKt4EMTopoJets_RC"),
  m_OutJetContainerBase("AntiKtRCJets"),
  m_InputJetContainer(  "AntiKt4EMTopoJets_RC"),
  m_OutputJetContainer( "AntiKtRCJets"),
  m_loose_hashValue(2),
  m_jet_def_rebuild(nullptr),
  m_nSub1_beta1(nullptr),
  m_nSub2_beta1(nullptr),
  m_nSub3_beta1(nullptr),
  m_ECF1(nullptr),
  m_ECF2(nullptr),
  m_ECF3(nullptr),
  m_split12(nullptr),
  m_split23(nullptr),
  m_qw(nullptr),
  m_unique_syst(false){
    declareProperty( "config" , m_config );
    declareProperty( "VarRCjets", m_VarRCjets=false);
    declareProperty( "VarRCjets_rho",       m_VarRCjets_rho="");
    declareProperty( "VarRCjets_mass_scale",m_VarRCjets_mass_scale="");
  }


RCJetMC15::~RCJetMC15() {}


StatusCode RCJetMC15::initialize(){
    /* Initialize the re-clustered jets */
    ATH_MSG_INFO(" Initializing Re-clustered jets ");

    // load the necessary parameters from the Dynamic Keys in the config file
    top::ConfigurationSettings* configSettings = top::ConfigurationSettings::get();

    m_name = m_VarRCjets_rho+m_VarRCjets_mass_scale;
    if (m_VarRCjets){
        m_ptcut  = std::stof(configSettings->value("VarRCJetPt"));        // 100 GeV
        m_etamax = std::stof(configSettings->value("VarRCJetEta"));       // 2.5
        m_trim   = std::stof(configSettings->value("VarRCJetTrim"));      // 0.05 (5% jet pT)
        m_radius = std::stof(configSettings->value("VarRCJetMaxRadius")); // 1.2  (min=0.4)
        m_minradius   = 0.4;                                // 0.4 default (until we have smaller jets!)
        std::string original_rho(m_VarRCjets_rho);
        std::replace( original_rho.begin(), original_rho.end(), '_', '.');
        float rho     = std::stof(original_rho);
        float m_scale = mass_scales.at(m_VarRCjets_mass_scale);
        m_massscale   = rho*m_scale*1e-3;                   // e.g., 2*m_top; in [GeV]!
    }
    else{
        m_ptcut  = std::stof( configSettings->value("RCJetPt") );     // for initialize [GeV] & passSelection
        m_etamax = std::stof( configSettings->value("RCJetEta") );    // for passSelection
        m_trim   = std::stof( configSettings->value("RCJetTrim") );   // for initialize
        m_radius = std::stof( configSettings->value("RCJetRadius") ); // for initialize    
        m_minradius = -1.0;
        m_massscale = -1.0;
	if (configSettings->value("UseRCJetSubstructure") == "True" || configSettings->value("UseRCJetSubstructure") == "true"){
	  m_useJSS = true;
	  ATH_MSG_INFO("Calculating RCJet Substructure");


	  // clean up the null ptrs
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
    }

    for (auto treeName : *m_config->systAllTTreeNames()) {
        // only make a new tool if it is the nominal systematic or one that could affect small-r jets (el, mu, jet)
        std::string hash_name("");

        if ( isUniqueSyst(treeName.second) ){
	  
            if (treeName.second.compare("nominal")!=0) hash_name = treeName.second; // no extra strings for nominal (so all other non-unique systs have same name as nominal)

            m_InputJetContainer  = m_InJetContainerBase+hash_name+m_name;
            m_OutputJetContainer = m_OutJetContainerBase+hash_name+m_name;

            // build a jet re-clustering tool for each case
            JetReclusteringTool* tool = new JetReclusteringTool(treeName.second+m_name);
            top::check(tool->setProperty("InputJetContainer",  m_InputJetContainer),"Failed inputjetcontainer initialize reclustering tool");
            top::check(tool->setProperty("OutputJetContainer", m_OutputJetContainer),"Failed outputjetcontainer initialize reclustering tool");
            top::check(tool->setProperty("ReclusterRadius",    m_radius),"Failed re-clustering radius initialize reclustering tool");
            top::check(tool->setProperty("RCJetPtMin",         m_ptcut*1e-3),"Failed ptmin [GeV] initialize reclustering tool");
            top::check(tool->setProperty("RCJetPtFrac",        m_trim),"Failed pT fraction initialize reclustering tool");
            top::check(tool->setProperty("VariableRMinRadius", m_minradius),"Failed VarRC min radius initialize reclustering tool");
            top::check(tool->setProperty("VariableRMassScale", m_massscale),"Failed VarRC mass scale initialize reclustering tool");
            top::check(tool->initialize(),"Failed to initialize reclustering tool");

            m_jetReclusteringTool.insert({treeName.first,tool}); // insert the re-clustering tool into map
                                                                 // this stores a tool for each systematic based on hash value

            // map of container names to access in event saver
            m_inputContainerNames.insert({treeName.first,m_InputJetContainer});
            m_outputContainerNames.insert({treeName.first,m_OutputJetContainer});

            // make a re-clustering tool for 'loose' events, too.
            if (m_config->doLooseEvents()){
                JetReclusteringTool* tool_loose = new JetReclusteringTool(treeName.second+m_name+"_Loose");
                top::check(tool_loose->setProperty("InputJetContainer",  m_InputJetContainer+"_Loose"),"Failed inputjetcontainer reclustering tool");
                top::check(tool_loose->setProperty("OutputJetContainer", m_OutputJetContainer+"_Loose"),"Failed outputjetcontainer loose initialize reclustering tool");
                top::check(tool_loose->setProperty("ReclusterRadius",    m_radius),"Failed re-clustering radius initialize reclustering tool");
                top::check(tool_loose->setProperty("RCJetPtMin",         m_ptcut*1e-3),"Failed ptmin [GeV] reclustering tool");
                top::check(tool_loose->setProperty("RCJetPtFrac",        m_trim),"Failed pT fraction initialize reclustering tool");
                top::check(tool_loose->setProperty("VariableRMinRadius", m_minradius),"Failed VarRC min radius initialize reclustering tool");
                top::check(tool_loose->setProperty("VariableRMassScale", m_massscale),"Failed VarRC mass scale initialize reclustering tool");
                top::check(tool_loose->initialize(),"Failed to initialize reclustering tool");

                m_jetReclusteringTool.insert({m_loose_hashValue*treeName.first,tool_loose}); // making up a number as index for the loose event
                // map of container names to access in event saver
                m_inputContainerNames.insert({m_loose_hashValue*treeName.first,m_InputJetContainer+"_Loose"});
                m_outputContainerNames.insert({m_loose_hashValue*treeName.first,m_OutputJetContainer+"_Loose"});
            } // end if loose
        } // end if unique syst
        else{


	  m_InputJetContainer  = m_InJetContainerBase+m_name;
	  m_OutputJetContainer = m_OutJetContainerBase+m_name;

	  // map of container names to access in event saver
	  if(m_config->doLooseEvents()){
	    m_inputContainerNames.insert({m_loose_hashValue*treeName.first,m_InputJetContainer+"_Loose"});
	    m_outputContainerNames.insert({m_loose_hashValue*treeName.first,m_OutputJetContainer+"_Loose"});
	  }
	  else{
	    m_inputContainerNames.insert({treeName.first,m_InputJetContainer});
	    m_outputContainerNames.insert({treeName.first,m_OutputJetContainer});
	  }

        }
    } // end for loop over systematics

    ATH_MSG_INFO(" Re-clustered jets initialized ");

    return StatusCode::SUCCESS;
} // end initialize()



StatusCode RCJetMC15::execute(const top::Event& event) {
    /*
      Make the jet container (if necessary) and execute the re-clustering tool
        https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TopPhys/xAOD/TopEvent/trunk/Root/TopEventMaker.cxx#L31
    */
    m_InputJetContainer  = inputContainerName( event.m_hashValue, event.m_isLoose );
    m_OutputJetContainer = rcjetContainerName( event.m_hashValue, event.m_isLoose );


    // -- Save the jet container to the TStore (only if it doesn't already exist!)
    // -- Then, we can access it with the re-clustering tool further down
    if (!evtStore()->contains<xAOD::JetContainer>(m_InputJetContainer)) {
        // Save the nominal container once, and each jet systematic container once
        // Make the new jet container (only do this if we have to!)
        // 22 Feb 2016:
        //   Code significantly shortened to make this container
        //   thanks to email exchange between Davide Gerbaudo & Attila Krasznahorkay

        typedef ConstDataVector< xAOD::JetContainer > CJets;
        std::unique_ptr< CJets > rcjets( new CJets( event.m_jets.begin(), event.m_jets.end(), SG::VIEW_ELEMENTS ) );
        top::check( evtStore()->tds()->record( std::move( rcjets ), m_InputJetContainer ), "Failed to put jets in TStore for re-clustering" );
    } // end if jet container exists

    // const xAOD::JetContainer* smalljets(nullptr);
    // top::check(evtStore()->retrieve(smalljets, "AntiKt4EMTopoJets"), "hi mike it ***REMOVED***ed up");
    //  for (auto smalljet : *smalljets){

    //     if (smalljet->getConstituents().size() < 1) {
    // 	  std::cout << "constituents have been thinned, skipping" << std::endl;
    // 	  ATH_MSG_WARNING("Jet Constituents have been thinned - will not be included in RCJet JSS calculation");
    // 	}
    // 	if (smalljet->jetP4("DFCommonJets_Calib").pt()/1000 < 7.0){
    // 	  std::cout << "constituents have been thinned, skipping" << std::endl;
    // 	  std::cout << "smalljet->getConstituents().size() = " << smalljet->getConstituents().size() << std::endl;
    // 	  std::cout << " pt 0 = " << smalljet->getConstituents()[0].pt() << std::endl;
    // 	  if (!smalljet->getConstituents()[0].pt()) continue;
    // 	  for (auto consti : smalljet->getConstituents())
    // 	    { std::cout << " get consti " << std::endl;}
    // 	  ATH_MSG_WARNING("Jet Constituents have been thinned - will not be included in RCJet JSS calculation");
    // 	}
    //}
	

    // --- EXECUTE --- //
    // only execute if the jet container doesn't exist 
    // (do not re-make the 'nominal' jet container over & over again!)
    if (!evtStore()->contains<xAOD::JetContainer>(m_OutputJetContainer)) {
        int hash_factor = (event.m_isLoose) ? m_loose_hashValue : 1;

        // tools only exist for unique systematics & nominal (save time/space)!
        m_tool_iterator tool_iter = m_jetReclusteringTool.find( hash_factor*event.m_hashValue ); 

        // if this is a unique systematic or nominal, execute from the tool; else execute nominal
        if (tool_iter!=m_jetReclusteringTool.end())
            tool_iter->second->execute();
        else
            m_jetReclusteringTool.at(hash_factor*m_config->nominalHashValue())->execute();
    }

      if (m_useJSS){     
       // get the rcjets
       xAOD::JetContainer* myJets(nullptr);
       top::check(evtStore()->retrieve(myJets,m_OutputJetContainer),"Failed to retrieve RC JetContainer");
   
       // get the subjets and clusters of the rcjets
       for (auto rcjet : *myJets){
       
 	std::vector<fastjet::PseudoJet> clusters;
 	clusters.clear();
 
 	for (auto subjet : rcjet->getConstituents() ){
 	  const xAOD::Jet* subjet_raw = static_cast<const xAOD::Jet*>(subjet->rawConstituent());
 
 	  // Avoid trying to access jets that have had the clusters thinned in TOPQ1
 	  // Is there a better way to do this?
 	  if (subjet_raw->jetP4("DFCommonJets_Calib").pt()/1000 < 7.0){ continue;}
 
 	  for (auto clus_itr : subjet_raw->getConstituents() ){
 	    if(clus_itr->e() > 0){
 	      TLorentzVector temp_p4;
 	      temp_p4.SetPtEtaPhiM(clus_itr->pt(), clus_itr->eta(), clus_itr->phi(), clus_itr->m());
 	      clusters.push_back(fastjet::PseudoJet(temp_p4.Px(),temp_p4.Py(),temp_p4.Pz(),temp_p4.E()));
	    }
 	  }
 	
 	}
            
 	// Now rebuild the large jet from the small jet constituents aka the original clusters
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
       } // end of rcjet loop
     }// end of if useJSS

    

    return StatusCode::SUCCESS;
} // end execute()



StatusCode RCJetMC15::finalize() {
    /*Delete the re-clustering tool*/
    for ( auto& x: m_jetReclusteringTool )
        delete x.second;
    m_jetReclusteringTool.clear();

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


bool RCJetMC15::isUniqueSyst( const std::string syst_name ){
    /* 
       Check if the given systematic (besides nominal) needs a unique container
       Keep this in one function so it easier to update than having multiple checks everywhere.
       Only need jet containers for EGamma, Muon, Jet and Nominal systematics
    */
    m_unique_syst = (syst_name.find(m_egamma)   == 0 ||
                     syst_name.find(m_muonsyst) == 0 ||
                     syst_name.find(m_jetsyst)  == 0 || 
                     syst_name.compare("nominal")==0);

    return m_unique_syst;
}


std::string RCJetMC15::inputContainerName( std::size_t hash_value, bool isLooseEvent ){
    /* Return the name of the input container */
    std::string this_container_name("");
    if (isLooseEvent)
        hash_value *= m_loose_hashValue; // loose events have a slightly different hash value to keep track of

    m_iterator iter = m_inputContainerNames.find( hash_value );

    if (iter!=m_inputContainerNames.end())
        this_container_name = iter->second;
    else
        this_container_name = m_InJetContainerBase;

    return this_container_name;
}


std::string RCJetMC15::rcjetContainerName( std::size_t hash_value, bool isLooseEvent ){
    /* Return the name of the rcjet container for a given systematic */
    std::string this_container_name("");
    if (isLooseEvent)
       	hash_value *= m_loose_hashValue; // loose events have a	slightly different hash	value

    m_iterator iter = m_outputContainerNames.find( hash_value );

    if (iter!=m_outputContainerNames.end())
        this_container_name = iter->second;
    else
        this_container_name = m_OutJetContainerBase;

    return this_container_name;
}


bool RCJetMC15::passSelection(const xAOD::Jet& jet) const {
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
