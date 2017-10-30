/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/BoostedXbbTagger.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"

static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> m_parent("Parent");                                                                        
static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > m_ghostMatchedTrackJets("GhostAntiKt2TrackJet"); 

BoostedXbbTagger::BoostedXbbTagger( const std::string& name ) :

  JSSTaggerBase( name ),
  m_name(name),
  m_jetMassMaxTF1(nullptr),
  m_jetMassMinTF1(nullptr),
  m_jetSubCutTF1(nullptr),
  m_dec_jetMassMin( "jetMassMin" ),
  m_dec_jetMassMax( "jetMassMax" ),
  m_dec_jssCut( "jssCut" ),
  m_dec_correctedJet( "correctedJet" ),
  m_dec_calibratedMuon( "calibratedMuon" ),
  m_dec_muonsInTrackJetLink( "muonsInTrackJetLink" ),
  m_dec_muonsInFatJetLink( "muonsInFatJetLink" ),
  m_dec_trackJetsInFatJet( "trackJetsInFatJet" )
  {

      // load parameter from configuration file
      declareProperty( "WorkingPoint",          m_wkpt = "" );       // allows to specify more WP inside one configuration file
      declareProperty( "ConfigFile",            m_configFile = "");
      declareProperty( "DecorateJet",           m_decorate = "");

      // set sensible defaults
      // b-tagging (double)
      declareProperty( "BTagCut",               m_bTagCut = 0.3706);
      declareProperty( "NumBTags",              m_numBTags = 2);
      declareProperty( "BTagAlgorithm",         m_bTagAlg = "MV2c10");

      declareProperty( "JetMassMin",            m_jetMassMinStr = "93.0");  // constant mass window for a start
      declareProperty( "JetMassMax",            m_jetMassMaxStr = "134.0");

      declareProperty( "JetPtMin",              m_jetPtMin = 250000.0);
      declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
      declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

      declareProperty( "JetSubCut",             m_jetSubCutStr = "");
      declareProperty( "JetSubVar",             m_jetSubVarStr = "");

      // muon-in-jet correction
      declareProperty( "MuonContainer",         m_muonContName = "Muons");
      declareProperty( "MuonCorrectionScheme",  m_muonCorrectionSchemeName = "Combined");
      declareProperty( "MuonPtMin",             m_muonPtMin = 10000.0);
      declareProperty( "MuonEtaMax",            m_muonEtaMax = 2.5);
      declareProperty( "MuonMatchDR",           m_muonMatchDR = 0.2);

      // track jets
      declareProperty( "TrackJetPtMin",         m_trackJetPtMin = 10000.0);
      declareProperty( "TrackJetEtaMax",        m_trackJetEtaMax = 2.5);
      declareProperty( "TrackJetNConstituents", m_trackJetNConst = 2);
      declareProperty( "TrackJetContainer",     m_trackJetContName = "GhostAntiKt2TrackJet");

}

// destructor has to be specified because virtual interface class requires it
BoostedXbbTagger::~BoostedXbbTagger() {
    delete m_jetMassMaxTF1;
    delete m_jetMassMinTF1;
    delete m_jetSubCutTF1;
}

StatusCode BoostedXbbTagger::initialize()
{
    ATH_MSG_INFO("Initializing BoostedXbbTagger tool");
    if( ! m_configFile.empty() ) {
      ATH_MSG_INFO( "Using config file : "<< m_configFile );
      // check for the existence of the configuration file
      std::string configPath;
      int releaseSeries = atoi(getenv("ROOTCORE_RELEASE_SERIES"));
      if(releaseSeries>=25){
        // r21 with release >= 21.2.4
        configPath = PathResolverFindDataFile(("BoostedJetTaggers/"+m_configFile).c_str());
      }
      else {
        // r20.7 with release <= AnalysisBase,2.4.24
        #ifdef ROOTCORE
            configPath = gSystem->ExpandPathName(("$ROOTCOREBIN/data/BoostedJetTaggers/"+m_configFile).c_str());
        #else
            configPath = PathResolverFindXMLFile(("$ROOTCOREBIN/data/BoostedJetTaggers/"+m_configFile).c_str());
        #endif
      }
      /* https://root.cern.ch/root/roottalk/roottalk02/5332.html */
      FileStat_t fStats;
      int fSuccess = gSystem->GetPathInfo(configPath.c_str(), fStats);
      if(fSuccess != 0){
        ATH_MSG_ERROR("Recommendations file could not be found : " << configPath);
        return StatusCode::FAILURE;
      }
      else {
        ATH_MSG_DEBUG("Recommendations file was found : "<<configPath);
      }


      TEnv configReader;
      if(configReader.ReadFile( configPath.c_str(), EEnvLevel(0) ) != 0 ) {
        ATH_MSG_ERROR( "Error while reading config file : "<< configPath );
        return StatusCode::FAILURE;
      }

      if( m_wkpt.empty() ){
           m_bTagAlg                        = configReader.GetValue("BTagAlgorithm",         m_bTagAlg.c_str());
           m_numBTags                       = configReader.GetValue("NumBTags",              m_numBTags);
           m_bTagCut                        = configReader.GetValue("BTagCut",               m_bTagCut);
           m_jetMassMinStr                  = configReader.GetValue("JetMassMin",         m_jetMassMinStr.c_str());
           m_jetMassMaxStr                  = configReader.GetValue("JetMassMax",         m_jetMassMaxStr.c_str());
           m_jetPtMin                       = configReader.GetValue("JetPtMin",              m_jetPtMin );
           m_jetPtMax                       = configReader.GetValue("JetPtMax",              m_jetPtMax );
           m_jetEtaMax                      = configReader.GetValue("JetEtaMax",             m_jetEtaMax );
           m_jetSubCutStr                   = configReader.GetValue("JetSubCut",             m_jetSubCutStr.c_str() );
           m_jetSubVarStr                   = configReader.GetValue("JetSubVar",             m_jetSubVarStr.c_str() );
           m_muonPtMin                      = configReader.GetValue("MuonPtMin",             m_muonPtMin );
           m_muonEtaMax                     = configReader.GetValue("MuonEtaMax",            m_muonEtaMax );
           m_muonMatchDR                    = configReader.GetValue("MuonMatchDR",           m_muonMatchDR );
           m_muonContName                   = configReader.GetValue("MuonContainer",         m_muonContName.c_str() );
           m_muonCorrectionSchemeName       = configReader.GetValue("MuonCorrectionScheme",  m_muonCorrectionSchemeName.c_str() );
           m_trackJetPtMin                  = configReader.GetValue("TrackJetPtMin",         m_trackJetPtMin );
           m_trackJetEtaMax                 = configReader.GetValue("TrackJetEtaMax",        m_trackJetEtaMax );
           m_trackJetNConst                 = configReader.GetValue("TrackJetNConstituents", m_trackJetNConst );
           m_trackJetContName               = configReader.GetValue("TrackJetContainer",     m_trackJetContName.c_str() );
      }
      else {
            m_bTagAlg                        = configReader.GetValue((m_wkpt+"BTagAlgorithm").c_str(),         m_bTagAlg.c_str());
            m_numBTags                       = configReader.GetValue((m_wkpt+"NumBTags").c_str(),              m_numBTags);
            m_bTagCut                        = configReader.GetValue((m_wkpt+"BTagCut").c_str(),               m_bTagCut);
            m_jetMassMinStr                  = configReader.GetValue((m_wkpt+"JetMassMin").c_str(),         m_jetMassMinStr.c_str());
            m_jetMassMaxStr                  = configReader.GetValue((m_wkpt+"JetMassMax").c_str(),         m_jetMassMaxStr.c_str());
            m_jetPtMin                       = configReader.GetValue((m_wkpt+"JetPtMin").c_str(),              m_jetPtMin );
            m_jetPtMax                       = configReader.GetValue((m_wkpt+"JetPtMax").c_str(),              m_jetPtMax );
            m_jetEtaMax                      = configReader.GetValue((m_wkpt+"JetEtaMax").c_str(),             m_jetEtaMax );
            m_jetSubCutStr                   = configReader.GetValue((m_wkpt+"JetSubCut").c_str(),             m_jetSubCutStr.c_str() );
            m_jetSubVarStr                   = configReader.GetValue((m_wkpt+"JetSubVar").c_str(),             m_jetSubVarStr.c_str() );
            m_muonPtMin                      = configReader.GetValue((m_wkpt+"MuonPtMin").c_str(),             m_muonPtMin );
            m_muonEtaMax                     = configReader.GetValue((m_wkpt+"MuonEtaMax").c_str(),            m_muonEtaMax );
            m_muonMatchDR                    = configReader.GetValue((m_wkpt+"MuonMatchDR").c_str(),           m_muonMatchDR );
            m_muonContName                   = configReader.GetValue((m_wkpt+"MuonContainer").c_str(),         m_muonContName.c_str() );
            m_muonCorrectionSchemeName       = configReader.GetValue((m_wkpt+"MuonCorrectionScheme").c_str(),  m_muonCorrectionSchemeName.c_str() );
            m_trackJetPtMin                  = configReader.GetValue((m_wkpt+"TrackJetPtMin").c_str(),         m_trackJetPtMin );
            m_trackJetEtaMax                 = configReader.GetValue((m_wkpt+"TrackJetEtaMax").c_str(),        m_trackJetEtaMax );
            m_trackJetNConst                 = configReader.GetValue((m_wkpt+"TrackJetNConstituents").c_str(), m_trackJetNConst );
            m_trackJetContName               = configReader.GetValue((m_wkpt+"TrackJetContainer").c_str(),     m_trackJetContName.c_str() );
      }

      // get the decoration name
      m_decorationName = configReader.GetValue("DecorationName" ,"");

    }

    // set up muon tools
    m_muonSelectionTool.setTypeAndName("CP::MuonSelectionTool/"+m_name+"MuonSelection");
    m_muonSelectionTool.retrieve();

    m_muonCalibrationAndSmearingTool.setTypeAndName("CP::MuonCalibrationAndSmearingTool/"+m_name+"MuonCalibration");
    m_muonCalibrationAndSmearingTool.retrieve();

    if (!getMuonCorrectionScheme(m_muonCorrectionSchemeName, m_muonCorrectionScheme)) {
        ATH_MSG_ERROR( "Error setting mass calibration scheme to " << m_muonCorrectionSchemeName );
        return StatusCode::FAILURE;
    }

    if (m_jetSubCutStr != "") {
        ATH_MSG_INFO("Initialize formula for substructure cut using " << m_jetSubCutStr.c_str());
        m_jetSubCutTF1 = new TF1((m_name + "SubstructureCut").c_str(), m_jetSubCutStr.c_str(), 0, 14000);
        m_jetSubCutStr = m_jetSubCutTF1->GetExpFormula(); // cross check
    }

    ATH_MSG_INFO("Initialize formula for lower mass cut using " << m_jetMassMinStr.c_str());
    m_jetMassMinTF1 = new TF1((m_name + "MassMin").c_str(), m_jetMassMinStr.c_str(), 0, 14000);
    m_jetMassMinStr = m_jetMassMinTF1->GetExpFormula(); // cross check

    ATH_MSG_INFO("Initialize formula for upper mass cut using " << m_jetMassMaxStr.c_str());
    m_jetMassMaxTF1 = new TF1((m_name + "MassMax").c_str(), m_jetMassMaxStr.c_str(), 0, 14000);
    m_jetMassMaxStr = m_jetMassMaxTF1->GetExpFormula(); // cross check

    ATH_MSG_INFO("BoostedXbbTagger configured successfully using the following parameters:");
    ATH_MSG_INFO("Decorator:             " << m_name);
    ATH_MSG_INFO("BTagAlgorithm:         " << m_bTagAlg);
    ATH_MSG_INFO("BTagCut:               " << m_bTagCut);
    ATH_MSG_INFO("NumBTags:              " << m_numBTags );
    ATH_MSG_INFO("JetMassMin:            " << m_jetMassMinStr);
    ATH_MSG_INFO("JetMassMax:            " << m_jetMassMaxStr);
    ATH_MSG_INFO("JetPtMin:              " << m_jetPtMin);
    ATH_MSG_INFO("JetPtMax:              " << m_jetPtMax);
    ATH_MSG_INFO("JetEtaMax:             " << m_jetEtaMax);
    ATH_MSG_INFO("JetSubCut:             " << m_jetSubCutStr);
    ATH_MSG_INFO("JetSubVar:             " << m_jetSubVarStr);
    ATH_MSG_INFO("MuonContainer:         " << m_muonContName);
    ATH_MSG_INFO("MuonCorrectionScheme:  " << m_muonCorrectionSchemeName);
    ATH_MSG_INFO("MuonPtMin:             " << m_muonPtMin);
    ATH_MSG_INFO("MuonEtaMax:            " << m_muonEtaMax);
    ATH_MSG_INFO("MuonMatchDR:           " << m_muonMatchDR);
    ATH_MSG_INFO("TrackJetPtMin:         " << m_trackJetPtMin);
    ATH_MSG_INFO("TrackJetEtaMax:        " << m_trackJetEtaMax);
    ATH_MSG_INFO("TrackJetNConstituents: " << m_trackJetNConst);
    ATH_MSG_INFO("TrackJetContainer:     " << m_trackJetContName);
    ATH_MSG_INFO("decorationName:        " << m_decorationName);

    // initialize decorators as decorationName+_decorator
    ATH_MSG_INFO( "Decorators that will be attached to jet :" );
    std::string dec_name;

    dec_name = m_decorationName+"_MassMin";
    ATH_MSG_INFO( "  "<<dec_name<<" : lower mass cut for tagger choice" );
    m_dec_jetMassMin          = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_MassMax";
    ATH_MSG_INFO( "  "<<dec_name<<" : upper mass cut for tagger choice" );
    m_dec_jetMassMax          = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_JSSCut";
    ATH_MSG_INFO( "  "<<dec_name<<" : JSS cut for tagger choice" );
    m_dec_jssCut              = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_CorrectedJetP4";
    ATH_MSG_INFO( "  "<<dec_name<<" : the four-vector of the muon-corrected jet" );
    m_dec_correctedJet        = SG::AuxElement::Decorator<TLorentzVector>((dec_name).c_str());
    dec_name = m_decorationName+"_CalibratedMuonP4";
    ATH_MSG_INFO( "  "<<dec_name<<" : the four-vector of the muon used for the correction" );
    m_dec_calibratedMuon      = SG::AuxElement::Decorator<TLorentzVector>((dec_name).c_str());
    dec_name = m_decorationName+"_MatchedMuons";
    ATH_MSG_INFO( "  "<<dec_name<<" : the element links to all matched muons" );
    m_dec_muonsInTrackJetLink = SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > >((dec_name).c_str());
    dec_name = m_decorationName+"_CorrectionMuons";
    ATH_MSG_INFO( "  "<<dec_name<<" : the element links to the muons used for the correction" );
    m_dec_muonsInFatJetLink   = SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > >((dec_name).c_str());
    dec_name = m_decorationName+"_TrackJets";
    ATH_MSG_INFO( "  "<<dec_name<<" : the element links to the track jets that were used for b-tagging" );
    m_dec_trackJetsInFatJet   = SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > >((dec_name).c_str());

    // initialize the tagger states
    m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
    m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
    m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );
    m_accept.addCut( "ValidJetContent"     , "True if the jet is alright technicall (e.g. all attributes necessary for tag)"        );
    m_accept.addCut( "PassMassLow"         , "True if the jet passes the lower mass bound : mJet>MCutLow"       );
    m_accept.addCut( "PassMassHigh"        , "True if the jet passes the upper mass bound : mJet<MCutHigh"      );
    m_accept.addCut( "PassBTag"            , "True if the jet is flagged with b-tagging - one or two tags depending on configuration"          );
    m_accept.addCut( "PassJSS"             , "True if the jet passes the substructure cut" ); //not enabled for the moment and only acts as a pass through

    //loop over and print out the cuts that have been configured
    ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
    showCuts();

    return StatusCode::SUCCESS;
}

StatusCode BoostedXbbTagger::finalize(){
    /* Delete or clear anything */
    return StatusCode::SUCCESS;
}

Root::TAccept BoostedXbbTagger::tag(const xAOD::Jet& jet) const
{
  ATH_MSG_DEBUG( ": Obtaining Standard Xbb tagger result" );

  // reset the TAccept cut results to false
  m_accept.clear();

  // set the jet validity bits to 1 by default
  m_accept.setCutResult( "ValidPtRangeHigh", true);
  m_accept.setCutResult( "ValidPtRangeLow" , true);
  m_accept.setCutResult( "ValidEtaRange"   , true);
  m_accept.setCutResult( "ValidJetContent" , true);

  // no associated muons available at the moment in DxAOD
  // decorate track jets with muons using simple dR matching
  if (decorateWithMuons(jet) == StatusCode::FAILURE) {
    ATH_MSG_ERROR("No muon decoration for muon-in-jet correction available.");
    m_accept.setCutResult("ValidJetContent" , false);
  }
  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    m_accept.setCutResult("ValidEtaRange", false);
  }
  if (jet.pt() < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeLow", false);
  }
  if (jet.pt() > m_jetPtMax) {
    ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeHigh", false);
  }
  /* Steps:
      1. Get all AntiKt2TrackJets asssociated with the ungroomed jet
      2. B-tag the two leading track-jets
        - if double b-tagging, require that both track-jets are b-tagged
        - if single b-tagging, and there is more than one track-jet, take the highest MV2 of the leading two jets
      3. Match the muon (if any) to these b-tagged track-jets
        - if more than 1 muon matches a track jet (within the radius of the track jet), only use the muon closest in DR
      4. Correct the fat-jet mass by putting the matched muon back (if there is a matched muon)
      5. Set a cut on the corrected fat jet mass
      6. Evaluate JSS cut (not there yet)
  */
  // global pass variables, set to false by default
  bool pass_btag = false;
  bool pass_lowmass = false;
  bool pass_highmass = false;
  bool pass_jss  = true; // always true unless a cut is specified
  // Step 1
  // std::vector<const xAOD::Jet*> associated_trackJets;
  std::vector<ElementLink<xAOD::IParticleContainer> > associated_trackJets;

  // get the track jets from the parent
  bool problemWithParent = false;
  ElementLink<xAOD::JetContainer> parentEL;
  if(!m_parent.isAvailable(jet)) problemWithParent = true;
  else parentEL = m_parent(jet);
  if(problemWithParent || !parentEL.isValid()){
    if(problemWithParent) ATH_MSG_ERROR("Parent decoration does not exist.");
    if(!parentEL.isValid()) ATH_MSG_ERROR("Parent link is not valid.");
    m_accept.setCutResult("ValidJetContent" , false);
  }
  else {
    const xAOD::Jet* parentJet = *parentEL;
    // use accessor instead of getAssociatedObject in order to have EL
    if (!m_ghostMatchedTrackJets.isAvailable(*parentJet)) {
      ATH_MSG_ERROR("Ghostmatched jet collection does not exist.");
    }
    associated_trackJets = m_ghostMatchedTrackJets(*parentJet);
  }

  // decorate all trackjets by default
  // filter out the track jets we do not want (pT > 10 GeV and |eta| < 2.5 and at least 2 constituents)
  std::vector<ElementLink<xAOD::IParticleContainer> > associated_trackJets_filtered;
  for(const auto& trackJetEL: associated_trackJets) {
    if(!trackJetEL.isValid()) {
        ATH_MSG_ERROR("Track jet link is not valid.");
        m_accept.setCutResult("ValidJetContent" , false);
    }
    const xAOD::Jet *trackJet(static_cast<const xAOD::Jet*>(*trackJetEL));
    if (trackJet->pt() < m_trackJetPtMin) continue;
    if (fabs(trackJet->eta()) > m_trackJetEtaMax) continue;
    if (trackJet->numConstituents() < m_trackJetNConst) continue;
    associated_trackJets_filtered.push_back(trackJetEL);
  }
  std::sort(associated_trackJets_filtered.begin(), associated_trackJets_filtered.end(), [](ElementLink<xAOD::IParticleContainer> lhs, ElementLink<xAOD::IParticleContainer> rhs) -> bool { return ((static_cast<const xAOD::IParticle*>(*lhs))->pt() > (static_cast<const xAOD::IParticle*>(*rhs))->pt()); });
  if ((int)associated_trackJets_filtered.size() < m_numBTags) {
    ATH_MSG_DEBUG("Number of associated track jets below number of required b-tags for this working point.");
    m_accept.setCutResult("ValidJetContent" , false);
  }
  if (associated_trackJets_filtered.size() < 1) {
    ATH_MSG_DEBUG("Number of associated track jets is zero."); // only happens when b-tagging is switched off.
    m_accept.setCutResult("ValidJetContent" , false);
  }

  // Step 2
  int num_bTags(0);
  int num_trackJets(0);
  std::vector<ElementLink<xAOD::IParticleContainer> > associated_trackJets_links;
  for (const auto& trackJetEL: associated_trackJets_filtered) {
    if (num_trackJets >= 2) break; // make configurable?
    const xAOD::Jet *trackJet(static_cast<const xAOD::Jet*>(*trackJetEL));
    double bTagWeight(FLT_MIN);
    if(!trackJet->btagging()->MVx_discriminant(m_bTagAlg, bTagWeight)){
      ATH_MSG_ERROR("Could not retrieve the discriminant for " << m_bTagAlg << ".");
      m_accept.setCutResult("ValidJetContent" , false);
    }
    if (bTagWeight > m_bTagCut) {
      num_bTags++;
      associated_trackJets_links.push_back(trackJetEL);  // only keep track jets that are b-tagged
    }
    num_trackJets++;
  }
  if( num_bTags < m_numBTags ){
    ATH_MSG_DEBUG("Jet FAILED the b-tagging: b-tags found = " << num_bTags << ", b-tags required = " << m_numBTags << ".");
  } else {
    ATH_MSG_DEBUG("Jet PASSED the b-tagging: b-tags found = " << num_bTags << ", b-tags required = " << m_numBTags << ".");
    pass_btag = true;
  }
  if (m_decorate)
    m_dec_trackJetsInFatJet(jet) = associated_trackJets_links;

  // Step 3
  std::vector<xAOD::Muon*> calibratedMuons;
  std::vector<const xAOD::Muon*> matched_muons;
  std::vector<ElementLink<xAOD::IParticleContainer> > matched_muons_links;
  for (const auto& trackJetEL: associated_trackJets_links) {
      const xAOD::Jet *trackJet(static_cast<const xAOD::Jet*>(*trackJetEL));
      float maxDR(m_muonMatchDR);
//      trackJet->getAttribute("SizeParameter", maxDR);
      const xAOD::Muon *closest_muon(nullptr);
      ElementLink<xAOD::IParticleContainer> closest_muonEL;
      // get muons from jet decoration
      if( ! m_dec_muonsInTrackJetLink.isAvailable( *trackJet ) ) {
          ATH_MSG_ERROR("No muons link found for jet.");
          m_accept.setCutResult("ValidJetContent" , false);
      }
      for (auto muonLink : m_dec_muonsInTrackJetLink( *trackJet)) {
          if(!muonLink.isValid()) {
              ATH_MSG_DEBUG("Muon link not valid."); // ok to continue?
              continue;
          }
          const xAOD::Muon *muon(static_cast<const xAOD::Muon*>(*muonLink));
          // apply muon correction if not calibrated yet
          static SG::AuxElement::Accessor<float> acc_muonSpectrometerPt ("MuonSpectrometerPt");
          static SG::AuxElement::Accessor<float> acc_innerDetectorPt ("InnerDetectorPt");
          if (!(acc_innerDetectorPt.isAvailable(*muon) && acc_muonSpectrometerPt.isAvailable(*muon))) {
              ATH_MSG_DEBUG("No decorators for MuonSpectrometerPt or InnerDetectorPt found. Calibrate muons on-the-fly.");
              xAOD::Muon *muon_calib(nullptr);

              if( m_muonCalibrationAndSmearingTool->correctedCopy( *muon, muon_calib) != CP::CorrectionCode::Ok ){
                  ATH_MSG_ERROR("Could not get calibrated copy of muon.");
                  m_accept.setCutResult("ValidJetContent" , false);
              }

              if (m_decorate)
                  m_dec_calibratedMuon(*muon) = muon_calib->p4();
              // save the pointers for deletion later
              calibratedMuons.push_back(muon_calib);
              // work with calibrated muon
              muon = muon_calib;
          }
          // muon quality selection
          if (muon->pt() < m_muonPtMin) continue;
          if (m_muonSelectionTool->getQuality(*muon) > xAOD::Muon::Medium) continue;
          if (fabs(muon->eta()) > m_muonEtaMax) continue;
          // find clostest muon
          float DR( trackJet->p4().DeltaR(muon->p4()) );
          if (DR > maxDR) continue;
          maxDR = DR;
          closest_muon = muon;
          closest_muonEL = muonLink;
      }

      // check if the closest muon was already selected
      for (const auto muon: matched_muons){
          if(closest_muon == muon){
              closest_muon = nullptr;
              ATH_MSG_DEBUG("Muon duplicate found! Skipping.");
              break;
          }
      }
      if (closest_muon) {
          matched_muons.push_back(closest_muon);
          matched_muons_links.push_back(closest_muonEL);
      }
      if (m_decorate) m_dec_muonsInFatJetLink(jet) = matched_muons_links;
  }

  // Step 4
  xAOD::JetFourMom_t corrected_jet_p4 = getMuonCorrectedJetFourMom(jet, matched_muons, m_muonCorrectionScheme);
  TLorentzVector corrected_jet(corrected_jet_p4.x(), corrected_jet_p4.y(), corrected_jet_p4.z(), corrected_jet_p4.t());
  // delete the vector of pointers to muons after you have used them to correct the four vector
  for(xAOD::Muon * muonPointer : calibratedMuons)
    delete muonPointer;
  if (m_decorate) m_dec_correctedJet(jet) = corrected_jet;

  // Step 5
  float jetMassMin = m_jetMassMinTF1->Eval(corrected_jet.Pt()/1.e3);
  float jetMassMax = m_jetMassMaxTF1->Eval(corrected_jet.Pt()/1.e3);
  if (corrected_jet.M()/1.e3 >= jetMassMin) pass_lowmass = true;
  if (corrected_jet.M()/1.e3 <= jetMassMax) pass_highmass = true;
  if (pass_lowmass && pass_highmass) {
    ATH_MSG_DEBUG("Jet PASSED the mass window cut. Mass: " << corrected_jet.M()/1.e3 << " GeV, Mass Window: [ " << jetMassMin << ", " << jetMassMax << " ]");
  } else {
    ATH_MSG_DEBUG("Jet FAILED the mass window cut. Mass: " << corrected_jet.M()/1.e3 << " GeV, Mass Window: [ " << jetMassMin << ", " << jetMassMax << " ]");
  }
  if (m_decorate) {
      m_dec_jetMassMin(jet) = jetMassMin;
      m_dec_jetMassMax(jet) = jetMassMax;
  }

  // step 6
  float jsscut(0.);
  float jssvar(0.);
  if (m_jetSubCutTF1) {
      if (!getJSSVar(jssvar, jet, m_jetSubVarStr)) {
          ATH_MSG_DEBUG("Could not retrieve jet substructure variable.");
          m_accept.setCutResult("ValidJetContent" , false);
      }
      jsscut = m_jetSubCutTF1->Eval(jet.pt()/1.e3);
      pass_jss = (jssvar < jsscut);
  }
  if (m_decorate)
    m_dec_jssCut(jet) = jsscut;
  if (pass_jss) {
    ATH_MSG_DEBUG("Jet PASSED the substructure cut. " << m_jetSubVarStr.c_str() << "=" << jssvar << ", cut=" << jsscut);
  }
  else {
    ATH_MSG_DEBUG("Jet FAILED the substructure cut. " << m_jetSubVarStr.c_str() << "=" << jssvar << ", cut=" << jsscut);
  }

  // set the bits of the TAccept
  if(pass_lowmass)
    m_accept.setCutResult( "PassMassLow"    , true);

  if(pass_highmass)
    m_accept.setCutResult( "PassMassHigh"   , true);

  if(pass_btag)
    m_accept.setCutResult( "PassBTag"       , true);

  // the substructure selection is not commissioned and acts as a pass through at this point
  if(pass_jss)
    m_accept.setCutResult( "PassJSS"        , true);

  // return the TAccept to be queried by the user
  return m_accept;

}

StatusCode BoostedXbbTagger::decorateWithMuons(const xAOD::Jet& jet) const
{
    // retrieve muons from StoreGate
    const xAOD::MuonContainer* muons;
    if (!evtStore()->retrieve(muons, m_muonContName)) {
        ATH_MSG_ERROR("Cannot retrieve muon collection " << m_muonContName << " from event store.");
        return StatusCode::FAILURE;
    }
    // retrieve ghost-associated track jets from large-R jet
    std::vector<const xAOD::Jet*> associated_trackJets;
    // get the element links to the parent, ungroomed jet
    if(!m_parent.isAvailable(jet)) {
        ATH_MSG_ERROR("Parent (ungroomed) jet collection does not exist.");
        return StatusCode::FAILURE;
    }
    ElementLink<xAOD::JetContainer> parentEL = m_parent(jet);
    if(!parentEL.isValid()) {
        ATH_MSG_ERROR("Parent link is not valid.");
        return StatusCode::FAILURE;
    }
    // access the track jets
    const xAOD::Jet* parentJet = *parentEL;
    if(!parentJet->getAssociatedObjects<xAOD::Jet>(m_trackJetContName, associated_trackJets)) {
        ATH_MSG_DEBUG("No associated track jets found on parent jet.");
        return StatusCode::SUCCESS;
    }

    // decorate all track jets by default, no selection, no muon overlap removal (will be done later)
    float maxDR(m_muonMatchDR);
    for(auto trackJet: associated_trackJets) {
        std::vector<ElementLink<xAOD::IParticleContainer> > muons_in_jet;
        for(const auto& muon: *muons) {
            float DR( trackJet->p4().DeltaR(muon->p4()) );
            if (DR > maxDR) continue;
            ElementLink<xAOD::IParticleContainer> muonEL( *muons, muon->index() );
            muons_in_jet.push_back(muonEL);
        }
        m_dec_muonsInTrackJetLink(*trackJet) = muons_in_jet;
        ATH_MSG_DEBUG("Found " << muons_in_jet.size() << " muons within R < " << m_muonMatchDR << " of associated track jet.");
    }
    return StatusCode::SUCCESS;
}

StatusCode BoostedXbbTagger::getMuonCorrectionScheme(std::string scheme_name, MuonCorrectionScheme& scheme) const {
    if (scheme_name == "Combined") scheme = Combined;
    else if (scheme_name == "Calorimeter") scheme =  Calorimeter;
    else if (scheme_name == "TrackAssisted") scheme =  TrackAssisted;
    else if (scheme_name == "SimpleMuon") scheme =  SimpleMuon;
    else if (scheme_name == "NoMuon") scheme =  NoMuon;
    else {
        ATH_MSG_ERROR("Muon correction scheme \"" << scheme_name.c_str() << "\" not in list: \"Combined\", \"Calorimeter\", \"TrackAssisted\", \"SimpleMuon\", \"NoMuon\".");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;;
}

std::string BoostedXbbTagger::getMuonCorrectionSchemeName(MuonCorrectionScheme scheme) const {
    if      (scheme == Combined)      return "Combined";
    else if (scheme == Calorimeter)   return "Calorimeter";
    else if (scheme == TrackAssisted) return "TrackAssisted";
    else if (scheme == SimpleMuon)    return "SimpleMuon";
    else if (scheme == NoMuon)        return "NoMuon";
    else {
        ATH_MSG_ERROR("Unknown muon correction scheme.");
    }
    return "";
}

const xAOD::JetFourMom_t BoostedXbbTagger::getMuonCorrectedJetFourMom(const xAOD::Jet& jet, std::vector<const xAOD::Muon*> muons, MuonCorrectionScheme scheme, bool useJMSScale) const
{
    xAOD::JetFourMom_t JetCorr_tlv = jet.jetP4();
    ATH_MSG_DEBUG("Derive muon-in-jet correction: nMuons=" << (int) muons.size() << "\tMuonCorrectionScheme=" << getMuonCorrectionSchemeName(scheme).c_str() << "\tuseJMSScale=" << useJMSScale);
    if (scheme == Calorimeter) {
        // muon-in-jet correction for jets calibrated using calorimeter mass
        xAOD::JetFourMom_t CaloJet_tlv = jet.jetP4();
        if (useJMSScale) CaloJet_tlv = jet.jetP4("JetJMSScaleMomentumCalo");
        for(auto muon : muons) {
            // get energy loss of muon in the calorimeter
            float eLoss(0.0);
            muon->parameter(eLoss, xAOD::Muon::EnergyLoss);
            ATH_MSG_DEBUG("Energy loss in calorimter=" << eLoss);
            // use muon tlv to get x/y/z compontent of energy loss
            auto muon_tlv = muon->p4();
            double eLossX = eLoss * sin(muon_tlv.Theta()) * cos(muon_tlv.Phi());
            double eLossY = eLoss * sin(muon_tlv.Theta()) * sin(muon_tlv.Phi());
            double eLossZ = eLoss * cos(muon_tlv.Theta());
            TLorentzVector mLoss(eLossX, eLossY, eLossZ, eLoss);
            auto muonCorr_tlv = muon_tlv - mLoss;
            // apply muon-in-jet correction for track-assisted mass calibration to output jet TLV
            CaloJet_tlv += xAOD::JetFourMom_t(muonCorr_tlv.Pt(), muonCorr_tlv.Eta(), muonCorr_tlv.Phi(), muonCorr_tlv.M());
        }
        JetCorr_tlv = CaloJet_tlv;
    } else if (scheme == TrackAssisted) {
        // muon-in-jet correction for jets calibrated using track-assisted mass
        xAOD::JetFourMom_t TAJet_tlv = jet.jetP4();
        if (useJMSScale) TAJet_tlv = jet.jetP4("JetJMSScaleMomentumTA");
        xAOD::JetFourMom_t CaloJet_tlv = jet.jetP4("JetJMSScaleMomentumCalo");
        xAOD::JetFourMom_t CaloJetCorr_tlv =  getMuonCorrectedJetFourMom(jet, muons, Calorimeter, true);
        float TAJetCorr_m = TAJet_tlv.M() / CaloJet_tlv.Pt() * CaloJetCorr_tlv.Pt() ;
        float TAJetCorr_pt = sqrt((CaloJetCorr_tlv.E() * CaloJetCorr_tlv.E()) - (TAJetCorr_m * TAJetCorr_m)) / cosh( CaloJetCorr_tlv.Eta() );
        // apply muon-in-jet correction for track-assisted mass calibration to output jet TLV
        JetCorr_tlv = xAOD::JetFourMom_t(TAJetCorr_pt, CaloJetCorr_tlv.Eta(), CaloJetCorr_tlv.Phi(), TAJetCorr_m);
    } else if (scheme == Combined) {
        // muon-in-jet correction for jets calibrated using combined mass
        xAOD::JetFourMom_t TAJet_tlv = jet.jetP4("JetJMSScaleMomentumTA");
        xAOD::JetFourMom_t TAJetCorr_tlv = getMuonCorrectedJetFourMom(jet, muons, TrackAssisted, true);
        xAOD::JetFourMom_t CaloJet_tlv = jet.jetP4("JetJMSScaleMomentumCalo");
        xAOD::JetFourMom_t CaloJetCorr_tlv = getMuonCorrectedJetFourMom(jet, muons, Calorimeter, true);
        xAOD::JetFourMom_t CombJet_tlv = jet.jetP4();
        float CaloWeight = (CombJet_tlv.M() -   TAJet_tlv.M()) / (CaloJet_tlv.M() - TAJet_tlv.M());
        float TAWeight   = (CaloJet_tlv.M() - CombJet_tlv.M()) / (CaloJet_tlv.M() - TAJet_tlv.M());
        ATH_MSG_DEBUG("CaloWeight=" << CaloWeight << "\tTAWeight=" << TAWeight);
        float CombJetCorr_m = CaloWeight * CaloJetCorr_tlv.M() + TAWeight * TAJetCorr_tlv.M();
        float CombJetCorr_pt = sqrt((CaloJetCorr_tlv.E() * CaloJetCorr_tlv.E()) - (CombJetCorr_m * CombJetCorr_m)) / cosh( CaloJetCorr_tlv.Eta() );
        // apply muon-in-jet correction for track-assisted mass calibration to output jet TLV
        JetCorr_tlv = xAOD::JetFourMom_t(CombJetCorr_pt, CaloJetCorr_tlv.Eta(), CaloJetCorr_tlv.Phi(), CombJetCorr_m);
    } else if (scheme == SimpleMuon) {
        // unknown mass calibration; just add muon 4-momentum
        for(auto muon : muons) {
            JetCorr_tlv += xAOD::JetFourMom_t(muon->pt(), muon->eta(), muon->phi(), muon->m());;
        }
    }
    ATH_MSG_DEBUG("Before muon-in-jet:  pt=" << jet.pt() << "\teta=" << jet.eta() << "\tphi=" << jet.phi() << "\tm=" << jet.m());
    ATH_MSG_DEBUG("After muon-in-jet:   pt=" << JetCorr_tlv.pt() << "\teta=" << JetCorr_tlv.eta() << "\tphi=" << JetCorr_tlv.phi() << "\tm=" << JetCorr_tlv.M());
    for(auto muon : muons) {
        ATH_MSG_DEBUG("         muons:  pt=" << muon->pt() << "\teta=" << muon->eta() << "\tphi=" << muon->phi() << "\tm=" << muon->m());
    }
    return JetCorr_tlv;
}

StatusCode BoostedXbbTagger::getJSSVar(float& jssvar, const xAOD::Jet& jet, std::string name) const {
    jssvar = 0.;
    static SG::AuxElement::ConstAccessor<float>acc_jssvar(name);
    if (acc_jssvar.isAvailable(jet)) {
        jssvar = acc_jssvar(jet);
        return StatusCode::SUCCESS;
    }
    if (m_jetSubVarStr == "D2") {
        // try to recover by calculatinc it ourselves
        static SG::AuxElement::ConstAccessor<float>    acc_ECF1 ("ECF1");
        static SG::AuxElement::ConstAccessor<float>    acc_ECF2 ("ECF2");
        static SG::AuxElement::ConstAccessor<float>    acc_ECF3 ("ECF3");
        if ((!acc_ECF1.isAvailable(jet) || !acc_ECF2.isAvailable(jet) || !acc_ECF3.isAvailable(jet))) {
            ATH_MSG_VERBOSE(": D2 is not available in the file your using, neither are the ECF# variables" );
            return StatusCode::FAILURE;
        }
        if ( acc_ECF2(jet) > 0.0 ) {
            jssvar = acc_ECF3(jet) * pow(acc_ECF1(jet), 3.0) / pow(acc_ECF2(jet), 3.0);
            return StatusCode::SUCCESS;
        }
    }
    return StatusCode::FAILURE;
}

std::vector<const xAOD::Muon*> BoostedXbbTagger::getCorrectionMuons(const xAOD::Jet& jet) const {
    std::vector<const xAOD::Muon*> muons;
    std::string accName = m_name+"CorrectionMuons";
    static SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > accMatchedMuons(accName);
    if (!accMatchedMuons.isAvailable(jet)) {
      ATH_MSG_DEBUG("Matched muon collection \"" << accName << "\" does not exist.");
      return muons;
    }
    for (auto muonLink : accMatchedMuons(jet)) {
        if(!muonLink.isValid()) {
            ATH_MSG_WARNING("Muon link not valid.");
        }
        muons.push_back(static_cast<const xAOD::Muon*>(*muonLink));
    }
    return muons;
}

std::vector<const xAOD::Jet*> BoostedXbbTagger::getTrackJets(const xAOD::Jet& jet) const {
    std::vector<const xAOD::Jet*> trackJets;
    std::string accName = m_name+"TrackJets";
    static SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > accTrackJets(accName);
    if (!accTrackJets.isAvailable(jet)) {
      ATH_MSG_DEBUG("Track jet collection \"" << accName << "\" does not exist.");
      return trackJets;
    }
    for (auto trackJetLink : accTrackJets(jet)) {
        if(!trackJetLink.isValid()) {
            ATH_MSG_WARNING("Track jet link not valid.");
        }
        trackJets.push_back(static_cast<const xAOD::Jet*>(*trackJetLink));
    }
    return trackJets;
}

TLorentzVector BoostedXbbTagger::getCorrectedJetTLV(const xAOD::Jet& jet) const {
  return m_dec_correctedJet(jet);
}

TLorentzVector BoostedXbbTagger::getCalibratedMuonTLV(const xAOD::Muon& muon) const {
  return m_dec_calibratedMuon(muon);
}

float BoostedXbbTagger::getMassMin(const xAOD::Jet& jet) const {
  return m_dec_jetMassMin(jet);
}

float BoostedXbbTagger::getMassMax(const xAOD::Jet& jet) const {
  return m_dec_jetMassMax(jet);
}
