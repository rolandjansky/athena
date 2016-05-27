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

//Default constructor -- likely to be used by Athena based analyses
JetCalibrationTool::JetCalibrationTool()
  : asg::AsgTool( "JetCalibrationTool::JetCalibrationTool" ),  JetCalibrationToolBase::JetCalibrationToolBase(),
    m_jetAlgo(""), m_config(""), m_calibSeq(""), m_calibAreaTag(""), m_devMode(false), m_isData(true), m_mass(false), m_rhoKey(""), m_dir(""), m_eInfoName(""), m_globalConfig(NULL),
    m_doJetArea(true), m_doResidual(true), m_doOrigin(true), m_doGSC(true),
    m_jetPileupCorr(NULL), m_etaJESCorr(NULL), m_globalSequentialCorr(NULL), m_insituDataCorr(NULL), m_jetMassCorr(NULL)
{ 

  declareProperty( "JetCollection", m_jetAlgo = "AntiKt4LCTopo" );
  declareProperty( "RhoKey", m_rhoKey = "auto" );
  declareProperty( "ConfigFile", m_config = "" );
  declareProperty( "CalibSequence", m_calibSeq = "JetArea_Residual_AbsoluteEtaJES_Insitu" );
  declareProperty( "IsData", m_isData = true );
  declareProperty( "ConfigDir", m_dir = "JetCalibTools/CalibrationConfigs/" );
  declareProperty( "EventInfoName", m_eInfoName = "EventInfo" );

}

JetCalibrationTool::JetCalibrationTool(const std::string& name)
  : asg::AsgTool( name ),  JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_jetAlgo(""), m_config(""), m_calibSeq(""), m_calibAreaTag(""), m_devMode(false), m_isData(true), m_mass(false), m_rhoKey("auto"), m_dir(""), m_eInfoName(""), m_globalConfig(NULL),
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

//Contructor for Root based analyses
JetCalibrationTool::JetCalibrationTool(const std::string& name, TString jetAlgo, TString config, TString calibSeq, bool isData, bool mass, TString rhoKey, TString dir, TString eInfoName)
  : asg::AsgTool( name ),
    m_jetAlgo(jetAlgo), m_config(config), m_calibSeq(calibSeq), m_calibAreaTag(""), m_devMode(false), m_isData(isData), m_mass(mass), m_rhoKey(rhoKey), m_dir(dir), m_eInfoName(eInfoName), m_globalConfig(NULL),
    m_doJetArea(true), m_doResidual(true), m_doOrigin(true), m_doGSC(true), 
    m_jetPileupCorr(NULL), m_etaJESCorr(NULL), m_globalSequentialCorr(NULL), m_insituDataCorr(NULL), m_jetMassCorr(NULL)
{ 

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

  ATH_MSG_INFO("===================================\n");
  ATH_MSG_INFO("Initializing the xAOD Jet Calibration Tool for " << jetAlgo << "jets\n");

  //Make sure the necessary properties were set via the constructor or python configuration
  if ( jetAlgo.EqualTo("") || calibSeq.EqualTo("") ) {
    ATH_MSG_FATAL("JetCalibrationTool::initializeTool : At least one of your constructor arguments is not set. Did you use the copy constructor?");
    return StatusCode::FAILURE;
  }

  if ( config.EqualTo("") || !config ) { ATH_MSG_FATAL("No configuration file specified."); return StatusCode::FAILURE; } 
  m_calibAreaTag.insert(0,"CalibArea-00-04-65/"); // Hard-coding the CalibArea tag
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
  ATH_MSG_INFO("resolved in: " << fn << "\n");
  
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

  //Make sure the residual correction is turned on if requested, protect against applying it without the jet area subtraction                    
  if ( !calibSeq.Contains("JetArea") && !calibSeq.Contains("Residual") ) {
    m_doJetArea = false;
    m_doResidual = false;
  } else if ( calibSeq.Contains("JetArea") ) {
    if ( m_rhoKey.compare("auto") == 0 ) {
      if ( m_jetScale == EM ) m_rhoKey = "Kt4EMTopoEventShape";
      else if ( m_jetScale == LC ) m_rhoKey = "Kt4LCTopoEventShape";
      else if ( m_jetScale == PFLOW ) m_rhoKey = "Kt4EMPFlowEventShape";
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

  //Loop over the request calib sequence
  //Initialize derived classes for applying the requested calibrations and add them to a vector
  std::vector<TString> vecCalibSeq = JetCalibUtils::Vectorize(calibSeq,"_");
  TString vecCalibSeqtmp;
  for ( unsigned int i=0; i<vecCalibSeq.size(); ++i) {
    if ( vecCalibSeq[i].EqualTo("Residual") || vecCalibSeq[i].EqualTo("Origin") || vecCalibSeq[i].EqualTo("DEV") ) continue;
    vecCalibSeqtmp = vecCalibSeq[i];
    if(m_devMode) vecCalibSeqtmp += "DEV";
    ATH_CHECK( getCalibClass(name,vecCalibSeqtmp ));
  }

  ATH_MSG_INFO("===================================");

  return StatusCode::SUCCESS;
}

//Method for initializing the requested calibration derived classes
StatusCode JetCalibrationTool::getCalibClass(const std::string&name, TString calibration) {
  TString jetAlgo = m_jetAlgo;
  TString calibAreaTag = m_calibAreaTag;
  std::string suffix = "";
  bool dev = false;
  if(calibration.Contains("DEV")){
    dev = true;
    suffix += "_DEV";
  }
  //ATH_MSG_INFO("Initializing sub tools.");
  if ( calibration.Contains("JetArea") ) {
    ATH_MSG_INFO("Initializing pileup correction.");
    suffix="_Pileup";
    m_jetPileupCorr = new JetPileupCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,m_doResidual,m_doOrigin,m_isData,dev);
    m_jetPileupCorr->msg().setLevel( this->msg().level() );
    if( m_jetPileupCorr->initializeTool(name+suffix).isFailure() ) { 
      ATH_MSG_FATAL("Couldn't initialize the pileup correction. Aborting"); 
      return StatusCode::FAILURE; 
    } else { 
      m_calibClasses.push_back(m_jetPileupCorr); 
      return StatusCode::SUCCESS; 
    }
  } else if ( calibration.Contains("EtaJES") ) {
    ATH_MSG_INFO("Initializing JES correction.");
    suffix="_EtaJES";
    m_etaJESCorr = new EtaJESCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,false,dev);
    m_etaJESCorr->msg().setLevel( this->msg().level() );
    if ( m_etaJESCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the Monte Carlo JES correction. Aborting"); 
      return StatusCode::FAILURE; 
    } else { 
      m_calibClasses.push_back(m_etaJESCorr); 
      return StatusCode::SUCCESS; 
    }
  } else if ( calibration.Contains("EtaMassJES") ) {
    ATH_MSG_INFO("Initializing JES correction.");
    suffix="_EtaMassJES";
    m_etaJESCorr = new EtaJESCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,true,dev);
    m_etaJESCorr->msg().setLevel( this->msg().level() );
    if ( m_etaJESCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the Monte Carlo JES correction. Aborting");
      return StatusCode::FAILURE;
    } else {
      m_calibClasses.push_back(m_etaJESCorr);
      return StatusCode::SUCCESS;
    }
  } else if ( calibration.Contains("GSC") ) {
    ATH_MSG_INFO("Initializing GSC correction.");
    suffix="_GSC";
    m_globalSequentialCorr = new GlobalSequentialCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,dev);
    m_globalSequentialCorr->msg().setLevel( this->msg().level() );
    if ( m_globalSequentialCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the Global Sequential Calibration. Aborting"); 
      return StatusCode::FAILURE; 
    } else { 
      m_calibClasses.push_back(m_globalSequentialCorr); 
      return StatusCode::SUCCESS; 
    }
  } else if ( calibration.Contains("JMS") ) {
    ATH_MSG_INFO("Initializing JMS correction.");
    suffix="_JMS";
    m_jetMassCorr = new JMSCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,dev);
    m_jetMassCorr->msg().setLevel( this->msg().level() );
    if ( m_jetMassCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the JMS Calibration. Aborting");
      return StatusCode::FAILURE;
    } else {
      m_calibClasses.push_back(m_jetMassCorr);
      return StatusCode::SUCCESS;
    }
  } else if ( calibration.Contains("Insitu") ) {
    ATH_MSG_INFO("Initializing Insitu correction.");
    suffix="_Insitu";
    m_insituDataCorr = new InsituDataCorrection(name+suffix,m_globalConfig,jetAlgo,calibAreaTag,dev);
    m_insituDataCorr->msg().setLevel( this->msg().level() );
    if ( m_insituDataCorr->initializeTool(name+suffix).isFailure() ) {
      ATH_MSG_FATAL("Couldn't initialize the In-situ data correction. Aborting"); 
      return StatusCode::FAILURE; 
    } else { 
      m_calibClasses.push_back(m_insituDataCorr); 
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
  ATH_CHECK( initializeEvent(jetEventInfo) );
  xAOD::JetContainer::iterator jet_itr = jets.begin();
  xAOD::JetContainer::iterator jet_end = jets.end(); 
  for ( ; jet_itr != jet_end; ++jet_itr )
    ATH_CHECK( calibrateImpl(**jet_itr,jetEventInfo) );
 return 0;
}

int JetCalibrationTool::modifyJet(xAOD::Jet& jet) const {
  ATH_MSG_VERBOSE("Modifying jet.");
  ATH_CHECK( applyCalibration(jet) );
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
  // static accessor for PV index access
  static SG::AuxElement::ConstAccessor<int> PVIndexAccessor("PVIndex");
  
  ATH_MSG_VERBOSE("Initializing event.");

  //Determine the rho value to use for the jet area subtraction
  //Should be determined using EventShape object, use hard coded values if EventShape doesn't exist
  double rho=0;
  const xAOD::EventShape * eventShape = 0;
  //std::string rhoKey = m_jetScale == EM ? "Kt4EMTopoEventShape" : "Kt4LCTopoEventShape";
  static unsigned int eventShapeWarnings = 0;
  if ( m_doJetArea && evtStore()->contains<xAOD::EventShape>(m_rhoKey) ) {
    ATH_MSG_VERBOSE("  Found event density container " << m_rhoKey);
    if ( evtStore()->retrieve(eventShape, m_rhoKey).isFailure() || !eventShape ) {
      ATH_MSG_VERBOSE("  Event shape container not found.");
      ++eventShapeWarnings;
      rho = ( m_jetScale == EM || m_jetScale == PFLOW ? 6000. : 12000.);
      if ( eventShapeWarnings < 20 )
        ATH_MSG_WARNING("Could not retrieve xAOD::EventShape from evtStore, using hard-coded value, rho = " << rho/m_GeV << " GeV.");
    } else if ( !eventShape->getDensity( xAOD::EventShape::Density, rho ) ) {
      ATH_MSG_VERBOSE("  Event density not found in container.");
      ++eventShapeWarnings;
      rho = ( m_jetScale == EM || m_jetScale == PFLOW ? 6000. : 12000.);
      if ( eventShapeWarnings < 20 )
        ATH_MSG_WARNING("Could not retrieve xAOD::EventShape::Density from xAOD::EventShape, using hard-coded value, rho = " << rho/m_GeV << " GeV.");
    } else {
      ATH_MSG_VERBOSE("  Event density retrieved.");
    }
  } else if ( m_doJetArea && !evtStore()->contains<xAOD::EventShape>(m_rhoKey) ) {
    ATH_MSG_VERBOSE("  Rho container not found: " << m_rhoKey);
    ++eventShapeWarnings;
    rho = ( m_jetScale == EM ? 6000. : 12000.);
    if ( eventShapeWarnings < 20 )
      ATH_MSG_WARNING("Could not retrieve xAOD::EventShape from evtStore, using hard-coded value, rho = " << rho/m_GeV << " GeV.");
  }
  jetEventInfo.setRho(rho);
  ATH_MSG_VERBOSE("  Rho = " << 0.001*rho << " GeV");

  //Retrieve EventInfo object, use it to obtain mu for the residual correction
  if ( m_doResidual || m_doGSC ) {
    const xAOD::EventInfo * eventObj = 0;
    static unsigned int eventInfoWarnings = 0;
    if ( evtStore()->retrieve(eventObj,m_eInfoName).isFailure() || !eventObj ) {
      ++eventInfoWarnings;
      if ( eventInfoWarnings < 20 )
        ATH_MSG_ERROR("   JetCalibrationTool::initializeEvent : Failed to retrieve event information.");
      jetEventInfo.setMu(0); //Hard coded value mu = 0 in case of failure (to prevent seg faults later).
      jetEventInfo.setPVIndex(0);
      return StatusCode::SUCCESS; //error is recoverable, so return SUCCESS
    }
    jetEventInfo.setMu( eventObj->averageInteractionsPerCrossing() );


    //Check if the input jets are coming from data or MC
    //if ( m_eventObj->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
    //     m_isData = false; // controls mu scaling in the pile up correction, no scaling for data
    //}

    //Retrieve VertexContainer object, use it to obtain NPV for the residual correction
    const xAOD::VertexContainer * vertices = 0;
    static unsigned int vertexContainerWarnings = 0;
    if ( evtStore()->retrieve(vertices,"PrimaryVertices").isFailure() || !vertices ) {
      ++vertexContainerWarnings;
      if ( vertexContainerWarnings < 20 )
        ATH_MSG_ERROR("   JetCalibrationTool::initializeEvent : Failed to retrieve primary vertices.");
      jetEventInfo.setNPV(0); //Hard coded value NPV = 0 in case of failure (to prevent seg faults later).
      return StatusCode::SUCCESS; //error is recoverable, so return SUCCESS
    }

    int eventNPV = 0;
    xAOD::VertexContainer::const_iterator vtx_itr = vertices->begin();
    xAOD::VertexContainer::const_iterator vtx_end = vertices->end(); 
    for ( ; vtx_itr != vtx_end; ++vtx_itr ) 
      if ( (*vtx_itr)->nTrackParticles() >= 2 ) ++eventNPV;
  
    jetEventInfo.setNPV(eventNPV);

    // Retrieve the PVIndex if specified
    // Default is to not specify this, so no warning if it doesn't exist
    // However, if specified, it should be a sane value - fail if not
    if ( m_doGSC && PVIndexAccessor.isAvailable(*eventObj) )
      jetEventInfo.setPVIndex( PVIndexAccessor(*eventObj) );
    else
      jetEventInfo.setPVIndex(0);
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
  xAOD::JetFourMom_t jetconstitP4 = jet.getAttribute<xAOD::JetFourMom_t>("JetConstitScaleMomentum");
  jet.setAttribute<float>("DetectorEta",jetconstitP4.eta()); //saving constituent scale eta for later use
  for (unsigned int i=0; i<m_calibClasses.size(); ++i) //Loop over requested calibations
    ATH_CHECK ( m_calibClasses[i]->calibrateImpl(jet,jetEventInfo) );
  return StatusCode::SUCCESS; 
}
