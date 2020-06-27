/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CPAnalysisExamples/MyToolProvider.h"

MyToolProvider::MyToolProvider(const std::string& name):
  AsgMessaging(name),
  jetCalibTool(0),
  jetUncertaintiesTool(0),
  jetCleaningTool(0),
  muonCalibrationAndSmearingTool(0),
  muonEfficiencySFTool(0),
  muonSelectionTool(0),
  elecEfficiencySFTool(0),
  egammaCalibTool(0),
  tauSelTool(0),
  tauSmearingTool(0),
  tauEffTool(0),
  btagTool(0),
  metRebuilder(0)
{}

MyToolProvider::~MyToolProvider()
{
  if (jetCalibTool) delete jetCalibTool;
  if (jetUncertaintiesTool) delete jetUncertaintiesTool;
  if (jetCleaningTool) delete jetCleaningTool;
  if (muonCalibrationAndSmearingTool) delete muonCalibrationAndSmearingTool;
  if (muonEfficiencySFTool) delete muonEfficiencySFTool;
  if (muonSelectionTool) delete muonSelectionTool;
  if (elecEfficiencySFTool) delete elecEfficiencySFTool;
  if (egammaCalibTool) delete egammaCalibTool;
  if (tauSelTool) delete tauSelTool;
  if (tauSmearingTool) delete tauSmearingTool;
  if (tauEffTool) delete tauEffTool;
  if (btagTool) delete btagTool;
  if (metRebuilder) delete metRebuilder;
}

StatusCode MyToolProvider::initialize(bool isData, bool isAtlfast)
{

  std::string maindir = "";
  char *tmparea=getenv("ROOTCOREBIN");
  
  if (tmparea != NULL) {
    maindir = tmparea;
    maindir = maindir + "/data/";
  }
  else {
    std::cout << "ROOTCOREBIN is not set." << std::endl;
    return StatusCode::FAILURE;
  }

  
  // Jet calibration tool
  std::string JES_config_file = "JES_Prerecommendation2015_Feb2015.config";
  if(isData) {
    jetCalibTool = new JetCalibrationTool("JetCalibToolMine");
    jetCalibTool->setProperty("JetCollection","AntiKt4LCTopo");
    jetCalibTool->setProperty("ConfigFile",JES_config_file);
    jetCalibTool->setProperty("CalibSequence","EtaJES_Insitu");
    jetCalibTool->setProperty("IsData",isData);
  } else {
    jetCalibTool = new JetCalibrationTool("JetCalibToolMine");
    jetCalibTool->setProperty("JetCollection","AntiKt4LCTopo");
    jetCalibTool->setProperty("ConfigFile",JES_config_file);
    jetCalibTool->setProperty("CalibSequence","JetArea_Residual_EtaJES");
    jetCalibTool->setProperty("IsData",isData);
  }
  ATH_CHECK( jetCalibTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet uncertainty tool

  jetUncertaintiesTool = new JetUncertaintiesTool("JetUncertaintiesTool");
  ATH_CHECK( jetUncertaintiesTool->setProperty("JetDefinition","AntiKt4LCTopo") );
  ATH_CHECK( jetUncertaintiesTool->setProperty("MCType","MC12a") );
  ATH_CHECK( jetUncertaintiesTool->setProperty("ConfigFile","JES_2012/Moriond2013/InsituJES2012_14NP.config") );
  ATH_CHECK( jetUncertaintiesTool->initialize() );       
  CP::SystematicSet defaultSet;
  CP::SystematicCode ret = jetUncertaintiesTool->applySystematicVariation(defaultSet);
  if (ret != CP::SystematicCode::Ok){
    ATH_MSG_ERROR("Cannot configure JetUncertaintiesTool for default systematic setting.");
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet cleaning tools 

  jetCleaningTool = new JetCleaningTool("JetCleaningTool");
  ATH_CHECK( jetCleaningTool->setProperty("CutLevel", "LooseBad") );
  ATH_CHECK( jetCleaningTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon calibration tool
  
  muonCalibrationAndSmearingTool = new CP::MuonCalibrationAndSmearingTool("MuonCalibrationAndSmearingTool");
  //   MuonCalibrationAndSmearingTool->msg().setLevel( MSG::DEBUG);
  ATH_CHECK( muonCalibrationAndSmearingTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon efficiency tool
  
  muonEfficiencySFTool = new CP::MuonEfficiencyScaleFactors("MuonEfficiencyScaleFactors");
  ATH_CHECK( muonEfficiencySFTool->setProperty("WorkingPoint","Medium") );
  //  ATH_CHECK( muonEfficiencySFTool->setProperty("DataPeriod","2012") );
  ATH_CHECK( muonEfficiencySFTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon selection tool

  muonSelectionTool = new CP::MuonSelectionTool("MuonSelectionTool");
  //  muonSelectionTool->msg().setLevel( MSG::VERBOSE );
  ATH_CHECK( muonSelectionTool->setProperty( "MaxEta", 2.4 ) );
  ATH_CHECK( muonSelectionTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise electron efficiency tool
      
  elecEfficiencySFTool = new AsgElectronEfficiencyCorrectionTool("AsgElectronEfficiencyCorrectionTool");
  std::vector< std::string > corrFileNameList;
  corrFileNameList.push_back(maindir+"ElectronEfficiencyCorrection/efficiencySF.offline.RecoTrk.2012.8TeV.rel17p2.GEO20.v08.root");
  corrFileNameList.push_back(maindir+"ElectronEfficiencyCorrection/efficiencySF.offline.Tight.2012.8TeV.rel17p2.v07.root");
  corrFileNameList.push_back(maindir+"ElectronEfficiencyCorrection/efficiencySF.e24vhi_medium1_e60_medium1.Tight.2012.8TeV.rel17p2.v07.root");
  ATH_CHECK( elecEfficiencySFTool->setProperty("CorrectionFileNameList",corrFileNameList) );
  if (isData==0) {
    PATCore::ParticleDataType::DataType data_type;
    if(isAtlfast) data_type = PATCore::ParticleDataType::Fast;
    else data_type = PATCore::ParticleDataType::Full;
    ATH_MSG_DEBUG( "Setting data type to " << data_type);
    ATH_CHECK (elecEfficiencySFTool->setProperty("ForceDataType",(int) data_type) );
  }
  ATH_CHECK( elecEfficiencySFTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise egamma calibration tool      

  egammaCalibTool = new CP::EgammaCalibrationAndSmearingTool("EgammaCalibrationAndSmearingTool");
  egammaCalibTool->setProperty("ESModel", "es2012c");
  ATH_MSG_DEBUG( "Initialising EgcalibTool " );
  ATH_CHECK( egammaCalibTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise tau selection tool      

  tauSelTool = new TauAnalysisTools::TauSelectionTool("TauSelectionTool");
  tauSelTool->msg().setLevel( MSG::INFO );
  // deprecated
  //tauSelTool->setRecommendedProperties();
  ATH_CHECK( tauSelTool->setProperty("SelectionCuts", (int) TauAnalysisTools::SelectionCuts(TauAnalysisTools::CutPt | TauAnalysisTools::CutAbsEta | TauAnalysisTools::CutAbsCharge | TauAnalysisTools::CutNTrack)) );
  ATH_CHECK( tauSelTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise tau smearing tool      
      
  tauSmearingTool = new TauAnalysisTools::TauSmearingTool("TauSmearingTool");
  tauSmearingTool->msg().setLevel( MSG::INFO );
  ATH_CHECK( tauSmearingTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise tau efficiency tool

  tauEffTool = new TauAnalysisTools::TauEfficiencyCorrectionsTool("TauEffTool");
  tauEffTool->msg().setLevel( MSG::INFO );
  ATH_CHECK( tauEffTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise B-tag scale factor tool

  btagTool = new BTaggingEfficiencyTool("BTagTest");   
  ATH_CHECK( btagTool->setProperty("TaggerName",          "MV2c20") );
  ATH_CHECK( btagTool->setProperty("OperatingPoint",      "-0_0436") );
  ATH_CHECK( btagTool->setProperty("JetAuthor",           "AntiKt4EMTopoJets") ); 
  ATH_CHECK( btagTool->setProperty("ScaleFactorFileName","13TeV/2015-PreRecomm-13TeV-MC12-CDI_August3-v1.root") );
  // btagTool->setProperty("JetAuthor",           "AntiKt4TopoEMJVF0_5");
  // btagTool->setProperty("ScaleFactorFileName", "../../../MCMC/2014-Winter-8TeV-MC12-CDI-build880.root");
  // btagTool->setProperty("ScaleFactorFileName", "/mnt/lustre/epp_scratch/atlas/ks429/xAODDev/2.0.6/testCDI.root");
  ATH_CHECK( btagTool->initialize() );

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise MET tool

  metRebuilder = new met::METRebuilder("METRebuilderTool");
  // the below is used for a closure test
  // MET in xAODs has uncalibrated jets with a cut at 0 GeV
  // metRebuilder->setProperty("CalibJetPtCut",0.);
  // metRebuilder->msg().setLevel( MSG::VERBOSE);
  if( metRebuilder->initialize().isFailure()){
    ATH_MSG_ERROR( "Cannot intialize METRebuilder" );
    ATH_MSG_ERROR( "Exiting... " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "METRebuilder initialized... " );
  }

  return StatusCode::SUCCESS;
}
