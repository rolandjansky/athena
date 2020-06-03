/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/********************************************************************

NAME:     IDPerfMonEoverP.cxx
PACKAGE:  InDetPerformanceMonitoring

AUTHORS:  A. Morley
CREATED:  Jul 2011

PURPOSE:  Create  a simple ntuple to perform EoverP studies with
********************************************************************/

// INCLUDE HEADER FILES:
#include "egammaInterfaces/IegammaTrkRefitterTool.h"

// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include <algorithm>
#include <math.h>
#include <vector>
// Validation mode - TTree includes
#include "GaudiKernel/ITHistSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "TrigDecisionTool/TrigDecisionTool.h"


//#include "JetUtils/JetCaloQualityUtils.h"
#include "InDetPerformanceMonitoring/IDPerfMonEoverP.h"

#include "TTree.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLorentzVector.h"

using namespace std;
using namespace Trk;
//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

IDPerfMonEoverP::IDPerfMonEoverP(const std::string& name,
  ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_TrackRefitter(""),
  m_TrackRefitter_no2(""),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_refitEverything(true),
  m_isDATA(true),
  m_validationMode(true),
  m_fillDetailedTree(false),
  m_validationTreeName("EGrefitter"),
  m_validationTreeDescription("egamma track refitter results"),
  m_validationTreeFolder("/eoverpValidation/efitterValidation"),
  m_validationTree(0),
  m_electronCounter(0),
  m_ZeeLooseMassOS_Cluster(0),
  m_ZeeLooseMassSS_Cluster(0),
  m_ZeeMediumMassOS_Cluster(0),
  m_ZeeMediumMassSS_Cluster(0),
  m_ZeeMediumOS_ClusterPtEta(0),
  m_WenuLooseElectronET(0),
  m_WenuTight_Met(0),
  m_WenuTight_MT(0),
  m_WenuTightMet_MT(0),
  m_WenuTightElectronET(0),
  m_WenuTightW_PT(0),
  m_WenuTightElectron_PTEtaPos(0),
  m_WenuTightElectron_PTEtaNeg(0),
  m_smallValidationTreeName("EGrefitterSmall"),
  m_smallValidationTreeDescription("Small Tree for E/p fits"),
  m_smallValidationTreeFolder("/eoverpValidation2/efitterValidation2"),
  m_smallValidationTree(0),
  m_isxAOD(false)
{

// The following properties are specified at run-time
// (declared in jobOptions file)
  declareProperty("RefitTracks",                      m_refitEverything);
  declareProperty("isDATA", m_isDATA);
  declareProperty("ReFitterTool",                     m_TrackRefitter, "ToolHandle for track fitter implementation");
  declareProperty("ReFitterTool2",                    m_TrackRefitter_no2, "ToolHandle for track fitter implementation");
  declareProperty("TrigDecisionTool",                 m_trigDec, "The TrigDecisionTool instance.");
  declareProperty("InputElectronContainerName",       m_InputElectronContainerName  = "Electrons");
  declareProperty("InputJetContainerName",            m_jetContainerName  = "AntiKt4LCTopoJets");
  //  declareProperty("MissingEtObjectName",              m_missingEtObjectName  = "MET_RefFinal");
  declareProperty("METFinalName",                     m_metRefFinalName= "FinalClus");
  declareProperty("MissingEtObjectName",              m_missingEtObjectName  = "MET_Reference_AntiKt4LCTopo");
  declareProperty("primaryVertexCollection",          m_primaryVertexCollection = "PrimaryVertices");
  declareProperty("RefittedElectronTrackContainer1",  m_OutputTrackCollectionName_no1="GSFTracks");
  declareProperty("RefittedElectronTrackContainer2",  m_OutputTrackCollectionName_no2="DNATracks");
  declareProperty("ValidationMode",                   m_validationMode);
  declareProperty("FillDetailedTree",                 m_fillDetailedTree);
  declareProperty("ElectronLikelihoodTune",           m_lhTune="mc15_20150712");
  declareProperty("isxAOD",                           m_isxAOD);
}

// DESTRUCTOR:

IDPerfMonEoverP::~IDPerfMonEoverP()
{

}

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:

StatusCode IDPerfMonEoverP::initialize()
{

// MSGStream object to output messages from your algorithm

  ATH_MSG_INFO("Initializing IDPerfMonEoverP");
  // Retrieve Jet selector tool
  m_jetCleaningTool.setTypeAndName("JetCleaningTool/JetCleaningTool");
  CHECK( m_jetCleaningTool.retrieve() );

  // Retrieve fitter
  if (m_TrackRefitter.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter );

  // Retrieve the second fitter
  if (m_TrackRefitter_no2.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter_no2 );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter_no2 );
  }

  if(m_trigDec.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_trigDec << " turn it off");
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO("Retrieved tool" << m_trigDec );


  //
  //  Electron PID Variables
  //
  // Following are of ShowerShapeType
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::f1, "f1"));        //0
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::f1core,"f1core")); //1
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::emins1,"Emins1")); //2
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::fracs1,"fside"));  //3
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::e2tsts1,"Emax2"));//4
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::weta1,"ws3")); //5
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::wtots1,"wstot"));//6
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::emaxs1,"emaxs1"));//7
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::e233,"E233")); //8
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::e237,"E237")); //9
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::e277,"E277"));//10
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::weta2,"weta2"));//11
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::f3,"f3"));//12
  m_PID_ShowerType_Names.push_back( std::make_pair( xAOD::EgammaParameters::f3core,"f3core"));//13
  // Following are of isolation type
  //  m_PID_IsolationType_Names.push_back( std::make_pair( xAOD::Iso::etcone45,"Etcone45"));//14
  m_PID_IsolationType_Names.push_back( std::make_pair( xAOD::Iso::etcone20,"Etcone20"));//15
  m_PID_IsolationType_Names.push_back( std::make_pair( xAOD::Iso::etcone30,"Etcone30"));//16
  m_PID_IsolationType_Names.push_back( std::make_pair( xAOD::Iso::etcone40,"Etcone40"));//17
  m_PID_IsolationType_Names.push_back( std::make_pair( xAOD::Iso::ptcone30,"ptcone30"));  //18
  // Following are of trackcalomatch type
  m_PID_TrackCaloMatchType_Names.push_back( std::make_pair( xAOD::EgammaParameters::deltaEta1,"deltaeta1"));//19
  m_PID_TrackCaloMatchType_Names.push_back( std::make_pair( xAOD::EgammaParameters::deltaEta2,"deltaeta2"));//20
  m_PID_TrackCaloMatchType_Names.push_back( std::make_pair( xAOD::EgammaParameters::deltaPhi2,"deltaphi2"));//21
  //  m_PID_TrackCaloMatchType_Names.push_back( std::make_pair( xAOD::EgammaParameters::deltaPhiRescaled,"deltaphiRescaled"));//22
  // Following are of summary type
  //m_PID_SummaryType_Names.push_back( std::make_pair(xAOD::expectBLayerHit,"expectHitInBLayer"));//23 // throwing an error for some reason
  //m_PID_SummaryType_Names.push_back( std::make_pair(xAOD::numberOfBLayerHits+xAOD::numberOfBLayerOutliers,"BLayerHitsPlusOutliers"));//24


  //
  //  Trigger
  //
  m_triggerNames.push_back("L1_MBTS_1");
  m_triggerNames.push_back("L1_EM2");
  m_triggerNames.push_back("L1_2EM2");
  m_triggerNames.push_back("L1_2EM4");
  m_triggerNames.push_back("L1_2EM5");
  m_triggerNames.push_back("L1_EM3");
  m_triggerNames.push_back("L1_EM4");
  m_triggerNames.push_back("L1_EM5");
  m_triggerNames.push_back("L1_EM10");
  m_triggerNames.push_back("L1_EM10I");
  m_triggerNames.push_back("L1_EM14");
  m_triggerNames.push_back("L1_EM18");
  m_triggerNames.push_back("L1_EM18I");
  m_triggerNames.push_back("L1_EM23I");
  m_triggerNames.push_back("L1_EM100");
  m_triggerNames.push_back("EF_2e5_medium");
  m_triggerNames.push_back("EF_e10_medium");
  m_triggerNames.push_back("EF_e20_loose");
  m_triggerNames.push_back("EF_em105_passHLT");
  m_triggerNames.push_back("EF_g20_loose");
  m_triggerNames.push_back("EF_e20_medium");
  m_triggerNames.push_back("EF_e22_medium");
  // run 2 test
  m_triggerNames.push_back("HLT_e24_lhmedium_idperf_L1EM20VH");
  m_triggerNames.push_back("HLT_e24_medium_L1EM18VH");
  m_triggerNames.push_back("e5_vloose");
  m_triggerNames.push_back("ALL");
  m_triggerNames.push_back("HLT_ALL");


  // If the validation nuptle has been requested Setup the ntuple
  if (m_validationMode){

    if (m_validationTree == 0 ){
      // create the new Tree
      m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

      std::string FitterNames[3] = {"GX2","Refitted1","Refitted2"};


      m_validationTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
      m_validationTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
      m_validationTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");


      // counter for number of electrons
      m_validationTree->Branch("nElectrons"      ,  &m_nelectrons,  "nElectrons/I");

      // Track fitter information
      for(int fitter(0); fitter < 3; ++fitter){
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_Theta").c_str()   ,  m_electronTheta[fitter], std::string(FitterNames[fitter]+"_Theta0[nElectrons]/F").c_str() );
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_Phi0").c_str()    ,  m_electronPhi[fitter], std::string(FitterNames[fitter]+"_Phi0[nElectrons]/F").c_str() );
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_QoverP").c_str()  ,  m_electronQoverP[fitter], std::string(FitterNames[fitter]+"_QoverP[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_d0").c_str()      ,  m_electrond0[fitter], std::string(FitterNames[fitter]+"_d0[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_z0").c_str()      ,  m_electronz0[fitter], std::string(FitterNames[fitter]+"_z0[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_LMQoverP").c_str()  ,  m_electronLMQoverP[fitter], std::string(FitterNames[fitter]+"_LMQoverP[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_ThetaErr").c_str()   ,  m_electronErrTheta[fitter], std::string(FitterNames[fitter]+"_Theta0Err[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_Phi0Err").c_str()    ,  m_electronErrPhi[fitter], std::string(FitterNames[fitter]+"_Phi0Err[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_QoverPErr").c_str()  ,  m_electronErrQoverP[fitter], std::string(FitterNames[fitter]+"_QoverPErr[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_d0Err").c_str()      ,  m_electronErrd0[fitter], std::string(FitterNames[fitter]+"_d0Err[nElectrons]/F").c_str());
        m_validationTree->Branch(std::string(FitterNames[fitter]+"_z0Err").c_str()      ,  m_electronErrz0[fitter], std::string(FitterNames[fitter]+"_z0Err[nElectrons]/F").c_str());
      }

      m_validationTree->Branch("associatedToVtx" ,  m_associatedToVtx, "associatedToVtx[nElectrons]/I");
      m_validationTree->Branch("nbpv",    &m_nbpv, "nbpv/I");
      m_validationTree->Branch("pvtype",  m_pvtype, "pvtype[nbpv]/I");
      m_validationTree->Branch("pvnbtk",  m_pvnbtk, "pvnbtk[nbpv]/I");
      m_validationTree->Branch("pvsumpt", m_pvsumpt, "pvsumpt[nbpv]/F");
      m_validationTree->Branch("pvx", m_pvx, "pvx[nbpv]/F");
      m_validationTree->Branch("pvy", m_pvy, "pvy[nbpv]/F");
      m_validationTree->Branch("pvz", m_pvz, "pvz[nbpv]/F");
      m_validationTree->Branch("errpvx", m_errpvx, "errpvx[nbpv]/F");
      m_validationTree->Branch("errpvy", m_errpvy, "errpvy[nbpv]/F");
      m_validationTree->Branch("errpvz", m_errpvz, "errpvz[nbpv]/F");
      m_validationTree->Branch("covpvxpvy", m_covpvxpvy, "covpvxpvy[nbpv]/F");
      m_validationTree->Branch("covpvzpvx", m_covpvzpvx, "covpvzpvx[nbpv]/F");
      m_validationTree->Branch("covpvypvz", m_covpvypvz, "covpvypvz[nbpv]/F");


      m_validationTree->Branch("METgoodness",    &m_METgoodness, "METgoodness/B");
      m_validationTree->Branch("sumet",    &m_sumet, "sumet/F");
      m_validationTree->Branch("missingEt",    &m_missingEt, "missingEt/F");
      m_validationTree->Branch("missingEtx",    &m_missingEtx, "missingEtx/F");
      m_validationTree->Branch("missingEty",    &m_missingEty, "missingEty/F");


      m_validationTree->Branch("ElectronAuthor"     ,  m_author, "ElectronAuthor[nElectrons]/I");
      m_validationTree->Branch("ClusterEnergy"      ,  m_ClusterEnergy, "ClusterEnergy[nElectrons]/F");
      m_validationTree->Branch("ClusterEta"         ,  m_ClusterEta, "ClusterEta[nElectrons]/F");
      m_validationTree->Branch("ClusterPhi"         ,  m_ClusterPhi, "ClusterPhi[nElectrons]/F");

      m_validationTree->Branch("IsEMLoose"          ,  m_IsEMLoose, "m_IsEMLoose[nElectrons]/B");
      m_validationTree->Branch("IsEMMedium"         ,  m_IsEMMedium, "m_IsEMMedium[nElectrons]/B");
      m_validationTree->Branch("IsEMTight"          ,  m_IsEMTight, "m_IsEMTight[nElectrons]/B");
      m_validationTree->Branch("isGoodOQ"          ,  m_isGoodOQ, "isGoodOQ[nElectrons]/B");

      m_validationTree->Branch("nTRT"               , m_nTRT , "nTRT[nElectrons]/I");
      m_validationTree->Branch("nSCT"               , m_nSCT, "nSCT[nElectrons]/I");
      m_validationTree->Branch("nBLayer"            , m_nBLayer, "nBLayer[nElectrons]/I");
      m_validationTree->Branch("nBLayerOutlier"     , m_nBLayerOutlier,"nBLayerOutlier[nElectrons]/I");
      m_validationTree->Branch("nexpectBLayer"      , m_nexpectBLayer, "nexpectBLayer[nElectrons]/I");
      m_validationTree->Branch("nBLayerHitPlusOutlier",m_nBLayerHitPlusOutlier,"nBLayerHitPlusOutlier[nElectrons]/I");
      m_validationTree->Branch("nPIX"               , m_nPIX, "nPIX[nElectrons]/I");
      m_validationTree->Branch("nTRTout"            , m_nTRTout, "nTRTout[nElectrons]/I");
      m_validationTree->Branch("nSCTout"            , m_nSCTout, "nSCTout[nElectrons]/I");
      m_validationTree->Branch("nPIXout"            , m_nPIXout, "nPIXout[nElectrons]/I");
      m_validationTree->Branch("nTRTHT"             , m_nTRTHT, "nTRTHT[nElectrons]/I");
      m_validationTree->Branch("nTRTHout"           , m_nTRTHTout, "nTRTHTout[nElectrons]/I");


      for(unsigned int item(0); item < m_PID_ShowerType_Names.size(); ++item){
        m_validationTree->Branch( m_PID_ShowerType_Names[item].second.c_str()  , m_ePID_ShowerType[item], std::string( m_PID_ShowerType_Names[item].second + "[nElectrons]/F").c_str() );
      }

      for(unsigned int item(0); item < m_PID_IsolationType_Names.size(); ++item){
        m_validationTree->Branch( m_PID_IsolationType_Names[item].second.c_str()  , m_ePID_IsolationType[item], std::string( m_PID_IsolationType_Names[item].second + "[nElectrons]/F").c_str() );
      }

      for(unsigned int item(0); item < m_PID_TrackCaloMatchType_Names.size(); ++item){
        m_validationTree->Branch( m_PID_TrackCaloMatchType_Names[item].second.c_str()  , m_ePID_TrackCaloMatchType[item], std::string( m_PID_TrackCaloMatchType_Names[item].second + "[nElectrons]/F").c_str() );
      }

      for(unsigned int item(0); item < m_PID_SummaryType_Names.size(); ++item){
        m_validationTree->Branch( m_PID_SummaryType_Names[item].second.c_str()  , m_ePID_SummaryType[item], std::string( m_PID_SummaryType_Names[item].second + "[nElectrons]/F").c_str() );
      }

      for(unsigned int item(0); item < m_triggerNames.size(); ++item){
        m_validationTree->Branch( m_triggerNames[item].c_str()  , &m_trigger[item], std::string( m_triggerNames[item]+ "/B" ).c_str() );
      }

    }


    if(m_smallValidationTree == 0){

      m_smallValidationTree = new TTree(m_smallValidationTreeName.c_str(), m_smallValidationTreeDescription.c_str());

      m_smallValidationTree->Branch("charge",&m_smallCharge,"charge/D");
      m_smallValidationTree->Branch("runNumber"  ,  &m_runNumber,  "runNumber/I");
      m_smallValidationTree->Branch("Default_QoverP",&m_small_QoverP,"Default_QoverP/D");
      m_smallValidationTree->Branch("Refitted1_QoverP",&m_small1_QoverP,"Refitted1_QoverP/D");
      m_smallValidationTree->Branch("Refitted2_QoverP",&m_small2_QoverP,"Refitted2_QoverP/D");
      m_smallValidationTree->Branch("ClusterEnergy",&m_smallClusterEnergy,"ClusterEnergy/D");
      m_smallValidationTree->Branch("ClusterEta"   ,&m_smallClusterEta,"ClusterEta/D");
      m_smallValidationTree->Branch("ClusterPhi"   ,&m_smallClusterPhi,"ClusterPhi/D");
      m_smallValidationTree->Branch("TrackTheta"   ,&m_smallTrackTheta,"TrackTheta/D");
      //m_smallValidationTree->Branch("d0"           ,&m_smalld0,"d0/D");
      //m_smallValidationTree->Branch("z0"           ,&m_smallz0,"z0/D");
    }

      // now register the Tree
    ITHistSvc* tHistSvc = 0;
    if (service("THistSvc",tHistSvc).isFailure()){
      ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
      delete m_validationTree;      m_validationTree = 0;
      delete m_smallValidationTree; m_smallValidationTree = 0;
      m_validationMode = false;
    }
    if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_validationTree; m_validationTree = 0;
      m_validationMode = false;
    }
    if ((tHistSvc->regTree(m_smallValidationTreeFolder, m_smallValidationTree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_smallValidationTree; m_smallValidationTree = 0;
      m_validationMode = false;
    } else {

      m_ZeeLooseMassOS_Cluster = new TH1F("ZeeMassLooseOS","ZeeMassLooseOS", 120, 60000 ,120000);
      m_ZeeLooseMassSS_Cluster  = new TH1F("ZeeMassLooseSS","ZeeMassLooseSS", 120, 60000 ,120000);
      m_ZeeMediumMassOS_Cluster = new TH1F("ZeeMassMediumOS","ZeeMassMediumOS", 120, 60000 ,120000);
      m_ZeeMediumMassSS_Cluster  = new TH1F("ZeeMassMediumSS","ZeeMassMediumSS", 120, 60000 ,120000);
      m_WenuLooseElectronET= new TH1F("WenuLooseElectronET","WenuLooseElectronET", 80, 20000 ,100000);
      m_WenuTight_Met = new TH1F("WenuTight_Met","WenuTight_Met", 120, 00000 ,120000);
      m_WenuTight_MT = new TH1F("WenuTight_MT","WenuTight_MT", 120, 00000 ,120000);
      m_WenuTightMet_MT  = new TH1F("WenuTightMet_MT","WenuTightMet_MT", 120, 00000 ,120000);
      m_WenuTightElectronET  = new TH1F("WenuTightElectronET","WenuTightElectronET", 100, 00000 ,100000);
      m_WenuTightW_PT  = new TH1F("WenuTightW_PT","WenuTightW_PT", 100, 00000 ,100000);

      m_LHResult = new TH1F("LHResult","LHResult (all/none/l/m/t)",5,0,5);

      m_ZeeMediumOS_ClusterPtEta   = new TH2F("ZeeMediumOS_ClusterPtEta", "ZeeMediumOS_ClusterPtEta",100,20000,120000,50,-2.5,2.5);
      m_WenuTightElectron_PTEtaPos = new TH2F("WenuTightElectron_PTEtaPos", "WenuTightElectron_PTEtaPos",100,20000,120000,50,-2.5,2.5);
      m_WenuTightElectron_PTEtaNeg = new TH2F("WenuTightElectron_PTEtaNeg", "WenuTightElectron_PTEtaNeg",100,20000,120000,50,-2.5,2.5);

      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/ZeeMassLooseOS",m_ZeeLooseMassOS_Cluster )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/ZeeMassLooseSS",m_ZeeLooseMassSS_Cluster )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/ZeeMassMediumOS",m_ZeeMediumMassOS_Cluster )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/ZeeMassMediumSS",m_ZeeMediumMassSS_Cluster )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuLooseElectronET",m_WenuLooseElectronET )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuTight_Met",m_WenuTight_Met )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuTight_MT",m_WenuTight_MT )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuTightMet_MT",m_WenuTightMet_MT )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuTightElectronET",m_WenuTightElectronET )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuTightW_PT",m_WenuTightW_PT )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/ZeeMediumOS_ClusterPtEta",m_ZeeMediumOS_ClusterPtEta )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuTightElectron_PTEtaPos",m_WenuTightElectron_PTEtaPos )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/WenuTightElectron_PTEtaNeg",m_WenuTightElectron_PTEtaNeg )).isFailure() )
        ATH_MSG_ERROR("initialize() Could not register histogram ");
      if ( (tHistSvc->regHist(m_smallValidationTreeFolder + "/LHResult",m_LHResult )).isFailure() )
	ATH_MSG_ERROR("initialize() Could not register histogram LHResult");

      ATH_MSG_INFO("Booked Small Tree add histograms");

    }

    ATH_MSG_INFO("Loaded THistSvc");
  }

  //---Electron Likelihood tool---
  ATH_MSG_INFO("IDPerfMonEoverP::Initialize() -- Setting up electron LH tool.");
  m_LHToolLoose2015  = new AsgElectronLikelihoodTool ("m_LHToolLoose2015");
  m_LHToolMedium2015 = new AsgElectronLikelihoodTool ("m_LHToolMedium2015");
  m_LHToolTight2015  = new AsgElectronLikelihoodTool ("m_LHToolTight2015");

  if((m_LHToolLoose2015->setProperty("primaryVertexContainer",m_primaryVertexCollection)).isFailure())
    ATH_MSG_WARNING("Failure setting primary vertex container " << m_primaryVertexCollection << "in loose electron likelihood tool");
  if((m_LHToolMedium2015->setProperty("primaryVertexContainer",m_primaryVertexCollection)).isFailure())
    ATH_MSG_WARNING("Failure setting primary vertex container " << m_primaryVertexCollection << "in medium electron likelihood tool");
  if((m_LHToolTight2015->setProperty("primaryVertexContainer",m_primaryVertexCollection)).isFailure())
    ATH_MSG_WARNING("Failure setting primary vertex container " << m_primaryVertexCollection << "in tight electron likelihood tool");

  //Set up electron LH level
  /* Depricated -- I will restore this to functionality at some point in case we ever need to use old tunes
  std::string confDir = "ElectronPhotonSelectorTools/offline/"+m_lhTune+"/";
  std::string configFileL = confDir+"ElectronLikelihoodLooseOfflineConfig2015.conf";
  std::string configFileM = confDir+"ElectronLikelihoodMediumOfflineConfig2015.conf";
  std::string configFileT = confDir+"ElectronLikelihoodTightOfflineConfig2015.conf";

  if((m_LHToolLoose2015->setProperty("ConfigFile",configFileL)).isFailure())
    ATH_MSG_WARNING("Failure loading ConfigFile in loose electron likelihood tool.");
  if((m_LHToolMedium2015->setProperty("ConfigFile",configFileM)).isFailure())
    ATH_MSG_WARNING("Failure loading ConfigFile in medium electron likelihood tool.");
  if((m_LHToolTight2015->setProperty("ConfigFile",configFileT)).isFailure())
    ATH_MSG_WARNING("Failure loading ConfigFile in tight electron likelihood tool.");
  */

  if((m_LHToolLoose2015->setProperty("WorkingPoint","LooseLHElectron")).isFailure())
    ATH_MSG_WARNING("Failure loading ConfigFile in loose electron likelihood tool.");
  if((m_LHToolMedium2015->setProperty("WorkingPoint","MediumLHElectron")).isFailure())
    ATH_MSG_WARNING("Failure loading ConfigFile in medium electron likelihood tool.");
  if((m_LHToolTight2015->setProperty("WorkingPoint","TightLHElectron")).isFailure())
    ATH_MSG_WARNING("Failure loading ConfigFile in tight electron likelihood tool.");

  StatusCode lhl = m_LHToolLoose2015->initialize();
  if(lhl.isFailure())
    ATH_MSG_WARNING("Loose electron likelihood tool initialize() failed!");
  StatusCode lhm = m_LHToolMedium2015->initialize();
  if(lhm.isFailure())
    ATH_MSG_WARNING("Medium electron likelihood tool initialize() failed!");
  StatusCode lht = m_LHToolTight2015->initialize();
  if(lht.isFailure())
    ATH_MSG_WARNING("Tight electron likelihood tool initialize() failed!");

  ATH_MSG_INFO( "Initialization completed successfully");
  return StatusCode::SUCCESS;

}

// FINALIZE METHOD:

StatusCode IDPerfMonEoverP::finalize()
{

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:

StatusCode IDPerfMonEoverP::execute()
{
  ATH_MSG_VERBOSE( "Executing IDPerfMonEoverP" );

  StatusCode sc(StatusCode::SUCCESS);

  m_trackParticleVertexMap.clear();
  m_primaryVertexFirstCandidate = 0;

  if (m_validationMode) clearValidationNtuple();
  else{
    ATH_MSG_WARNING( "  Validation Mode has been turned off ALG will do not fill Ntuples" );
    return sc;
  }

  ATH_MSG_VERBOSE("Retrieving event info.");
  const EventInfo * eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure())
    ATH_MSG_ERROR("Could not retrieve event info.");
  else
  {
    m_runNumber = eventInfo->event_ID()->run_number();
    m_evtNumber = eventInfo->event_ID()->event_number();
    m_lumi_block = eventInfo->event_ID()->lumi_block();
  }

  ATH_MSG_VERBOSE("Retrieved Trigger info.");
  fillTriggerInformation();

  if ( fillVertexInformation() ){
    ATH_MSG_VERBOSE("Retrieved Primary Vertex info.");
  } else {
    ATH_MSG_DEBUG("No Primary Vertex");
  }

  ATH_MSG_VERBOSE("MET info.being stored");
  if( storeMETinformation() )
    ATH_MSG_DEBUG("MET info. stored");




  // Get the electron AOD container
  const xAOD::ElectronContainer* ElectronInput_container;

  sc =  evtStore()->retrieve(ElectronInput_container, m_InputElectronContainerName);
  if (sc!=StatusCode::SUCCESS){
    ATH_MSG_WARNING("No electron container");
    deleteAction();
    return StatusCode::SUCCESS;
  }


  if (ElectronInput_container->size() < 1 ){
    deleteAction();
    return StatusCode::SUCCESS;
  }

  m_refittedTracks_no1 = new TrackCollection;
  m_refittedTracks_no2 = new TrackCollection;


  typedef xAOD::ElectronContainer::const_iterator electron_iterator;
  electron_iterator iter    = ElectronInput_container->begin();
  electron_iterator iterEnd = ElectronInput_container->end();

  // Loop over the Electrons
  ATH_MSG_VERBOSE("Electron info. being stored");
  //std::cout << "No Electrons: " << ElectronInput_container->size() <<std::endl;
  for(; iter != iterEnd ; iter++) {

    if (m_electronCounter >= NOS_ELECTRONS) break;

    //if ((*iter)->author() == 8) continue;
    const xAOD::Electron *ELE = (*iter);
    m_author[m_electronCounter] = ELE->author(xAOD::EgammaParameters::AuthorElectron);

    // Cluster Info
    fillElectronInfo ( ELE );
    // Fill General info
    fillGeneral( ELE );
    // Fill IsEm info
    fillIsEM( ELE );


    //Get the track particle
    const xAOD::TrackParticle* mytp = (*iter)->trackParticle();
    if(mytp){
      uint8_t dummy(-1);
      m_nTRT[m_electronCounter]      = mytp->summaryValue(  dummy , xAOD::numberOfTRTHits  )? dummy :-1;
      m_nSCT[m_electronCounter]      = mytp->summaryValue(  dummy , xAOD::numberOfSCTHits )? dummy :-1;
      m_nPIX[m_electronCounter]      = mytp->summaryValue(  dummy , xAOD::numberOfPixelHits )? dummy :-1;
      m_nBLayer[m_electronCounter]        = mytp->summaryValue(  dummy , xAOD::numberOfBLayerHits)? dummy :-1;
      m_nBLayerOutlier[m_electronCounter] = mytp->summaryValue(  dummy , xAOD::numberOfBLayerOutliers)? dummy :-1;
      m_nexpectBLayer[m_electronCounter]  = mytp->summaryValue(  dummy , xAOD::expectBLayerHit)?dummy :-1;
      m_nBLayerHitPlusOutlier[m_electronCounter] = m_nBLayer[m_electronCounter] + m_nBLayerOutlier[m_electronCounter];
      m_nTRTout[m_electronCounter]   = mytp->summaryValue(  dummy , xAOD::numberOfTRTOutliers)? dummy :-1;
      m_nSCTout[m_electronCounter]   = mytp->summaryValue(  dummy , xAOD::numberOfSCTOutliers)? dummy :-1;
      m_nTRTHT[m_electronCounter]    = mytp->summaryValue(  dummy , xAOD::numberOfTRTHighThresholdHits)? dummy :-1;
      m_nTRTHTout[m_electronCounter] = mytp->summaryValue(  dummy , xAOD::numberOfTRTHighThresholdOutliers)? dummy :-1;
    } else{
      ATH_MSG_DEBUG("Electron with no track particle??   Possibly Forward");
      continue;
    }

    //Find which if any vertex the electron track is associated to
    VxPos myVxPos = findAssociatedVertex( (*iter) );
    m_associatedToVtx[m_electronCounter] = myVxPos.second;

    if( m_isxAOD ){
      // if it's an xAOD, we don't have a track particle, so we can't fill all the cool stuff
      // Instead we'll just fill the small ntuple with the xAOD quantities
      addToValidationNtuplexAOD(ELE);
    }

    else{
      if( mytp->track() ){
	const Trk::Track* oTrkTrack = mytp->track();
	if (oTrkTrack){
	  const Trk::Perigee* oMeasPer =  oTrkTrack->perigeeParameters() ;
	  if (oMeasPer) addToValidationNtuple( oMeasPer, (*iter)->caloCluster(), 0 );
	  fillLastMeasurement( oTrkTrack , 0 );
	}
      }
      else {
	ATH_MSG_DEBUG( "Could not retrieve track from track particle" );
      }
      
      
      if(m_refitEverything) {
	// First Refitter................
	
	ATH_MSG_VERBOSE(  "Refitting the track" );
	
	StatusCode sc = m_TrackRefitter->refitElectronTrack( (*iter) );
	
	if (sc == StatusCode::SUCCESS){
	  m_refittedTracks_no1->push_back(m_TrackRefitter->refittedTrack());
	  addToValidationNtuple( m_TrackRefitter->refittedTrackPerigee() ,(*iter)->caloCluster(), 1 );
	  fillLastMeasurement( m_TrackRefitter->refittedTrack(), 1 );
	} else {
	  ATH_MSG_DEBUG(  "Track Refit Failed" );
	}
	
	
	//******************************************************//
	// Refit tracks using the second refitter if it is present
	//******************************************************//
	ATH_MSG_VERBOSE(  "Refitting the track again" );
	
	sc = m_TrackRefitter_no2->refitElectronTrack( (*iter) );
	
	if (sc == StatusCode::SUCCESS){
	  //Add the refitted track to the TrackCollection
	  m_refittedTracks_no2->push_back( m_TrackRefitter_no2->refittedTrack() );
	  //Add data to the trkRefitterNtuple
	  addToValidationNtuple( m_TrackRefitter_no2->refittedTrackPerigee() ,(*iter)->caloCluster(), 2 );
	  fillLastMeasurement( m_TrackRefitter_no2->refittedTrack(), 2 );
	} else {
	  ATH_MSG_DEBUG( "Track Refit Failed" );
	}
      } else {
	
	ATH_MSG_DEBUG(  "Not Refitting the track -- DO NOTHING" );
	
      }
    }// end if not xAOD

      //Increment the electron counter for the validation nutple
    ++m_electronCounter;
  }
  
  // Commit Data to Ntuple;
  if( m_validationMode ) validationAction();

  // Fill much smaller tree
  std::vector<int> goodElectrons = FillSimpleTree();


  TrackCollection* selectedElectrons = new TrackCollection;

  for( unsigned int i(0); i < goodElectrons.size(); ++i){
    StatusCode sc = m_TrackRefitter->refitElectronTrack( (*ElectronInput_container)[goodElectrons[i]] );
    if (sc == StatusCode::SUCCESS){
      selectedElectrons->push_back(m_TrackRefitter->refittedTrack());
    }
	}

  //******************************************************//
  //* Add the newly created TrackCollection to StoreGate *//
  //******************************************************//

  sc = evtStore()->record( m_refittedTracks_no1, m_OutputTrackCollectionName_no1, false );
  if (sc.isFailure())
  {
    ATH_MSG_ERROR("Could not record "<< m_OutputTrackCollectionName_no1 <<" object.");
    return (StatusCode::FAILURE);
  }

  sc = evtStore()->record( m_refittedTracks_no2, m_OutputTrackCollectionName_no2, false );
  if (sc.isFailure())
  {
    ATH_MSG_ERROR("Could not record "<< m_OutputTrackCollectionName_no2 <<" object.");
    return (StatusCode::FAILURE);
  }

  sc = evtStore()->record( selectedElectrons, m_OutputTrackCollectionName_no1 + "Selected", false );
	if (sc.isFailure())
  {
    ATH_MSG_ERROR("Could not record "<< m_OutputTrackCollectionName_no1+"Selected" <<" object.");
    return (StatusCode::FAILURE);
  }


  ATH_MSG_VERBOSE(  "execute completed successfully");

  return StatusCode::SUCCESS;
}

void IDPerfMonEoverP::addToValidationNtuplexAOD(const xAOD::Electron* ele) const{
  const xAOD::TrackParticle* ele_track = ele->trackParticle();
  m_electrond0[0][m_electronCounter]     = ele_track->d0();
  m_electronz0[0][m_electronCounter]     = ele_track->z0();
  m_electronPhi[0][m_electronCounter]    = ele_track->phi0();
  m_electronTheta[0][m_electronCounter]  = ele_track->theta();
  m_electronQoverP[0][m_electronCounter] = ele_track->qOverP();
  for(int i=1;i<3;i++){ // fill the "refit" information with 0 for obvious reasons
    m_electrond0[i][m_electronCounter]     = 0;
    m_electronz0[i][m_electronCounter]     = 0;
    m_electronPhi[i][m_electronCounter]    = 0;
    m_electronTheta[i][m_electronCounter]  = 0;
    m_electronQoverP[i][m_electronCounter] = 0;
  }
  return;
}

void IDPerfMonEoverP::addToValidationNtuple(const Trk::Perigee* perigee,const xAOD::CaloCluster* /*cluster*/, int isOriginal){
  if (perigee){
    ATH_MSG_VERBOSE(  "Adding data to ntuple" );
    if (isOriginal >=  0  &&  isOriginal <3){

      m_electrond0[isOriginal][m_electronCounter]     = perigee->parameters()[Trk::d0];
      m_electronz0[isOriginal][m_electronCounter]     = perigee->parameters()[Trk::z0];
      m_electronPhi[isOriginal][m_electronCounter]    = perigee->parameters()[Trk::phi0];
      m_electronTheta[isOriginal][m_electronCounter]  = perigee->parameters()[Trk::theta];
      m_electronQoverP[isOriginal][m_electronCounter] = perigee->parameters()[Trk::qOverP];


      const AmgSymMatrix(5)* matrix = perigee->covariance();
      if (matrix){
        m_electronErrd0[isOriginal][m_electronCounter]     = sqrt( (*matrix)(Trk::d0,Trk::d0) );
        m_electronErrz0[isOriginal][m_electronCounter]     = sqrt( (*matrix)(Trk::z0,Trk::z0) );
        m_electronErrPhi[isOriginal][m_electronCounter]    = sqrt( (*matrix)(Trk::phi0,Trk::phi0) );
        m_electronErrTheta[isOriginal][m_electronCounter]  = sqrt( (*matrix)(Trk::theta,Trk::theta) );
        m_electronErrQoverP[isOriginal][m_electronCounter] = sqrt( (*matrix)(Trk::qOverP,Trk::qOverP) );
      }
    }
  } else {
   ATH_MSG_DEBUG(  "No MeasuredPerigee - cannot add data to ntuple" );
  }

   ATH_MSG_VERBOSE(  "Finished adding data to ntuple" );

}




void IDPerfMonEoverP::fillIsEM(const xAOD::Electron *eg) const
{

  ATH_MSG_DEBUG(  "fillIsEM" );
  int el_goodOQ = (int)eg->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON);

  m_isGoodOQ[m_electronCounter] = el_goodOQ;

  m_LHResult->Fill(0.);

  // check loose LH
  bool val_loose=m_LHToolLoose2015->accept(eg);
  //ATH_MSG_DEBUG( "Loose value : " << val_loose);
  if(val_loose){
    m_IsEMLoose[m_electronCounter] = val_loose;
    m_LHResult->Fill(2.);
  }

  // check medium LH
  bool val_med=m_LHToolMedium2015->accept(eg);
  //ATH_MSG_DEBUG( "Medium value : " << val_med );
  if(val_med){
    m_IsEMMedium[m_electronCounter] = val_med;
    m_LHResult->Fill(3.);
  }

  // check tight LH
  bool val_tight=m_LHToolTight2015->accept(eg);
  //ATH_MSG_DEBUG( "Tight value : " << val_tight);
  if(val_tight){
    m_IsEMTight[m_electronCounter] = val_tight;
    m_LHResult->Fill(4.);
  }

  if( (val_loose+val_med+val_tight) == 0 )
    m_LHResult->Fill(1.);

  // print out result for debug purposes
  ATH_MSG_DEBUG( "Tight/Medium/Loose: " << val_tight << val_med << val_loose );
  
  return;

}


void IDPerfMonEoverP::fillGeneral(const xAOD::Electron *eg) const
{
  ATH_MSG_VERBOSE(  "fillGeneral" );
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if(!cluster) return;
  m_ClusterEnergy[m_electronCounter] = cluster->e();
  m_ClusterPhi[m_electronCounter] = cluster->phi();
  m_ClusterEta[m_electronCounter] = cluster->eta();
}




void IDPerfMonEoverP::clearValidationNtuple()
{
  ATH_MSG_DEBUG(  "clearValidationNtuple" );

  for (int i = 0 ;  i < NOS_ELECTRONS ; ++i){
    for (int j = 0 ;  j <3 ; ++j){
      m_electrond0[j][i]        = 0;
      m_electronz0[j][i]        = 0;
      m_electronPhi[j][i]       = 0;
      m_electronTheta[j][i]     = 0;
      m_electronQoverP[j][i]    = 0;
      m_electronLMQoverP[j][i]    = 0;

      m_electronErrd0[j][i]     = 0;
      m_electronErrz0[j][i]     = 0;
      m_electronErrPhi[j][i]    = 0;
      m_electronErrTheta[j][i]  = 0;
      m_electronErrQoverP[j][i] = 0;

    }
    m_associatedToVtx[i] = 0;

    m_author[i]               = 0;

    m_ClusterEnergy[i]        = 0;

    m_IsEMLoose[i]            = false;
    m_IsEMMedium[i]           = false;
    m_IsEMTight[i]            = false;
    m_IsEMTightTRT[i]         = false;


    for (int j = 0 ;  j < 50 ; ++j){
      m_ePID_ShowerType[j][i] =0;
      m_ePID_IsolationType[j][i] =0;
      m_ePID_TrackCaloMatchType[j][i] =0;
      m_ePID_SummaryType[j][i] =0;
      m_trigger[j] = false;
    }
  }


  //Vertex information
  m_nbpv =0;
  for (int i(0); i< NO_PV; ++i){
    m_pvtype[i] =0;
    m_pvnbtk[i]=0;
    m_pvsumpt[i]=0;
    m_pvx[i]=0;
    m_pvy[i]=0;
    m_pvz[i]=0;
    m_errpvx[i]=0;
    m_errpvy[i]=0;
    m_errpvz[i]=0;
    m_covpvxpvy[i]=0;
    m_covpvypvz[i]=0;
    m_covpvzpvx[i]=0;
  }
  //MET
  m_METgoodness=false;
  m_sumet=0;
  m_missingEt=0;
  m_missingEtx=0;
  m_missingEty=0;

}




void IDPerfMonEoverP::deleteAction() const
{

}

void IDPerfMonEoverP::validationAction() const
{
  ATH_MSG_DEBUG( "Writing Data to ntuple" );

  // first record the values
  if (m_validationTree){
    m_nelectrons = long(m_electronCounter);
    if(m_fillDetailedTree)m_validationTree->Fill();
    // then reset
    m_electronCounter = 0;
  }
}


bool IDPerfMonEoverP::storeMETinformation() const
{
  ATH_MSG_VERBOSE("In storeMETinformation()");
  const xAOD::MissingETContainer *pMissingCont(0);
  const xAOD::MissingET *MET;
  if (!evtStore()->contains<xAOD::MissingETContainer>(m_missingEtObjectName)){
    ATH_MSG_WARNING("No collection with name " << m_missingEtObjectName << " found in StoreGate");
    return false;
  }

  StatusCode sc;
  sc = evtStore()->retrieve(pMissingCont,m_missingEtObjectName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve MissingETContainer" );
    return false;
  }
  MET = (*pMissingCont)[m_metRefFinalName];
  m_METgoodness = passMETCleaningCuts();
  m_sumet       = MET->sumet();
  m_missingEt   = MET->met();
  m_missingEtx  = MET->mpx();
  m_missingEty  = MET->mpy();
  return true;
}


bool IDPerfMonEoverP::passMETCleaningCuts() const
{


  const xAOD::JetContainer* jetTDS = 0;

  StatusCode sc= evtStore()->retrieve( jetTDS, m_jetContainerName);
  if( sc.isFailure()  ||  !jetTDS ) {
    ATH_MSG_WARNING("No " << m_jetContainerName << " jet container found in TDS");
    return false;
  }

  if (jetTDS->size() == 0) return true;

  bool cleanJet = true;
  for(const auto* jet_elem : *jetTDS ) {
    if(!m_jetCleaningTool->keep( *jet_elem )) cleanJet=false;
    if(!cleanJet) break;
  }
  return cleanJet;
}



//  bool badMET = false;

//  xAOD::JetContainer::const_iterator jetItr  = jetTDS->begin();
//xAOD::JetContainer::const_iterator jetItrE = jetTDS->end();


//for(; jetItr != jetItrE; ++jetItr)
//  {
//    double jetpt= (*jetItr)->pt(P4SignalState::JETFINAL);
//    if (jetpt<=20000) continue;
//    if (JetCaloQualityUtils::isBad( (*jetItr), false ) ) badMET =true;
//    if (badMET) break;
//  }
//return !badMET;
//  }

  //  ATH_MSG_WARNING("Need to reimplement MET CLeaning for xAOD ");
  //  return true;
  //MET cleaning
  // bool isBad = false;
  // const xAOD::JetContainer *jetTDS(0);
  ////  bool printedErrorJetCollection = false;
  //if (evtStore()->contains<xAOD::JetContainer>(m_jetContainerName)) {
  //StatusCode sc= evtStore()->retrieve( jetTDS, m_jetContainerName);
  //if( sc.isFailure()  ||  !jetTDS ) {
  //  ATH_MSG_WARNING("No " <<m_jetContainerName << " jet container found in TDS");
      //     isBad=true;
  //  return false;
  //}

  //if (jetTDS->size() == 0) return true;
  ////  else{
  ////    ATH_MSG_WARNING("Could not find JetContainer");
  //// if (!printedErrorJetCollection) ATH_MSG_WARNING( "No jet collection found in TDS" );
  //// printedErrorJetCollection = true;
  //// isBad = true;
  //// return StatusCode::SUCCESS;
  //// }
  //ATH_MSG_DEBUG( "JetCollection successfully retrieved" );
  //return true;
  //}
  //else{
  //return true;
  //}
  //}

//  bool badMET = false;

//  JetCollection::const_iterator jetItr  = jetTDS->begin();
//  JetCollection::const_iterator jetItrE = jetTDS->end();
//  for(const auto* jet_elem : *jetTDS ) {
//  for(; jetItr != jetItrE; ++jetItr)
//  {
//  const xAOD::Jet *jet = jet_elem;
//  double jetpt= jet->pt(P4SignalState::JETFINAL);
//  if (jetpt<=20000) continue;
//  if (JetCaloQualityUtils::isBad( jet, false ) ) badMET =true;
//  if (badMET) break;
//}
//return !badMET;
//}

bool IDPerfMonEoverP::fillVertexInformation() const
{
  ATH_MSG_DEBUG( "fillVertexInformation()" );
  const xAOD::VertexContainer* vxContainer(0);
  int npv = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_primaryVertexCollection);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Could not retrieve primary vertex info: " << m_primaryVertexCollection );
    return false;
  } else {
    ATH_MSG_DEBUG( "Found primary vertex info: " << m_primaryVertexCollection );
    if(vxContainer) {
      ATH_MSG_DEBUG("Nb of reco primary vertex for coll "
		    << " = " << vxContainer->size() );
      m_primaryVertexFirstCandidate = std::begin(*vxContainer)[0];
      ATH_MSG_DEBUG( "The primary vertex : " << m_primaryVertexFirstCandidate->type() );
      for(const auto* vxI : *vxContainer ) {
        int type = (int)(vxI)->vertexType();
	const xAOD::Vertex* primaryVertex = vxI;
        int nbtk = 0;
	const std::vector< ElementLink< xAOD::TrackParticleContainer > > tpLinks =  vxI->trackParticleLinks();
	float sumpt = 0.;
	if(tpLinks.size() > 0) {
	  nbtk = tpLinks.size();
	  for(const auto& tp_elem : tpLinks ){
	    const xAOD::TrackParticle* trk = *tp_elem;
	    VxPos myVxPos = std::make_pair(vxI,npv);
	    m_trackParticleVertexMap.insert( std::make_pair( trk, myVxPos )  );
	    if(trk) {
	      sumpt += trk->p4().Perp();
	    }
	  }
	  ATH_MSG_VERBOSE( "Reco PV " << npv << ": ("
			   << primaryVertex->position().x() << ","
			   << primaryVertex->position().y() << ","
			   << primaryVertex->position().z() << ")"
			   << " type=" << type
			   << " nbtk=" << nbtk
			   << " sumpt=" << sumpt);
	  
	  if(npv >= NO_PV) {
	    ATH_MSG_WARNING( "More than " << NO_PV << " reconstructed primary vertices in event "
			     << "for run= " << m_runNumber << " evt= " << m_evtNumber
			     << ". Truncated event. " );
	  } 
	  else {
	    m_pvtype[npv] = type;
	    m_pvnbtk[npv] = nbtk;
	    m_pvsumpt[npv] = sumpt;
	    m_pvx[npv] = primaryVertex->position().x();
	    m_pvy[npv] = primaryVertex->position().y();
	    m_pvz[npv] = primaryVertex->position().z();
	    //m_errpvx[npv] = sqrt( (primaryVertex->covariancePosition())(Trk::x,Trk::x));
	    //m_errpvy[npv] = sqrt( (primaryVertex->covariancePosition())(Trk::y,Trk::y));
	    //m_errpvz[npv] = sqrt( (primaryVertex->covariancePosition())(Trk::z,Trk::z));
	    //m_covpvxpvy[npv] = (primaryVertex->covariancePosition())(Trk::x,Trk::y);
	    //m_covpvypvz[npv] = (primaryVertex->covariancePosition())(Trk::y,Trk::z);
	    //m_covpvzpvx[npv] = (primaryVertex->covariancePosition())(Trk::z,Trk::x);
	  }
	  ++npv;
	} 
	else {
	  ATH_MSG_DEBUG( "Vertex " << npv << " has no tracks associated to it!"  );
	}
      }
    } else {
      ATH_MSG_DEBUG( "No container in collection?? " << m_primaryVertexCollection );
    }
  }
  m_nbpv = npv;

  ATH_MSG_DEBUG("Done filling Vertex information");

  if (npv == 0)  return false;
  return true;
}

VxPos IDPerfMonEoverP::findAssociatedVertex(const xAOD::Electron* eg) const
{
  ATH_MSG_VERBOSE("In findAssociatedVertex()");

  std::map<const xAOD::TrackParticle*, VxPos>::iterator tpVx =
    m_trackParticleVertexMap.find(eg->trackParticle());


  if (tpVx == m_trackParticleVertexMap.end() ){
    return std::make_pair( m_primaryVertexFirstCandidate,-1 );
  } else
    return (*tpVx).second;

}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//get the TrackParticleBase, TrackParticleBase points back to the original track on ESD for all hypos and this has the original tracks, not the ones modified by the vertex fit
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//const xAOD::TrackParticle* IDPerfMonEoverP::getTrackParticle(const Trk::VxTrackAtVertex *trkAtVx)const
// {

//   //find the link to the TrackParticleBase
//   const Trk::ITrackLink* trkLink = trkAtVx->trackOrParticleLink();
//   const Trk::TrackParticleBase* trkPB(0);
//   if(0!= trkLink){
//     const Trk::LinkToTrackParticleBase* linktrkPB = dynamic_cast<const Trk::LinkToTrackParticleBase *>(trkLink);
//     if(0!= linktrkPB){
//       if(linktrkPB->isValid()) trkPB = linktrkPB->cachedElement();
//     }//end of dynamic_cast check
//   }//end of ITrackLink existance check

//   //cast to TrackParticle
//   if(trkPB){
//     const xAOD::TrackParticle* trkP = dynamic_cast<const xAOD::TrackParticle*>(trkPB);
//     return trkP;
//   }else{
//     return 0;
//   }
// }


void IDPerfMonEoverP::fillTriggerInformation() const
{
  ATH_MSG_VERBOSE("In fillTriggerInformation()");

  //std::cout << "Filling Trigger Info!" << std::endl;
  ATH_MSG_VERBOSE( "Pass state All = " << m_trigDec->isPassed( ".*" ) );
  ATH_MSG_VERBOSE( "Pass state L1 = " << m_trigDec->isPassed( "L1_.*" ) );
  ATH_MSG_VERBOSE( "Pass state L2 = " << m_trigDec->isPassed( "L2_.*" ) );
  ATH_MSG_VERBOSE( "Pass state EF = " << m_trigDec->isPassed( "EF_.*" ) );
  ATH_MSG_VERBOSE("HLT_.* is passed: " << m_trigDec->isPassed("HLT_.*"));

  ///
  //  auto cg = m_trigDec->getChainGroup("HLT_.*");
  //auto feat = cg->features();

  //std::vector<Trig::Feature<xAOD::TrigElectronContainer> > el_feats = feat.get<xAOD::TrigElectronContainer>();
  //ATH_MSG_INFO("found " << el_feats.size() << " el features");

  //for(auto& feat : el_feats){
  //  cout << "  -> TrigElectronContainer: " << feat.label() << endl;
  //  for(auto e : *feat.cptr()){
  //    ATH_MSG_INFO("feature pointing to: " << feat.cptr() << " pt: " << e->pt());
  //  }
  //}
  ///

  for (unsigned int i=0; i < m_triggerNames.size(); ++i){
    if(m_triggerNames[i] == "ALL")
      m_trigger[i] = m_trigDec->isPassed(".*");
    else if(m_triggerNames[i] == "HLT_ALL")
      m_trigger[i] = m_trigDec->isPassed("HLT_.*");
    else
      m_trigger[i] = m_trigDec->isPassed(m_triggerNames[i]);
  }
  return;
}

void IDPerfMonEoverP::fillElectronInfo (const xAOD::Electron *p) const
{
  ATH_MSG_VERBOSE(  "In fillElectronInfo()" );
  for (size_t i = 0; i < m_PID_ShowerType_Names.size(); i++) {
    float dummy(-1);
    m_ePID_ShowerType[i][m_electronCounter] = static_cast<float> (p->showerShapeValue( dummy, m_PID_ShowerType_Names[i].first ))? dummy :-1;
  }
  // for (size_t i = 0; i < m_PID_IsolationType_Names.size(); i++) {
  //      float dummy(-1);
  //      m_ePID_IsolationType[i][m_electronCounter] = static_cast<float> (p.isolationValue( dummy, m_PID_IsolationType_Names[i].first ))? dummy :-1;
  //     }
  for (size_t i = 0; i < m_PID_TrackCaloMatchType_Names.size(); i++) {
    float dummy(-1);
    m_ePID_TrackCaloMatchType[i][m_electronCounter] = static_cast<float> (p->trackCaloMatchValue( dummy, m_PID_TrackCaloMatchType_Names[i].first ))? dummy :-1;
  }

for (size_t i = 0; i < m_PID_SummaryType_Names.size(); i++) {
    float dummy(-1);
    m_ePID_SummaryType[i][m_electronCounter] = static_cast<float> (p->trackParticleSummaryValue( dummy, m_PID_SummaryType_Names[i].first ))? dummy :-1;
  }

  return;

}

bool IDPerfMonEoverP::fillLastMeasurement(const Trk::Track* track, const int fitter)const
{
  ATH_MSG_VERBOSE("In fillLastMeasurement()");
  if(!track) return false;
  const Trk::TrackParameters* trkPara =0;

  const DataVector<const Trk::TrackStateOnSurface>* oldTrackStates = track->trackStateOnSurfaces();
  if (oldTrackStates == 0)
  {
    return false;
  }

  for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = oldTrackStates->rbegin(); rItTSoS != oldTrackStates->rend(); ++rItTSoS)
  {
    if (trkPara!=0){
        break;
    }

    if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0)
    {
       trkPara = (*rItTSoS)->trackParameters();
    }
  }

  if (trkPara !=0 ){
    m_electronLMQoverP[fitter][m_electronCounter] =  trkPara->parameters()[Trk::qOverP] ;
    return true;
  }

  return false;

}





bool IDPerfMonEoverP::passZeeSelection(std::vector<int>& electrons)
{
  ATH_MSG_VERBOSE("In passZeeSelection()");
  //ATH_MSG_WARNING("Zee seletion needs to be adjusted for run2");
  // Adjusted according to https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/WZCommonAnalysisTopics2015
  if(m_nbpv<1) return false;
  bool primaryVertexOk(false);
  electrons.clear();

  for (int i=0; i<m_nbpv; ++i){
    if (m_pvnbtk[i] > 2) primaryVertexOk = true;
  }
  if(!primaryVertexOk) return false;

  //Trigger
  //No need to pass the trigger for tracking performance studies

  //Loose electron selection
  std::vector<int> goodLooseElectrons;

  //  std::cout << " N electrons " << m_nelectrons <<std::endl;
  for(int ele=0; ele<m_nelectrons; ele++){
    //Loose electron selection
    if (!m_IsEMLoose[ele]) continue;
    //Pt
    double clusterEt = cosh( m_ClusterEta[ele] ) != 0 ?  m_ClusterEnergy[ele] / cosh( m_ClusterEta[ele] ) : 0.;
    if (clusterEt <= 25000) continue;

    //Range
    double absEta = fabs(m_ClusterEta[ele]);
    if (absEta >= 2.47 || ( absEta >= 1.37 && absEta <= 1.52 )) continue;

    //OTx ...
    if(!m_isGoodOQ[ele]) continue;

    //Author
    if(m_author[ele]!=1 && m_author[ele]!=3) continue;

    goodLooseElectrons.push_back(ele);
  }

  int pairsLooseInMassWindow = 0;

  ATH_MSG_DEBUG("N Loose electrons " << goodLooseElectrons.size());

  //Fill Loose electron ET spectrum;
  for(int gele1 = 0; gele1 < (int)goodLooseElectrons.size()-1; ++gele1){
    for(int gele2 = gele1+1; gele2 < (int)goodLooseElectrons.size(); ++gele2){
      int ele1 = goodLooseElectrons[gele1];
      int ele2 = goodLooseElectrons[gele2];
      double mass = getMassCluster(ele1,ele2);
      if (mass > 60000 && mass < 120000){
        ++pairsLooseInMassWindow;
        if(m_electronQoverP[0][ele1]*m_electronQoverP[0][ele2]<0)
          m_ZeeLooseMassOS_Cluster->Fill(mass);
        else
          m_ZeeLooseMassSS_Cluster->Fill(mass);
      }
    }
  }

  if (pairsLooseInMassWindow < 1) return false;
  std::vector<int> goodMediumElectrons;

  for(int gele = 0; gele < (int)goodLooseElectrons.size(); ++gele){
    int ele  = goodLooseElectrons[gele];
    if(m_IsEMMedium[ele]){
      goodMediumElectrons.push_back(ele);
    }
  }

  ATH_MSG_DEBUG("N Medium electrons " << goodMediumElectrons.size());

  //Reject events with more than two good electrons
  if (goodMediumElectrons.size() != 2) return false;
  
  // Make the mass out of the highest pt electrons ....
  double mass = getMassCluster(goodMediumElectrons[0],goodMediumElectrons[1]);
  if (mass > 66000 && mass < 116000){
    if(m_electronQoverP[0][goodMediumElectrons[0]]*m_electronQoverP[0][goodMediumElectrons[1]]<0){
      m_ZeeMediumMassOS_Cluster->Fill(mass);
      electrons.push_back(goodMediumElectrons[0]);
      electrons.push_back(goodMediumElectrons[1]);

      double trackEt = m_ClusterEnergy[goodMediumElectrons[0]]*sin(m_electronTheta[0][goodMediumElectrons[0]]);
      m_ZeeMediumOS_ClusterPtEta->Fill(trackEt, m_ClusterEta[0] );
      trackEt = m_ClusterEnergy[goodMediumElectrons[1]]*sin(m_electronTheta[0][goodMediumElectrons[1]]);
      m_ZeeMediumOS_ClusterPtEta->Fill(trackEt, m_ClusterEta[1] );

      return true;
    } else{
      m_ZeeMediumMassSS_Cluster->Fill(mass);
      return false;
    }
  }
  return false;
}

bool IDPerfMonEoverP::passWenuSelection(std::vector<int>& electrons)
{
  ATH_MSG_VERBOSE("In passWenuSelection()");
  if(m_nbpv<1) return false;
  bool primaryVertexOk(false);
  electrons.clear();

  for (int i(0); i<m_nbpv; ++i){
    if (m_pvnbtk[i] > 2) primaryVertexOk = true;
  }
  if(!primaryVertexOk) return false;

  //MET Goodness
  if( m_isDATA && !m_METgoodness) return false;

  //Trigger
  //no need to pass trigger for perfromance studies


  //Loose electron selection
  std::vector<int> goodLooseElectrons;
  for(int ele(0); ele<m_nelectrons; ++ele){
    //Loose electron selection
    if (!m_IsEMLoose[ele]) continue;
    //Pt
    double clusterEt = cosh( m_ClusterEta[ele] ) != 0 ?  m_ClusterEnergy[ele] / cosh( m_ClusterEta[ele] ) : 0.;
    if (clusterEt <= 25000) continue;
    //Range
    double absEta = fabs(m_ClusterEta[ele]);
    if (absEta >= 2.47 || ( absEta >= 1.37 && absEta <= 1.52 ) ) continue;
    //OTx ...
    if(!m_isGoodOQ[ele]) continue;
    //Author
    if(m_author[ele]!=1 && m_author[ele]!=3) continue;
    goodLooseElectrons.push_back(ele);
  }

  //Fill Loose electron ET spectrum;
  for(int gele(0); gele < (int) goodLooseElectrons.size(); ++gele){
    int ele  = goodLooseElectrons[gele];
    double trackEt = m_ClusterEnergy[gele]*sin(m_electronTheta[0][ele]);
    m_WenuLooseElectronET->Fill(trackEt);
  }

  int nMediumElectrons(0);
  int nTightElectrons(0);
  int tightElectron(0);

  for(int gele(0); gele < (int) goodLooseElectrons.size(); ++gele){
    //
    int ele  = goodLooseElectrons[gele];
    if(m_IsEMMedium[ele]) ++nMediumElectrons;
    if (!m_IsEMTight[ele]) continue;
    ++nTightElectrons;
    tightElectron = ele;
  }

  //Reject events with more than one good electron
  if (nMediumElectrons > 1)
    return false;
  //Reject events with no tight electrons;
  if (nTightElectrons <1)
    return false;

  double metphi = atan2(m_missingEty,m_missingEtx);
  double trackEt = m_ClusterEnergy[tightElectron]*sin(m_electronTheta[0][tightElectron]);
  double clusterEt = cosh( m_ClusterEta[tightElectron] ) !=0 ?  m_ClusterEnergy[tightElectron] / cosh( m_ClusterEta[tightElectron] ) : 0.;

  double massT = 2*m_missingEt*trackEt*(1-cos(m_electronPhi[0][tightElectron]-metphi));
  if(massT > 0) massT =sqrt(massT);
  else massT = 0;


  m_WenuTight_Met->Fill(m_missingEt);
  m_WenuTight_MT->Fill(massT);

  if (m_missingEt <= 25000) return false;
  m_WenuTightMet_MT->Fill(massT);

  if (massT <= 50000) return false;

  m_WenuTightElectronET->Fill(clusterEt);

  double sumpx = m_ClusterEnergy[tightElectron]*sin( m_electronPhi[0][tightElectron] ) * sin(m_electronTheta[0][tightElectron]) + m_missingEtx;
  double sumpy = m_ClusterEnergy[tightElectron]*cos( m_electronPhi[0][tightElectron] ) * sin(m_electronTheta[0][tightElectron]) + m_missingEty;

  double wpt = sumpx*sumpx +sumpy*sumpy - massT*massT;
  if(wpt > 0) wpt  =sqrt(wpt);
  else wpt = 0;

  m_WenuTightW_PT->Fill(wpt);


  if(m_electronQoverP[0][tightElectron] > 0){
    m_WenuTightElectron_PTEtaPos->Fill(trackEt,m_ClusterEta[tightElectron]);
  } else {
    m_WenuTightElectron_PTEtaNeg->Fill(trackEt,m_ClusterEta[tightElectron]);
  }

  electrons.push_back(tightElectron);

  return true;
}

double IDPerfMonEoverP::getMassCluster(int el1, int el2)
{
  ATH_MSG_VERBOSE("In getMassCluster()");
  double ELECTRON_MASS  = 0.5109989; //MeV


  TLorentzVector v0,v1;

  double pt1 = m_ClusterEnergy[el1]*sin(m_electronTheta[0][el1]);
  double pt2 = m_ClusterEnergy[el2]*sin(m_electronTheta[0][el2]);

  double eta1 = -log( tan(m_electronTheta[0][el1] /2) );
  double eta2 = -log( tan(m_electronTheta[0][el2] /2) );

  v0.SetPtEtaPhiM(pt1,
                  eta1,
                   m_electronPhi[0][el1], ELECTRON_MASS);
  v1.SetPtEtaPhiM(pt2,
                  eta2,
                   m_electronPhi[0][el2], ELECTRON_MASS);

  double mass  =  (v0+v1).M();

  return mass;

}

std::vector<int> IDPerfMonEoverP::FillSimpleTree()
{
  ATH_MSG_VERBOSE("In fillSimpleTree()");

  std::vector<int> electronsZee;
  std::vector<int> electronsWenu;

  passZeeSelection(electronsZee);
  passWenuSelection(electronsWenu);

  std::vector<int> allInterestingElectrons;

  for(int i(0); i < (int)electronsZee.size(); ++i){
    int ele  = electronsZee[i];
    if ( std::find(allInterestingElectrons.begin(), allInterestingElectrons.end(), ele)==allInterestingElectrons.end() ){
      allInterestingElectrons.push_back(ele);
    }
  }

  for(int i(0); i < (int)electronsWenu.size(); ++i){
    int ele  = electronsWenu[i];
    if ( std::find(allInterestingElectrons.begin(), allInterestingElectrons.end(), ele)==allInterestingElectrons.end() ){
      allInterestingElectrons.push_back(ele);
    }
  }

  ATH_MSG_DEBUG(" SimpleTreeElectrons " << allInterestingElectrons.size() );

  for(int i(0); i < (int)allInterestingElectrons.size(); ++i){

    int ele = allInterestingElectrons[i];
    if (m_electronQoverP[0][ele] > 0 )
      m_smallCharge = 1.;
    else
      m_smallCharge =-1.;
    m_smallTrackTheta    = m_electronTheta[0][ele];
    m_small_QoverP       = m_electronQoverP[0][ele];
    m_small1_QoverP      = m_electronQoverP[1][ele];
    m_small2_QoverP      = m_electronQoverP[2][ele];
    m_smallClusterEnergy = m_ClusterEnergy[ele];
    m_smallClusterPhi    = m_ClusterPhi[ele];
    m_smallClusterEta    = m_ClusterEta[ele];
    m_smallValidationTree->Fill();
  }

	return allInterestingElectrons;
}
