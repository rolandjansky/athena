/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JetQGTagger.h"

#include <TRandom3.h>
#include <TSystem.h>

#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthFilterTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"

#include "xAODTracking/VertexContainer.h"

namespace CP {

  JetQGTagger::JetQGTagger( const std::string& name): JSSTaggerBase( name ),
                  m_appliedSystEnum(QG_NONE),
                  m_hquark(nullptr),
                  m_hgluon(nullptr),
                  m_topo_hquark(nullptr),
                  m_exp_hquark_up(nullptr),
                  m_exp_hquark_down(nullptr),
                  m_exp_hgluon_up(nullptr),
                  m_exp_hgluon_down(nullptr),
                  m_me_hquark_up(nullptr),
                  m_me_hquark_down(nullptr),
                  m_me_hgluon_up(nullptr),
                  m_me_hgluon_down(nullptr),
                  m_pdf_hquark_up(nullptr),
                  m_pdf_hquark_down(nullptr),
                  m_pdf_hgluon_up(nullptr),
                  m_pdf_hgluon_down(nullptr),
                  m_trackeff_hquark(nullptr),
                  m_trackeff_hgluon(nullptr),
                  m_fake_hquark(nullptr),
                  m_fake_hgluon(nullptr),
                  m_trkSelectionTool(name+"_trackselectiontool", this),
                  m_trkTruthFilterTool(name+"_trackfiltertool",this),
                  m_trkFakeTool(name+"_trackfaketool",this),
                  m_jetTrackFilterTool(name+"_jettrackfiltertool",this),
                  m_originTool(name+"_origintool",this)
  {

    declareProperty( "NTrackCut",    m_NTrackCut=-1);
    declareProperty( "cuttype",      m_cuttype="log_pt");
    declareProperty( "slope",        m_slope=9.779);
    declareProperty( "intercept",    m_intercept=-32.28);
    declareProperty( "UseJetVars",   m_mode = 0); // 0 uses the tracks. 1 uses variables from the jets

    declareProperty( "Tagger", m_taggername = "ntrack");
    m_calibArea = "BoostedJetTaggers/QGTagger/May2019/"; // Overwrite base class default
    declareProperty( "TopoWeightFile", m_topofile = "");
    declareProperty( "ExpWeightFile", m_expfile = "qgsyst_exp.root");
    declareProperty( "MEWeightFile",  m_mefile  = "qgsyst_me.root");
    declareProperty( "PDFWeightFile", m_pdffile = "qgsyst_pdf.root");
    declareProperty( "TrackEffFile", m_trackefffile = "track_systs.root");//REPLACE when file available
    declareProperty( "FakeFile", m_fakefile = "track_systs.root");//REPLACE when file available
    declareProperty( "MinPt", m_jetPtMin = 50e3);
    declareProperty( "MaxEta", m_jetEtaMax = 2.1);
    declareProperty( "WeightDecorationName", m_weight_decoration_name = "qgTaggerWeight");
    declareProperty( "TaggerDecorationName", m_tagger_decoration_name = "qgTagger");


    applySystematicVariation(SystematicSet()).ignore();
  
  }

  StatusCode JetQGTagger::initialize() {

    ATH_MSG_INFO( "Initializing QuarkGluonTagger tool" );

    if( ! m_configFile.empty() ) {
      ATH_MSG_INFO( "Using config file : "<< m_configFile );
      // check for the existence of the configuration file
      std::string configPath;
      configPath = PathResolverFindDataFile(("BoostedJetTaggers/"+m_configFile).c_str());
      FileStat_t fStats;
      int fSuccess = gSystem->GetPathInfo(configPath.c_str(), fStats);

      if ( fSuccess ){
        ATH_MSG_ERROR( "Recommendations file " << m_configFile << " could not be found");
        return StatusCode::FAILURE;
      }
      else {
        ATH_MSG_DEBUG( "Recommendations file was found : " << configPath );
      }

      TEnv configReader;
      if(configReader.ReadFile( configPath.c_str(), EEnvLevel(0) ) != 0 ) {
        ATH_MSG_ERROR( "Error while reading config file : "<< configPath );
        return StatusCode::FAILURE;
      }

      // read in the specified track cut in the config file
      m_NTrackCut=configReader.GetValue("NTrackCut" ,-1);

      ATH_MSG_VERBOSE( "NTrackCut by config file : "<<m_NTrackCut );

    }
    else {
      // no config file
      // Assume the cut functions have been set through properties.
      // check they are non empty
      if( m_NTrackCut!=-1){
        ATH_MSG_VERBOSE( "NTrackCut by manual setting of property : "<<m_NTrackCut );
      }
      else {
        ATH_MSG_WARNING( "No config file provided AND no NTrackCut specified." ) ;
      }
    }
    if(m_cuttype != "linear_pt" && m_cuttype != "threshold" && m_cuttype != "log_pt"){
	ATH_MSG_ERROR("Cuttype set to: " << m_cuttype );
	ATH_MSG_ERROR("Cuttype invalid. Must use 'linear_pt', 'log_pt', or 'threshold'");
	return StatusCode::FAILURE;	
    }
 
    // decorators used to store
    // 1) ntracks
    // 2) tagger weight
    ATH_MSG_INFO( "Decorators that will be attached to jet :" );
    ATH_MSG_INFO( "  " << m_tagger_decoration_name << " : Number of tracks for tagging decision" );
    ATH_MSG_INFO( "  " << m_weight_decoration_name << " : Scale factor weight given the number of tracks" );
    
    m_decTagKey = m_containerName + "." + m_tagger_decoration_name;
    m_decWeightKey = m_containerName + "." + m_weight_decoration_name;

    ATH_CHECK( m_decTagKey.initialize() );
    ATH_CHECK( m_decWeightKey.initialize() );

    /// ReadDecorHandles for Ntrk variables
    m_readNumTrkPt500PVKey = m_containerName + "." + m_readNumTrkPt500PVKey.key();
    m_readNtrkKey = m_containerName + "." + m_readNtrkKey.key();

    ATH_CHECK( m_readNumTrkPt500PVKey.initialize() );
    ATH_CHECK( m_readNtrkKey.initialize() );

    // set up InDet selection tool
    ANA_CHECK( ASG_MAKE_ANA_TOOL( m_trkSelectionTool,  InDet::InDetTrackSelectionTool ) );
    ANA_CHECK( m_trkSelectionTool.setProperty( "CutLevel", "Loose" ) );
    ANA_CHECK( m_trkSelectionTool.retrieve() );

    // set up InDet truth track selection tools
    ANA_CHECK( ASG_MAKE_ANA_TOOL( m_trkTruthFilterTool, InDet::InDetTrackTruthFilterTool ) );
    ANA_CHECK( ASG_MAKE_ANA_TOOL( m_trkFakeTool, InDet::InDetTrackTruthFilterTool ) );

    ANA_CHECK( ASG_MAKE_ANA_TOOL( m_originTool, InDet::InDetTrackTruthOriginTool ) );
    ANA_CHECK( m_originTool.retrieve() );

    ANA_CHECK( m_trkTruthFilterTool.setProperty( "Seed", 1234 ) );
    ANA_CHECK( m_trkTruthFilterTool.setProperty( "trackOriginTool", m_originTool ) );
    ANA_CHECK( m_trkTruthFilterTool.retrieve() );
    CP::SystematicSet systSetTrk = {
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_GLOBAL],
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_IBL],
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_PP0],
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_PHYSMODEL]
    };
    ANA_CHECK( m_trkTruthFilterTool->applySystematicVariation(systSetTrk) );

    // set up tools used for systematic variations of tracks
    ANA_CHECK( m_trkFakeTool.setProperty( "Seed", 1234 ) );
    ANA_CHECK( m_trkFakeTool.setProperty( "trackOriginTool", m_originTool ) );
    ANA_CHECK( m_trkFakeTool.retrieve() );
    CP::SystematicSet systSetTrkFake = {
      InDet::TrackSystematicMap[InDet::TRK_FAKE_RATE_LOOSE]
    };
    ANA_CHECK( m_trkFakeTool->applySystematicVariation(systSetTrkFake) );

    ANA_CHECK( ASG_MAKE_ANA_TOOL( m_jetTrackFilterTool, InDet::JetTrackFilterTool ) );
    ANA_CHECK( m_jetTrackFilterTool.setProperty( "Seed", 1234 ) );
    ANA_CHECK( m_jetTrackFilterTool.setProperty( "trackOriginTool", m_originTool ) ); 
    ANA_CHECK( m_jetTrackFilterTool.retrieve() );
    CP::SystematicSet systSetJet = {
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_TIDE]
    };
    ANA_CHECK( m_jetTrackFilterTool->applySystematicVariation(systSetJet) );

    // specify systematic variations relevant for this tool
    if (!addAffectingSystematic(QGntrackSyst::trackfakes,true) ||
        !addAffectingSystematic(QGntrackSyst::trackefficiency,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedtopo,false /*for topology differences */) ||
        !addAffectingSystematic(QGntrackSyst::nchargedexp_up,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedme_up,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedpdf_up,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedexp_down,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedme_down,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedpdf_down,true) ||
        !addAffectingSystematic(QGntrackSyst::trackeff,true)||
        !addAffectingSystematic(QGntrackSyst::fake,true)
      )
    {
      ATH_MSG_ERROR("failed to set up JetQGTagger systematics");
      return StatusCode::FAILURE;
    }

    // load in the histograms that store the ntrack systematics
    if(m_topofile!="")//load topology file only if explicitly configured (default is "")
    ANA_CHECK( this->loadHist(m_topo_hquark,    m_topofile,"h2dquark") );
    ANA_CHECK( this->loadHist(m_exp_hquark_up,  m_expfile,"h2dquark_up")  );
    ANA_CHECK( this->loadHist(m_exp_hquark_down,m_expfile,"h2dquark_down"));
    ANA_CHECK( this->loadHist(m_exp_hgluon_up,  m_expfile,"h2dgluon_up")  );
    ANA_CHECK( this->loadHist(m_exp_hgluon_down,m_expfile,"h2dgluon_down"));
    ANA_CHECK( this->loadHist(m_me_hquark_up,   m_mefile, "h2dquark_up")  );
    ANA_CHECK( this->loadHist(m_me_hquark_down, m_mefile, "h2dquark_down"));
    ANA_CHECK( this->loadHist(m_me_hgluon_up,   m_mefile, "h2dgluon_up")  );
    ANA_CHECK( this->loadHist(m_me_hgluon_down, m_mefile, "h2dgluon_down"));
    ANA_CHECK( this->loadHist(m_pdf_hquark_up,  m_pdffile,"h2dquark_up")  );
    ANA_CHECK( this->loadHist(m_pdf_hquark_down,m_pdffile,"h2dquark_down"));
    ANA_CHECK( this->loadHist(m_pdf_hgluon_up,  m_pdffile,"h2dgluon_up")  );
    ANA_CHECK( this->loadHist(m_pdf_hgluon_down,m_pdffile,"h2dgluon_down"));
    ATH_MSG_INFO("about to load track syst histos");
    ATH_MSG_INFO("trackeff file: " << m_trackefffile);
    ANA_CHECK( this->loadHist(m_trackeff_hquark,m_trackefffile,"track_syste_quark"));//REPLACE w/ right histo
    ANA_CHECK( this->loadHist(m_trackeff_hgluon,m_trackefffile,"track_syste_gluon"));//REPLACE w/ right histo
    ANA_CHECK( this->loadHist(m_fake_hquark,m_fakefile,"track_systf_quark"));//REPLACE w/ right histo
    ANA_CHECK( this->loadHist(m_fake_hgluon,m_fakefile,"track_systf_gluon"));//REPLACE w/ right histo

    ATH_MSG_INFO( ": JetQGTagger tool initialized" );
    ATH_MSG_INFO( "  NTrackCut   : "<< m_NTrackCut );

    /// Initialize the tagger states
    m_acceptInfo.addCut( "QuarkJetTag", "True if the jet is deemed a quark jet because NTrack<NCut, False if jet deemed gluon jet because NTrack<NCut" );

    /// Call base class initialize
    ATH_CHECK( JSSTaggerBase::initialize() );
    
    /// Loop over and print out the cuts that have been configured
    printCuts();

    return StatusCode::SUCCESS;
  
  }

  JetQGTagger::~JetQGTagger(){

    delete m_topo_hquark;
    delete m_exp_hquark_up;
    delete m_exp_hquark_down;
    delete m_exp_hgluon_up;
    delete m_exp_hgluon_down;
    delete m_me_hquark_up;
    delete m_me_hquark_down;
    delete m_me_hgluon_up;
    delete m_me_hgluon_down;
    delete m_pdf_hquark_up;
    delete m_pdf_hquark_down;
    delete m_pdf_hgluon_up;
    delete m_pdf_hgluon_down;
    delete m_trackeff_hquark;
    delete m_trackeff_hgluon;
    delete m_fake_hquark;
    delete m_fake_hgluon;

  }

  StatusCode JetQGTagger::tag( const xAOD::Jet& jet, const xAOD::Vertex * pv ) const {

    ATH_MSG_DEBUG( "Obtaining QG result" );

    double jetWeight = -1;
    int    jetNTrack = -1;

    /// Create asg::AcceptData object
    asg::AcceptData acceptData( &m_acceptInfo );

    /// Reset the AcceptData cut results
    ATH_CHECK( resetCuts( acceptData ) );

    /// Check basic kinematic selection
    ATH_CHECK( checkKinRange( jet, acceptData ) );

    /// Create WriteDecorHandles
    SG::WriteDecorHandle<xAOD::JetContainer, bool> decTagged(m_decTaggedKey);
    SG::WriteDecorHandle<xAOD::JetContainer, bool> decValidEventContent(m_decValidEventContentKey);

    /// If the jet isn't valid there's no point applying the remaining cuts
    /// TODO: Is this actually needed?
    if ( !passKinRange(jet) ) {
      decTagged(jet) = false;
      return StatusCode::SUCCESS;
    }

    if ( m_mode == 0 ) { //do tagging assuming relevant track particle, PV, etc containers exist
      bool isValid = true;
      if ( pv ) ATH_MSG_DEBUG( "Obtaining JetQGTagger decision with user specific primary vertex" );
      else ATH_MSG_DEBUG( "Obtaining JetQGTagger decision default" );

      // if no primary vertex is specified, then the 0th primary vertex is used
      if ( !pv ) {
        const xAOD::VertexContainer* vxCont = nullptr;
        if ( evtStore()->retrieve( vxCont, "PrimaryVertices" ).isFailure() ) {
          ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
          acceptData.setCutResult("ValidEventContent", false);
          isValid = false;
        }
        else if ( vxCont->empty() ) {
          ATH_MSG_WARNING("Event has no primary vertices!");
          acceptData.setCutResult("ValidEventContent", false);
          isValid = false;
        }
        else {
          for ( const auto& vx : *vxCont ) {
            // take the first vertex in the list that is a primary vertex
            if ( vx->vertexType()==xAOD::VxType::PriVtx ) {
              pv = vx;
              break;
            }
          }
        }
        // Now we have to make sure that we did ID one as PV
        // I think this can happen in physics events (though they've got to be removed in order to perform a lot of calibrations)
        // so I've elected to not spit out a warning message here
        if ( !pv ) {
          acceptData.setCutResult("ValidEventContent", false);
          isValid = false;
        }
      }

      // If the object isn't valid there's no point applying the remaining cuts
      if ( !isValid ) return StatusCode::SUCCESS;

      // obtain the relevant information for tagging
      // 1) the number of tracks
      // 2) jet-by-jet event weight  
      ATH_CHECK( getNTrack(&jet, /*pv,*/ jetNTrack) );
      ATH_CHECK( getNTrackWeight(&jet, jetWeight) );
    
    }

    if ( m_mode == 1 ) { //only calculating uncertainty using given jet info (nTrk already calculated, etc)
      ATH_CHECK( simplegetNTrackWeight(&jet, jetWeight) );
      SG::ReadDecorHandle<xAOD::JetContainer, int> readNumTrkPt500PV(m_readNumTrkPt500PVKey);
      SG::ReadDecorHandle<xAOD::JetContainer, int> readNtrk(m_readNtrkKey);
      if ( readNtrk.isAvailable() ) jetNTrack = readNtrk(jet);
      else if ( readNumTrkPt500PV.isAvailable() ) jetNTrack = readNumTrkPt500PV(jet);
      else { 
        ATH_MSG_ERROR("Neither NumTrkPt500PV nor DFCommonJets_QGTagger_NTracks is available for your jet. Please add it before running in mode 1 of the JetQGTagger.");
        return StatusCode::FAILURE;
      }

      // decorate the cut value if specified
      SG::WriteDecorHandle<xAOD::JetContainer, float> decWeight(m_decWeightKey);
      decWeight(jet) = jetWeight;
    }

    decValidEventContent(jet) = acceptData.getCutResult( "ValidEventContent" );

    // decorate the cut value if specified
    SG::WriteDecorHandle<xAOD::JetContainer, float> decTag(m_decTagKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> decWeight(m_decWeightKey);

    decTag(jet) = jetNTrack;
    decWeight(jet) = jetWeight;

    // fill the AcceptData
    ATH_MSG_DEBUG("NTrack       = "<<jetNTrack);
    ATH_MSG_DEBUG("NTrackWeight = "<<jetWeight);
    double variable_nTrk = -999.0;
    if (m_cuttype=="linear_pt"){
      variable_nTrk=(m_slope*jet.pt())+m_intercept;
      if(jetNTrack<variable_nTrk) acceptData.setCutResult("QuarkJetTag", true);
    }
    else if (m_cuttype=="log_pt"){
      variable_nTrk=(m_slope*TMath::Log10(jet.pt()))+m_intercept;
      if(jetNTrack<variable_nTrk) acceptData.setCutResult("QuarkJetTag", true);
    }
    else if(m_cuttype=="threshold" && jetNTrack<m_NTrackCut) acceptData.setCutResult("QuarkJetTag", true);

    decTagged(jet) = acceptData.getCutResult( "QuarkJetTag" );

    return StatusCode::SUCCESS;

  }

  StatusCode JetQGTagger::simplegetNTrackWeight(const xAOD::Jet * jet, double &weight) const {

    ATH_MSG_DEBUG( "Getting the jet weight for systematic variation " << m_appliedSystEnum );

    // initially set the weight to unity
    // this is the weight returned if you are *not* dealing with a systematic variation
    weight = 1.0;
    ATH_MSG_DEBUG("Getting the jet weight for systematic variation " << m_appliedSystEnum);
    ATH_MSG_DEBUG("made it into simplegetntrk");

    // if you are not dealing with a systematic variation, then exit
    if ( m_appliedSystEnum!=QG_NCHARGEDEXP_UP &&
        m_appliedSystEnum!=QG_NCHARGEDME_UP &&
        m_appliedSystEnum!=QG_NCHARGEDPDF_UP &&
        m_appliedSystEnum!=QG_NCHARGEDEXP_DOWN &&
        m_appliedSystEnum!=QG_NCHARGEDME_DOWN &&
        m_appliedSystEnum!=QG_NCHARGEDPDF_DOWN &&
        m_appliedSystEnum!=QG_TRACKEFFICIENCY &&
        m_appliedSystEnum!=QG_TRACKFAKES
       )
    {
      return StatusCode::SUCCESS;
    }

    // use the lookup tables loaded in initialize() to find the systematically shifted weights
    bool truthsyst = m_appliedSystEnum==QG_NCHARGEDEXP_UP || m_appliedSystEnum==QG_NCHARGEDME_UP || m_appliedSystEnum==QG_NCHARGEDPDF_UP || m_appliedSystEnum == QG_NCHARGEDEXP_DOWN || m_appliedSystEnum== QG_NCHARGEDME_DOWN || m_appliedSystEnum == QG_NCHARGEDPDF_DOWN;
    bool recosyst = m_appliedSystEnum==QG_TRACKEFFICIENCY || m_appliedSystEnum == QG_TRACKFAKES;

    int ptbin, ntrkbin;
    int pdgid = jet->getAttribute<int>("PartonTruthLabelID");
    if (truthsyst){
      int tntrk = jet->getAttribute<int>("DFCommonJets_QGTagger_truthjet_nCharged");
      float tjetpt = jet->getAttribute<float>("DFCommonJets_QGTagger_truthjet_pt")*0.001;
      float tjeteta = jet->getAttribute<float>("DFCommonJets_QGTagger_truthjet_eta");
      ATH_MSG_DEBUG("truth jet pdgid: " << pdgid << " pt: " << tjetpt);
      if ( pdgid<0 ) {
        ATH_MSG_DEBUG("Undefined pdg ID: setting weight to 1");
        return StatusCode::SUCCESS;
      }

      // if the jet is outside of the measurement fiducial region
      // the systematic uncertainty is set to 0
      if ( tjetpt < m_jetPtMin*1e-3 || std::abs(tjeteta) > m_jetEtaMax ) {
        ATH_MSG_DEBUG( "Outside of fiducial region: setting weight to 1" );
        return StatusCode::SUCCESS;
      }

      if ( pdgid==21 && m_appliedSystEnum!=QG_NCHARGEDTOPO ) {
        ptbin = m_hgluon->GetXaxis()->FindBin(tjetpt);
        ntrkbin = m_hgluon->GetYaxis()->FindBin(tntrk);
        weight = m_hgluon->GetBinContent(ptbin,ntrkbin);
      }// gluon
      else if ( pdgid < 5 && m_appliedSystEnum != QG_NCHARGEDTOPO && m_appliedSystEnum != QG_TRACKEFFICIENCY && m_appliedSystEnum != QG_TRACKFAKES ) {
        ptbin = m_hquark->GetXaxis()->FindBin(tjetpt);
        ntrkbin = m_hquark->GetYaxis()->FindBin(tntrk);
        weight = m_hquark->GetBinContent(ptbin,ntrkbin);
      }//quarks
      else {
        ATH_MSG_INFO( "Neither quark nor gluon jet: setting weight to 1" );
      }
    }

    // check if jet contains at least one NTracks variables
    // prefer to use DFCommonJets* version
    int ntrk = -1;
    if ( recosyst) {
      SG::ReadDecorHandle<xAOD::JetContainer, int> readNumTrkPt500PV(m_readNumTrkPt500PVKey);
      SG::ReadDecorHandle<xAOD::JetContainer, int> readNtrk(m_readNtrkKey);
      if ( readNtrk.isAvailable() ) ntrk = readNtrk(*jet);
      else if ( readNumTrkPt500PV.isAvailable() ) ntrk = readNumTrkPt500PV(*jet);
      else ATH_MSG_ERROR("Neither NumTrkPt500PV nor DFCommonJets_QGTagger_NTracks is available for your jet. Please add it before running mode 1 JetQGTagger.");
      //float rjetpt = jet->getAttribute<float>("truthjet_pt")*0.001;
      float rjetpt = jet->pt()*1e-3;
      float rjeteta = jet->eta();

      ATH_MSG_DEBUG("reco jet Pt: " << rjetpt << " eta: " << rjeteta);
      if( rjetpt<m_jetPtMin*1e-3 || std::abs(rjeteta)>m_jetEtaMax){
        ATH_MSG_DEBUG("Outside of fiducial region: setting weight to 1");
        return StatusCode::SUCCESS;
      }

      if ( pdgid < 5 ) {
        ptbin = m_hquark->GetXaxis()->FindBin(rjetpt);
        ntrkbin = m_hquark->GetYaxis()->FindBin(ntrk);
        weight = m_hquark->GetBinContent(ptbin,ntrkbin);
      }
      if ( pdgid == 21 ) {
        ptbin = m_hgluon->GetXaxis()->FindBin(rjetpt);
        ntrkbin = m_hgluon->GetYaxis()->FindBin(ntrk);
        weight = m_hgluon->GetBinContent(ptbin,ntrkbin);
      }
    }

    ATH_MSG_DEBUG("weight: " << weight);

    return StatusCode::SUCCESS;

  }

  StatusCode JetQGTagger::getNTrack(const xAOD::Jet * jet, /*const xAOD::Vertex * pv,*/ int &ntracks) const {

    ATH_MSG_DEBUG( "Counting the number of tracks in the jet" );

    ntracks = 0;
    // loop over the tracks associated to the jet of interest
    std::vector<const xAOD::IParticle*> jettracks;

    if(!jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack,jettracks)){
      ATH_MSG_ERROR("This jet has no associated objects, so it will not be tagged. Please check the jet collection you are using.");
      ntracks=999;
      //Returning failure as this jet has no associated objects and we do not want to wrongly classify it as a gluon or quark using tag(). 
      //Physics should be independent of skimming, which may have removed tracks.
      //So we are returning a failure, and throwing an exception. 
      return StatusCode::FAILURE;
    }

    for (size_t i = 0; i < jettracks.size(); i++) {

      const xAOD::TrackParticle* trk = static_cast<const xAOD::TrackParticle*>(jettracks[i]);

      if(!trk){
        ATH_MSG_ERROR("This jet has null tracks, so it will not be tagged. Please check the jet collection you are using.");
        ntracks=998;
        //Returning failure as this jet has null tracks and we do not want to wrongly classify it as a gluon or quark using tag(). 
        //Physics should be independent of skimming, which may have introduced null tracks.
        //So we are returning a failure, and throwing an exception. 
        return StatusCode::FAILURE;
      }

      // if you are applying a systematic variation then
      // FRANCESCO ADD COMMENT

      bool acceptSyst = true;

      if ( m_appliedSystEnum==QG_TRACKEFFICIENCY )
        acceptSyst = ( m_trkTruthFilterTool->accept(trk) && m_jetTrackFilterTool->accept(trk,jet) );
      else if ( m_appliedSystEnum==QG_TRACKFAKES )
        acceptSyst = m_trkFakeTool->accept(trk);

      if (!acceptSyst)
        continue;

      // only count tracks with selections
      // 1) pt>500 MeV
      // 2) accepted track from InDetTrackSelectionTool with CutLevel==Loose
      // 3) associated to primary vertex OR within 3mm of the primary vertex
      bool accept = (trk->pt()>500 &&
          m_trkSelectionTool->accept(*trk)
          // TODO: Implement alternative to TrackParticle::vertex()
          //&& (trk->vertex()==pv || (!trk->vertex() && std::abs((trk->z0()+trk->vz()-pv->z())*sin(trk->theta()))<3.))
          );
      if (!accept)
        continue;

      ntracks++;
    }

    return StatusCode::SUCCESS;
  }



  StatusCode JetQGTagger::getNTrackWeight(const xAOD::Jet * jet, double &weight) const {

    ATH_MSG_DEBUG( "Getting the jet weight for systematic variation " << m_appliedSystEnum );

    // initially set the weight to unity
    // this is the weight returned if you are *not* dealing with a systematic variation
    weight = 1.0;

    // if you are not dealing with a systematic variation, then exit
    if ( m_appliedSystEnum!=QG_NCHARGEDTOPO &&
        m_appliedSystEnum!=QG_NCHARGEDEXP_UP &&
        m_appliedSystEnum!=QG_NCHARGEDME_UP &&
        m_appliedSystEnum!=QG_NCHARGEDPDF_UP &&
        m_appliedSystEnum!=QG_NCHARGEDEXP_DOWN &&
        m_appliedSystEnum!=QG_NCHARGEDME_DOWN &&
        m_appliedSystEnum!=QG_NCHARGEDPDF_DOWN
       )
      return StatusCode::SUCCESS;

    int pdgid = jet->getAttribute<int>("PartonTruthLabelID");
    if ( pdgid<0 ) {
      ATH_MSG_DEBUG("Undefined pdg ID: setting weight to 1");
      return StatusCode::SUCCESS;
    }

    // getting the associated truth jet
    // FRANCESCO COMMENT
    const xAOD::Jet* tjet;
    if(jet->isAvailable< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink") ){
      ATH_MSG_DEBUG("Accessing GhostTruthAssociationLink: is available");
      if(jet->auxdata< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink").isValid() ){
        ATH_MSG_DEBUG("Accessing GhostTruthAssociationLink: is valid");
        ElementLink<xAOD::JetContainer> truthlink = jet->auxdata< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
        if(truthlink)
          tjet = * truthlink;
        else{
          ATH_MSG_WARNING("Cannot access truth: setting weight to 1");
          return StatusCode::SUCCESS;
        }//endelse NULL pointer
      }
      else {
        ATH_MSG_WARNING("Cannot access truth: setting weight to 1");
        return StatusCode::SUCCESS;
      } //endelse isValid
    } //endif isAvailable
    else {
      ATH_MSG_WARNING("Cannot access truth: setting weight to 1");
      return StatusCode::SUCCESS;
    }//endelse isAvailable

    // if the jet is outside of the measurement fiducial region
    // the systematic uncertainty is set to 0
    double tjetpt = tjet->pt()*0.001;
    double tjeteta = tjet->eta();
    if( tjetpt<m_jetPtMin*1.0e-3 || std::abs(tjeteta)>m_jetEtaMax){
      ATH_MSG_DEBUG("Outside of fiducial region: setting weight to 1");
      return StatusCode::SUCCESS;
    }

    // compute truth ntrk
    int tntrk = 0;
    for (size_t ind = 0; ind < tjet->numConstituents(); ind++) {
      const xAOD::TruthParticle *part = static_cast<const xAOD::TruthParticle*>(tjet->rawConstituent(ind));

      // dont count invalid truth particles
      if (!part) continue;
      // require the particle in the final state
      if( ! (part->status() == 1) ) continue;
      // require that the particle type (e.g. production type) be valid (e.g. not primaries)
      if ((part->barcode())>2e5) continue;
      // pt>500 MeV
      if( ! (part->pt()>500.) )  continue;
      // charged
      if( !(part->isCharged()) ) continue;
      // this seems redundant
      // FRANCESCO COMMENT
      double pt = part->pt();
      if( pt>500 ) tntrk++;

    }

    // use the lookup tables loaded in initialize() to find the systematically shifted weights
    if ( pdgid==21 && m_appliedSystEnum!=QG_NCHARGEDTOPO){
      int ptbin = m_hgluon->GetXaxis()->FindBin(tjetpt);
      int ntrkbin = m_hgluon->GetYaxis()->FindBin(tntrk);
      weight = m_hgluon->GetBinContent(ptbin,ntrkbin);
    }// gluon
    else if ( pdgid<5 ){
      int ptbin = m_hquark->GetXaxis()->FindBin(tjetpt);
      int ntrkbin = m_hquark->GetYaxis()->FindBin(tntrk);
      weight = m_hquark->GetBinContent(ptbin,ntrkbin);
    }//quarks
    else{
      ATH_MSG_DEBUG("Neither quark nor gluon jet: setting weight to 1");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetQGTagger::sysApplySystematicVariation(const SystematicSet& systSet){

    // FRANCESCO COMMENT
    ATH_MSG_DEBUG( "Applying systematic variation by weight" );

    // by default no systematics are applied
    m_appliedSystEnum = QG_NONE;

    if (systSet.size()==0) {
      ATH_MSG_DEBUG("No affecting systematics received.");
      return StatusCode::SUCCESS;
    }
    else if (systSet.size()>1) {
      ATH_MSG_WARNING("Tool does not support multiple systematics, returning unsupported" );
      return StatusCode::FAILURE;
    }
    SystematicVariation systVar = *systSet.begin();
    if (systVar == SystematicVariation(""))
      m_appliedSystEnum = QG_NONE;
    else if (systVar == QGntrackSyst::nchargedtopo){
      m_appliedSystEnum = QG_NCHARGEDTOPO;
      m_hquark=m_topo_hquark;
    }
    else if (systVar == QGntrackSyst::trackefficiency)
      m_appliedSystEnum = QG_TRACKEFFICIENCY;
    else if (systVar == QGntrackSyst::trackfakes)
      m_appliedSystEnum = QG_TRACKFAKES;
    else if (systVar == QGntrackSyst::nchargedexp_up){
      m_appliedSystEnum = QG_NCHARGEDEXP_UP;
      m_hquark=m_exp_hquark_up;
      m_hgluon=m_exp_hgluon_up;
    }
    else if (systVar == QGntrackSyst::nchargedme_up){
      m_appliedSystEnum = QG_NCHARGEDME_UP;
      m_hquark=m_me_hquark_up;
      m_hgluon=m_me_hgluon_up;
    }
    else if (systVar == QGntrackSyst::nchargedpdf_up){
      m_appliedSystEnum = QG_NCHARGEDPDF_UP;
      m_hquark=m_pdf_hquark_up;
      m_hgluon=m_pdf_hgluon_up;
    }
    else if (systVar == QGntrackSyst::nchargedexp_down){
      m_appliedSystEnum = QG_NCHARGEDEXP_DOWN;
      m_hquark=m_exp_hquark_down;
      m_hgluon=m_exp_hgluon_down;
    }
    else if (systVar == QGntrackSyst::nchargedme_down){
      m_appliedSystEnum = QG_NCHARGEDME_DOWN;
      m_hquark=m_me_hquark_down;
      m_hgluon=m_me_hgluon_down;
    }
    else if (systVar == QGntrackSyst::nchargedpdf_down){
      m_appliedSystEnum = QG_NCHARGEDPDF_DOWN;
      m_hquark=m_pdf_hquark_down;
      m_hgluon=m_pdf_hgluon_down;
    }
    else if (systVar == QGntrackSyst::trackeff){
      m_appliedSystEnum = QG_TRACKEFFICIENCY;
      m_hquark = m_trackeff_hquark;
      m_hgluon = m_trackeff_hgluon;
    }
    else if (systVar == QGntrackSyst::fake){
      m_appliedSystEnum = QG_TRACKFAKES;
      m_hquark = m_fake_hquark;
      m_hgluon = m_fake_hgluon;
    }

    else {
      ATH_MSG_WARNING("unsupported systematic applied");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("applied systematic is " << m_appliedSystEnum);
    return StatusCode::SUCCESS;
  }

  StatusCode JetQGTagger::loadHist(TH2D *&hist,std::string fname,std::string histname){

    std::string filename = PathResolverFindCalibFile( (m_calibArea+fname).c_str() );
    ATH_MSG_INFO("CALIB FILE: " << filename << " histo: " << histname);
    if (filename.empty()){
      ATH_MSG_ERROR( "Could NOT resolve file name " << fname );
      return StatusCode::FAILURE;
    }
    else{
      ATH_MSG_DEBUG( "Path found = " << filename );
    }
    TFile* infile = TFile::Open(filename.c_str());
    hist = dynamic_cast<TH2D*>(infile->Get(histname.c_str()));
    hist->SetDirectory(0);
    return StatusCode::SUCCESS;
  }


} /* namespace CP */
