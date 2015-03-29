/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MagFieldAthenaSvc.h"
#include "BFieldAth/MagFieldSvcWrapper.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "PathResolver/PathResolver.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h" 
#include "StoreGate/StoreGateSvc.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "BFieldCore/MagFieldAccess.h"
//#include "BFieldStand/MagFieldStand.h"
//#include "BFieldStand/MagFieldFactory.h"
//#include "BFieldStand/MagFieldStandXX.h"
//#include "BFieldStand/MFSXXNoTilt.h"
//#include "BFieldStand/noBfield.h"
//#include "BFieldStand/MagFieldStandH8.h"
//#include "BFieldStand/MagFieldStandH8Sym.h"
#include "BFieldCore/MagFieldParam.h" 
// Exception handling for non-existing fields in the database
#include <stdexcept>
#include "CoralBase/AttributeListException.h"

// Standard Constructor
MagFieldAthenaSvc::MagFieldAthenaSvc(const std::string& name,ISvcLocator* svc):
  base_class(name,svc),
  m_geoModel("GeoModelSvc", name),
  par_onlineSolCur(0.),par_onlineTorCur(0.),
  par_setupcool(false),par_usedcs(false),
  par_scaletolerance(1.E-3),par_zerotolerance(1.0),par_currenttolerance(5.),
  par_maptolerance(0.01),par_crashmap(false),
  m_coolBFieldMap("/GLOBAL/BField/Map"),
  m_coolCurrents("/EXT/DCS/MAGNETS/SENSORDATA"),
  m_refSolCur(7730.),m_refTorCur(20400.),
  m_detStore("DetectorStore", name),
  m_gotcooldata(false),m_coolupdated(false),
  m_getbad(0),m_getgood(0),m_getcoolbad(0),m_getcoolgood(0),
  m_fieldService("MagField::AtlasFieldSvc/AtlasFieldSvc",name) {
  //
  p_MagFieldAthena                = 0       ;
  //  std::cout << " before create param in constructor MagfieldAthSvc " << std::endl;
  semiAnSol                       = 99999   ;
  m_alreadyReadFile               = ""      ;
  par_param.m_NameOfTheFile       = ""      ;
  par_param.m_NameOfTheSource     = "ORACLE";
  par_param.m_NoField             = ""      ;
  par_param.m_SolenoidField       = ""      ;
  par_param.calcul                = false   ;
  //  Declare Properties
  declareProperty("NameOfTheBfieldFile", par_param.m_NameOfTheFile)  ;
  declareProperty("NameOfTheSource",     par_param.m_NameOfTheSource);
  declareProperty("SolenoidField",       par_param.m_SolenoidField);
  declareProperty("NOFIELD",             par_param.m_NoField);
  declareProperty("Calcul",              par_param.calcul);
  /*
   * Temporary properties until we can read the "online" current from COOL.
   * These are set by the HLT framework *before* BeginRun is fired (by reading
   * the real-time currents directly from DCS). Only used if NameOfTheSource=="COOL_HLT"
   */  
  declareProperty("onlineSolCur",par_onlineSolCur,"Solenoid current for online running");
  declareProperty("onlineTorCur",par_onlineTorCur,"Toroid current for online running");
    
  declareProperty("noTiltNoShift", par_param.noTiltNoShift);
  declareProperty("InfosChain"   , par_param.m_InfosChain);
  declareProperty("itf"    , par_param.m_itf);
  declareProperty("dx"     , par_param.m_dx) ;
  declareProperty("dy"     , par_param.m_dy) ;
  declareProperty("dz"     , par_param.m_dz) ;
  declareProperty("dphi"   , par_param.m_dphi)    ;
  declareProperty("dtheta" , par_param.m_dtheta)  ;
  declareProperty("dpsi"   , par_param.m_dpsi)    ;
  // Solenoid displacements
  declareProperty("dxsol"     , par_param.m_dxsol) ;
  declareProperty("dysol"     , par_param.m_dysol) ;
  declareProperty("dzsol"     , par_param.m_dzsol) ;
  declareProperty("dphisol"   , par_param.m_dphisol)    ;
  declareProperty("dthetasol" , par_param.m_dthetasol)  ;
  declareProperty("dpsisol"   , par_param.m_dpsisol)    ;
  // COOL stuff
  declareProperty("SetupCOOL",par_setupcool);
  declareProperty("UseDCS",par_usedcs);
  declareProperty("ScaleTolerance",par_scaletolerance);
  declareProperty("ZeroTolerance",par_zerotolerance);
  declareProperty("CurrentTolerance",par_currenttolerance);
  declareProperty("MapTolerance",par_maptolerance);
  declareProperty("CrashMap",par_crashmap);
  //
  //  Test H8 only  
  declareProperty("H8dx1"  , par_param.m_h8dx1) ;// mpbs1 displacement
  declareProperty("H8dy1"  , par_param.m_h8dy1) ;
  declareProperty("H8dz1"  , par_param.m_h8dz1) ;
  declareProperty("H8dx2"  , par_param.m_h8dx2) ;// mpbl displacement
  declareProperty("H8dy2"  , par_param.m_h8dy2) ;
  declareProperty("H8dz2"  , par_param.m_h8dz2) ;
  declareProperty("H8dx3"  , par_param.m_h8dx3) ;// mpbs2 displacement
  declareProperty("H8dy3"  , par_param.m_h8dy3) ;
  declareProperty("H8dz3"  , par_param.m_h8dz3) ;

  declareProperty("UseIMagFieldSvc"  , m_useFieldService = true ) ;
  // par_param.printParam();
  // std::cout << " End constructor MagfieldAthSvc " << std::endl;
  // std::cout << " ************************** " << std::endl;
  //
  alreadyReadFile = false;
}
 
//*Standard Destructor
MagFieldAthenaSvc::~MagFieldAthenaSvc(){}

void MagFieldAthenaSvc::getTagOracle (StatusCode &Sc) {
  ATH_MSG_INFO(  "***********************************************" );
  ATH_MSG_INFO(  "* Retrieving Magnetic Field from DD DataBase! *" );
  ATH_MSG_INFO(  "***********************************************" );
  //
  // Get GeoModelSvc and RDBAccessSvc in order to get the bfield options from the database
  IRDBAccessSvc * accessSvc;
  Sc = service("RDBAccessSvc",accessSvc);
  if (!Sc.isSuccess()) {
    ATH_MSG_ERROR(  "Unable to get RDBAccessSvc." );
    //   return Sc;
  }
  accessSvc->connect();
  //
  // Check for NoField from Oracle
  std::string bfTagName = m_geoModel->magFieldVersion();
  if(bfTagName.find("NOFIELD")!=std::string::npos){
    m_param.m_NoField = "NOFIELD";
  } else{  
    getBfieldFileName (&*m_geoModel, accessSvc);
    getBfieldDisplacements (&*m_geoModel, accessSvc);
    getCurrent (&*m_geoModel, accessSvc);
  }
  msg(MSG::INFO) << "bfTagName: " << bfTagName;
  if(m_geoModel->magFieldVersion()==m_geoModel->magFieldVersionOverride())
    msg() << " ! Overrides " << accessSvc->getChildTag("MagneticField",m_geoModel->atlasVersion(),"ATLAS");
  msg() << endreq;
  
  //if(bfTagName.find("CSC-01-01-00")!=std::string::npos || bfTagName.find("CSC-01-02-00")!=std::string::npos ){
  if((m_geoModel->atlasVersion().find("CSC-01-01-00")!=std::string::npos || 
      m_geoModel->atlasVersion().find("CSC-01-02-00")!=std::string::npos) && m_geoModel->magFieldVersionOverride().empty() ){
    m_param.m_itf=1;
    ATH_MSG_INFO(  "ATTENTION Tilt is wrong but bfield is coherent with simulation: " << m_geoModel->atlasVersion() );
    ATH_MSG_INFO(  "itf       : " << m_param.m_itf );
  } else {
    ATH_MSG_INFO(  "ATTENTION : " << m_geoModel->atlasVersion() );
    ATH_MSG_INFO(  "itf       : " << m_param.m_itf );
  }
  accessSvc->disconnect();
}

void MagFieldAthenaSvc::getTagOracle2 (StatusCode &Sc) {
  ATH_MSG_INFO(  "INFOSBMAG from Oracle old mecansime < release 16...!" );
  IRDBAccessSvc * accessSvc;
  Sc = service("RDBAccessSvc",accessSvc);
  if (!Sc.isSuccess()) {
    ATH_MSG_ERROR(  "Unable to get RDBAccessSvc." );
    //   return Sc;
  }
  accessSvc->connect();
  std::string bfTagName = m_geoModel->magFieldVersion();
  getBfieldFileName2 (&*m_geoModel, accessSvc);
  accessSvc->disconnect();
}

void MagFieldAthenaSvc::getBfieldFileName(const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc){
  ATH_MSG_INFO(  "Using Atlas Geometry version tag: " << geoModel->atlasVersion() );
  if(geoModel->magFieldVersionOverride()!="")
    ATH_MSG_INFO(  "Default MagneticField tag overriden by " << geoModel->magFieldVersionOverride() );
  const IRDBRecordset *bfieldOptionsSet = accessSvc->getRecordset("MagneticFieldOptions",
								  geoModel->magFieldVersion(),"MagneticField");
//   const IRDBRecordset *bfieldOptionsSet = accessSvc->getRecordset("MagneticFieldOptions","MagneticFieldOptions-05");
  if (bfieldOptionsSet->size()==0){
    ATH_MSG_FATAL(  "bfieldOptionsSet->size() is 0" );
    //return StatusCode::FAILURE;
  }
  const IRDBRecord *bfieldOptions =  (*bfieldOptionsSet)[0];
  m_param.m_NameOfTheFile    = bfieldOptions->getString("FILENAME");
  ATH_MSG_INFO(  "Using Atlas Geometry version tag: " << m_param.m_NameOfTheFile );


  try
  {
    if(bfieldOptions->isFieldNull("SOLENOID_FILE")){
      semiAnSol = 0;
      ATH_MSG_INFO(  "****************************************************" );
      ATH_MSG_INFO(  "* No Solenoid Map available for this tag           *" );
      ATH_MSG_INFO(  "* We use ATLM Solenoid semi-Analytical Calculation *" );
      ATH_MSG_INFO(  "****************************************************" );
      ATH_MSG_INFO(  "* semiAnSol =   "<< semiAnSol  );
    }else{
      m_param.m_SolenoidField  = bfieldOptions->getString("SOLENOID_FILE");
      ATH_MSG_INFO(  "Solenoid Map available for this tag is: " << m_param.m_SolenoidField );
    }

    if(bfieldOptions->isFieldNull("INFOSBMAG")){
      m_param.m_InfosChain = 0;
      ATH_MSG_INFO(  "InfosBMAG From Oracle, Old chain NO CHANGE: "<< m_param.m_InfosChain  );
    }else{
      m_param.m_InfosChain  = bfieldOptions->getInt("INFOSBMAG");
      ATH_MSG_INFO(  "InfosBMAG From Oracle, New chain Correction: " << m_param.m_InfosChain );
    }

  }
  catch(std::runtime_error& ex)
  {
    ATH_MSG_DEBUG(  "Exception caught. " << ex.what() );
    ATH_MSG_INFO(  "No Solenoid Map available for this tag " );
  }
}
void MagFieldAthenaSvc::getBfieldFileName2(const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc){
  const IRDBRecordset *bfieldOptionsSet = accessSvc->getRecordset("MagneticFieldOptions",
								  geoModel->magFieldVersion(),"MagneticField");
  if (bfieldOptionsSet->size()==0){
    ATH_MSG_FATAL(  "bfieldOptionsSet->size() is 0" );
    //return StatusCode::FAILURE;
  }
  const IRDBRecord *bfieldOptions =  (*bfieldOptionsSet)[0];
  try
  {
    if(bfieldOptions->isFieldNull("INFOSBMAG")){
      m_param.m_InfosChain = 0;
      ATH_MSG_INFO(  "Old chain NO CHANGE (2):  "<< m_param.m_InfosChain  );
    }else{
      m_param.m_InfosChain  = bfieldOptions->getInt("INFOSBMAG");
      ATH_MSG_INFO(  "New chain Correction (2): " << m_param.m_InfosChain );
    }
  }
  catch(std::runtime_error& ex)
  {
    ATH_MSG_DEBUG(  "Exception caught. " << ex.what() );
  }
}

void MagFieldAthenaSvc::getBfieldDisplacements(const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc){
  //
  //* Tilts & shifts for the full map
  //* new way to get BFieldDisplacements
  const IRDBRecordset *bfieldDisplacementsSet = accessSvc->getRecordset("BFieldDisplacements",
									geoModel->magFieldVersion(),"MagneticField");
  if(bfieldDisplacementsSet->size()==0){
    ATH_MSG_DEBUG(  "bfieldDisplacementsSet->size() is 0, trying the default set" );
    bfieldDisplacementsSet = accessSvc->getRecordset("BFieldDisplacements","BFieldDisplacements-01");
    if(bfieldDisplacementsSet->size()==0){
      ATH_MSG_FATAL(  "bfieldDisplacementsSet->size() is 0");
      //return StatusCode::FAILURE;
    }
  }
  std::string tag = accessSvc->getChildTag("BFieldDisplacements", geoModel->magFieldVersion(),"MagneticField"); 
  ATH_MSG_INFO(  "BFieldDisplacements   tag = "<< tag );
  if(tag == "BFieldDisplacements-01" || tag == "BFieldDisplacements-03" || tag == ""){
    m_param.noTiltNoShift = 1;
    ATH_MSG_INFO(  "NO Shifts & NO Tilts For the Global map: tag = "<< tag );
  }else{
    m_param.noTiltNoShift = 0;
    ATH_MSG_INFO(  "   Shifts &    Tilts: tag = "<< tag );
  }
  
  for(size_t index = 0; index < bfieldDisplacementsSet->size(); index++){
    const IRDBRecord *bfieldDisplacements =  (*bfieldDisplacementsSet)[index];
    if(bfieldDisplacements->isFieldNull("FIELDNAME")){
      ATH_MSG_INFO(  " Only Global displacement: BFieldDisplacements Global Map" );
      m_param.m_dx        = bfieldDisplacements->getDouble("DX");
      m_param.m_dy        = bfieldDisplacements->getDouble("DY");
      m_param.m_dz        = bfieldDisplacements->getDouble("DZ");
      m_param.m_dtheta    = bfieldDisplacements->getDouble("DALPHA");
      m_param.m_dphi      = bfieldDisplacements->getDouble("DBETA");
      m_param.m_dpsi      = bfieldDisplacements->getDouble("DGAMMA");
    }else{
      std::string aaa = bfieldDisplacements->getString("FIELDNAME");
      if( aaa == "GlobalMap"){
	ATH_MSG_INFO(  "BFieldDisplacements Global Map" );
	m_param.m_dx        = bfieldDisplacements->getDouble("DX");
	m_param.m_dy        = bfieldDisplacements->getDouble("DY");
	m_param.m_dz        = bfieldDisplacements->getDouble("DZ");
	m_param.m_dtheta    = bfieldDisplacements->getDouble("DALPHA");
	m_param.m_dphi      = bfieldDisplacements->getDouble("DBETA");
	m_param.m_dpsi      = bfieldDisplacements->getDouble("DGAMMA");
      }else if( aaa == "SolenoidMap"){
	ATH_MSG_INFO(  "BFieldDisplacements For Solenoid " );
	m_param.m_dxsol     = bfieldDisplacements->getDouble("DX");
	m_param.m_dysol     = bfieldDisplacements->getDouble("DY");
	m_param.m_dzsol     = bfieldDisplacements->getDouble("DZ");
	m_param.m_dthetasol = bfieldDisplacements->getDouble("DALPHA");
	m_param.m_dphisol   = bfieldDisplacements->getDouble("DBETA");
	m_param.m_dpsisol   = bfieldDisplacements->getDouble("DGAMMA");
      }
    }
    ATH_MSG_INFO(  "BFieldDisplacements printParam() " );
    std::ostringstream outperso;
    m_param.printParam(outperso);
    ATH_MSG_INFO(  outperso.str() );
  }
}

// Get Current for Toroids and Solenoid from oracle (mainly for the simulation)
void MagFieldAthenaSvc::getCurrent(const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc){
  const IRDBRecordset *bfieldScaleSet = accessSvc->getRecordset("MagneticFieldOptions",geoModel->magFieldVersion(),"MagneticField");
  ATH_MSG_INFO(  "Using Atlas Geometry version tag: " << geoModel->atlasVersion() );
  if(geoModel->magFieldVersionOverride()!="")
    ATH_MSG_INFO(  "Default MagneticField tag overriden by " << geoModel->magFieldVersionOverride() );
  if (bfieldScaleSet->size()==0) ATH_MSG_FATAL(  "bfieldScaleSet->size() is 0" );
  const IRDBRecord *bfieldScale = (*bfieldScaleSet)[0];
  if (bfieldScale->isFieldNull("SOL_CURRENT")) {
    m_param.m_SolCur=0.;
  } else {
    m_param.m_SolCur = bfieldScale->getDouble("SOL_CURRENT");
  }
  ATH_MSG_INFO(  "Using Atlas Geometry the b-field current for the solenoid is " << m_param.m_SolCur    << " amps " );
  ATH_MSG_INFO(  "                                        the nominal value is " << m_param.m_SolCurNom << " amps " );
  if (bfieldScale->isFieldNull("TOROIDS_CURRENT")) {
    m_param.m_TorCur=0.;
  } else {
    m_param.m_TorCur = bfieldScale->getDouble("TOROIDS_CURRENT");
  }
  ATH_MSG_INFO(  "Using Atlas Geometry the b-field current for the toroids is  " << m_param.m_TorCur    << " amps " );
  ATH_MSG_INFO(  "                                        the nominal value is " << m_param.m_TorCurNom << " amps " );
  if(m_param.m_TorCur != 0 || m_param.m_SolCur != 0) m_param.doScale = true;
}

//
//*Service initialisation
StatusCode MagFieldAthenaSvc::initialize() {
  //
  ATH_MSG_INFO(  "in initialize()" );
  //
  ATH_CHECK( AthService::initialize() );
  ATH_CHECK( m_detStore.retrieve() );

  // get the parameters from the joboption buffer
  m_param=par_param;

  // book COOL callback if needed
  if (par_setupcool || m_param.m_NameOfTheSource == "COOL") {
    ATH_CHECK( m_detStore->regFcn(
      &IMagFieldAthenaSvc::updateField,dynamic_cast<IMagFieldAthenaSvc *>(this),
      h_coolBFieldMap,m_coolBFieldMap) );
    ATH_MSG_INFO(  "Booked callback for " << m_coolBFieldMap );

    if (par_usedcs) {
      // boook callback for BField current DCS data
      ATH_CHECK( m_detStore->regFcn(
          &IMagFieldAthenaSvc::updateField,
          dynamic_cast<IMagFieldAthenaSvc*>(this),
          h_coolCurrents,m_coolCurrents) );
      ATH_MSG_INFO(  "Booked callback for " << m_coolCurrents );
    }
  }

  if (m_param.m_NameOfTheSource == "ORACLE") { 
    ATH_MSG_INFO(  " Magnetic Field information from ORACLE " );
    //
    ATH_CHECK( m_geoModel.retrieve() );

    //
    //*Dummy parameters for the callback
    int par1 = 0;
    std::list<std::string> par2;

    if(m_geoModel->geoInitialized())
      //*GeoModelSvc is configured by job options
      return initGeoModel(par1,par2);
    else
      {
	//*GeoModelSvc is configured automatically
	//*Register MagFieldAthenaSvc::initGeoModel() as a callback function on DetStore
	//*to be called after IGeoModelSvc::align()
	ATH_CHECK( m_detStore->regFcn(&IGeoModelSvc::align,&*m_geoModel,
                                      &MagFieldAthenaSvc::initGeoModel,this) );
        return StatusCode::SUCCESS;
      }

  } else if (m_param.m_NameOfTheSource == "BYPYTH") { 
    ATH_MSG_WARNING(  "********************************************************" );
    ATH_MSG_WARNING(  "* Magnetic Field information from CUSTOM configuration.*" );
    ATH_MSG_WARNING(  "********************************************************" );
    ATH_MSG_WARNING(  " NEW Param from CUSTOM configuration.                   " );
    semiAnSol = 0;
    std::ostringstream outperso;
    m_param.printParam(outperso);
    ATH_MSG_INFO(  outperso.str() );
    m_coolupdated=true;
    return initMagField();
  } else if (m_param.m_NameOfTheSource == "COOL") {
    par_setupcool=true;
    ATH_MSG_INFO(  "Magnetic Field information from COOL" );
    return StatusCode::SUCCESS;
  } else if (m_param.m_NameOfTheSource == "COOL_HLT") {
    par_setupcool=true;
    ATH_MSG_INFO(  "Magnetic Field information from COOL and/or HLT (for online running)" );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_FATAL(  "Selected source " << m_param.m_NameOfTheSource  << " unknown ");
    ATH_MSG_INFO(  "InfosChain      " << m_param.m_InfosChain  );
    return StatusCode::FAILURE;
  }
}

StatusCode MagFieldAthenaSvc::initGeoModel(IOVSVC_CALLBACK_ARGS)
{
  
  ATH_MSG_INFO(  "in initGeoModel()" );
  StatusCode Sc;
  //
  //*Get correct information from the Geometry DB
  getTagOracle (Sc);
  //*Proceed with the rest of MagField initialization
  m_coolupdated=true;
  return initMagField();
}
  

StatusCode MagFieldAthenaSvc::initMagField(){


  //
  //*Do the actual MagField initialization here

  //  log << MSG::INFO << "in initMagField()" << endreq;
  //
  //   Hack to get bz2 or unzip file   //
  //   trick to test the hack...      //
  //   fileName = "/afs/cern.ch/user/c/chevalie/afsmag/Map/AtlasFull/bmagatlas_09_fullAsym20400.data.bz2";
  std::string fileName    = PathResolver::find_file(m_param.m_NameOfTheFile,"DATAPATH");
  std::string fileNameSol = PathResolver::find_file(m_param.m_SolenoidField,"DATAPATH");
  std::string fileNameUnzip;
  ATH_MSG_INFO(  "Selected source  : " << m_param.m_NameOfTheSource  );
  ATH_MSG_INFO(  "Input Bfield file: " << m_param.m_NameOfTheFile    );
  if(fileName.empty()) {
    fileNameUnzip = m_param.m_NameOfTheFile.substr( 0,  m_param.m_NameOfTheFile.size() - 4 );
    ATH_MSG_INFO(  "fileName empty, build fileNameUnzip: " << fileNameUnzip );
    fileNameUnzip = PathResolver::find_file(fileNameUnzip,"DATAPATH");
    ATH_MSG_INFO(  "fileNameUnzip:                       " << fileNameUnzip );
  }
  
  if (fileName.find(".bz2")!=std::string::npos) {
    if(!fileNameUnzip.empty()) {
      fileName = fileNameUnzip;
      ATH_MSG_INFO(  "MagField file unzipped found: " << fileName  );
    } else {
      ATH_MSG_INFO(  "MagField file zipped found: " << fileName  );
    }
  } else if(!fileNameUnzip.empty()) {
    fileName = fileNameUnzip;
    ATH_MSG_INFO(  "NO fileName zipped!! Only UnZipped file New Map: " << fileName  );
  } else if(!fileName.empty()) {
    ATH_MSG_INFO(  "NO fileName zipped!! Only UnZipped file Old Map: " << fileName  );
  } else if(m_param.m_NoField == "NOFIELD") {
    ATH_MSG_INFO(  "NO Magnetic Field COOL!!! " << m_param.m_NoField  );
    ATH_MSG_INFO(  "NO Magnetic Field current Toroid   " << par_onlineTorCur );
    ATH_MSG_INFO(  "NO Magnetic Field current Solenoid " << par_onlineSolCur );
  } else {
    ATH_MSG_FATAL(  "Cannot locate MagField " << fileName << " from ${DATAPATH}"  );
    return StatusCode::FAILURE;
  }
  //   Hack end   //
  ATH_MSG_INFO(  "fileName:          " << fileName );
  ATH_MSG_INFO(  "fileNameSol:       " << fileNameSol );
  ATH_MSG_INFO(  "fileNameUnzip:     " << fileNameUnzip );
  //*Get correct information from the Geometry DB
  ATH_CHECK( m_geoModel.retrieve() );
  if(m_param.m_NameOfTheFile=="bmagatlas05_test2.data"){
    ATH_MSG_INFO(  "Old map get InfosBmag " );
    StatusCode Sc;
    getTagOracle2 (Sc);
    Sc.ignore();
  }
  //
  if(m_param.m_NoField == "NOFIELD"){
    ATH_MSG_INFO(  "No Magnetic Field Oracle Tag :  "<< m_param.m_NoField );
    ATH_MSG_INFO(  "*******************************" );
    ATH_MSG_INFO(  "*    No Magnetic Field       * " );
    ATH_MSG_INFO(  "*    No Magnetic Field       * " );
    ATH_MSG_INFO(  "*    No Magnetic Field       * " );
    ATH_MSG_INFO(  "*    No Magnetic Field       * " );
    ATH_MSG_INFO(  "*    No Magnetic Field       * " );
    ATH_MSG_INFO(  "*******************************" );
  }else{
    if (fileName == ""){
      ATH_MSG_FATAL(  "Cannot locate " << m_param.m_NameOfTheFile << " from ${DATAPATH}"  );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO(  "Global Shifts and Tilts for Magnetic Field:" );
      int sommeParTilt;
      sommeParTilt = m_param.m_dx +  m_param.m_dy + m_param.m_dz + m_param.m_dtheta + m_param.m_dphi + m_param.m_dpsi;
      if(sommeParTilt == 0){ 
	ATH_MSG_INFO(  "No shift & No Tilts for the Global Mag Field Map             "<< sommeParTilt );
      } else { 
	ATH_MSG_INFO(  "Shift &/or Tilts for the Global Mag Field Map                "<< sommeParTilt );
	ATH_MSG_INFO(  "Dx "<< m_param.m_dx );
	ATH_MSG_INFO(  "Dy "<< m_param.m_dy );
	ATH_MSG_INFO(  "Dz "<< m_param.m_dz );
	ATH_MSG_INFO(  "Da "<< m_param.m_dtheta );
	ATH_MSG_INFO(  "Db "<< m_param.m_dphi);
	ATH_MSG_INFO(  "Dc "<< m_param.m_dpsi);
      }
    }
    if (fileNameSol == "" && semiAnSol != 0) {
      ATH_MSG_FATAL(  "Cannot locate " <<  m_param.m_SolenoidField << " from ${DATAPATH}" );
      return StatusCode::FAILURE;
    } else {
      m_param.m_SolenoidField = fileNameSol;
      ATH_MSG_INFO(  "Fitted Solenoid Map used: " << m_param.m_SolenoidField  );
      ATH_MSG_INFO(  "Shifts and Tilts for Solenoid Map:" );
      ATH_MSG_INFO(  "Dxsol "<< m_param.m_dxsol );
      ATH_MSG_INFO(  "Dysol "<< m_param.m_dysol );
      ATH_MSG_INFO(  "Dzsol "<< m_param.m_dzsol );
      ATH_MSG_INFO(  "Dasol "<< m_param.m_dthetasol );
      ATH_MSG_INFO(  "Dbsol "<< m_param.m_dphisol);
      ATH_MSG_INFO(  "Dcsol "<< m_param.m_dpsisol);
    }
    //* if Map already open
    if(m_param.m_NameOfTheFile == m_alreadyReadFile){
      ATH_MSG_INFO(  " Toroid map already open  "<< m_alreadyReadFile );
      alreadyReadFile = false;
    } else {
      alreadyReadFile = true;
      m_alreadyReadFile = m_param.m_NameOfTheFile;
      ATH_MSG_INFO(  " Toroid map first call    "<< m_alreadyReadFile );
    };  
  }
  if( m_useFieldService ){

    
    ATH_CHECK( m_fieldService.retrieve() );
    
    p_MagFieldAthena = new MagFieldSvcWrapper( *&*m_fieldService );

  } else {
    ATH_MSG_FATAL(  "Support for old field service is DISCONTINUED" );
    ATH_MSG_FATAL(  "Please switch to the new MagFieldServices or set UseIMagFieldSvc (now " << m_useFieldService << ") to true" );
    return StatusCode::FAILURE;
  }
//}else if(m_param.m_NoField == "NOFIELD") {
//  log << MSG::INFO << " m_param.m_NoField  "<< m_param.m_NoField  << endreq; 
//  p_MagFieldAthena = MagFieldFactory::instantiateMagneticField (fileName, &m_param);
//}else if(alreadyReadFile) {
//  //  m_param=par_param;
//  m_param.m_NoField = par_param.m_NoField;
//  m_param.calcul    = par_param.calcul;
//  log << MSG::INFO << " alreadyReadFile    "<< alreadyReadFile << endreq; 
//  log << MSG::INFO << " m_param.m_NoField  "<< m_param.m_NoField  << endreq; 
//  log << MSG::INFO << " m_param.calcul     "<< m_param.calcul  << endreq; 
//  p_MagFieldAthena = MagFieldFactory::instantiateMagneticField (fileName, &m_param);
//} 
//if (!p_MagFieldAthena) {
//  log << MSG::FATAL<< "DTB does not correspond to known Type ??? " << endreq;
//  return StatusCode::FAILURE;
//}
  p_MagFieldAthena->setAllToroidOn(false);
  p_MagFieldAthena->setFieldStatusOn(false);  
  if(p_MagFieldAthena->getToroidBarrelOn() &&
     p_MagFieldAthena->getToroidECTAOn()   &&
     p_MagFieldAthena->getToroidECTCOn() ){
     p_MagFieldAthena->setAllToroidOn(true);
  }
  if(p_MagFieldAthena->getAllToroidOn() &&  
     p_MagFieldAthena->getSolenoidOn() ){
     p_MagFieldAthena->setFieldStatusOn(true);  
  }    
//  std::ostringstream outperso;
//  m_param.printParam(outperso);
//  log << MSG::INFO << outperso.str() << endreq;
  ATH_MSG_INFO(  m_param.outFactory );
  //
  ATH_MSG_INFO(  "*** Magnetic Field Status *** " );
  ATH_MSG_INFO(  "*    ToroidBarrelOn()        * " <<  p_MagFieldAthena->getToroidBarrelOn() );
  ATH_MSG_INFO(  "*    ToroidECTAOn()          * " <<  p_MagFieldAthena->getToroidECTAOn()   );
  ATH_MSG_INFO(  "*    ToroidECTCOn()          * " <<  p_MagFieldAthena->getToroidECTCOn()   );
  ATH_MSG_INFO(  "*    AllToroidOn()           * " <<  p_MagFieldAthena->getAllToroidOn()    );
  ATH_MSG_INFO(  "*    SolenoidOn()            * " <<  p_MagFieldAthena->getSolenoidOn()     );
  ATH_MSG_INFO(  "*    FieldStatusOn()         * " <<  p_MagFieldAthena->getFieldStatusOn()  );
  ATH_MSG_INFO(  "*******************************" );
  //
  return StatusCode::SUCCESS;
}
 
//*Service finalisation
StatusCode MagFieldAthenaSvc::finalize()    {
  //
  ATH_MSG_INFO(  "in finalize()" );
  ATH_MSG_INFO(  "Retrieves through getMagFieldAthena() [init, event] ["
                 << m_getbad << "," << m_getgood << "]" );
  ATH_MSG_INFO(  "Retrieves through getUpdatedMagFieldAthena() [init, event] ["
                 << m_getcoolbad << "," << m_getcoolgood << "]" );
  //
  //*Reset p_MagFieldAthena
  delete p_MagFieldAthena ;
  p_MagFieldAthena = 0;
  return Service::finalize();
}

MagFieldAthena* MagFieldAthenaSvc::GetMagFieldAthena() {
  // count retrievals before and after the COOL setup has been done
  if (m_gotcooldata) {
    ++m_getgood;
  } else {
    ++m_getbad;
  }
  // if COOL is in use, throw exception as this method should not be called
  if (m_param.m_NameOfTheSource=="COOL") {
    ATH_MSG_FATAL(  "GetMagFieldAthena called when magnetic field is read from COOL - please use GetUpdatedMagFieldAthena instead" );
    throw GaudiException("GetMagFieldAthena called when magnetic field read from COOL","MagFieldAthenaSvc",StatusCode::FAILURE);
  }
  return p_MagFieldAthena;
}

MagFieldAthena* MagFieldAthenaSvc::GetUpdatedMagFieldAthena() {
  // return the pointer to the magnetic field retrieved after COOL callback
  // to be called in client callbacks attached to MagFieldAthenaSvc::updateField  // count how many times this is called before/after callback done
  if (m_gotcooldata) {
    ++m_getcoolgood;
    return p_MagFieldAthena;
  } else {
    ++m_getcoolbad;
    return 0;
  }
  ATH_MSG_INFO(  "GetUpdatedMagFieldAthena" );
}

bool MagFieldAthenaSvc::WasFieldUpdated() {
  return m_coolupdated;
}

// callback function called when COOL Bfield data (map, currents) changes
StatusCode MagFieldAthenaSvc::updateField(IOVSVC_CALLBACK_ARGS_P(/*I*/,
								 keys)) {
  // callback function when COOL field information changes
  ATH_MSG_INFO(  "COOL callback updateField called for " << 
                 keys.size() << " folders" );
  m_coolupdated=true;

  // first determine what has changed
  bool newmap=false;
  bool newcur=false;
  for (std::list<std::string>::const_iterator ktr=keys.begin();ktr!=keys.end();
       ++ktr) {
    if (*ktr==m_coolBFieldMap) newmap=true;
    if (*ktr==m_coolCurrents) newcur=true;
  }
  float solcur=0.;
  float torcur=0.;
  if (m_param.m_NameOfTheSource=="COOL_HLT") {
    // obtain currents from HLT parameters
    solcur=par_onlineSolCur;
    torcur=par_onlineTorCur;
  } else if (par_usedcs) {
    // if DCS callback enabled, get parameters from DCS
    ATH_MSG_DEBUG(  "Process magnetic field currents from COOL, size "
                    << h_coolCurrents->size() );
    bool gotsol=false;
    bool gottor=false;
    for (CondAttrListCollection::const_iterator itr=h_coolCurrents->begin();
	     itr!=h_coolCurrents->end();++itr) {
      const coral::AttributeList& cpar=itr->second;
      if (itr->first==1) {
        // channel 1 is solenoid current
        solcur=cpar["value"].data<float>();
	gotsol=true;
      } else if (itr->first==3) {
        // channel 3 is toroid current
        torcur=cpar["value"].data<float>();
	gottor=true;
      }
    }
    if (!gotsol || !gottor) {
      ATH_MSG_ERROR(  "Missing DCS field information: solenoid " <<
                      gotsol << " toroid " << gottor );
      return StatusCode::FAILURE;
    }
  } else {
    // fixed current parameters corresponding to hardwired values
    solcur=m_refSolCur;
    torcur=m_refTorCur;
  }
  ATH_MSG_INFO(  "Obtained currents - Solenoid: " << solcur << 
                 " amps, Toriod: " << torcur << " amps" );

  // create param object to retrieve configuration from COOL
  MagFieldParam param;
  // transfer the parameter type from the one currently in use
  param.m_NameOfTheSource=m_param.m_NameOfTheSource;

  // obtain the best configuration from COOL
  // (including currents from HLT if appropriate)
  ATH_CHECK( chooseParamFromCOOL(solcur,torcur,param) );

  // check if the change in currents is significant
  float delsol=param.m_SolCur-m_param.m_SolCur;
  float deltor=param.m_TorCur-m_param.m_TorCur;
  if (fabs(delsol)<par_currenttolerance && fabs(deltor)<par_currenttolerance) {
    // only print if map has not changed to avoid spurious warning
    if (!newmap) 
      ATH_MSG_INFO(   "Difference between previous and new currents too small (solenoid "
                      << delsol << ", toriod " << deltor << ")" );
    newcur=false;
  }

  // decide whether to use the new parameters and (re)init the field
  if (param.m_NameOfTheSource=="COOL" || 
       param.m_NameOfTheSource=="COOL_HLT") {
    if (newmap || newcur) {
      // initialise field with new parameters
      m_param=param;
      if (m_param.m_SolenoidField!="") {
	semiAnSol=99999;
      } else {
	semiAnSol=0;
      }
      ATH_CHECK( initMagField() );
      m_gotcooldata=true;
      ATH_MSG_INFO(  "Initialised field from " <<  param.m_NameOfTheSource );
    } else {
      // no change of parameters
      ATH_MSG_INFO( 
                   "Field update skipped as no significant difference in parameters" );
      m_coolupdated=false;
    }
  } else {
    // not using COOL or COOL_HLT - just compare the COOL parameters with
    // what is already in use - check for consistency
    // MagFieldParams does not define an equality operator, just check the
    // most important paramters explicitly
    if (!checkTol(m_param.m_dx,param.m_dx) || 
	!checkTol(m_param.m_dy,param.m_dy) ||
	!checkTol(m_param.m_dz,param.m_dz) ||
	!checkTol(m_param.m_dpsi,param.m_dpsi) ||
	!checkTol(m_param.m_dphi,param.m_dphi) ||
	!checkTol(m_param.m_dtheta,param.m_dtheta) ||
      	((!checkTol(m_param.m_dxsol,param.m_dxsol) ||
      	  !checkTol(m_param.m_dysol,param.m_dysol) ||
      	  !checkTol(m_param.m_dzsol,param.m_dzsol) ||
      	  !checkTol(m_param.m_dpsisol,param.m_dpsisol) ||
      	  !checkTol(m_param.m_dphisol,param.m_dphisol) ||
      	  !checkTol(m_param.m_dthetasol,param.m_dthetasol)) &&
	 m_param.m_SolenoidField!="" && param.m_SolenoidField!="") ||
	m_param.m_NoField!=param.m_NoField ||
	(m_param.m_NoField=="" &&
         (m_param.m_NameOfTheFile!=param.m_NameOfTheFile ||
          m_param.m_SolenoidField!=param.m_SolenoidField ||
        (m_param.noTiltNoShift!=param.noTiltNoShift && 
	 m_param.noTiltNoShift!=9999) ||
	  m_param.doScale!=param.doScale))) {
      ATH_MSG_WARNING(  "Oracle and COOL parameters DISAGREE!"  );
      // dump out the parameters to see which is different
      std::ostringstream outperso;
      ATH_MSG_INFO(  "Parameters read from COOL" );
      m_param.printParam(outperso);
      ATH_MSG_INFO(  outperso.str() );
      ATH_MSG_INFO(  "Field filenames: " << param.m_NameOfTheFile << " : "
                     << param.m_SolenoidField );
      ATH_MSG_INFO(  "doScale " << param.doScale );
      ATH_MSG_INFO(  "NoField: " << param.m_NoField );
      ATH_MSG_INFO(  "CF parameters from Oracle" );
    } else {
      ATH_MSG_INFO(  "Oracle and COOL parameters agree" );
    }
    m_gotcooldata=true;
  }
  return StatusCode::SUCCESS;
}


bool MagFieldAthenaSvc::checkTol(const double xo,const double xc) {
  // check values from Oracle and COOL consistent (or Oracle=9999. and COOL=0)
  double tol=1.E-3;
  if (fabs(xo-xc)<tol) return true;
  if (fabs(xo-9999.)<tol && fabs(xc)<tol) return true;
  return false;
}

std::string MagFieldAthenaSvc::decodePath(const std::string& path) {
  // decode a path name stored in the COOL /GLOBAL/BField/Map folder into 
  // something ready for PathResolver
  // only plain file: prefix for now, foresee to add GUID lookup later
  if (path.substr(0,5)=="file:") {
    return path.substr(5);
  } else {
    // if not recognised, return empty string
    return "";
  }
}

StatusCode MagFieldAthenaSvc::chooseParamFromCOOL(const float reqSolCur,
				  const float reqTorCur,
				  MagFieldParam& param) {
  // choose the best parameters from COOL /GLOBAL/BField/Map folder
  // given the input currents (which come from COOL or params in HLT case)

  // set currents to zero if below tolerance
  float rSolCur,rTorCur;
  bool solOff=false;
  bool torOff=false;
  if (fabs(reqSolCur)>par_zerotolerance) {
    rSolCur=reqSolCur;
  } else {
    ATH_MSG_INFO(  "Solenoid current only " << reqSolCur 
                   << " assume zero" );
    rSolCur=0.;
    solOff=true;
  }
  if (fabs(reqTorCur)>par_zerotolerance) {
    rTorCur=reqTorCur;
  } else {
    ATH_MSG_INFO(  "Toroid current only " << reqTorCur  
                   << " assume zero" );
    rTorCur=0.;
    torOff=true;
  }
  ATH_MSG_DEBUG(  "Magnet settings: solenoid ON? " << !solOff << 
                  " toroid ON? " << !torOff );

  // loop over all the available B-field configurations to choose the one
  // closest to the actual currents being used
  // step through from 1 in units of 2 (global+solenoid field)
  CondAttrListCollection::const_iterator itr1,itr2;
  // first find the number of field slots defined
  unsigned int nfield=0;
  itr2=h_coolBFieldMap->end();
  for (itr1=h_coolBFieldMap->begin();itr1!=itr2;++itr1)
    if ((itr1->first)>nfield) nfield=itr1->first;
  unsigned int ibest=0;
  float bestdist=1.E9;
  // skip the check if only 1 configuration (nfield=1/2) available
  if (nfield>0 && nfield<=2) {
    ibest=1;
    bestdist=0.;
    ATH_MSG_DEBUG(  "Only " << nfield << 
                    " sets of configuration data available - take dataslot 1" );
  } else {
    for (unsigned int ifield=1;ifield<=nfield;ifield+=2) {
      itr1=h_coolBFieldMap->chanAttrListPair(ifield);
      if (itr1!=h_coolBFieldMap->end()) {
        const coral::AttributeList& bp1=itr1->second;
        float torCur=bp1["Current"].data<float>();
        float solCur=0.;
        itr2=h_coolBFieldMap->chanAttrListPair(ifield+1);
        if (itr2!=h_coolBFieldMap->end()) {
          const coral::AttributeList& bp2=itr2->second;
          solCur=bp2["Current"].data<float>();
	}
	// only consider configurations which have right magnets on/off
	if (!((solOff && fabs(solCur)>par_zerotolerance) ||
	      (torOff && fabs(torCur)>par_zerotolerance)) &&
	    !((!solOff && fabs(solCur)<par_zerotolerance) ||
	      (!torOff && fabs(torCur)<par_zerotolerance))) {
	  // evaluate this one based on normalised difference from req currents
          float dist=fabsf(torCur-rTorCur)/m_refTorCur+
	             fabsf(solCur-rSolCur)/m_refSolCur;
          ATH_MSG_DEBUG(  "Field choice index " << ifield << 
                          " has score " << dist );
          if (dist<bestdist) {
           // new best fieldmap match
           bestdist=dist;
           ibest=ifield;
	  }
	} else {
	  ATH_MSG_DEBUG(  "Field choice index " << ifield << 
                          " has wrong magnet on/off configuration" );
	}
      }
    }
  }
  // check something acceptable was found
  if (ibest==0) {
    ATH_MSG_ERROR(  "Did not find any " << m_coolBFieldMap << 
                    " parameters - cannot load field" );
    return StatusCode::FAILURE;
  }
  if (bestdist>par_maptolerance) {
    ATH_MSG_WARNING(  "No available field map has close enough currents ("
                      << bestdist << "," << par_maptolerance << ")" );
    if (par_crashmap) {
      ATH_MSG_FATAL(  "CrashMap enabled - will not setup field" );
      return StatusCode::FAILURE;
    }
  }

  // now have chosen the best field configuration, set it up in params obj
  itr1=h_coolBFieldMap->chanAttrListPair(ibest);
  const coral::AttributeList& bfieldpar=itr1->second;
  const std::string& ftype=bfieldpar["FieldType"].data<std::string>();
  ATH_MSG_INFO(  "Chosen BFieldMap dataslot 1 of type: " << ftype << 
                 " from index " << ibest );
  bool noTiltNoShift=true;
  float tol=1.E-4; // tolerance on tilt/shift to be considered non-zero
  if (ftype=="NOFIELD" || ftype=="OptNOFIELD") {
    param.m_NoField="NOFIELD";
    param.m_NameOfTheFile="";
    param.m_SolenoidField="";
  } else if (ftype=="GlobalMap" || ftype=="OptGlobalMap") {
    param.m_dx=bfieldpar["dx"].data<float>();
    param.m_dy=bfieldpar["dy"].data<float>();
    param.m_dz=bfieldpar["dz"].data<float>();
    param.m_dphi=bfieldpar["dphi"].data<float>();
    param.m_dtheta=bfieldpar["dtheta"].data<float>();
    param.m_dpsi=bfieldpar["dpsi"].data<float>();
    if (fabsf(param.m_dx)>tol || fabsf(param.m_dy)>tol ||
		fabsf(param.m_dz)>tol || fabsf(param.m_dphi)>tol ||
                fabsf(param.m_dtheta)>tol || fabsf(param.m_dpsi)>tol)
      noTiltNoShift=false;
    param.m_NameOfTheFile=
      decodePath(bfieldpar["FieldFileName"].data<std::string>());
    param.m_TorCurNom=bfieldpar["Current"].data<float>();
    param.m_TorCur=rTorCur;
    ATH_MSG_INFO(  "param.m_TorCurNom from COOL:  " << param.m_TorCurNom );
    ATH_MSG_INFO(  "param.m_TorCur    from COOL:  " << param.m_TorCur );
    //  Add info and semiAnSol = 0 to be coherent with ORACLE
    semiAnSol = 0;
    // check the InfosBMap exists
    int infosbmag=0;
    try {
      infosbmag=bfieldpar["BMAGINFO"].data<int>();
      ATH_MSG_INFO(  "Found InfosBMag from COOL of " << infosbmag );
    }
    catch (coral::AttributeListException& e) {
      ATH_MSG_INFO(  "No InfosBMag available, assume -1" );
      infosbmag=-1;
    }
  } else {
    ATH_MSG_ERROR(  "Unrecognised field type in position 1: " << ftype );
    return StatusCode::FAILURE;
  }
  // now deal with a solenoid field map, if present
  itr2=h_coolBFieldMap->chanAttrListPair(ibest+1);
  if (itr2!=h_coolBFieldMap->end()) {
    const coral::AttributeList& bfieldpar2=itr2->second;
    const std::string& ftype2=bfieldpar2["FieldType"].data<std::string>();
    ATH_MSG_INFO(  "Chosen BFieldMap dataslot 2 of type: " << ftype2 << 
                   " from index " << ibest+1 );
    if (ftype2=="SolenoidMap" || ftype2=="OptSolenoidMap") {
      param.m_dxsol=bfieldpar2["dx"].data<float>();
      param.m_dysol=bfieldpar2["dy"].data<float>();
      param.m_dzsol=bfieldpar2["dz"].data<float>();
      param.m_dphisol=bfieldpar2["dphi"].data<float>();
      param.m_dthetasol=bfieldpar2["dtheta"].data<float>();
      param.m_dpsisol=bfieldpar2["dpsi"].data<float>();
      if (fabsf(param.m_dxsol)>tol || fabsf(param.m_dysol)>tol ||
        fabsf(param.m_dzsol)>tol || fabsf(param.m_dphisol)>tol ||
        fabsf(param.m_dthetasol)>tol || fabsf(param.m_dpsisol)>tol)
        noTiltNoShift=false;
    // solenoid field is stored as resolved path (!)
      param.m_SolenoidField=PathResolver::find_file(decodePath(
        bfieldpar2["FieldFileName"].data<std::string>()),"DATAPATH");
      param.m_SolCurNom=bfieldpar2["Current"].data<float>();
      param.m_SolCur=rSolCur;
    } else {
      ATH_MSG_ERROR(  "Unrecognised field type in position 2: " << ftype2 );
      return StatusCode::FAILURE;
    }
  }

  // set the tilt/shift flag according to the parameters retrieved
  if (ftype!="NOFIELD" && ftype!="OptNOFIELD")
    param.noTiltNoShift=(noTiltNoShift ? 1 : 0);
  //
  //            !!!!  Temporary 29/10/10 start !!!!
  //   std::string str2 ("20400");
  // size_t found;
  // found=param.m_NameOfTheFile.find(str2);
  // if (found!=std::string::npos){
  //   log << MSG::INFO << "first '20400' found at: " << int(found) << endreq;
  //   param.m_TorCurNom = 20400.0;
  // }
  // log << MSG::INFO << "param.m_TorCurNon: " << param.m_TorCurNom << endreq;
  //            !!!!  Temporary 29/10/10 end   !!!!
  //
  // check if the field map needs to be scaled
  param.doScale=
    ( (param.m_SolCurNom!=0. && 
       (fabs(param.m_SolCur/param.m_SolCurNom-1.)>par_scaletolerance)) ||
      (param.m_TorCurNom!=0. && 
       (fabs(param.m_TorCur/param.m_TorCurNom-1.)>par_scaletolerance)) );
  if (param.doScale) {
    // avoid problem when only solenoid is on, and we scale toriod from a 
    // reference current of zero - in this case set cur/ref to 20500A to avoid
    // NaN in toriod scaling (instead becomes 20500/20500, avoiding scaling)
    if (torOff && !solOff && param.m_TorCurNom==0.) {
      param.m_TorCurNom=m_refTorCur;
      param.m_TorCur=m_refTorCur;
    }
    ATH_MSG_INFO(  "COOL Rescale magnetic fields: Solenoid current "
                   << param.m_SolCur << " amps vs " << param.m_SolCurNom );
    ATH_MSG_INFO(  "COOL Rescale magnetic fields: Toroid current " << param.m_TorCur << " amps vs "
                   << param.m_TorCurNom << " nominal" );
  }
  ATH_MSG_INFO(  "chooseParamFromCOOL" );
  return StatusCode::SUCCESS;
}

