/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include <cmath>

namespace CP {

  MuonCalibrationAndSmearingTool::MuonCalibrationAndSmearingTool( const std::string& name ) :
    asg::AsgTool( name ),
    m_useExternalSeed(false),
    m_externalSeed(0),
    m_Tsmear( 0 ),
    m_Tdata( 0 ),
    m_Trel( 0 ),
    m_Talgo( 0 ),
    m_useNsigmaForICombine( 0. ),
    m_loadNames( false ),
    m_nb_regions( 0. ),
    m_doMacroRegions( false ),
    m_StatCombPtThreshold(300.00),
    m_useStatComb(false),
    m_SagittaCorrPhaseSpace(false),
    m_doSagittaCorrection(false),
    m_doSagittaMCDistortion(false),
    m_SagittaRelease("sagittaBiasDataAll_02_08_17"){

    declareProperty("Year", m_year = "Data16" );
    declareProperty("Algo", m_algo = "muons" );
    declareProperty("SmearingType", m_type = "q_pT" );
    declareProperty("Release", m_release = "Recs2016_15_07" );
    declareProperty("ToroidOff", m_toroidOff = false );
    declareProperty("FilesPath", m_FilesPath = "" );
    declareProperty("StatComb", m_useStatComb = false);
    declareProperty("MinCombPt", m_StatCombPtThreshold=300.0);
    declareProperty("SagittaCorr", m_doSagittaCorrection = false);
    declareProperty("SagittaRelease", m_SagittaRelease = "sagittaBiasDataAll_25_07_17");
    declareProperty("doSagittaMCDistortion",m_doSagittaMCDistortion=false);
    declareProperty("SagittaCorrPhaseSpace",m_SagittaCorrPhaseSpace=false);
    declareProperty("sgItersCB",m_sgItersCB=11);
    declareProperty("sgItersID",m_sgItersID=11);
    declareProperty("sgItersME",m_sgItersME=11);
    declareProperty("sgIetrsMamual",m_sgIetrsMamual=false);
    declareProperty("fixedRho",m_fixedRho=1.0);
    declareProperty("useFixedRho",m_useFixedRho=false);
    declareProperty("noEigenDecor" ,m_doNotUseAMGMATRIXDECOR=false);
    declareProperty("useExternalSeed" ,m_useExternalSeed=false);
    declareProperty("externalSeed" ,m_externalSeed=0);

    m_currentParameters = NULL;
    m_SagittaIterations.push_back(0);
    m_SagittaIterations.push_back(0);
    m_SagittaIterations.push_back(0);
    m_sagittasCB=new std::vector <TProfile2D*>();
    m_sagittasID=new  std::vector <TProfile2D*>();
    m_sagittasME=new std::vector <TProfile2D*>();
    m_sagittaPhaseSpaceCB=NULL;
    m_sagittaPhaseSpaceID=NULL;
    m_sagittaPhaseSpaceME=NULL;
  }

  MuonCalibrationAndSmearingTool::MuonCalibrationAndSmearingTool( const MuonCalibrationAndSmearingTool& tool ) :
    asg::AsgTool( tool.name() + "Copy" ),

    m_useExternalSeed( tool.m_useExternalSeed ),
    m_externalSeed( tool.m_externalSeed ),

    m_Tsmear( tool.m_Tsmear ),
    m_Tdata( tool.m_Tdata ),
    m_Trel( tool.m_Trel ),
    m_Talgo( tool.m_Talgo ),
    m_useNsigmaForICombine( tool.m_useNsigmaForICombine ),

    m_scale_ID( tool.m_scale_ID ),
    m_enLoss_MS( tool.m_enLoss_MS ),
    m_scale_MS( tool.m_scale_MS ),
    m_scale_CB( tool.m_scale_CB ),

    m_scaleSyst_ID( tool.m_scaleSyst_ID ),
    m_enLossSyst_MS( tool.m_enLossSyst_MS ),
    m_scaleSyst_MS( tool.m_scaleSyst_MS ),
    m_scaleSyst_CB( tool.m_scaleSyst_CB ),

    m_scaleSystUp_ID( tool.m_scaleSystUp_ID ),
    m_enLossSystUp_MS( tool.m_enLossSystUp_MS ),
    m_scaleSystUp_MS( tool.m_scaleSystUp_MS ),

    m_scaleSystDw_ID( tool.m_scaleSystDw_ID ),
    m_enLossSystDw_MS( tool.m_enLossSystDw_MS ),
    m_scaleSystDw_MS( tool.m_scaleSystDw_MS ),

    m_p1_ID( tool.m_p1_ID ),
    m_p2_ID( tool.m_p2_ID ),
    m_p2_ID_TAN( tool.m_p2_ID_TAN ),
    m_p0_MS( tool.m_p0_MS ),
    m_p1_MS( tool.m_p1_MS ),
    m_p2_MS( tool.m_p2_MS ),

    m_E_p1_ID( tool.m_E_p1_ID ),
    m_E_p2_ID( tool.m_E_p2_ID ),
    m_E_p2_ID_TAN( tool.m_E_p2_ID_TAN ),
    m_E_p0_MS( tool.m_E_p0_MS ),
    m_E_p1_MS( tool.m_E_p1_MS ),
    m_E_p2_MS( tool.m_E_p2_MS ),
    m_S_p1_ID( tool.m_S_p1_ID ),
    m_S_p2_ID( tool.m_S_p2_ID ),
    m_S_p2_ID_TAN( tool.m_S_p2_ID_TAN ),
    m_S_p0_MS( tool.m_S_p0_MS ),
    m_S_p1_MS( tool.m_S_p1_MS ),
    m_S_p2_MS( tool.m_S_p2_MS ),

    m_SUp_p1_ID( tool.m_SUp_p1_ID ),
    m_SUp_p2_ID( tool.m_SUp_p2_ID ),
    m_SUp_p2_ID_TAN( tool.m_SUp_p2_ID_TAN ),
    m_SUp_p0_MS( tool.m_SUp_p0_MS ),
    m_SUp_p1_MS( tool.m_SUp_p1_MS ),
    m_SUp_p2_MS( tool.m_SUp_p2_MS ),
    m_SDw_p1_ID( tool.m_SDw_p1_ID ),
    m_SDw_p2_ID( tool.m_SDw_p2_ID ),
    m_SDw_p2_ID_TAN( tool.m_SDw_p2_ID_TAN ),
    m_SDw_p0_MS( tool.m_SDw_p0_MS ),
    m_SDw_p1_MS( tool.m_SDw_p1_MS ),
    m_SDw_p2_MS( tool.m_SDw_p2_MS ),

    m_MC_p1_ID( tool.m_MC_p1_ID ),
    m_MC_p2_ID( tool.m_MC_p2_ID ),
    m_MC_p2_ID_TAN( tool.m_MC_p2_ID_TAN ),
    m_MC_p0_MS( tool.m_MC_p0_MS ),
    m_MC_p1_MS( tool.m_MC_p1_MS ),
    m_MC_p2_MS( tool.m_MC_p2_MS ),

    m_names( tool.m_names ),
    m_loadNames( tool.m_loadNames ),
    m_nb_regions( tool.m_nb_regions ),
    m_eta_min( tool.m_eta_min ),
    m_eta_max( tool.m_eta_max ),
    m_phi_min( tool.m_phi_min ),
    m_phi_max( tool.m_phi_max ),

    m_doMacroRegions( tool.m_doMacroRegions ),
    m_MacroRegionIdxMap( tool.m_MacroRegionIdxMap ),
    m_MacroRegionName( tool.m_MacroRegionName ),
    m_MacroRegionInnerEta( tool.m_MacroRegionInnerEta ),
    m_Parameters( tool.m_Parameters ),
    m_currentParameters( NULL),
    m_StatCombPtThreshold(tool.m_StatCombPtThreshold),
    m_useStatComb(tool.m_useStatComb),
    m_SagittaCorrPhaseSpace(tool.m_SagittaCorrPhaseSpace),
    m_doSagittaCorrection(tool.m_doSagittaCorrection),
    m_doNotUseAMGMATRIXDECOR(tool.m_doNotUseAMGMATRIXDECOR),
    m_SagittaIterations(tool.m_SagittaIterations),
    m_GlobalZScales(tool.m_GlobalZScales){
    declareProperty( "Year", m_year = "Data16" );
    declareProperty( "Algo", m_algo = "muons" );
    declareProperty( "SmearingType", m_type = "q_pT" );
    declareProperty( "Release", m_release = "Recs2016_15_07" );
    declareProperty( "ToroidOff", m_toroidOff = false );
    declareProperty( "FilesPath", m_FilesPath = "" );

  }


  MuonCalibrationAndSmearingTool::~MuonCalibrationAndSmearingTool(){
    if(m_sagittasCB!=NULL){
      for( auto t : *m_sagittasCB) delete t;
      delete m_sagittasCB;
    }

    if(m_sagittasID!=NULL){
      for( auto t : *m_sagittasID) delete t;
      delete m_sagittasID;
    }

    if(m_sagittasME!=NULL){
      for( auto t : *m_sagittasME) delete t;
      delete m_sagittasME;
    }

    if(m_sagittaPhaseSpaceCB!=NULL)
      delete m_sagittaPhaseSpaceCB;

    if(m_sagittaPhaseSpaceID!=NULL)
      delete m_sagittaPhaseSpaceID;

    if(m_sagittaPhaseSpaceME!=NULL)
      delete m_sagittaPhaseSpaceME;
  }


  StatusCode MuonCalibrationAndSmearingTool::initialize(){

    // Greet the user:
    ATH_MSG_INFO( "Initialising..." );

    if( SetData( m_year ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
    if( SetAlgorithm( m_algo ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
    if( SetRelease( m_release ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
    if( SetType( m_type ) == StatusCode::FAILURE ) return StatusCode::FAILURE;

    ATH_MSG_DEBUG( "Checking Initialization - Year: " << m_year );
    ATH_MSG_DEBUG( "Checking Initialization - Algo: " << m_algo );
    ATH_MSG_DEBUG( "Checking Initialization - Type: " << m_type );
    ATH_MSG_DEBUG( "Checking Initialization - Release: " << m_release );

    std::string regionsPath;
    int regionMode = 0; // simple Eta Bins
    // Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
    if ( m_FilesPath == "" ) {
      if ( m_Trel <= MCAST::Release::Rel17 ) {
        regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/Regions.dat" );
      }
      else if ( m_Trel == MCAST::Release::Rel17_2_Repro ) {
        regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsRepro.dat" );
      }
      else if ( m_Trel == MCAST::Release::Rel17_2_Sum13 ) {
        regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsPhi18.dat" );
        regionMode = 2;//MS: 48Bins L/S split plus 11_15 and Feet in the BARREL. ID has values from 16 eta bins
        m_useNsigmaForICombine = 0;
      }
      else if ( m_Trel >= MCAST::Release::PreRec ) {
        regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsPhi18.dat" );
        regionMode = 1;
        m_useNsigmaForICombine = 0;
      }

      //    else if ( m_Trel >= MCAST::Release::Recs2016_08_07 ) {
      //regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsPhi18.dat" );
      //regionMode = 1;
      //m_useNsigmaForICombine = 0;
      //}

      else {
        ATH_MSG_ERROR( "Unknown release" );
        return StatusCode::FAILURE;
      }
    }
    else {
      regionsPath = m_FilesPath + "RegionsPhi18.dat";
      regionMode = 1;
      m_useNsigmaForICombine = 0;
    }
    ATH_MSG_DEBUG( "Checking Initialization - Regions file: " << regionsPath );

    if( Regions( regionsPath, regionMode ) == StatusCode::FAILURE ) return StatusCode::FAILURE;

    if( FillValues() == StatusCode::FAILURE ) return StatusCode::FAILURE;

    if( !applySystematicVariation( SystematicSet() ) ) {
      ATH_MSG_ERROR( "Unable to run with no systematic" );
      return StatusCode::FAILURE;
    }
    SystematicRegistry& registry = SystematicRegistry::getInstance();
    if( registry.registerSystematics( *this ) != SystematicCode::Ok ) return StatusCode::FAILURE;


    if(m_Tdata == MCAST::DataType::Data15 ||  m_Tdata == MCAST::DataType::Data16 ){
      m_StatCombPtThreshold=300.0;
    }

    if(m_useStatComb){
      ATH_MSG_INFO("Using statistical combination abovbe "<<m_StatCombPtThreshold<<" GeV");
    }

    if(m_doSagittaCorrection){
      ATH_MSG_INFO("Will be correcting the sagitta for data ");
    }

    if(m_Tdata == MCAST::DataType::Data16 && ( m_doSagittaCorrection || m_doSagittaMCDistortion) ){
      m_GlobalZScales.clear();
      m_GlobalZScales.push_back(90.2893);  m_GlobalZScales.push_back(90.4996);    m_GlobalZScales.push_back(90.1407);

      m_SagittaIterations.clear();

      // First batch of corrections
      if(m_SagittaRelease.compare("sagittaBiasDataAll")==0){
        m_SagittaIterations.push_back(10); m_SagittaIterations.push_back(10); m_SagittaIterations.push_back(10);
      }
      // Second batch of corrections reduced uncertainty
      else if (m_SagittaRelease.compare("sagittaBiasDataAll_06_02_17")==0){
        m_SagittaIterations.push_back(11); m_SagittaIterations.push_back(11); m_SagittaIterations.push_back(11);
      }

      // R21 batch of corrections reduced uncertainty
      else if (m_SagittaRelease.compare("sagittaBiasDataAll_02_08_17")==0){
        m_SagittaIterations.push_back(11); m_SagittaIterations.push_back(11); m_SagittaIterations.push_back(11);
      }

      // R21 first batch of corrections reduced uncertainty
      else if (m_SagittaRelease.compare("sagittaBiasDataAll_25_07_17")==0){
        m_SagittaIterations.push_back(11); m_SagittaIterations.push_back(11); m_SagittaIterations.push_back(11);
      }

      else {
        ATH_MSG_WARNING("Unknown SagittaBiasRelease: Number of sagitta iterations set to 0");
        m_SagittaIterations.push_back(0); m_SagittaIterations.push_back(0); m_SagittaIterations.push_back(0);
      }

      if(m_sgIetrsMamual){
        ATH_MSG_INFO("Number of sagitta iterations set to manual CB "<<m_sgItersCB<<" ID "<<m_sgItersID<<" "<<m_sgItersME);
        m_SagittaIterations.clear();
        m_SagittaIterations.push_back(m_sgItersCB);
        m_SagittaIterations.push_back(m_sgItersID);
        m_SagittaIterations.push_back(m_sgItersME);
      }


      std::vector<std::string> trackNames; trackNames.push_back("CB"); trackNames.push_back("ID");  trackNames.push_back("ME");


      for( unsigned int i=0; i<m_SagittaIterations.size(); i++){
        ATH_MSG_VERBOSE("Case "<<i<<" track Name "<<trackNames.at(i)<<" and iterations "<<m_SagittaIterations.at(i));
        for( unsigned int j=0; j< m_SagittaIterations.at(i) ;  j++){
          ATH_MSG_VERBOSE("Track "<<i<<" file "<< PathResolverFindCalibFile(Form("MuonMomentumCorrections/%s/outqDeltamPlots_iter%d/",m_SagittaRelease.c_str(),j) + trackNames.at(i) + "_data.root"));
          MuonCalibrationAndSmearingTool::setSagittaHistogramsSingle(GetHist( PathResolverFindCalibFile(Form("MuonMomentumCorrections/%s/outqDeltamPlots_iter%d/",m_SagittaRelease.c_str(),j) + trackNames.at(i) + "_data.root"),"inclusive",m_GlobalZScales.at(i)),i);
        }
      }
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
  }



  void MuonCalibrationAndSmearingTool::ConvertToSagittaBias(TH2F *h,float mean){
    if(h==NULL)return ;
    ATH_MSG_VERBOSE("Will normalise w.r.t the scale of "<<mean);
    h->Scale(4/mean*1e3);
    h->GetZaxis()->SetTitle("#delta#it{s} [TeV^{-1}]");
    h->GetZaxis()->SetRangeUser(-1,+1);
  }

  TProfile2D* MuonCalibrationAndSmearingTool::GetHist(std::string fname, std::string hname,double GlobalScale){
    if( fname.size() == 0 || hname.size()==0 ) return NULL;

    ATH_MSG_INFO("Opening correction file : " <<fname);
    TFile *fmc=TFile::Open(fname.c_str());

    if(fmc==NULL) {
      ATH_MSG_ERROR("File not found");
      return NULL;
    }

    TH3F *h3=NULL;
    h3=(TH3F*)fmc->Get(hname.c_str());

    if( h3==NULL ){
      ATH_MSG_ERROR("NULL sagitta map");
      return NULL;
    }

    h3->SetDirectory(0);
    TProfile2D *hinclusive=(TProfile2D*)h3->Project3DProfile("yx");
    hinclusive->SetDirectory(0);
    hinclusive->GetXaxis()->SetTitle(h3->GetXaxis()->GetTitle());
    hinclusive->GetYaxis()->SetTitle(h3->GetYaxis()->GetTitle());
    hinclusive->GetZaxis()->SetTitle(h3->GetZaxis()->GetTitle());

    ConvertToSagittaBias((TH2F*)hinclusive,GlobalScale);

    delete h3;
    fmc->Close();
    return hinclusive;
  }


  // This may cause a leak
  unsigned int MuonCalibrationAndSmearingTool::setSagittaHistogramsSingle(TProfile2D *pCB,unsigned int track){
    if( track == 0 ) {
      pCB->SetName((Form("pCB_%d",(int)m_sagittasCB->size())));
      m_sagittasCB->push_back(pCB);
      return m_sagittasCB->size();
    }
    else if( track == 1 ) {
      pCB->SetName(Form("pID_%d",(int)m_sagittasID->size()));
      m_sagittasID->push_back(pCB);
      return m_sagittasID->size();
    }
    else if( track == 2 ) {
      pCB->SetName(Form("pME_%d",(int)m_sagittasME->size()));
      m_sagittasME->push_back(pCB);
      return  m_sagittasME->size();
    }
    else
      return 0;
  }

  double  MuonCalibrationAndSmearingTool::sagitta(TProfile2D* corrM, TLorentzVector &lv) const {
    if( corrM == NULL) return 0; ;
    int binEta=corrM->GetXaxis()->FindBin(lv.Eta());
    int binPhi=corrM->GetYaxis()->FindBin(lv.Phi());
    if(std::abs(lv.Eta()) >2.5) {
      ATH_MSG_DEBUG("Muon eta > 2.5 sagitta bias = 0");
      return 0.0;
    }
    double p2=corrM->GetBinContent(binEta,binPhi);

    if(m_currentParameters->SagittaBias != MCAST::SystVariation::Default){
      if(m_currentParameters->SagittaBias == MCAST::SystVariation::Up){
        p2=0.5*p2;
      }
      else if (m_currentParameters->SagittaBias == MCAST::SystVariation::Down){
        p2=-0.5*p2;
      }
    }
    return p2;
  }

  CorrectionCode MuonCalibrationAndSmearingTool::CorrectForCharge(double p2, double& pt, int q, bool isMC) const {

    if( q==0 ) {
      ATH_MSG_DEBUG("Muon charge is 0");
      return CorrectionCode::OutOfValidityRange;
    }
    double corrPt=pt;
    if(isMC)
      corrPt = corrPt/(1 - q*p2*1e-3*corrPt);
    else
      corrPt = corrPt/(1 + q*p2*1e-3*corrPt);
    pt=corrPt;
    return CorrectionCode::Ok;
  }

  CorrectionCode MuonCalibrationAndSmearingTool::applySagittaBiasCorrection(const unsigned int SgCorrType, xAOD::Muon& mu, unsigned int iter, bool stop, bool isMC, InfoHelper& muonInfo) const {

    if(m_doSagittaMCDistortion && isMC && iter == 0) stop=true;

    if(iter == 0){
      ATH_MSG_DEBUG("Sagitta correction method "<< SgCorrType <<" iter "<<iter);
      if( ( mu.primaryTrackParticleLink() ).isValid() ) {
        const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.primaryTrackParticleLink();
        muonInfo.ptcb = ( !cb_track ) ? 0. : ( *cb_track )->pt() / 1000.;
      }
      else muonInfo.ptcb = 0.;
      if( m_useStatComb && muonInfo.ptcb > m_StatCombPtThreshold && isBadMuon(mu, muonInfo)) {
        if(m_doNotUseAMGMATRIXDECOR){
          muonInfo.ptcb = sin(muonInfo.cbParsA[3])/fabs(muonInfo.cbParsA[4])/ 1000;
        }
        else {
          if(!mu.isAvailable < AmgVector(5) >( "StatCombCBPars" )) return CorrectionCode::Error;
          AmgVector(5) parsCB = mu.auxdata < AmgVector(5) >( "StatCombCBPars" );
          muonInfo.ptcb = sin(parsCB[3])/fabs(parsCB[4])/ 1000;
        }}

      if( ( mu.inDetTrackParticleLink() ).isValid() ) {
        const ElementLink< xAOD::TrackParticleContainer >& id_track = mu.inDetTrackParticleLink();
        muonInfo.ptid = ( !id_track ) ? 0. : ( *id_track )->pt() / 1000.;
      }
      else muonInfo.ptid = 0.;

      if( ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() ) {
        const ElementLink< xAOD::TrackParticleContainer >& ms_track = mu.extrapolatedMuonSpectrometerTrackParticleLink();
        muonInfo.ptms = ( !ms_track ) ? 0. : ( *ms_track )->pt() / 1000.;
      }
      else
        muonInfo.ptms = 0.;
    }

    if(muonInfo.ptid==0 && muonInfo.ptms ==0){
      ATH_MSG_DEBUG("ME and ID momenta == 0");
      return CorrectionCode::OutOfValidityRange;
    }

    muonInfo.eta = mu.eta();
    muonInfo.phi = mu.phi();


    ATH_MSG_VERBOSE("iter "<<iter<< " Muon info pt "<<muonInfo.ptcb<<" id "<<muonInfo.ptid<<" ms "<<muonInfo.ptms<<" eta "<<muonInfo.eta<<" phi "<<muonInfo.phi<<" charge "<< muonInfo.charge);


    TLorentzVector lvCB; lvCB.SetPtEtaPhiM(muonInfo.ptcb,muonInfo.eta,muonInfo.phi,mu.m()/1e3);
    TLorentzVector lvID; lvID.SetPtEtaPhiM(muonInfo.ptid,muonInfo.eta,muonInfo.phi,mu.m()/1e3);
    TLorentzVector lvME; lvME.SetPtEtaPhiM(muonInfo.ptms,muonInfo.eta,muonInfo.phi,mu.m()/1e3);

    int q=muonInfo.charge;

    if( q==0 ){
      ATH_MSG_DEBUG("Charge == 0");
      return CorrectionCode::OutOfValidityRange;
    }

    if(SgCorrType==MCAST::SagittaCorType::CB) {
      if(muonInfo.ptcb == 0 ) return CorrectionCode::Ok;
      if( iter >=  m_sagittasCB->size())  return CorrectionCode::Ok;
      CorrectionCode corr = CorrectForCharge( sagitta(m_sagittasCB->at(iter),lvCB)*0.5, muonInfo.ptcb, q, isMC);
      iter++;
      if(corr != CorrectionCode::Ok) return corr;
      if(!stop)  return applySagittaBiasCorrection(MCAST::SagittaCorType::CB, mu, iter, stop, isMC, muonInfo);
    }

    else if(SgCorrType == MCAST::SagittaCorType::ID){
      if(muonInfo.ptid == 0 ) return CorrectionCode::Ok;
      if( iter >= m_sagittasID->size())  return CorrectionCode::Ok;
      CorrectionCode corr = CorrectForCharge(  sagitta(m_sagittasID->at(iter),lvID)*0.5 , muonInfo.ptid, q, isMC);
      iter++;
      if(corr != CorrectionCode::Ok) return corr;
      if(!stop)return applySagittaBiasCorrection(MCAST::SagittaCorType::ID, mu, iter, stop, isMC, muonInfo);
    }

    else if(SgCorrType == MCAST::SagittaCorType::ME){
      if(muonInfo.ptms == 0 ) return CorrectionCode::Ok;
      if( iter >=  m_sagittasME->size() )  return CorrectionCode::Ok;
      CorrectionCode corr = CorrectForCharge(sagitta(m_sagittasME->at(iter),lvME) *0.5 , muonInfo.ptms, q, isMC);
      iter++;
      if(corr != CorrectionCode::Ok) return corr;
      if(!stop) return applySagittaBiasCorrection(MCAST::SagittaCorType::ME, mu, iter, stop, isMC, muonInfo);
    }

    else if( SgCorrType == MCAST::SagittaCorType::WEIGHTS){

      const xAOD::TrackParticle* ID_track  = mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      const xAOD::TrackParticle* ME_track =  mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );

      double CBqOverPE = 1e10;
      if( ID_track != NULL && ME_track != NULL )
        CBqOverPE=pow(sqrt( ID_track->definingParametersCovMatrix()( 4, 4 ) + ME_track->definingParametersCovMatrix()( 4, 4 ))/1e3,2);

      double IDqOverPE =  1e10;
      if(ID_track!=NULL)
        IDqOverPE=pow( ID_track->definingParametersCovMatrix()( 4, 4 )/1e3,2);

      double MEqOverPE = 1e10;
      if(ME_track!=NULL)
        MEqOverPE = pow( ME_track->definingParametersCovMatrix()( 4, 4 )/1e3,2);


      CalcCBWeights( mu, muonInfo );


      double ptTilde = (1/IDqOverPE*muonInfo.ptid + 1/MEqOverPE*muonInfo.ptms)/(1/IDqOverPE+1/MEqOverPE);
      double pTilde = ((1/IDqOverPE)*(1/muonInfo.ptid) + (1/MEqOverPE)*(1/muonInfo.ptms))/(1/IDqOverPE+1/MEqOverPE);
      double deltaPTilde  =(1/muonInfo.ptcb-pTilde)/(1/muonInfo.ptcb);

      double deltaID=fabs((1/muonInfo.ptid-1/muonInfo.ptcb)/(1/muonInfo.ptcb));
      double deltaMS=fabs((1/muonInfo.ptms-1/muonInfo.ptcb)/(1/muonInfo.ptcb));
      if(deltaID == 0 ) deltaID=1e-6;
      if(deltaMS == 0 ) deltaMS=1e-6;

      bool dump=false;

      if(iter==0){

        ATH_MSG_DEBUG("eta "<<muonInfo.eta<<" phi  "<<muonInfo.phi);
        ATH_MSG_DEBUG(" ptCB: "<<muonInfo.ptcb<<" --> "<<ptTilde<<" diff "<< (muonInfo.ptcb-ptTilde)*100/muonInfo.ptcb<<" %");
        ATH_MSG_DEBUG(" 1/pT: "<<1/muonInfo.ptcb<<" --> "<<pTilde<<" diff "<< deltaPTilde *100 <<" %"<< "1/pttilde "<< 1/ptTilde);
        ATH_MSG_DEBUG(" deltaID "<<(muonInfo.ptid-muonInfo.ptcb)*100/muonInfo.ptcb<<" % delta MS "<<(muonInfo.ptms-muonInfo.ptcb)*100/muonInfo.ptcb<<" % ");
        ATH_MSG_DEBUG(" sigma(q/p) CB "<<CBqOverPE*100<<" "<<IDqOverPE*100<<" ME "<<MEqOverPE*100);
      }


      if( iter ==  m_sagittasCB->size() ) {
        if(dump)
          ATH_MSG_DEBUG("--> " << muonInfo.ptcb);
        return CorrectionCode::Ok;
      }


      float sagittaID=iter >= m_sagittasID->size() ? 0 : sagitta(m_sagittasID->at(iter),lvID);
      float sagittaME=iter >= m_sagittasME->size() ? 0 : sagitta(m_sagittasME->at(iter),lvME);

      double tmpPtID = lvID.Pt();   //muonInfo.ptid;
      double tmpPtMS = lvME.Pt();   //muonInfo.ptms;

      double tmpDeltaID=0;
      double tmpDeltaMS=0;

      CorrectionCode idOK=applySagittaBiasCorrection(MCAST::SagittaCorType::ID, mu, iter, true, isMC, muonInfo);
      TLorentzVector lvIDCorr; lvIDCorr.SetPtEtaPhiM(muonInfo.ptid,muonInfo.eta,muonInfo.phi,mu.m()/1e3);

      if(idOK == CorrectionCode::Ok && tmpPtID!=0 ) tmpDeltaID = ( -tmpPtID +lvIDCorr.Pt() )/ tmpPtID  ;
      else tmpDeltaID=0;
      ATH_MSG_VERBOSE( "Shift ID "<<tmpDeltaID );


      CorrectionCode meOK=applySagittaBiasCorrection(MCAST::SagittaCorType::ME, mu, iter, true, isMC, muonInfo);
      TLorentzVector lvMECorr;  lvMECorr.SetPtEtaPhiM(muonInfo.ptms,muonInfo.eta,muonInfo.phi,mu.m()/1e3);
      if(meOK == CorrectionCode::Ok && tmpPtMS!=0 ) tmpDeltaMS = ( -tmpPtMS + lvMECorr.Pt()/1e3 ) /tmpPtMS  ;
      else tmpDeltaMS=0;
      ATH_MSG_VERBOSE( "Shift MS "<<tmpDeltaMS );



      double CBsagitta3 = (1/(IDqOverPE*deltaID) * sagittaID
                           + 1/(MEqOverPE*deltaMS) * sagittaME)/(1/(IDqOverPE*deltaID)+1/(MEqOverPE*deltaMS));


      CorrectionCode corr = CorrectForCharge(CBsagitta3, muonInfo.ptcb, q, isMC);
      iter++;
      if(corr != CorrectionCode::Ok) return corr;
      if(!stop) return applySagittaBiasCorrection(MCAST::SagittaCorType::WEIGHTS, mu, iter, stop, isMC, muonInfo);
    }

    return CorrectionCode::Ok;
  }


  CorrectionCode MuonCalibrationAndSmearingTool::applySagittaBiasCorrectionAuto(const int DetType, xAOD::Muon& mu, bool isMC, const unsigned int SytCase, InfoHelper& muonInfo) const {
    //isSystematics ==false

    unsigned int itersCB=0;
    if(SytCase == MCAST::SagittaSysType::BIAS && m_SagittaIterations.at(0) > 1)
      itersCB= m_SagittaIterations.at(0) - 1;

    unsigned int itersID=0;

    if(SytCase == MCAST::SagittaSysType::BIAS && m_SagittaIterations.at(1) > 1)
      itersID= m_SagittaIterations.at(1) - 1;

    unsigned int itersME=0;
    if(SytCase == MCAST::SagittaSysType::BIAS && m_SagittaIterations.at(2) > 1)
      itersME= m_SagittaIterations.at(2) - 1;

    // In case one distrots the MC iterations are set to 1. Systamtics willl be calculated based on the full effect.
    if(m_doSagittaMCDistortion && isMC){
      itersCB =0; itersID =0;  itersME=0;
    }


    if(DetType == MCAST::DetectorType::ID){  // Correct the ID momentum
      return applySagittaBiasCorrection(MCAST::SagittaCorType::ID, mu, isMC, itersID, false, muonInfo);
    }

    else if(DetType == MCAST::DetectorType::MS){  // Correct the ME momentum
      return applySagittaBiasCorrection(MCAST::SagittaCorType::ME, mu, isMC, itersME, false, muonInfo);
    }
    else if(DetType == MCAST::DetectorType::CB){  // Correct the CB momentum;
      if( muonInfo.ptcb == 0) {
        ATH_MSG_VERBOSE("Combined pt = 0 correcting separtly ID and ME");
        if(muonInfo.ptid !=0 && muonInfo.ptms !=0){
          if( applySagittaBiasCorrectionAuto(MCAST::DetectorType::ID, mu, isMC, SytCase, muonInfo) != CorrectionCode::Ok &&
              applySagittaBiasCorrectionAuto(MCAST::DetectorType::MS, mu, isMC, SytCase, muonInfo) != CorrectionCode::Ok ) return CorrectionCode::Error;
        }
        else if(muonInfo.ptid !=0 ){
          if (applySagittaBiasCorrectionAuto(MCAST::DetectorType::ID, mu, isMC, SytCase, muonInfo) != CorrectionCode::Ok) return CorrectionCode::Error;
        }
        else if(muonInfo.ptms !=0 ){
          if (applySagittaBiasCorrectionAuto(MCAST::DetectorType::MS, mu, isMC, SytCase, muonInfo) != CorrectionCode::Ok) return CorrectionCode::Error;
        }
        else {
          return CP::CorrectionCode::Ok;
        }
      }

      double central= 45.2;
      double width=15.5;

      bool isSystematic = SytCase == MCAST::SagittaSysType::RHO;

      if(isSystematic) {
        double sigmaID = ExpectedResolution( MCAST::DetectorType::ID, mu, true ) * muonInfo.ptcb;
        double sigmaMS = ExpectedResolution( MCAST::DetectorType::MS, mu, true ) * muonInfo.ptcb;
        double denominator = (  muonInfo.ptcb  ) * sqrt( sigmaID*sigmaID + sigmaMS*sigmaMS );
        double res= denominator ? sqrt( 2. ) * sigmaID * sigmaMS / denominator : 0.;

        if(m_currentParameters->SagittaRho==MCAST::SystVariation::Up){
          central=central + std::fabs(0.5 * res  * central);
        }
        else if(m_currentParameters->SagittaRho==MCAST::SystVariation::Down){
          central=central - std::fabs(0.5 * res  * central);
        }
      }

      double sigmas=(fabs(muonInfo.ptcb - central)/width);
      double rho= 1/sigmas;
      if(sigmas <  1 ) rho=1;

      // For standalone muons and Silicon associated fowrad do not use the combined
      if( muonInfo.ptid ==0 || muonInfo.ptms ==0){
        ATH_MSG_VERBOSE("Applying sagitta correction for Standalone");

        rho=0;
        if(muonInfo.ptid == 0  && muonInfo.ptms != 0 )  {
          if(applySagittaBiasCorrection(MCAST::SagittaCorType::ME, mu, isMC, itersME, false, muonInfo)!=CorrectionCode::Ok){
            return CP::CorrectionCode::Error;
          }
        }
        else if(muonInfo.ptid != 0  && muonInfo.ptms == 0 )  {
          if(applySagittaBiasCorrection(MCAST::SagittaCorType::ID, mu, isMC, itersID, false, muonInfo)!=CorrectionCode::Ok){
            return CP::CorrectionCode::Error;
          }
        }
        else {
          ATH_MSG_DEBUG("All momenta are 0");
          return    CP::CorrectionCode::OutOfValidityRange;
        }

        ATH_MSG_VERBOSE("Final pt "<<muonInfo.ptcb);
        return CP::CorrectionCode::Ok;
      }

      double ptCB=muonInfo.ptcb;
      double ptWeight=muonInfo.ptcb;

      ATH_MSG_VERBOSE("Applying CB sagitta correction");

      if(applySagittaBiasCorrection(MCAST::SagittaCorType::CB, mu, isMC, itersCB, false, muonInfo)!=CP::CorrectionCode::Ok){
        return CP::CorrectionCode::Error;
      }

      else {
        ptCB = muonInfo.ptcb;
      }

      ATH_MSG_VERBOSE("Applying Weighted sagitta correction");

      if(applySagittaBiasCorrection(MCAST::SagittaCorType::WEIGHTS, mu, isMC, itersCB, false, muonInfo)!=CP::CorrectionCode::Ok){
        return CP::CorrectionCode::Error;
      }
      else {
        ptWeight =  muonInfo.ptcb;
      }

      if(m_useFixedRho){
        ATH_MSG_VERBOSE("Using fixed rho value "<<m_fixedRho);
        rho=m_fixedRho;
      }


      muonInfo.ptcb = rho*ptCB + (1-rho)*ptWeight;
      ATH_MSG_VERBOSE("Final pt "<<muonInfo.ptcb<<" "<<rho<<" * "<<ptCB<<" 1- rho "<<1-rho<<"  *  "<<ptWeight<<" sigmas "<<sigmas);
      return CorrectionCode::Ok;
    }
    else{
      return CorrectionCode::Ok;
    }
  }


  CorrectionCode MuonCalibrationAndSmearingTool::applyCorrection( xAOD::Muon& mu ) const {

    ATH_MSG_VERBOSE( "Muon Type = " << mu.muonType() << " ( 0: Combined, 1: StandAlone, 2: SegmentTagged, 3: CaloTagged )" );
    ATH_MSG_VERBOSE( "Muon Author = " << mu.author() );

    // Make InfoHelper for passing muon info between functions
    InfoHelper muonInfo;

    // Set pt ID:

    ATH_MSG_VERBOSE( "Retrieving ElementLink to ID TrackParticle..." );
    ATH_MSG_VERBOSE( "Setting Pt  [ID]: if no track available, set to 0..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"inDetTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "inDetTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() == NULL ) = " << ( mu.inDetTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.inDetTrackParticleLink() = " << mu.inDetTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() ).isValid() = " << ( mu.inDetTrackParticleLink() ).isValid() );

    if( ( mu.inDetTrackParticleLink() ).isValid() ) {
      //if( mu.isAvailable<ElementLink<xAOD::TrackParticleContainer> > ("inDetTrackParticleLink")){
      const ElementLink< xAOD::TrackParticleContainer >& id_track = mu.inDetTrackParticleLink();
      muonInfo.ptid = ( !id_track ) ? 0. : ( *id_track )->pt() / 1000.;
    } else {
      ATH_MSG_VERBOSE("The ID track link is not valid - setting pT(ID)=0");
      muonInfo.ptid = 0.;
    }

    // Set pt MS:

    ATH_MSG_VERBOSE( "Retrieving ElementLink to MS TrackParticle..." );
    ATH_MSG_VERBOSE( "Setting Pt  [MS]: if no track available, set to 0..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"extrapolatedMuonSpectrometerTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "extrapolatedMuonSpectrometerTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.extrapolatedMuonSpectrometerTrackParticleLink() = " << mu.extrapolatedMuonSpectrometerTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() );
    if( ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() ) {
      // if( mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "extrapolatedMuonSpectrometerTrackParticleLink" ) ) {
      const ElementLink< xAOD::TrackParticleContainer >& ms_track = mu.extrapolatedMuonSpectrometerTrackParticleLink();
      muonInfo.ptms = ( !ms_track ) ? 0. : ( *ms_track )->pt() / 1000.;
    }
    else{
      ATH_MSG_VERBOSE("No link to extrapolatedMuonSpectrometerTrackParticleLink setting pT(MS)=0");
      muonInfo.ptms = 0.;
    }

    // Set pt CB:

    ATH_MSG_VERBOSE( "Retrieving ElementLink to CB TrackParticle..." );
    ATH_MSG_VERBOSE( "Setting Pt  [CB]: if no track available, set to 0..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"primaryTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "primaryTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.primaryTrackParticleLink() == NULL ) = " << ( mu.primaryTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.primaryTrackParticleLink() = " << mu.primaryTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.primaryTrackParticleLink() ).isValid() = " << ( mu.primaryTrackParticleLink() ).isValid() );

    if( ( mu.primaryTrackParticleLink() ).isValid() ) {
      //if( mu.isAvailable< ElementLink < xAOD::TrackParticleContainer > > ( "primaryTrackParticleLink" ) ) {
      const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.primaryTrackParticleLink();
      muonInfo.ptcb = ( !cb_track ) ? 0. : ( *cb_track )->pt() / 1000.;
    }
    else{
      ATH_MSG_VERBOSE("The ID+MS combined track link is not valid - setting pT(ID)=0");
      muonInfo.ptcb = 0.;
    }

    // Set the charge

    if( mu.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) {
      if( mu.isAvailable<ElementLink<xAOD::TrackParticleContainer > > ("combinedTrackParticleLink")){
        if(mu.combinedTrackParticleLink().isValid()){
          const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.combinedTrackParticleLink();
          muonInfo.charge = ( *cb_track )->charge();
        }
      }
    }
    else{
      muonInfo.charge = mu.charge();
    }

    // Set remaining muon information:
    // Check if muon has been already corrected! If so, take original pt...
    //ATH_MSG_VERBOSE( "Setting Pt  [CB]..." );
    //if( mu.isAvailable< float >( "OriginalMuonPt" ) && mu.auxdata< float >( "OriginalMuonPt" ) > 0 ) {
    //  muonInfo.ptcb = mu.auxdata< float >( "OriginalMuonPt" ) / 1000.;
    //  ATH_MSG_DEBUG( "Found \"OriginalMuonPt\" decoration, with value: " << muonInfo.ptcb * 1000. );
    //}
    //else {
    //  muonInfo.ptcb = mu.pt() / 1000.;
    //  mu.auxdata< float >( "OriginalMuonPt" ) = mu.pt();
    //  ATH_MSG_DEBUG( "Setting \"OriginalMuonPt\" decoration to value: " << mu.auxdata< float >( "OriginalMuonPt" ) );
    //}
    ATH_MSG_VERBOSE( "Setting Eta [CB]..." );
    muonInfo.eta = mu.eta();
    ATH_MSG_VERBOSE( "Setting Phi [CB]..." );
    muonInfo.phi = mu.phi();



    // Getting detector region
    if( int( muonInfo.phi ) == DEFAULT_INIT_VAL && m_Trel == MCAST::Release::Rel17_2_Sum13 )
      ATH_MSG_ERROR( "You need phi of the muon for after Rel17.2Sum13!" );

    if( int( muonInfo.phi ) == DEFAULT_INIT_VAL && m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
      //Lower release have just Eta bins, so usually the user doesn't input
      //Phi and 0 is a good default for these regions
      muonInfo.detRegion = GetRegion( muonInfo.eta, 0 );
    }
    else {
      muonInfo.detRegion = GetRegion( muonInfo.eta, muonInfo.phi );
    }

    // Clear the vector of cov matrices
    muonInfo.cbParsA.clear();
    muonInfo.cbCovMat.clear();

    // Set the statistical combination for muons above given pT threshotld.
    if( m_useStatComb && muonInfo.ptcb > m_StatCombPtThreshold && isBadMuon(mu, muonInfo)) {
      CorrectionCode cbCode=applyStatCombination(mu, muonInfo);
      if( cbCode==CorrectionCode::Ok){
        if(m_doNotUseAMGMATRIXDECOR){
          muonInfo.ptcb = sin(muonInfo.cbParsA[3])/fabs(muonInfo.cbParsA[4])/ 1000;
        }
        else {
          if(!mu.isAvailable < AmgVector(5) >( "StatCombCBPars" )) return CorrectionCode::Error;
          AmgVector(5) parsCB = mu.auxdata < AmgVector(5) >( "StatCombCBPars" );
          muonInfo.ptcb = sin(parsCB[3])/fabs(parsCB[4])/ 1000;
        }
      }
    }

    // Retrieve the event information:
    const xAOD::EventInfo* evtInfo = 0;
    ATH_MSG_VERBOSE( "Retrieving EventInfo from the EventStore..." );
    if( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() ) {
      ATH_MSG_ERROR( "No EventInfo object could be retrieved" );
      ATH_MSG_ERROR( "Random number generation not configured correctly, impossible to determine if dealing with data or MC" );
      return CorrectionCode::Error;
    }
    ATH_MSG_VERBOSE( "Checking Simulation flag: " << evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) );

    if( !evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {

      // Statistical combiantion specifics
      if(m_useStatComb){
        mu.setP4( muonInfo.ptcb * 1000 , muonInfo.eta, muonInfo.phi );
      }

      // Sagitta Correction  specifics
      if(m_doSagittaCorrection){
        CorrectionCode sgCode = applySagittaBiasCorrectionAuto(MCAST::DetectorType::CB, mu, false, MCAST::SagittaSysType::NOMINAL, muonInfo);
        if(sgCode!=CorrectionCode::Ok) return sgCode;
        mu.setP4( muonInfo.ptcb * 1000 , muonInfo.eta, muonInfo.phi );
        //mu.auxdata< float >( "InnerDetectorPt" ) = muonInfo.ptid * 1000.;
        //mu.auxdata< float >( "MuonSpectrometerPt" ) = muonInfo.ptms * 1000.;
      }

      mu.auxdata< float >( "InnerDetectorPt" ) = muonInfo.ptid * 1000.;

      // Toroid-off specifics
      if( m_toroidOff ) {
        mu.auxdata< float >( "MuonSpectrometerPt" ) = 0.;
        mu.setP4( muonInfo.ptid * 1000., muonInfo.eta, muonInfo.phi );
      }
      else {
        mu.auxdata< float >( "MuonSpectrometerPt" ) = muonInfo.ptms * 1000.;
      }

      // SAF specifics
      if( mu.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) {
        ATH_MSG_VERBOSE( "Retrieving ElementLink to CB TrackParticle..." );
        ATH_MSG_VERBOSE( "Setting Pt  [CB]: if no track available, set to 0..." );
        //ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"combinedTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "combinedTrackParticleLink" ) );
        //ATH_MSG_VERBOSE( "( mu.combinedTrackParticleLink() == NULL ) = " << ( mu.combinedTrackParticleLink() == NULL ) );
        //ATH_MSG_VERBOSE( "mu.combinedTrackParticleLink() = " << mu.combinedTrackParticleLink() );
        //ATH_MSG_VERBOSE( "( mu.combinedTrackParticleLink() ).isValid() = " << ( mu.combinedTrackParticleLink() ).isValid() );
        float temp_cb_pt = 0.;
        //if( mu.isAvailable<ElementLink<xAOD::TrackParticleContainer> > ("inDetTrackParticleLink")){
        if( mu.isAvailable<ElementLink<xAOD::TrackParticleContainer > > ("combinedTrackParticleLink")){
          if(mu.combinedTrackParticleLink().isValid()){
            const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.combinedTrackParticleLink();
            temp_cb_pt = ( !cb_track ) ? 0. : ( *cb_track )->pt();
          }
        }
        mu.setP4( temp_cb_pt, muonInfo.eta, muonInfo.phi );
      }

      return CorrectionCode::Ok;
    }

    //Now get the specific regions and the MC smearing/scale calib
    if( SetInfoHelperCorConsts(muonInfo) == StatusCode::FAILURE ) {
      ATH_MSG_DEBUG( "Can't configure Correction constants! Set smearing to 0." );
      mu.auxdata< float >( "InnerDetectorPt" ) = muonInfo.ptid * 1000.;
      mu.auxdata< float >( "MuonSpectrometerPt" ) = muonInfo.ptms * 1000.;
      return CorrectionCode::OutOfValidityRange;
    }

    CalcCBWeights( mu, muonInfo );
    ATH_MSG_VERBOSE( "Checking Weights - weightID: " << muonInfo.weightID << " - fabs( weightID - 1 ): " << fabs( muonInfo.weightID - 1 ) );
    ATH_MSG_VERBOSE( "Checking Weights - weightMS: " << muonInfo.weightMS << " - fabs( weightMS - 1 ): " << fabs( muonInfo.weightMS - 1 ) );
    muonInfo.smearDeltaCB = muonInfo.smearDeltaID * muonInfo.weightID + muonInfo.smearDeltaMS * muonInfo.weightMS;

    // Calibrate the pt of the muon:
    double res_idPt = 1000. * CalculatePt( MCAST::DetectorType::ID, muonInfo.smearDeltaID, muonInfo.smearDeltaMS, m_currentParameters->Scale, muonInfo );
    double res_msPt = 1000. * CalculatePt( MCAST::DetectorType::MS, muonInfo.smearDeltaID, muonInfo.smearDeltaMS, m_currentParameters->Scale, muonInfo );
    double res_cbPt = 1000. * CalculatePt( MCAST::DetectorType::CB, muonInfo.smearDeltaID, muonInfo.smearDeltaMS, m_currentParameters->Scale, muonInfo );

    if( ( m_doSagittaCorrection ||  m_doSagittaMCDistortion ) &&  (m_currentParameters->SagittaRho != MCAST::SystVariation::Default ||
                                                                   m_currentParameters->SagittaBias != MCAST::SystVariation::Default) ){
      ATH_MSG_VERBOSE( "Systematic uncertainties for sagitta bias "<< muonInfo.ptcb << res_idPt);

      muonInfo.ptid = res_idPt/1000.;
      muonInfo.ptms = res_msPt/1000.;
      muonInfo.ptcb = res_cbPt/1000.;


      CorrectionCode sgCode=CorrectionCode::Ok;
      if(m_currentParameters->SagittaRho != MCAST::SystVariation::Default){
        if(m_currentParameters->SagittaRho == MCAST::SystVariation::Up || m_currentParameters->SagittaRho == MCAST::SystVariation::Down){
          sgCode=applySagittaBiasCorrectionAuto(MCAST::DetectorType::CB, mu, true, MCAST::SagittaSysType::RHO, muonInfo);
        }
      }

      else if(m_currentParameters->SagittaBias != MCAST::SystVariation::Default){
        if(m_currentParameters->SagittaBias == MCAST::SystVariation::Up || m_currentParameters->SagittaBias == MCAST::SystVariation::Down){
          sgCode=applySagittaBiasCorrectionAuto(MCAST::DetectorType::CB, mu, true, MCAST::SagittaSysType::BIAS, muonInfo);
        }
      }

      if(sgCode!=CorrectionCode::Ok)
        return sgCode;

      res_idPt=muonInfo.ptid *1000.;
      res_msPt=muonInfo.ptms *1000.;
      res_cbPt=muonInfo.ptcb *1000.;
    }

    // Sagitta Distrotion  specifics
    //if(m_doSagittaMCDistortion){
    //CorrectionCode sgCode=applySagittaBiasCorrectionAuto(MCAST::DetectorType::CB, mu, true, MCAST::SagittaSysType::NOMINAL, muonInfo);
    //if(sgCode!=CorrectionCode::Ok) return sgCode;
    //mu.setP4( muonInfo.ptcb * 1000 , muonInfo.eta, muonInfo.phi );
    //}

    // Override combined momentum for special cases
    if( fabs( muonInfo.weightID - 1 ) < EPSILON ) res_cbPt = res_idPt;
    if( fabs( muonInfo.weightMS - 1 ) < EPSILON ) res_cbPt = res_msPt;
    // Using ToroidOff flag
    mu.auxdata< float >( "InnerDetectorPt" ) = res_idPt;
    if( m_toroidOff ) {
      mu.auxdata< float >( "MuonSpectrometerPt" ) = 0.;
      mu.setP4( res_idPt, muonInfo.eta, muonInfo.phi );
    }
    else {
      mu.auxdata< float >( "MuonSpectrometerPt" ) = res_msPt;
      double cbPT=0;

      if( ( mu.primaryTrackParticleLink() ).isValid() ) {
        const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.primaryTrackParticleLink();
        cbPT=(*cb_track)->pt()/1000;
      }
      ATH_MSG_VERBOSE("CB pt "<<cbPT<<" stat comb "<<muonInfo.ptcb<<" corrected pt "<<res_cbPt/1000);
      mu.setP4( res_cbPt, muonInfo.eta, muonInfo.phi );
    }
    ATH_MSG_DEBUG( "Checking Output Muon Info - Pt_ID: " << res_idPt );
    ATH_MSG_DEBUG( "Checking Output Muon Info - Pt_MS: " << res_msPt );
    ATH_MSG_DEBUG( "Checking Output Muon Info - Pt_CB: " << res_cbPt );
    ATH_MSG_VERBOSE( "Checking Output Muon Info - Pt_CB - Pt_ID: " << res_cbPt - res_idPt );

    // Return gracefully:
    return CorrectionCode::Ok;
  }


  CorrectionCode MuonCalibrationAndSmearingTool::applyCorrectionTrkOnly( xAOD::TrackParticle& inTrk, const int DetType) const {
    // Make InfoHelper for passing muon info between functions
    InfoHelper muonInfo;

    if( DetType == MCAST::DetectorType::ID ) {//ID-trk only correction
      muonInfo.ptid = inTrk.pt() / 1000.;
      muonInfo.ptms = 0.;
      muonInfo.ptcb = muonInfo.ptid;
      muonInfo.weightID = 1.;
      muonInfo.weightMS = 0.;
    } else if ( DetType == MCAST::DetectorType::MS){ //MS-trk only correction
      muonInfo.ptid = 0.;
      muonInfo.ptms = inTrk.pt() / 1000.;
      muonInfo.ptcb = muonInfo.ptms;
      muonInfo.weightID = 0.;
      muonInfo.weightMS = 1.;
    } else {
      ATH_MSG_ERROR( "Track Type not recongnized!");
      return CorrectionCode::Error;
    }
    muonInfo.charge = inTrk.charge();
    muonInfo.eta = inTrk.eta();
    muonInfo.phi = inTrk.phi();

    //::: Retrieve the event information:
    const xAOD::EventInfo* evtInfo = 0;
    ATH_MSG_VERBOSE( "Retrieving EventInfo from the EventStore..." );
    if( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() ) {
      ATH_MSG_ERROR( "No EventInfo object could be retrieved" );
      ATH_MSG_ERROR( "Random number generation not configured correctly, impossible to determine if dealing with data or MC" );
      return CorrectionCode::Error;
    }
    ATH_MSG_VERBOSE( "Checking Simulation flag: " << evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) );

    if( evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
      //Now get the specific regions and the MC smearing/scale calib
      if( SetInfoHelperCorConsts(muonInfo) == StatusCode::FAILURE ){
        ATH_MSG_DEBUG( "Can't configure Correction constants! Set smearing to 0." );
        return CorrectionCode::OutOfValidityRange;
      }

      ATH_MSG_VERBOSE( "Checking Weights - weightID: " << muonInfo.weightID <<" - weightMS: " << muonInfo.weightMS);

      //::: Calibrate the pt of the muon:
      double res_pt = muonInfo.ptcb; //at this level ptcb is a dummy copy of ptID or ptMS
      if(DetType == MCAST::DetectorType::ID) {
        res_pt = CalculatePt( MCAST::DetectorType::ID, muonInfo.smearDeltaID, muonInfo.smearDeltaMS, m_currentParameters->Scale, muonInfo );
      } else if ( DetType == MCAST::DetectorType::MS){
        res_pt = CalculatePt( MCAST::DetectorType::MS, muonInfo.smearDeltaID, muonInfo.smearDeltaMS, m_currentParameters->Scale, muonInfo);
      } else {
        return CorrectionCode::Error;
      }

      inTrk.setDefiningParameters(inTrk.d0(), inTrk.z0(), inTrk.phi0(), inTrk.theta(), inTrk.charge()/(res_pt*cosh(inTrk.eta())));

    } //+++++++ DO HERE THE SAGITTA BIAS CASE!

    return CorrectionCode::Ok;
  }

  StatusCode MuonCalibrationAndSmearingTool::SetInfoHelperCorConsts(InfoHelper& inMuonInfo) const{

    ATH_MSG_DEBUG( "Checking Input Muon Info -  Pt_ID: " << inMuonInfo.ptid * 1000. );
    ATH_MSG_DEBUG( "Checking Input Muon Info -  Pt_MS: " << inMuonInfo.ptms * 1000. );
    ATH_MSG_DEBUG( "Checking Input Muon Info -  Pt_CB: " << inMuonInfo.ptcb * 1000. );
    ATH_MSG_VERBOSE( "Checking Input Muon Info -    Eta: " << inMuonInfo.eta );
    ATH_MSG_VERBOSE( "Checking Input Muon Info -    Phi: " << inMuonInfo.phi );
    ATH_MSG_VERBOSE( "Checking Input Muon Info - Charge: " << ( ( inMuonInfo.charge > 0 ) ? "+" : "-" ) );
    ATH_MSG_VERBOSE( "Checking Input Muon Info -  Pt_CB - Pt_ID: " << ( inMuonInfo.ptcb - inMuonInfo.ptid ) * 1000. );

    // Random number generation for smearing
    TRandom3   loc_random3;

    // if you don't want to use the external seed it will be set based on the eventNumber and the muon(eta,phi)
    if( !m_useExternalSeed ) {
      // Get Event Number, Retrieve the event information:
      const xAOD::EventInfo* evtInfo = 0;
      ATH_MSG_VERBOSE( "Retrieving EventInfo from the EventStore..." );
      if( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() ) {
        ATH_MSG_ERROR( "No EventInfo object could be retrieved" );
        ATH_MSG_ERROR( "Random number generation not configured correctly, impossible to determine if dealing with data or MC" );
        return CorrectionCode::Error;
      }

      const unsigned long long eventNumber = evtInfo ? evtInfo->eventNumber() : 0;
      // Construct a seed for the random number generator:
      const UInt_t seed = 1 + std::abs( inMuonInfo.phi) * 1E6 + std::abs( inMuonInfo.eta ) * 1E3 + eventNumber;
      loc_random3.SetSeed( seed );
    }
    else{
      const UInt_t seed = m_externalSeed;
      loc_random3.SetSeed( seed );
    }

    inMuonInfo.smearDeltaMS = 0.;
    inMuonInfo.smearDeltaID = 0.;
    inMuonInfo.smearDeltaCB = 0.;
    inMuonInfo.detRegion = -1;

    // Getting detector region
    if( int( inMuonInfo.phi ) == DEFAULT_INIT_VAL && m_Trel == MCAST::Release::Rel17_2_Sum13 )
      ATH_MSG_ERROR( "You need phi of the muon for after Rel17.2Sum13!" );
    if( int( inMuonInfo.phi ) == DEFAULT_INIT_VAL && m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
      //Lower release have just Eta bins, so usually the user doesn't input
      //Phi and 0 is a good default for these regions
      inMuonInfo.detRegion = GetRegion( inMuonInfo.eta, 0 );
    }
    else {
      inMuonInfo.detRegion = GetRegion( inMuonInfo.eta, inMuonInfo.phi );
    }

    if( inMuonInfo.detRegion == -1 ) {
      ATH_MSG_DEBUG( "Can't find correct detector region! Set smearing to 0." );
      return CorrectionCode::OutOfValidityRange;
    }

    // Getting random numbers for smearing
    inMuonInfo.g0 = loc_random3.Gaus( 0, 1 );
    inMuonInfo.g1 = loc_random3.Gaus( 0, 1 );
    inMuonInfo.g2 = loc_random3.Gaus( 0, 1 );
    inMuonInfo.g3 = loc_random3.Gaus( 0, 1 );
    inMuonInfo.g4 = loc_random3.Gaus( 0, 1 );
    ATH_MSG_VERBOSE( "Checking Random Values - g_0: " << inMuonInfo.g0 );
    ATH_MSG_VERBOSE( "Checking Random Values - g_1: " << inMuonInfo.g1 );
    ATH_MSG_VERBOSE( "Checking Random Values - g_2: " << inMuonInfo.g2 );
    ATH_MSG_VERBOSE( "Checking Random Values - g_3: " << inMuonInfo.g3 );
    ATH_MSG_VERBOSE( "Checking Random Values - g_4: " << inMuonInfo.g4 );

    // Getting smearing values
    // MS
    if( m_currentParameters->SmearTypeMS == MCAST::SystVariation::Default ) {
      inMuonInfo.smearDeltaMS = GetSmearing( MCAST::DetectorType::MS, inMuonInfo );
    }
    else if( m_currentParameters->SmearTypeMS == MCAST::SystVariation::Up ) {
      inMuonInfo.smearDeltaMS = GetSystVariation( MCAST::DetectorType::MS, 1., inMuonInfo );
    }
    else if( m_currentParameters->SmearTypeMS == MCAST::SystVariation::Down ) {
      inMuonInfo.smearDeltaMS = GetSystVariation( MCAST::DetectorType::MS, -1., inMuonInfo );
    }
    else {
      ATH_MSG_ERROR( "Invalid value for m_currentParameters->SmearTypeMS" );
    }
    // ID
    if( m_currentParameters->SmearTypeID == MCAST::SystVariation::Default ) {
      inMuonInfo.smearDeltaID = GetSmearing( MCAST::DetectorType::ID, inMuonInfo );
    }
    else if( m_currentParameters->SmearTypeID == MCAST::SystVariation::Up ) {
      inMuonInfo.smearDeltaID = GetSystVariation( MCAST::DetectorType::ID, 1., inMuonInfo );
    }
    else if( m_currentParameters->SmearTypeID == MCAST::SystVariation::Down ) {
      inMuonInfo.smearDeltaID = GetSystVariation( MCAST::DetectorType::ID, -1., inMuonInfo );
    }
    else {
      ATH_MSG_ERROR( "Invalid value for m_currentParameters->SmearTypeID" );
    }

    ATH_MSG_VERBOSE( "Checking Smearing - smearDeltaID: " << inMuonInfo.smearDeltaID );
    ATH_MSG_VERBOSE( "Checking Smearing - smearDeltaMS: " << inMuonInfo.smearDeltaMS );
    return StatusCode::SUCCESS;
  }


  CorrectionCode MuonCalibrationAndSmearingTool::correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const {

    // A sanity check:
    if( output ) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );

    // Create a new object:
    ATH_MSG_VERBOSE( "Going to create new xAOD::Muon..." );
    output = new xAOD::Muon();
    ATH_MSG_VERBOSE( "Calling makePrivateStore..." );
    output->makePrivateStore( input );

    // Use the other function to modify this object:
    ATH_MSG_VERBOSE( "Calling applyCorrection..." );

    CP::CorrectionCode retCode = applyCorrection( *output );

    return retCode;

  }

  bool MuonCalibrationAndSmearingTool::isAffectedBySystematic( const SystematicVariation& systematic ) const {
    SystematicSet sys = affectingSystematics();
    return sys.find( systematic ) != sys.end();
  }

  SystematicSet MuonCalibrationAndSmearingTool::affectingSystematics() const {

    SystematicSet result;

    // ID systematics
    result.insert( SystematicVariation( "MUON_ID", 1 ) );
    result.insert( SystematicVariation( "MUON_ID", -1 ) );

    // MS systematics
    result.insert( SystematicVariation( "MUON_MS", 1 ) );
    result.insert( SystematicVariation( "MUON_MS", -1 ) );

    // Scale systematics
    result.insert( SystematicVariation( "MUON_SCALE", 1 ) );
    result.insert( SystematicVariation( "MUON_SCALE", -1 ) );

    // Sagitta correction rho
    result.insert( SystematicVariation( "MUON_SAGITTA_RHO", 1 ) );
    result.insert( SystematicVariation( "MUON_SAGITTA_RHO", -1 ) );

    // Sagitta correction resid bias
    result.insert( SystematicVariation( "MUON_SAGITTA_RESBIAS", 1 ) );
    result.insert( SystematicVariation( "MUON_SAGITTA_RESBIAS", -1 ) );

    return result;

  }

  SystematicSet MuonCalibrationAndSmearingTool::recommendedSystematics() const {
    return affectingSystematics();
  }

  SystematicCode MuonCalibrationAndSmearingTool::applySystematicVariation( const SystematicSet& systConfig ) {

    // First check if we already know this systematic configuration
    boost::unordered_map< SystematicSet, ParameterSet >::iterator parIter = m_Parameters.find( systConfig );
    if( parIter != m_Parameters.end() ) {
      m_currentParameters = &parIter->second;
      return SystematicCode::Ok;
    }

    // Then check if it is actually supported
    static CP::SystematicSet affSysts = affectingSystematics();
    SystematicSet checkSysConf;
    if( !SystematicSet::filterForAffectingSystematics( systConfig, affSysts, checkSysConf ) ) {
      ATH_MSG_ERROR( "Passing unsupported systematic to the tool!" );
      return SystematicCode::Unsupported;
    }

    ParameterSet param;
    param.SmearTypeID = MCAST::SystVariation::Default;
    param.SmearTypeMS = MCAST::SystVariation::Default;
    param.Scale = MCAST::SystVariation::Default;
    param.SagittaRho=MCAST::SystVariation::Default;
    param.SagittaBias=MCAST::SystVariation::Default;

    // ID systematics
    SystematicVariation syst = systConfig.getSystematicByBaseName( "MUON_ID" );

    if( syst == SystematicVariation( "MUON_ID", 1 ) ) {
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Up;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( syst == SystematicVariation( "MUON_ID", -1 ) ) {
      //param.SmearTypeMS = muonInfo.smearDeltaMS;
      //param.SmearTypeID = GetSystVariation( MCAST::DetectorType::ID, -1., muonInfo );
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Down;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( !syst.empty() ) return SystematicCode::Unsupported;

    // MS systematics
    syst = systConfig.getSystematicByBaseName( "MUON_MS" );

    if( syst == SystematicVariation( "MUON_MS", 1 ) ) {
      //param.SmearTypeMS = GetSystVariation( MCAST::DetectorType::MS, 1., muonInfo );
      //param.SmearTypeID = muonInfo.smearDeltaID;
      param.SmearTypeMS = MCAST::SystVariation::Up;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( syst == SystematicVariation( "MUON_MS", -1 ) ) {
      //param.SmearTypeID = muonInfo.smearDeltaID;
      //param.SmearTypeMS = GetSystVariation( MCAST::DetectorType::MS, -1., muonInfo );
      param.SmearTypeMS = MCAST::SystVariation::Down;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( !syst.empty() ) return SystematicCode::Unsupported;

    // Scale systematics
    syst = systConfig.getSystematicByBaseName( "MUON_SCALE" );

    if( syst == SystematicVariation( "MUON_SCALE", 1 ) ) {
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Down;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( syst == SystematicVariation( "MUON_SCALE", -1 ) ) {
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Up;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( !syst.empty() ) return SystematicCode::Unsupported;


    // Sagitta Rho systematics
    syst = systConfig.getSystematicByBaseName( "MUON_SAGITTA_RHO" );

    if( syst == SystematicVariation( "MUON_SAGITTA_RHO", 1 ) ) {
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Down;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( syst == SystematicVariation( "MUON_SAGITTA_RHO", -1 ) ) {
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Up;
      param.SagittaBias=MCAST::SystVariation::Default;
    }
    else if( !syst.empty() ) return SystematicCode::Unsupported;


    // Sagitta Residual Bias systematics
    syst = systConfig.getSystematicByBaseName( "MUON_SAGITTA_RESBIAS" );

    if( syst == SystematicVariation( "MUON_SAGITTA_RESBIAS", 1 ) ) {
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Down;
    }
    else if( syst == SystematicVariation( "MUON_SAGITTA_RESBIAS", -1 ) ) {
      param.SmearTypeMS = MCAST::SystVariation::Default;
      param.SmearTypeID = MCAST::SystVariation::Default;
      param.Scale = MCAST::SystVariation::Default;
      param.SagittaRho=MCAST::SystVariation::Default;
      param.SagittaBias=MCAST::SystVariation::Up;
    }
    else if( !syst.empty() ) return SystematicCode::Unsupported;


    //
    ATH_MSG_DEBUG( "Systematic variation's parameters, SmearTypeID: " << param.SmearTypeID );
    ATH_MSG_DEBUG( "Systematic variation's parameters, SmearTypeMS: " << param.SmearTypeMS );
    ATH_MSG_DEBUG( "Systematic variation's parameters, Scale: " << param.Scale );
    // store this calibration for future use, and make it current
    m_currentParameters = &m_Parameters.insert( std::make_pair( systConfig, param ) ).first->second;
    return SystematicCode::Ok;

  }

  StatusCode MuonCalibrationAndSmearingTool::SetData( std::string data ) {

    if( data == "Data10" ) {
      m_Tdata = MCAST::DataType::Data10;
    }
    else if( data == "Data11" ) {
      m_Tdata = MCAST::DataType::Data11;
    }
    else if( data == "Data12" ) {
      m_Tdata = MCAST::DataType::Data12;
    }
    else if( data == "Data15" ) {
      m_Tdata = MCAST::DataType::Data15;
    }
    else if( data == "Data16" ) {
      m_Tdata = MCAST::DataType::Data16;
    }
    else {
      ATH_MSG_ERROR( "Unrecognized value for SetData" );
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;

  }

  StatusCode MuonCalibrationAndSmearingTool::SetAlgorithm( std::string algo ) {

    if( algo == "muid" ) {
      m_Talgo = MCAST::AlgoType::Muid;
    }
    else if( algo == "staco" ) {
      m_Talgo = MCAST::AlgoType::Staco;
    }
    else if( algo == "muons" ) {
      m_Talgo = MCAST::AlgoType::Muons;
    }
    else {
      ATH_MSG_ERROR( "Unrecognized value for SetAlgorithm" );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;

  }

  StatusCode MuonCalibrationAndSmearingTool::SetType( std::string type ) {

    if( type == "pT" ) {
      m_Tsmear = MCAST::SmearingType::Pt;
      ATH_MSG_DEBUG( "Using " << type << " Smearing " );
    }
    else if( type == "q_pT" ) {
      m_Tsmear = MCAST::SmearingType::QoverPt;
      ATH_MSG_DEBUG( "Using " << type << " Smearing " );
    }
    else {
      ATH_MSG_ERROR( "Unrecognized value for SetType" );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;

  }

  StatusCode MuonCalibrationAndSmearingTool::SetRelease( std::string rel ) {

    if( rel == "Rel16.6" ) {
      m_Trel = MCAST::Release::Rel16_6;
    }
    else if( rel == "Rel17" ) {
      m_Trel = MCAST::Release::Rel17;
    }
    else if( rel == "Rel17.2" ) {
      m_Trel = MCAST::Release::Rel17_2;
    }
    else if( rel == "Rel17.2Repro" ) {
      m_Trel = MCAST::Release::Rel17_2_Repro;
    }
    else if( rel == "Rel17.2Sum13" ) {
      m_Trel = MCAST::Release::Rel17_2_Sum13;
    }
    else if( rel == "PreRecs" ) {
      m_Trel = MCAST::Release::PreRec;
    }
    else if( rel == "PreRecs2015_06_22" ) {
      m_Trel = MCAST::Release::PreRec_2015_06_22;
    }
    else if( rel == "PreRecs2015_08_06" ) {
      m_Trel = MCAST::Release::PreRec_2015_08_06;
    }
    else if( rel == "Recs2015_11_15" ) {
      m_Trel = MCAST::Release::Rec_2015_11_15;
    }
    else if( rel == "Recs2016_01_13" ) {
      m_Trel = MCAST::Release::Rec_2016_01_13;
    }
    else if( rel == "Recs2016_01_19" ) {
      m_Trel = MCAST::Release::Rec_2016_01_19;
    }
    else if( rel == "PreRecs2016_05_23" ) {
      m_Trel = MCAST::Release::PreRec_2016_05_23;
    }
    else if (rel == "Recs2016_08_07") {
      m_Trel = MCAST::Release::Recs2016_08_07;
    }
    else if (rel == "Recs2016_15_07") {
      m_Trel = MCAST::Release::Recs2016_08_07;
    }
    else {
      m_Trel = MCAST::Release::Recs2016_08_07;
      //ATH_MSG_ERROR( "Unrecognized value for SetRelease" );
      //return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;

  }

  double MuonCalibrationAndSmearingTool::ScaleApply( const double pt, double S, const double S_enLoss, InfoHelper& muonInfo ) const {

    // Sanity checks:
    if( muonInfo.detRegion < 0 || muonInfo.detRegion >= m_nb_regions ) return pt;

    // default momentum scale (derived form template fits)
    return pt * S + S_enLoss;
    //bool apply_to_data = false;
    //return apply_to_data ? pt / S : ( pt * S + S_enLoss );

  }


  double MuonCalibrationAndSmearingTool::CalculatePt( const int DetType, const double inSmearID, const double inSmearMS, const double scaleVar, InfoHelper& muonInfo ) const {

    double scaleID = 0., enLossCorrMS = 0., scaleMS = 0., scaleCB = 0.;//initialize all to 0
    // These are alternative scale corrections (KPKM,KC,K,C) they are != 0. if Tscale != SCALE_DEFAULT.
    //double s1_ID = 0., s2_ID = 0., s1_MS = 0., s2_MS = 0., s1_CB = 0., s2_CB = 0.;//Description of these is in ApplyScale

    if( fabs( scaleVar ) != 1. && scaleVar != 0. ) ATH_MSG_ERROR( "Unpredicted scale variation of Delta "<<scaleVar<<" sigmas!" );

    if( m_scale_ID[muonInfo.detRegion] != -1 ) {
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        scaleID = scaleVar > 0. ? m_scaleSystUp_ID[muonInfo.detRegion] : m_scaleSystDw_ID[muonInfo.detRegion];
      }
      else {
        scaleID = m_scaleSyst_ID[muonInfo.detRegion];
      }
      scaleID = m_scale_ID[muonInfo.detRegion] + scaleVar*scaleID;
    }
    else {
      scaleID = ( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) ? 0. : 1.;
    }
    if( m_scale_MS[muonInfo.detRegion] != -1 ) {
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        scaleMS = scaleVar > 0. ? m_scaleSystUp_MS[muonInfo.detRegion] : m_scaleSystDw_MS[muonInfo.detRegion];
        enLossCorrMS = scaleVar > 0. ? m_enLossSystUp_MS[muonInfo.detRegion] : m_enLossSystDw_MS[muonInfo.detRegion];
      }
      else {
        scaleMS = m_scaleSyst_MS[muonInfo.detRegion];
      }
      scaleMS =  m_scale_MS[muonInfo.detRegion] + scaleVar*scaleMS;
      if( true ) enLossCorrMS = m_enLoss_MS[muonInfo.detRegion] + scaleVar*enLossCorrMS;
    }
    else {
      scaleMS = ( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) ? 0. : 1.;
    }
    if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) { //NB: MCAST::Release::Rel17_2_Sum13 has scale parametrized around 0, not 1!!
      scaleID += 1.;
      scaleMS += 1.;
    }
    if( m_scale_CB[muonInfo.detRegion] != -1 ) {
      scaleCB = m_scale_CB[muonInfo.detRegion] + scaleVar * m_scaleSyst_CB[muonInfo.detRegion];
    }
    else {
      if( muonInfo.ptms ) scaleCB = ( scaleID * muonInfo.weightID + ( scaleMS + enLossCorrMS / muonInfo.ptms ) * muonInfo.weightMS );
      else         scaleCB = scaleID; // was scaleID * muonInfo.weightID
    }

    double tmpDelta = 1.;
    double outPtID = muonInfo.ptid, outPtMS = muonInfo.ptms, outPtCB = muonInfo.ptcb;
    if( DetType == MCAST::DetectorType::ID ) {
      ATH_MSG_VERBOSE( "Checking s1_ID = " << scaleID );
      ATH_MSG_VERBOSE( "Double-Checking outPtID = " << outPtID << " at first..." );
      if( outPtID == 0. ) return outPtID;
      //Load the ID scale and smearing that you will use
      ATH_MSG_VERBOSE( "Double-Checking int( inSmearID ) = " << int( inSmearID ) );
      ATH_MSG_VERBOSE( "Double-Checking DEFAULT_INIT_VAL = " << DEFAULT_INIT_VAL );
      tmpDelta = ( int( inSmearID ) == DEFAULT_INIT_VAL ) ? 1. + muonInfo.smearDeltaID : 1. + inSmearID;
      ATH_MSG_VERBOSE( "Double-Checking inSmearID = " << inSmearID );
      ATH_MSG_VERBOSE( "Double-Checking muonInfo.smearDeltaID = " << muonInfo.smearDeltaID );
      ATH_MSG_VERBOSE( "Double-Checking tmpDelta = " << tmpDelta );
      if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
        if( m_Tsmear == MCAST::SmearingType::Pt )  outPtID = outPtID * tmpDelta;
        if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtID = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtID / tmpDelta;
      }
      outPtID = ScaleApply( fabs(outPtID), scaleID, 0., muonInfo );
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        if( m_Tsmear == MCAST::SmearingType::Pt )  outPtID = outPtID * tmpDelta;
        if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtID = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtID / tmpDelta;
      }
      return outPtID;
    }
    if( DetType == MCAST::DetectorType::MS ) {
      ATH_MSG_VERBOSE( "Checking s0_MS = " << enLossCorrMS );
      ATH_MSG_VERBOSE( "Checking s1_MS = " << scaleMS );
      ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at first..." );
      if( outPtMS == 0. ) return outPtMS;
      //Load the MS scale and smearing that you will use
      ATH_MSG_VERBOSE( "Double-Checking int( inSmearMS ) = " << int( inSmearMS ) );
      ATH_MSG_VERBOSE( "Double-Checking DEFAULT_INIT_VAL = " << DEFAULT_INIT_VAL );
      tmpDelta = ( int( inSmearMS ) == DEFAULT_INIT_VAL ) ? 1. + muonInfo.smearDeltaMS : 1. + inSmearMS;
      ATH_MSG_VERBOSE( "Double-Checking inSmearMS = " << inSmearMS );
      ATH_MSG_VERBOSE( "Double-Checking muonInfo.smearDeltaMS = " << muonInfo.smearDeltaMS );
      ATH_MSG_VERBOSE( "Double-Checking tmpDelta = " << tmpDelta );
      //In these releases the smearing was applied to the pt before the scale
      if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
        if( m_Tsmear == MCAST::SmearingType::Pt )  outPtMS = outPtMS * tmpDelta;
        if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtMS = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtMS / tmpDelta;
      }
      ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at second..." );
      outPtMS = ScaleApply( fabs( outPtMS ), scaleMS, enLossCorrMS, muonInfo );
      ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at third..." );
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        if( m_Tsmear == MCAST::SmearingType::Pt )  outPtMS = outPtMS * tmpDelta;
        if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtMS = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtMS / tmpDelta;
      }
      ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at fourth..." );
      return outPtMS;
    }
    if( DetType == MCAST::DetectorType::CB ) {
      if( int( inSmearID ) != DEFAULT_INIT_VAL && int( inSmearMS ) != DEFAULT_INIT_VAL ) {
        tmpDelta = 1. + inSmearID * muonInfo.weightID + inSmearMS * muonInfo.weightMS;
      }
      else if( int( inSmearID ) == DEFAULT_INIT_VAL && int( inSmearMS ) == DEFAULT_INIT_VAL ) {
        tmpDelta = 1. + muonInfo.smearDeltaCB;
      }
      else {
      }
      //In these releases the smearing was applied to the pt before the scale
      if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
        if( m_Tsmear == MCAST::SmearingType::Pt )  outPtCB = outPtCB * tmpDelta;
        if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtCB = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtCB / tmpDelta;
      }

      outPtCB = ScaleApply( fabs(outPtCB), scaleCB, 0., muonInfo );
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        if( m_Tsmear == MCAST::SmearingType::Pt )  outPtCB = outPtCB * tmpDelta;
        if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtCB = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtCB / tmpDelta;
      }

      return outPtCB;
    }
    return 0.;

  }

  StatusCode MuonCalibrationAndSmearingTool::FillValues() {
    ATH_MSG_VERBOSE( "Initializing lookup tables [FillValues()] : "<<m_algo<<"   "<<m_year<<"   "<<m_release );
    Clean();
    double tmpval;
    std::string tmpname;
    std::ifstream InValues;

    // Retrieving scale corrections!
    std::string scale_val;
    // Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
    if ( m_FilesPath == "" ) {
      if( m_Trel >= MCAST::Release::PreRec ) {
        scale_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Scale_" + m_algo + "_" + m_year + "_" + m_release + ".dat" );
      }
      else if( m_year == "Data11" && m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        scale_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Data11_Scale_values_" + m_algo + "_" + m_release + ".dat" );
      }
      else {
        scale_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Scale_values_" + m_algo + "_" + m_release + ".dat" );
      }
    }
    else {
      scale_val = m_FilesPath + "Scale_" + m_algo + "_" + m_year + "_" + m_release + ".dat";
    }
    //if ( m_Trel >= MCAST::Release::PreRec ) scale_val = "Scales_Test.dat";
    ATH_MSG_DEBUG( "Checking Files - Scales: " << scale_val );

    InValues.open( scale_val.c_str() );
    int i = 0;
    if( !InValues.good() ) {
      ATH_MSG_ERROR( "File " << scale_val <<" not found!!" );
      return StatusCode::FAILURE;
    }
    else {
      while( InValues.good() && i < m_nb_regions ) {
        tmpval = 0;
        if( i == 0 ) {
          getline( InValues, tmpname );
        }
        if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
          //These have a different order and Up/Dw split sys
          InValues>>tmpval;
          m_scale_ID.push_back( tmpval );
          InValues>>tmpval;
          m_enLoss_MS.push_back( tmpval );
          InValues>>tmpval;
          m_scale_MS.push_back( tmpval );
          InValues>>tmpval;
          m_scaleSystUp_ID.push_back( tmpval );
          InValues>>tmpval;
          m_enLossSystUp_MS.push_back( tmpval );
          InValues>>tmpval;
          m_scaleSystUp_MS.push_back( tmpval );
          InValues>>tmpval;
          m_scaleSystDw_ID.push_back( tmpval );
          InValues>>tmpval;
          m_enLossSystDw_MS.push_back( tmpval );
          InValues>>tmpval;
          m_scaleSystDw_MS.push_back( tmpval );
        }
        else {
          m_enLoss_MS.push_back( 0. );
          m_enLossSyst_MS.push_back( 0. );
          InValues>>tmpval;
          m_scale_ID.push_back( tmpval );
          InValues>>tmpval;
          m_scaleSyst_ID.push_back( tmpval );
          InValues>>tmpval;
          m_scale_MS.push_back( tmpval );
          InValues>>tmpval;
          m_scaleSyst_MS.push_back( tmpval );
        }
        //CB is always on the cfg file although rarely used
        InValues>>tmpval;
        m_scale_CB.push_back( tmpval );
        InValues>>tmpval;
        m_scaleSyst_CB.push_back( tmpval );
        i++;
      }
    }
    InValues.close();
    InValues.clear();

    // Retrieving data values
    std::string data_val;
    // Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
    if ( m_FilesPath == "" ) {
      if( m_Trel >= MCAST::Release::PreRec ) {
        data_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Smearing_" + m_algo + "_" + m_year + "_" + m_release + ".dat" );
      }
      else {
        data_val = PathResolverFindCalibFile( "MuonMomentumCorrections/" + m_year + "_values_" + m_algo + "_" + m_release + ".dat" );
      }
    }
    else {
      data_val = m_FilesPath + "Smearing_" + m_algo + "_" + m_year + "_" + m_release + ".dat";
    }
    //if ( m_Trel >= MCAST::Release::Test ) data_val = "Smearings_Test.dat";
    ATH_MSG_DEBUG( "Checking Files - Data: " << data_val );

    InValues.open( data_val.c_str() );
    i = 0;
    if( !InValues.good() ) {
      ATH_MSG_ERROR( "File " << data_val << " not found!!" );
      return StatusCode::FAILURE;
    }
    else {
      while( InValues.good() && i < m_nb_regions ) {
        tmpval = 0;
        if( i == 0 ) {
          getline( InValues,tmpname );
        }
        InValues>>tmpval;
        m_p1_ID.push_back( tmpval );
        InValues>>tmpval;
        m_p2_ID.push_back( tmpval );
        InValues>>tmpval;
        m_p2_ID_TAN.push_back( tmpval );
        InValues>>tmpval;
        if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
          m_p0_MS.push_back( tmpval );
          InValues>>tmpval;
        }
        else {
          m_p0_MS.push_back( 0 );
        }
        m_p1_MS.push_back( tmpval );
        InValues>>tmpval;
        m_p2_MS.push_back( tmpval );
        //Syst and stat uncertianties: they are split and symmetrized before Rel17.2Sum13, after
        // that, the stat is added in ^2 and they are split in Up/Dw
        if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
          InValues>>tmpval;
          m_SUp_p1_ID.push_back( tmpval );
          InValues>>tmpval;
          m_SUp_p2_ID.push_back( tmpval );
          InValues>>tmpval;
          m_SUp_p2_ID_TAN.push_back( tmpval );
          InValues>>tmpval;
          m_SUp_p0_MS.push_back( tmpval );
          InValues>>tmpval;
          m_SUp_p1_MS.push_back( tmpval );
          InValues>>tmpval;
          m_SUp_p2_MS.push_back( tmpval );
          InValues>>tmpval;
          m_SDw_p1_ID.push_back( tmpval );
          InValues>>tmpval;
          m_SDw_p2_ID.push_back( tmpval );
          InValues>>tmpval;
          m_SDw_p2_ID_TAN.push_back( tmpval );
          InValues>>tmpval;
          m_SDw_p0_MS.push_back( tmpval );
          InValues>>tmpval;
          m_SDw_p1_MS.push_back( tmpval );
          InValues>>tmpval;
          m_SDw_p2_MS.push_back( tmpval );
        }
        else {
          m_E_p0_MS.push_back( 0 );
          m_S_p0_MS.push_back( 0 );
          //Stat
          InValues>>tmpval;
          m_E_p1_ID.push_back( tmpval );
          InValues>>tmpval;
          m_E_p2_ID.push_back( tmpval );
          InValues>>tmpval;
          m_E_p2_ID_TAN.push_back( tmpval );
          InValues>>tmpval;
          m_E_p1_MS.push_back( tmpval );
          InValues>>tmpval;
          m_E_p2_MS.push_back( tmpval );
          /*systematic*/
          InValues>>tmpval;
          m_S_p1_ID.push_back( tmpval );
          InValues>>tmpval;
          m_S_p2_ID.push_back( tmpval );
          InValues>>tmpval;
          m_S_p2_ID_TAN.push_back( tmpval );
          InValues>>tmpval;
          m_S_p1_MS.push_back( tmpval );
          InValues>>tmpval;
          m_S_p2_MS.push_back( tmpval );
        }
      }
    }
    InValues.close();
    InValues.clear();

    // Retrieving MC values
    std::string mc_val;
    // Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
    if ( m_FilesPath == "" ) {
      if ( m_Trel >= MCAST::Release::PreRec ) {
        mc_val = PathResolverFindCalibFile( "MuonMomentumCorrections/MC_values_" + m_algo + "_" + m_year + "_" + m_release + ".dat" );
      }
      else if ( m_Trel >= MCAST::Release::PreRec ) {
        mc_val = PathResolverFindCalibFile( "MuonMomentumCorrections/MC_values_" + m_algo + "_Rel17.2Sum13.dat" );
      }
      else {
        mc_val = PathResolverFindCalibFile( "MuonMomentumCorrections/MC_values_" + m_algo + "_" + m_release + ".dat" );
      }
    }
    else {
      mc_val = m_FilesPath + "MC_values_" + m_algo + "_Rel17.2Sum13.dat";
    }
    ATH_MSG_DEBUG( "Checking Files - MC: " << mc_val );

    InValues.open( mc_val.c_str() );
    i = 0;
    if( !InValues.good() ) {
      ATH_MSG_ERROR( "File " << mc_val << " not found!!" );
      return StatusCode::FAILURE;
    }
    else {
      while( InValues.good() && i<m_nb_regions ) {
        tmpval = 0;
        if( i == 0 ) {
          getline( InValues,tmpname );
        }
        InValues>>tmpval;
        m_MC_p1_ID.push_back( tmpval );
        InValues>>tmpval;
        m_MC_p2_ID.push_back( tmpval );
        InValues>>tmpval;
        m_MC_p2_ID_TAN.push_back( tmpval );
        InValues>>tmpval;
        m_MC_p0_MS.push_back( tmpval );
        InValues>>tmpval;
        m_MC_p1_MS.push_back( tmpval );
        InValues>>tmpval;
        m_MC_p2_MS.push_back( tmpval );
      }
    }
    InValues.close();
    InValues.clear();

    return StatusCode::SUCCESS;

  }

  double MuonCalibrationAndSmearingTool::GetSmearing( int DetType, InfoHelper& muonInfo ) const {

    bool useTan2 = true;
    if ( m_Trel >= MCAST::Release::PreRec && m_Trel < MCAST::Release::Rec_2015_11_15 ) useTan2 = false;
    if ( muonInfo.detRegion < 0 || muonInfo.detRegion >= m_nb_regions ) return 0; //++++++ HOW TO IMPROVE THIS CHECK?!
    double smear = 0.;
    if ( DetType == MCAST::DetectorType::MS ) {
      if( muonInfo.ptms == 0 ) {
        return 0.;
      }
      else {
        smear = m_p0_MS[muonInfo.detRegion]*muonInfo.g0/muonInfo.ptms + m_p1_MS[muonInfo.detRegion]*muonInfo.g1 + m_p2_MS[muonInfo.detRegion]*muonInfo.g2*muonInfo.ptms;
        return smear;
      }
    }
    else if( DetType == MCAST::DetectorType::ID ) {
      if( m_Trel >= MCAST::Release::Rec_2015_11_15 ) {
        float additional_weight = 1.;
        ATH_MSG_VERBOSE( "muonInfo.eta = " << muonInfo.eta << ",  abs( muonInfo.eta ) = " << std::abs( muonInfo.eta ) << ",  fabs( muonInfo.eta ) = " << fabs( muonInfo.eta ) );
        ATH_MSG_VERBOSE( "Case 0: useTan2 && abs( muonInfo.eta ) > 2 = " << useTan2 << " && " << ( std::abs( muonInfo.eta ) > 2 ) );
        if ( useTan2 && fabs( muonInfo.eta ) > 2 ) {
          ATH_MSG_VERBOSE( "Case 1: Using p1ID = " << m_p1_ID[muonInfo.detRegion] << " and p2ID = " << m_p2_ID[muonInfo.detRegion] );
          additional_weight = sinh( muonInfo.eta ) * sinh( muonInfo.eta );
        }
        else ATH_MSG_VERBOSE( "Case 2: Using p1ID = " << m_p1_ID[muonInfo.detRegion] << " and p2ID = " << m_p2_ID[muonInfo.detRegion] );
        smear = m_p1_ID[muonInfo.detRegion]*muonInfo.g3 + m_p2_ID[muonInfo.detRegion]*muonInfo.g4*muonInfo.ptid*additional_weight;
        return smear;
      }
      else {
        if ( useTan2 && m_p2_ID_TAN[muonInfo.detRegion] != 0. ) {
          ATH_MSG_VERBOSE( "Case 3: Using p1ID = " << m_p1_ID[muonInfo.detRegion] << " and p2ID = " << m_p2_ID[muonInfo.detRegion] );
          smear = m_p1_ID[muonInfo.detRegion]*muonInfo.g3 + m_p2_ID_TAN[muonInfo.detRegion]*muonInfo.g4*muonInfo.ptid*sinh( muonInfo.eta )*sinh( muonInfo.eta );
          return smear;
        }
        else {
          ATH_MSG_VERBOSE( "Case 4: Using p1ID = " << m_p1_ID[muonInfo.detRegion] << " and p2ID = " << m_p2_ID[muonInfo.detRegion] );
          smear = m_p1_ID[muonInfo.detRegion]*muonInfo.g3 + m_p2_ID[muonInfo.detRegion]*muonInfo.g4*muonInfo.ptid;
          return smear;
        }
      }
    }
    else {
      ATH_MSG_ERROR( " DetType not defined "<<DetType );
    }
    return 0.;

  }

  void MuonCalibrationAndSmearingTool::CalcCBWeights( xAOD::Muon& mu, InfoHelper& muonInfo ) const {

    //Calculate the weights of ID and MS used for CB. Two ways are implemented:
    // - weight by ^2 of exp reso
    // - from equation pT_CB = R(pT_ID) + (1-R)(pT_MS)
    //(weights should be correctly normalized)
    muonInfo.weightID = 0.5;
    muonInfo.weightMS = 0.5;
    if ( muonInfo.detRegion < 0 || muonInfo.detRegion >= m_nb_regions ) return;
    if ( muonInfo.ptcb == 0 ) {
      ATH_MSG_DEBUG( "Combined Pt = 0" );
      return;
    }
    const bool doNotAddSmearing = true;
    double SigmaMS = ExpectedResolution( MCAST::DetectorType::MS, mu, doNotAddSmearing );
    double SigmaID = ExpectedResolution( MCAST::DetectorType::ID, mu, doNotAddSmearing );
    double Nsigma = m_useNsigmaForICombine;
    if( fabs( muonInfo.ptcb - muonInfo.ptms ) > Nsigma * SigmaMS * muonInfo.ptcb || fabs( muonInfo.ptcb - muonInfo.ptid ) > Nsigma * SigmaID * muonInfo.ptcb ) {
      double R=1, Rplus;
      if( fabs( muonInfo.ptcb-muonInfo.ptms ) == fabs( muonInfo.ptcb - muonInfo.ptid ) ) return; //This case returns weightID = weightMS = 0.5
      if( fabs( muonInfo.ptcb-muonInfo.ptms ) != 0 && fabs( muonInfo.ptcb-muonInfo.ptms ) > fabs( muonInfo.ptcb-muonInfo.ptid ) ) {
        R = ( muonInfo.ptid - muonInfo.ptcb )/( muonInfo.ptcb - muonInfo.ptms ); /* R~wMS/wID */
        Rplus = 1 + R;
        if ( Rplus != 0 && R > 0 ) {
          muonInfo.weightID = 1/Rplus;
          muonInfo.weightMS = R/Rplus;
          return;
        }
      }
      if ( fabs( muonInfo.ptcb-muonInfo.ptid ) != 0 && fabs( muonInfo.ptcb-muonInfo.ptms ) < fabs( muonInfo.ptcb-muonInfo.ptid ) ) {
        R = ( muonInfo.ptms - muonInfo.ptcb )/( muonInfo.ptcb - muonInfo.ptid ); /* R~wID/wMS */
        Rplus = 1 + R;
        if ( Rplus != 0 && R > 0 ) {
          muonInfo.weightID = R/Rplus;
          muonInfo.weightMS = 1/Rplus;
          return;
        }
      }
    }

    double wMS = muonInfo.ptms/muonInfo.ptcb/pow(SigmaMS,2);
    double wID = muonInfo.ptid/muonInfo.ptcb/pow(SigmaID,2);
    muonInfo.weightID =  wID/(wMS + wID);
    muonInfo.weightMS =  wMS/(wMS + wID);
    return;

  }

  void MuonCalibrationAndSmearingTool::CleanScales() {

    m_scaleBins.clear();

  }

  double MuonCalibrationAndSmearingTool::expectedResolution( const std::string& DetType, xAOD::Muon& mu, const bool mc ) const {
    // Expected resolution in data (or unsmeared MC if second argument is true)
    return ExpectedResolution(DetType, mu, mc);
  }

  double MuonCalibrationAndSmearingTool::expectedResolution( const int& DetType, xAOD::Muon& mu, const bool mc ) const {
    // Expected resolution in data (or unsmeared MC if second argument is true)
    return ExpectedResolution(DetType, mu, mc);
  }

  double MuonCalibrationAndSmearingTool::ExpectedResolution( const std::string &DetType, xAOD::Muon& mu, const bool mc ) const {
    // Expected resolution in data (or unsmeared MC if second argument is true)
    if( DetType == "MS" ) {
      return ExpectedResolution( MCAST::DetectorType::MS, mu, mc );
    }
    else if( DetType == "ID" ) {
      return ExpectedResolution( MCAST::DetectorType::ID, mu, mc );
    }
    else if( DetType == "CB" ) {
      return ExpectedResolution( MCAST::DetectorType::CB, mu, mc );
    }
    else {
      ATH_MSG_ERROR( "The DetType that you entered is not allows - DetType = " << DetType );
      return 0.;
    }
  }

  double MuonCalibrationAndSmearingTool::ExpectedResolution( const int DetType,xAOD::Muon& mu, const bool mc ) const {

    // get the pt measurements from the xAOD::Muon object

    double loc_ptid = 0;
    double loc_ptms = 0;
    double loc_ptcb = 0;

    // Set pt ID:

    ATH_MSG_VERBOSE( "Retrieving ElementLink to ID TrackParticle..." );
    ATH_MSG_VERBOSE( "Setting Pt  [ID]: if no track available, set to 0..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"inDetTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "inDetTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() == NULL ) = " << ( mu.inDetTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.inDetTrackParticleLink() = " << mu.inDetTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() ).isValid() = " << ( mu.inDetTrackParticleLink() ).isValid() );

    if( ( mu.inDetTrackParticleLink() ).isValid() ) {
      //if( mu.isAvailable<ElementLink<xAOD::TrackParticleContainer> > ("inDetTrackParticleLink")){
      const ElementLink< xAOD::TrackParticleContainer >& id_track = mu.inDetTrackParticleLink();
      loc_ptid = ( !id_track ) ? 0. : ( *id_track )->pt() / 1000.;
    }
    else{
      ATH_MSG_VERBOSE("The ID track link is not valid - setting pT(ID)=0");
      loc_ptid = 0.;
    }

    // Set pt MS:

    ATH_MSG_VERBOSE( "Retrieving ElementLink to MS TrackParticle..." );
    ATH_MSG_VERBOSE( "Setting Pt  [MS]: if no track available, set to 0..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"extrapolatedMuonSpectrometerTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "extrapolatedMuonSpectrometerTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.extrapolatedMuonSpectrometerTrackParticleLink() = " << mu.extrapolatedMuonSpectrometerTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() );
    if( ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() ) {
      // if( mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "extrapolatedMuonSpectrometerTrackParticleLink" ) ) {
      const ElementLink< xAOD::TrackParticleContainer >& ms_track = mu.extrapolatedMuonSpectrometerTrackParticleLink();
      loc_ptms = ( !ms_track ) ? 0. : ( *ms_track )->pt() / 1000.;
    }
    else{
      ATH_MSG_VERBOSE("No link to extrapolatedMuonSpectrometerTrackParticleLink setting pT(MS)=0");
      loc_ptms = 0.;
    }

    // Set pt CB:

    ATH_MSG_VERBOSE( "Retrieving ElementLink to CB TrackParticle..." );
    ATH_MSG_VERBOSE( "Setting Pt  [CB]: if no track available, set to 0..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"primaryTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "primaryTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.primaryTrackParticleLink() == NULL ) = " << ( mu.primaryTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.primaryTrackParticleLink() = " << mu.primaryTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.primaryTrackParticleLink() ).isValid() = " << ( mu.primaryTrackParticleLink() ).isValid() );

    if( ( mu.primaryTrackParticleLink() ).isValid() ) {
      //if( mu.isAvailable< ElementLink < xAOD::TrackParticleContainer > > ( "primaryTrackParticleLink" ) ) {
      const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.primaryTrackParticleLink();
      loc_ptcb = ( !cb_track ) ? 0. : ( *cb_track )->pt() / 1000.;
    }
    else{
      ATH_MSG_VERBOSE("The ID+MS combined track link is not valid - setting pT(ID)=0");
      loc_ptcb = 0.;
    }

    // obtain detRegion given the muon (eta,phi)
    int loc_detRegion = 0;
    double loc_eta = mu.eta();
    double loc_phi = mu.phi();

    // Getting detector region
    if( int( loc_phi ) == DEFAULT_INIT_VAL && m_Trel == MCAST::Release::Rel17_2_Sum13 ) ATH_MSG_ERROR( "You need phi of the muon for after Rel17.2Sum13!" );

    if( int( loc_phi ) == DEFAULT_INIT_VAL && m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
      //Lower release have just Eta bins, so usually the user doesn't input
      //Phi and 0 is a good default for these regions
      loc_detRegion = GetRegion( loc_eta, 0 );
    }
    else {
      loc_detRegion = GetRegion( loc_eta, loc_phi );
    }

    ATH_MSG_VERBOSE( "Getting Expected Resolution: " );
    ATH_MSG_VERBOSE( " detRegion = "<<loc_detRegion);
    ATH_MSG_VERBOSE( " ptid      = "<<loc_ptid);
    ATH_MSG_VERBOSE( " ptms      = "<<loc_ptms);
    ATH_MSG_VERBOSE( " ptcb      = "<<loc_ptcb);
    ATH_MSG_VERBOSE( " m_nb_regions = "<<m_nb_regions);
    ATH_MSG_VERBOSE( " mc = "<<mc);

    // Expected resolution in data (or unsmeared MC if second argument is true)
    bool useTan2 = true;
    /** do the average with the EXPECTED resolutions **/
    if ( loc_detRegion<0 || loc_detRegion>=m_nb_regions ) return 0;
    double expRes = 0.;
    if ( DetType == MCAST::DetectorType::MS ) {
      ATH_MSG_VERBOSE("MS resolution");
      if (loc_ptms == 0) return 1e12;
      double p0 = mc ? m_MC_p0_MS[loc_detRegion] : ( m_MC_p0_MS[loc_detRegion] + m_p0_MS[loc_detRegion] );
      double p1 = mc ? m_MC_p1_MS[loc_detRegion] : ( m_MC_p1_MS[loc_detRegion] + m_p1_MS[loc_detRegion] );
      double p2 = mc ? m_MC_p2_MS[loc_detRegion] : ( m_MC_p2_MS[loc_detRegion] + m_p2_MS[loc_detRegion] );
      ATH_MSG_VERBOSE("p0,p1,p2 = "<<p0<<"  "<<p1<<"  "<<p2);
      expRes =  sqrt( pow( p0/loc_ptms, 2 ) + pow( p1, 2 ) + pow( p2*loc_ptms ,2 ) );
      ATH_MSG_VERBOSE("expRes = "<<expRes);
      return expRes; //+++++No SYS!!!
    }
    else if ( DetType == MCAST::DetectorType::ID ) {
      ATH_MSG_VERBOSE("ID resolution");
      if ( loc_ptid == 0 ) ATH_MSG_DEBUG( "ptid == 0" );
      double p1 = mc ? m_MC_p1_ID[loc_detRegion] : ( m_MC_p1_ID[loc_detRegion] + m_p1_ID[loc_detRegion] );
      double p2 = mc ? m_MC_p2_ID[loc_detRegion] : ( m_MC_p2_ID[loc_detRegion] + m_p2_ID[loc_detRegion] );
      if ( m_MC_p2_ID_TAN[loc_detRegion] != 0 && useTan2 ) {
        p2 = mc ? m_MC_p2_ID_TAN[loc_detRegion] : ( m_MC_p2_ID_TAN[loc_detRegion] + m_p2_ID_TAN[loc_detRegion] );
        p2 = p2*sinh( mu.eta() )*sinh( mu.eta() );
      }
      ATH_MSG_VERBOSE("p1,p2 = "<<p1<<"  "<<p2);
      expRes = sqrt( pow( p1, 2 ) + pow( p2*loc_ptid ,2 ) );
      ATH_MSG_VERBOSE("expRes = "<<expRes);
      return expRes; //+++++No SYS!!!
    }
    else if ( DetType == MCAST::DetectorType::CB ) {
      ATH_MSG_VERBOSE("CB resolution");
      // Due to complicated maths, the expected combined resolution
      // is given by this equation (note: all sigmas are fractional uncertainties):
      // sigma_CB = sqrt(2) * sigma_ID * sigma_MS * pTMS * pTID / {pTCB * sqrt({sigma_ID*pTID}^2 + {sigma_MS*pTMS}^2)}
      // Do a little recursive calling to make things easier to read
      // Turn these into *absolute* uncertainties to make life easier
      double sigmaID = ExpectedResolution( MCAST::DetectorType::ID, mu, mc ) * loc_ptid;
      double sigmaMS = ExpectedResolution( MCAST::DetectorType::MS, mu, mc ) * loc_ptms;
      ATH_MSG_VERBOSE("sigmaID,sigmaMS = "<<sigmaID<<"  "<<sigmaMS);
      double denominator = ( loc_ptcb ) * sqrt( sigmaID*sigmaID + sigmaMS*sigmaMS );
      return denominator ? sqrt( 2. ) * sigmaID * sigmaMS / denominator : 0.;
    }
    else {
      ATH_MSG_ERROR( "wrong DetType in input "<<DetType );
      return 0.;
    }

  }

  void MuonCalibrationAndSmearingTool::Clean() {

    m_scale_ID.clear();
    m_enLoss_MS.clear();
    m_scale_MS.clear();
    m_scale_CB.clear();
    m_scaleSyst_ID.clear();
    m_enLossSyst_MS.clear();
    m_scaleSyst_MS.clear();
    m_scaleSyst_CB.clear();
    m_scaleSystUp_ID.clear();
    m_enLossSystUp_MS.clear();
    m_scaleSystUp_MS.clear();
    m_scaleSystDw_ID.clear();
    m_enLossSystDw_MS.clear();
    m_scaleSystDw_MS.clear();
    CleanScales();
    m_p1_ID.clear();
    m_p2_ID.clear();
    m_p2_ID_TAN.clear();
    m_p0_MS.clear();
    m_p1_MS.clear();
    m_p2_MS.clear();
    m_E_p1_ID.clear();
    m_E_p2_ID.clear();
    m_E_p2_ID_TAN.clear();
    m_E_p0_MS.clear();
    m_E_p1_MS.clear();
    m_E_p2_MS.clear();
    m_S_p1_ID.clear();
    m_S_p2_ID.clear();
    m_S_p2_ID_TAN.clear();
    m_S_p0_MS.clear();
    m_S_p1_MS.clear();
    m_S_p2_MS.clear();
    m_SUp_p1_ID.clear();
    m_SUp_p2_ID.clear();
    m_SUp_p2_ID_TAN.clear();
    m_SUp_p0_MS.clear();
    m_SUp_p1_MS.clear();
    m_SUp_p2_MS.clear();
    m_SDw_p1_ID.clear();
    m_SDw_p2_ID.clear();
    m_SDw_p2_ID_TAN.clear();
    m_SDw_p0_MS.clear();
    m_SDw_p1_MS.clear();
    m_SDw_p2_MS.clear();
    m_MC_p1_ID.clear();
    m_MC_p2_ID.clear();
    m_MC_p2_ID_TAN.clear();
    m_MC_p0_MS.clear();
    m_MC_p1_MS.clear();
    m_MC_p2_MS.clear();

  }

  void MuonCalibrationAndSmearingTool::PrintRegions() const {

    if( m_doMacroRegions ) {
      for( std::map<int, int>::const_iterator it = m_MacroRegionIdxMap.begin(); it!=m_MacroRegionIdxMap.end(); ++it ) {
        int bReg = it->first, mReg = it->second;
        ATH_MSG_INFO( "Base region n "<<bReg );
        ATH_MSG_INFO( "phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg] );
        ATH_MSG_INFO( "eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg] );
        ATH_MSG_INFO( "included in Macro region N "<<mReg<<" with innerEta="<<m_MacroRegionInnerEta[mReg] );
      }
    }
    else {
      for( int bReg=0; bReg<m_nb_regions; ++bReg ) {
        ATH_MSG_INFO( "Base region n "<<bReg<<": phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg] );
        ATH_MSG_INFO( "eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg] );
      }
    }

  }

  unsigned int MuonCalibrationAndSmearingTool::GetNRegions() const {
    if( m_doMacroRegions )
      return m_MacroRegionName.size();
    else
      return m_nb_regions;
  }

  StatusCode MuonCalibrationAndSmearingTool::Regions( std::string inRegionFile, int doMacroRegionsFlag ) {

    m_eta_min.clear();
    m_eta_max.clear();
    m_phi_min.clear();
    m_phi_max.clear();
    m_loadNames = false;
    m_doMacroRegions = false;
    std::string tmpname;
    double tmpval;
    int i = 0;
    std::ifstream inFile( inRegionFile.c_str() );
    if( !inFile.good() ) {
      ATH_MSG_ERROR( "File not good" );
      return StatusCode::FAILURE;
    }
    else {
      while( !inFile.eof() ) {
        tmpval=0;
        tmpname="";
        if( i==0 ) {
          getline( inFile,tmpname );
          //Check if I am loading the name of the regions in the file
          if( tmpname.find( "name" ) != std::string::npos && !m_loadNames ) m_loadNames = true;
        }
        else {
          inFile>>tmpval;
          if( inFile.eof() ) break; //this will break immediatly as soon as the new line is empty
          m_eta_min.push_back( tmpval );
          inFile>>tmpval;
          m_eta_max.push_back( tmpval );
          inFile>>tmpval;
          m_phi_min.push_back( tmpval );
          inFile>>tmpval;
          m_phi_max.push_back( tmpval );
          if( m_loadNames ) { //Fill the name vector only if you have them!
            inFile>>tmpname;
            m_names.push_back( tmpname );
          }
        }
        ++i;
      }
    }
    inFile.close();
    m_nb_regions = ( int ) m_eta_min.size();
    if( doMacroRegionsFlag ) { //In case I want macroRegions, I need to define the criteria here
      m_doMacroRegions = true;
      switch( doMacroRegionsFlag ) {
      case 1:
        //Collects all the Large and Small sectors if they belong to the same eta bin
        CollectMacroRegionsSL();
        break;
      case 2:
        //Large,Small sectors split plus Feet(12+14) and 11+15 sector split in Barrel
        CollectMacroRegionsSL_SplitBAR();
        break;
      case 3:
        //Collects all the Large and Small, Up and Down sectors if they belong to the same eta bin
        CollectMacroRegionsSL_UpDn();
        break;
      case 4:
        //Collects all the Large and Small, Up and Down sectors if they belong to the same eta bin
        CollectSectors();
        break;
      default:
        ATH_MSG_ERROR( "doMacroRegionFlag=" << doMacroRegionsFlag << " not defined!" );
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;

  }

  int MuonCalibrationAndSmearingTool::GetRegion( const double eta, const double phi ) const {

    ATH_MSG_VERBOSE( "eta,phi = " << eta << "," << phi );
    int ret_k =-1;
    for( int k=0; k < m_nb_regions; ++k ) {
      if( eta>=m_eta_min[k] && eta<m_eta_max[k] ) {
        if( m_phi_min[k] <= m_phi_max[k] ) { //Standard case
          if( phi>=m_phi_min[k] && phi<m_phi_max[k] ) {
            ret_k = k;
            break;
          }
        }
        else {  //Circular closure of Phi, min and max are excanged in the .dat file
          if( phi>=m_phi_min[k] || phi<m_phi_max[k] ) {
            ret_k = k;
            break;
          }
        }
      }
    }
    if( ret_k == -1 ) {
      ATH_MSG_DEBUG( "Region corresponding to Eta=" << eta << ", Phi=" << phi << " NOT FOUND!" );
      return -1;
    }
    if( m_doMacroRegions ) {
      ATH_MSG_VERBOSE( "INDEX = " << m_MacroRegionIdxMap.find( ret_k )->second );
      return m_MacroRegionIdxMap.find( ret_k )->second;
    }
    return ret_k;

  }

  float MuonCalibrationAndSmearingTool::GetRegionInnerEta( const int r_i ) const { //Return Eta closer to the origin

    if( m_doMacroRegions ) {
      if( r_i>=0 && r_i < ( int )m_MacroRegionName.size() ) return m_MacroRegionInnerEta[r_i];
    }
    else {
      if( r_i >= 0 && r_i < m_nb_regions ) {
        if( m_eta_min[r_i] >= 0. ) return m_eta_min[r_i];
        else return m_eta_max[r_i];
      }
    }
    //ATH_MSG_WARNING( "Region Inner Eta corresponding to Region index=" << r_i << " NOT FOUND!" );
    return -999.;

  }

  std::string MuonCalibrationAndSmearingTool::GetRegionName( const int r_i ) const {

    if( m_loadNames ) {
      if( m_doMacroRegions ) {
        if( r_i>=0 && r_i < ( int )m_MacroRegionName.size() ) return m_MacroRegionName[r_i];
      }
      else {
        if( r_i>=0 && r_i < m_nb_regions ) return m_names[r_i];
      }
      ATH_MSG_WARNING( "Region Name corresponding to Region index=" << r_i << " NOT FOUND!" );
    }
    ATH_MSG_WARNING( "Region Names are not set!" );
    return "NAN";

  }

  std::string MuonCalibrationAndSmearingTool::GetRegionName( const double eta, const double phi ) const {

    if( m_loadNames ) return GetRegionName( GetRegion( eta, phi ) );
    ATH_MSG_WARNING( "Region Names are not set!" );
    return "NAN";

  }

  //Collects all the Large and Small sectors if they belong to the same eta bin
  void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL() {

    double etaMin = -999., etaMax = -999.;
    int macroRegionIdx = 0;
    for( int k=0; k < m_nb_regions; ++k ) {
      if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
        //Build a new macroRegion
        etaMin = m_eta_min[k];
        etaMax = m_eta_max[k];
        //One of the following is true
        std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
        m_MacroRegionName.push_back( macroRegName+"Large" );
        m_MacroRegionName.push_back( macroRegName+"Small" );
        //insert twice the innerEta, for Large and for Small sectors
        for( int i=0; i<2; ++i ) {
          if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
          else         m_MacroRegionInnerEta.push_back( etaMax );
        }
        macroRegionIdx+=2;
      }
      if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos )
        m_MacroRegionIdxMap[k] = macroRegionIdx-2;//Large sectors
      if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos )
        m_MacroRegionIdxMap[k] = macroRegionIdx-1;//Small sectors
    }
    return;

  }

  //Collects all the Large and Small sectors if they belong to the same eta bin
  void MuonCalibrationAndSmearingTool::CollectSectors() {

    double etaMin = -999., etaMax = -999.;
    int macroRegionIdx = 0;
    for( int k=0; k < m_nb_regions; ++k ) {
      if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
        //Build a new macroRegion
        etaMin = m_eta_min[k];
        etaMax = m_eta_max[k];
        //One of the following is true
        std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
        m_MacroRegionName.push_back( macroRegName+"LargeDn" );
        m_MacroRegionName.push_back( macroRegName+"SmallDn" );
        m_MacroRegionName.push_back( macroRegName+"LargeUp" );
        m_MacroRegionName.push_back( macroRegName+"SmallUp" );
        //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
        for( int i=0; i<4; ++i ) {
          if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
          else         m_MacroRegionInnerEta.push_back( etaMax );
        }
        macroRegionIdx+=4;
      }
      if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos ) { //Large sectors
        if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "13" ) != std::string::npos ||
            m_names[k].find( "15" ) != std::string::npos ) {
          m_MacroRegionIdxMap[k] = macroRegionIdx-4;//Down Large sectors (within 10 to 16)
        }
        else {
          m_MacroRegionIdxMap[k] = macroRegionIdx-2; //Up, large sectors
        }
      }
      if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos ) { //Small sectors
        if( m_names[k].find( "10" ) != std::string::npos || m_names[k].find( "12" ) != std::string::npos ||
            m_names[k].find( "14" ) != std::string::npos || m_names[k].find( "16" ) != std::string::npos ) {
          m_MacroRegionIdxMap[k] = macroRegionIdx-3; //Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
        }
        else {
          m_MacroRegionIdxMap[k] = macroRegionIdx-1; ; //Up, Small sectors
        }
      }
    }
    return;

  }

  void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL_UpDn() {

    double etaMin = -999., etaMax = -999.;
    int macroRegionIdx = 0;
    for( int k=0; k < m_nb_regions; ++k ) {
      if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
        //Build a new macroRegion
        etaMin = m_eta_min[k];
        etaMax = m_eta_max[k];
        //One of the following is true
        std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
        m_MacroRegionName.push_back( macroRegName+"LargeDn" );
        m_MacroRegionName.push_back( macroRegName+"SmallDn" );
        m_MacroRegionName.push_back( macroRegName+"LargeUp" );
        m_MacroRegionName.push_back( macroRegName+"SmallUp" );
        //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
        for( int i=0; i<4; ++i ) {
          if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
          else         m_MacroRegionInnerEta.push_back( etaMax );
        }
        macroRegionIdx+=4;
      }
      if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos ) { //Large sectors
        if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "13" ) != std::string::npos ||
            m_names[k].find( "15" ) != std::string::npos ) {
          m_MacroRegionIdxMap[k] = macroRegionIdx-4;//Down Large sectors (within 10 to 16)
        }
        else {
          m_MacroRegionIdxMap[k] = macroRegionIdx-2; //Up, large sectors
        }
      }
      if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos ) { //Small sectors
        if( m_names[k].find( "10" ) != std::string::npos || m_names[k].find( "12" ) != std::string::npos ||
            m_names[k].find( "14" ) != std::string::npos || m_names[k].find( "16" ) != std::string::npos ) {
          m_MacroRegionIdxMap[k] = macroRegionIdx-3; //Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
        }
        else {
          m_MacroRegionIdxMap[k] = macroRegionIdx-1; ; //Up, Small sectors
        }
      }
    }
    return;

  }

  //Collects all the Large and Small sectors if they belong to the same eta bin and splits
  // the barrel 12,14 smalls and 11+15 larges that may need different calibrations
  void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL_SplitBAR() {

    double etaMin = -999., etaMax = -999.;
    int macroRegionIdx = 0;
    for( int k=0; k < m_nb_regions; ++k ) {
      if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) { //Build a new macroRegion
        etaMin = m_eta_min[k];
        etaMax = m_eta_max[k];
        std::string macroRegName ="";
        if( m_names[k].find( "EL" )!= std::string::npos || m_names[k].find( "ES" ) != std::string::npos ) {
          //We are in the End-Cap sectors so we split only Large and Small sectors
          macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
          macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
          //insert 4 time the innerEta, for Large and Small sectors
          m_MacroRegionName.push_back( macroRegName+"Large" );
          m_MacroRegionName.push_back( macroRegName+"Small" );
          for( int i=0; i<2; ++i ) {
            if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
            else               m_MacroRegionInnerEta.push_back( etaMax );
          }
          macroRegionIdx+=2;
        }
        else {  //We are in the Barrels sectors so we split Large, Small, 12+14, 11+15 sectors
          macroRegName = m_names[k].substr( 0, m_names[k].find( "BL" ) );
          macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
          //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
          m_MacroRegionName.push_back( macroRegName+"Large" );
          m_MacroRegionName.push_back( macroRegName+"Small" );
          m_MacroRegionName.push_back( macroRegName+"Large11_15" );
          m_MacroRegionName.push_back( macroRegName+"SmallFeet" );
          for( int i=0; i<4; ++i ) {
            if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
            else               m_MacroRegionInnerEta.push_back( etaMax );
          }
          macroRegionIdx+=4;
        }
      }
      if( m_names[k].find( "EL" ) != std::string::npos ) { //End-Cap Large sectors
        m_MacroRegionIdxMap[k] = macroRegionIdx-2;
      }
      else if( m_names[k].find( "ES" ) != std::string::npos ) { //End-Cap Small sectors
        m_MacroRegionIdxMap[k] = macroRegionIdx-1; //
      }
      else if ( m_names[k].find( "BL" ) != std::string::npos ) { //Barrel Large sectors
        if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "15" ) != std::string::npos ) {
          m_MacroRegionIdxMap[k] = macroRegionIdx-2;//Barrel Large sectors with different alignment (11,15)
        }
        else {
          m_MacroRegionIdxMap[k] = macroRegionIdx-4; //Standard Barrel Large sectors
        }
      }
      else if( m_names[k].find( "BS" ) != std::string::npos ) { //Barrel Small sectors
        if( m_names[k].find( "12" ) != std::string::npos || m_names[k].find( "14" ) != std::string::npos ) {
          m_MacroRegionIdxMap[k] = macroRegionIdx-1; //Feet Small sectors (12+14, Down).
        }
        else {
          m_MacroRegionIdxMap[k] = macroRegionIdx-3; //All standard Barrel Small sectors
        }
      }
    }
    return;

  }

  double MuonCalibrationAndSmearingTool::GetSystVariation( int DetType, double var, InfoHelper& muonInfo ) const {

    if( var != 1. && var != -1 ) {
      ATH_MSG_WARNING( "Systematic variation is not +/- 1 sigma, returning 0." );
      return 0;
    }
    if( muonInfo.detRegion < 0 || muonInfo.detRegion >= m_nb_regions ) return 0;

    double p0_MS_var = 0., p1_MS_var = 0., p2_MS_var = 0., p1_ID_var = 0., p2_ID_var = 0., p2_ID_TAN_var = 0.;
    double newSmear = 0.;
    if( DetType == MCAST::DetectorType::MS ) {
      if( muonInfo.ptms == 0 ) {
        return 0;
      } else {
        if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
          p0_MS_var = pow( m_E_p0_MS[ muonInfo.detRegion ] * m_E_p0_MS[ muonInfo.detRegion ] + m_S_p0_MS[ muonInfo.detRegion ] * m_S_p0_MS[ muonInfo.detRegion ], 0.5 );
          p1_MS_var = pow( m_E_p1_MS[ muonInfo.detRegion] * m_E_p1_MS[ muonInfo.detRegion ] + m_S_p1_MS[ muonInfo.detRegion ] * m_S_p1_MS[ muonInfo.detRegion ], 0.5 );
          p2_MS_var = pow( m_E_p2_MS[ muonInfo.detRegion] * m_E_p2_MS[ muonInfo.detRegion ] + m_S_p2_MS[ muonInfo.detRegion ] * m_S_p2_MS[ muonInfo.detRegion ], 0.5 );
        } else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
          p0_MS_var = var > 0. ? m_SUp_p0_MS[ muonInfo.detRegion ] : m_SDw_p0_MS[ muonInfo.detRegion ];
          p1_MS_var = var > 0. ? m_SUp_p1_MS[ muonInfo.detRegion ] : m_SDw_p1_MS[ muonInfo.detRegion ];
          p2_MS_var = var > 0. ? m_SUp_p2_MS[ muonInfo.detRegion ] : m_SDw_p2_MS[ muonInfo.detRegion ];
        }

        p0_MS_var = m_p0_MS[ muonInfo.detRegion ] + var * p0_MS_var;
        p1_MS_var = m_p1_MS[ muonInfo.detRegion ] + var * p1_MS_var;
        p2_MS_var = m_p2_MS[ muonInfo.detRegion ] + var * p2_MS_var;

        if( p0_MS_var < 0. ) p0_MS_var = 0.; //Truncation of unphysical variations
        if( p1_MS_var < 0. ) p1_MS_var = 0.;
        if( p2_MS_var < 0. ) p2_MS_var = 0.;
        newSmear = ( p0_MS_var * muonInfo.g0 / muonInfo.ptms + p1_MS_var * muonInfo.g1 + p2_MS_var * muonInfo.g2 * muonInfo.ptms );
        return newSmear;
      }
    } else if( DetType == MCAST::DetectorType::ID ) {
      if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
        p1_ID_var     = pow( m_E_p1_ID[ muonInfo.detRegion ] * m_E_p1_ID[ muonInfo.detRegion ] + m_S_p1_ID[ muonInfo.detRegion ] * m_S_p1_ID[ muonInfo.detRegion ], 0.5 );
        p2_ID_var     = pow( m_E_p2_ID[ muonInfo.detRegion ] * m_E_p2_ID[ muonInfo.detRegion ] + m_S_p2_ID[ muonInfo.detRegion ] * m_S_p2_ID[ muonInfo.detRegion ], 0.5 );
        p2_ID_TAN_var = pow( m_E_p2_ID_TAN[ muonInfo.detRegion ] * m_E_p2_ID_TAN[ muonInfo.detRegion ] + m_S_p2_ID_TAN[ muonInfo.detRegion ] * m_S_p2_ID_TAN[ muonInfo.detRegion ], 0.5 );
      } else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
        p1_ID_var     = var > 0. ? m_SUp_p1_ID[ muonInfo.detRegion ] : m_SDw_p1_ID[ muonInfo.detRegion ];
        p2_ID_var     = var > 0. ? m_SUp_p2_ID[ muonInfo.detRegion ] : m_SDw_p2_ID[ muonInfo.detRegion ];
        p2_ID_TAN_var = var > 0. ? m_SUp_p2_ID_TAN[ muonInfo.detRegion ] : m_SDw_p2_ID_TAN[ muonInfo.detRegion ];
      }
      p1_ID_var     = m_p1_ID[ muonInfo.detRegion ] + var * p1_ID_var;
      p2_ID_var     = m_p2_ID[ muonInfo.detRegion ] + var * p2_ID_var;
      p2_ID_TAN_var = m_p2_ID_TAN[ muonInfo.detRegion ] + var * p2_ID_TAN_var;

      if( p1_ID_var < 0. ) p1_ID_var = 0.; //Truncation of unphysical variations
      if( p2_ID_var < 0. ) p2_ID_var = 0.;
      if( p2_ID_TAN_var < 0. ) p2_ID_TAN_var = 0.;

      bool useTan2 = true;
      if( useTan2 && m_p2_ID_TAN[ muonInfo.detRegion ] != 0 ) {
        newSmear = ( p1_ID_var * muonInfo.g3 + p2_ID_TAN_var * muonInfo.g4 * muonInfo.ptid * sinh( muonInfo.eta ) * sinh( muonInfo.eta ) );
      } else {
        newSmear = ( p1_ID_var * muonInfo.g3 + p2_ID_var * muonInfo.g4 * muonInfo.ptid );
      }
      return newSmear;
    }
    return 0;
  }


  CorrectionCode MuonCalibrationAndSmearingTool::applyStatCombination( xAOD::Muon& mu, InfoHelper& muonInfo ) const {

    // Set pt ID:
    ATH_MSG_VERBOSE( "Stat comb: Retrieving ElementLink to ID TrackParticle..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"inDetTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "inDetTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() == NULL ) = " << ( mu.inDetTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.inDetTrackParticleLink() = " << mu.inDetTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() ).isValid() = " << ( mu.inDetTrackParticleLink() ).isValid() );


    if( !( mu.inDetTrackParticleLink() ).isValid() )
      return CorrectionCode::Error;

    const ElementLink< xAOD::TrackParticleContainer >& id_track=mu.inDetTrackParticleLink();

    // Set pt MS:

    ATH_MSG_VERBOSE( "Stat comb: Retrieving ElementLink to MS TrackParticle..." );
    ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"extrapolatedMuonSpectrometerTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "extrapolatedMuonSpectrometerTrackParticleLink" ) );
    ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) );
    ATH_MSG_VERBOSE( "mu.extrapolatedMuonSpectrometerTrackParticleLink() = " << mu.extrapolatedMuonSpectrometerTrackParticleLink() );
    ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() );

    if( !( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() )
      return CorrectionCode::Error;

    const ElementLink< xAOD::TrackParticleContainer >& ms_track = mu.extrapolatedMuonSpectrometerTrackParticleLink();

    // Set the charge
    int charge = mu.charge();

    // SAF specifics
    if( mu.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) {
      if( mu.isAvailable<ElementLink<xAOD::TrackParticleContainer > > ("combinedTrackParticleLink")){
        if(mu.combinedTrackParticleLink().isValid()){
          if(mu.combinedTrackParticleLink().isValid()){
            const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.combinedTrackParticleLink();
            muonInfo.charge = ( *cb_track )->charge();
          }
        }
      }}

    const xAOD::TrackParticle*       cbtrack = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
    AmgVector(5) parsCB=cbtrack->definingParameters();
    AmgSymMatrix(5) covCB=cbtrack->definingParametersCovMatrix();
    double chi2=-999;


    if(applyStatCombination(id_track,ms_track,charge,parsCB,covCB,chi2)!=CorrectionCode::Ok)
      return CorrectionCode::Error;

    muonInfo.cbParsA.clear();
    muonInfo.cbCovMat.clear();

    for(unsigned int i=0 ; i < 5; i++)
      muonInfo.cbParsA.push_back(parsCB[i]);

    for(unsigned int i=0 ; i < 5; i++)
      for(unsigned int j=0 ; j < 5; j++)
        muonInfo.cbCovMat.push_back(covCB(i,j));

    if(m_doNotUseAMGMATRIXDECOR){
      mu.auxdata< std::vector <float> > ( "StatCombCBParsVector" ) =  muonInfo.cbParsA;
      mu.auxdata< std::vector <float > > ( "StatCombCBCovarianceVector" ) = muonInfo.cbCovMat;
    }
    if(!m_doNotUseAMGMATRIXDECOR){
      mu.auxdata< AmgVector(5) >( "StatCombCBPars" ) = parsCB;
      mu.auxdata< AmgSymMatrix(5) >( "StatCombCBCovariance" ) = covCB;
    }
    mu.auxdata< double > ("StatCombChi2") = chi2;

    return CorrectionCode::Ok;
  }

  CorrectionCode MuonCalibrationAndSmearingTool::applyStatCombination( const ElementLink< xAOD::TrackParticleContainer >& inDetTrackParticle,
                                                                       const ElementLink< xAOD::TrackParticleContainer >& extrTrackParticle,
                                                                       int charge,
                                                                       AmgVector(5)& parsCB,
                                                                       AmgSymMatrix(5)& covCB,
                                                                       double& chi2) const {

    chi2   = 1e20;
    AmgVector(5) parsID = (*inDetTrackParticle)->definingParameters();;
    parsID[4] = fabs(parsID[4]);

    AmgVector(5) parsMS = (*extrTrackParticle)->definingParameters();;
    //int chargeMS = parsMS[4]/fabs(parsMS[4]);
    parsMS[4] = fabs(parsMS[4]);

    AmgSymMatrix(5) covID = (*inDetTrackParticle)->definingParametersCovMatrix();

    const AmgSymMatrix(5)  weightID = covID.inverse();
    if  ( weightID.determinant() == 0 ){
      ATH_MSG_WARNING( " ID weight matrix computation failed     " ) ;
      return CorrectionCode::Error;
    }

    AmgSymMatrix(5) covMS = (*extrTrackParticle)->definingParametersCovMatrix();

    // Error inflation to account for ID-MS misaligment
    double dSigma[5] = { 1.0, 2.0/sin(parsMS[3]), 0.001, 0.001, 0.0};
    double factor[5];
    for ( int i = 0; i<5 ; i++ ) {
      factor[i] = sqrt((covMS(i,i)+dSigma[i]*dSigma[i])/covMS(i,i));
    }
    for ( int i = 0 ; i<5 ; i++ ) {
      for ( int j = 0 ; j<5 ; j++ ) {
        if ( i==j ) {
          covMS(i,j) = covMS(i,j)*factor[i]*factor[j];
        }
      }
    }

    const AmgSymMatrix(5)  weightMS = covMS.inverse();
    if  ( weightMS.determinant() == 0 ){
      ATH_MSG_WARNING( "weightMS computation failed      " ) ;
      return CorrectionCode::Error;
    }

    AmgSymMatrix(5) weightCB = weightID + weightMS ;
    covCB = weightCB.inverse();
    if (covCB.determinant() == 0){
      ATH_MSG_WARNING( " Inversion of weightCB failed " ) ;
      return CorrectionCode::Error;
    }


    AmgSymMatrix(5) covSum = covID + covMS ;
    AmgSymMatrix(5) invCovSum = covSum.inverse();

    if (invCovSum.determinant() == 0){
      ATH_MSG_WARNING( " Inversion of covSum failed " ) ;
      return CorrectionCode::Error;
    }

    double  diffPhi = parsMS[2] - parsID[2] ;
    if(diffPhi>M_PI)       parsMS[2] -= 2.*M_PI;
    else if(diffPhi<-M_PI) parsMS[2] += 2.*M_PI;

    AmgVector(5) diffPars = parsID - parsMS;
    chi2 = diffPars.transpose() * invCovSum * diffPars;
    chi2 = chi2/5. ;

    parsCB = covCB * ( weightID * parsID + weightMS * parsMS ) ;
    parsCB[4] *= charge;

    if(parsCB[2]>M_PI)       parsCB[2] -= 2.*M_PI;
    else if(parsCB[2]<-M_PI) parsCB[2] += 2.*M_PI;
    return CorrectionCode::Ok;
  }

  //   void MuonCalibrationAndSmearingTool::setUseStatCombination(bool flag){
  //     m_useStatComb=flag;
  //   }


  bool MuonCalibrationAndSmearingTool::isBadMuon( const xAOD::Muon& mu, InfoHelper& muonInfo ) const {

    if( mu.muonType() != xAOD::Muon::Combined ) return false;
    // ::
    const xAOD::TrackParticle*       idtrack = mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
    const xAOD::TrackParticle*       metrack = mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
    const xAOD::TrackParticle*       cbtrack = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );

    // ::
    bool IsBadMuon = false;
    if( idtrack && metrack && cbtrack ) {
      ATH_MSG_VERBOSE("ME ERROR "<<sqrt( metrack->definingParametersCovMatrix()(4,4) )<<" ID ERROR"<<sqrt( idtrack->definingParametersCovMatrix()(4,4) )<<" CB error "<<sqrt( cbtrack->definingParametersCovMatrix()(4,4) ));
      // ::
      double qOverP_ME = metrack->qOverP();
      double qOverPerr_ME = sqrt( metrack->definingParametersCovMatrix()(4,4) );
      // ::
      // recipe for high-pt selection
      // From Peter ( qOverPerr_ME*1000. > sqrt( pow(0.07*fabs(qOverP_ME*1000),2) + pow(0.0005*sin(metrack->theta()),2) ) );
      // an other recipie....
      ATH_MSG_VERBOSE("fabs("<<qOverPerr_ME<<"/"<<qOverP_ME<<") < sqrt( pow(8/"<<muonInfo.ptms<<",2) + pow(0.07,2) + pow(0.0005*"<<muonInfo.ptms<<",2))");

      IsBadMuon = fabs(qOverPerr_ME / qOverP_ME)<sqrt( pow(8/muonInfo.ptms,2) + pow(0.07,2) + pow(0.0005*muonInfo.ptms,2));
    }
    ATH_MSG_VERBOSE( std::string(IsBadMuon ? " IS BAD MUON ": " IS GOOD MUON"));
    return IsBadMuon;
  }


} // namespace CP
