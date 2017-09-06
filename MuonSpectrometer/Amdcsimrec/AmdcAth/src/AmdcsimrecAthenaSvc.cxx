/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

//----------------------------------------------------------------//
#include "StoreGate/DataHandle.h"

#include "PathResolver/PathResolver.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "EventInfo/TagInfo.h"

//----------------------------------------------------------------//
#include "EventInfoMgt/ITagInfoMgr.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

/////////////////////////////////////////////////////////
#include "AmdcAth/AmdcsimrecAthenaSvc.h"

#include "AmdcStand/AmdcsimrecStand.h"
 
#include "AmdcCore/cppbigdump.h"

//----------------------------------------------------------------//
#include "AmdcCore/AmdcAlineStore.h"
#include "AmdcCore/AmdcBlineStore.h"
#include "AmdcCore/AmdcIlineStore.h"

#include "AmdcStand/controlaline.h"
#include "AmdcStand/setablines.h"
#include "AmdcStand/fgetamdcagdd.h"
#include "AmdcStand/amdcread.h"
#include "AmdcStand/bigamdcdump.h"
#include "AmdcStand/loadamdcagddfromstring.h"

/// Standard Constructor
AmdcsimrecAthenaSvc::AmdcsimrecAthenaSvc(const std::string& name,ISvcLocator* svc):
  AthService(name,svc),
p_IGeoModelSvc(0),
p_detStore(0),
p_MuonDetectorManager(0),
p_ITagInfoMgr(0)
{

//*Set Default values
   p_AmdcAlineStoreInternal = new AmdcAlineStore;
   p_AmdcBlineStoreInternal = new AmdcBlineStore;
   p_AmdcIlineStoreInternal = new AmdcIlineStore ;

   p_AmdcAlineStoreExternal = new AmdcAlineStore;
   p_AmdcBlineStoreExternal = new AmdcBlineStore;
   p_AmdcIlineStoreExternal = new AmdcIlineStore ;

   m_LocationOfTheFile = "" ;

   m_AmdcString = "" ;
   m_AgddString = "" ;

   m_IsInitialized = false;
   m_IsUsable      = false;

   p_Amdcsimrec = 0;


   m_AmdcABlinesStamp = 1;
   
   m_TagInfoXMLStamp = -1 ;
   m_TagInfoXML.clear();
   
   m_AGDD2GeoSwitchesStamp = -1 ;
   m_AGDD2GeoSwitches.clear();
   
//*Declare the properties
   declareProperty("NameOfTheSource"        , m_NameOfTheSource        = "GEOMODEL" ) ;
   declareProperty("NameOfTheAmdbFile"      , m_NameOfTheFile          = ""         ) ;
   declareProperty("OracleNodedetectorKey"  , m_OracleNodedetectorKey  = ""         ) ;
   declareProperty("OracleNodedetectorNode" , m_OracleNodedetectorNode = ""         ) ;

   declareProperty("CtrlDumpCo"        , m_CtrlDumpCo         = 0          ) ;
   declareProperty("CtrlFileStrings"   , m_CtrlFileStrings    = 0          ) ;
   declareProperty("CtrlFileOracle"    , m_CtrlFileOracle     = 0          ) ;
   declareProperty("EmergencyOut"      , m_EmergencyOut       = 0          ) ;
   declareProperty("ControlALine"      , m_ControlALine       = 111111     ) ;
   declareProperty("ControlILine"      , m_ControlILine       = 111111     ) ;
   declareProperty("TruncateALine"     , m_TruncateALine      = 0          ) ;
   declareProperty("TruncatePLine"     , m_TruncatePLine      = 0          ) ;
   declareProperty("HardSoftCheck"     , m_HardSoftCheck      = 0          );
   declareProperty("XtomoCheck"        , m_XtomoCheck         = 0          );

//-----------------------------------------------------------------
// Alignment Corrections :
// AlignmentSource  2: Get them from Geomodel
//                  3: Get them from Amdc string from Ascii file or from Oracle
// AlignmentCorr    0 : off 
//                  1: A 
//                  2: B 
//                  3: A&B

   declareProperty( "AlignmentSource"                   , m_AlignmentSource                   = 3 ) ;
   declareProperty( "AlignmentCorr"                     , m_AlignmentCorr                     = 1 ) ;

   declareProperty( "SortAlignContainer"                , m_SortAlignContainer                = 0 );
   declareProperty( "RmAlignCSC"                        , m_RmAlignCSC                        = 0 );
   declareProperty( "ModifyALineContainer"              , m_ModifyALineContainer              = 2 );
   
   declareProperty( "CompareInternalExternalALineContainer" , m_CompareInternalExternalALineContainer      = 0 );
  
   declareProperty( "UseMuonDetectorManagerForInternal", m_UseMuonDetectorManagerForInternal              = 0 );
   
   declareProperty( "DontSetAmdcABlineFromCool", m_DontSetAmdcABlineFromCool              = 0 );
   
   declareProperty( "PrintLevel", m_PrintLevel = 0 );
   
   p_AtlasDetectorID = 0 ;
   
}
 
/// Standard Destructor
AmdcsimrecAthenaSvc::~AmdcsimrecAthenaSvc()  {
  delete p_AmdcAlineStoreInternal ;
  delete p_AmdcBlineStoreInternal ;
  delete p_AmdcIlineStoreInternal ;
  delete p_AmdcAlineStoreExternal ;
  delete p_AmdcBlineStoreExternal ;
  delete p_AmdcIlineStoreExternal ;
  delete p_Amdcsimrec ;
}
 
// Service initialisation
StatusCode AmdcsimrecAthenaSvc::initialize() {

  ATH_MSG_DEBUG( "Initialisation started     " ) ;

  StatusCode sc=AthService::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Service::initialize() failed" ) ;
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "================================" ) ;
  ATH_MSG_DEBUG( "=Proprieties are         " ) ;
  ATH_MSG_DEBUG( "= NameOfTheSource        " << m_NameOfTheSource      ) ;
  ATH_MSG_DEBUG( "= NameOfTheAmdbFile      " << m_NameOfTheFile        ) ;
  ATH_MSG_DEBUG( "= OracleNodedetectorKey  " << m_OracleNodedetectorKey  ) ;
  ATH_MSG_DEBUG( "= OracleNodedetectorNode " << m_OracleNodedetectorNode ) ;
  ATH_MSG_DEBUG( "= CtrlDumpCo             " << m_CtrlDumpCo           ) ;
  ATH_MSG_DEBUG( "= CtrlFileStrings        " << m_CtrlFileStrings      ) ;
  ATH_MSG_DEBUG( "= CtrlFileOracle         " << m_CtrlFileOracle     ) ;
  ATH_MSG_DEBUG( "= EmergencyOut           " << m_EmergencyOut         ) ;
  ATH_MSG_DEBUG( "= ControlALine           " << m_ControlALine         ) ;
  ATH_MSG_DEBUG( "= ControlILine           " << m_ControlILine         ) ;
  ATH_MSG_DEBUG( "= TruncateALine          " << m_TruncateALine         ) ;
  ATH_MSG_DEBUG( "= TruncatePLine          " << m_TruncatePLine         ) ;
  ATH_MSG_DEBUG( "= AlignmentSource        " << m_AlignmentSource ) ;
  ATH_MSG_DEBUG( "= AlignmentCorr          " << m_AlignmentCorr ) ;
  ATH_MSG_DEBUG( "= HardSoftCheck          " << m_HardSoftCheck         ) ;
  ATH_MSG_DEBUG( "= XtomoCheck             " << m_XtomoCheck         ) ;
  ATH_MSG_DEBUG( "= SortAlignContainer                    " << m_SortAlignContainer         ) ;
  ATH_MSG_DEBUG( "= RmAlignCSC                            " << m_RmAlignCSC                 ) ;
  ATH_MSG_DEBUG( "= ModifyALineContainer                  " << m_ModifyALineContainer                 ) ;
  ATH_MSG_DEBUG( "= CompareInternalExternalALineContainer " << m_CompareInternalExternalALineContainer                 ) ;
  ATH_MSG_DEBUG( "= UseMuonDetectorManagerForInternal     " << m_UseMuonDetectorManagerForInternal                 ) ;
  ATH_MSG_DEBUG( "= DontSetAmdcABlineFromCool             " << m_DontSetAmdcABlineFromCool                 ) ;
  ATH_MSG_DEBUG( "================================" ) ;


//Set pointer on DetectorStore 
  sc = service("DetectorStore",p_detStore);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DetectorStore service not found !" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Found DetectorStore ") ;

//Set pointer on AtlasDetectorID
/*  sc = p_detStore->retrieve(p_AtlasDetectorID, "AtlasID" );
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get AtlasDetectorID ") ;
    return( StatusCode::FAILURE );
  }
  ATH_MSG_DEBUG( "Found AtlasDetectorID ") ;
*/
//Set pointer on GeoModelSvc
  if ( m_AlignmentSource == 2 
  || m_NameOfTheSource=="POOL" 
  || m_NameOfTheSource=="GEOMODEL" ) {
    sc = service ("GeoModelSvc",p_IGeoModelSvc);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "GeoModelSvc service not found !" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found GeoModelSvc ") ;
  }

//Set pointer on Muondetector Manager
  if ( m_UseMuonDetectorManagerForInternal == 1 ) {
    StatusCode sc = p_detStore->retrieve(p_MuonDetectorManager);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "p_MuonDetectorManager not found !" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found MuonDetectorManager ") ;
  }


//TagInfo 
  const DataHandle<TagInfo> tagInfoH;
  std::string tagInfoKey = "";
 
  sc = service("TagInfoMgr", p_ITagInfoMgr);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( " Unable to locate TagInfoMgr service" ) ; 
  }else{
    tagInfoKey = p_ITagInfoMgr->tagInfoKey();
  }

  sc = p_detStore->regFcn(&AmdcsimrecAthenaSvc::SetTagInfoXML,this, tagInfoH, tagInfoKey) ;
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( "Cannot register AmdcsimrecAthenaSvc::SetTagInfoXML function for key "  << tagInfoKey ) ;
  }else{
    ATH_MSG_DEBUG( "Registered AmdcsimrecAthenaSvc::SetTagInfoXML callback for key: " << tagInfoKey ) ;
  }


//Check geometry related proprieties
  if (  m_NameOfTheSource!="ASCII"
     && m_NameOfTheSource!="GEOMODEL" 
     && m_NameOfTheSource!="ORACLENODE" 
     && m_NameOfTheSource!="POOL" 
     && m_NameOfTheSource!="POOLHARD") {
    ATH_MSG_FATAL( "Selected source " << m_NameOfTheSource << " unknown " ) ;
    return StatusCode::FAILURE;
  }
  if ( m_AlignmentCorr != 0 && m_AlignmentCorr != 1 && m_AlignmentCorr != 2 && m_AlignmentCorr != 3 ) {
    ATH_MSG_FATAL( " AlignmentCorr is " << m_AlignmentCorr ) ;
    ATH_MSG_FATAL( "  while it should be 0, 1, 2 or 3 " ) ;
    return StatusCode::FAILURE;
  }
  if ( m_AlignmentCorr != 0 ) {
    if ( m_AlignmentSource != 2 && m_AlignmentSource != 3){
      ATH_MSG_FATAL( " AlignmentSource is " << m_AlignmentSource ) ;
      ATH_MSG_FATAL( "  while it should be 2: Get them from Geomodel ") ;
      ATH_MSG_FATAL( "  or                 3: Get them from Amdc string from Ascii file or from Oracle ") ;
      return StatusCode::FAILURE;
    }
  }


//Strings come Ascii file 
  if (m_NameOfTheSource=="ASCII"){

    if (m_AlignmentSource == 3 ){
      ATH_MSG_DEBUG( "=>Strings come from Ascii file and A/B line stores as well<=" ) ;
      m_IsInitialized = true ;
      m_IsUsable      = true ;
    }else{
      ATH_MSG_DEBUG( "=>Strings come from Ascii file and A/B line stores from cool<=" ) ;
      m_IsInitialized = false ;
      m_IsUsable      = false ;
    }

    sc=initializeAscii();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("initializeAscii failed" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: initializeAscii " ) ;
    
    if (m_AlignmentSource == 2 ){
      sc=regFcnSetAmdcABlineFromCool();
      if ( sc.isFailure() ) {
        ATH_MSG_FATAL("regFcnSetAmdcABlineFromCool failed" ) ;
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "Done: regFcnSetAmdcABlineFromCool " ) ;
    }

  }

//Strings come from Geomodel and A/B line stores as well
  if ( (m_NameOfTheSource=="POOL" || m_NameOfTheSource=="GEOMODEL" ) && m_AlignmentSource == 3 ){
    ATH_MSG_DEBUG( "=>Strings come from Geomodel and A/B line stores as well<=" ) ;
    
    m_IsInitialized = true ;
    m_IsUsable      = true ;
    
    sc = initializeFromGeomodel();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("initializeFromGeomodel failed" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: initializeFromGeomodel " ) ;
  }
  
//Strings come from Geomodel and A/B line stores from cool
   if ( (m_NameOfTheSource=="POOL" || m_NameOfTheSource=="GEOMODEL" ) && m_AlignmentSource == 2 ){
    ATH_MSG_DEBUG( "=>Strings come from Geomodel and A/B line stores from cool<=" ) ;
    m_IsInitialized = false ;
    m_IsUsable      = false ;

    ATH_MSG_DEBUG( "      p_IGeoModelSvc->geoInitialized() true "  ) ;

    sc = initializeFromGeomodel();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("initializeFromGeomodel failed" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: initializeFromGeomodel " ) ;
    
    sc=regFcnSetAmdcABlineFromCool();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("regFcnSetAmdcABlineFromCool failed" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: regFcnSetAmdcABlineFromCool " ) ;

  }
  

//Strings come from Oracle 
  if (m_NameOfTheSource=="POOLHARD" || m_NameOfTheSource=="ORACLENODE" ){

    if (m_AlignmentSource == 3 ){
      ATH_MSG_DEBUG( "=>Strings come from Oracle and A/B line stores as well<=" ) ;
      m_IsInitialized = true ;
      m_IsUsable      = true ;
    }else{
      ATH_MSG_DEBUG( "=>Strings come from Oracle and A/B line stores from cool<=" ) ;
      m_IsInitialized = false ;
      m_IsUsable      = false ;
    }

    sc=initializeFromOracleNode();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("initializeFromOracleNode failed" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: initializeFromOracleNode " ) ;
    
    if (m_AlignmentSource == 2 ){
      sc=regFcnSetAmdcABlineFromCool();
      if ( sc.isFailure() ) {
        ATH_MSG_FATAL("regFcnSetAmdcABlineFromCool failed" ) ;
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "Done: regFcnSetAmdcABlineFromCool " ) ;
    }

  }

//Set pointer on Muondetector Manager
  if ( m_UseMuonDetectorManagerForInternal == 1 ) {
    ATH_MSG_DEBUG( "=>A/B line in internal stores come from  MuonDetectorManager<=" ) ;
  }

  ATH_MSG_DEBUG( "Initialisation ended     " ) ;
  if ( m_EmergencyOut == 1 ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;

}
 
/// Service finalisation
StatusCode AmdcsimrecAthenaSvc::finalize(){return StatusCode::SUCCESS;}
 
StatusCode AmdcsimrecAthenaSvc::queryInterface( const InterfaceID& riid, void** ppvInterface ) {
  if ( IID_IAmdcsimrecAthenaSvc == riid )    {
    *ppvInterface = (AmdcsimrecAthenaSvc*)this;
  }else{
    return Service::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

bool AmdcsimrecAthenaSvc::UsableSvc()      {return m_IsUsable     ;}
bool AmdcsimrecAthenaSvc::InitializedSvc() {return m_IsInitialized;}
StatusCode AmdcsimrecAthenaSvc::UpdatedSvc(IOVSVC_CALLBACK_ARGS)
{
 ATH_MSG_DEBUG("----> UpdatedSvc is called" ) ; 
 return StatusCode::SUCCESS;
}
 
Amdcsimrec* AmdcsimrecAthenaSvc::GetAmdcsimrec(){return p_Amdcsimrec;}

std::string AmdcsimrecAthenaSvc::GetNameOfTheSource(){return m_NameOfTheSource;}

std::string AmdcsimrecAthenaSvc::GetNameOfTheFile(){return m_NameOfTheFile;}
std::string AmdcsimrecAthenaSvc::GetLocationOfTheFile(){return m_LocationOfTheFile;}

std::string AmdcsimrecAthenaSvc::GetAmdcString(){return m_AmdcString;}
std::string AmdcsimrecAthenaSvc::GetAgddString(){return m_AgddString;}

/// Service initialisation
StatusCode AmdcsimrecAthenaSvc::initializeAscii()
{

  ATH_MSG_DEBUG("----> initializeAscii is called" ) ; 

  StatusCode sc = SetLocation(m_NameOfTheFile,m_LocationOfTheFile);
  if (sc.isFailure()){
    ATH_MSG_FATAL( "SetLocation for " << m_NameOfTheFile << " failed " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: SetLocation " ) ;

  ATH_MSG_DEBUG( "File to be read " << m_NameOfTheFile ) ;
  ATH_MSG_DEBUG( " found as       " << m_LocationOfTheFile ) ;

//Initialize Muon Spectrometer Geometry
  int  SizeName = m_LocationOfTheFile.size();
  char* FileName = new char[SizeName];
  int  Istate = 0;
  for (int i= 0; i <SizeName; i++){FileName[i]=m_LocationOfTheFile[i];}
  int IFLAG = 100 + m_PrintLevel ;
  amdcreadnn_(FileName,SizeName,Istate,IFLAG);
  delete [] FileName ;
  if (Istate == 0) {
    ATH_MSG_FATAL( "amdcreadn failed " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: amdcreadn " ) ;

//Post Geometry Loading Sequence
  sc=PostGeometryLoadingSequence();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("PostGeometryLoadingSequence failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: PostGeometryLoadingSequence " ) ;

  return StatusCode::SUCCESS;

}

StatusCode AmdcsimrecAthenaSvc::regFcninitializeFromGeomodel() 
{

  ATH_MSG_DEBUG("----> regFcninitializeFromGeomodel is called" ) ; 

  StatusCode sc = p_detStore->regFcn(
                          &IGeoModelSvc::align, &*p_IGeoModelSvc,
                          &AmdcsimrecAthenaSvc::initializeFromGeomodelCallback, this
                         );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcsimrecAthenaSvc::initializeFromGeomodelCallback from IGeoModelSvc::align" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: Register callback on AmdcsimrecAthenaSvc::initializeFromGeomodelCallback from IGeoModelSvc::align" ) ;

  sc = p_detStore->regFcn(
               &AmdcsimrecAthenaSvc::initializeFromGeomodelCallback, this,
               &AmdcsimrecAthenaSvc::UpdatedSvc, this
              );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcsimrecAthenaSvc::UpdatedSvc from AmdcsimrecAthenaSvc::initializeFromGeomodelCallback" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: Register callback on AmdcsimrecAthenaSvc::UpdatedSvc from AmdcsimrecAthenaSvc::initializeFromGeomodelCallback" ) ;

  return StatusCode::SUCCESS;
  
}

StatusCode AmdcsimrecAthenaSvc::initializeFromGeomodelCallback(IOVSVC_CALLBACK_ARGS) 
{

  ATH_MSG_DEBUG("----> initializeFromGeomodelCallback is called" ) ; 
  
  StatusCode sc = initializeFromGeomodel();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("initializeFromGeomodel failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: initializeFromGeomodel" ) ;

  m_IsUsable      = true ;

  return StatusCode::SUCCESS;
}

StatusCode AmdcsimrecAthenaSvc::initializeFromGeomodel()
{

  ATH_MSG_DEBUG("----> initializeFromGeomodel is called" ) ; 

  std::string AtlasVersion = p_IGeoModelSvc->atlasVersion();
  std::string MuonVersion  = p_IGeoModelSvc->muonVersionOverride();

  m_detectorKey  = MuonVersion.empty() ? AtlasVersion : MuonVersion;
  m_detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";
  if ( MuonVersion == "CUSTOM"){
    m_detectorKey  = AtlasVersion ;
    m_detectorNode = "ATLAS"  ;
  } 
  StatusCode sc = initializeFromOracle() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "initializeFromOracle failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: initializeFromOracle" ) ;

  return StatusCode::SUCCESS;

}

StatusCode AmdcsimrecAthenaSvc::initializeFromOracleNode() {

  ATH_MSG_DEBUG("----> initializeFromOracleNode is called" ) ; 

  m_detectorKey  = m_OracleNodedetectorKey  ;
  m_detectorNode = m_OracleNodedetectorNode ;
  
  StatusCode sc = initializeFromOracle() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "initializeFromOracle failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: initializeFromOracle" ) ;

  return StatusCode::SUCCESS;

}

StatusCode AmdcsimrecAthenaSvc::initializeFromOracle()
{

  ATH_MSG_DEBUG("----> initializeFromOracle is called" ) ; 

  IRDBAccessSvc* pIRDBAccessSvc;
  StatusCode sc = service("RDBAccessSvc",pIRDBAccessSvc);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Unable to get RDBAccessSvc." ) ;
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "      Keys are  (key) "  << m_detectorKey  
      << " (node) " << m_detectorNode 
      ) ;

  IRDBRecordset_ptr recordsetAMDC = pIRDBAccessSvc->getRecordsetPtr("AMDC",m_detectorKey,m_detectorNode);
  if (recordsetAMDC->size()==0){
    ATH_MSG_FATAL( "recordsetAMDC->size() is 0" ) ;
    return StatusCode::FAILURE;
  }
  const IRDBRecord *recordAMDC =  (*recordsetAMDC)[0];
  std::string AmdcString = recordAMDC->getString("DATA");
  ATH_MSG_DEBUG( "        AMDC::VNAME " << recordAMDC->getString("VNAME") ) ;

  IRDBRecordset_ptr recordsetAGDD = pIRDBAccessSvc->getRecordsetPtr("AGDD",m_detectorKey,m_detectorNode);
  if (recordsetAGDD->size()==0){
    ATH_MSG_FATAL( "        recordsetAGDD->size() is 0" ) ;
    return StatusCode::FAILURE;
  }
  const IRDBRecord *recordAGDD =  (*recordsetAGDD)[0];
  std::string AgddString = recordAGDD->getString("DATA");
  ATH_MSG_DEBUG( "        AGDD::VNAME " << recordAGDD->getString("VNAME") ) ;


//Retrieve AGDD2GeoSwitches data
  std::string agdd2geoVersion = pIRDBAccessSvc->getChildTag("AGDD2GeoSwitches",m_detectorKey,m_detectorNode);
  m_AGDD2GeoSwitches.clear();
  if(!agdd2geoVersion.empty()) {
    m_AGDD2GeoSwitchesStamp = m_AGDD2GeoSwitchesStamp + 1;
    std::string TheKEYNAME;
    int TheKEYVALUE; 
    IRDBRecordset_ptr pIRDBRecordset = pIRDBAccessSvc->getRecordsetPtr("AGDD2GeoSwitches",m_detectorKey,m_detectorNode);
    for(unsigned int i=0; i<pIRDBRecordset->size(); i++) {
      const IRDBRecord* record = (*pIRDBRecordset)[i];
      TheKEYNAME = record->getString("KEYNAME");
      TheKEYVALUE = record->getInt("KEYVALUE");
      if ( TheKEYVALUE == 1 ){
        ATH_MSG_DEBUG( "        Add to m_AGDD2GeoSwitches " << TheKEYNAME ) ;
        m_AGDD2GeoSwitches.push_back(TheKEYNAME);
      }
    }
  }else{
    ATH_MSG_DEBUG( "      agdd2geoVersion is empty " ) ;
  }
  ATH_MSG_DEBUG( "      AGDD2GeoSwitches data collected " ) ;

  pIRDBAccessSvc->shutdown();

  if (m_CtrlFileOracle==1){
    std::ofstream  GeneratedFile;
    GeneratedFile.open("Generated_amdb_simrec_Oracle");
    GeneratedFile << AmdcString << std::endl;
    GeneratedFile << AgddString << std::endl;
    GeneratedFile.close();
  }

//Set Muon Spectrometer Geometry
  sc = SetFromString(AmdcString,AgddString);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "SetFromString failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: SetFromString" ) ;

//Post Geometry Loading Sequence
  sc=PostGeometryLoadingSequence();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("PostGeometryLoadingSequence failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: PostGeometryLoadingSequence" ) ;

  return StatusCode::SUCCESS;

}

StatusCode AmdcsimrecAthenaSvc::PostGeometryLoadingSequence() 
{

  ATH_MSG_DEBUG("----> PostGeometryLoadingSequence is called" ) ; 

//Produce strings
  StatusCode sc=ProduceString();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("ProduceString failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: ProduceString" ) ;

//Possibly modify A line
  controlaline_(m_ControlALine);

//Possibly modify I line
  controliline_(m_ControlILine);

//Possibly truncate A line rotation parameters
  if (m_TruncateALine == 1 ) truncatealine_();

//Possibly truncate P line rotation parameters
  if (m_TruncatePLine == 1 ) truncatepline_();

//Delete p_Amdcsimrec and Set it
  delete p_Amdcsimrec ;
  p_Amdcsimrec = new AmdcsimrecStand;

//Set Internal Stores
  sc = SetAliStoreInternal() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("SetAliStoreInternal failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: SetAliStoreInternal" ) ;

//Possibly Check Hard/Soft schemes
  if (m_HardSoftCheck == 1 ) {
    TestHardSoftStuff() ;
  }

//Possibly Xtomo stuff
  if (m_XtomoCheck == 1 ) {
    TestXtomoStuff() ;
  }

//Do Dumps
  if (m_CtrlDumpCo==1){
    cppbigdump(p_Amdcsimrec);
    bigamdcdump2_();
  }

  return StatusCode::SUCCESS;

}

StatusCode AmdcsimrecAthenaSvc::ProduceString()
{

  ATH_MSG_DEBUG("----> ProduceString is called" ) ; 

  m_AmdcString = "";
  int  Namdc = 0;
  fgetamdccharnumber_(Namdc);
  if (Namdc!=0){
    char* Camdc = new char[Namdc];
    fgetamdccharstring_(Namdc,Camdc);
    m_AmdcString.resize(Namdc);
    for (int i= 0; i <Namdc; i++){m_AmdcString[i]=Camdc[i];}
    delete [] Camdc ;
  }

  m_AgddString = "";
  int  Nagdd = 0;
  fgetagddcharnumber_(Nagdd);
  if (Nagdd!=0){
    char* Cagdd = new char[Nagdd];
    fgetagddcharstring_(Nagdd,Cagdd);
    m_AgddString.resize(Nagdd);
    for (int i= 0; i <Nagdd; i++){m_AgddString[i]=Cagdd[i];}
    delete [] Cagdd ;
  } 

  if (m_CtrlFileStrings==1){
    std::ofstream  GeneratedFile;
    GeneratedFile.open("Generated_amdb_simrec_String");
    GeneratedFile << m_AmdcString << std::endl;
    GeneratedFile << m_AgddString << std::endl;
    GeneratedFile.close();
  }

  return StatusCode::SUCCESS;

} 

StatusCode AmdcsimrecAthenaSvc::SetFromString(std::string AmdcString,std::string AgddString)
{

  ATH_MSG_DEBUG("----> SetFromString is called" ) ; 

  int   NAmdc = AmdcString.size();
  char* CAmdc = new char[NAmdc];
  for (int i= 0; i <NAmdc; i++){CAmdc[i]=AmdcString[i];}
  int   NAgdd = AgddString.size();
  char* CAgdd = new char[NAgdd];
  for (int i= 0; i <NAgdd; i++){CAgdd[i]=AgddString[i];}
  int IFLAG = 100 + m_PrintLevel ;
  loadamdcagddfromstringn_(NAmdc,CAmdc,NAgdd,CAgdd,IFLAG);
  delete [] CAmdc ;
  delete [] CAgdd ;

  return StatusCode::SUCCESS;

} 

StatusCode AmdcsimrecAthenaSvc::GetAmdcAliStores(
                               int&  AmdcABlinesStamp,
                               const AmdcAlineStore*& pAmdcAlineStore ,
                               const AmdcBlineStore*& pAmdcBlineStore ,
                               const AmdcIlineStore*& pAmdcIlineStore
){

  pAmdcAlineStore = 0 ;
  pAmdcBlineStore = 0 ;
  pAmdcIlineStore = 0 ;
  
  if ( m_AlignmentCorr != 0 ) {
    if ( m_AlignmentSource == 2 ) {
      pAmdcAlineStore = p_AmdcAlineStoreExternal ;
      pAmdcBlineStore = p_AmdcBlineStoreExternal ;
      pAmdcIlineStore = p_AmdcIlineStoreExternal ;
      AmdcABlinesStamp = m_AmdcABlinesStamp ;
      return StatusCode::SUCCESS;
    } 
  }
  pAmdcAlineStore = p_AmdcAlineStoreInternal ;
  pAmdcBlineStore = p_AmdcBlineStoreInternal ;
  pAmdcIlineStore = p_AmdcIlineStoreInternal ;
  AmdcABlinesStamp = m_AmdcABlinesStamp ;
  return StatusCode::SUCCESS; 

}
StatusCode AmdcsimrecAthenaSvc::SetAliStoreInternal()
{

  ATH_MSG_DEBUG("----> SetAliStoreInternal is called" ) ; 
  
  p_AmdcAlineStoreInternal->Reset();
  p_AmdcBlineStoreInternal->Reset();  
  p_AmdcIlineStoreInternal->Reset();  
  
  if ( m_UseMuonDetectorManagerForInternal == 0 ){
    ATH_MSG_DEBUG( "      UseMuonDetectorManagerForInternal == 0 " ) ;
    int DB_Jadjust_Max = p_Amdcsimrec->NBadjust();
    for (int DB_Jadjust=1 ; DB_Jadjust<=DB_Jadjust_Max ; DB_Jadjust++){
      int    DB_JTYP ;
      int    DB_JFF  ;
      int    DB_JZZ  ;
      int    DB_JOB  ;
      double Amdc_TraS ;
      double Amdc_TraZ ;
      double Amdc_TraT ;
      double Amdc_RotS ;
      double Amdc_RotZ ;
      double Amdc_RotT ;
      p_Amdcsimrec->GetStationDisplacementFromAMDCJadjust(
                                                          DB_Jadjust,
                                                          DB_JTYP,
                                                          DB_JFF, 
                                                          DB_JZZ,
                                                          DB_JOB,
                                                          Amdc_TraS, 
                                                          Amdc_TraZ, 
                                                          Amdc_TraT,
                                                          Amdc_RotS, 
                                                          Amdc_RotZ, 
                                                          Amdc_RotT 
                                                         );
      if ( m_ModifyALineContainer == 2) 
      reformataline_( 
                     Amdc_TraS , 
                     Amdc_TraZ , 
                     Amdc_TraT , 
                     Amdc_RotS , 
                     Amdc_RotZ , 
                     Amdc_RotT 
                    );

      AmdcAline aAmdcAline;
      aAmdcAline.SetStationType(p_Amdcsimrec->StationName(DB_JTYP));
      aAmdcAline.Setjff  ( DB_JFF    );
      aAmdcAline.Setjzz  ( DB_JZZ    );
      aAmdcAline.Setjob  ( DB_JOB    );
      aAmdcAline.Sets    ( Amdc_TraS );
      aAmdcAline.Setz    ( Amdc_TraZ );
      aAmdcAline.Sett    ( Amdc_TraT );
      aAmdcAline.SetrotS ( Amdc_RotS );
      aAmdcAline.SetrotZ ( Amdc_RotZ );
      aAmdcAline.SetrotT ( Amdc_RotT );
      int StoreIt = 1 ;
      if ( p_Amdcsimrec->StationName(DB_JTYP) == "CSS" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
      if ( p_Amdcsimrec->StationName(DB_JTYP) == "CSL" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
      if ( StoreIt == 1 ) {
        p_AmdcAlineStoreInternal->Add(aAmdcAline);
      }
    }
    
    if ( m_SortAlignContainer == 1 ) p_AmdcAlineStoreInternal->Sort() ;
    
    int DB_Jdeform_Max = p_Amdcsimrec->NBdeform();
    for (int DB_Jdeform=1 ; DB_Jdeform<=DB_Jdeform_Max ; DB_Jdeform++){

      int    DB_JTYP ;
      int    DB_JFF  ;
      int    DB_JZZ  ;
      int    DB_JOB  ;
      double Amdc_bz ;
      double Amdc_bp ;
      double Amdc_bn ;
      double Amdc_sp ;
      double Amdc_sn ;
      double Amdc_tw ;
      double Amdc_pg ;
      double Amdc_tr ;
      double Amdc_eg ;
      double Amdc_ep ;
      double Amdc_en ;
      p_Amdcsimrec->GetStationDeformationFromAMDCJdeform(
                                                         DB_Jdeform,
                                                         DB_JTYP,
                                                         DB_JFF, 
                                                         DB_JZZ,
                                                         DB_JOB,
                                                         Amdc_bz,
                                                         Amdc_bp,
                                                         Amdc_bn,
                                                         Amdc_sp,
                                                         Amdc_sn,
                                                         Amdc_tw,
                                                         Amdc_pg,
                                                         Amdc_tr,
                                                         Amdc_eg,
                                                         Amdc_ep,
                                                         Amdc_en  
                                                        );
      AmdcBline aAmdcBline;
      aAmdcBline.SetStationType(p_Amdcsimrec->StationName(DB_JTYP));
      aAmdcBline.Setjff ( DB_JFF  );
      aAmdcBline.Setjzz ( DB_JZZ  );
      aAmdcBline.Setjob ( DB_JOB  );
      aAmdcBline.Setbz  ( Amdc_bz );
      aAmdcBline.Setbp  ( Amdc_bp );
      aAmdcBline.Setbn  ( Amdc_bn );
      aAmdcBline.Setsp  ( Amdc_sp );
      aAmdcBline.Setsn  ( Amdc_sn );
      aAmdcBline.Settw  ( Amdc_tw );
      aAmdcBline.Setpg  ( Amdc_pg );
      aAmdcBline.Settr  ( Amdc_tr );
      aAmdcBline.Seteg  ( Amdc_eg );
      aAmdcBline.Setep  ( Amdc_ep );
      aAmdcBline.Seten  ( Amdc_en );
      int StoreIt = 1 ;
      if ( p_Amdcsimrec->StationName(DB_JTYP) == "CSS" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
      if ( p_Amdcsimrec->StationName(DB_JTYP) == "CSL" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
      if ( StoreIt == 1 ) {
        p_AmdcBlineStoreInternal->Add(aAmdcBline);
      }
    }
    
    int DB_Jdwnlay_Max = p_Amdcsimrec->NBdwnlay();
    for (int DB_Jdwnlay=1 ; DB_Jdwnlay<=DB_Jdwnlay_Max ; DB_Jdwnlay++){
      int    DB_JTYP ;
      int    DB_JFF  ;
      int    DB_JZZ  ;
      int    DB_JOB  ;
      int    DB_JLAY ;
      double Amdc_TraS ;
      double Amdc_TraZ ;
      double Amdc_TraT ;
      double Amdc_RotS ;
      double Amdc_RotZ ;
      double Amdc_RotT ;
      p_Amdcsimrec->GetStationInternalAlignmentFromAMDCJdwnlay(
                                                          DB_Jdwnlay,
                                                          DB_JTYP,
                                                          DB_JFF, 
                                                          DB_JZZ,
                                                          DB_JOB,
                                                          DB_JLAY,
                                                          Amdc_TraS, 
                                                          Amdc_TraZ, 
                                                          Amdc_TraT,
                                                          Amdc_RotS, 
                                                          Amdc_RotZ, 
                                                          Amdc_RotT 
                                                         );
      AmdcIline aAmdcIline;
      aAmdcIline.SetStationType(p_Amdcsimrec->StationName(DB_JTYP));
      aAmdcIline.Setjff        ( DB_JFF       );
      aAmdcIline.Setjzz        ( DB_JZZ       );
      aAmdcIline.Setjob        ( DB_JOB       );
      aAmdcIline.Setjlay       ( DB_JLAY      );
      aAmdcIline.Sets          ( Amdc_TraS    );
      aAmdcIline.Setz          ( Amdc_TraZ    );
      aAmdcIline.Sett          ( Amdc_TraT    );
      aAmdcIline.SetrotS       ( Amdc_RotS    );
      aAmdcIline.SetrotZ       ( Amdc_RotZ    );
      aAmdcIline.SetrotT       ( Amdc_RotT    );
      p_AmdcIlineStoreInternal->Add(aAmdcIline);
    }
  }else{
    ATH_MSG_DEBUG( "      UseMuonDetectorManagerForInternal != 0 " ) ;
    int LoadIer = 0 ;
    StatusCode sc = SetAmdcAlineStoreFromExternal(p_AmdcAlineStoreInternal,LoadIer) ;
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "SetAmdcAlineStoreFromExternal failed" ) ; 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: SetAmdcAlineStoreFromExternal" ) ;
    LoadIer = 0 ;
    sc = SetAmdcBlineStoreFromExternal(p_AmdcBlineStoreInternal,LoadIer) ;
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "SetAmdcBlineStoreFromExternal failed" ) ; 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: SetAmdcBlineStoreFromExternal" ) ;
    LoadIer = 0 ;
    sc = SetAmdcIlineStoreFromExternal(p_AmdcIlineStoreInternal,LoadIer) ;
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "SetAmdcIlineStoreFromExternal failed" ) ; 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Done: SetAmdcIlineStoreFromExternal" ) ;
  }

  p_AmdcAlineStoreExternal->Reset() ;
  int SizeAInternal = p_AmdcAlineStoreInternal->NberOfObjects();
  for (int Item=0 ; Item<SizeAInternal ; Item++){
    const AmdcAline* pAmdcAlineInternal = p_AmdcAlineStoreInternal->GetAmdcAline( Item ) ;
    AmdcAline aAmdcAline;
    aAmdcAline.SetStationType( pAmdcAlineInternal->GetStationType() );
    aAmdcAline.Setjff        ( pAmdcAlineInternal->Getjff        () );
    aAmdcAline.Setjzz        ( pAmdcAlineInternal->Getjzz        () );
    aAmdcAline.Setjob        ( pAmdcAlineInternal->Getjob        () );
    aAmdcAline.Sets          ( pAmdcAlineInternal->Gets          () );
    aAmdcAline.Setz          ( pAmdcAlineInternal->Getz          () );
    aAmdcAline.Sett          ( pAmdcAlineInternal->Gett          () );
    aAmdcAline.SetrotS       ( pAmdcAlineInternal->GetrotS       () );
    aAmdcAline.SetrotZ       ( pAmdcAlineInternal->GetrotZ       () );
    aAmdcAline.SetrotT       ( pAmdcAlineInternal->GetrotT       () );
    p_AmdcAlineStoreExternal->Add(aAmdcAline);
  }
  
  p_AmdcBlineStoreExternal->Reset() ;
  int SizeBInternal = p_AmdcBlineStoreInternal->NberOfObjects();
  for (int Item=0 ; Item<SizeBInternal ; Item++){
    const AmdcBline* pAmdcBlineInternal = p_AmdcBlineStoreInternal->GetAmdcBline(Item);
    AmdcBline aAmdcBline;
    aAmdcBline.SetStationType( pAmdcBlineInternal->GetStationType() );
    aAmdcBline.Setjff        ( pAmdcBlineInternal->Getjff        () );
    aAmdcBline.Setjzz        ( pAmdcBlineInternal->Getjzz        () );
    aAmdcBline.Setjob        ( pAmdcBlineInternal->Getjob        () );
    aAmdcBline.Setbz         ( pAmdcBlineInternal->Getbz         () );
    aAmdcBline.Setbp         ( pAmdcBlineInternal->Getbp         () );
    aAmdcBline.Setbn         ( pAmdcBlineInternal->Getbn         () );
    aAmdcBline.Setsp         ( pAmdcBlineInternal->Getsp         () );
    aAmdcBline.Setsn         ( pAmdcBlineInternal->Getsn         () );
    aAmdcBline.Settw         ( pAmdcBlineInternal->Gettw         () );
    aAmdcBline.Setpg         ( pAmdcBlineInternal->Getpg         () );
    aAmdcBline.Settr         ( pAmdcBlineInternal->Gettr         () );
    aAmdcBline.Seteg         ( pAmdcBlineInternal->Geteg         () );
    aAmdcBline.Setep         ( pAmdcBlineInternal->Getep         () );
    aAmdcBline.Seten         ( pAmdcBlineInternal->Geten         () );
    p_AmdcBlineStoreExternal->Add(aAmdcBline);
  }

  p_AmdcIlineStoreExternal->Reset() ;
  int SizeIInternal = p_AmdcIlineStoreInternal->NberOfObjects();
  for (int Item=0 ; Item<SizeIInternal ; Item++){
    const AmdcIline* pAmdcIlineInternal = p_AmdcIlineStoreInternal->GetAmdcIlineForUpdate( Item ) ;
    AmdcIline aAmdcIline;
    aAmdcIline.SetStationType( pAmdcIlineInternal->GetStationType() );
    aAmdcIline.Setjff        ( pAmdcIlineInternal->Getjff        () );
    aAmdcIline.Setjzz        ( pAmdcIlineInternal->Getjzz        () );
    aAmdcIline.Setjob        ( pAmdcIlineInternal->Getjob        () );
    aAmdcIline.Setjlay       ( pAmdcIlineInternal->Getjlay       () );
    aAmdcIline.Sets          ( pAmdcIlineInternal->Gets          () );
    aAmdcIline.Setz          ( pAmdcIlineInternal->Getz          () );
    aAmdcIline.Sett          ( pAmdcIlineInternal->Gett          () );
    aAmdcIline.SetrotS       ( pAmdcIlineInternal->GetrotS       () );
    aAmdcIline.SetrotZ       ( pAmdcIlineInternal->GetrotZ       () );
    aAmdcIline.SetrotT       ( pAmdcIlineInternal->GetrotT       () );
    p_AmdcIlineStoreExternal->Add(aAmdcIline);
  }

  ATH_MSG_DEBUG( "      p_AmdcAlineStoreInternal->NberOfObjects()" << p_AmdcAlineStoreInternal->NberOfObjects() ) ;
  ATH_MSG_DEBUG( "      p_AmdcAlineStoreExternal->NberOfObjects()" << p_AmdcAlineStoreExternal->NberOfObjects() ) ;
  ATH_MSG_DEBUG( "      p_AmdcBlineStoreInternal->NberOfObjects()" << p_AmdcBlineStoreInternal->NberOfObjects() ) ;
  ATH_MSG_DEBUG( "      p_AmdcBlineStoreExternal->NberOfObjects()" << p_AmdcBlineStoreExternal->NberOfObjects() ) ;
  ATH_MSG_DEBUG( "      p_AmdcIlineStoreInternal->NberOfObjects()" << p_AmdcIlineStoreInternal->NberOfObjects() ) ;
  ATH_MSG_DEBUG( "      p_AmdcIlineStoreExternal->NberOfObjects()" << p_AmdcIlineStoreExternal->NberOfObjects() ) ;

  return StatusCode::SUCCESS;

}
StatusCode AmdcsimrecAthenaSvc::regFcninitializeFromGeomodelSetAmdcABlineFromCool()
{

  ATH_MSG_DEBUG("----> regFcninitializeFromGeomodelSetAmdcABlineFromCool is called" ) ; 

  StatusCode sc = p_detStore->retrieve(p_MuonDetectorManager);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "p_MuonDetectorManager not found !" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done:p_MuonDetectorManager found " ) ;
 
  sc = p_detStore->regFcn(
                          &IGeoModelSvc::align, &*p_IGeoModelSvc,
                          &AmdcsimrecAthenaSvc::initializeFromGeomodelSetAmdcABlineFromCoolCallback, this
                         );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcsimrecAthenaSvc::initializeFromGeomodelSetAmdcABlineFromCoolCallback from IGeoModelSvc::align" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: Register callback on AmdcsimrecAthenaSvc::initializeFromGeomodelSetAmdcABlineFromCoolCallback from IGeoModelSvc::align" ) ;

  sc = p_detStore->regFcn(
               &AmdcsimrecAthenaSvc::initializeFromGeomodelSetAmdcABlineFromCoolCallback, this,
               &AmdcsimrecAthenaSvc::UpdatedSvc, this
              );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcsimrecAthenaSvc::UpdatedSvc from AmdcsimrecAthenaSvc::initializeFromGeomodelSetAmdcABlineFromCoolCallback" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: Register callback on AmdcsimrecAthenaSvc::UpdatedSvc from AmdcsimrecAthenaSvc::initializeFromGeomodelSetAmdcABlineFromCoolCallback" ) ;

  return StatusCode::SUCCESS;
  
}
StatusCode AmdcsimrecAthenaSvc::initializeFromGeomodelSetAmdcABlineFromCoolCallback(IOVSVC_CALLBACK_ARGS)
{

  ATH_MSG_DEBUG( "----> initializeFromGeomodelSetAmdcABlineFromCoolCallback is called " ) ;

  StatusCode sc = initializeFromGeomodel();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("initializeFromGeomodel failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: initializeFromGeomodel" ) ;
  sc = SetAmdcABlineFromCool();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " SetAmdcABlineFromCool failed " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: SetAmdcABlineFromCool" ) ;

  m_IsUsable      = true ;

  return StatusCode::SUCCESS;
  
}

StatusCode AmdcsimrecAthenaSvc::regFcnSetAmdcABlineFromCool()
{

  ATH_MSG_DEBUG("----> regFcnSetAmdcABlineFromCool is called" ) ; 

  StatusCode sc = p_detStore->retrieve(p_MuonDetectorManager);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "p_MuonDetectorManager not found !" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done:p_MuonDetectorManager found " ) ;
  
  sc = p_detStore->regFcn(
                          &IGeoModelSvc::align,p_IGeoModelSvc,
                          &AmdcsimrecAthenaSvc::SetAmdcABlineFromCoolCallback,this
                         );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcsimrecAthenaSvc::SetAmdcABlineFromCoolCallback from IGeoModelSvc::align" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: Register callback on AmdcsimrecAthenaSvc::SetAmdcABlineFromCoolCallback from IGeoModelSvc::align" ) ;

  sc = p_detStore->regFcn(
               &AmdcsimrecAthenaSvc::SetAmdcABlineFromCoolCallback, this,
               &AmdcsimrecAthenaSvc::UpdatedSvc, this
              );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcsimrecAthenaSvc::UpdatedSvc from AmdcsimrecAthenaSvc::SetAmdcABlineFromCoolCallback" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: Register callback on AmdcsimrecAthenaSvc::UpdatedSvc from AmdcsimrecAthenaSvc::SetAmdcABlineFromCoolCallback" ) ;

  return StatusCode::SUCCESS;
  
}
StatusCode AmdcsimrecAthenaSvc::SetAmdcABlineFromCoolCallback(IOVSVC_CALLBACK_ARGS)
{

  ATH_MSG_DEBUG( "----> SetAmdcABlineFromCoolCallback is called " ) ;

  StatusCode sc = SetAmdcABlineFromCool();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " SetAmdcABlineFromCool failed " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: SetAmdcABlineFromCool" ) ;

  m_IsUsable      = true ;

  return StatusCode::SUCCESS;
  
}
StatusCode AmdcsimrecAthenaSvc::SetAmdcABlineFromCool()
{

 ATH_MSG_DEBUG( "----> SetAmdcABlineFromCool is called " ) ;
 
 if ( m_DontSetAmdcABlineFromCool == 1 ){
   ATH_MSG_DEBUG( "DontSetAmdcABlineFromCool == 1; SetAmdcABlineFromCool will not change the containers " ) ; 
   return StatusCode::SUCCESS;
 }

 if ( m_AlignmentCorr == 1  || m_AlignmentCorr == 3 ){
   StatusCode sc = SetAmdcAlineStoreExternal();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL( " SetAmdcAlineStoreExternal failed " ) ;
     return StatusCode::FAILURE;
   }
   ATH_MSG_DEBUG( "Done: SetAmdcAlineStoreExternal" ) ;
 }
 
 if ( m_AlignmentCorr == 2 || m_AlignmentCorr == 3 ){
   StatusCode sc = SetAmdcBlineStoreExternal();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL( " SetAmdcBlineStoreExternal failed " ) ;
     return StatusCode::FAILURE;
   }
   ATH_MSG_DEBUG( "Done: SetAmdcBlineStoreExternal" ) ;
 }

 StatusCode sc = SetAmdcIlineStoreExternal();
 if ( sc.isFailure() ) {
   ATH_MSG_FATAL( " SetAmdcIlineStoreExternal failed " ) ;
   return StatusCode::FAILURE;
 }
 ATH_MSG_DEBUG( "Done: SetAmdcIlineStoreExternal" ) ;

 return StatusCode::SUCCESS;
  
}

StatusCode AmdcsimrecAthenaSvc::SetAmdcAlineStoreExternal()
{

  ATH_MSG_DEBUG( "----> SetAmdcAlineStoreExternal is called " ) ;

  int LoadIer = 0 ;
  StatusCode sc = SetAmdcAlineStoreFromExternal(p_AmdcAlineStoreExternal,LoadIer) ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "SetAmdcAlineStoreFromExternal failed" ) ; 
    return StatusCode::FAILURE;
  }
  if (LoadIer==1){
    ATH_MSG_DEBUG( "SetAmdcAlineStoreFromExternal did not changed the store " ) ; 
    ATH_MSG_DEBUG( "=> will not change the container " ) ; 
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Done: SetAmdcAlineStoreFromExternal" ) ;

  m_AmdcABlinesStamp = m_AmdcABlinesStamp + 1 ;

  if ( m_ModifyALineContainer == 1) {
//  int SizeExternal = p_AmdcAlineStoreExternal->NberOfObjects();
    int SizeInternal = p_AmdcAlineStoreInternal->NberOfObjects();
    for (int Item=0 ; Item<SizeInternal ; Item++){
      const AmdcAline* pAmdcAlineInternal = p_AmdcAlineStoreInternal->GetAmdcAline( Item ) ;
      int JTYP_Internal = pAmdcAlineInternal->Getjtyp() ;
      int JFF_Internal  = pAmdcAlineInternal->Getjff()  ;
      int JZZ_Internal  = pAmdcAlineInternal->Getjzz()  ;
      int JOB_Internal  = pAmdcAlineInternal->Getjob()  ;
      int Item_External = p_AmdcAlineStoreExternal->getData(JTYP_Internal, JFF_Internal ,JZZ_Internal, JOB_Internal);
      if ( Item_External != -1) {
        double The_s    = pAmdcAlineInternal->Gets   () ;
        double The_z    = pAmdcAlineInternal->Getz   () ;
        double The_t    = pAmdcAlineInternal->Gett   () ;
        double The_rotS = pAmdcAlineInternal->GetrotS() ;
        double The_rotZ = pAmdcAlineInternal->GetrotZ() ;
        double The_rotT = pAmdcAlineInternal->GetrotT() ;
        AmdcAline* pAmdcAlineExternal = p_AmdcAlineStoreExternal->GetAmdcAlineForUpdate( Item_External ) ;
        pAmdcAlineExternal->Sets    ( The_s    );
        pAmdcAlineExternal->Setz    ( The_z    );
        pAmdcAlineExternal->Sett    ( The_t    );
        pAmdcAlineExternal->SetrotS ( The_rotS );
        pAmdcAlineExternal->SetrotZ ( The_rotZ );
        pAmdcAlineExternal->SetrotT ( The_rotT );
      }
    }
  }
         
  ATH_MSG_DEBUG( "      p_AmdcAlineStoreExternal->NberOfObjects()" << p_AmdcAlineStoreExternal->NberOfObjects() ) ;

  if ( m_CompareInternalExternalALineContainer == 1) {
  
    int SizeExternal = p_AmdcAlineStoreExternal->NberOfObjects();
    int SizeInternal = p_AmdcAlineStoreInternal->NberOfObjects();

    std::cout << " AmdcsimrecAthenaSvc::SetAmdcAlineStoreExternal Compare " << std::endl; 
    if ( SizeExternal != SizeInternal ){
      std::cout << "=> SizeExternal != SizeInternal " << " " << SizeExternal << " " << SizeInternal << std::endl;
    }
    double MaxDiffGets    = 0. ;
    double MaxDiffGetz    = 0. ;
    double MaxDiffGett    = 0. ;
    double MaxDiffGetrotS = 0. ;
    double MaxDiffGetrotZ = 0. ;
    double MaxDiffGetrotT = 0. ;
    for (int Item=0 ; Item<SizeInternal ; Item++){
      const AmdcAline* pAmdcAlineInternal = p_AmdcAlineStoreInternal->GetAmdcAline( Item ) ;
      pAmdcAlineInternal->SuperPrint();
      int JTYP_Internal = pAmdcAlineInternal->Getjtyp() ;
      int JFF_Internal  = pAmdcAlineInternal->Getjff()  ;
      int JZZ_Internal  = pAmdcAlineInternal->Getjzz()  ;
      int JOB_Internal  = pAmdcAlineInternal->Getjob()  ;
      int Item_External = p_AmdcAlineStoreExternal->getData(JTYP_Internal, JFF_Internal ,JZZ_Internal, JOB_Internal);
      if ( Item_External != -1){
        const AmdcAline* pAmdcAlineExternal = p_AmdcAlineStoreExternal->GetAmdcAline( Item_External ) ;
        pAmdcAlineExternal->SuperPrint();
        double DiffGets    = pAmdcAlineExternal->Gets    () - pAmdcAlineInternal->Gets    () ;
        double DiffGetz    = pAmdcAlineExternal->Getz    () - pAmdcAlineInternal->Getz    () ;
        double DiffGett    = pAmdcAlineExternal->Gett    () - pAmdcAlineInternal->Gett    () ;
        double DiffGetrotS = pAmdcAlineExternal->GetrotS () - pAmdcAlineInternal->GetrotS () ;
        double DiffGetrotZ = pAmdcAlineExternal->GetrotZ () - pAmdcAlineInternal->GetrotZ () ;
        double DiffGetrotT = pAmdcAlineExternal->GetrotT () - pAmdcAlineInternal->GetrotT () ;
        std::cout << setiosflags(std::ios::fixed);
        std::cout  
            <<  std::setw(3)                        << pAmdcAlineExternal->GetStationType() 
            <<  std::setw(4)                        << pAmdcAlineExternal->Getjff        () 
            <<  std::setw(4)                        << pAmdcAlineExternal->Getjzz        () 
            <<  std::setw(4)                        << pAmdcAlineExternal->Getjob        ()
            <<  std::setw(13)<<std::setprecision(6) << DiffGets    
            <<  std::setw(13)<<std::setprecision(6) << DiffGetz    
            <<  std::setw(13)<<std::setprecision(6) << DiffGett    
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetrotS 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetrotZ 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetrotT 
            << std::endl; 
       if ( fabs( DiffGets    ) > MaxDiffGets    ) MaxDiffGets    = DiffGets    ;
       if ( fabs( DiffGetz    ) > MaxDiffGetz    ) MaxDiffGetz    = DiffGetz    ;
       if ( fabs( DiffGett    ) > MaxDiffGett    ) MaxDiffGett    = DiffGett    ;
       if ( fabs( DiffGetrotS ) > MaxDiffGetrotS ) MaxDiffGetrotS = DiffGetrotS ;
       if ( fabs( DiffGetrotZ ) > MaxDiffGetrotZ ) MaxDiffGetrotZ = DiffGetrotZ ;
       if ( fabs( DiffGetrotT ) > MaxDiffGetrotT ) MaxDiffGetrotT = DiffGetrotT ;
      }
    }

    std::cout  
        <<  std::setw(3)                        << "Max" 
        <<  std::setw(4)                        << "Diff" 
        <<  std::setw(4)                        << "    " 
        <<  std::setw(4)                        << "    "
        <<  std::setw(13)<<std::setprecision(6) << MaxDiffGets    
        <<  std::setw(13)<<std::setprecision(6) << MaxDiffGetz    
        <<  std::setw(13)<<std::setprecision(6) << MaxDiffGett    
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetrotS 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetrotZ 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetrotT 
        << std::endl; 

    std::cout << " In Internal and not in External" << std::endl; 
    for (int Item=0 ; Item<SizeInternal ; Item++){
      const AmdcAline* pAmdcAlineInternal = p_AmdcAlineStoreInternal->GetAmdcAline( Item ) ;
      int JTYP_Internal = pAmdcAlineInternal->Getjtyp() ;
      int JFF_Internal  = pAmdcAlineInternal->Getjff()  ;
      int JZZ_Internal  = pAmdcAlineInternal->Getjzz()  ;
      int JOB_Internal  = pAmdcAlineInternal->Getjob()  ;
      int Item_External = p_AmdcAlineStoreExternal->getData(JTYP_Internal, JFF_Internal ,JZZ_Internal, JOB_Internal);
      if ( Item_External == -1) pAmdcAlineInternal->SuperPrint();
    }

    std::cout << " In External and not in Internal" << std::endl; 
    for (int Item=0 ; Item<SizeExternal ; Item++){
      const AmdcAline* pAmdcAlineExternal = p_AmdcAlineStoreExternal->GetAmdcAline( Item ) ;
      int JTYP_External = pAmdcAlineExternal->Getjtyp() ;
      int JFF_External  = pAmdcAlineExternal->Getjff()  ;
      int JZZ_External  = pAmdcAlineExternal->Getjzz()  ;
      int JOB_External  = pAmdcAlineExternal->Getjob()  ;
      int Item_Internal = p_AmdcAlineStoreInternal->getData(JTYP_External, JFF_External ,JZZ_External, JOB_External);
      if ( Item_Internal == -1) pAmdcAlineExternal->SuperPrint();
    }
    
  }
  
  sc = LoadAlineStoreExternal();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "LoadAlineStoreExternal failed" ) ; 
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: LoadAlineStoreExternal" ) ; 

  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::SetAmdcBlineStoreExternal()
{

  ATH_MSG_DEBUG( "----> SetAmdcBlineStoreExternal is called " ) ;

  int LoadIer = 0 ;
  StatusCode sc = SetAmdcBlineStoreFromExternal(p_AmdcBlineStoreExternal,LoadIer) ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "SetAmdcBlineStoreFromExternal failed" ) ; 
    return StatusCode::FAILURE;
  }
  if (LoadIer==1){
    ATH_MSG_DEBUG( "SetAmdcBlineStoreFromExternal did not changed the store " ) ; 
    ATH_MSG_DEBUG( "=> will not change the container " ) ; 
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Done: SetAmdcBlineStoreFromExternal" ) ; 

  m_AmdcABlinesStamp = m_AmdcABlinesStamp + 1 ;
   
  ATH_MSG_DEBUG( "      p_AmdcBlineStoreExternal->NberOfObjects()" << p_AmdcBlineStoreExternal->NberOfObjects() ) ;

  if ( m_CompareInternalExternalALineContainer == 1) {
    int SizeExternal = p_AmdcBlineStoreExternal->NberOfObjects();
    int SizeInternal = p_AmdcBlineStoreInternal->NberOfObjects();

    std::cout << " AmdcsimrecAthenaSvc::SetAmdcBlineStoreExternal Compare " << std::endl; 
    if ( SizeExternal != SizeInternal ){
      std::cout << "=> SizeExternal != SizeInternal " << " " << SizeExternal << " " << SizeInternal << std::endl;
    }
    double MaxDiffGetbz = 0. ;
    double MaxDiffGetbp = 0. ;
    double MaxDiffGetbn = 0. ;
    double MaxDiffGetsp = 0. ;
    double MaxDiffGetsn = 0. ;
    double MaxDiffGettw = 0. ;
    double MaxDiffGetpg = 0. ;
    double MaxDiffGettr = 0. ;
    double MaxDiffGeteg = 0. ;
    double MaxDiffGetep = 0. ;
    double MaxDiffGeten = 0. ;
    for (int Item=0 ; Item<SizeInternal ; Item++){
      const AmdcBline* pAmdcBlineInternal = p_AmdcBlineStoreInternal->GetAmdcBline( Item ) ;
      pAmdcBlineInternal->SuperPrint();
      int JTYP_Internal = pAmdcBlineInternal->Getjtyp() ;
      int JFF_Internal  = pAmdcBlineInternal->Getjff()  ;
      int JZZ_Internal  = pAmdcBlineInternal->Getjzz()  ;
      int JOB_Internal  = pAmdcBlineInternal->Getjob()  ;
      int Item_External = p_AmdcBlineStoreExternal->getData(JTYP_Internal, JFF_Internal ,JZZ_Internal, JOB_Internal);
      if ( Item_External != -1){
        const AmdcBline* pAmdcBlineExternal = p_AmdcBlineStoreExternal->GetAmdcBline( Item_External ) ;
        pAmdcBlineExternal->SuperPrint();
        double DiffGetbz = pAmdcBlineExternal->Getbz () - pAmdcBlineInternal->Getbz () ;
        double DiffGetbp = pAmdcBlineExternal->Getbp () - pAmdcBlineInternal->Getbp () ;
        double DiffGetbn = pAmdcBlineExternal->Getbn () - pAmdcBlineInternal->Getbn () ;
        double DiffGetsp = pAmdcBlineExternal->Getsp () - pAmdcBlineInternal->Getsp () ;
        double DiffGetsn = pAmdcBlineExternal->Getsn () - pAmdcBlineInternal->Getsn () ;
        double DiffGettw = pAmdcBlineExternal->Gettw () - pAmdcBlineInternal->Gettw () ;
        double DiffGetpg = pAmdcBlineExternal->Getpg () - pAmdcBlineInternal->Getpg () ;
        double DiffGettr = pAmdcBlineExternal->Gettr () - pAmdcBlineInternal->Gettr () ;
        double DiffGeteg = pAmdcBlineExternal->Geteg () - pAmdcBlineInternal->Geteg () ;
        double DiffGetep = pAmdcBlineExternal->Getep () - pAmdcBlineInternal->Getep () ;
        double DiffGeten = pAmdcBlineExternal->Geten () - pAmdcBlineInternal->Geten () ;
        std::cout << setiosflags(std::ios::fixed);
        std::cout  
            <<  std::setw(3)                        << pAmdcBlineExternal->GetStationType() 
            <<  std::setw(4)                        << pAmdcBlineExternal->Getjff        () 
            <<  std::setw(4)                        << pAmdcBlineExternal->Getjzz        () 
            <<  std::setw(4)                        << pAmdcBlineExternal->Getjob        ()
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetbz 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetbp 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetbn 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetsp 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetsn 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGettw 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetpg 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGettr 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGeteg 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGetep 
            <<  std::setw(16)<<std::setprecision(12)<< DiffGeten 
            << std::endl; 
       if ( fabs( DiffGetbz) > MaxDiffGetbz ) MaxDiffGetbz = DiffGetbz ;
       if ( fabs( DiffGetbp) > MaxDiffGetbp ) MaxDiffGetbp = DiffGetbp ;
       if ( fabs( DiffGetbn) > MaxDiffGetbn ) MaxDiffGetbn = DiffGetbn ;
       if ( fabs( DiffGetsp) > MaxDiffGetsp ) MaxDiffGetsp = DiffGetsp ;
       if ( fabs( DiffGetsn) > MaxDiffGetsn ) MaxDiffGetsn = DiffGetsn ;
       if ( fabs( DiffGettw) > MaxDiffGettw ) MaxDiffGettw = DiffGettw ;
       if ( fabs( DiffGetpg) > MaxDiffGetpg ) MaxDiffGetpg = DiffGetpg ;
       if ( fabs( DiffGettr) > MaxDiffGettr ) MaxDiffGettr = DiffGettr ;
       if ( fabs( DiffGeteg) > MaxDiffGeteg ) MaxDiffGeteg = DiffGeteg ;
       if ( fabs( DiffGetep) > MaxDiffGetep ) MaxDiffGetep = DiffGetep ;
       if ( fabs( DiffGeten) > MaxDiffGeten ) MaxDiffGeten = DiffGeten ;
      }
    }

    std::cout  
        <<  std::setw(3)                        << "Max" 
        <<  std::setw(4)                        << "Diff" 
        <<  std::setw(4)                        << "    " 
        <<  std::setw(4)                        << "    "
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetbz 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetbp 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetbn 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetsp 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetsn 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGettw 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetpg 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGettr 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGeteg 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGetep 
        <<  std::setw(16)<<std::setprecision(12)<< MaxDiffGeten 
        << std::endl; 

    std::cout << " In Internal and not in External" << std::endl; 
    for (int Item=0 ; Item<SizeInternal ; Item++){
      const AmdcBline* pAmdcBlineInternal = p_AmdcBlineStoreInternal->GetAmdcBline( Item ) ;
      int JTYP_Internal = pAmdcBlineInternal->Getjtyp() ;
      int JFF_Internal  = pAmdcBlineInternal->Getjff()  ;
      int JZZ_Internal  = pAmdcBlineInternal->Getjzz()  ;
      int JOB_Internal  = pAmdcBlineInternal->Getjob()  ;
      int Item_External = p_AmdcBlineStoreExternal->getData(JTYP_Internal, JFF_Internal ,JZZ_Internal, JOB_Internal);
      if ( Item_External == -1) pAmdcBlineInternal->SuperPrint();
    }

    std::cout << " In External and not in Internal" << std::endl; 
    for (int Item=0 ; Item<SizeExternal ; Item++){
      const AmdcBline* pAmdcBlineExternal = p_AmdcBlineStoreExternal->GetAmdcBline( Item ) ;
      int JTYP_External = pAmdcBlineExternal->Getjtyp() ;
      int JFF_External  = pAmdcBlineExternal->Getjff()  ;
      int JZZ_External  = pAmdcBlineExternal->Getjzz()  ;
      int JOB_External  = pAmdcBlineExternal->Getjob()  ;
      int Item_Internal = p_AmdcBlineStoreInternal->getData(JTYP_External, JFF_External ,JZZ_External, JOB_External);
      if ( Item_Internal == -1) pAmdcBlineExternal->SuperPrint();
    }
    
  }
  
  sc = LoadBlineStoreExternal();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "LoadBlineStoreExternal failed" ) ; 
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: LoadBlineStoreExternal" ) ; 

  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::SetAmdcIlineStoreExternal()
{

  ATH_MSG_DEBUG( "----> SetAmdcIlineStoreExternal is called " ) ;

  int LoadIer = 0 ;
  StatusCode sc = SetAmdcIlineStoreFromExternal(p_AmdcIlineStoreExternal,LoadIer) ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "SetAmdcIlineStoreFromExternal failed" ) ; 
    return StatusCode::FAILURE;
  }
  if (LoadIer==1){
    ATH_MSG_DEBUG( "SetAmdcIlineStoreFromExternal did not changed the store " ) ; 
    ATH_MSG_DEBUG( "=> will not change the container " ) ; 
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Done: SetAmdcIlineStoreFromExternal" ) ; 

  m_AmdcABlinesStamp = m_AmdcABlinesStamp + 1 ;
   
  sc = LoadCscInternalAlignmentStoreExternal();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "LoadCscInternalAlignmentStoreExternal failed" ) ; 
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Done: LoadCscInternalAlignmentStoreExternal" ) ; 

  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::SetAmdcAlineStoreFromExternal(AmdcAlineStore* pAmdcAlineStore,int& LoadIer)
{

  ATH_MSG_DEBUG( "----> SetAmdcAlineStoreFromExternal is called " ) ;

  LoadIer = 0 ;
  const ALineMapContainer* pALineMapContainer = 0;
  StatusCode sc = GetALineMapContainer(pALineMapContainer) ;
  if ( sc.isFailure() ) {
    LoadIer = 1 ;
    ATH_MSG_FATAL( "GetALineMapContainer failed" ) ; 
    return StatusCode::FAILURE;
  }
  if (pALineMapContainer==0){
    LoadIer = 1 ;
    ATH_MSG_DEBUG( "GetALineMapContainer returns null pointer " ) ; 
    ATH_MSG_DEBUG( "=> will not change the container " ) ; 
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Done: GetALineMapContainer" ) ; 

  pAmdcAlineStore->Reset() ;

  std::string StationType ;
  int         jff         ;
  int         jzz         ;
  int         job         ;
  float s    ;
  float z    ;
  float t    ;
  float rotS ;
  float rotZ ;
  float rotT ;
   

  ALineMapContainer::const_iterator ialineF = pALineMapContainer->begin();
  ALineMapContainer::const_iterator ialineL = pALineMapContainer->end();

  for (ALineMapContainer::const_iterator ialine = ialineF; ialine != ialineL; ++ialine){

    ALinePar*  al = ialine->second;
    al->getAmdbId(StationType, jff, jzz, job);
    al->getParameters(s, z, t, rotS, rotZ, rotT);
     
    double double_s    = static_cast<double>( s    ) ;
    double double_z    = static_cast<double>( z    ) ;
    double double_t    = static_cast<double>( t    ) ;
    double double_rotS = static_cast<double>( rotS ) ;
    double double_rotZ = static_cast<double>( rotZ ) ;
    double double_rotT = static_cast<double>( rotT ) ;

    if ( m_ModifyALineContainer == 2) 
    reformataline_( 
                   double_s    , 
                   double_z    , 
                   double_t    , 
                   double_rotS , 
                   double_rotZ , 
                   double_rotT 
                  );

    std::string TheStationType = StationType.substr(0,3);
    AmdcAline aAmdcAline;
    aAmdcAline.SetStationType(TheStationType);
    aAmdcAline.Setjff	     (jff);
    aAmdcAline.Setjzz	     (jzz);
    aAmdcAline.Setjob	     (job);
    aAmdcAline.Sets    ( double_s    );
    aAmdcAline.Setz    ( double_z    );
    aAmdcAline.Sett    ( double_t    );
    aAmdcAline.SetrotS ( double_rotS );
    aAmdcAline.SetrotZ ( double_rotZ );
    aAmdcAline.SetrotT ( double_rotT );
    int StoreIt = 1 ;
    if ( TheStationType == "CSS" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
    if ( TheStationType == "CSL" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
    if ( StoreIt == 1 ) {
      pAmdcAlineStore->Add(aAmdcAline);
    }
  }

  if ( m_SortAlignContainer == 1 ) pAmdcAlineStore->Sort() ;

  ATH_MSG_DEBUG( "      pAmdcAlineStore->NberOfObjects()" << pAmdcAlineStore->NberOfObjects() ) ;

  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::SetAmdcBlineStoreFromExternal(AmdcBlineStore* pAmdcBlineStore,int& LoadIer)
{

  ATH_MSG_DEBUG( "----> SetAmdcBlineStoreFromExternal is called " ) ;

  LoadIer = 0 ;
  const BLineMapContainer* pBLineMapContainer = 0;
  StatusCode sc = GetBLineMapContainer(pBLineMapContainer) ;
  if ( sc.isFailure() ) {
    LoadIer = 1 ;
    ATH_MSG_FATAL( "GetBLineMapContainer failed" ) ; 
    return StatusCode::FAILURE;
  }
  if (pBLineMapContainer==0){
    LoadIer = 1 ;
    ATH_MSG_DEBUG( "GetBLineMapContainer returns null pointer " ) ; 
    ATH_MSG_DEBUG( "=> will not change the container " ) ; 
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Done: GetBLineMapContainer" ) ; 

  pAmdcBlineStore->Reset() ;

  std::string StationType ;
  int         jff         ;
  int         jzz         ;
  int         job         ;
  float bz; // tube bow in plane
  float bp; // tube bow out of plane
  float bn; // tube bow out of plane
  float sp; // cross plate sag out of plane
  float sn; // cross plate sag out of plane
  float tw; // twist
  float pg; // parallelogram
  float tr; // trapezoid
  float eg; // global expansion
  float ep; // local expansion
  float en; // local expansion
   
  BLineMapContainer::const_iterator iblineF = pBLineMapContainer->begin();
  BLineMapContainer::const_iterator iblineL = pBLineMapContainer->end();
  
  for (BLineMapContainer::const_iterator ibline = iblineF; ibline != iblineL; ++ibline){
     
    BLinePar*  bl = ibline->second;
    bl->getAmdbId(StationType, jff, jzz, job);
     
    bl->getParameters(bz, bp, bn, sp, sn, tw,
                      pg, tr, eg, ep, en);
                      
//     ATH_MSG_DEBUG( "=============================SetAmdcBlineStoreFromExternal " ) ; 
//     ATH_MSG_DEBUG( " StationType "  << StationType ) ; 
//     ATH_MSG_DEBUG( " jff         "  << jff         ) ; 
//     ATH_MSG_DEBUG( " jzz         "  << jzz         ) ; 
//     ATH_MSG_DEBUG( " job         "  << job         ) ; 
//     ATH_MSG_DEBUG( " bz          "  << bz          ) ; 
//     ATH_MSG_DEBUG( " bp          "  << bp          ) ; 
//     ATH_MSG_DEBUG( " bn          "  << bn          ) ; 
//     ATH_MSG_DEBUG( " sp          "  << sp          ) ; 
//     ATH_MSG_DEBUG( " sn          "  << sn          ) ; 
//     ATH_MSG_DEBUG( " tw          "  << tw          ) ; 
//     ATH_MSG_DEBUG( " pg          "  << pg          ) ; 
//     ATH_MSG_DEBUG( " tr          "  << tr          ) ; 
//     ATH_MSG_DEBUG( " eg          "  << eg          ) ; 
//     ATH_MSG_DEBUG( " ep          "  << ep          ) ; 
//     ATH_MSG_DEBUG( " en          "  << en          ) ; 

    double double_bz = static_cast<double>( bz ) ;
    double double_bp = static_cast<double>( bp ) ;
    double double_bn = static_cast<double>( bn ) ;
    double double_sp = static_cast<double>( sp ) ;
    double double_sn = static_cast<double>( sn ) ;
    double double_tw = static_cast<double>( tw ) ;
    double double_pg = static_cast<double>( pg ) ;
    double double_tr = static_cast<double>( tr ) ;
    double double_eg = static_cast<double>( eg ) ;
    double double_ep = static_cast<double>( ep ) ;
    double double_en = static_cast<double>( en ) ;
    
//     ATH_MSG_DEBUG( " double_bz          "  << double_bz          ) ; 
//     ATH_MSG_DEBUG( " double_bp          "  << double_bp          ) ; 
//     ATH_MSG_DEBUG( " double_bn          "  << double_bn          ) ; 
//     ATH_MSG_DEBUG( " double_sp          "  << double_sp          ) ; 
//     ATH_MSG_DEBUG( " double_sn          "  << double_sn          ) ; 
//     ATH_MSG_DEBUG( " double_tw          "  << double_tw          ) ; 
//     ATH_MSG_DEBUG( " double_pg          "  << double_pg          ) ; 
//     ATH_MSG_DEBUG( " double_tr          "  << double_tr          ) ; 
//     ATH_MSG_DEBUG( " double_eg          "  << double_eg          ) ; 
//     ATH_MSG_DEBUG( " double_ep          "  << double_ep          ) ; 
//     ATH_MSG_DEBUG( " double_en          "  << double_en          ) ; 

    std::string TheStationType = StationType.substr(0,3);
    AmdcBline aAmdcBline;
    aAmdcBline.SetStationType(TheStationType);
    aAmdcBline.Setjff	     (jff);
    aAmdcBline.Setjzz	     (jzz);
    aAmdcBline.Setjob	     (job);
    aAmdcBline.Setbz ( double_bz );
    aAmdcBline.Setbp ( double_bp );
    aAmdcBline.Setbn ( double_bn );
    aAmdcBline.Setsp ( double_sp );
    aAmdcBline.Setsn ( double_sn );
    aAmdcBline.Settw ( double_tw );
    aAmdcBline.Setpg ( double_pg );
    aAmdcBline.Settr ( double_tr );
    aAmdcBline.Seteg ( double_eg );
    aAmdcBline.Setep ( double_ep );
    aAmdcBline.Seten ( double_en );
    int StoreIt = 1 ;
    if ( TheStationType == "CSS" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
    if ( TheStationType == "CSL" && m_RmAlignCSC == 1 ) StoreIt = 0 ;
    if ( StoreIt == 1 ) {
      pAmdcBlineStore->Add(aAmdcBline);
    }
  }
   
  ATH_MSG_DEBUG( "      pAmdcBlineStore->NberOfObjects()" << pAmdcBlineStore->NberOfObjects() ) ;

  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::SetAmdcIlineStoreFromExternal(AmdcIlineStore* pAmdcIlineStore,int& LoadIer)
{

  ATH_MSG_DEBUG( "----> SetAmdcIlineStoreFromExternal is called " ) ;

  LoadIer = 0 ;
  const CscInternalAlignmentMapContainer* pCscInternalAlignmentMapContainer = 0;
  StatusCode sc = GetCscInternalAlignmentMapContainer(pCscInternalAlignmentMapContainer) ;
  if ( sc.isFailure() ) {
    LoadIer = 1 ;
    ATH_MSG_FATAL( "GetCscInternalAlignmentMapContainer failed" ) ; 
    return StatusCode::FAILURE;
  }
  if (pCscInternalAlignmentMapContainer==0){
    LoadIer = 1 ;
    ATH_MSG_DEBUG( "GetCscInternalAlignmentMapContainer returns null pointer " ) ; 
    ATH_MSG_DEBUG( "=> will not change the container " ) ; 
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Done: GetCscInternalAlignmentMapContainer" ) ; 
  
  pAmdcIlineStore->Reset() ;

  std::string StationType ;
  int         jff         ;
  int         jzz         ;
  int         job         ;
  int         jlay        ;
  float s    ;
  float z    ;
  float t    ;
  float rotS ;
  float rotZ ;
  float rotT ;

  CscInternalAlignmentMapContainer::const_iterator iCscInternalAlignmentF = pCscInternalAlignmentMapContainer->begin();
  CscInternalAlignmentMapContainer::const_iterator iCscInternalAlignmentL = pCscInternalAlignmentMapContainer->end();

  for (CscInternalAlignmentMapContainer::const_iterator iCscInternalAlignment = iCscInternalAlignmentF; iCscInternalAlignment != iCscInternalAlignmentL; ++iCscInternalAlignment){

    CscInternalAlignmentPar*  al = iCscInternalAlignment->second;
    al->getAmdbId(StationType, jff, jzz, job,jlay);
    al->getParameters(s, z, t, rotS, rotZ, rotT);

    double double_s    = static_cast<double>( s    ) ;
    double double_z    = static_cast<double>( z    ) ;
    double double_t    = static_cast<double>( t    ) ;
    double double_rotS = static_cast<double>( rotS ) ;
    double double_rotZ = static_cast<double>( rotZ ) ;
    double double_rotT = static_cast<double>( rotT ) ;

    std::string TheStationType = StationType.substr(0,3);
    AmdcIline aAmdcIline;
    aAmdcIline.SetStationType(TheStationType);
    aAmdcIline.Setjff	     (jff      );
    aAmdcIline.Setjzz	     (jzz      );
    aAmdcIline.Setjob	     (job      );
    aAmdcIline.Setjlay       (jlay     );
    aAmdcIline.Sets    ( double_s    );
    aAmdcIline.Setz    ( double_z    );
    aAmdcIline.Sett    ( double_t    );
    aAmdcIline.SetrotS ( double_rotS );
    aAmdcIline.SetrotZ ( double_rotZ );
    aAmdcIline.SetrotT ( double_rotT );
    pAmdcIlineStore->Add(aAmdcIline);
  }

  ATH_MSG_DEBUG( "      AmdcIlineStore->NberOfObjects()" << pAmdcIlineStore->NberOfObjects() ) ;

  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::GetALineMapContainer(const ALineMapContainer*& pALineMapContainer)
{

  ATH_MSG_DEBUG( "----> GetALineMapContainer is called " ) ;

  pALineMapContainer = p_MuonDetectorManager->ALineContainer() ;

  CheckALineMapContainer(pALineMapContainer);

  return StatusCode::SUCCESS;

}
StatusCode AmdcsimrecAthenaSvc::GetBLineMapContainer(const BLineMapContainer*& pBLineMapContainer)
{

  ATH_MSG_DEBUG( "----> GetBLineMapContainer is called " ) ;

  pBLineMapContainer = p_MuonDetectorManager->BLineContainer() ;

  CheckBLineMapContainer(pBLineMapContainer);
  
  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::GetCscInternalAlignmentMapContainer(const CscInternalAlignmentMapContainer*& pCscInternalAlignmentMapContainer)
{

  ATH_MSG_DEBUG( "----> GetCscInternalAlignmentMapContainer is called " ) ;

  pCscInternalAlignmentMapContainer = p_MuonDetectorManager->CscInternalAlignmentContainer() ;
  
  return StatusCode::SUCCESS;
   
}
void AmdcsimrecAthenaSvc::CheckALineMapContainer(const ALineMapContainer*& pALineMapContainer)
{

  if (pALineMapContainer){
//Check
    std::string StationType1 ;
    int         jff1         ;
    int         jzz1         ;
    int         job1         ;
    float s1    ;
    float z1    ;
    float t1    ;
    float rotS1 ;
    float rotZ1 ;
    float rotT1 ;
    std::string StationType2 ;
    int         jff2         ;
    int         jzz2         ;
    int         job2         ;
    float s2    ;
    float z2    ;
    float t2    ;
    float rotS2 ;
    float rotZ2 ;
    float rotT2 ;
    
    ALineMapContainer::const_iterator line1F = pALineMapContainer->begin();
    ALineMapContainer::const_iterator line1L = pALineMapContainer->end();
    ALineMapContainer::const_iterator line2F = pALineMapContainer->begin();

    for (ALineMapContainer::const_iterator line1 = line1F; line1 != line1L; ++line1){
      ALinePar*  a1 = line1->second;
      a1->getAmdbId(StationType1, jff1, jzz1, job1);
      for (ALineMapContainer::const_iterator line2 = line2F; line2 != line1; ++line2){
        ALinePar*  a2 = line2->second;
        a2->getAmdbId(StationType2, jff2, jzz2, job2);
        if ( StationType1 == StationType2
        &&   jff1         == jff2         
        &&   jzz1         == jzz2         
        &&   job1         == job2         ){
          ATH_MSG_DEBUG( " A LINE DOUBLE ENTRY for     " 
                           << "  StationType " << StationType1  
                           << "  jff         " << jff1         
                           << "  jzz         " << jzz1         
                           << "  job         " << job1         
                           ) ; 
/*          ATH_MSG_DEBUG( " A LINE DOUBLE ENTRY id1     " 
                           << " " << line1->first
                           << " " << p_AtlasDetectorID->print_to_string(line1->first)
                           ) ; 
          ATH_MSG_DEBUG( " A LINE DOUBLE ENTRY id2     " 
                           << " " << line2->first
                           << " " << p_AtlasDetectorID->print_to_string(line2->first)
                           ) ; */
          a1->getParameters(s1, z1, t1, rotS1, rotZ1, rotT1);
          a2->getParameters(s2, z2, t2, rotS2, rotZ2, rotT2);
          ATH_MSG_DEBUG( " A LINE DOUBLE ENTRY values1 " 
                           << " " << s1            
                           << " " << z1            
                           << " " << t1            
                           << " " << rotS1         
                           << " " << rotZ1         
                           << " " << rotT1         
                           ) ; 
          ATH_MSG_DEBUG( " A LINE DOUBLE ENTRY values2 " 
                           << " " << s2            
                           << " " << z2            
                           << " " << t2            
                           << " " << rotS2         
                           << " " << rotZ2         
                           << " " << rotT2         
                           ) ; 
        }
      }
    }
  }

}
void AmdcsimrecAthenaSvc::CheckBLineMapContainer(const BLineMapContainer*& pBLineMapContainer)
{

  if (pBLineMapContainer){
//Check
    std::string StationType1 ;
    int         jff1         ;
    int         jzz1         ;
    int         job1         ;
    float bz1;
    float bp1;
    float bn1;
    float sp1;
    float sn1;
    float tw1;
    float pg1;
    float tr1;
    float eg1;
    float ep1;
    float en1;
    std::string StationType2 ;
    int         jff2         ;
    int         jzz2         ;
    int         job2         ;
    float bz2;
    float bp2;
    float bn2;
    float sp2;
    float sn2;
    float tw2;
    float pg2;
    float tr2;
    float eg2;
    float ep2;
    float en2;
    
    BLineMapContainer::const_iterator line1F = pBLineMapContainer->begin();
    BLineMapContainer::const_iterator line1L = pBLineMapContainer->end();
    BLineMapContainer::const_iterator line2F = pBLineMapContainer->begin();

    for (BLineMapContainer::const_iterator line1 = line1F; line1 != line1L; ++line1){
      BLinePar*  b1 = line1->second;
      b1->getAmdbId(StationType1, jff1, jzz1, job1);
      for (BLineMapContainer::const_iterator line2 = line2F; line2 != line1; ++line2){
        BLinePar*  b2 = line2->second;
        b2->getAmdbId(StationType2, jff2, jzz2, job2);
        if ( StationType1 == StationType2
        &&   jff1         == jff2         
        &&   jzz1         == jzz2         
        &&   job1         == job2         ){
          ATH_MSG_DEBUG( " B LINE DOUBLE ENTRY for     " 
                           << "  StationType " << StationType1  
                           << "  jff         " << jff1         
                           << "  jzz         " << jzz1         
                           << "  job         " << job1         
                           ) ; 
/*          ATH_MSG_DEBUG( " B LINE DOUBLE ENTRY id1     " 
                           << " " << line1->first
                           << " " << p_AtlasDetectorID->print_to_string(line1->first)
                           ) ; 
          ATH_MSG_DEBUG( " B LINE DOUBLE ENTRY id2     " 
                           << " " << line2->first
                           << " " << p_AtlasDetectorID->print_to_string(line2->first)
                           ) ; */
          b1->getParameters(bz1, bp1, bn1, sp1, sn1, tw1,
                            pg1, tr1, eg1, ep1, en1);
          b2->getParameters(bz2, bp2, bn2, sp2, sn2, tw2,
                            pg2, tr2, eg2, ep2, en2);
          ATH_MSG_DEBUG( " B LINE DOUBLE ENTRY values1 " 
                           << " " << bz1           
                           << " " << bp1          
                           << " " << bn1          
                           << " " << sp1           
                           << " " << sn1           
                           << " " << tw1         
                           << " " << pg1         
                           << " " << tr1           
                           << " " << eg1           
                           << " " << ep1
			   << " " << en1           
                           ) ; 
          ATH_MSG_DEBUG( " B LINE DOUBLE ENTRY values2 " 
                           << " " << bz2           
                           << " " << bp2          
                           << " " << bn2          
                           << " " << sp2           
                           << " " << sn2           
                           << " " << tw2         
                           << " " << pg2         
                           << " " << tr2           
                           << " " << eg2           
                           << " " << ep2
                           << " " << en2
                           ) ; 
        }
      }
    }
  }
  
}

void AmdcsimrecAthenaSvc::TestHardSoftStuff(){ p_Amdcsimrec->TestHardSoftStuff(); }
void AmdcsimrecAthenaSvc::TestXtomoStuff(){ p_Amdcsimrec->TestXtomoStuff(); }



int AmdcsimrecAthenaSvc::GetTagInfoXMLStamp() { return m_TagInfoXMLStamp ;}

std::vector< std::pair<std::string,std::string> > AmdcsimrecAthenaSvc::GetTagInfoXML() { return m_TagInfoXML;}

int AmdcsimrecAthenaSvc::GetAGDD2GeoSwitchesStamp() { return m_AGDD2GeoSwitchesStamp ;}

std::vector<std::string> AmdcsimrecAthenaSvc::GetAGDD2GeoSwitches()  {return m_AGDD2GeoSwitches;}


StatusCode AmdcsimrecAthenaSvc::SetTagInfoXML(IOVSVC_CALLBACK_ARGS)
{

  ATH_MSG_DEBUG( "----> SetTagInfoXML is called " ) ;

  const TagInfo* pTagInfo = 0;
  StatusCode sc = p_detStore->retrieve(pTagInfo);
  if (sc.isFailure()){
    ATH_MSG_DEBUG( " Retrieve pTagInfo failed     " ) ;
  }else{
    TagInfo::NameTagPairVec aNameTagPairVec;
    pTagInfo->getInputTags(aNameTagPairVec);
    m_TagInfoXMLStamp = m_TagInfoXMLStamp + 1;
    m_TagInfoXML.clear(); 
    for (unsigned int Item = 0; Item < aNameTagPairVec.size(); Item++) {
      std::string aNameTagPairVecFirst  = aNameTagPairVec[Item].first;   
      std::string aNameTagPairVecSecond = aNameTagPairVec[Item].second;   
      std::pair<std::string,std::string> aPair(aNameTagPairVecFirst,aNameTagPairVecSecond);
      m_TagInfoXML.push_back(aPair);
    }
    ATH_MSG_DEBUG( "     "  
       << " One gets " 
       <<  m_TagInfoXML.size()
       << " items from TagInfo "
       ) ;
    for (unsigned int Item = 0; Item < m_TagInfoXML.size(); Item++) {
      std::string aNameTagPairVecFirst  = m_TagInfoXML[Item].first;   
      std::string aNameTagPairVecSecond = m_TagInfoXML[Item].second;   
      ATH_MSG_DEBUG( "      " 
          << "  " << aNameTagPairVecFirst
          << "  " << aNameTagPairVecSecond
          ) ;
    }
  }
  
  return StatusCode::SUCCESS;

}

StatusCode AmdcsimrecAthenaSvc::SetLocation(std::string NameFile, std::string& FileLocation)
{

  ATH_MSG_DEBUG( "----> SetLocation is called " ) ;

  if (""==NameFile) {
    ATH_MSG_FATAL( "NameFile not set !" ) ;
    return StatusCode::FAILURE;
  }

  FileLocation = PathResolver::find_file (NameFile, "DATAPATH");

  if (FileLocation == ""){
    ATH_MSG_FATAL( "Cannot locate " << NameFile << " from ${DATAPATH}" ) ;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

StatusCode AmdcsimrecAthenaSvc::LoadAlineStoreExternal()
{

  ATH_MSG_DEBUG( "----> LoadAlineStoreExternal is called " ) ;

  std::string StationType ; 
  int         jff         ;
  int         jzz         ;
  int         job         ;
  char STANAME[3];

  int ierr ;

  double DataAline[6];
  double s    ;
  double z    ;
  double t    ;
  double rotS ;
  double rotZ ;
  double rotT ;
  
  jff  = 0 ;
  jzz  = 0 ;
  job  = 0 ;
  ierr = 0 ;

  STANAME[0] = 'Z' ;
  STANAME[1] = 'Z' ;
  STANAME[2] = 'Z' ;
  
  stasetalines_(STANAME, jff, jzz, job, DataAline, ierr);

  int TheSize = p_AmdcAlineStoreExternal->NberOfObjects();
  for (int Index= 0 ; Index <TheSize ; Index++){
    const AmdcAline* pAmdcAline = p_AmdcAlineStoreExternal->GetAmdcAline(Index);
      
      StationType = pAmdcAline->GetStationType();
      jff         = pAmdcAline->Getjff        ();
      jzz         = pAmdcAline->Getjzz        ();
      job         = pAmdcAline->Getjob        ();
      pAmdcAline->GetStationNameChar(STANAME);

      s   = pAmdcAline->Gets    () ;
      z   = pAmdcAline->Getz    () ;
      t   = pAmdcAline->Gett    () ;
      rotS = pAmdcAline->GetrotS () ;
      rotZ = pAmdcAline->GetrotZ () ;
      rotT = pAmdcAline->GetrotT () ;

      DataAline[0] = s    ;
      DataAline[1] = z    ;
      DataAline[2] = t    ;
      DataAline[3] = rotS ;
      DataAline[4] = rotZ ;
      DataAline[5] = rotT ;
      stasetalines_(STANAME, jff, jzz, job, DataAline, ierr);
      if ( ierr != 0 ) {
        ATH_MSG_FATAL( "stasetalines_ failed on " ) ; 
        ATH_MSG_FATAL( "  StationType " << StationType ) ; 
        ATH_MSG_FATAL( "  jff         " << jff         ) ; 
        ATH_MSG_FATAL( "  jzz         " << jzz         ) ; 
        ATH_MSG_FATAL( "  job         " << job         ) ; 
        ATH_MSG_FATAL( "  s    " << s    ) ; 
        ATH_MSG_FATAL( "  z    " << z    ) ; 
        ATH_MSG_FATAL( "  t    " << t    ) ; 
        ATH_MSG_FATAL( "  rotS " << rotS ) ; 
        ATH_MSG_FATAL( "  rotZ " << rotZ ) ; 
        ATH_MSG_FATAL( "  rotT " << rotT ) ; 
        return StatusCode::FAILURE;
      }

  }

  return StatusCode::SUCCESS;
   
}

StatusCode AmdcsimrecAthenaSvc::LoadBlineStoreExternal()
{

  ATH_MSG_DEBUG( "----> LoadBlineStoreExternal is called " ) ;

  std::string StationType ; 
  int         jff         ;
  int         jzz         ;
  int         job         ;
  char STANAME[3];

  int ierr ;

  double DataBline[11];
  double bz ;
  double bp ;
  double bn ;
  double sp ;
  double sn ;
  double tw ;
  double pg ;
  double tr ;
  double eg ;
  double ep ;
  double en ;
  
  jff  = 0 ;
  jzz  = 0 ;
  job  = 0 ;
  ierr = 0 ;

  STANAME[0] = 'Z' ;
  STANAME[1] = 'Z' ;
  STANAME[2] = 'Z' ;
  
  stasetblines_(STANAME, jff, jzz, job, DataBline, ierr);

  int TheSize = p_AmdcBlineStoreExternal->NberOfObjects();
  for (int Index= 0 ; Index <TheSize ; Index++){
    const AmdcBline* pAmdcBline = p_AmdcBlineStoreExternal->GetAmdcBline(Index);
      
      StationType = pAmdcBline->GetStationType();
      jff         = pAmdcBline->Getjff        ();
      jzz         = pAmdcBline->Getjzz        ();
      job         = pAmdcBline->Getjob        ();
      pAmdcBline->GetStationNameChar(STANAME);

      bz = pAmdcBline->Getbz();
      bp = pAmdcBline->Getbp();
      bn = pAmdcBline->Getbn();
      sp = pAmdcBline->Getsp();
      sn = pAmdcBline->Getsn();
      tw = pAmdcBline->Gettw();
      pg = pAmdcBline->Getpg();
      tr = pAmdcBline->Gettr();
      eg = pAmdcBline->Geteg();
      ep = pAmdcBline->Getep();
      en = pAmdcBline->Geten();

      DataBline[ 0] = bz;
      DataBline[ 1] = bp;
      DataBline[ 2] = bn;
      DataBline[ 3] = sp;
      DataBline[ 4] = sn;
      DataBline[ 5] = tw;
      DataBline[ 6] = pg;
      DataBline[ 7] = tr;
      DataBline[ 8] = eg;
      DataBline[ 9] = ep;
      DataBline[10] = en;
      stasetblines_(STANAME, jff, jzz, job, DataBline, ierr);
      if ( ierr != 0 ) {
        ATH_MSG_FATAL( " stasetblines_ failed on " ) ; 
        ATH_MSG_FATAL( "  StationType " << StationType ) ; 
        ATH_MSG_FATAL( "  jff         " << jff         ) ; 
        ATH_MSG_FATAL( "  jzz         " << jzz         ) ; 
        ATH_MSG_FATAL( "  job         " << job         ) ; 
        ATH_MSG_FATAL( "  bz  " << bz ) ; 
        ATH_MSG_FATAL( "  bp  " << bp ) ; 
        ATH_MSG_FATAL( "  bn  " << bn ) ; 
        ATH_MSG_FATAL( "  sp  " << sp ) ; 
        ATH_MSG_FATAL( "  sn  " << sn ) ; 
        ATH_MSG_FATAL( "  tw  " << tw ) ; 
        ATH_MSG_FATAL( "  pg  " << pg ) ; 
        ATH_MSG_FATAL( "  tr  " << tr ) ; 
        ATH_MSG_FATAL( "  eg  " << eg ) ; 
        ATH_MSG_FATAL( "  ep  " << ep ) ;
        ATH_MSG_FATAL( "  en  " << en ) ; 
        return StatusCode::FAILURE;
      }

  }

  return StatusCode::SUCCESS;
   
}
StatusCode AmdcsimrecAthenaSvc::LoadCscInternalAlignmentStoreExternal()
{

  ATH_MSG_DEBUG( "----> LoadCscInternalAlignmentStoreExternal is called " ) ;

  std::string StationType ; 
  int         jff         ;
  int         jzz         ;
  int         job         ;
  int         jlay        ;
  char STANAME[3];

  int ierr ;

  double DataIline[6];
  double s    ;
  double z    ;
  double t    ;
  double rotS ;
  double rotZ ;
  double rotT ;
  
  jff  = 0 ;
  jzz  = 0 ;
  job  = 0 ;
  jlay = 0 ;
  ierr = 0 ;

  STANAME[0] = 'Z' ;
  STANAME[1] = 'Z' ;
  STANAME[2] = 'Z' ;
  
  stasetilines_(STANAME, jff, jzz, job, jlay, DataIline, ierr);

  int TheSize = p_AmdcIlineStoreExternal->NberOfObjects();
  for (int Index= 0 ; Index <TheSize ; Index++){
    const AmdcIline* pAmdcIline = p_AmdcIlineStoreExternal->GetAmdcIline(Index);
      
      StationType = pAmdcIline->GetStationType();
      jff         = pAmdcIline->Getjff        ();
      jzz         = pAmdcIline->Getjzz        ();
      job         = pAmdcIline->Getjob        ();
      jlay        = pAmdcIline->Getjlay       ();
      pAmdcIline->GetStationNameChar(STANAME);

      s   = pAmdcIline->Gets    () ;
      z   = pAmdcIline->Getz    () ;
      t   = pAmdcIline->Gett    () ;
      rotS = pAmdcIline->GetrotS () ;
      rotZ = pAmdcIline->GetrotZ () ;
      rotT = pAmdcIline->GetrotT () ;

      DataIline[0] = s    ;
      DataIline[1] = z    ;
      DataIline[2] = t    ;
      DataIline[3] = rotS ;
      DataIline[4] = rotZ ;
      DataIline[5] = rotT ;
      stasetilines_(STANAME, jff, jzz, job, jlay, DataIline, ierr);
      if ( ierr != 0 ) {
        ATH_MSG_FATAL( "stasetilines_ failed on " ) ; 
        ATH_MSG_FATAL( "  StationType " << StationType ) ; 
        ATH_MSG_FATAL( "  jff         " << jff         ) ; 
        ATH_MSG_FATAL( "  jzz         " << jzz         ) ; 
        ATH_MSG_FATAL( "  job         " << job         ) ; 
        ATH_MSG_FATAL( "  jlay        " << jlay        ) ; 
        ATH_MSG_FATAL( "  s    " << s    ) ; 
        ATH_MSG_FATAL( "  z    " << z    ) ; 
        ATH_MSG_FATAL( "  t    " << t    ) ; 
        ATH_MSG_FATAL( "  rotS " << rotS ) ; 
        ATH_MSG_FATAL( "  rotZ " << rotZ ) ; 
        ATH_MSG_FATAL( "  rotT " << rotT ) ; 
        return StatusCode::FAILURE;
      }

  }
  seticsc_();


  return StatusCode::SUCCESS;
   
}

