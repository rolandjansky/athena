///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetCalibrationTool.cxx 
// Implementation file for class JetCalibrationTool
// Author: Joe Taenzer <joseph.taenzer@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "JetCalibTools/JetCalibrationTool.h"
#include "JetCalibTools/CalibrationMethods/JetPileupCorrection.h"
#include "JetCalibTools/CalibrationMethods/ResidualOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/BcidOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/EtaJESCorrection.h"
#include "JetCalibTools/CalibrationMethods/GlobalSequentialCorrection.h"
#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"
#include "JetCalibTools/CalibrationMethods/JMSCorrection.h"
#include "JetCalibTools/CalibrationMethods/JetSmearingCorrection.h"
#include "PathResolver/PathResolver.h"
#include "AsgDataHandles/ReadDecorHandle.h"

JetCalibrationTool::JetCalibrationTool(const std::string& name)
  : asg::AsgTool( name ),
    m_jetAlgo(""), m_config(""), m_calibSeq(""), m_calibAreaTag(""), m_originScale(""), m_devMode(false),
    m_isData(true), m_timeDependentCalib(false), m_dir(""), m_eInfoName(""), m_globalConfig(nullptr),
    m_doBcid(true), m_doJetArea(true), m_doResidual(true), m_doOrigin(true), m_doGSC(true), m_gscDepth("auto"), m_smearIndex(-1)
{ 

  declareProperty( "JetCollection", m_jetAlgo = "AntiKt4LCTopo" );
  declareProperty( "ConfigFile", m_config = "" );
  declareProperty( "CalibSequence", m_calibSeq = "JetArea_Offset_AbsoluteEtaJES_Insitu" );
  declareProperty( "IsData", m_isData = true );
  declareProperty( "ConfigDir", m_dir = "JetCalibTools/CalibrationConfigs/" );
  declareProperty( "EventInfoName", m_eInfoName = "EventInfo");
  declareProperty( "DEVmode", m_devMode = false);
  declareProperty( "OriginScale", m_originScale = "JetOriginConstitScaleMomentum");
  declareProperty( "CalibArea", m_calibAreaTag = "00-04-82");
  declareProperty( "GSCDepth", m_gscDepth);

}

JetCalibrationTool::~JetCalibrationTool() {
  if(m_globalConfig) delete m_globalConfig;
  for(TEnv* config : m_globalTimeDependentConfigs) delete config;
  for(TEnv* config : m_globalInsituCombMassConfig) delete config;
}


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode JetCalibrationTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << " to calibrate " << m_jetAlgo << "jets");

  TString jetAlgo = m_jetAlgo;
  TString calibSeq = m_calibSeq;
  std::string dir = m_dir;

  //Make sure the necessary properties were set via the constructor or python configuration
  if ( jetAlgo.EqualTo("") || calibSeq.EqualTo("") ) {
    ATH_MSG_FATAL("JetCalibrationTool::initialize : At least one of your constructor arguments is not set. Did you use the copy constructor?");
    return StatusCode::FAILURE;
  }

  if ( m_config.empty() ) { ATH_MSG_FATAL("No configuration file specified."); return StatusCode::FAILURE; } 
  // The calibration area tag is a property of the tool
  const std::string calibPath = "CalibArea-" + m_calibAreaTag + "/";
  if(m_devMode){
    ATH_MSG_WARNING("Dev Mode is ON!!!");
    ATH_MSG_WARNING("Dev Mode is NOT RECOMMENDED!!!");
    dir = "JetCalibTools/";
  }
  else{dir.insert(14,calibPath);} // Obtaining the path of the configuration file
  std::string configPath=dir+m_config; // Full path
  TString fn =  PathResolverFindCalibFile(configPath);

  ATH_MSG_INFO("Reading global JES settings from: " << m_config);
  ATH_MSG_INFO("resolved in: " << fn);
  
  m_globalConfig = new TEnv();
  int status=m_globalConfig->ReadFile(fn ,EEnvLevel(0));
  if (status!=0) { ATH_MSG_FATAL("Cannot read config file " << fn ); return StatusCode::FAILURE; }

  //Make sure that one of the standard jet collections is being used
  if ( calibSeq.Contains("JetArea") ) {
    if ( jetAlgo.Contains("PFlow") ) m_jetScale = PFLOW;
    else if ( jetAlgo.Contains("EM") ) m_jetScale = EM;
    else if ( jetAlgo.Contains("LC") ) m_jetScale = LC;
    else { ATH_MSG_FATAL("jetAlgo " << jetAlgo << " not recognized."); return StatusCode::FAILURE; }
  }

  // Settings for R21/2.5.X
  m_originCorrectedClusters = m_globalConfig->GetValue("OriginCorrectedClusters",false);
  m_doSetDetectorEta = m_globalConfig->GetValue("SetDetectorEta",true);

  // Rho key specified in the config file?
  std::string rhoKey_config = m_globalConfig->GetValue("RhoKey", "None");

  // Get name of vertex container
  m_vertexContainerName = m_globalConfig->GetValue("VertexContainerName","PrimaryVertices");
  
  bool requireRhoInput = false;

  //Make sure the residual correction is turned on if requested
  if ( !calibSeq.Contains("JetArea") && !calibSeq.Contains("Residual") ) {
    m_doJetArea = false;
    m_doResidual = false;
  } else if ( calibSeq.Contains("JetArea") ) {
    if ( m_rhoKey.key().compare("auto") == 0 && rhoKey_config.compare("None") == 0) {
      if(!m_originCorrectedClusters){
        if ( m_jetScale == EM ) m_rhoKey = "Kt4EMTopoEventShape";
        else if ( m_jetScale == LC ) m_rhoKey = "Kt4LCTopoEventShape";
        else if ( m_jetScale == PFLOW ) m_rhoKey = "Kt4EMPFlowEventShape";
      } else{
        if ( m_jetScale == EM ) m_rhoKey = "Kt4EMTopoOriginEventShape";
        else if ( m_jetScale == LC ) m_rhoKey = "Kt4LCTopoOriginEventShape";
        else if ( m_jetScale == PFLOW ) m_rhoKey = "Kt4EMPFlowEventShape";
      }
    }
    else if(rhoKey_config.compare("None") != 0 && m_rhoKey.key().compare("auto") == 0){
      m_rhoKey = rhoKey_config;
    }
    requireRhoInput = true;
    if ( !calibSeq.Contains("Residual") ) m_doResidual = false;
  } else if ( !calibSeq.Contains("JetArea") && calibSeq.Contains("Residual") ) {
    m_doJetArea = false;
    ATH_MSG_INFO("ApplyOnlyResidual should be true if only Residual pile up correction wants to be applied. Need to specify pile up starting scale in the configuration file.");
  }
  // get nJet threshold and name
  m_useNjetInResidual = m_globalConfig->GetValue("OffsetCorrection.UseNjet", false);
  m_nJetThreshold = m_globalConfig->GetValue("OffsetCorrection.nJetThreshold", 20);
  m_nJetContainerName = m_globalConfig->GetValue("OffsetCorrection.nJetContainerName",
                                                 "HLT_xAOD__JetContainer_a4tcemsubjesISFS");

  if ( !calibSeq.Contains("Origin") ) m_doOrigin = false;
  if ( !calibSeq.Contains("GSC") ) m_doGSC = false;
  if ( !calibSeq.Contains("Bcid") ) m_doBcid = false;

  //Protect against the in-situ calibration being requested when isData is false
  if ( calibSeq.Contains("Insitu") && !m_isData ) {
    ATH_MSG_FATAL("JetCalibrationTool::initialize : calibSeq string contains Insitu with isData set to false. Can't apply in-situ correction to MC!!");
    return StatusCode::FAILURE;
  }

  // Time-Dependent Insitu Calibration
  m_timeDependentCalib = m_globalConfig->GetValue("TimeDependentInsituCalibration",false);
  if(m_timeDependentCalib && calibSeq.Contains("Insitu")){ // Read Insitu Configs
    m_timeDependentInsituConfigs = JetCalibUtils::Vectorize( m_globalConfig->GetValue("InsituTimeDependentConfigs","") );
    if(m_timeDependentInsituConfigs.size()==0) ATH_MSG_ERROR("Please check there are at least two insitu configs");
    m_runBins = JetCalibUtils::VectorizeD( m_globalConfig->GetValue("InsituRunBins","") );
    if(m_runBins.size()!=m_timeDependentInsituConfigs.size()+1) ATH_MSG_ERROR("Please check the insitu run bins");
    for(unsigned int i=0;i<m_timeDependentInsituConfigs.size();++i){

      std::string configPath_insitu = dir+m_timeDependentInsituConfigs.at(i).Data(); // Full path
      TString fn_insitu =  PathResolverFindCalibFile(configPath_insitu);

      ATH_MSG_INFO("Reading time-dependent insitu settings from: " << m_timeDependentInsituConfigs.at(i));
      ATH_MSG_INFO("resolved in: " << fn_insitu);
  
      TEnv *globalConfig_insitu = new TEnv();
      int status = globalConfig_insitu->ReadFile(fn_insitu ,EEnvLevel(0));
      if (status!=0) { ATH_MSG_FATAL("Cannot read config file " << fn_insitu ); return StatusCode::FAILURE; }
      m_globalTimeDependentConfigs.push_back(globalConfig_insitu);
    }
  }

  //Combined Mass Calibration:
  m_insituCombMassCalib = m_globalConfig->GetValue("InsituCombinedMassCorrection",false);
  if(m_insituCombMassCalib && calibSeq.Contains("InsituCombinedMass")){ // Read Combination Config
    m_insituCombMassConfig = JetCalibUtils::Vectorize( m_globalConfig->GetValue("InsituCombinedMassCorrectionFile","") );
    if(m_insituCombMassConfig.size()==0) ATH_MSG_ERROR("Please check there is a combination config");
    for(unsigned int i=0;i<m_insituCombMassConfig.size();++i){

      std::string configPath_comb = dir+m_insituCombMassConfig.at(i).Data(); // Full path
      TString fn_comb =  PathResolverFindCalibFile(configPath_comb);

      ATH_MSG_INFO("Reading combination settings from: " << m_insituCombMassConfig.at(i));
      ATH_MSG_INFO("resolved in: " << fn_comb);

      TEnv *globalInsituCombMass = new TEnv();
      int status = globalInsituCombMass->ReadFile(fn_comb ,EEnvLevel(0));
      if (status!=0) { ATH_MSG_FATAL("Cannot read config file " << fn_comb ); return StatusCode::FAILURE; }
      m_globalInsituCombMassConfig.push_back(globalInsituCombMass);
    }
  }

  //Loop over the request calib sequence
  //Initialize derived classes for applying the requested calibrations and add them to a vector
  std::vector<TString> vecCalibSeq = JetCalibUtils::Vectorize(calibSeq,"_");
  for ( unsigned int i=0; i<vecCalibSeq.size(); ++i) {
    if ( vecCalibSeq[i].EqualTo("Origin") || vecCalibSeq[i].EqualTo("DEV") ) continue;
    if ( vecCalibSeq[i].EqualTo("Residual") && m_doJetArea ) continue;
    ATH_CHECK( getCalibClass(vecCalibSeq[i] ));
  }

  // Initialise ReadHandle(s)
  ATH_CHECK( m_evtInfoKey.initialize() );
  ATH_CHECK( m_muKey.initialize() );
  ATH_CHECK( m_rhoKey.initialize(requireRhoInput) );
  if(m_pvKey.empty()) {
    // No PV key: -- check if it is required
    if(m_doResidual) {
      // May require modification in case of residual that does not require NPV
      ATH_MSG_ERROR("Residual calibration requested but no primary vertex container specified!");
      return StatusCode::FAILURE;
    }
    else if(m_doGSC) {
      if(m_jetAlgo.find("PFlow")!=std::string::npos) {
        ATH_MSG_ERROR("GSC calibration for PFlow requested but no primary vertex container specified!");
        return StatusCode::FAILURE;
      }
      else if((m_gscDepth!="Tile0" && m_gscDepth!="EM3")) {
        ATH_MSG_ERROR("GSC calibration with tracks requested but no primary vertex container specified!");
        return StatusCode::FAILURE;
      }
    }
  } else {
    // Received a PV key, declare the data dependency
    ATH_CHECK( m_pvKey.initialize() );
  }

  return StatusCode::SUCCESS;
}

//Method for initializing the requested calibration derived classes
StatusCode JetCalibrationTool::getCalibClass(TString calibration) {
  TString jetAlgo = m_jetAlgo;
  const TString calibPath = "CalibArea-" + m_calibAreaTag + "/";
  if ( calibration.EqualTo("Bcid") ){
    m_globalConfig->SetValue("PileupStartingScale","JetBcidScaleMomentum");
    std::unique_ptr<JetCalibrationStep> bcidCorr = std::make_unique<BcidOffsetCorrection>(this->name()+"_Bcid", m_globalConfig, jetAlgo, calibPath, m_isData);
    ATH_CHECK(bcidCorr->initialize());
    m_calibSteps.push_back(std::move(bcidCorr));
    return StatusCode::SUCCESS;
  }
  else if ( calibration.EqualTo("JetArea") || calibration.EqualTo("Residual") ) {
    std::unique_ptr<JetCalibrationStep> puCorr = std::make_unique<JetPileupCorrection>(this->name()+"_Pileup", m_globalConfig, jetAlgo, calibPath,
                                                   m_doResidual, m_doJetArea, m_doOrigin, m_originScale.c_str(), m_isData, m_devMode);
    puCorr->msg().setLevel( this->msg().level() );
    ATH_CHECK(puCorr->initialize());
    m_calibSteps.push_back(std::move(puCorr));
    return StatusCode::SUCCESS;
  }
  else if ( calibration.EqualTo("EtaJES") || calibration.EqualTo("AbsoluteEtaJES") ) {
    std::unique_ptr<JetCalibrationStep> etaJESCorr = std::make_unique<EtaJESCorrection>(this->name()+"_EtaJES", m_globalConfig, jetAlgo, calibPath, false, m_devMode);
    etaJESCorr->msg().setLevel( this->msg().level() );
    ATH_CHECK(etaJESCorr->initialize());
    m_calibSteps.push_back(std::move(etaJESCorr)); 
    return StatusCode::SUCCESS; 
  }
  else if ( calibration.EqualTo("EtaMassJES") ) {
    std::unique_ptr<JetCalibrationStep> etaJESCorr = std::make_unique<EtaJESCorrection>(this->name()+"_EtaMassJES", m_globalConfig, jetAlgo, calibPath, true, m_devMode);
    etaJESCorr->msg().setLevel( this->msg().level() );
    ATH_CHECK(etaJESCorr->initialize());
    m_calibSteps.push_back(std::move(etaJESCorr)); 
    return StatusCode::SUCCESS; 
  }
  else if ( calibration.EqualTo("GSC") ) {
    std::unique_ptr<JetCalibrationStep> gsc = std::make_unique<GlobalSequentialCorrection>(this->name()+"_GSC", m_globalConfig, jetAlgo, m_gscDepth, calibPath, m_devMode);
    gsc->msg().setLevel( this->msg().level() );
    ATH_CHECK(gsc->initialize());
    m_calibSteps.push_back(std::move(gsc)); 
    return StatusCode::SUCCESS; 
  }
  else if ( calibration.EqualTo("JMS") ) {
    std::unique_ptr<JetCalibrationStep> jetMassCorr = std::make_unique<JMSCorrection>(this->name()+"_JMS", m_globalConfig, jetAlgo, calibPath, m_devMode);
    jetMassCorr->msg().setLevel( this->msg().level() );
    ATH_CHECK(jetMassCorr->initialize());
    m_calibSteps.push_back(std::move(jetMassCorr));
    return StatusCode::SUCCESS;
  }
  else if ( calibration.EqualTo("InsituCombinedMass") ){
    for(unsigned int i=0;i<m_insituCombMassConfig.size();++i){
      std::unique_ptr<JetCalibrationStep> jetMassCorr = std::make_unique<JMSCorrection>(this->name()+"_InsituCombinedMass", m_globalInsituCombMassConfig.at(i), jetAlgo, calibPath, m_devMode);
      jetMassCorr->msg().setLevel( this->msg().level() );
      ATH_CHECK(jetMassCorr->initialize());
      m_calibSteps.push_back(std::move(jetMassCorr));
    }
    return StatusCode::SUCCESS;
  }
  else if ( calibration.EqualTo("Insitu") ) {
    if(!m_timeDependentCalib){
      std::unique_ptr<JetCalibrationStep> insituDataCorr = std::make_unique<InsituDataCorrection>(this->name()+"_Insitu", m_globalConfig, jetAlgo, calibPath, m_devMode);
      insituDataCorr->msg().setLevel( this->msg().level() );
      ATH_CHECK(insituDataCorr->initialize());
      m_calibSteps.push_back(std::move(insituDataCorr));
      return StatusCode::SUCCESS; 
    }
    else{
      ATH_MSG_INFO("Initializing Time-Dependent Insitu Corrections");
      for(unsigned int i=0;i<m_timeDependentInsituConfigs.size();++i){
        // Add 0.5 before casting to avoid floating-point precision issues
        unsigned int firstRun = static_cast<unsigned int>(m_runBins.at(i)+1.5);
        unsigned int lastRun = static_cast<unsigned int>(m_runBins.at(i+1)+0.5);
        std::unique_ptr<JetCalibrationStep> insituDataCorr = std::make_unique<InsituDataCorrection>(this->name()+"_Insitu_"+std::to_string(i), m_globalTimeDependentConfigs.at(i), jetAlgo,
                                                                                                    calibPath, m_devMode, firstRun, lastRun);
        insituDataCorr->msg().setLevel( this->msg().level() );
        ATH_CHECK(insituDataCorr->initialize());
        m_calibSteps.push_back(std::move(insituDataCorr));
      }
      return StatusCode::SUCCESS; 
    }
  }
  else if ( calibration.EqualTo("Smear") ) {
    if(m_isData){
      ATH_MSG_FATAL("Asked for smearing of data, which is not supported.  Aborting.");
      return StatusCode::FAILURE;
    }
    std::unique_ptr<JetCalibrationStep> jetSmearCorr = std::make_unique<JetSmearingCorrection>(this->name()+"_Smear", m_globalConfig,jetAlgo,calibPath,m_devMode);
    jetSmearCorr->msg().setLevel(this->msg().level());
    ATH_CHECK(jetSmearCorr->initialize());
    m_calibSteps.push_back(std::move(jetSmearCorr));
    m_smearIndex = m_calibSteps.size() - 1;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_FATAL("Calibration string not recognized: " << calibration << ", aborting.");
  return StatusCode::FAILURE;
}

StatusCode JetCalibrationTool::applyCalibration(xAOD::JetContainer& jets) const {
  //Grab necessary event info for pile up correction and store it in a JetEventInfo class object
  ATH_MSG_VERBOSE("Modifying jet collection.");
  JetEventInfo jetEventInfo;
  ATH_CHECK( initializeEvent(jetEventInfo) );
  for (xAOD::Jet* jet : jets) ATH_CHECK( calibrate(*jet, jetEventInfo) );
  return StatusCode::SUCCESS;
}

// Private/Protected Methods
///////////////

StatusCode JetCalibrationTool::initializeEvent(JetEventInfo& jetEventInfo) const {

  // Check if the tool was initialized
  if( m_calibSteps.size() == 0 ){
    ATH_MSG_FATAL("   JetCalibrationTool::initializeEvent : The tool was not initialized.");
    return StatusCode::FAILURE;
  }

  // static accessor for PV index access
  static SG::AuxElement::ConstAccessor<int> PVIndexAccessor("PVIndex");
  
  ATH_MSG_VERBOSE("Initializing event.");

  if( m_doJetArea ) {
    //Determine the rho value to use for the jet area subtraction
    //Should be determined using EventShape object, use hard coded values if EventShape doesn't exist
    double rho=0;
    const xAOD::EventShape * eventShape = 0;

    SG::ReadHandle<xAOD::EventShape> rhRhoKey(m_rhoKey);

    if ( rhRhoKey.isValid() ) {
      ATH_MSG_VERBOSE("  Found event density container " << m_rhoKey.key());
      eventShape = rhRhoKey.cptr();
      if ( !rhRhoKey.isValid() ) {
        ATH_MSG_VERBOSE("  Event shape container not found.");
        ATH_MSG_FATAL("Could not retrieve xAOD::EventShape DataHandle.");
        return StatusCode::FAILURE;
      } else if ( !eventShape->getDensity( xAOD::EventShape::Density, rho ) ) {
        ATH_MSG_VERBOSE("  Event density not found in container.");
        ATH_MSG_FATAL("Could not retrieve xAOD::EventShape::Density from xAOD::EventShape.");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_VERBOSE("  Event density retrieved.");
      }
    } else if ( m_doJetArea && !rhRhoKey.isValid() ) {
      ATH_MSG_VERBOSE("  Rho container not found: " << m_rhoKey.key());
      ATH_MSG_FATAL("Could not retrieve xAOD::EventShape DataHandle.");
      return StatusCode::FAILURE;
    }
    jetEventInfo.setRho(rho);
    ATH_MSG_VERBOSE("  Rho = " << 0.001*rho << " GeV");

    // Necessary retrieval and calculation for use of nJetX instead of NPV
    if(m_useNjetInResidual) {
      // retrieve the container
      const xAOD::JetContainer * jets = 0;
      if (evtStore()->contains<xAOD::JetContainer>(m_nJetContainerName) ) {
        ATH_MSG_VERBOSE("  Found jet container " << m_nJetContainerName);
        if ( evtStore()->retrieve(jets, m_nJetContainerName).isFailure() || !jets ) {
          ATH_MSG_FATAL("Could not retrieve xAOD::JetContainer from evtStore.");
          return StatusCode::FAILURE;
        }
      } else {
        ATH_MSG_FATAL("Could not find jet container " << m_nJetContainerName);
        return StatusCode::FAILURE;
      }

      // count jets above threshold
      int nJets = 0;
      for (auto jet : *jets) {
        if(jet->pt()/1000. > m_nJetThreshold)
          nJets += 1;
      }
      jetEventInfo.setNjet(nJets);
    }
  }

  // Retrieve EventInfo object, which now has multiple uses
  if ( m_doResidual || m_doGSC || m_doBcid || m_timeDependentCalib) {
    const xAOD::EventInfo * eventObj = 0;
    static unsigned int eventInfoWarnings = 0;
    SG::ReadHandle<xAOD::EventInfo> rhEvtInfo(m_evtInfoKey);
    if ( rhEvtInfo.isValid() ) {
      eventObj = rhEvtInfo.cptr();
    } else {
      ++eventInfoWarnings;
      if ( eventInfoWarnings < 20 )
        ATH_MSG_ERROR("   JetCalibrationTool::initializeEvent : Failed to retrieve event information.");
      jetEventInfo.setMu(0); //Hard coded value mu = 0 in case of failure (to prevent seg faults later).
      jetEventInfo.setPVIndex(0);
      return StatusCode::SUCCESS; //error is recoverable, so return SUCCESS
    }
    jetEventInfo.setRunNumber( eventObj->runNumber() );

    // If we are applying the reisdual, then store mu
    if (m_doResidual || m_doBcid) {
      SG::ReadDecorHandle<xAOD::EventInfo,float> eventInfoDecor(m_muKey);
      if(!eventInfoDecor.isPresent()) {
        ATH_MSG_ERROR("EventInfo decoration not available!");
        return StatusCode::FAILURE;
      }
      jetEventInfo.setMu( eventInfoDecor(0) );
    }

    // If this is GSC, we need EventInfo to determine the PV to use
    // This is support for groups where PV0 is not the vertex of interest (H->gamgam, etc)
    if (m_doGSC)
    {
      // First retrieve the PVIndex if specified
      // Default is to not specify this, so no warning if it doesn't exist
      // However, if specified, it should be a sane value - fail if not
      if ( m_doGSC && PVIndexAccessor.isAvailable(*eventObj) )
        jetEventInfo.setPVIndex( PVIndexAccessor(*eventObj) );
      else
        jetEventInfo.setPVIndex(0);
      
    }

    // Extract the BCID information for the BCID correction
    if (m_doBcid)
    {
      jetEventInfo.setBcidDistanceFromFront( eventObj->auxdata<int>("DFCommonJets_BCIDDistanceFromFront") );
      jetEventInfo.setBcidGapBeforeTrain( eventObj->auxdata<int>("DFCommonJets_BCIDGapBeforeTrain") );
      jetEventInfo.setBcidGapBeforeTrainMinus12( eventObj->auxdata<int>("DFCommonJets_BCIDGapBeforeTrainMinus12") );
    }
    
    // If PV index is not zero, we need to confirm it's a reasonable value
    // To do this, we need the primary vertices
    // However, other users of the GSC may not have the PV collection (in particular: trigger GSC in 2016)
    // So only retrieve vertices if needed for NPV (residual) or a non-zero PV index was specified (GSC)
    if ((m_doResidual && !m_useNjetInResidual) || (m_doGSC && jetEventInfo.PVIndex()))
    {
      //Retrieve VertexContainer object, use it to obtain NPV for the residual correction or check validity of GSC non-PV0 usage
      const xAOD::VertexContainer * vertices = 0;

      SG::ReadHandle<xAOD::VertexContainer> rhPV(m_pvKey);
      if (rhPV.isValid()) {
        vertices = rhPV.cptr(); 
      } else {
        ATH_MSG_WARNING("   JetCalibrationTool::initializeEvent : Failed to retrieve primary vertices.");
        jetEventInfo.setNPV(0); //Hard coded value NPV = 0 in case of failure (to prevent seg faults later).
        return StatusCode::SUCCESS; //error is recoverable, so return SUCCESS
      }

      // Calculate and set NPV if this is residual
      if (m_doResidual)
      {
        int eventNPV = 0;
        xAOD::VertexContainer::const_iterator vtx_itr = vertices->begin();
        xAOD::VertexContainer::const_iterator vtx_end = vertices->end(); 
        for ( ; vtx_itr != vtx_end; ++vtx_itr ) 
          if ( (*vtx_itr)->nTrackParticles() >= 2 ) ++eventNPV;
  
        jetEventInfo.setNPV(eventNPV);
      }
      
      // Validate value of non-standard PV index usage
      if (m_doGSC && jetEventInfo.PVIndex())
      {
        static unsigned int vertexIndexWarnings = 0;
        if (jetEventInfo.PVIndex() < 0 || static_cast<size_t>(jetEventInfo.PVIndex()) >= vertices->size())
        {
          ++vertexIndexWarnings;
          if (vertexIndexWarnings < 20)
            ATH_MSG_WARNING("   JetCalibrationTool::initializeEvent : PV index is out of bounds.");
          jetEventInfo.setPVIndex(0); // Hard coded value PVIndex = 0 in case of failure (to prevent seg faults later).
          return StatusCode::SUCCESS; // error is recoverable, so return SUCCESS
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode JetCalibrationTool::calibrate(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const {

  //Check for OriginCorrected and PileupCorrected attributes, assume they are false if not found
  int tmp = 0; //temporary int for checking getAttribute
  if ( !jet.getAttribute<int>("OriginCorrected",tmp) )
    jet.setAttribute<int>("OriginCorrected",false);
  if ( !jet.getAttribute<int>("PileupCorrected",tmp) )
    jet.setAttribute<int>("PileupCorrected",false);

  ATH_MSG_VERBOSE("Calibrating jet " << jet.index());
  if(m_doSetDetectorEta) {
    xAOD::JetFourMom_t jetconstitP4 = jet.getAttribute<xAOD::JetFourMom_t>("JetConstitScaleMomentum");
    jet.setAttribute<float>("DetectorEta",jetconstitP4.eta()); //saving constituent scale eta for later use
  }

  for (unsigned int i=0; i<m_calibSteps.size(); ++i) ATH_CHECK(m_calibSteps[i]->calibrate(jet, jetEventInfo));

  return StatusCode::SUCCESS; 
}


StatusCode JetCalibrationTool::getNominalResolutionData(const xAOD::Jet& jet, double& resolution) const{

  if(m_smearIndex < 0){
    ATH_MSG_ERROR("Cannot retrieve the nominal data resolution - smearing was not configured during initialization");
    return StatusCode::FAILURE;
  }
  return m_calibSteps.at(m_smearIndex)->getNominalResolutionData(jet, resolution);
}

StatusCode JetCalibrationTool::getNominalResolutionMC(const xAOD::Jet& jet, double& resolution) const{
  
  if(m_smearIndex < 0){
    ATH_MSG_ERROR("Cannot retrieve the nominal MC resolution - smearing was not configured during initialization");
    return StatusCode::FAILURE;
  }
  return m_calibSteps.at(m_smearIndex)->getNominalResolutionMC(jet, resolution);
}

