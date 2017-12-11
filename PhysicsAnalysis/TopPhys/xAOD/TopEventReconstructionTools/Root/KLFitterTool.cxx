/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: KLFitterTool.cxx 790035 2016-12-15 19:47:38Z aknue $

// Local include(s):
#include "TopEventReconstructionTools/KLFitterTool.h"
#include "TopEvent/KLFitterResult.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "PathResolver/PathResolver.h"
#include "KLFitter/LikelihoodTopAllHadronic.h"

#include <algorithm>

namespace top{
  
  KLFitterTool::KLFitterTool( const std::string& name) :
    asg::AsgTool( name ),
    m_config(nullptr),
    m_massTop(172.5), // This is the MC top mass in GeV - only change if you change the MC mass  
    m_bTagCutValue(9999.9), 
    m_transferFunctionsPathPrefix("SetMe"),
    m_transferFunctionsPath("SetMe"),
    m_leptonType("SetMe"),
    m_LHType("SetMe"),
    m_myFitter(nullptr)
  {
    declareProperty( "config" , m_config , "Set the configuration" );
    declareProperty( "LeptonType", m_leptonType = "kUndefined" , "Define the lepton type" );
    declareProperty( "LHType",     m_LHType = "kUndefined" , "Define the Likelihood type" );
  }
  
  /// Function initialising the tool
  StatusCode KLFitterTool::initialize()
  {    
    // Have you set the config??
    if (m_config == nullptr) {
      ATH_MSG_ERROR("Please set the top::TopConfig");
      return StatusCode::FAILURE;
    }
    m_config->setKLFitter();
    m_LHType = m_config -> KLFitterLH();
    m_transferFunctionsPathPrefix = PathResolverFindCalibDirectory( "KLFitter/transferfunctions/" );
    m_transferFunctionsPath = m_config->KLFitterTransferFunctionsPath();
    
    std::string transferFunctionAbsPath = m_transferFunctionsPathPrefix + m_transferFunctionsPath + "/";
    
    // 1) create an instance of the fitter
    m_myFitter = std::unique_ptr<KLFitter::Fitter>( new KLFitter::Fitter{} );
    
    // 2) create an instance of the detector, which holds the information on the resolutions (transfer functions);
    // it takes as an argument the folder which contains the parameter files for the transfer functions
    KLFitter::DetectorBase * myDetector = new KLFitter::DetectorAtlas_8TeV( transferFunctionAbsPath );
    
    // 3) tell the fitter which detector to use
    if (!m_myFitter->SetDetector(myDetector)) {
      ATH_MSG_ERROR( "ERROR setting detector to fitter" );
      return StatusCode::FAILURE;
    }    
    
    // 4) create an instance of the likelihood for ttbar->l+jets channel and customize it according to your needs
    KLFitter::LikelihoodTopLeptonJets * myLikelihood     = new KLFitter::LikelihoodTopLeptonJets{}; 
 
    // 4) create an instance of the likelihood for ttH -> l+jets channel and customize it according to your needs
    KLFitter::LikelihoodTTHLeptonJets * myLikelihood_TTH = new KLFitter::LikelihoodTTHLeptonJets{};

    // 4) create an instance of the likelihood for ttbar->l+jets channel using jet angles channel and customize it according to your needs
    KLFitter::LikelihoodTopLeptonJets_JetAngles * myLikelihood_JetAngles = new KLFitter::LikelihoodTopLeptonJets_JetAngles{};

    // 4) create an instance of the likelihood for ttZ -> trilepton channel and customize it according to your needs
    KLFitter::LikelihoodTTZTrilepton * myLikelihood_TTZ  = new KLFitter::LikelihoodTTZTrilepton{};

    // 4) create an instance of the likelihood for ttbar -> allhadronic channel and customize it according to your needs
    KLFitter::LikelihoodTopAllHadronic * myLikelihood_AllHadronic  = new KLFitter::LikelihoodTopAllHadronic{};

    // 4.a) SetleptonType
    if (m_LHType != "ttbar_AllHadronic"){ // no lepton type for all hadronic
      if (m_leptonType == "kElectron") {
        m_leptonTypeKLFitterEnum = KLFitter::LikelihoodTopLeptonJets::LeptonType::kElectron;
        m_leptonTypeKLFitterEnum_TTH = KLFitter::LikelihoodTTHLeptonJets::LeptonType::kElectron;
        m_leptonTypeKLFitterEnum_JetAngles = KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kElectron;
        m_leptonTypeKLFitterEnum_TTZ = KLFitter::LikelihoodTTZTrilepton::LeptonType::kElectron;
      }
      else if (m_leptonType == "kMuon") {
        m_leptonTypeKLFitterEnum = KLFitter::LikelihoodTopLeptonJets::LeptonType::kMuon;
        m_leptonTypeKLFitterEnum_TTH = KLFitter::LikelihoodTTHLeptonJets::LeptonType::kMuon;
        m_leptonTypeKLFitterEnum_JetAngles = KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kMuon;
        m_leptonTypeKLFitterEnum_TTZ = KLFitter::LikelihoodTTZTrilepton::LeptonType::kMuon;
      } 
      else if (m_leptonType == "kTriElectron") {
        if (m_LHType != "ttZTrilepton") {
          ATH_MSG_ERROR(" LeptonType kTriElectron is only defined for the ttZTrilepton likelihood");
          return StatusCode::FAILURE;
        }
        m_leptonTypeKLFitterEnum = KLFitter::LikelihoodTopLeptonJets::LeptonType::kElectron;
        m_leptonTypeKLFitterEnum_TTH = KLFitter::LikelihoodTTHLeptonJets::LeptonType::kElectron;
        m_leptonTypeKLFitterEnum_JetAngles = KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kElectron;
        m_leptonTypeKLFitterEnum_TTZ = KLFitter::LikelihoodTTZTrilepton::LeptonType::kElectron;
      }
      else if (m_leptonType == "kTriMuon") {
        if (m_LHType != "ttZTrilepton") {
          ATH_MSG_ERROR(" LeptonType kTriMuon is only defined for the ttZTrilepton likelihood");
          return StatusCode::FAILURE;
        }
        m_leptonTypeKLFitterEnum = KLFitter::LikelihoodTopLeptonJets::LeptonType::kMuon;
        m_leptonTypeKLFitterEnum_TTH = KLFitter::LikelihoodTTHLeptonJets::LeptonType::kMuon;
        m_leptonTypeKLFitterEnum_JetAngles = KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kMuon;
        m_leptonTypeKLFitterEnum_TTZ = KLFitter::LikelihoodTTZTrilepton::LeptonType::kMuon;
      }
      else {
        ATH_MSG_ERROR(" Please supply a valid LeptonType : kElectron or kMuon");
        return StatusCode::FAILURE;
      }
          
      myLikelihood     -> SetLeptonType( m_leptonTypeKLFitterEnum ); 
      myLikelihood_TTH -> SetLeptonType( m_leptonTypeKLFitterEnum_TTH );
      myLikelihood_JetAngles -> SetLeptonType( m_leptonTypeKLFitterEnum_JetAngles );
      myLikelihood_TTZ -> SetLeptonType( m_leptonTypeKLFitterEnum_TTZ );

    }
    // 4.b) Jet Selection Mode
    if (m_config->KLFitterJetSelectionMode() == "kLeadingFour" ){
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kLeadingFour;
    }
    else if (m_config->KLFitterJetSelectionMode() == "kLeadingFive") {
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kLeadingFive;
    }
    else if (m_config->KLFitterJetSelectionMode() == "kLeadingSix") {
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kLeadingSix;
    }
    else if (m_config->KLFitterJetSelectionMode() == "kLeadingSeven") {
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kLeadingSeven;
    }
    else if (m_config->KLFitterJetSelectionMode() == "kBtagPriorityFourJets") {
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kBtagPriorityFourJets;
    }
    else if (m_config->KLFitterJetSelectionMode() == "kBtagPriorityFiveJets") {
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kBtagPriorityFiveJets;
    }  
    else if (m_config->KLFitterJetSelectionMode() == "kBtagPrioritySixJets") {
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kBtagPrioritySixJets;
    }
    else if (m_config->KLFitterJetSelectionMode() == "kBtagPrioritySevenJets") {
      m_jetSelectionModeKLFitterEnum = top::KLFitterJetSelection::kBtagPrioritySevenJets;
    }
    else {
      ATH_MSG_ERROR("Please supply a valid JetSelectionMode : kLeadingFour , kLeadingFive , kLeadingSix , kLeadingSeven , kBtagPriorityFourJets , kBtagPriorityFiveJets , kBtagPrioritySixJets , kBtagPrioritySevenJets" );
      return StatusCode::FAILURE;      
    }
    
    if(m_jetSelectionModeKLFitterEnum != top::KLFitterJetSelection::kLeadingSix && m_jetSelectionModeKLFitterEnum != top::KLFitterJetSelection::kBtagPrioritySixJets && m_jetSelectionModeKLFitterEnum != top::KLFitterJetSelection::kLeadingSeven && m_jetSelectionModeKLFitterEnum != top::KLFitterJetSelection::kBtagPrioritySevenJets){
      if(m_LHType == "ttH" || m_LHType == "ttbar_AllHadronic"){

	ATH_MSG_ERROR("You want to run the ttH or ttbar_AllHadronic Likelihood, you need to use either : kLeadingSix , kBtagPrioritySixJets , kLeadingSeven , kBtagPrioritySevenJets" );
	return StatusCode::FAILURE;
	
      }
    } 

    // 4.c) SetBTagging method
    if (m_config->KLFitterBTaggingMethod() == "kNotag") {
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kNotag;
    }
    else if (m_config->KLFitterBTaggingMethod() == "kVetoNoFit" ){
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kVetoNoFit;
    }
    else if (m_config->KLFitterBTaggingMethod() == "kVetoNoFitLight" ){
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kVetoNoFitLight;
    }
    else if (m_config->KLFitterBTaggingMethod() == "kVetoNoFitBoth" ){
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kVetoNoFitBoth;
    }
    else if (m_config->KLFitterBTaggingMethod() == "kWorkingPoint" ){
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kWorkingPoint;
    }    
    else if (m_config->KLFitterBTaggingMethod() == "kVeto" ){
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kVeto;
    }    
    else if (m_config->KLFitterBTaggingMethod() == "kVetoLight" ){
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kVetoLight;
    }
    else if (m_config->KLFitterBTaggingMethod() == "kVetoBoth" ){
      m_bTaggingMethodKLFitterEnum = KLFitter::LikelihoodBase::BtaggingMethod::kVetoBoth;
    }        
    else {
      ATH_MSG_ERROR("Please supply a valid BTaggingMethod : kNotag,kVetoNoFit,kVetoNoFitLight,kVetoNoFitBoth,kWorkingPoint,kVeto,kVetoLight or kVetoBoth" );
      return StatusCode::FAILURE;
    }
    myLikelihood             -> SetBTagging( m_bTaggingMethodKLFitterEnum );
    myLikelihood_TTH         -> SetBTagging( m_bTaggingMethodKLFitterEnum );
    myLikelihood_JetAngles   -> SetBTagging( m_bTaggingMethodKLFitterEnum );
    myLikelihood_TTZ         -> SetBTagging( m_bTaggingMethodKLFitterEnum );
    myLikelihood_AllHadronic -> SetBTagging( m_bTaggingMethodKLFitterEnum );
    // 4.d) SetTopMass
    myLikelihood             -> PhysicsConstants()->SetMassTop( m_massTop ); 
    myLikelihood_TTH         -> PhysicsConstants()->SetMassTop( m_massTop );
    myLikelihood_JetAngles   -> PhysicsConstants()->SetMassTop( m_massTop );
    myLikelihood_TTZ         -> PhysicsConstants()->SetMassTop( m_massTop );
    myLikelihood_AllHadronic -> PhysicsConstants()->SetMassTop( m_massTop );
    // 4.e) TopMassFixed
    myLikelihood             -> SetFlagTopMassFixed( m_config->KLFitterTopMassFixed() ); 
    myLikelihood_TTH         -> SetFlagTopMassFixed( m_config->KLFitterTopMassFixed() );
    myLikelihood_JetAngles   -> SetFlagTopMassFixed( m_config->KLFitterTopMassFixed() );
    myLikelihood_TTZ         -> SetFlagTopMassFixed( m_config->KLFitterTopMassFixed() );
    myLikelihood_AllHadronic -> SetFlagTopMassFixed( m_config->KLFitterTopMassFixed() );

    // 5) tell the fitter which likelihood to use
    if(m_LHType == "ttbar")
      m_myFitter->SetLikelihood(myLikelihood);  
    else if (m_LHType == "ttH")
      m_myFitter->SetLikelihood(myLikelihood_TTH);
    else if (m_LHType == "ttbar_JetAngles")
      m_myFitter->SetLikelihood(myLikelihood_JetAngles);
    else if (m_LHType == "ttZTrilepton" && (m_leptonType == "kTriElectron" || m_leptonType == "kTriMuon")) {
      // For ttZ->trilepton, we can have difficult combinations of leptons in the
      // final state (3x same flavour, or mixed case). The latter is trivial, for
      // which we can default back to the ljets likelihood. So we distinguish here:
      //  - kTriMuon, kTriElectron: dedicated TTZ->trilepton likelihood,
      //  - kMuon, kElectron: standard ttbar->l+jets likelihood.
      m_myFitter->SetLikelihood(myLikelihood_TTZ);
    } else if (m_LHType == "ttZTrilepton") {
      m_myFitter->SetLikelihood(myLikelihood);
    } else if (m_LHType == "ttbar_AllHadronic"){
      m_myFitter->SetLikelihood(myLikelihood_AllHadronic);
    }

    else{

      ATH_MSG_ERROR( "KLFitter: This likelihood is not defined..." );
      return StatusCode::FAILURE;

    }
    
    // 6) Figure out the b tagging working point
    // All the blame for this horrible code rests with the b-tagging people
    if (m_config->bTagWP_available().size() != 1) {
      ATH_MSG_INFO(m_config->bTagWP_available().size()<<" b-tagging WP - cannot pick b-jets. Please select only 1 WP if you are using KLFitter");
    }
    
    const auto btagWP = m_config->bTagWP_available()[0];
    m_btagging_eff_tool = "BTaggingEfficiencyTool_"+btagWP+"_"+m_config->sgKeyJets();
    top::check( m_btagging_eff_tool.retrieve(), "Failed to retrieve b-tagging Efficiency tool" );
    
    ATH_MSG_INFO("++++++++++++++++++++++++++++++");
    ATH_MSG_INFO("Configured KLFitter with name "<<name());
    ATH_MSG_INFO("  Using " << "BTaggingEfficiencyTool_"+btagWP+"_"+m_config->sgKeyJets());
    ATH_MSG_INFO("  Using transfer functions with full path " << transferFunctionAbsPath);
    ATH_MSG_INFO("  Using Lepton \t\t"         << m_leptonType);
    ATH_MSG_INFO("  Using JetSelectionMode \t" << m_config->KLFitterJetSelectionMode());
    ATH_MSG_INFO("  Using BTaggingMethod \t"   << m_config->KLFitterBTaggingMethod());
    ATH_MSG_INFO("  Using TopMassFixed \t"     << m_config->KLFitterTopMassFixed());
    
    if (m_config->KLFitterSaveAllPermutations()) 
        ATH_MSG_INFO("  Saving All permutations");
    if (!m_config->KLFitterSaveAllPermutations())
        ATH_MSG_INFO("  Saving only the permutation with the highest event probability");
    ATH_MSG_INFO("++++++++++++++++++++++++++++++");    
    
    /// Return gracefully:
    return StatusCode::SUCCESS;     
  }
  
  /// Function executing the tool
  StatusCode KLFitterTool::execute(const top::Event& event)
  {
    
    // run KLFitter
    // create an instance of the particles class filled with the particles to be fitted;
    // here, you need to make sure that
    // - the particles are in the range allowed by the transfer functions (eta and pt)
    // - the energies and momenta are in GeV
    // - be aware that *all* particles you're adding are considered in the fit
    //   (many particles lead to many permutations to be considered and hence a long
    //   running time and not necessarily good fitting results due to the many available
    //   permutations)
    // the arguments taken py AddParticle() are
    // - TLorentzVector of the physics 4-momentum
    // - detector eta for the evaluation of the transfer functions (for muons: just use the physics eta)
    // - type of particle
    // - an optional name of the particle (pass empty string in case you don't want to give your particle a name)
    // - index of the particle in your original collection (for convenience)
    // - for jets:
    //   * bool isBtagged : mandatory only if you want to use b-tagging in the fit
  
    KLFitter::Particles * myParticles = new KLFitter::Particles{};

    if(m_LHType == "ttbar"){

      if (m_leptonTypeKLFitterEnum == KLFitter::LikelihoodTopLeptonJets::LeptonType::kElectron) {
	TLorentzVector el;
	el.SetPtEtaPhiE( event.m_electrons.at(0)->pt()/1.e3 ,  event.m_electrons.at(0)->eta() , event.m_electrons.at(0)->phi(),  event.m_electrons.at(0)->e() / 1.e3);
	myParticles->AddParticle(&el, event.m_electrons.at(0)->caloCluster()->etaBE(2) , KLFitter::Particles::kElectron);
      }
      if (m_leptonTypeKLFitterEnum == KLFitter::LikelihoodTopLeptonJets::LeptonType::kMuon) {
	TLorentzVector mu;
	mu.SetPtEtaPhiE( event.m_muons.at(0)->pt()/1.e3 ,  event.m_muons.at(0)->eta() , event.m_muons.at(0)->phi(),  event.m_muons.at(0)->e() / 1.e3);
	myParticles->AddParticle(&mu, mu.Eta(), KLFitter::Particles::kMuon);
      }
    }
    if(m_LHType == "ttH"){

      if (m_leptonTypeKLFitterEnum_TTH == KLFitter::LikelihoodTTHLeptonJets::LeptonType::kElectron) {
        TLorentzVector el;
        el.SetPtEtaPhiE( event.m_electrons.at(0)->pt()/1.e3 ,  event.m_electrons.at(0)->eta() , event.m_electrons.at(0)->phi(),  event.m_electrons.at(0)->e() / 1.e3);
        myParticles->AddParticle(&el, event.m_electrons.at(0)->caloCluster()->etaBE(2) , KLFitter::Particles::kElectron);
      }
      if (m_leptonTypeKLFitterEnum_TTH == KLFitter::LikelihoodTTHLeptonJets::LeptonType::kMuon) {
        TLorentzVector mu;
        mu.SetPtEtaPhiE( event.m_muons.at(0)->pt()/1.e3 ,  event.m_muons.at(0)->eta() , event.m_muons.at(0)->phi(),  event.m_muons.at(0)->e() / 1.e3);
        myParticles->AddParticle(&mu, mu.Eta(), KLFitter::Particles::kMuon);
      }
    }
    if(m_LHType == "ttbar_JetAngles"){
      if (m_leptonTypeKLFitterEnum_JetAngles == KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kElectron) {
	TLorentzVector el;
	el.SetPtEtaPhiE( event.m_electrons.at(0)->pt()/1.e3 ,  event.m_electrons.at(0)->eta() , event.m_electrons.at(0)->phi(),  event.m_electrons.at(0)->e() / 1.e3);
	myParticles->AddParticle(&el, event.m_electrons.at(0)->caloCluster()->etaBE(2) , KLFitter::Particles::kElectron);
      }
      if (m_leptonTypeKLFitterEnum_JetAngles == KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kMuon) {
	TLorentzVector mu;
	mu.SetPtEtaPhiE( event.m_muons.at(0)->pt()/1.e3 ,  event.m_muons.at(0)->eta() , event.m_muons.at(0)->phi(),  event.m_muons.at(0)->e() / 1.e3);
	myParticles->AddParticle(&mu, mu.Eta(), KLFitter::Particles::kMuon);
      }
    }

    if (m_LHType == "ttZTrilepton") {
      if (m_leptonTypeKLFitterEnum_TTZ == KLFitter::LikelihoodTTZTrilepton::LeptonType::kElectron) {
        if (m_leptonType == "kTriElectron") {
          // This is the "true" trilepton case with three leptons of the same flavour.
          if (event.m_electrons.size() < 3) {
            ATH_MSG_ERROR( "KLFitter: kTriElectron requires three electrons..." );
            return StatusCode::FAILURE;
          }
          TLorentzVector el;
          for (unsigned int i = 0; i < 3; ++i) {
            const auto& electron = event.m_electrons.at(i);
            el.SetPtEtaPhiE(electron->pt() / 1.e3, electron->eta(), electron->phi(), electron->e() / 1.e3);
            myParticles->AddParticle(&el, el.Eta(), KLFitter::Particles::kElectron, "", i);
          }
        } else {
          // Trivial case of mixed lepton flavours. Use ttbar->l+jets likelihood and only add the single lepton.
          TLorentzVector el;
          el.SetPtEtaPhiE(event.m_electrons.at(0)->pt() / 1.e3, event.m_electrons.at(0)->eta(), event.m_electrons.at(0)->phi(), event.m_electrons.at(0)->e() / 1.e3);
          myParticles->AddParticle(&el, event.m_electrons.at(0)->caloCluster()->etaBE(2) , KLFitter::Particles::kElectron);
        }
      }

      if (m_leptonTypeKLFitterEnum_TTZ == KLFitter::LikelihoodTTZTrilepton::LeptonType::kMuon) {
        if (m_leptonType == "kTriMuon") {
          // This is the "true" trilepton case with three leptons of the same flavour.
          if (event.m_muons.size() < 3) {
            ATH_MSG_ERROR( "KLFitter: kTriMuon requires three muons..." );
            return StatusCode::FAILURE;
          }
          TLorentzVector mu;
          for (unsigned int i = 0; i < 3; ++i) {
            const auto& muon = event.m_muons.at(i);
            mu.SetPtEtaPhiE(muon->pt() / 1.e3, muon->eta(), muon->phi(), muon->e() / 1.e3);
            myParticles->AddParticle(&mu, mu.Eta(), KLFitter::Particles::kMuon, "", i);
          }
        } else {
          // Trivial case of mixed lepton flavours. Use ttbar->l+jets likelihood and only add the single lepton.
          TLorentzVector mu;
          mu.SetPtEtaPhiE(event.m_muons.at(0)->pt() / 1.e3, event.m_muons.at(0)->eta(), event.m_muons.at(0)->phi(), event.m_muons.at(0)->e() / 1.e3);
          myParticles->AddParticle(&mu, mu.Eta(), KLFitter::Particles::kMuon);
        }
      }
    }

    // set the jets, depending on the Jet Selection Mode
    setJets(event, myParticles); 
    
    // add the particles to the fitter
    if (!m_myFitter->SetParticles(myParticles)) {
      ATH_MSG_ERROR( "KLFitter: Error adding particles to fitter..." );
      return StatusCode::FAILURE;
    } 
    
    // add the MET x and y components as well as the SumET to the fitter
    const double met_ex = event.m_met->mpx() / 1.e3;
    const double met_ey = event.m_met->mpy() / 1.e3;
    const double met_sumet = event.m_met->sumet() / 1.e3;

    if (!m_myFitter->SetET_miss_XY_SumET(met_ex, met_ey, met_sumet)) {
      ATH_MSG_ERROR( "KLFitter: Error adding MET to fitter..." );
      return StatusCode::FAILURE;
    }    
    
    // create the xAOD::KLFitterResultContainer
    xAOD::KLFitterResultAuxContainer* resultAuxContainer = new xAOD::KLFitterResultAuxContainer{};
    xAOD::KLFitterResultContainer* resultContainer = new xAOD::KLFitterResultContainer{};
    resultContainer->setStore( resultAuxContainer );
    
    
    // loop over all permutations
    const int nperm = m_myFitter->Permutations()->NPermutations();
    for (int iperm  = 0; iperm < nperm; ++iperm) {
      // Perform the fit
      m_myFitter->Fit(iperm); 
      // create a result 
      xAOD::KLFitterResult* result = new xAOD::KLFitterResult{};
      resultContainer->push_back( result );
      
      unsigned int ConvergenceStatusBitWord = m_myFitter->ConvergenceStatus();
      bool MinuitDidNotConverge = (ConvergenceStatusBitWord & m_myFitter->MinuitDidNotConvergeMask) != 0;
      bool FitAbortedDueToNaN = (ConvergenceStatusBitWord & m_myFitter->FitAbortedDueToNaNMask) != 0;
      bool AtLeastOneFitParameterAtItsLimit = (ConvergenceStatusBitWord & m_myFitter->AtLeastOneFitParameterAtItsLimitMask) != 0;
      bool InvalidTransferFunctionAtConvergence = (ConvergenceStatusBitWord & m_myFitter->InvalidTransferFunctionAtConvergenceMask) != 0;      

      result->setMinuitDidNotConverge( ((MinuitDidNotConverge) ? 1 : 0) );
      result->setFitAbortedDueToNaN( ((FitAbortedDueToNaN) ? 1 : 0) );
      result->setAtLeastOneFitParameterAtItsLimit( ((AtLeastOneFitParameterAtItsLimit) ? 1 : 0 ) );
      result->setInvalidTransferFunctionAtConvergence( ((InvalidTransferFunctionAtConvergence) ? 1 : 0) );
      
      result->setLogLikelihood( m_myFitter->Likelihood()->LogLikelihood(m_myFitter->Likelihood()->GetBestFitParameters()) );
      result->setEventProbability( std::exp(m_myFitter->Likelihood()->LogEventProbability()) );
      result->setParameters( m_myFitter->Likelihood()->GetBestFitParameters() );
      result->setParameterErrors( m_myFitter->Likelihood()->GetBestFitParameterErrors() );
      
      KLFitter::Particles * myModelParticles = m_myFitter->Likelihood()->ParticlesModel();
      KLFitter::Particles ** myPermutedParticles = m_myFitter->Likelihood()->PParticlesPermuted();
      

      if (m_LHType == "ttbar" || m_LHType == "ttH" || m_LHType == "ttbar_JetAngles" || m_LHType == "ttZTrilepton"){

        result->setModel_bhad_pt( myModelParticles->Parton(0)->Pt() );
        result->setModel_bhad_eta( myModelParticles->Parton(0)->Eta() );
        result->setModel_bhad_phi( myModelParticles->Parton(0)->Phi() );
        result->setModel_bhad_E( myModelParticles->Parton(0)->E() );
        result->setModel_bhad_jetIndex( (*myPermutedParticles)->JetIndex(0) );
        
        result->setModel_blep_pt( myModelParticles->Parton(1)->Pt() );
        result->setModel_blep_eta( myModelParticles->Parton(1)->Eta() );
        result->setModel_blep_phi( myModelParticles->Parton(1)->Phi() );
        result->setModel_blep_E( myModelParticles->Parton(1)->E() );
        result->setModel_blep_jetIndex( (*myPermutedParticles)->JetIndex(1) );

        result->setModel_lq1_pt( myModelParticles->Parton(2)->Pt() );
        result->setModel_lq1_eta( myModelParticles->Parton(2)->Eta() );
        result->setModel_lq1_phi( myModelParticles->Parton(2)->Phi() );
        result->setModel_lq1_E( myModelParticles->Parton(2)->E() );
        result->setModel_lq1_jetIndex( (*myPermutedParticles)->JetIndex(2) );

        result->setModel_lq2_pt( myModelParticles->Parton(3)->Pt() );
        result->setModel_lq2_eta( myModelParticles->Parton(3)->Eta() );
        result->setModel_lq2_phi( myModelParticles->Parton(3)->Phi() );
        result->setModel_lq2_E( myModelParticles->Parton(3)->E() );
        result->setModel_lq2_jetIndex( (*myPermutedParticles)->JetIndex(3) ); 

        if(m_LHType == "ttH"){

	  result->setModel_Higgs_b1_pt( myModelParticles->Parton(4)->Pt() );
	  result->setModel_Higgs_b1_eta( myModelParticles->Parton(4)->Eta() );
	  result->setModel_Higgs_b1_phi( myModelParticles->Parton(4)->Phi() );
	  result->setModel_Higgs_b1_E( myModelParticles->Parton(4)->E() );
	  result->setModel_Higgs_b1_jetIndex( (*myPermutedParticles)->JetIndex(4) );

	  result->setModel_Higgs_b2_pt( myModelParticles->Parton(5)->Pt() );
	  result->setModel_Higgs_b2_eta( myModelParticles->Parton(5)->Eta() );
	  result->setModel_Higgs_b2_phi( myModelParticles->Parton(5)->Phi() );
	  result->setModel_Higgs_b2_E( myModelParticles->Parton(5)->E() );
	  result->setModel_Higgs_b2_jetIndex( (*myPermutedParticles)->JetIndex(5) );

        } 
        
        if (m_leptonTypeKLFitterEnum == KLFitter::LikelihoodTopLeptonJets::LeptonType::kElectron || 
            m_leptonTypeKLFitterEnum_TTH == KLFitter::LikelihoodTTHLeptonJets::LeptonType::kElectron ||
            m_leptonTypeKLFitterEnum_TTZ == KLFitter::LikelihoodTTZTrilepton::LeptonType::kElectron ||
            m_leptonTypeKLFitterEnum_JetAngles == KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kElectron) {
          result->setModel_lep_pt( myModelParticles->Electron(0)->Pt() );
          result->setModel_lep_eta( myModelParticles->Electron(0)->Eta() );
          result->setModel_lep_phi( myModelParticles->Electron(0)->Phi() );
          result->setModel_lep_E( myModelParticles->Electron(0)->E() );

          if (m_leptonType == "kTriElectron") {
            result->setModel_lep_index( (*myPermutedParticles)->ElectronIndex(0) );

            result->setModel_lepZ1_pt( myModelParticles->Electron(1)->Pt() );
            result->setModel_lepZ1_eta( myModelParticles->Electron(1)->Eta() );
            result->setModel_lepZ1_phi( myModelParticles->Electron(1)->Phi() );
            result->setModel_lepZ1_E( myModelParticles->Electron(1)->E() );
            result->setModel_lepZ1_index( (*myPermutedParticles)->ElectronIndex(1) );

            result->setModel_lepZ2_pt( myModelParticles->Electron(2)->Pt() );
            result->setModel_lepZ2_eta( myModelParticles->Electron(2)->Eta() );
            result->setModel_lepZ2_phi( myModelParticles->Electron(2)->Phi() );
            result->setModel_lepZ2_E( myModelParticles->Electron(2)->E() );
            result->setModel_lepZ2_index( (*myPermutedParticles)->ElectronIndex(2) );
          }
        }
        
        if (m_leptonTypeKLFitterEnum == KLFitter::LikelihoodTopLeptonJets::LeptonType::kMuon || 
            m_leptonTypeKLFitterEnum_TTH == KLFitter::LikelihoodTTHLeptonJets::LeptonType::kMuon || 
            m_leptonTypeKLFitterEnum_TTZ == KLFitter::LikelihoodTTZTrilepton::LeptonType::kMuon ||
            m_leptonTypeKLFitterEnum_JetAngles == KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType::kMuon) {
          result->setModel_lep_pt( myModelParticles->Muon(0)->Pt() );
          result->setModel_lep_eta( myModelParticles->Muon(0)->Eta() );
          result->setModel_lep_phi( myModelParticles->Muon(0)->Phi() );
          result->setModel_lep_E( myModelParticles->Muon(0)->E() );

          if (m_leptonType == "kTriMuon") {
            result->setModel_lep_index( (*myPermutedParticles)->MuonIndex(0) );

            result->setModel_lepZ1_pt( myModelParticles->Muon(1)->Pt() );
            result->setModel_lepZ1_eta( myModelParticles->Muon(1)->Eta() );
            result->setModel_lepZ1_phi( myModelParticles->Muon(1)->Phi() );
            result->setModel_lepZ1_E( myModelParticles->Muon(1)->E() );
            result->setModel_lepZ1_index( (*myPermutedParticles)->MuonIndex(1) );

            result->setModel_lepZ2_pt( myModelParticles->Muon(2)->Pt() );
            result->setModel_lepZ2_eta( myModelParticles->Muon(2)->Eta() );
            result->setModel_lepZ2_phi( myModelParticles->Muon(2)->Phi() );
            result->setModel_lepZ2_E( myModelParticles->Muon(2)->E() );
            result->setModel_lepZ2_index( (*myPermutedParticles)->MuonIndex(2) );
          }
        }

        result->setModel_nu_pt( myModelParticles->Neutrino(0)->Pt() );
        result->setModel_nu_eta( myModelParticles->Neutrino(0)->Eta() );
        result->setModel_nu_phi( myModelParticles->Neutrino(0)->Phi() );
        result->setModel_nu_E( myModelParticles->Neutrino(0)->E() );

      } else if (m_LHType == "ttbar_AllHadronic"){
        result->setModel_b_from_top1_pt( myModelParticles->Parton(0)->Pt() );
        result->setModel_b_from_top1_eta( myModelParticles->Parton(0)->Eta() );
        result->setModel_b_from_top1_phi( myModelParticles->Parton(0)->Phi() );
        result->setModel_b_from_top1_E( myModelParticles->Parton(0)->E() );
        result->setModel_b_from_top1_jetIndex( (*myPermutedParticles)->JetIndex(0) );

        result->setModel_b_from_top2_pt( myModelParticles->Parton(1)->Pt() );
        result->setModel_b_from_top2_eta( myModelParticles->Parton(1)->Eta() );
        result->setModel_b_from_top2_phi( myModelParticles->Parton(1)->Phi() );
        result->setModel_b_from_top2_E( myModelParticles->Parton(1)->E() );
        result->setModel_b_from_top2_jetIndex( (*myPermutedParticles)->JetIndex(1) );

        result->setModel_lj1_from_top1_pt( myModelParticles->Parton(2)->Pt() );
        result->setModel_lj1_from_top1_eta( myModelParticles->Parton(2)->Eta() );
        result->setModel_lj1_from_top1_phi( myModelParticles->Parton(2)->Phi() );
        result->setModel_lj1_from_top1_E( myModelParticles->Parton(2)->E() );
        result->setModel_lj1_from_top1_jetIndex( (*myPermutedParticles)->JetIndex(2) );

        result->setModel_lj2_from_top1_pt( myModelParticles->Parton(3)->Pt() );
        result->setModel_lj2_from_top1_eta( myModelParticles->Parton(3)->Eta() );
        result->setModel_lj2_from_top1_phi( myModelParticles->Parton(3)->Phi() );
        result->setModel_lj2_from_top1_E( myModelParticles->Parton(3)->E() );
        result->setModel_lj2_from_top1_jetIndex( (*myPermutedParticles)->JetIndex(3) );

        result->setModel_lj1_from_top2_pt( myModelParticles->Parton(4)->Pt() );
        result->setModel_lj1_from_top2_eta( myModelParticles->Parton(4)->Eta() );
        result->setModel_lj1_from_top2_phi( myModelParticles->Parton(4)->Phi() );
        result->setModel_lj1_from_top2_E( myModelParticles->Parton(4)->E() );
        result->setModel_lj1_from_top2_jetIndex( (*myPermutedParticles)->JetIndex(4) );

        result->setModel_lj2_from_top2_pt( myModelParticles->Parton(5)->Pt() );
        result->setModel_lj2_from_top2_eta( myModelParticles->Parton(5)->Eta() );
        result->setModel_lj2_from_top2_phi( myModelParticles->Parton(5)->Phi() );
        result->setModel_lj2_from_top2_E( myModelParticles->Parton(5)->E() );
        result->setModel_lj2_from_top2_jetIndex( (*myPermutedParticles)->JetIndex(5) );

      }
      
    } // Loop over permutations
    
    // Normalize event probability to unity
    // work out best permutation
    float sumEventProbability(0.),bestEventProbability(0.);
    unsigned int bestPermutation(999),iPerm(0);
    
    // First loop
    for (auto x : *resultContainer) {
      float prob = x->eventProbability();
      sumEventProbability += prob;
      
      if (prob > bestEventProbability) {
        bestEventProbability = prob;
        bestPermutation = iPerm;
      }
      ++iPerm;
    }
    
    // Second loop
    iPerm = 0;
    for (auto x : *resultContainer) {
      x->setEventProbability( x->eventProbability() / sumEventProbability );
      if (iPerm == bestPermutation) {
        x->setBestPermutation(1);
      }
      else{
        x->setBestPermutation(0);
      }
      ++iPerm;
    }
    
    

    // Save to StoreGate / TStore
    std::string outputSGKey("SetMe");
    if (!event.m_isLoose) {
      outputSGKey = m_config->sgKeyKLFitter( event.m_hashValue );
    }
    if (event.m_isLoose) {
      outputSGKey = m_config->sgKeyKLFitterLoose( event.m_hashValue );
    }    
    
    std::string outputSGKeyAux = outputSGKey + "Aux.";
    
    // Save all permutations or only the highest event probability?
    
    // Save all
    if (m_config->KLFitterSaveAllPermutations()) {   
      xAOD::TReturnCode save = evtStore()->tds()->record( resultContainer ,outputSGKey  );
      xAOD::TReturnCode saveAux = evtStore()->tds()->record( resultAuxContainer , outputSGKeyAux );
      if( !save || !saveAux ){
        return StatusCode::FAILURE;
      }    
    }
    
    // Save only the best
    if (!m_config->KLFitterSaveAllPermutations()) {
      // create the xAOD::KLFitterResultContainer
      xAOD::KLFitterResultAuxContainer* bestAuxContainer = new xAOD::KLFitterResultAuxContainer{};
      xAOD::KLFitterResultContainer* bestContainer = new xAOD::KLFitterResultContainer{};
      bestContainer->setStore( bestAuxContainer );      
      
      for (auto x : *resultContainer) {
        if (x->bestPermutation() == 1) {
          xAOD::KLFitterResult* result = new xAOD::KLFitterResult{};
          result->makePrivateStore( *x );
          bestContainer->push_back( result );
        }
      }
      
      xAOD::TReturnCode save = evtStore()->tds()->record( bestContainer ,outputSGKey  );
      xAOD::TReturnCode saveAux = evtStore()->tds()->record( bestAuxContainer , outputSGKeyAux );
      if( !save || !saveAux ){
        return StatusCode::FAILURE;
      }       
      
      // watch out for memory leaks!
      // raw pointers have not been put into a DataVector
      // we still actually own them
      // AnalysisTop will actually do some memory management (which is very wierd and we don't like it) 
      delete resultContainer;
      delete resultAuxContainer;
    }
     
    // Pull the const result back out of StoreGate and attach to top::Event
    top::check(evtStore()->retrieve(event.m_KLFitterResults,outputSGKey),"Failed to add KLFitterResults to top::Event");

    delete myParticles;
   
    /// Return gracefully:
    return StatusCode::SUCCESS;     
  }
  
  bool KLFitterTool::HasTag(const xAOD::Jet& jet, double& weight) const {
    weight = -99.;
    const auto& btag_object = jet.btagging();
    const auto& tagger_name = m_btagging_eff_tool->getTaggerName();
    if (!btag_object || !btag_object->MVx_discriminant(tagger_name, weight)) {
      ATH_MSG_ERROR("Failed to retrieve "+tagger_name+" weight!");
    }

    for(const auto& tagWP : m_config->bTagWP_available()){
      if(!jet.isAvailable<char>("isbtagged_"+tagWP)) {
        ATH_MSG_ERROR("Failed to retrieve jet decoration isbtagged_"+tagWP);
        break;
      }
      return jet.auxdataConst<char>("isbtagged_"+tagWP);
    }
    return false;
  }
  
  
  void KLFitterTool::retrieveEfficiencies(const xAOD::Jet& jet, float* efficiency, float* inefficiency) {
    *efficiency = .7725;        // dummy values
    *inefficiency = 1./125.93;  // dummy values
    
    auto pretend_to_be_b = new xAOD::Jet(jet);
    auto pretend_to_be_light = new xAOD::Jet(jet);

    pretend_to_be_b->setAttribute("HadronConeExclTruthLabelID", 5);
    pretend_to_be_light->setAttribute("HadronConeExclTruthLabelID", 0);

    top::check(m_btagging_eff_tool->getMCEfficiency(*pretend_to_be_b, *efficiency),
               "Could not retrieve tagging efficiency for b-jet");
    top::check(m_btagging_eff_tool->getMCEfficiency(*pretend_to_be_light, *inefficiency),
               "Could not retrieve tagging efficiency for light jet");

    delete pretend_to_be_b;
    delete pretend_to_be_light;

  }



  void KLFitterTool::setJets(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kLeadingFour) {
      setJetskLeadingFour( event , inputParticles );
    }
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kLeadingFive) {
      setJetskLeadingFive( event , inputParticles );
    } 
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kLeadingSix) {
      setJetskLeadingSix( event , inputParticles );
    }
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kLeadingSeven) {
      setJetskLeadingSeven( event , inputParticles );
    }
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kBtagPriorityFourJets) {
      setJetskBtagPriorityFourJets( event , inputParticles );
    }
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kBtagPriorityFiveJets) {
      setJetskBtagPriorityFiveJets( event , inputParticles );
    }     
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kBtagPrioritySixJets) {
      setJetskBtagPrioritySixJets( event , inputParticles );
    }
    if (m_jetSelectionModeKLFitterEnum == top::KLFitterJetSelection::kBtagPrioritySevenJets) {
      setJetskBtagPrioritySevenJets( event , inputParticles );
    }


  }
  
  void KLFitterTool::setJetskLeadingFour(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskLeadingX(event, inputParticles, 4);
  }

  void KLFitterTool::setJetskLeadingFive(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskLeadingX(event, inputParticles, 5);
  }

  void KLFitterTool::setJetskLeadingSix(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskLeadingX(event, inputParticles, 6);
  }

  void KLFitterTool::setJetskLeadingSeven(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskLeadingX(event, inputParticles, 7);
  }

  void KLFitterTool::setJetskLeadingX(const top::Event& event,KLFitter::Particles* inputParticles, int njets)
  {
    int index(0);
    for (const auto& jet : event.m_jets) {
      if (index > njets-1) break;

      TLorentzVector jet_p4;
      jet_p4.SetPtEtaPhiE(jet->pt() / 1.e3, jet->eta(), jet->phi(), jet->e() / 1.e3);

      double weight(-99.);
      const bool isTagged = HasTag(*jet, weight);

      float eff(0), ineff(0);
      retrieveEfficiencies(*jet, &eff, &ineff);

      inputParticles->AddParticle(&jet_p4, jet_p4.Eta(), KLFitter::Particles::kParton, "", index,
                                  isTagged, eff, 1./ineff, KLFitter::Particles::kNone, weight);
      ++index;
    }
  }

  
  void KLFitterTool::setJetskBtagPriorityFourJets(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskBtagPriority( event , inputParticles , 4 );
  }
  
  void KLFitterTool::setJetskBtagPriorityFiveJets(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskBtagPriority( event , inputParticles , 5 );
  }  

  void KLFitterTool::setJetskBtagPrioritySixJets(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskBtagPriority( event , inputParticles , 6 );
  }
  
  void KLFitterTool::setJetskBtagPrioritySevenJets(const top::Event& event,KLFitter::Particles* inputParticles)
  {
    setJetskBtagPriority( event , inputParticles , 7 );
  }
  
  void KLFitterTool::setJetskBtagPriority(const top::Event& event,KLFitter::Particles* inputParticles,const unsigned int maxJets)
  {
    // kBtagPriority mode first adds the b jets, then the light jets                                                                                                                    
    // If your 6th or 7th jet is a b jet, then you probably want this option                                                                                                    

    unsigned int totalJets(0);

    // First find the b-jets                                                                                                                                                           
    unsigned int index(0);
    double weight(0);
    for (const auto& jet : event.m_jets) {
      if (totalJets >= maxJets) break;
      if (HasTag(*jet, weight)) {
	TLorentzVector jet_p4;
        jet_p4.SetPtEtaPhiE(jet->pt() / 1.e3, jet->eta(), jet->phi(), jet->e() / 1.e3);

        float eff(0), ineff(0);
        retrieveEfficiencies(*jet, &eff, &ineff);

	inputParticles->AddParticle(&jet_p4, jet_p4.Eta(), KLFitter::Particles::kParton, "", index,
                                    true, eff, 1./ineff, KLFitter::Particles::kNone, weight);
        ++totalJets;
      }  // HasTag                                                                                                                                                                    
      
      ++index;
    }  // for (jet)                                                                                                                                                                    
  

    // Second, find the light jets                                                                                                                                                     
    index = 0;
    for (const auto& jet : event.m_jets) {
      if (totalJets >= maxJets) break;
      if (!HasTag(*jet, weight)) {
        TLorentzVector jet_p4;
        jet_p4.SetPtEtaPhiE(jet->pt() / 1.e3, jet->eta(), jet->phi(), jet->e() / 1.e3);

        float eff(0), ineff(0);
        retrieveEfficiencies(*jet, &eff, &ineff);

        inputParticles->AddParticle(&jet_p4, jet_p4.Eta(), KLFitter::Particles::kParton, "", index,
                                    false, eff, 1./ineff, KLFitter::Particles::kNone, weight);
        ++totalJets;
      }  // !HasTag                                                                                                                                                                    
      
      ++index;
    }  // for (jet)                                                                                                                                                                   
    
  }

     
  /// Function finalizing the tool
  StatusCode KLFitterTool::finalize()
  {
    /// Return gracefully:
    return StatusCode::SUCCESS;    
  }
}
