/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JetQGTaggerBDT.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#define APP_NAME "JetQGTaggerBDT"

namespace CP {

  JetQGTaggerBDT::JetQGTaggerBDT( const std::string& name ) :
    JSSTaggerBase( name ),
    m_name(name),
    m_APP_NAME(APP_NAME),
    m_BDTmethod("BDT_method")
    {

      declareProperty( "ConfigFile",   m_configFile="");

      declareProperty( "JetPtMin",              m_jetPtMin = 20000.0);
      declareProperty( "JetPtMax",              m_jetPtMax = 1500000.0);
      declareProperty( "JetEtaMax",             m_jetEtaMax = 2.5);

      declareProperty( "CalibArea",      m_calibarea = "BoostedJetTaggers/JetQGTaggerBDT/Oct18/");
      declareProperty( "TMVAConfigFile", m_tmvaConfigFileName="TMVAClassification_BDTQGTagger_Oct18_BDT.weights.xml");


  }

  JetQGTaggerBDT::~JetQGTaggerBDT() {}


  StatusCode JetQGTaggerBDT::initialize(){
    /* Initialize the BDT tagger tool */
    ATH_MSG_INFO( (m_APP_NAME+": Initializing JetQGTaggerBDT tool").c_str() );
    ATH_MSG_INFO( (m_APP_NAME+": Using config file :"+m_configFile).c_str() );

    // initialize decorators as decorationName+_decorator
    ATH_MSG_INFO( "Decorators that will be attached to jet :" );


    if( ! m_configFile.empty() ) {
      ATH_MSG_INFO( "Using config file : "<< m_configFile );
      // check for the existence of the configuration file
      std::string configPath;
      configPath = PathResolverFindDataFile(("BoostedJetTaggers/"+m_configFile).c_str());

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

      // get the CVMFS calib area where stuff is stored
      m_calibarea = configReader.GetValue("CalibArea" ,"");

      // get the name/path of the JSON config
      m_tmvaConfigFileName = configReader.GetValue("TMVAConfigFile" ,"");

      m_strScoreCut = configReader.GetValue("ScoreCut" ,"");

      ATH_MSG_INFO( "scoreCut: "<<m_strScoreCut );

    }
    // if the calibarea is specified to be "Local" then it looks in the same place as the top level configs
    if( m_calibarea.empty() ){
      ATH_MSG_INFO( (m_APP_NAME+": You need to specify where the calibarea is as either being Local or on CVMFS") );
      return StatusCode::FAILURE;
    }
    else if(m_calibarea.compare("Local")==0){
      std::string localCalibArea = "BoostedJetTaggers/share/JetQGTaggerBDT/";
      ATH_MSG_INFO( (m_APP_NAME+": Using Local calibarea "+localCalibArea));
      // convert the JSON config file name to the full path
      m_tmvaConfigFilePath = PathResolverFindCalibFile(localCalibArea+m_tmvaConfigFileName);
    }
    else{
      ATH_MSG_INFO( (m_APP_NAME+": Using CVMFS calibarea") );
      // get the config file from CVMFS
      // necessary because xml files are too large to house on the data space
      m_tmvaConfigFilePath = PathResolverFindCalibFile( (m_calibarea+m_tmvaConfigFileName).c_str() );
    }

    //transform string to TF1
    if(m_strScoreCut.empty()){
      ATH_MSG_ERROR( "Score cut function is empty!" );
      return StatusCode::FAILURE;
    }
    else{
      m_funcScoreCut = std::unique_ptr<TF1> (new TF1("strScoreCut",    m_strScoreCut.c_str(),    0, 14000));
    }
    // set up InDet selection tool
    ANA_CHECK( ASG_MAKE_ANA_TOOL( m_trkSelectionTool,  InDet::InDetTrackSelectionTool ) );
    ANA_CHECK( m_trkSelectionTool.setProperty( "CutLevel", "Loose" ) );
    ANA_CHECK( m_trkSelectionTool.retrieve() );

    // read json file for DNN weights
    ATH_MSG_INFO( (m_APP_NAME+": BDT Tagger configured with: "+m_tmvaConfigFilePath.c_str() ));

    // -- Initialize TMVA for BDTs
    TMVA::Tools::Instance();
    m_bdtTagger = std::unique_ptr<TMVA::Reader> ( new TMVA::Reader( "!Color:!Silent" ) );

    m_bdtTagger->AddVariable( "NTracks", &m_ntracks);
    m_bdtTagger->AddVariable( "TrackWidth", &m_trackwidth  );
    m_bdtTagger->AddVariable( "JetPt",  &m_pt );
    m_bdtTagger->AddVariable( "JetEta", &m_eta  );
    m_bdtTagger->AddVariable( "TrackC1", &m_trackC1  );

    // configure the bdt
    m_bdtTagger->BookMVA( m_BDTmethod.c_str(), m_tmvaConfigFilePath.c_str() );

    //setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
    m_accept.addCut( "ValidEventContent"   , "True if the event is alright technically (e.g. primary vertices)"        );
    m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
    m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
    m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );

    ATH_MSG_INFO( "BDT Tagger tool initialized" );

    return StatusCode::SUCCESS;
  }


  Root::TAccept JetQGTaggerBDT::tag(const xAOD::Jet& jet) const {
    // helpful execute message
    ATH_MSG_DEBUG("Obtaining BDT result" );

    //clear all accept values
    m_accept.clear();

    // set the jet validity bits to 1 by default
    m_accept.setCutResult( "ValidPtRangeHigh", true);
    m_accept.setCutResult( "ValidPtRangeLow" , true);
    m_accept.setCutResult( "ValidEtaRange"   , true);
    bool isValid = true;

    if (jet.pt() < m_jetPtMin) {
      ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
      m_accept.setCutResult("ValidPtRangeLow", false);
      isValid = false;
    }
    if (jet.pt() > m_jetPtMax) {
      ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
      m_accept.setCutResult("ValidPtRangeHigh", false);
      isValid = false;
    }
    if (TMath::Abs(jet.eta()) > m_jetEtaMax) {
      ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| > " << m_jetEtaMax<< "). Jet eta = " << jet.eta());
      m_accept.setCutResult("ValidEtaRange", false);
      isValid = false;
    }

    if (!isValid)
      return m_accept;

    // get BDT score
    float jet_score = getScore(jet);
    ATH_MSG_DEBUG(TString::Format("jet score %g",jet_score) );

    //get cut from cut function
    float cut = m_funcScoreCut->Eval(jet.pt()/1000.);

    if(jet_score < cut) m_accept.setCutResult("QuarkJetTag", true);

    // return the TAccept object that you created and filled
    return m_accept;
  }


  float JetQGTaggerBDT::getScore(const xAOD::Jet& jet) const{

    // load the new values of the variables for this jet
    getJetProperties(jet);

    // evaluate bdt
    float bdt_score(-666.);
    if(m_undefInput){
      ATH_MSG_WARNING("One (or more) tagger input variable has an undefined value (NaN), setting score to -666");
      return bdt_score;
    }
    bdt_score = m_bdtTagger->EvaluateMVA( m_BDTmethod.c_str() );

    return bdt_score;
  }

  void JetQGTaggerBDT::getJetProperties(const xAOD::Jet& jet) const{
    /* Update the jet substructure variables for this jet */

    m_pt   = jet.pt()/1000.0;
    m_eta   = jet.eta();

    ATH_MSG_DEBUG(TString::Format("pT: %g, eta: %g",m_pt,m_eta) );

    m_undefInput = false;
    bool undefNTracks = false;
    bool undefTrackWidth = false;

    const xAOD::Vertex* primvertex {nullptr};
    bool isValid = true;

    const xAOD::VertexContainer* vxCont = 0;
    if(evtStore()->retrieve( vxCont, "PrimaryVertices" ).isFailure()){
      ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
      m_accept.setCutResult("ValidEventContent", false);
      isValid = false;
    }
    else if(vxCont->empty()){
      ATH_MSG_WARNING("Event has no primary vertices!");
      m_accept.setCutResult("ValidEventContent", false);
      isValid = false;
    }
    else{
      for(const auto& vx : *vxCont){
        // take the first vertex in the list that is a primary vertex
        if(vx->vertexType()==xAOD::VxType::PriVtx){
          primvertex = vx;
          break;
        }
      }
    }
    if(!primvertex) isValid = false;

    if (!isValid)
      return;

    //NTracks
    std::vector<int> nTrkVec;
    if(jet.getAttribute(xAOD::JetAttribute::NumTrkPt500, nTrkVec)){
      ATH_MSG_DEBUG(nTrkVec.size());
      m_ntracks = (float) nTrkVec[primvertex->index()];
    }
    else
      undefNTracks = true;

    //TrackWidth
    std::vector<float> trkWidthVec;
    if(jet.getAttribute(xAOD::JetAttribute::TrackWidthPt500, trkWidthVec)){
      ATH_MSG_DEBUG(trkWidthVec.size());
      m_trackwidth = trkWidthVec[primvertex->index()];
    }
    else
      undefTrackWidth = true;
    float weightedwidth = 0.;

    //TrackC1
    float beta = 0.2;
    float weightedwidth2 = 0.;
    float sumPt = 0.;

    std::vector<const xAOD::TrackParticle*> trackParttmp;
    if(!jet.getAssociatedObjects("GhostTrack",trackParttmp)){
      ATH_MSG_ERROR("This jet has no associated objects");
      m_undefInput = true;
    }
    //track selection
    for(unsigned i=trackParttmp.size();i>0; i--){
      if(!trackParttmp[i-1]){
        trackParttmp.erase(trackParttmp.begin()+i-1);
        continue;
      }
      const xAOD::TrackParticle* trk = static_cast<const xAOD::TrackParticle*>(trackParttmp[i-1]);
      bool accept = (trk->pt()>500 &&
          m_trkSelectionTool->accept(*trk) &&
          (trk->vertex()==primvertex || (!trk->vertex() &&
              fabs((trk->z0()+trk->vz()-primvertex->z())*sin(trk->theta()))<3.))
      );
      if (!accept){
        trackParttmp.erase(trackParttmp.begin()+i-1);
      }
    }

    //calculate TrackC1 (and TrackWidth if necessary)
    for(unsigned i=0; i<trackParttmp.size(); i++){
      double ipt = trackParttmp.at(i)->pt();
      double ieta = trackParttmp.at(i)->eta();
      double iphi = trackParttmp.at(i)->phi();
      sumPt += ipt;
      if(undefTrackWidth){
        double deta_i = trackParttmp.at(i)->eta() - jet.eta();
        double dphi_i = TVector2::Phi_mpi_pi(trackParttmp.at(i)->phi() - jet.phi());
        double dR_i = sqrt( deta_i*deta_i + dphi_i*dphi_i );
        weightedwidth += ipt * dR_i;
      }

      for(unsigned j=i+1; j<trackParttmp.size(); j++){
        double deta = ieta - trackParttmp.at(j)->eta();
        double dphi = TVector2::Phi_mpi_pi(iphi - trackParttmp.at(j)->phi());
        double dR = sqrt( deta*deta + dphi*dphi );
        weightedwidth2 += ipt * trackParttmp.at(j)->pt() * pow(dR,beta);
      }
    }

    if(undefNTracks)
      m_ntracks = trackParttmp.size();
    if(undefTrackWidth)
      m_trackwidth = sumPt>0 ? weightedwidth/sumPt : -0.1;
    m_trackC1 = sumPt>0 ? weightedwidth2/(sumPt*sumPt) : -0.1;

    //    ATH_MSG_DEBUG(TString::Format("n_trk: %g, w_trk: %g, C_1,trk: %g, pT: %g, eta: %g",m_ntracks,m_trackwidth,m_trackC1,m_pt,m_eta) );

    return;
  }

  StatusCode JetQGTaggerBDT::finalize(){
      /* Delete or clear anything */
      return StatusCode::SUCCESS;
  }

} /* namespace CP */

// the end
