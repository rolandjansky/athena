///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgTool.cxx 
// Implementation file for class AthAlgTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes

// AthenaBaseComps includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "./VHKASupport.h"
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthAlgTool::AthAlgTool( const std::string& type, 
			const std::string& name, 
			const IInterface* parent ) : 
  ::AlgTool      ( type, name, parent ),
  m_evtStore     ( "StoreGateSvc/StoreGateSvc",  name ),
  m_detStore     ( "StoreGateSvc/DetectorStore", name ),
  m_userStore    ( "UserDataSvc/UserDataSvc", name ),
  m_varHandleArraysDeclared (false)
{
  //
  // Property declaration
  // 

  auto props = getProperties();
  for( Property* prop : props ) {
    if (prop->name() == "ExtraOutputs" || prop->name() == "ExtraInputs") {
      prop->declareUpdateHandler
        (&AthAlgTool::extraDeps_update_handler, this);
    }
  }

  declareProperty( "EvtStore",
                   m_evtStore = StoreGateSvc_t ("StoreGateSvc", name),
                   "Handle to a StoreGateSvc instance: it will be used to "
                   "retrieve data during the course of the job" );

  declareProperty( "DetStore",
                   m_detStore = StoreGateSvc_t ("StoreGateSvc/DetectorStore", name),
                   "Handle to a StoreGateSvc/DetectorStore instance: it will be used to "
                   "retrieve data during the course of the job" );

  declareProperty( "UserStore",
                   m_userStore = UserDataSvc_t ("UserDataSvc/UserDataSvc", name),
                   "Handle to a UserDataSvc/UserDataSvc instance: it will be used to "
                   "retrieve user data during the course of the job" );
}

// Destructor
///////////////
AthAlgTool::~AthAlgTool()
{ 
}


/**
 * @brief Perform system initialization for a tool.
 *
 * We override this to declare all the elements of handle key arrays
 * at the end of initialization.
 * See comments on updateVHKA.
 */
StatusCode AthAlgTool::sysInitialize()
{
  ATH_CHECK( AlgTool::sysInitialize() );

  for (  SG::VarHandleKeyArray* a: m_vhka ) {
    a->declare( this );
  }
  m_varHandleArraysDeclared = true;

  return StatusCode::SUCCESS;
}


/**
 * @brief Handle START transition.
 *
 * We override this in order to make sure that conditions handle keys
 * can cache a pointer to the conditions container.
 */
StatusCode AthAlgTool::sysStart()
{
  ATH_CHECK( AlgTool::sysStart() );

  // Call start() on all input handles.
  // This allows CondHandleKeys to cache pointers to their conditions containers.
  // (CondInputLoader makes the containers that it creates during start(),
  // so initialize() is too early for this.)
  for (Gaudi::DataHandle* h : inputHandles()) {
    if (h->isCondition()) {
      if (SG::VarHandleKey* k = dynamic_cast<SG::VarHandleKey*> (h)) {
        ATH_CHECK( k->start() );
      }
    }
  }
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Return this tool's input handles.
 *
 * We override this to include handle instances from key arrays
 * if they have not yet been declared.
 * See comments on updateVHKA.
 */
std::vector<Gaudi::DataHandle*> AthAlgTool::inputHandles() const
{
  std::vector<Gaudi::DataHandle*> v = AlgTool::inputHandles();

  if (!m_varHandleArraysDeclared) {
    VHKASupport::insertInput( m_vhka, v );
  }

  return v;
}

void AthAlgTool::renounceArray( SG::VarHandleKeyArray& vh ) {
  vh.renounce();
}


/**
 * @brief Return this tool's output handles.
 *
 * We override this to include handle instances from key arrays
 * if they have not yet been declared.
 * See comments on updateVHKA.
 */
std::vector<Gaudi::DataHandle*> AthAlgTool::outputHandles() const
{
  std::vector<Gaudi::DataHandle*> v = AlgTool::outputHandles();
  if (!m_varHandleArraysDeclared) {
    VHKASupport::insertOutput( m_vhka, v );
  }  
  return v;
}


/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void 
AthAlgTool::msg_update_handler( Property& outputLevel ) 
{
   // We can't just rely on the return value of msgLevel() here. Since it's
   // not well defined whether the base class gets updated with the new
   // output level first, or this class. So by default just use the property
   // itself. The fallback is only there in case Gaudi changes its property
   // type at one point, to be able to fall back on something.
   IntegerProperty* iprop = dynamic_cast< IntegerProperty* >( &outputLevel );
   if( iprop ) {
     msgStream().setLevel( static_cast<MSG::Level> (iprop->value()) );
   } else {
     msgStream().setLevel( msgLevel() );
   }
}

/**
 * @brief Add StoreName to extra input/output deps as needed
 *
 * use the logic of the VarHandleKey to parse the DataObjID keys
 * supplied via the ExtraInputs and ExtraOuputs Properties to add
 * the StoreName if it's not explicitly given
 */
void 
AthAlgTool::extraDeps_update_handler( Property& ExtraDeps ) 
{
  DataObjIDColl newColl;
  Gaudi::Property<DataObjIDColl> *prop = dynamic_cast<Gaudi::Property<DataObjIDColl>*> (&ExtraDeps);
  if ( prop ) {
    for (auto id : prop->value()) {
      SG::VarHandleKey vhk(id.clid(), id.key(), Gaudi::DataHandle::Reader);
      id.updateKey( vhk.objKey() );
      newColl.emplace( id );
    }
    if (newColl.size() != 0) prop->setValue( newColl );
  } else {
    ATH_MSG_ERROR("unable to dcast ExtraInput/Output Property");
  }
}
