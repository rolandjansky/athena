/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedWZTagger.h"

#include <stdexcept>

SmoothedWZTagger::SmoothedWZTagger( const std::string& name ) :
  JSSTaggerBase( name )
{

  /// Functional forms for cuts
  declareProperty( "MassCutLowFunc",  m_strMassCutLow = "",  "Lower mass cut");
  declareProperty( "MassCutHighFunc", m_strMassCutHigh = "", "Higher mass cut");
  declareProperty( "D2CutFunc",       m_strD2Cut = "",       "Upper cut on D2");
  declareProperty( "NtrkCutFunc",     m_strNtrkCut = "",     "Upper cut on Ntrk");

}

StatusCode SmoothedWZTagger::initialize() {

  ATH_MSG_INFO( "Initializing SmoothedWZTagger tool" );

  /// Pt values are defined in GeV
  m_ptGeV = true;

  /// Use mass cut
  m_useMassCut = true;

  if ( ! m_configFile.empty() ) {

    /// Get configReader
    ATH_CHECK( getConfigReader() );

    if ( m_wkpt.empty() ) {
      m_strMassCutLow = m_configReader.GetValue("MassCutLow", "");
      m_strMassCutHigh = m_configReader.GetValue("MassCutHigh", "");
      m_strD2Cut = m_configReader.GetValue("D2Cut", "");
      m_strNtrkCut = m_configReader.GetValue("NtrkCut", "");
    }
    else {
      m_strMassCutLow = m_configReader.GetValue((m_wkpt+".MassCutLow").c_str(), "");
      m_strMassCutHigh = m_configReader.GetValue((m_wkpt+".MassCutHigh").c_str(), "");
      m_strD2Cut = m_configReader.GetValue((m_wkpt+".D2Cut").c_str(), "");
      m_strNtrkCut = m_configReader.GetValue((m_wkpt+".NtrkCut").c_str(), "");
    }

    /// Get min and max jet pt
    m_jetPtMin = m_configReader.GetValue("pTCutLow", 200.0);
    m_jetPtMax = m_configReader.GetValue("pTCutHigh", 4000.0);

    /// Get the decoration name
    m_decorationName = m_configReader.GetValue("DecorationName", "");

    /// Get the scale factor configuration
    m_calcSF = m_configReader.GetValue("CalcSF", false);
    if ( m_calcSF ) {
      m_weightdecorationName = m_configReader.GetValue("WeightDecorationName", "");
      m_weightFileName = m_configReader.GetValue("WeightFile", "");
      m_weightHistogramName = m_configReader.GetValue("WeightHistogramName", "");
      m_efficiencyHistogramName = m_configReader.GetValue("EfficiencyHistogramName", "");
      m_weightFlavors = m_configReader.GetValue("WeightFlavors", "");
    
      /// Get truth label name information
      m_truthLabelName = m_configReader.GetValue("TruthLabelName", "R10TruthLabel_R21Consolidated");

      if ( m_calibArea.compare("Local") == 0 ) {
        m_weightConfigPath = PathResolverFindCalibFile(("$WorkDir_DIR/data/BoostedJetTaggers/SmoothedWZTaggers/"+m_weightFileName).c_str());      
      }
      else if ( m_calibArea.find("eos") != std::string::npos ) {
        m_weightConfigPath = PathResolverFindCalibFile((m_calibArea+"/"+m_weightFileName).c_str());
      }
      else {
        m_weightConfigPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibArea+"/"+m_weightFileName).c_str());
      }
    }
    
  }
  else { /// No config file
    /// Assume the cut functions have been set through properties.
    /// Check they are non empty
    if( m_strD2Cut.empty() || m_strMassCutLow.empty() || m_strMassCutHigh.empty() ||
        ((m_weightdecorationName.empty() ||
          m_weightHistogramName.empty() ||
	  m_weightFlavors.empty()) && m_calcSF) ) {
      ATH_MSG_ERROR( "No config file provided AND no parameters specified." ) ;
      return StatusCode::FAILURE;
    }
  }

  /// Set flag to indicate if Ntrk cut is used
  m_useNtrk = !m_strNtrkCut.empty();

  /// Transform these strings into functions
  m_funcD2Cut = std::make_unique<TF1>("strD2Cut", m_strD2Cut.c_str(), 0, 14000);
  if ( m_useNtrk ) m_funcNtrkCut = std::make_unique<TF1>("strNtrkCut", m_strNtrkCut.c_str(), 0, 14000);

  ATH_MSG_INFO( "Smoothed WZ Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low      : " << m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High     : " << m_strMassCutHigh );
  ATH_MSG_INFO( "  D2 cut low        : " << m_strD2Cut );
  if ( m_useNtrk )
    ATH_MSG_INFO( "  Ntrk cut low      : " << m_strNtrkCut );
  ATH_MSG_INFO( "  Decorate          : " << m_decorate );
  ATH_MSG_INFO( "  DecorationName    : " << m_decorationName );
  if ( m_calcSF ) {
    ATH_MSG_INFO( "weightdecorationName    : " << m_weightdecorationName );
    ATH_MSG_INFO( "weightFile              : " << m_weightFileName );
    ATH_MSG_INFO( "weightHistogramName     : " << m_weightHistogramName );
    ATH_MSG_INFO( "efficiencyHistogramName : " << m_efficiencyHistogramName );
    ATH_MSG_INFO( "weightFlavors           : " << m_weightFlavors );
    ATH_MSG_INFO( "TruthLabelName          : " << m_truthLabelName );
  }
  ATH_MSG_INFO( "  Pt cut low        : " << m_jetPtMin );
  ATH_MSG_INFO( "  Pt cut high       : " << m_jetPtMax );

  /// Set the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "PassMassLow", "mJet > mCutLow" );
  m_accept.addCut( "PassMassHigh", "mJet < mCutHigh" );
  m_accept.addCut( "PassD2", "D2Jet < D2Cut" );
  if ( m_useNtrk ) {
    m_accept.addCut( "PassNtrk", "NtrkJet < NtrkCut" );
  }

  /// Loop over and print out the cuts that have been configured
  printCuts();

  /// Call base class initialize
  ATH_CHECK( JSSTaggerBase::initialize() );

  /// Initialize additional decorators
  ATH_MSG_INFO( "Additional decorators that will be attached to jet :" );

  ATH_MSG_INFO( "  " << m_decorationName << "_Cut_D2 : D2 cut" );
  
  m_dec_d2cut = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_D2" );

  if ( m_useNtrk ) {
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_Ntrk : Ntrk cut" );
    m_dec_ntrkcut = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_Ntrk" );
  }
  
  return StatusCode::SUCCESS;

}

Root::TAccept& SmoothedWZTagger::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining Smooth WZ result" );

  /// Reset the TAccept cut results
  resetCuts();

  /// Check basic kinematic selection
  checkKinRange(jet);

  /// Get the relevant attributes of the jet
  /// Mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  /// Initialize d2 to 0.  This probably gets used when the jet has one constituent, so it will fail the mass cut anyways
  float jet_d2 = 0;

  /// Calculate NSubjettiness and ECF ratios
  calculateJSSRatios(jet);

  /// Get D2 value
  jet_d2 = acc_D2(jet);

  /// Evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_d2        = m_funcD2Cut      ->Eval(jet_pt);

  /// Decorate the cut value if needed;
  if ( m_decorate ) {
    (*m_dec_d2cut)(jet) = cut_d2;
    (*m_dec_mcutH)(jet) = cut_mass_high;
    (*m_dec_mcutL)(jet) = cut_mass_low;
  }

  /// Evaluate the cut criteria on mass and d2
  ATH_MSG_VERBOSE( "Cut Values : MassWindow = [" << cut_mass_low << "," << cut_mass_high << "], D2Cut = " << cut_d2 );
  ATH_MSG_VERBOSE( "Cut Values : JetMass = " << jet_mass << ", D2 = " << jet_d2 );

  if ( jet_mass >= cut_mass_low ) m_accept.setCutResult( "PassMassLow", true );

  if ( jet_mass <= cut_mass_high ) m_accept.setCutResult( "PassMassHigh", true );

  if ( jet_d2 < cut_d2 ) m_accept.setCutResult( "PassD2", true );

  /// Check if it's a smooth three-variable tagger (ntrk)
  if ( m_useNtrk ) {

    float cut_ntrk = m_funcNtrkCut->Eval(jet_pt);

    if ( m_decorate ) (*m_dec_ntrkcut)(jet) = cut_ntrk;

    /// Get the primary vertex
    bool validVtx = false;
    const xAOD::Vertex* primaryVertex = 0;

    const xAOD::VertexContainer* vxCont = 0;
    if ( evtStore()->retrieve( vxCont, "PrimaryVertices" ).isFailure() ) {
      ATH_MSG_WARNING( "Unable to retrieve primary vertex container PrimaryVertices" );
      validVtx = false;
    }
    else {
      for ( const auto& vx : *vxCont ) {
        if ( vx->vertexType()==xAOD::VxType::PriVtx ) {
          primaryVertex = vx;
          break;
        }
      }

      if ( primaryVertex ) validVtx = true;
    
    }

    if ( validVtx ) {
      const xAOD::Jet * ungroomedJet = 0;

      if ( acc_parent.isAvailable(jet) ) {
        ElementLink<xAOD::JetContainer> linkToUngroomed = acc_parent(jet);
        if (  linkToUngroomed.isValid() ) {
          ungroomedJet = *linkToUngroomed;
          if ( acc_NumTrkPt500.isAvailable(*ungroomedJet) ) {

            const std::vector<int> NTrkPt500 = acc_NumTrkPt500(*ungroomedJet);

            int jet_ntrk = NTrkPt500.at(primaryVertex->index());
            jet.auxdecor<int>("ParentJetNTrkPt500") = jet_ntrk;

            if(jet_ntrk < cut_ntrk) m_accept.setCutResult( "PassNtrk", true );
          
          }
          else {
            m_accept.setCutResult( "ValidJetContent", false );
            /// Note: throwing an exception here because we can't return StatusCode::FAILURE or similar
            /// This error message should only occur if analyses are not using smart slimming in their derivations
            throw std::runtime_error("ERROR: Unable to retrieve Ntrk of the ungroomed parent jet. Please make sure this variable is in your derivations!!!");
          }
        }
        else {
          m_accept.setCutResult( "ValidJetContent", false );
          /// Note: throwing an exception here because we can't return StatusCode::FAILURE or similar
          /// This error message should only occur if analyses are not using smart slimming in their derivations
          throw std::runtime_error("ERROR: Unable to retrieve the parent ungroomed jet. Please make sure this variable is in your derivations!!!");
        }
      }
      else {
        m_accept.setCutResult( "ValidJetContent", false );
        /// Note: throwing an exception here because we can't return StatusCode::FAILURE or similar
        /// This error message should only occur if analyses are not using smart slimming in their derivations
        throw std::runtime_error("ERROR: Unable to retrieve the link to the parent ungroomed jet. Please make sure this variable is in your derivations!!!");
      }
    }
    else {
      m_accept.setCutResult( "ValidEventContent", false );
    }
 
  }

  /// Get enum to decorate m_accept state if only using 2-var tagger
  TagResult::TypeEnum myCutResultForSF = TagResult::UNKNOWN;
  if ( !m_useNtrk ) {
    /// Pass mass cut
    if ( m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh") ) {
      if ( m_accept.getCutResult("PassD2") ) {
        myCutResultForSF = TagResult::passMpassD2_2Var;
      }
      else {
        myCutResultForSF = TagResult::passMfailD2_2Var;
      }
    }
    /// Fail mass cut
    else {
      if ( m_accept.getCutResult("PassD2") ) {
        myCutResultForSF = TagResult::failMpassD2_2Var;
      }
      else {
        myCutResultForSF = TagResult::failMfailD2_2Var;
      }
    }
  }
  else{
    if ( m_accept ) myCutResultForSF = TagResult::passAll;
    else myCutResultForSF = TagResult::fail;
  }

  /// Decorate truth label for SF provider
  if ( m_calcSF && (!m_acc_truthLabel->isAvailable(jet) || LargeRJetTruthLabel::intToEnum((*m_acc_truthLabel)(jet))==LargeRJetTruthLabel::UNKNOWN) ) {
    if ( m_isMC ) {
      if ( m_JetTruthLabelingTool->modifyJet(jet) == StatusCode::FAILURE ) {
        ATH_MSG_FATAL( "Failed to decorate jet truth label. Please check truth container names" );
      }
    }
  }

  /// Get SF weight
  getWeight( jet, m_accept );

  if ( m_decorate ) {
    if ( m_calcSF ) { 
      (*m_dec_accept)(jet)     = myCutResultForSF;
    }
  }

  /// Return the TAccept to be queried later
  return m_accept;

}

