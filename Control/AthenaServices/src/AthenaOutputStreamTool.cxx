/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @date   Tue Feb 18 16:37:10 2003
 * 
 * @brief Tool to stream out objects
 * 
 * $Id: AthenaOutputStreamTool.cxx,v 1.24 2008-09-23 22:00:47 binet Exp $
 */


// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "AthenaKernel/IClassIDSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/SGIFolder.h"

#include "AthenaOutputStreamTool.h"

/// Constructor
AthenaOutputStreamTool::AthenaOutputStreamTool(const std::string& type, 
					       const std::string& name, 
					       const IInterface* parent)
	:
        AthAlgTool(type, name, parent),
	m_store   ( "DetectorStore",    name ),
	m_cnvSvc  ( "AthenaPoolCnvSvc", name ),
	m_clidSvc ( "ClassIDSvc",       name ),
	m_connectionOpen(false),
	m_extendProvenanceRecord(false)
{
    // Declare IAthenaOutputStreamTool interface
    declareInterface<IAthenaOutputStreamTool>(this);

    // Properties
    declareProperty("OutputFile",       m_outputName="");
    declareProperty("CnvSvc", 
		    m_cnvSvc = ICnvSvc_t( "AthenaPoolCnvSvc", name ) );

    declareProperty( "Store", 
		     m_store = StoreGateSvc_t( "DetectorStore", name ) );

//    declareProperty("ProcessingTag",    m_processTag=m_dataHeaderKey);
}

/// Destructor
AthenaOutputStreamTool::~AthenaOutputStreamTool()
{}


/// Initialize 
StatusCode
AthenaOutputStreamTool::initialize()
{
    ATH_MSG_INFO ("AthenaOutputStreamTool initialize");


    // Get the ClassIDSvc - to get typename for clid
    StatusCode sc = m_clidSvc.retrieve(); 
    if (sc != StatusCode::SUCCESS ) {
        ATH_MSG_ERROR (" Cannot get IClassIDSvc interface of the CLIDSvc ");
	return sc ;
    }
    ATH_MSG_DEBUG ("Found CLIDSvc ");

    return (StatusCode::SUCCESS);
}


/// Finalize 
StatusCode 
AthenaOutputStreamTool::finalize()
{
    ATH_MSG_INFO ("AthenaOutputStreamTool finalize");
    return (StatusCode::SUCCESS);
}

/// Specify which data store and conversion service to use
StatusCode 
AthenaOutputStreamTool::connectServices(const std::string& dataStore, 
					const std::string& cnvSvc,
					bool extendProvenenceRecord)
{
    StatusCode status;

    ATH_MSG_DEBUG ("In connectServices ");

    m_store  = StoreGateSvc_t( dataStore, this->name() );
    m_cnvSvc = ICnvSvc_t     ( cnvSvc,    this->name() );
    m_extendProvenanceRecord = extendProvenenceRecord;
    
    status = connectServices();
    return (status);
}


/// Specify which data store and conversion service to use
StatusCode 
AthenaOutputStreamTool::connectServices()
{

    StatusCode status;

    ATH_MSG_DEBUG ("In connectServices ");

    // Find the data store
    status = m_store.retrieve();
    if( !status.isSuccess() ) {
        ATH_MSG_ERROR ("Could not locate " << m_store.type() << " store");
	return status;
    }
    else {
        ATH_MSG_DEBUG ("Found " << m_store.type() << " store.");
    }
    if ( !m_store ) {
	ATH_MSG_ERROR ("Invalid pointer to " << m_store.type() << " store");
    }

    // Find the conversion service
    status = m_cnvSvc.retrieve();
    if( !status.isSuccess() ) {
      ATH_MSG_ERROR 
	("Unable to locate IConversionSvc interface of "<< m_cnvSvc.type());
      return status;
    }
    else {
      ATH_MSG_DEBUG 
	("Found " << m_cnvSvc.type() << " conversion service.");
    }

    if ( m_cnvSvc.type() == "AthenaPoolCnvSvc" ) {
      ATH_MSG_ERROR
	("The use of AthenaOutputStreamTool for AthenaPool data is deprecated !"
	 << endreq
	 << "   Please use AthenaPoolOutputStreamTool from OutputStreamAthenaPool instead." 
	 << endreq
	 << "      theApp.Dlls += [ \"OutputStreamAthenaPool\" ]" << endreq
	 << "      StreamExample = Algorithm( \"AthenaOutputStream/\" + streamName )" 
	 << endreq
	 << "      StreamExample.WritingTool = \"AthenaPoolOutputStreamTool\"" 
	 << endreq
	 << "      ..." << endreq
	 << "   Or, try importing 'AthenaPoolCnvSvc.WriteAthenaPool'");
      return (StatusCode::FAILURE);
    }

    return (status);
}

    /// Connect to the output stream
StatusCode 
AthenaOutputStreamTool::connectOutput(const std::string& outputName)
{
    StatusCode status(StatusCode::FAILURE);

    ATH_MSG_DEBUG ("In connectOutput ");

    // Use arg if not empty, save the output name
    if (outputName.size()) m_outputName = outputName;

    if (!m_outputName.size()) {
      ATH_MSG_ERROR ("No OutputName provided ");
      return StatusCode::FAILURE;
    }

    // Connect services if not already available
    if (!m_cnvSvc || !m_store) {
	status = connectServices();
	if( !status.isSuccess() ) {
	  ATH_MSG_ERROR ("Unable to connect services ");
	  return status;
	}
    }

    // Connect the output file to the service
    status = m_cnvSvc->connectOutput(m_outputName);
    if( !status.isSuccess() ) {
      ATH_MSG_ERROR ("Unable to connect output " << m_outputName);
      return status;
    }
    else {
      ATH_MSG_DEBUG ("Connected to " << m_outputName);
    }

    // Set flag that connection is open
    m_connectionOpen = true;

    return (status);
}

    
    /// Commit the output stream after having streamed out objects
StatusCode 
AthenaOutputStreamTool::commitOutput()
{
    StatusCode status;

    ATH_MSG_DEBUG ("In connectOutput ");

    // Connect the output file to the service
    status = m_cnvSvc->commitOutput(m_outputName, true);
    if( !status.isSuccess() )   
    {
      ATH_MSG_ERROR ("Unable to commit output " << m_outputName);
      return status;
    }
    else {
      ATH_MSG_DEBUG ("Committed: " << m_outputName);
    }

    // Set flag that connection is closed
    m_connectionOpen = false;

    return (status);
}

StatusCode AthenaOutputStreamTool::finalizeOutput()
{
    return( StatusCode::SUCCESS);
}


StatusCode 
AthenaOutputStreamTool::streamObjects(const TypeKeyPairs& typeKeys ) 
{

    ATH_MSG_DEBUG (" in streamObjects");

    // Check that a connection has been opened
    if(!m_connectionOpen) {
      ATH_MSG_ERROR
	("Connection NOT open. "
	 "Please open a connection before streaming out objects. ");
      return( StatusCode::FAILURE);
    } 
   
    // Now iterate over the type/key pairs and stream out each object
    std::vector< DataObject* >    dataObjects;
    StatusCode sc;
    TypeKeyPairs::const_iterator   first = typeKeys.begin();
    TypeKeyPairs::const_iterator   last  = typeKeys.end();
    for (; first != last; ++first) {
	const std::string& type = (*first).first;
	const std::string& key  = (*first).second;

	// Find the clid for type name from the CLIDSvc
	CLID clid;
	sc = m_clidSvc->getIDOfTypeName(type, clid);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR ("Could not get clid for typeName " << type);
	  return( StatusCode::FAILURE);
	}
	
	DataObject* dObj = 0;
	// Two options: no key or explicit key
	if (key.size() == 0) {
	    // No key

	    ATH_MSG_DEBUG ("Get data object with no key  ");

	    // Get DataObject and stream out
	    dObj = m_store->accessData(clid);
	    if (!dObj) {
		// No object - print warning and return
	      ATH_MSG_DEBUG ("No object found for clid " << clid 
			     << " typeName " << type << " no key ");
	      return (StatusCode::SUCCESS);
	    }
	    else {
	      ATH_MSG_DEBUG ("Found object for clid " << clid 
			     << " typeName " << type << " no key ");
	    }
	}
	else {

	    ATH_MSG_DEBUG ("Get data object with key  ");

	    // Get DataObjects with key
	    dObj = m_store->accessData(clid, key);
	    if (!dObj) {
		// No object - print warning and return
	      ATH_MSG_DEBUG ("No object found for clid " << clid 
			     << " typeName " << type << " key " << key);
	      return (StatusCode::SUCCESS);
	    }
	    else {
	      ATH_MSG_DEBUG ("Found object for clid " << clid 
			     << " typeName " << type << " key " << key);
	    }
	    
	}
	
	// Save the dObj
	dataObjects.push_back(dObj);
    }

    // Stream out objects
    if (dataObjects.size() == 0) {
      ATH_MSG_DEBUG ("No data objects found ");
      return StatusCode::SUCCESS;
    }
    sc = streamObjects(dataObjects);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Could not stream out objects ");
      return( StatusCode::FAILURE);
    }

    return StatusCode::SUCCESS;
}

/// Stream out an object 
///   Must convert to DataObject, e.g.    
///   \#include "SGTools/StorableConversions.h"
///     T* obj = xxx;
///     DataObject* dataObject = SG::asStorable(obj);
StatusCode	
AthenaOutputStreamTool::streamObjects  (const DataObjectVec& dataObjects)
{

    // Check that a connection has been opened
    if(!m_connectionOpen) {
      ATH_MSG_ERROR 
	("Connection NOT open. "
	 "Please open a connection before streaming out objects. ");
	return( StatusCode::FAILURE);
    }

    StatusCode status(StatusCode::SUCCESS);
    std::set<DataObject*> written;
    for (unsigned int i = 0; i < dataObjects.size(); ++i) {
	DataObject* dataObject = dataObjects[i];
	// Do not stream out same object twice
	if (written.find(dataObject) != written.end()) {
	    // print out warning and skip
	    std::string typeName;
	    if (m_clidSvc->getTypeNameOfID(dataObject->clID(), typeName).isFailure()) {
	      ATH_MSG_ERROR ("Could not get typeName for clid " 
			     << dataObject->clID());
	    }
	    ATH_MSG_DEBUG ("Trying to write out DataObject twice (type/key): "
			   << typeName << " " << dataObject->name()
			   << endreq
			   << "    Skipping this one. ");
	    continue;
	}
	else {
	    written.insert(dataObject);
	}
	// Write out object
	IOpaqueAddress* addr(0);
	if ( (m_cnvSvc->createRep(dataObject, addr)).isSuccess() ) {
	    dataObject->registry()->setAddress(addr);
        // SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(dataObject->registry());
        // m_dataHeader->insert(proxy->transientAddress());
	}  else {
	  ATH_MSG_ERROR ("Could not create Rep for DataObject (clid/key):"
			 << dataObject->clID() << " " << dataObject->name());
	    status = StatusCode::RECOVERABLE;
	}
    }

    if ( !fillObjectRefs(dataObjects).isSuccess() ) {
	status = StatusCode::RECOVERABLE;
    }

    return status;
}

/// Fill refs of an object - done as second iteration over
/// objects, after streamObject
StatusCode 
AthenaOutputStreamTool::fillObjectRefs(const DataObjectVec& dataObjects)
{
    StatusCode status = StatusCode::SUCCESS;
    for (unsigned int i = 0; i < dataObjects.size(); ++i) {
	DataObject* dataObject = dataObjects[i];
	// call fillRepRefs of persistency service
	if (! (m_cnvSvc->fillRepRefs( dataObject->registry()->address(), dataObject)).isSuccess() ) status = StatusCode::FAILURE;
    }
    return status;
}

StatusCode AthenaOutputStreamTool::getInputItemList(SG::IFolder* /*pItemList*/)
{
   return StatusCode::SUCCESS;
}
