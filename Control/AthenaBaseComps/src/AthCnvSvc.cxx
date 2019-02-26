///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthCnvSvc.cxx 
// Implementation file for class AthCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"

// AthenaBaseComps includes
#include "AthenaBaseComps/AthCnvSvc.h"

#include "GaudiKernel/Converter.h"

#include "CxxUtils/checker_macros.h"

enum CnvSvcAction   {
  CREATE_OBJ,
  FILL_OBJ_REFS,
  UPDATE_OBJ,
  UPDATE_OBJ_REFS,
  CREATE_REP,
  FILL_REP_REFS,
  UPDATE_REP,
  UPDATE_REP_REFS
};

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthCnvSvc::AthCnvSvc (const std::string& name, 
                      ISvcLocator* pSvcLocator,
                      long type) : 
  ::AthService (name, pSvcLocator),
  m_dataSvc    (0),
  m_addressCreator(0),
  m_type       (type),
  m_workers    ( )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  setAddressCreator(this).ignore();

  m_cnvSvc = this;
}

// Destructor
///////////////
AthCnvSvc::~AthCnvSvc()
{ 
  // the following is hopefully (!) temporary hack to prevent deleting
  // services we hold a reference count to.
  // the problem is that these references might point back to us (so we
  // end up recursively calling the d-tor) or be double-deleted later on
  // by the ServiceManager.
  // the correct fix is to have everybody use SmartIF<T> all over the code...
  //
  // "temporary hack" ha ha! (CGL 5/2014)
  //
  if (m_cnvSvc) {
    m_cnvSvc->addRef();
    m_cnvSvc = 0;
  }
  if (m_addressCreator) {
    m_addressCreator->addRef();
    m_addressCreator = 0;
  }
  if (m_dataSvc) {
    m_dataSvc->addRef();
    m_dataSvc = 0;
  }

  ATH_MSG_DEBUG ("release-ing all workers (" << m_workers.size() << ")...");
  for ( Workers::iterator
	  i    = m_workers.begin(),
	  iend = m_workers.end();
	i != iend;
	++i ) {
    IConverter *cnv = i->second.m_converter;
    if (cnv) {
      
      if (!cnv->release()) {
        i->second.m_converter = 0;
      }
    }
  }
  ATH_MSG_DEBUG ("release-ing all workers (" << m_workers.size() << ")... [done]");
  m_workers.clear();

}

// Athena Service's Hooks
////////////////////////////
StatusCode AthCnvSvc::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode AthCnvSvc::finalize()
{
  ATH_MSG_DEBUG ("releasing all workers");
  for ( Workers::iterator 
	  i    = m_workers.begin(),
	  iend = m_workers.end();
	i != iend;
	++i ) {
    if (!i->second.converter()->finalize().isSuccess()) {
      ATH_MSG_ERROR ("finalizing worker w/ clid=[" << i->first << "]");
    }
    i->second.converter()->release();
  }
  //m_workers.clear();
  Workers().swap (m_workers);

  if (m_addressCreator) m_addressCreator->addRef();
  if (m_dataSvc) m_dataSvc->addRef();
  if (m_cnvSvc) m_cnvSvc->addRef();

  m_addressCreator = 0;
  m_dataSvc = 0;
  m_cnvSvc = 0;

  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
AthCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( riid == IConversionSvc::interfaceID() )  {
    *ppvInterface = (IConversionSvc*)this;
  }
  else if ( riid == IConverter::interfaceID() )  {
    *ppvInterface = (IConverter*)this;
  }
  else if ( riid == IAddressCreator::interfaceID() )  {
    *ppvInterface = (IAddressCreator*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

///@name @c IConversionSvc implementation
//@{
/// Retrieve the class type of the data store the converter uses.
long 
AthCnvSvc::repSvcType() const
{
  return m_type;
}

/// Implementation of IConverter: dummy call
const CLID& 
AthCnvSvc::objType() const
{
  return CLID_NULL;
}

/** Implementation of IConverter: Set Data provider service
    @return    Status code indicating success or failure
    @param     pService   Pointer to data provider service
*/
StatusCode
AthCnvSvc::setDataProvider(IDataProviderSvc* pDataSvc)
{
  if ( !pDataSvc ) return StatusCode::SUCCESS; //Atlas does not use DataSvc
  if ( m_dataSvc ) m_dataSvc->release();
  m_dataSvc = pDataSvc;
  m_dataSvc->addRef();
  Workers::iterator stop  = m_workers.end();
  Workers::iterator start = m_workers.begin();
  for(Workers::iterator i=start; i != stop; i++ )    {
    IConverter* cnv = i->second.converter();
    if ( 0 != cnv )   {
      if (cnv->setDataProvider(m_dataSvc).isFailure()) {
        ATH_MSG_ERROR ("setting Data Provider");
      }
    }
  }
  return StatusCode::SUCCESS;
}

/** Implementation of IConverter: Get Data provider service
    @return    Pointer to data provider service
*/
SmartIF<IDataProviderSvc>&
AthCnvSvc::dataProvider()  const
{
  // FIXME: Ideally the return type should be const, but that would require
  //        changes in the Gaudi IConverter base class.
  SmartIF<IDataProviderSvc>& svc ATLAS_THREAD_SAFE =
    const_cast<SmartIF<IDataProviderSvc>&>(m_dataSvc);
  return svc;
}

/// Implementation of IConverter: 
/// Set conversion service the converter is connected to
StatusCode
AthCnvSvc::setConversionSvc(IConversionSvc* /*svc*/)
{
  return StatusCode::FAILURE;
}

/// Implementation of IConverter:
/// Get conversion service the converter is connected to
SmartIF<IConversionSvc>&
AthCnvSvc::conversionSvc() const
{
  // FIXME: Ideally the return type should be const, but that would require
  //        changes in the Gaudi IConverter base class.
  SmartIF<IConversionSvc>& svc ATLAS_THREAD_SAFE =
    const_cast<SmartIF<IConversionSvc>&>(m_cnvSvc);
  return svc;
}

/// Set address creator facility
StatusCode
AthCnvSvc::setAddressCreator(IAddressCreator* creator)
{
  m_addressCreator = creator;
  Workers::iterator stop  = m_workers.end();
  Workers::iterator start = m_workers.begin();
  for(Workers::iterator i=start; i != stop; i++ )    {
    IConverter* cnv = i->second.converter();
    if ( 0 != cnv )   {
      if (cnv->setAddressCreator(m_addressCreator).isFailure()) {
        ATH_MSG_ERROR ("setting Address Creator");
      }
    }
  }
  return StatusCode::SUCCESS;
}

/// Retrieve address creator facility
SmartIF<IAddressCreator>&
AthCnvSvc::addressCreator() const
{
  // FIXME: Ideally the return type should be const, but that would require
  //        changes in the Gaudi IConverter base class.
  SmartIF<IAddressCreator>& svc ATLAS_THREAD_SAFE =
    const_cast<SmartIF<IAddressCreator>&>(m_addressCreator);
  return svc;
}

/// Implementation of IConverter:
///  Create the transient representation of an object.
StatusCode
AthCnvSvc::createObj(IOpaqueAddress* pAddress,DataObject*& refpObject)
{
  return makeCall (CREATE_OBJ, false, true, false, pAddress, refpObject);
}

/// Implementation of IConverter:
/// Resolve the references of the created transient object.
StatusCode
AthCnvSvc::fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject)
{
  return makeCall (FILL_OBJ_REFS, false, true, true, pAddress, pObject);
}

/// Implementation of IConverter:
///  Update the transient object from the other representation.
StatusCode
AthCnvSvc::updateObj(IOpaqueAddress* pAddress, DataObject* pObject)
{
  return makeCall (UPDATE_OBJ, false, true, false, pAddress, pObject);
}

/// Implementation of IConverter: 
///  Update the references of an updated transient object.
StatusCode
AthCnvSvc::updateObjRefs(IOpaqueAddress* pAddress, DataObject* pObject)
{
  return makeCall (UPDATE_OBJ_REFS, false, true, true, pAddress, pObject);
}

/// Implementation of IConverter: 
///  Convert the transient object to the requested representation.
StatusCode
AthCnvSvc::createRep(DataObject* pObject, IOpaqueAddress*& refpAddress)
{
  return makeCall (CREATE_REP, true, false, false, refpAddress, pObject);
}

/// Implementation of IConverter:
///  Resolve the references of the converted object. 
StatusCode
AthCnvSvc::fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject)
{
  return makeCall (FILL_REP_REFS, true, false, false, pAddress, pObject);
}

/// Implementation of IConverter: 
///  Update the converted representation of a transient object.
StatusCode
AthCnvSvc::updateRep(IOpaqueAddress* pAddress, DataObject* pObject)
{
  return makeCall (UPDATE_REP, true, false, false, pAddress, pObject);
}

/// Implementation of IConverter: 
///  Update the references of an already converted object.
StatusCode
AthCnvSvc::updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject)
{
  return makeCall (UPDATE_REP_REFS, true, false, false, pAddress, pObject);
}

/// Add converter object to conversion service.
StatusCode
AthCnvSvc::addConverter(const CLID& clid)
{
  // First look for the more specific converter
  long typ = repSvcType();
  IConverter* pConverter = createConverter(typ, clid, 0);
  if ( 0 != pConverter )    {
    StatusCode status = configureConverter( typ, clid, pConverter );
    if ( status.isSuccess() )   {
      status = initializeConverter( typ, clid, pConverter );
      if ( status.isSuccess() )   {
        status = activateConverter( typ, clid, pConverter );
        if ( status.isSuccess() )   {
          long conv_typ  = pConverter->repSvcType();
          const CLID&   conv_clid = pConverter->objType();
          typ      = (typ<0xFF) ? typ : typ&0xFFFFFF00;
          conv_typ = (conv_typ<0xFF) ? conv_typ : conv_typ&0xFFFFFF00;
          if ( conv_typ == typ && conv_clid == clid )   {
            return addConverter(pConverter);
          }
        }
      }
    }
    pConverter->release();
  }
  return Status::NO_CONVERTER;
}

/// Add converter object to conversion service.
StatusCode
AthCnvSvc::addConverter(IConverter* pConverter)
{
  if ( 0 != pConverter )    {
    const CLID& clid = pConverter->objType();
    removeConverter(clid).ignore();
    m_workers.insert (std::make_pair(clid,WorkerEntry(clid, pConverter)));
    pConverter->addRef();
    return StatusCode::SUCCESS;
  }
  return Status::NO_CONVERTER;
}

/// Remove converter object from conversion service (if present).
StatusCode
AthCnvSvc::removeConverter(const CLID& clid)
{
  Workers::iterator worker = m_workers.find (clid);
  if ( worker != m_workers.end() ) {
    worker->second.converter()->finalize().ignore();
    worker->second.converter()->release();

    m_workers.erase (worker);
    return StatusCode::SUCCESS;
  }
  return Status::NO_CONVERTER;
}

/// Retrieve converter from list
IConverter*
AthCnvSvc::converter(const CLID& clid)
{
  std::lock_guard<CallMutex> lock(m_conv_mut);
  Workers::iterator worker = m_workers.find (clid);
  if ( worker != m_workers.end() ) {
    return worker->second.converter();
  }
  if (addConverter (clid).isSuccess()) {
    worker = m_workers.find (clid);
    if (worker != m_workers.end()) {
      return worker->second.converter();
    }
  } else {
    ATH_MSG_INFO("problem adding converter for CLID [" << clid << "]");
  }
  return 0;
}

/// Connect the output file to the service with open mode.
StatusCode
AthCnvSvc::connectOutput (const std::string& outputFile,
                          const std::string& /*openMode*/)
{
  return connectOutput (outputFile);
}

/// Connect the output file to the service.
StatusCode
AthCnvSvc::connectOutput(const std::string& /*output*/)
{
  return StatusCode::SUCCESS;
}

/// Commit pending output.
StatusCode
AthCnvSvc::commitOutput(const std::string& /*output*/, bool /*do_commit*/)
{
  return StatusCode::SUCCESS;
}
//@}

/// Disconnect output files from the service.
StatusCode
AthCnvSvc::disconnectOutput()
{
  return StatusCode::SUCCESS;
}

/// @name @c IAddressCreator interface implementation
//@{
/// Create a Generic address using explicit arguments to identify a 
/// single object.
StatusCode
AthCnvSvc::createAddress (long /*svc_type*/,
                          const CLID& /*clid*/,
                          const std::string* /*par*/, 
                          const unsigned long* /*ip*/,
                          IOpaqueAddress*& refpAddress)
{
  refpAddress = 0;
  return StatusCode::FAILURE;
}

/// Convert an address to string form
StatusCode
AthCnvSvc::convertAddress (const IOpaqueAddress* /*pAddress*/,
			   std::string& refAddress)
{
  refAddress = "";
  return StatusCode::FAILURE;
}

/// Convert an address in string form to object form
StatusCode
AthCnvSvc::createAddress (long /*svc_type*/, 
                          const CLID& /*clid*/,
                          const std::string& /*refAddress*/,
                          IOpaqueAddress*& refpAddress)
{
  refpAddress = 0;
  return StatusCode::FAILURE;
}

/// Update state of the service
StatusCode
AthCnvSvc::updateServiceState(IOpaqueAddress* /*pAddress*/)
{
  return StatusCode::SUCCESS;
}
//@}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/// Create new Converter using factory
IConverter*
AthCnvSvc::createConverter (long typ,
                            const CLID& clid, 
                            const ICnvFactory* /*fac*/)
{

  std::unique_ptr<IConverter> cnv{Converter::Factory::create
    (ConverterID(typ,clid), serviceLocator().get() )};

  if (!cnv) {
    typ = (typ<0xFF) ? typ : typ&0xFFFFFF00;
    cnv = Converter::Factory::create
      (ConverterID(typ,clid), serviceLocator().get() );
  }
  return cnv.release();

}

/// Configure the new converter before initialize is called
StatusCode
AthCnvSvc::configureConverter (long /*typ*/, const CLID& /*clid*/, 
                               IConverter* cnv)
{
  if (cnv) {
    cnv->setConversionSvc(this).ignore();
    cnv->setAddressCreator(m_addressCreator).ignore();
    cnv->setDataProvider(m_dataSvc).ignore();
    return StatusCode::SUCCESS;
  }
  return Status::NO_CONVERTER;

}

/// Initialize the new converter
StatusCode
AthCnvSvc::initializeConverter (long /*typ*/, const CLID& /*clid*/, 
                                IConverter* cnv)
{
  if (cnv) {
    return cnv->initialize();
  }
  return Status::NO_CONVERTER;
}

/// Activate the new converter after initialization
StatusCode
AthCnvSvc::activateConverter (long /*typ*/, const CLID& /*clid*/,
                              IConverter* cnv)
{
  if (cnv) {
    return StatusCode::SUCCESS;
  }
  return Status::NO_CONVERTER;
}

/// Load converter or dictionary needed by the converter
void
AthCnvSvc::loadConverter(DataObject* /*pObject*/)
{
}


StatusCode 
AthCnvSvc::makeCall (int typ,
                     bool ignore_add,
                     bool ignore_obj,
                     bool update,
                     IOpaqueAddress*& pAddress, 
                     DataObject*& pObject)      
{
  if ( 0 != pAddress || ignore_add )    {
    if ( 0 != pObject  || ignore_obj )    {
      const CLID&  obj_class =
        (0 != pObject && !ignore_obj) ? pObject->clID() 
        : (0 != pAddress && !ignore_add)
        ? pAddress->clID()                       
        : CLID_NULL;
      IConverter*  cnv  = converter(obj_class);
      if ( !cnv && pObject ) {
        //Give it a try to autoload the class (dictionary) for which the converter is needed
        loadConverter( pObject);
        cnv  = converter(obj_class);
      }
      
      StatusCode status(StatusCode::FAILURE,true);
      if ( 0 != cnv )   {
        switch(typ)   {
        case CREATE_OBJ:
          pObject = 0;
          status = cnv->createObj(pAddress, pObject);
          break;
        case FILL_OBJ_REFS:
          status = cnv->fillObjRefs(pAddress, pObject);
          break;
        case UPDATE_OBJ:
          status = cnv->updateObj(pAddress, pObject);
          break;
        case UPDATE_OBJ_REFS:
          status = cnv->updateObjRefs(pAddress, pObject);
          break;
        case CREATE_REP:
          pAddress = 0;
          status = cnv->createRep(pObject, pAddress);
          break;
        case FILL_REP_REFS:
          status = cnv->fillRepRefs(pAddress, pObject);
          break;
        case UPDATE_REP:
          status = cnv->updateRep(pAddress, pObject);
          break;
        case UPDATE_REP_REFS:
          status = cnv->updateRepRefs(pAddress, pObject);
          break;
        default:
          status = StatusCode::FAILURE;
          break;
        }
        if ( status.isSuccess() && update )   {
          status = updateServiceState(pAddress);
        }
        return status;
      }
      status.ignore();
      msg(MSG::INFO) << "No converter for object ";
      if ( pObject != 0 )   {
        msg(MSG::INFO) << System::typeinfoName(typeid(*pObject));
      }
      msg(MSG::INFO) << "  CLID= " << obj_class << endmsg;
      return Status::NO_CONVERTER;
    }
    return Status::INVALID_OBJECT;
  }
  return Status::INVALID_ADDRESS;
}

