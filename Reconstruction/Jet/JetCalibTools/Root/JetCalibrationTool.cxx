///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCalibrationTool.cxx 
// Implementation file for class JetCalibrationTool
// Author: Joe Taenzer <joseph.taenzer@cern.ch>
/////////////////////////////////////////////////////////////////// 


// JetCalibTools includes
#include "JetCalibTools/JetCalibrationTool.h"
#include "PathResolver/PathResolver.h"

// Constructors
////////////////

JetCalibrationTool::JetCalibrationTool(const std::string& name)
  : asg::AsgTool( name ),  JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_rhkEvtInfo("EventInfo"),
    m_rhkRhoKey(""),
    m_rhkPV("PrimaryVertices"),
    m_jetAlgo(""), m_config(""), m_calibSeq(""), m_calibAreaTag(""), m_devMode(false), m_isData(true), m_timeDependentCalib(false), m_rhoKey("auto"), m_dir(""), m_eInfoName(""), m_globalConfig(NULL),
    m_doJetArea(true), m_doResidual(true), m_doOrigin(true), m_doGSC(true), 
    m_jetPileupCorr(NULL), m_etaJESCorr(NULL), m_globalSequentialCorr(NULL), m_insituDataCorr(NULL), m_jetMassCorr(NULL)
{ 

  declareProperty( "JetCollection", m_jetAlgo = "AntiKt4LCTopo" );
  declareProperty( "RhoKey", m_rhoKey = "auto" );
  declareProperty( "ConfigFile", m_config = "" );
  declareProperty( "CalibSequence", m_calibSeq = "JetArea_Offset_AbsoluteEtaJES_Insitu" );
  declareProperty( "IsData", m_isData = true );
  declareProperty( "ConfigDir", m_dir = "JetCalibTools/CalibrationConfigs/" );
  declareProperty( "EventInfoName", m_eInfoName = "EventInfo");

}


// Destructor
///////////////
JetCalibrationTool::~JetCalibrationTool() {

  if (m_globalConfig) delete m_globalConfig;
  if (m_jetPileupCorr) delete m_jetPileupCorr;
  if (m_etaJESCorr) delete m_etaJESCorr;
  if (m_globalSequentialCorr) delete m_globalSequentialCorr;
  if (m_insituDataCorr) delete m_insituDataCorr;
  if (m_jetMassCorr) delete m_jetMassCorr;

}


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode JetCalibrationTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK( this->initializeTool( name() ) );

  // Initialise ReadHandle(s)
  ATH_CHECK( m_rhkEvtInfo.initialize() );
  ATH_CHECK( m_rhkRhoKey.assign(m_rhoKey)) ;  // set in `initializeTool'
  ATH_CHECK( m_rhkRhoKey.initialize() );
  ATH_CHECK( m_rhkPV.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode JetCalibrationTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


StatusCode JetCalibrationTool::initializeTool(const std::string& name) {

  TString jetAlgo = m_jetAlgo;
  TString config = m_config;
  TString calibSeq = m_calibSeq;
  std::string dir = m_dir;

  ATH_MSG_INFO("===================================");
  ATH_MSG_INFO("Initializing the xAOD Jet Calibration Tool for " << jetAlgo << "jets");

  //Make sure the necessary properties were set via the constructor or python configuration
  if ( jetAlgo.EqualTo("") || calibSeq.EqualTo("") ) {
    ATH_MSG_FATAL("JetCalibrationTool::initializeTool : At least one of your constructor arguments is not set. Did you use the copy constructor?");
    return StatusCode::FAILURE;
  }

  if ( config.EqualTo("") || !config ) { ATH_MSG_FATAL("No configuration file specified."); return StatusCode::FAILURE; } 
  m_calibAreaTag.insert(0,"CalibArea-00-04-77/"); // Hard-coding the CalibArea tag
  if(calibSeq.Contains("DEV")){
    m_devMode = true;
    ATH_MSG_WARNING("Dev Mode is ON!!! \n\n");
    ATH_MSG_WARNING("Dev Mode is NOT RECOMMENDED!!! \n\n");
    dir.insert(0,"JetCalibTools/");
    dir.insert(28,m_calibAreaTag); // Obtaining the path of the configuration file
  }
  else{dir.insert(14,m_calibAreaTag);} // Obtaining the path of the configuration file
  std::string configPath=dir+m_config; // Full path
  TString fn =  PathResolverFindCalibFile(configPath);

  ATH_MSG_INFO("Reading global JES settings from: " << m_config);
  ATH_MSG_INFO("resolved in: " << fn);
  
  m_globalConfig = new TEnv();
  //int status=m_globalConfig->ReadFile(FindFile(fn),EEnvLevel(0));
  int status=m_globalConfig->ReadFile(fn ,EEnvLevel(0));
  if (status!=0) { ATH_MSG_FATAL("Cannot read config file " << fn ); return StatusCode::FAILURE; }

  //Make sure that one of the standard jet collections is being used
  if ( calibSeq.Contains("JetArea") ) {
    if ( jetAlgo.Contains("PFlow") ) m_jetScale = PFLOW;
    else if ( jetAlgo.Contains("EM") ) m_jetScale = EM;
    else if ( jetAlgo.Contains("LC") ) m_jetScale = LC;
    else { ATH_MSG_FATAL("jetAlgo " << jetAlgo << " not recognized."); return StatusCode::FAILURE; }
  }

  //Set the default units to MeV, user can override by calling setUnitsGeV(true)
  setUnitsGeV(false);

  // Settings for R21/2.5.X
  m_originCorrectedClusters = m_globalConfig->GetValue("OriginCorrectedClusters",false);
  m_doSetDetectorEta = m_globalConfig->GetValue("SetDetectorEta",true);

  //Make sure the residual correction is turned on if requested, protect against applying it without the jet area subtraction                    
  if ( !calibSeq.Contains("JetArea") && !calibSeq.Contains("Residual") ) {
    m_doJetArea = false;
    m_doResidual = false;
  } else if ( calibSeq.Contains("JetArea") ) {
    if ( m_rhoKey.compare("auto") == 0 ) {
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
    if ( !calibSeq.Contains("Residual") ) m_doResidual = false;
  } else if ( !calibSeq.Contains("JetArea") && calibSeq.Contains("Residual") ) {
    ATH_MSG_FATAL("JetCalibrationTool::initializeTool : You are trying to initialize JetCalibTools with the jet area correction turned off and the residual offset correction turned on. This is inconsistent. Aborting.");
    return StatusCode::SUCCESS;
  }

  if ( !calibSeq.Contains("Origin") ) m_doOrigin = false;

  if ( !calibSeq.Contains("GSC") ) m_doGSC = false;

  //Protect against the in-situ calibration being requested when isData is false
  if ( calibSeq.Contains("Insitu") && !m_isData ) {
    ATH_MSG_FATAL("JetCalibrationTool::initializeTool : calibSeq string contains Insitu with isData set to false. Can't apply in-situ correction to MC!!");
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
      //InsituDataCorrection *insituTemp = NULL;
      //m_insituTimeDependentCorr.push_back(insituTemp);

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

  //Loop over the request calib sequence
  //Initialize derived classes for applying the requested calibrations and add them to a vector
  std::vector<TString> vecCalibSeq = JetCalibUtils::Vectorize(calibSeq,"_");
  TString vecCalibSeqtmp;
  for ( unsigned int i=0; i<vecCalibSeq.size(); ++i) {
    if ( vecCalibSeq[i].EqualTo("Residual") || vecCalibSeq[i].EqualTo("Origin") || vecCalibSeq[i].EqualTo("DEV") ) continue;
    ATH_CHECK( getCalibClass(name,vecCalibSeq[i] ));
  }

  ATH_MSG_INFO("===================================");

  return StatusCode::SUCCESS;
}

//Method for initializing the requested calibration derived classes
StatusCode JetCalibrationTool::getCalibClass(const std::string&name, TString calibration) {
  TString jetAlgo = m_jetAlgo;
  TString calibAreaTag = m_calibAreaTag;
  std::string suffix = "";
  //ATH_MSG_INFO("Initializing sub tools.");
  if ( calibration.EqualTo("JetArea") ) {
    ATH_MSG_INFO("Initializing pileup correction.");
    suffix="_Pileup";
    if(m_devMode) suffix+="_DEV";
    m_jetPileupCorr = new JetPileupCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,m_doResidual,m_doOrigin,m_isData,m_devMode);
    m_jetPileupCorr->msg().setLevel( this->msg().level() );
    if( m_jetPileupCorr->initializeTool(name+suffix).isFailure() ) { 
      ATH_MSG_FATAL("Couldn't initialize the pileup correction. Aborting"); 
      return StatusCode::FAILURE; 
    } else { 
      m_calibClasses.push_back(m_jetPileupCorr); 
      return StatusCode::SUCCESS; 
    }
  } else if ( calibration.EqualTo("EtaJES") || calibration.EqualTo("AbsoluteEtaJES") ) {
    ATH_MSG_INFO("Initializing JES correction.");
    suffix="_EtaJES";
    if(m_devMode) suffix+="_DEV";
    m_etaJESCorr = new EtaJESCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,false,m_devMode);
    m_etaJESCorr->msg().setLevel( this->msg().level() );
    if ( m_etaJESCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the Monte Carlo JES correction. Aborting"); 
      return StatusCode::FAILURE; 
    } else { 
      m_calibClasses.push_back(m_etaJESCorr); 
      return StatusCode::SUCCESS; 
    }
  } else if ( calibration.EqualTo("EtaMassJES") ) {
    ATH_MSG_INFO("Initializing JES correction.");
    suffix="_EtaMassJES";
    if(m_devMode) suffix+="_DEV";
    m_etaJESCorr = new EtaJESCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,true,m_devMode);
    m_etaJESCorr->msg().setLevel( this->msg().level() );
    if ( m_etaJESCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the Monte Carlo JES correction. Aborting");
      return StatusCode::FAILURE;
    } else {
      m_calibClasses.push_back(m_etaJESCorr);
      return StatusCode::SUCCESS;
    }
  } else if ( calibration.EqualTo("GSC") ) {
    ATH_MSG_INFO("Initializing GSC correction.");
    suffix="_GSC";
    if(m_devMode) suffix+="_DEV";
    m_globalSequentialCorr = new GlobalSequentialCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,m_devMode);
    m_globalSequentialCorr->msg().setLevel( this->msg().level() );
    if ( m_globalSequentialCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the Global Sequential Calibration. Aborting"); 
      return StatusCode::FAILURE; 
    } else { 
      m_calibClasses.push_back(m_globalSequentialCorr); 
      return StatusCode::SUCCESS; 
    }
  } else if ( calibration.EqualTo("JMS") ) {
    ATH_MSG_INFO("Initializing JMS correction.");
    suffix="_JMS";
    if(m_devMode) suffix+="_DEV";
    m_jetMassCorr = new JMSCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,m_devMode);
    m_jetMassCorr->msg().setLevel( this->msg().level() );
    if ( m_jetMassCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the JMS Calibration. Aborting");
      return StatusCode::FAILURE;
    } else {
      m_calibClasses.push_back(m_jetMassCorr);
      return StatusCode::SUCCESS;
    }
  } else if ( calibration.EqualTo("Insitu") ) {
    if(!m_timeDependentCalib){
      ATH_MSG_INFO("Initializing Insitu correction.");
      suffix="_Insitu";
      if(m_devMode) suffix+="_DEV";
      m_insituDataCorr = new InsituDataCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,m_devMode);
      m_insituDataCorr->msg().setLevel( this->msg().level() );
      if ( m_insituDataCorr->initializeTool(name+suffix).isFailure() ) {
        ATH_MSG_FATAL("Couldn't initialize the In-situ data correction. Aborting"); 
        return StatusCode::FAILURE; 
      } else { 
        m_calibClasses.push_back(m_insituDataCorr); 
        return StatusCode::SUCCESS; 
      }
    } else{
      ATH_MSG_INFO("Initializing Time-Dependent Insitu Corrections");
      for(unsigned int i=0;i<m_timeDependentInsituConfigs.size();++i){
        suffix="_Insitu"; suffix += "_"; suffix += std::to_string(i);
        if(m_devMode) suffix+="_DEV";
        InsituDataCorrection *insituTimeDependentCorr_Tmp = new InsituDataCorrection(name+suffix,m_globalTimeDependentConfigs.at(i),jetAlgo,calibAreaTag,m_devMode);
        insituTimeDependentCorr_Tmp->msg().setLevel( this->msg().level() );
        if ( insituTimeDependentCorr_Tmp->initializeTool(name+suffix).isFailure() ) {
          ATH_MSG_FATAL("Couldn't initialize the In-situ data correction. Aborting"); 
          return StatusCode::FAILURE; 
        } else {     		
          m_insituTimeDependentCorr.push_back(insituTimeDependentCorr_Tmp); 
        }
      }
      return StatusCode::SUCCESS; 
    }
  }
  ATH_MSG_FATAL("Calibration string not recognized: " << calibration << ", aborting.");
  //ATH_MSG_INFO("Initializing of sub tools is complete.");
  return StatusCode::FAILURE;
}

CP::CorrectionCode JetCalibrationTool::applyCorrection(xAOD::Jet& jet) {
  StatusCode sc = applyCalibration(jet);
  if ( sc != StatusCode::FAILURE ) return CP::CorrectionCode::Ok;
  return CP::CorrectionCode::Error;
}

StatusCode JetCalibrationTool::applyCalibration(xAOD::Jet& jet) const { 
  //Grab necessary event info for pile up correction and store it in a JetEventInfo class object
  JetEventInfo jetEventInfo;
  ATH_MSG_VERBOSE("Extracting event information.");
  ATH_CHECK( initializeEvent(jetEventInfo) );
  ATH_MSG_VERBOSE("Applying calibration.");
  ATH_CHECK( calibrateImpl(jet,jetEventInfo) );
  return StatusCode::SUCCESS; 
}


int JetCalibrationTool::modify(xAOD::JetContainer& jets) const {
  //Grab necessary event info for pile up correction and store it in a JetEventInfo class object
  ATH_MSG_VERBOSE("Modifying jet collection.");
  JetEventInfo jetEventInfo;
  ATH_CHECK( initializeEvent(jetEventInfo), 1 );
  xAOD::JetContainer::iterator jet_itr = jets.begin();
  xAOD::JetContainer::iterator jet_end = jets.end(); 
  for ( ; jet_itr != jet_end; ++jet_itr )
    ATH_CHECK( calibrateImpl(**jet_itr,jetEventInfo), 1 );
 return 0;
}

int JetCalibrationTool::modifyJet(xAOD::Jet& jet) const {
  ATH_MSG_VERBOSE("Modifying jet.");
  ATH_CHECK( applyCalibration(jet), 1 );
  return 0;
}

// Private/Protected Methods
///////////////

/*
StatusCode JetCalibrationTool::initializeEvent() {

  m_eventObj = 0;
  m_vertices = 0;

  //ATH_MSG_INFO("  Retrieving event information for the pile up corrections\n");
  if ( evtStore()->retrieve(m_eventObj,"EventInfo").isFailure() || !m_eventObj ) {
    ATH_MSG_ERROR("   JetCalibrationTool::initializeEvent : Failed to retrieve event information. Aborting.");
    return StatusCode::FAILURE;
  }
  if ( evtStore()->retrieve(m_vertices,"PrimaryVertices").isFailure() || !m_vertices ) {
    ATH_MSG_ERROR("   JetCalibrationTool::initializeEvent : Failed to retrieve primary vertices. Aborting.");
    return StatusCode::FAILURE;
  }
  //ATH_MSG_INFO("  Event information successfully retrieved.\n\n");

  //Check if the input jets are coming from data or MC
  //if ( m_eventObj->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
  //     m_isData = false; // controls mu scaling in the pile up correction, no scaling for data
  //}

  int eventNPV = 0;
  xAOD::VertexContainer::const_iterator vtx_itr = m_vertices->begin();
  xAOD::VertexContainer::const_iterator vtx_end = m_vertices->end(); 
  for ( ; vtx_itr != vtx_end; ++vtx_itr ) 
    if ( (*vtx_itr)->nTrackParticles() >= 2 ) ++eventNPV;

  m_jetEventInfo.NPV = eventNPV;
  m_jetEventInfo.mu = m_eventObj->averageInteractionsPerCrossing();

  //Test code for EventShape EDM
  //If xAODEventShape can't be retrieved from evtStore, default to hard-coded value (12GeV)
  const xAOD::EventShape * eventShape;
  std::string rhoKey = m_jetScale == EM ? "EMTopoEventShape" : "LCTopoEventShape";
  if ( evtStore()->retrieve(eventShape, rhoKey).isFailure() || !eventShape )
    m_jetEventInfo.rho = 12000.;
  else
   if ( !eventShape->getDensity( xAOD::EventShape::DensityForJetsR4, m_jetEventInfo.rho ) ) return StatusCode::FAILURE; 

  return StatusCode::SUCCESS;
}
*/

StatusCode JetCalibrationTool::initializeEvent(JetEventInfo& jetEventInfo) const {

  // Check if the tool was initialized
  if( m_calibClasses.size() == 0 ){
    ATH_MSG_FATAL("   JetCalibrationTool::initializeEvent : The tool was not initialized.");
    return StatusCode::FAILURE;
  }

  // static accessor for PV index access
  static SG::AuxElement::ConstAccessor<int> PVIndexAccessor("PVIndex");
  
  ATH_MSG_VERBOSE("Initializing event.");

  //Determine the rho value to use for the jet area subtraction
  //Should be determined using EventShape object, use hard coded values if EventShape doesn't exist
  double rho=0;
  const xAOD::EventShape * eventShape = 0;

  SG::ReadHandle<xAOD::EventShape> rhRhoKey(m_rhkRhoKey);

  if ( m_doJetArea && rhRhoKey.isValid() ) {
    ATH_MSG_VERBOSE("  Found event density container " << m_rhoKey);
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
    ATH_MSG_VERBOSE("  Rho container not found: " << m_rhoKey);
    ATH_MSG_FATAL("Could not retrieve xAOD::EventShape DataHandle.");
    return StatusCode::FAILURE;
  }
  jetEventInfo.setRho(rho);
  ATH_MSG_VERBOSE("  Rho = " << 0.001*rho << " GeV");

  // Retrieve EventInfo object, which now has multiple uses
  if ( m_doResidual || m_doGSC ) {
    const xAOD::EventInfo * eventObj = 0;
    static unsigned int eventInfoWarnings = 0;
    SG::ReadHandle<xAOD::EventInfo> rhEvtInfo(m_rhkEvtInfo);
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

    // If we are applying the reisdual, then store mu
    if (m_doResidual)
      jetEventInfo.setMu( eventObj->averageInteractionsPerCrossing() );
    
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

    // If PV index is not zero, we need to confirm it's a reasonable value
    // To do this, we need the primary vertices
    // However, other users of the GSC may not have the PV collection (in particular: trigger GSC in 2016)
    // So only retrieve vertices if needed for NPV (residual) or a non-zero PV index was specified (GSC)
    if (m_doResidual || (m_doGSC && jetEventInfo.PVIndex()))
    {
      //Retrieve VertexContainer object, use it to obtain NPV for the residual correction or check validity of GSC non-PV0 usage
      const xAOD::VertexContainer * vertices = 0;
      static unsigned int vertexContainerWarnings = 0;

      SG::ReadHandle<xAOD::VertexContainer> rhPV(m_rhkPV);
      if (rhPV.isValid()) {
        vertices = rhPV.cptr(); 
      } else {
        ++vertexContainerWarnings;
        if ( vertexContainerWarnings < 20 )
          ATH_MSG_ERROR("   JetCalibrationTool::initializeEvent : Failed to retrieve primary vertices.");
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
            ATH_MSG_ERROR("   JetCalibrationTool::initializeEvent : PV index is out of bounds.");
          jetEventInfo.setPVIndex(0); // Hard coded value PVIndex = 0 in case of failure (to prevent seg faults later).
          return StatusCode::SUCCESS; // error is recoverable, so return SUCCESS
        }
      }
    }

    //Check if the input jets are coming from data or MC
    //if ( m_eventObj->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
    //     m_isData = false; // controls mu scaling in the pile up correction, no scaling for data
    //}

  }

    return StatusCode::SUCCESS;
}

StatusCode JetCalibrationTool::calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const {

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
  for (unsigned int i=0; i<m_calibClasses.size(); ++i) //Loop over requested calibations
    ATH_CHECK ( m_calibClasses[i]->calibrateImpl(jet,jetEventInfo) );
  TString CalibSeq = m_calibSeq;
  if(CalibSeq.Contains("Insitu") && m_timeDependentCalib){ // Insitu Time-Dependent Correction
    for(unsigned int i=0;i<m_timeDependentInsituConfigs.size();++i){
      // Retrieve EventInfo container
      const xAOD::EventInfo* eventInfo(nullptr);
      SG::ReadHandle<xAOD::EventInfo> rhEvtInfo(m_rhkEvtInfo);
      if ( rhEvtInfo.isValid() ) {
        eventInfo = rhEvtInfo.cptr();
      } else {
        ATH_MSG_ERROR("   JetCalibrationTool::calibrateImpl : Failed to retrieve EventInfo.");
      }
      // Run Number Dependent Correction
      double runNumber = eventInfo->runNumber();
      if(runNumber>m_runBins.at(i) && runNumber<=m_runBins.at(i+1)){ ATH_CHECK ( m_insituTimeDependentCorr.at(i)->calibrateImpl(jet,jetEventInfo) );}
    }
  }
  return StatusCode::SUCCESS; 
}
