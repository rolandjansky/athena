/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"

#include "MdtCalibInterfaces/IMdtCalibDBTool.h"

#include <string>

#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/BFieldCorFunc.h"
#include "MdtCalibData/WireSagCorFunc.h"
#include "MdtCalibData/MdtSlewCorFuncHardcoded.h"
#include "MdtCalibData/CalibFunc.h"


MdtCalibrationDbSvc::MdtCalibrationDbSvc(const std::string& n,ISvcLocator* sl)
  : AthService(n,sl), m_rtData(0), m_tubeData(0), m_corData(0), 
    m_regionSvc("MdtCalibrationRegionSvc", name()),
    m_mdtIdHelper(0),
    m_dbTool("MuonCalib::MdtCalibDbAsciiTool"),
    m_detStore("StoreGateSvc/DetectorStore", name()),
    m_tubeDataLocation("TubeKey"),
    m_rtDataLocation("RtKey"),
    m_corDataLocation("CorKey")
{
  declareProperty("DBTool", m_dbTool,
		  "the Tool to be used to retreive the constants");
  declareProperty("AccessTubeConstants", m_getTubeConstants = true,
		  "configure the Tool to retrieve the constants per tube (t0)");
  declareProperty("AccessCorrections", m_getCorrections = true,
		  "configure the Tool to retreive the parameters of the correction functions");
  declareProperty("CreateBFieldFunctions", m_create_b_field_function = false,
		  "If set to true, the B-field correction functions are initialized for each rt-relation that is loaded.");
  declareProperty("CreateWireSagFunctions", m_createWireSagFunction = false,
		  "If set to true, the wire sag correction functions are initialized for each rt-relation that is loaded.");
  declareProperty("CreateSlewingFunctions", m_createSlewingFunction = false, 
		  "If set to true, the slewing correction functions are initialized for each rt-relation that is loaded.");
}

MdtCalibrationDbSvc::~MdtCalibrationDbSvc() {}

// queryInterface 
StatusCode MdtCalibrationDbSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{
  if ( interfaceID().versionMatch(riid) ) { 
    *ppvIF = dynamic_cast<MdtCalibrationDbSvc*>(this); 
  } else {
    return AthService::queryInterface(riid, ppvIF); 
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode MdtCalibrationDbSvc::initialize()
{
  if( AthService::initialize().isFailure() ) return StatusCode::FAILURE;

  if ( m_regionSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve MdtCalibrationRegionSvc" );
    return StatusCode::FAILURE;
  }
  // initialize the MdtIdHelper
  
  if ( m_detStore.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Can't locate the DetectorStore" ); 
    return StatusCode::FAILURE;
  }

  if (m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" ).isFailure()) {
    ATH_MSG_ERROR( "Can't retrieve MdtIdHelper" );
    return StatusCode::FAILURE;
  }
  
  // access the Tool for retreival of DB info
  if (m_dbTool.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Could not find tool " << m_dbTool << ". Exiting." );
    return StatusCode::FAILURE;
  }

  // register call backs requiring to be called after the Tool 
  // will work for Ascii Tool only
  // for real DB access the callback is registered with 2 functions 
  // within the Tool

  if (m_dbTool.type()=="MuonCalib::MdtCalibDbAsciiTool"){
    const DataHandle<MdtTubeCalibContainerCollection> tubeData;
    if(m_detStore->regFcn(&MdtCalibrationDbSvc::LoadCalibration,dynamic_cast<MdtCalibrationDbSvc*>(this),tubeData,m_tubeDataLocation).isFailure()){
      ATH_MSG_ERROR("Failed to register call-back on tube data");
      return StatusCode::FAILURE;
    }
    const DataHandle<MdtRtRelationCollection> rtData;
    if(m_detStore->regFcn(&MdtCalibrationDbSvc::LoadCalibration,
			  dynamic_cast<MdtCalibrationDbSvc*>(this),
			  rtData,m_rtDataLocation).isFailure() ){
      ATH_MSG_ERROR("Failed to register call-back on rt data");
      return StatusCode::FAILURE;
    }
  }
    
  return StatusCode::SUCCESS;
}

StatusCode MdtCalibrationDbSvc::LoadCalibration(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_DEBUG( "LoadCalibration has been triggered for the following keys " );                                                                                 
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    msg(MSG::DEBUG) << *itr << " I="<<I<<" ";
  }
  msg(MSG::DEBUG) << endreq;

  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    if(*itr==m_tubeDataLocation) {
      if(loadTube(I,keys).isFailure()) return StatusCode::FAILURE;
    }
    if(*itr==m_rtDataLocation) {
      if(loadRt(I,keys).isFailure()) return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MdtCalibrationDbSvc::loadTube(IOVSVC_CALLBACK_ARGS) 
{ 
  ATH_MSG_DEBUG( "In loadTube " );  
  return m_detStore->retrieve(m_tubeData, m_tubeDataLocation);
}

StatusCode MdtCalibrationDbSvc::loadRt(IOVSVC_CALLBACK_ARGS) 
{
  ATH_MSG_DEBUG( "In loadRt " );  
  if( m_detStore->retrieve(m_rtData, m_rtDataLocation).isFailure() ) return StatusCode::FAILURE;
  // corrections are still missing...
  
  if( !m_createSlewingFunction && !m_createWireSagFunction && !m_create_b_field_function ) return StatusCode::SUCCESS;

  if( m_corData ) delete m_corData;
  m_corData = new MdtCorFuncSetCollection();
  m_corData->resize(m_rtData->size());
  ATH_MSG_DEBUG( "Initializing " << m_corData->size() << " b-field functions" );
  for (unsigned int i=0; i < m_corData->size(); i++)
    {
      (*m_corData)[i] = new MuonCalib::MdtCorFuncSet();
      if (m_create_b_field_function) initialize_B_correction((*m_corData)[i], (*m_rtData)[i]);
      if (m_createWireSagFunction)   initializeSagCorrection((*m_corData)[i]);
      if (m_createSlewingFunction)   (*m_corData)[i]->setSlewing(new MuonCalib::MdtSlewCorFuncHardcoded(MuonCalib::CalibFunc::ParVec()));
    }

  return StatusCode::SUCCESS;
}

StatusCode MdtCalibrationDbSvc::finalize() 
{
  if( m_corData ) delete m_corData;
  return AthService::finalize();
}

MuonCalib::MdtFullCalibData MdtCalibrationDbSvc::getCalibration( const Identifier& idt ) const {

  Identifier id = m_mdtIdHelper->elementID( idt );

  MdtBasicRegionHash mlHash;
  IdContext idCont = m_mdtIdHelper->module_context();
  m_mdtIdHelper->get_hash( id, mlHash, &idCont );
  
  IdentifierHash chamberHash;
  idCont = m_mdtIdHelper->module_context();
  m_mdtIdHelper->get_hash( id, chamberHash, &idCont );
  
  return getCalibration( chamberHash, mlHash );
}

MuonCalib::MdtFullCalibData MdtCalibrationDbSvc::getCalibration( const IdentifierHash& chamberHash, const IdentifierHash& /*mlHash*/ ) const {

  const MuonCalib::MdtRtRelation*          rt = 0;
  const MuonCalib::MdtTubeCalibContainer* tub = 0;
  const MuonCalib::MdtCorFuncSet*         cor = 0;

  
  IdentifierHash rtHash = chamberHash;
  IdentifierHash corHash = chamberHash;
  IdentifierHash tubeHash = chamberHash;

  if ( rtHash.is_valid() ) {

    // find rt relation
    rt = getRtCalibration( rtHash );
    if ( !rt ) { 
      ATH_MSG_WARNING( "Not valid MdtRtRelation found " );
    } 

  }else{
    ATH_MSG_WARNING( "--- INVALID HASH, cannot access rt calibration constants " );
  }
   
  // find correction functions 
  if( m_getCorrections && corHash.is_valid()) {
    cor = getCorFunctions( corHash );
  }

  // find t0's
  if( m_getTubeConstants && tubeHash.is_valid() ) {
    tub = getTubeCalibContainer( tubeHash );
    if( !tub ){
      ATH_MSG_WARNING( "Not valid MdtTubeCalibContainer found " );
    }
  }
  
  return MuonCalib::MdtFullCalibData( cor, rt, tub );
}

const MuonCalib::MdtRtRelation* MdtCalibrationDbSvc::getRtCalibration( const Identifier& idt ) const {
  
  Identifier id = m_mdtIdHelper->elementID( idt );

  MdtBasicRegionHash hash;
  IdContext idCont = m_mdtIdHelper->module_context();
  m_mdtIdHelper->get_hash( id, hash, &idCont );

  return getRtCalibration( hash );
}

const MuonCalib::MdtRtRelation* MdtCalibrationDbSvc::getRtCalibration( const IdentifierHash& hash ) const 
{
  // check whether there are cached rt's
  if( !m_rtData ) {
    ATH_MSG_WARNING( "No rt-calibrations loaded"  );
    return 0;
  }
  // check if hash is ok
  if ( !hash.is_valid() ) {
    ATH_MSG_WARNING( "cannot get rt, invalid hash"  ); 
    return 0;
  }
  // calculated region hash
  MdtRegionHash rtRegionHash = m_regionSvc->getRtRegionHash( hash ); 

  // rt goes per region
  if ( rtRegionHash.is_valid() && rtRegionHash < m_rtData->size() ) { 
    return (*m_rtData)[ rtRegionHash ]; 
  }
  ATH_MSG_WARNING( "cannot get rt, region hash out of range"  ); 
  return 0;
}

const MuonCalib::MdtTubeCalibContainer* MdtCalibrationDbSvc::getTubeCalibContainer( const Identifier& idt ) const
{
  Identifier id = m_mdtIdHelper->elementID( idt );

  MdtBasicRegionHash hash;
  IdContext idCont = m_mdtIdHelper->module_context();
  m_mdtIdHelper->get_hash( id, hash, &idCont );

  return getTubeCalibContainer( hash );
}

const MuonCalib::MdtTubeCalibContainer* MdtCalibrationDbSvc::getTubeCalibContainer( const IdentifierHash& hash ) const 
{

  if( !m_tubeData ) return 0;

  // single tube calibration goes per chamber 
  if ( hash.is_valid() && hash < m_tubeData->size() ) { 
    return (*m_tubeData)[ hash ]; 
  }
  return 0;
}

const MuonCalib::MdtCorFuncSet* MdtCalibrationDbSvc::getCorFunctions( const Identifier& idt ) const
{
  Identifier id = m_mdtIdHelper->elementID( idt );

  MdtBasicRegionHash hash;
  IdContext idCont = m_mdtIdHelper->module_context();
  m_mdtIdHelper->get_hash( id, hash, &idCont );

  return getCorFunctions( hash );
}

const MuonCalib::MdtCorFuncSet* MdtCalibrationDbSvc::getCorFunctions( const IdentifierHash& hash ) const 
{
  // check whether there are cached rt's
  if( !m_corData ) return 0;

  // check if hash is ok
  if ( !hash.is_valid() ) return 0;

  // correction functions go per region
  MdtRegionHash corRegionHash = m_regionSvc->getCorRegionHash( hash ); 

  // corrections go per region 
  if ( corRegionHash.is_valid() && corRegionHash < m_corData->size() ) { 
    return (*m_corData)[ corRegionHash ]; 
  }  
  return 0;
}

void MdtCalibrationDbSvc::initialize_B_correction(MuonCalib::MdtCorFuncSet* funcSet,
                                                  const MuonCalib::MdtRtRelation* rt_rel)
{
  if (rt_rel==NULL)
    {
      funcSet->setBField(NULL);
      return;
    }
  ATH_MSG_VERBOSE( "initialize_B_correction..." );
  std::vector<double> corr_params(2);
  corr_params[0] = 3080.0; // high voltage
  corr_params[1] = 0.11; // epsilon parameter
  funcSet->setBField(new MuonCalib::BFieldCorFunc(std::string("medium"), corr_params, rt_rel->rt()));
}

void MdtCalibrationDbSvc::initializeSagCorrection(MuonCalib::MdtCorFuncSet* funcSet)
{
  ATH_MSG_VERBOSE( "initializeSagCorrection..." );
  std::vector<double> corr_params(0);
  funcSet->wireSag(new MuonCalib::WireSagCorFunc(corr_params));
}
