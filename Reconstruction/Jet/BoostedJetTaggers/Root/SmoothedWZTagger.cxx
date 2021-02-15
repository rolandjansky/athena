/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedWZTagger.h"

#include "xAODTracking/VertexContainer.h"

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
      m_weightDecorationName = m_configReader.GetValue("WeightDecorationName", "");
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
        ((m_weightDecorationName.empty() ||
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
  ATH_MSG_INFO( "  DecorationName    : " << m_decorationName );
  if ( m_calcSF ) {
    ATH_MSG_INFO( "weightDecorationName    : " << m_weightDecorationName );
    ATH_MSG_INFO( "weightFile              : " << m_weightFileName );
    ATH_MSG_INFO( "weightHistogramName     : " << m_weightHistogramName );
    ATH_MSG_INFO( "efficiencyHistogramName : " << m_efficiencyHistogramName );
    ATH_MSG_INFO( "weightFlavors           : " << m_weightFlavors );
    ATH_MSG_INFO( "TruthLabelName          : " << m_truthLabelName );
  }
  ATH_MSG_INFO( "  Pt cut low        : " << m_jetPtMin );
  ATH_MSG_INFO( "  Pt cut high       : " << m_jetPtMax );

  /// Set the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_acceptInfo.addCut( "PassMassLow", "mJet > mCutLow" );
  m_acceptInfo.addCut( "PassMassHigh", "mJet < mCutHigh" );
  m_acceptInfo.addCut( "PassD2", "D2Jet < D2Cut" );
  if ( m_useNtrk ) {
    m_acceptInfo.addCut( "PassNtrk", "NtrkJet < NtrkCut" );
  }

  /// Loop over and print out the cuts that have been configured
  printCuts();

  /// Call base class initialize
  ATH_CHECK( JSSTaggerBase::initialize() );

  /// Initialize additional decorators
  ATH_MSG_INFO( "Additional decorators that will be attached to jet :" );

  m_decPassD2Key = m_containerName + "." + m_decorationName + "_" + m_decPassD2Key.key();
  m_decCutD2Key = m_containerName + "." + m_decorationName + "_" + m_decCutD2Key.key();
  
  ATH_CHECK( m_decPassD2Key.initialize() );
  ATH_CHECK( m_decCutD2Key.initialize() );
  
  ATH_MSG_INFO( "  " << m_decPassD2Key.key() << " : pass D2 cut" );
  ATH_MSG_INFO( "  " << m_decCutD2Key.key() << " : D2 cut" );

  if ( m_useNtrk ) {
  
    m_decPassNtrkKey = m_containerName + "." + m_decorationName + "_" + m_decPassNtrkKey.key();
    m_decCutNtrkKey = m_containerName + "." + m_decorationName + "_" + m_decCutNtrkKey.key();
    
    ATH_CHECK( m_decPassNtrkKey.initialize() );
    ATH_CHECK( m_decCutNtrkKey.initialize() );
    
    ATH_MSG_INFO( "  " << m_decPassNtrkKey.key() << " : pass Ntrk cut" );
    ATH_MSG_INFO( "  " << m_decCutNtrkKey.key() << " : Ntrk cut" );

  }

  if ( m_calcSF ) {
    
    m_decAcceptKey = m_containerName + "." + m_decorationName + "_" + m_decAcceptKey.key();
    ATH_CHECK( m_decAcceptKey.initialize() );
  
  }

  return StatusCode::SUCCESS;

}

StatusCode SmoothedWZTagger::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining Smooth WZ result" );

  /// Create asg::AcceptData object
  asg::AcceptData acceptData( &m_acceptInfo );

  /// Reset the AcceptData cut results
  ATH_CHECK( resetCuts( acceptData ) );

  /// Check basic kinematic selection
  ATH_CHECK( checkKinRange( jet, acceptData ) );

  /// Get the relevant attributes of the jet
  /// Mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  /// Initialize d2 to 0.  This probably gets used when the jet has one constituent, so it will fail the mass cut anyways
  float jet_d2 = 0;

  /// Calculate NSubjettiness and ECF ratios
  calculateJSSRatios(jet);

  /// Create D2 read decor handle
  SG::ReadDecorHandle<xAOD::JetContainer, float> readD2(m_readD2Key);

  /// Get D2 value
  jet_d2 = readD2(jet);

  /// Evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_d2        = m_funcD2Cut      ->Eval(jet_pt);

  /// Decorate the cut values

  /// Create WriteDecorHandles
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decPassMass(m_decPassMassKey);
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decPassD2(m_decPassD2Key);
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decTagged(m_decTaggedKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decCutMLow(m_decCutMLowKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decCutMHigh(m_decCutMHighKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decCutD2(m_decCutD2Key);

  /// Decorate values
  decCutMLow(jet) = cut_mass_low;
  decCutMHigh(jet) = cut_mass_high;
  decCutD2(jet) = cut_d2;

  /// Evaluate the cut criteria on mass and d2
  ATH_MSG_VERBOSE( "Cut Values : MassWindow = [" << cut_mass_low << "," << cut_mass_high << "], D2Cut = " << cut_d2 );
  ATH_MSG_VERBOSE( "Cut Values : JetMass = " << jet_mass << ", D2 = " << jet_d2 );

  if ( jet_mass >= cut_mass_low ) acceptData.setCutResult( "PassMassLow", true );

  if ( jet_mass <= cut_mass_high ) acceptData.setCutResult( "PassMassHigh", true );

  if ( jet_d2 < cut_d2 ) acceptData.setCutResult( "PassD2", true );

  decPassMass(jet) = acceptData.getCutResult( "PassMassLow" ) && acceptData.getCutResult( "PassMassHigh" );
  decPassD2(jet) = acceptData.getCutResult( "PassD2" );

  bool passCuts = acceptData.getCutResult( "PassMassLow" ) && acceptData.getCutResult( "PassMassHigh" );
  passCuts = passCuts && acceptData.getCutResult( "PassD2" );

  /// Check if it's a smooth three-variable tagger (ntrk)
  if ( m_useNtrk ) {

    float cut_ntrk = m_funcNtrkCut->Eval(jet_pt);

    /// Decorate Ntrk cut value

    /// Create WriteDecorHandles
    SG::WriteDecorHandle<xAOD::JetContainer, bool> decValidJetContent(m_decValidJetContentKey);
    SG::WriteDecorHandle<xAOD::JetContainer, bool> decValidEventContent(m_decValidEventContentKey);
    SG::WriteDecorHandle<xAOD::JetContainer, bool> decPassNtrk(m_decPassNtrkKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> decCutNtrk(m_decCutNtrkKey);

    /// Decorate values
    decCutNtrk(jet) = cut_ntrk;

    /// Get the primary vertex
    bool validVtx = false;
    const xAOD::Vertex* primaryVertex = 0;

    const xAOD::VertexContainer* vxCont = 0;
    if ( evtStore()->retrieve( vxCont, "PrimaryVertices" ).isFailure() ) {
      ATH_MSG_WARNING( "Unable to retrieve primary vertex container PrimaryVertices" );
      validVtx = false;
    }
    else {
      for ( const auto *vx : *vxCont ) {
        if ( vx->vertexType()==xAOD::VxType::PriVtx ) {
          primaryVertex = vx;
          break;
        }
      }

      if ( primaryVertex ) validVtx = true;
    
    }

    if ( validVtx ) {
      static SG::AuxElement::Accessor<ElementLink<xAOD::JetContainer> > ungroomedLink("Parent");
      const xAOD::Jet * ungroomedJet = 0;

      if ( ungroomedLink.isAvailable(jet) ) {
        ElementLink<xAOD::JetContainer> linkToUngroomed = ungroomedLink(jet);
        if ( linkToUngroomed.isValid() ) {
          ungroomedJet = *linkToUngroomed;

          static SG::AuxElement::ConstAccessor< std::vector<int> >acc_Ntrk("NumTrkPt500");

          if ( acc_Ntrk.isAvailable(*ungroomedJet) ) {

            const std::vector<int> NTrkPt500 = acc_Ntrk(*ungroomedJet);

            int jet_ntrk = NTrkPt500.at(primaryVertex->index());
            jet.auxdecor<int>("ParentJetNTrkPt500") = jet_ntrk;

            if ( jet_ntrk < cut_ntrk ) acceptData.setCutResult( "PassNtrk", true );
            decPassNtrk(jet) = acceptData.getCutResult( "PassNtrk" );
            passCuts = passCuts && acceptData.getCutResult( "PassNtrk" );
          
          }
          else {
            acceptData.setCutResult( "ValidJetContent", false );
            decValidJetContent(jet) = false;
            ATH_MSG_ERROR( "Unable to retrieve Ntrk of the ungroomed parent jet. Please make sure this variable is in your derivations!!!" );
            return StatusCode::FAILURE;
          }
        }
        else {
          acceptData.setCutResult( "ValidJetContent", false );
          decValidJetContent(jet) = false;
          ATH_MSG_ERROR( "Unable to retrieve the parent ungroomed jet. Please make sure this variable is in your derivations!!!" );
          return StatusCode::FAILURE;
        }
      }
      else {
        acceptData.setCutResult( "ValidJetContent", false );
        decValidJetContent(jet) = false;
        ATH_MSG_ERROR( "Unable to retrieve the link to the parent ungroomed jet. Please make sure this variable is in your derivations!!!" );
        return StatusCode::FAILURE;
      }
    }
    else {
      acceptData.setCutResult( "ValidEventContent", false );
    }

    decValidJetContent(jet) = acceptData.getCutResult( "ValidJetContent" );
    decValidEventContent(jet) = acceptData.getCutResult( "ValidEventContent" );

  }

  /// Decorate jet with tagging summary
  decTagged(jet) = passCuts;

  /// Get enum to decorate acceptData state if only using 2-var tagger
  TagResult::TypeEnum myCutResultForSF = TagResult::UNKNOWN;
  if ( !m_useNtrk ) {
    /// Pass mass cut
    if ( acceptData.getCutResult("PassMassLow") && acceptData.getCutResult("PassMassHigh") ) {
      if ( acceptData.getCutResult("PassD2") ) {
        myCutResultForSF = TagResult::passMpassD2_2Var;
      }
      else {
        myCutResultForSF = TagResult::passMfailD2_2Var;
      }
    }
    /// Fail mass cut
    else {
      if ( acceptData.getCutResult("PassD2") ) {
        myCutResultForSF = TagResult::failMpassD2_2Var;
      }
      else {
        myCutResultForSF = TagResult::failMfailD2_2Var;
      }
    }
  }

  /// Get SF weight
  ATH_CHECK( getWeight( jet, (bool)acceptData, acceptData ) );

  if ( m_calcSF ) { 

    /// Create WriteDecorHandles
    SG::WriteDecorHandle<xAOD::JetContainer, float> decAccept(m_decAcceptKey);

    /// Decorate values
    decAccept(jet) = myCutResultForSF;

  }

  return StatusCode::SUCCESS;

}

