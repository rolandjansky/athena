/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonObjectCollectionMaker.cxx 810751 2017-09-29 14:41:39Z iconnell $
#include "TopSystematicObjectMaker/MuonObjectCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "AthContainers/AuxElement.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "PATInterfaces/SystematicsUtil.h"

namespace top{

  MuonObjectCollectionMaker::MuonObjectCollectionMaker( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),

    m_specifiedSystematics(),
    m_recommendedSystematics(),

    m_calibrationPeriodTool("CP::MuonCalibrationPeriodTool"),
    m_isolationTool_LooseTrackOnly("CP::IsolationTool_LooseTrackOnly"),
    m_isolationTool_Loose("CP::IsolationTool_Loose"),
    m_isolationTool_Gradient("CP::IsolationTool_Gradient"),
    m_isolationTool_GradientLoose("CP::IsolationTool_GradientLoose"),    
    m_isolationTool_FixedCutTight("CP::IsolationTool_FixedCutTight"),
    m_isolationTool_FixedCutTightTrackOnly("CP::IsolationTool_FixedCutTightTrackOnly"),
    m_isolationTool_FixedCutLoose("CP::IsolationTool_FixedCutLoose"),
    m_isolationTool_FCTight("CP::IsolationTool_FCTight"),
    m_isolationTool_FCLoose("CP::IsolationTool_FCLoose"),
    m_isolationTool_FCTightTrackOnly("CP::IsolationTool_FCTightTrackOnly"),
    m_muonSelectionToolVeryLooseVeto("CP::MuonSelectionToolVeryLooseVeto")
  {
    declareProperty( "config" , m_config );  
    
    declareProperty( "MuonCalibrationPeriodTool" ,       m_calibrationPeriodTool );
    declareProperty( "IsolationTool_LooseTrackOnly" ,         m_isolationTool_LooseTrackOnly );
    declareProperty( "IsolationTool_Loose" ,                  m_isolationTool_Loose );
    declareProperty( "IsolationTool_Gradient" ,               m_isolationTool_Gradient );
    declareProperty( "IsolationTool_GradientLoose" ,          m_isolationTool_GradientLoose );
    declareProperty( "IsolationTool_FixedCutTight" ,          m_isolationTool_FixedCutTight );
    declareProperty( "IsolationTool_FixedCutTightTrackOnly" , m_isolationTool_FixedCutTightTrackOnly );
    declareProperty( "IsolationTool_FixedCutLoose" ,          m_isolationTool_FixedCutLoose );
    declareProperty( "IsolationTool_FCTight" ,                m_isolationTool_FCTight );
    declareProperty( "IsolationTool_FCLoose" ,                m_isolationTool_FCLoose );
    declareProperty( "IsolationTool_FCTightTrackOnly" ,       m_isolationTool_FCTightTrackOnly );
    declareProperty( "MuonSelectionToolVeryLooseVeto" ,       m_muonSelectionToolVeryLooseVeto );
  }

  StatusCode MuonObjectCollectionMaker::initialize()
  {
    ATH_MSG_INFO(" top::MuonObjectCollectionMaker initialize" );  
    
    top::check( m_calibrationPeriodTool.retrieve()     , "Failed to retrieve muon calibration tool" );
    top::check( m_isolationTool_LooseTrackOnly.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_Loose.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_Gradient.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_GradientLoose.retrieve() , "Failed to retrieve Isolation Tool" );    
    top::check( m_isolationTool_FixedCutTight.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_FixedCutTightTrackOnly.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_FixedCutLoose.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_FCTight.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_FCLoose.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_isolationTool_FCTightTrackOnly.retrieve() , "Failed to retrieve Isolation Tool" );
    top::check( m_muonSelectionToolVeryLooseVeto.retrieve() , "Failed to retrieve Selection Tool" );

    ///-- Set Systematics Information --///
    const std:: string& syststr = m_config->systematics();
    std::set<std::string> syst;  

    if( !m_config->isSystNominal(syststr) && !m_config->isSystAll(syststr)){
      bool ok = m_config->getSystematicsList(syststr, syst);
      if (!ok) {
	ATH_MSG_ERROR(" top::MuonObjectCollectionMaker could not determine systematic list" );
	return StatusCode::FAILURE;
      }
      if (m_config->contains(syst, "AllMuons")) {
	syst.clear();
      }
    }
 
    specifiedSystematics( syst );
    
    m_config->systematicsMuons( specifiedSystematics() );
    
    ATH_MSG_INFO(" top::MuonObjectCollectionMaker completed initialize" );
    return StatusCode::SUCCESS;
  }
    
  StatusCode MuonObjectCollectionMaker::execute(bool executeNominal)
  {

    static SG::AuxElement::ConstAccessor<float> ptvarcone30_TightTTVA_pt1000("ptvarcone30_TightTTVA_pt1000");
    static SG::AuxElement::Accessor<char> AnalysisTop_Isol_FCTight("AnalysisTop_Isol_FCTight");
    static SG::AuxElement::Accessor<char> AnalysisTop_Isol_FCLoose("AnalysisTop_Isol_FCLoose");
    static SG::AuxElement::Accessor<char> AnalysisTop_Isol_FCTightTrackOnly("AnalysisTop_Isol_FCTightTrackOnly");

    const xAOD::EventInfo* eventInfo(nullptr);
    top::check( evtStore()->retrieve( eventInfo, m_config->sgKeyEventInfo() ), "Failed to retrieve EventInfo");
    float beam_pos_sigma_x = eventInfo->beamPosSigmaX();
    float beam_pos_sigma_y = eventInfo->beamPosSigmaY();
    float beam_pos_sigma_xy = eventInfo->beamPosSigmaXY();

    ///--- Need to know the year for each event until MCP have consistent recommendations for all years ---///
    unsigned int runnumber = -999999;
    if(m_config->isMC()){
      top::check(eventInfo->isAvailable<unsigned int>("RandomRunNumber"), "Require that RandomRunNumber decoration is available.");
      runnumber = eventInfo->auxdataConst<unsigned int>("RandomRunNumber");
    }
    else{
      runnumber = eventInfo->runNumber();
    }
    const std::string thisYear = m_config->getYear(runnumber);

    ///-- Get base muons and tracks from xAOD --///
    const xAOD::MuonContainer* xaod(nullptr);
    top::check( evtStore()->retrieve( xaod , m_config->sgKeyMuons() ) , "Failed to retrieve Muons" );
    
    ///-- Loop over all systematics --///
    for( auto systematic : m_specifiedSystematics ){

      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if(executeNominal && !m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;
      if(!executeNominal && m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;
        
      ///-- Tell tool which systematic to use --///
	top::check( m_calibrationPeriodTool->applySystematicVariation( systematic ) , "Failed to applySystematicVariation" );
      
      ///-- Shallow copy of the xAOD --///
      std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *xaod );
      
      ///-- Loop over the xAOD Container and apply corrections--///
      for( auto muon : *(shallow_xaod_copy.first) ){
          
	///-- Apply momentum correction --///
        if (muon->primaryTrackParticle()) {
	    top::check( m_calibrationPeriodTool->applyCorrection( *muon ) , "Failed to applyCorrection" );

          // don't do the decorations unless the muons are at least Loose
          // this is because it may fail if the muons are at just VeryLoose
          if (m_muonSelectionToolVeryLooseVeto->accept(*muon)) {
            double d0sig = xAOD::TrackingHelpers::d0significance( muon->primaryTrackParticle(),
                                                                  beam_pos_sigma_x,
                                                                  beam_pos_sigma_y,
                                                                  beam_pos_sigma_xy );
            muon->auxdecor<float>("d0sig") = d0sig;

            if( eventInfo->isAvailable<float>("AnalysisTop_PRIVTX_z_position") ){
              float vtx_z = eventInfo->auxdata<float>("AnalysisTop_PRIVTX_z_position");
              float delta_z0 = muon->primaryTrackParticle()->z0() + muon->primaryTrackParticle()->vz() - vtx_z;
              muon->auxdecor<float>("delta_z0") = delta_z0;
              muon->auxdecor<float>("delta_z0_sintheta") = delta_z0*std::sin( muon->primaryTrackParticle()->theta() );
            }
          }
	}
	
        ///-- Isolation selection --///
        char passIsol_LooseTrackOnly(0),passIsol_Loose(0),passIsol_FixedCutTightTrackOnly(0);
        char passIsol_Gradient(0),passIsol_GradientLoose(0),passIsol_FixedCutLoose(0);
        char passIsol_FixedCutTight(0);
        if (m_isolationTool_LooseTrackOnly->accept( *muon )) {passIsol_LooseTrackOnly = 1;}
        if (m_isolationTool_Loose->accept( *muon )) {passIsol_Loose = 1;}
        if (m_isolationTool_Gradient->accept( *muon )) {passIsol_Gradient = 1;}
        if (m_isolationTool_GradientLoose->accept( *muon )) {passIsol_GradientLoose = 1;}
	if (m_isolationTool_FixedCutTight->accept( *muon )) {passIsol_FixedCutTight = 1;}
	if (m_isolationTool_FixedCutTightTrackOnly->accept( *muon )) {passIsol_FixedCutTightTrackOnly = 1;}
	if (m_isolationTool_FixedCutLoose->accept( *muon )) {passIsol_FixedCutLoose = 1;}

        muon->auxdecor<char>("AnalysisTop_Isol_LooseTrackOnly") = passIsol_LooseTrackOnly;
        muon->auxdecor<char>("AnalysisTop_Isol_Loose") = passIsol_Loose;
        muon->auxdecor<char>("AnalysisTop_Isol_Gradient") = passIsol_Gradient;
        muon->auxdecor<char>("AnalysisTop_Isol_GradientLoose") = passIsol_GradientLoose; 	
	muon->auxdecor<char>("AnalysisTop_Isol_FixedCutTight") = passIsol_FixedCutTight;
	muon->auxdecor<char>("AnalysisTop_Isol_FixedCutTightTrackOnly") = passIsol_FixedCutTightTrackOnly;
	muon->auxdecor<char>("AnalysisTop_Isol_FixedCutLoose") = passIsol_FixedCutLoose;

        if (ptvarcone30_TightTTVA_pt1000.isAvailable(*muon)) {
          AnalysisTop_Isol_FCTight(*muon) = (m_isolationTool_FCTight->accept(*muon) ? 1 : 0);
          AnalysisTop_Isol_FCLoose(*muon) = (m_isolationTool_FCLoose->accept(*muon) ? 1 : 0);
          AnalysisTop_Isol_FCTightTrackOnly(*muon) = (m_isolationTool_FCTightTrackOnly->accept(*muon) ? 1 : 0);
        }

	// PromptLeptonIsolation - Some protection incase things change in R21
	if(muon->isAvailable<float>("PromptLeptonIso_TagWeight")){
	  muon->auxdecor<char>("AnalysisTop_Isol_PromptLepton") = (muon->auxdata<float>("PromptLeptonIso_TagWeight") < -0.5) ? 1 : 0;
	}
	else{
	  muon->auxdecor<char>("AnalysisTop_Isol_PromptLepton") = 0;
	}

      }

      ///-- set links to original objects- needed for MET calculation --///
      bool setLinks = xAOD::setOriginalObjectLink( *xaod, *shallow_xaod_copy.first );
      if (!setLinks)
	ATH_MSG_ERROR(" Cannot set original object links for muons, MET recalculation may struggle" );
      
      ///-- Save corrected xAOD Container to StoreGate / TStore --///
      std::string outputSGKey = m_config->sgKeyMuons( systematic.hash() );
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      xAOD::TReturnCode save = evtStore()->tds()->record( shallow_xaod_copy.first , outputSGKey );
      xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_xaod_copy.second , outputSGKeyAux );
      if( !save || !saveAux ){
        return StatusCode::FAILURE;
      }      
    }  // Loop over all systematics    
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode MuonObjectCollectionMaker::printout()
  {
    ///-- Loop over all systematics --///
    for( auto systematic : m_specifiedSystematics ){
      const xAOD::MuonContainer* xaod(nullptr);
      top::check( evtStore()->retrieve( xaod , m_config->sgKeyMuons( systematic.hash() ) ) , "Failed to retrieve Muons" );   
      
      ATH_MSG_INFO(" Muons with sgKey = "<< m_config->sgKeyMuons( systematic.hash() ) );
      for( auto muon : *xaod ){
        ATH_MSG_INFO("   MU pT , eta  = "<< muon->pt() <<" , "<< muon->eta() );
      }
    }
    
    return StatusCode::SUCCESS;    
  }
    
  
  void MuonObjectCollectionMaker::specifiedSystematics( const std::set<std::string>& specifiedSystematics )
  {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector( m_calibrationPeriodTool->recommendedSystematics() );
    
    for (auto s : systList) {
      m_recommendedSystematics.push_back(s);
      if (s.name() == "") {
        m_specifiedSystematics.push_back(s);
      }
      
      ///-- MC only --///
      if (m_config->isMC()) {
        ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
        if (!m_config->isSystNominal( m_config->systematics() )) {
          if (specifiedSystematics.size() == 0) {
            m_specifiedSystematics.push_back(s);
          }
          if (specifiedSystematics.size()  > 0) {
            for (auto i : specifiedSystematics) {
              if ( i == s.name() ) {
                m_specifiedSystematics.push_back(s);              
              }
            }
          }
        }
      }
    }
    m_recommendedSystematics.sort();    
    m_recommendedSystematics.unique();
    m_specifiedSystematics.sort();
    m_specifiedSystematics.unique();      
  }
  
}
