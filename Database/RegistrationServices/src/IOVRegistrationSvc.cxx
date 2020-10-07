/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IOVRegistrationSvc.cxx 
 * 
 * @brief This is an interface to a tool used to register conditions
 * objects in the Interval of Validity (IOV) database
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: IOVRegistrationSvc.cxx,v 1.36 2009-01-29 16:38:27 hawkings Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "IOVRegistrationSvc.h"

//#include "GaudiKernel/DeclareFactoryEntries.h"

// Athena includes
#include "IOVDbSvc/IIOVCondDbSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "SGTools/TransientAddress.h"

// Gaudi includes
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IConverter.h"

// AttrList and address
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttrListAddress.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollAddress.h"

// COOL includes
//#include "AttributeList/AttributeList.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/Record.h"

//<<<<<< METHOD DEFINITIONS

//--------------------------------------------------------------------------

IOVRegistrationSvc::IOVRegistrationSvc( const std::string& name, ISvcLocator* svc )
    : 
    AthService( name, svc ),
    m_recreateFolders(false),
    m_beginRun(IOVTime::MINRUN),
    m_endRun(IOVTime::MAXRUN),
    m_beginLB(IOVTime::MINEVENT),
    m_endLB(IOVTime::MAXEVENT),
    m_beginTime(IOVTime::MINTIMESTAMP),
    m_endTime(IOVTime::MAXEVENT), // as the time parameter is only 32bit
    m_tag(""),
    m_timeStamp(false),
    m_tagDescription("Athena IOVRegistrationSvc"),
    m_writeKeyInfo(true),
    m_userTags(true),
    m_userTagsUH(false),
    m_svFolder(false),
    m_payloadTable(false),
    m_forceGlobalIOV(false),
    m_iov_db  ( "IOVDbSvc",            name ),
    m_detStore( "DetectorStore",       name ),
    m_persSvc ( "EventPersistencySvc", name ),
    m_clidSvc ( "ClassIDSvc",          name )
{

    // Declare properties
    declareProperty("RecreateFolders", m_recreateFolders);
    declareProperty("BeginRun",        m_beginRun);
    declareProperty("EndRun",          m_endRun);
    declareProperty("BeginLB",         m_beginLB);
    declareProperty("EndLB",           m_endLB);
    declareProperty("BeginTime",       m_beginTime);
    declareProperty("EndTime",         m_endTime);
    declareProperty("IOVDbTag",        m_tag);
    declareProperty("IOVDbTimeStamp",  m_timeStamp);
    declareProperty("TagDescription",  m_tagDescription);
    declareProperty("writeKeyInfo",    m_writeKeyInfo);
    declareProperty("userTags",        m_userTags);
    declareProperty("userTagsUpdateHead",m_userTagsUH);
    declareProperty("SVFolder",        m_svFolder);
    declareProperty("PayloadTable",    m_payloadTable);
    declareProperty("OverrideNames",   m_overrideName);
    declareProperty("OverrideTypes",   m_overrideType);
    declareProperty("UseGlobalIOVForCollections", m_forceGlobalIOV);
}

//--------------------------------------------------------------------------
IOVRegistrationSvc::~IOVRegistrationSvc()
{}

const InterfaceID& IOVRegistrationSvc::type() const

{ 
    return IIOVRegistrationSvc::interfaceID();
}

//--------------------------------------------------------------------------
/// Identify interfaces to which this service is responsive
StatusCode
IOVRegistrationSvc::queryInterface( const InterfaceID& riid, void** ppvInterface ) 
{
    ATH_MSG_DEBUG ("in queryInterface()");

    if ( IIOVRegistrationSvc::interfaceID().versionMatch(riid) ) {
        ATH_MSG_DEBUG ("matched IIOVRegistrationSvc");
	*ppvInterface = (IIOVRegistrationSvc*)this;
    }
    else {
	// Interface is not directly available: try out a base class
        ATH_MSG_DEBUG ("Default to Service interface");
	return AthService::queryInterface(riid, ppvInterface);
    }

    return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::initialize()
{
    ATH_MSG_DEBUG ("in initialize()");

    StatusCode sc = AthService::initialize();
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR ("Unable to call base initialize method");
      return StatusCode::FAILURE;
    }

    // locate the conditions store ptr to it.
    sc = m_detStore.retrieve();
    if (!sc.isSuccess() || 0 == m_detStore)  {
       ATH_MSG_ERROR ("Could not find ConditionStore");
       return StatusCode::FAILURE;
    }

    // Get the IOVDbSvc
    sc = m_iov_db.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR ("Unable to get the IOVDbSvc");
      return StatusCode::FAILURE;
    }

    // Get the persistency mgr for conversion of IOpaqueAddress to
    // string
    sc = m_persSvc.retrieve(); 
    if ( sc != StatusCode::SUCCESS ) {
        ATH_MSG_ERROR (" Cannot get IAddressCreator interface of the EventPersistencySvc ");
	return sc ;
    }
    ATH_MSG_DEBUG ("Found PersistencySvc ");

    
    // Get the ClassIDSvc - to get typename for clid
    sc = m_clidSvc.retrieve(); 
    if (sc != StatusCode::SUCCESS ) {
        ATH_MSG_ERROR (" Cannot get IClassIDSvc interface of the CLIDSvc " );
	return sc ;
    }
    ATH_MSG_DEBUG ("Found CLIDSvc ");


    ATH_MSG_DEBUG ("Properties ");
    ATH_MSG_DEBUG ("RecreateFolders " <<  m_recreateFolders);
    if (m_timeStamp)
      {
	ATH_MSG_DEBUG ("BeginTime      " <<  m_beginTime);
	ATH_MSG_DEBUG ("EndTime        " <<  m_endTime);
      }
    else
      {
	ATH_MSG_DEBUG ("BeginRun        " <<  m_beginRun);
	ATH_MSG_DEBUG ("EndRun          " <<  m_endRun);
	ATH_MSG_DEBUG ("BeginLB      " <<  m_beginLB);
	ATH_MSG_DEBUG ("EndLB        " <<  m_endLB);
      }
    ATH_MSG_DEBUG ("IOVDbTag        " <<  m_tag);

    // check consistency of override specification, if any
    if (m_overrideName.size()!=m_overrideType.size()) {
      ATH_MSG_FATAL ("Inconsistent settings of OverrideNames and OverrideTypes parameters");
      return StatusCode::FAILURE;
    }
    for (unsigned int i=0;i<m_overrideName.size();++i)
      ATH_MSG_INFO ("Attributes with name " << m_overrideName[i] <<
                    " will be stored as COOL type " << m_overrideType[i]);
    return StatusCode::SUCCESS;

}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::finalize()
{
    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName ) const
{
  std::string key = "";
  
  if (m_timeStamp)
    {
      IOVTime start;
      start.setTimestamp(timeToNano(m_beginTime));
      IOVTime stop ;
      stop.setTimestamp(timeToNano(m_endTime));

      return ( registerIOV( typeName,
			key,key,
			m_tag.value(),
			start, 
			stop) );
    }
  else
    {
      IOVTime start;
      start.setRunEvent( (unsigned long)m_beginRun, (unsigned long)m_beginLB );
      IOVTime stop;
      stop.setRunEvent ( (unsigned long)m_endRun,   (unsigned long)m_endLB );

      return ( registerIOV( typeName,
			key,key,
			m_tag.value(),
			start, 
			stop) );
    }
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, const std::string& tag ) const
{
  std::string key = "";
  
  if (m_timeStamp)
    {
      IOVTime start;
      start.setTimestamp( timeToNano(m_beginTime) );
      IOVTime stop ;
      stop.setTimestamp ( timeToNano(m_endTime) );   
	      
      return ( registerIOV( typeName,
			key,key,
			tag,
			start, 
			stop) );
    }
  else
    {
      IOVTime start;
      start.setRunEvent( (unsigned long)m_beginRun, (unsigned long)m_beginLB );
      IOVTime stop;
      stop.setRunEvent ( (unsigned long)m_endRun,   (unsigned long)m_endLB );
      
      return ( registerIOV( typeName,
			key,key,
			tag,
			start, 
			stop) );
    }
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, const std::string& key,
				 const std::string& tag ) const
{
  if (m_timeStamp)
    {
      IOVTime start;
      start.setTimestamp( timeToNano(m_beginTime) );
      IOVTime stop ;
      stop.setTimestamp ( timeToNano(m_endTime) );   
	      
      return ( registerIOV( typeName,
			key,key,
			tag,
			start, 
			stop) );
    }
  else
    {
      IOVTime start;
      start.setRunEvent( (unsigned long)m_beginRun, (unsigned long)m_beginLB );
      IOVTime stop;
      stop.setRunEvent ( (unsigned long)m_endRun,   (unsigned long)m_endLB );
      
      return ( registerIOV( typeName,
			key,key,
			tag,
			start, 
			stop) );
    }
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, 
				 const std::string& tag,
				 unsigned int beginRun, 
				 unsigned int endRun, 
				 unsigned int beginLB, 
				 unsigned int endLB ) const
{
  IOVTime start;
  start.setRunEvent( (unsigned long)beginRun, (unsigned long)beginLB );
  IOVTime stop;
  stop.setRunEvent ( (unsigned long)endRun,   (unsigned long)endLB );
  std::string key = "";
  
  return ( registerIOV( typeName,
		    key,key,
		    tag,
		    start, 
		    stop) );
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, 
					    const std::string& tag,
					    uint64_t beginTime, 
					    uint64_t endTime ) const
{
  IOVTime start;
  start.setTimestamp( beginTime );
  IOVTime stop;
  stop.setTimestamp ( endTime );

  std::string key = "";
  
  return ( registerIOV( typeName,
		    key,key,
		    tag,
		    start, 
		    stop) );
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, 
				 const std::string& key,
				 const std::string& tag,
				 unsigned int beginRun, 
				 unsigned int endRun, 
				 unsigned int beginLB, 
				 unsigned int endLB ) const
{
  IOVTime start;
  start.setRunEvent( (unsigned long)beginRun, (unsigned long)beginLB );
  IOVTime stop;
  stop.setRunEvent ( (unsigned long)endRun,   (unsigned long)endLB );

  return ( registerIOV( typeName,
		    key,key,
		    tag,
		    start, 
		    stop) );
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, 
					    const std::string& key,
					    const std::string& tag,
					    uint64_t beginTime, 
					    uint64_t endTime ) const
{
  IOVTime start;
  start.setTimestamp( beginTime );
  IOVTime stop;
  stop.setTimestamp ( endTime );

  return ( registerIOV( typeName,
	            key,key,
		    tag,
		    start, 
		    stop) );
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, 
				 const std::string& key,
			         const std::string& folder,
				 const std::string& tag,
				 unsigned int beginRun, 
				 unsigned int endRun, 
				 unsigned int beginLB, 
				 unsigned int endLB ) const
{
  IOVTime start;
  start.setRunEvent( (unsigned long)beginRun, (unsigned long)beginLB );
  IOVTime stop;
  stop.setRunEvent ( (unsigned long)endRun,   (unsigned long)endLB );

  return ( registerIOV( typeName,
		    key,
		    folder,
		    tag,
		    start, 
		    stop) );
}

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName, 
					    const std::string& key,
					    const std::string& folder,
					    const std::string& tag,
					    uint64_t beginTime, 
					    uint64_t endTime ) const
{
  IOVTime start;
  start.setTimestamp( beginTime );
  IOVTime stop;
  stop.setTimestamp ( endTime );

  return ( registerIOV( typeName,
		    key,
                    folder,
		    tag,
		    start, 
		    stop) );
}

///////////////////////////////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOV( const std::string& typeName,
				 const std::string& spec_key,
				 const std::string& folder,
				 const std::string& tag,
				 const IOVTime&     start, 
				 const IOVTime&     stop ) const
{
    // Register the conditions objects in the IOV database with
    // start/stop as the time interval


    msg() << MSG::DEBUG <<" in registerIOV()" 
          << " typename: "     << typeName << " - tag: " << tag;
    if (spec_key.empty())
      {
	msg() << " key: *empty* " << endmsg;
      }
    else
      {
	msg() << " spec_key " << spec_key << endmsg;
      }
    msg() << " - begin time: "   << start
          << " - end time: "     << stop
          << endmsg;
  
    // Check validity of start/stop
    if(start.isBoth() || stop.isBoth() || 
       start.isTimestamp() != stop.isTimestamp() ||
       start.isRunEvent() != stop.isRunEvent()) {
        ATH_MSG_ERROR ("Incorrect start/stop: " 
                       << " isBoth: " << start.isBoth() << ":" << stop.isBoth() 
                       << " isTimestamp: " << start.isTimestamp() << ":" << stop.isTimestamp() 
                       << " isRunEvent: " << start.isRunEvent() << ":" << stop.isRunEvent());
	return( StatusCode::FAILURE);
    }
    
    return registerIOVCOOL( typeName,
				spec_key,
				folder,
				tag,
				start, 
				stop );
}

    

//--------------------------------------------------------------------------

StatusCode IOVRegistrationSvc::registerIOVCOOL( const std::string& typeName,
						const std::string& spec_key,
						const std::string& folderName,
						const std::string& tag,
						const IOVTime&     start, 
						const IOVTime&     stop ) const
{
    // Register the conditions objects in the IOV database with
    // start/stop as the time interval


    ATH_MSG_DEBUG (" in registerIOVCOOL()" );
  

    // Find the clid for type name from the CLIDSvc
    CLID clid;
    StatusCode sc = m_clidSvc->getIDOfTypeName(typeName, clid);
    if (sc.isFailure()) {
        ATH_MSG_ERROR ("Could not get clid for typeName " << typeName);
	return( StatusCode::FAILURE);
    }

    try {

	// There are two possible states:
	//   1) The DataObject is stored elsewhere and we store in the
	//      IOVDb only a stringified ref to the object, or
	//   2) The DataObject is an AthenaAttributeList or a
	//      CondAttrListCollection and then we store it directly into
	//      the IOVDb. 
	//
	//   The second case is detected either with the type name,
	//   for AthenaAttributeList or CondAttrListCollection, or by
	//   checking the real type of the IOA - i.e. if it is a
	//   CondAttrListCollAddress then a CondAttrListCollection may
	//   have been filled with pool refs stored in it.

	bool storeRef          = true;
	bool storeAttrListColl = false;
	bool needSGaddr=false;
	if ("AthenaAttributeList" == typeName) {
	  storeRef = false;
	  needSGaddr=false;
	}
	if ("CondAttrListCollection" == typeName) {
	    storeRef           = false;
	    storeAttrListColl  = true;
	    needSGaddr=false;
	}
	// (See after retrieval of the IOA for a final check of the
	// type of storage.)

	// Get IOpaqueAddress, key and symlinks for each data object
	// from StoreGate
	IOpaqueAddress* addr=0;
	std::string key = spec_key; 
	std::vector<CLID> symlinks;
	SG::DataProxy* proxy;
	if (key.empty()) {
	    // Get IOpaqueAddress and key for each data object from
	    // StoreGate
	    proxy = m_detStore->proxy(clid);
	    if (!proxy) {
	      ATH_MSG_ERROR ("Could not get proxy for clid " << clid);
	      return( StatusCode::FAILURE);
	    }
	    
	    // Get key to be used for folder name
	    key = proxy->name();
	    // get proxy address - this will fail if object not streamed out
	    // but this can be recovered later for inline data
	    addr = proxy->address();
	} else {
	    // Get IOpaqueAddress for each data object from StoreGate
	    proxy = m_detStore->proxy(clid, key);
	    if (!proxy) {
	      ATH_MSG_ERROR ("Could not get proxy for clid " << clid << " and key " << key);
	      return( StatusCode::FAILURE );
	    }
	    // get proxy address - this will fail if object not streamed out
	    // but this can be recovered later for inline data
	    addr = proxy->address();
	}
	std::string saddr;
	if (addr) {
  	  // Get symlinks, if any
          symlinks = proxy->transientID();
          auto it = std::find (symlinks.begin(), symlinks.end(), clid);
          if (it != symlinks.end()) {
            symlinks.erase (it);
          }

	  // Check whether the IOA is a CondAttrListCollAddress - if so
	  // we will store a CondAttrListCollection
	  CondAttrListCollAddress* collAddr = dynamic_cast<CondAttrListCollAddress*>(addr);
	  if (collAddr) {
	    storeRef           = false;
	    storeAttrListColl  = true;
	  }
  	  // Convert IOpaqueAddress to string via the persistency
	  // service. We then store the string address in the IOV DB
	  sc = m_persSvc->convertAddress(addr, saddr);   
	  if (sc.isFailure()) {
	    ATH_MSG_WARNING ("Could not get string from IOpaqueAddress for clid " << clid
                         << " is BAD_STORAGE_TYPE: " << (sc == IConversionSvc::Status::BAD_STORAGE_TYPE));
	  return( StatusCode::FAILURE);
	  }
	  ATH_MSG_DEBUG ("String address = \"" << saddr << "\"");
	} else {
	  // if no addr was found, object has not been streamed out
	  // this is OK providing we do not need the addr later
	  // i.e. plain CondAttrListCollection or AthenaAttributeList
	  if (needSGaddr) {
	    ATH_MSG_ERROR ("Could not get address for clid " << clid);
	    return( StatusCode::FAILURE );
	  } else {
	    ATH_MSG_DEBUG ("Faking address for " << typeName);
	    // fake the saddr contents 
 	    if ("AthenaAttributeList" == typeName) {
	      saddr="<address_header service_type=\"256\" clid=\"40774348\" /> POOLContainer_AthenaAttributeList][CLID=x";
	    } else if ("CondAttrListCollection" == typeName) {
	      saddr="<address_header service_type=\"256\" clid=\"1238547719\" /> POOLContainer_CondAttrListCollection][CLID=x";
	    } else {
	      ATH_MSG_ERROR ("Cannot fake stringaddress for typename "
                             << typeName);
	    }
 	  }
	}

	ATH_MSG_DEBUG ("Storing ref: " << storeRef 
                       << " Storing AttrListCollection: " << storeAttrListColl);

	// Set folder name - in the present case this is defined
	// to be the key 
	// RJH - unless a non-null folder is given on input
	std::string local_folder;
	if (""==folderName) {
  	  local_folder = key;
	} else {
	  local_folder=folderName;
	}
	ATH_MSG_DEBUG ("Using folder name " << local_folder);
	  
	// Get COOL database in update mode (false == readOnly flag)
	cool::IDatabasePtr db = m_iov_db->getDatabase(false); 
	if (!db) {
            ATH_MSG_ERROR ("Could not get pointer to COOL db ");
	    return(StatusCode::FAILURE);       
	}
	// get some information about the database connection for later checks
	const cool::DatabaseId& dbid=db->databaseId();
	// look for direct connection to production accounts
	bool dbidprod=(dbid.find("oracle")!=std::string::npos && (
		       dbid.find("ATLAS_COOLONL_")!=std::string::npos ||
		       dbid.find("ATLAS_COOLOFL_")!=std::string::npos ||
		       dbid.find("ATLAS_COOL_")!=std::string::npos));
	// look for use of writer account ('_W' in connection string)
	bool dbidwriter=(dbid.find("oracle")!=std::string::npos && 
			 dbid.find("_W")!=std::string::npos);
	ATH_MSG_DEBUG ("Identified prod/writer " << dbidprod << dbidwriter);
	// do not allow write accesss to production servers
	if (dbidprod) {
	  ATH_MSG_FATAL ("Direct update of production Oracle servers from Athena is FORBIDDEN");
	  ATH_MSG_FATAL ("Please write to SQLite file and then merge with AtlCoolMerge.py");
	  return StatusCode::FAILURE;
	}
	
	// For AthenaAttributeList/CondAttrListCollection, save a pointer to the object
	const AthenaAttributeList*    attrList     = 0;
	const CondAttrListCollection* attrListColl = 0;

	if (!storeRef) {
	    if ("CondAttrListCollection"==typeName) {
	      if (StatusCode::SUCCESS!=m_detStore->
		  retrieve(attrListColl,key)) {
                ATH_MSG_ERROR ("Could not find CondAttrListCollecton for "
                               << key);
		return StatusCode::FAILURE;
	      }
	    } else if (storeAttrListColl) {
	      // have to go through SG proxy, as it is a collection of POOLref
	      if (addr) {
		CondAttrListCollAddress* attrAddr = dynamic_cast<CondAttrListCollAddress*>(addr);
		if (attrAddr) {
		    // Successful cast
		    attrListColl = attrAddr->attrListColl();
		    ATH_MSG_DEBUG ("Set attr list coll ptr ");
		    ATH_MSG_DEBUG ("addr, attrAddr, coll " 
                                   << addr << " " << attrAddr << " " << attrListColl);
		} else {
		  ATH_MSG_ERROR ("Could not extract ptr for CondAttrListCollAddress ");
		  return StatusCode::FAILURE;
		}
	      } else {
		ATH_MSG_ERROR ("Cannot write out collection of POOLref without streaming them first" );
		return StatusCode::FAILURE;
	      }
	    } else {
	      // Just AttrList - get directly from Storegate
	      if (StatusCode::SUCCESS!=m_detStore->retrieve(attrList,key)) {
		ATH_MSG_ERROR ("Could not find AthenaAttributeList for "
                               << key);
		return StatusCode::FAILURE;
	      }
	    }
	    
	}


	ATH_MSG_DEBUG ("Set attr list coll ptr " << attrListColl);

	// Save folder pointer
	cool::IFolderPtr folder;

	// Create folders if required - a job option allows on to
	// delete existing folders and then recreate them

	bool createFolders = false;

	if(db->existsFolder(local_folder)) {

	    if (m_recreateFolders) {
  	        // do not allow this action on production schema or with writer
	        if (dbidprod || dbidwriter) {
		  ATH_MSG_FATAL ("Apparent attempt to delete folder on production COOL schema " << dbid);
		  return StatusCode::FAILURE;
	        }
	    
		ATH_MSG_DEBUG (" Deleting existing COOL Folder " << local_folder);
		db->dropNode( local_folder ); 
		createFolders = true;
	    }
	    else {
		// Get folder
                ATH_MSG_DEBUG ("COOL Folder " << local_folder << " exists");
		folder = db->getFolder(local_folder);
	    }

	}
	else {
            ATH_MSG_DEBUG ("COOL Folder " << local_folder 
                           << " does not exist - must create it");
	    createFolders = true;
	}
	    
	// Split the string address into header and data parts
	std::string address_header;
	std::string address_data;

	sc = splitAddress(saddr,address_header,address_data);
	if (sc.isFailure()) {
            ATH_MSG_ERROR ("Could not split address: "
                           << "addr: " << saddr << "\n"
                           << "hdr:  " << address_header << "\n"
                           << "data  " << address_data);
	    return( StatusCode::FAILURE);
	}
	msg() << MSG::DEBUG <<"split address: " << saddr << endmsg
              << "  hdr:  " << address_header << endmsg
              << "  data: " << address_data << endmsg;
				

	if(createFolders) {
  	    // first make sure not using writer account -if so abort
	    if (dbidwriter) {
	      ATH_MSG_FATAL ("Apparent attempt to create folder using writer account, dbID is: " << dbid);
	      return StatusCode::FAILURE;
	    }

	    // We store extra information in the folder description. 
	    // This info is:
	    //   typeName       - required information
	    //   symlinks       - the extra StoreGate keys, if any
	    //   key            - optional, only needed if key != folder name
	    //   timeStamp      - either run-lumi (default) or time
	    //   address_header - added by convention
	    //
	    //
	    // The convention is that the address_header is stored
	    // in the description, and the IOV interval data
	    // payload is just the pool reference in string form.
	    // The address_header and address_data can be obtained
	    // from the string address returned from the
	    // persistency service, using splitAddress

	    std::string mergedNames;

	    sc = buildDescription( "typeName", typeName,
					     mergedNames );
	    if (sc.isFailure()) {
                ATH_MSG_ERROR ("Could not merge towards merged description: "
                               << "typeName: " << typeName);
		return( StatusCode::FAILURE);
	    }
		
	    sc = buildDescription( "addrHeader", address_header,
					     mergedNames );
	    if (sc.isFailure()) {
                ATH_MSG_ERROR ("Could not merge towards merged description: "
                               << "addrHeader: " << address_header);
		return( StatusCode::FAILURE);
	    }
	    // RJH - if key is not same as folder, and we want the read-back
	    // objects to end up in the TDS with the given Storegate key rather
	    // than the folder name, need to set the <key> attribute
	    // a jobOption WriteKeyInfo=FALSE disables this, in case you have
	    // created objects with the 'wrong' SG key and want the key
	    // on read-back to correspond to the folder name
	    if (local_folder!=key && m_writeKeyInfo) {
	      sc=buildDescription("key",key,mergedNames);
	      if (sc.isFailure()) {
		ATH_MSG_ERROR ("Could not merge towards merged description: "
                               << "key: " << key);
		return( StatusCode::FAILURE);
	      }
	    }

	    // Add in symlink types
	    if (!symlinks.empty()) {
		std::string symlinkTypes;
		for (unsigned int i = 0; i < symlinks.size(); ++i) {
		    std::string type;
		    sc = m_clidSvc->getTypeNameOfID(symlinks[i], type);
		    if (sc.isFailure()) {
                        ATH_MSG_ERROR ("Could not get type name for symlink clid "
                                       << symlinks[i]);
			return( StatusCode::FAILURE);
		    }
		    else {
                        ATH_MSG_DEBUG ("adding symlink: clid, type  " 
                                       << symlinks[i] << " " << type);
		    }
		    if (symlinkTypes.size()) symlinkTypes += ':';
		    symlinkTypes += type;
		}
		sc=buildDescription("symlinks", symlinkTypes, mergedNames);
		if (sc.isFailure()) {
                  msg() << MSG::ERROR <<"Could not merge symlinks to merged description: "
			<< "symlink types: ";
		    for (unsigned int i = 0; i < symlinkTypes.size(); ++i) {
                      msg() << MSG::ERROR << symlinkTypes[i] << " ";
		    }
		    msg() << MSG::ERROR << endmsg;
		    return( StatusCode::FAILURE);
		}
		else {
                  ATH_MSG_DEBUG ("symlinks, merged names " 
                                 << symlinkTypes << " " << mergedNames);
		}
	    }

	    // Type of time is defined by start/stop

	    // Some checks on the times:
	    if (!start.isValid() || 
		!stop.isValid()  || 
		start.isTimestamp() != stop.isTimestamp() || 
		start.isRunEvent() != stop.isRunEvent()) {
                ATH_MSG_ERROR ("Invalid times: start isValid/isTimeStamp/isRunEvent "
                               << "addrHeader: " << address_header
                               << start.isValid() << " " << start.isTimestamp() << " " 
                               << start.isRunEvent());
                ATH_MSG_ERROR ("Invalid times: stop isValid/isTimeStamp/isRunEvent "
                               << "addrHeader: " << address_header
                               << stop.isValid() << " " << stop.isTimestamp() << " " 
                               << stop.isRunEvent());
		return( StatusCode::FAILURE);
	    }
	    bool isTimeStamp = false;
	    if (start.isTimestamp()) isTimeStamp = true;

	    if (isTimeStamp)
	      {
		sc = buildDescription( "timeStamp", "time",
						 mergedNames );
	      }
	    else
	      {
		sc = buildDescription( "timeStamp", "run-lumi",
						 mergedNames );
	      }
 	    if (sc.isFailure()) { 
                ATH_MSG_ERROR ("Could not merge timeStamp flag towards merged description. ");
 		return( StatusCode::FAILURE);
 	    }
	    // add <named/> for channels with names
	    if (storeAttrListColl && attrListColl!=0 && 
		attrListColl->name_size()>0) mergedNames+="<named/>";
	    
	    ATH_MSG_DEBUG (" create folder " << local_folder 
                           << " with description " << mergedNames);

	    // Create folder 

            ATH_MSG_DEBUG ("Set attr list coll ptr " << attrListColl);

	    cool::RecordSpecification payloadSpec;
	    if (storeRef) {
		// Folder with refs only:
		// Payload specification - contains just a string
	        // assuming POOL refs will not be longer than 4000 chars
		payloadSpec.extend("PoolRef",cool::StorageType::String4k);
	    }
	    else {
		// Store AttributeList or a collection of AttributeLists
	        const coral::AttributeList* atr4spec=0;
		if (storeAttrListColl) {

		    // Folder with CondAttrListCollection itself
		    // Get the attribute spec from the first element in the collection
		    if (0 == attrListColl) {
                        ATH_MSG_ERROR ("attrListColl not found. ");
			return( StatusCode::FAILURE);
		    }
		
		    if (0 == attrListColl->size()) {
                        ATH_MSG_ERROR ("attrListColl is empty. ");
			return( StatusCode::FAILURE);
		    }
		    // FIXME
                    ATH_MSG_DEBUG ("Size of AttrList collection" <<
                                   attrListColl->size());

                    atr4spec=&((*attrListColl->begin()).second);
		} else {
		    // folder with simple AttributeList
		    atr4spec=attrList;
                    ATH_MSG_DEBUG ("In simple atrlist branch");
		}
                ATH_MSG_DEBUG ("Pointer to atrList is " << atr4spec);
		// construct COOL specification
		for (coral::AttributeList::const_iterator itr=
		       atr4spec->begin();itr!=atr4spec->end();++itr) {
		  // extend specification with appropriate COOL type
		  // giving opportunity to override default choice
		  payloadSpec.extend(itr->specification().name(),
                    coralToCoolType(itr->specification().name(),
				    itr->specification().typeName()));
		}
	    }
	    // create folder
	    cool::FolderVersioning::Mode version=
	      cool::FolderVersioning::MULTI_VERSION;
	    if (m_svFolder) {
	      version=cool::FolderVersioning::SINGLE_VERSION;
              ATH_MSG_INFO ("Creating single version folder for "
                            << local_folder);
	    }
	    if (m_payloadTable)
              ATH_MSG_INFO ("Creating separate payload table for "
                            << local_folder);
	    cool::FolderSpecification folderSpec(version,payloadSpec,cool::PayloadMode::SEPARATEPAYLOAD);
	    folder = db->createFolder(local_folder,folderSpec,
                  mergedNames,true); 
            ATH_MSG_DEBUG ("Creation of CondDBFolder " << 
                           local_folder << " done");

	    // create channels if needed - only for CondAttrListColl with names
	    if (storeAttrListColl && attrListColl!=0 && 
		attrListColl->name_size()>0) {
              ATH_MSG_DEBUG ("Naming " << attrListColl->name_size() <<
                             " channels in " << local_folder);
	      for (CondAttrListCollection::name_const_iterator nitr=
		     attrListColl->name_begin();
		   nitr!=attrListColl->name_end();++nitr) {
		folder->createChannel(nitr->first,nitr->second);
	      }
	    }
	}
	    
	// Print out stop/start ONLY for non-collections - collections
	// have a per-channel start/stop
	if (storeAttrListColl) {
            ATH_MSG_DEBUG (" Global Start/stop time: "
                           << start << " " << stop << " Note: will be ignored for channels with differnt IOVs " );
	}
	else {
            ATH_MSG_DEBUG (" Start/stop time " << start << " " << stop << " ");
	}

	// Convert IOVTime to ValidityKey
	cool::ValidityKey ivStart = start.re_time();
	cool::ValidityKey ivStop  = stop.re_time();
	if(start.isTimestamp()) {
	    ivStart = start.timestamp();
	    ivStop  = stop.timestamp();
	}

	// get record specification of folder
		    
	const cool::RecordSpecification& rspec=folder->payloadSpecification();

	if (storeAttrListColl) {
	    //
	    // Need to store multiple channels
	    //
	    //  The policy for the IOV of each channel is to use the
	    //  channel IOV already stored in the collection, if it is
	    //  there. If it is not there, then use the global IOV. 
	    //
	    if (storeRef) {
		// Should NOT get here for a collection, signal error
                ATH_MSG_ERROR ("Trying to store a ref for a CondAttrListCollection. ");
		return( StatusCode::FAILURE);
	    }

	    ATH_MSG_DEBUG (" --> Storing Object( " << start << ", " << stop 
                           << ", " << tag << " )");
	    ATH_MSG_DEBUG (" -->   address: " << address_data);
	    // Loop over collection
	    CondAttrListCollection::const_iterator first = attrListColl->begin();
	    CondAttrListCollection::const_iterator last  = attrListColl->end();
	    for (; first != last; ++first) {

		// Copy from retrieved attribute list
		const coral::AttributeList& payload = (*first).second;
		// Get channel id
		CondAttrListCollection::ChanNum chanNum = (*first).first;

		// Check whether to use already stored IOV, or the
		// global IOV
		cool::ValidityKey ivStart1 = ivStart;
		cool::ValidityKey ivStop1  = ivStop;
		CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
		std::ostringstream attr;
		payload.toOutputStream( attr );
		ATH_MSG_DEBUG (" --> ChanNum: " << chanNum << " Payload: " << attr.str());
		if (!m_forceGlobalIOV && iovIt != attrListColl->iov_end()) {
		    const IOVRange& range = (*iovIt).second;
		    if(range.start().isTimestamp()) {
			ivStart1 = range.start().timestamp();
			ivStop1  = range.stop().timestamp();
		    }
		    else {
			ivStart1 = range.start().re_time();
			ivStop1  = range.stop().re_time();
		    }
		    ATH_MSG_DEBUG (" --> Start/stop time " 
                                   << range.start() << " " << range.stop() << " ");
		}
		else {
                  ATH_MSG_DEBUG (" --> Start/stop time " 
                                 << start << " " << stop << " ");
		}
		
		// Store address in folder with interval
		cool::Record record(rspec,payload);
		if (m_userTags && tag!="") {
  	          ATH_MSG_DEBUG ("Object stored with user tag " << tag );
  		  folder->storeObject( ivStart1, 
                                       ivStop1,
                                       record,
                                       chanNum,tag,!m_userTagsUH);
		} else {
  		  folder->storeObject( ivStart1, 
                                       ivStop1,
                                       record,
                                       chanNum);
		}
	    }
	}
	else {
	    // Store a single channel
	    cool::Record record(rspec);
	    if (storeRef) {
	      // PayLoad is ONLY the ref
	      record["PoolRef"].setValue<std::string>(address_data);
	    }
	    else {
	      // Copy from retrieved attribute list
	      record=cool::Record(rspec,*attrList);
	    }

	    // Store address in folder with interval
	    if (m_userTags && tag!="") {
	      ATH_MSG_DEBUG ("Object stored with user tag " << tag );
  	      folder->storeObject( ivStart, 
                                   ivStop,
                                   record,0,tag,!m_userTagsUH);
	    } else {
  	      folder->storeObject( ivStart, 
                                   ivStop,
                                   record,0);
	    }
	      
	    ATH_MSG_DEBUG (" --> Stored Object( " << start << ", " << stop 
                           << ", " << tag << " )");
	    ATH_MSG_DEBUG (" -->   address: " << address_data);
	}
	
	ATH_MSG_DEBUG (" storeData OK ");
	      
	// Now tag the folder if required
	if (!m_userTags) {
  	  if ("" == tag) {
              ATH_MSG_DEBUG (" tag is empty - folder is not being tagged ");
   	  }
	  else {
            ATH_MSG_INFO (" Tagging HEAD of folder " << local_folder << 
                          " with tag " << tag);
	    try { 
  	      folder->tagCurrentHead(tag,m_tagDescription);
	    }
	    catch ( cool::TagExists& e) {
              ATH_MSG_INFO ("Tag " << tag << 
                            " exists - attempt to delete tag and retag HEAD");
	      // first check this tag is really defined in THIS folder
              std::vector<std::string> taglist=folder->listTags();
	      if (find(taglist.begin(),taglist.end(),tag)==
		  taglist.end()) {
                  ATH_MSG_ERROR ("Tag is defined in another folder - tag names must be global");
	      } else if (folder->existsUserTag(tag)) {
		// this is a COOL user tag, in which case user
 	        // is trying to mix user and HEAD tags, not allowed in COOL1.3
		ATH_MSG_ERROR ("Tag " << tag << 
                               " is already USER tag - cannot mix tagging modes");
	      } else {
	        try {
	          folder->deleteTag(tag);
    	          folder->tagCurrentHead(tag,m_tagDescription);
	          ATH_MSG_INFO ("Delete and retag succeeded");
	        }
	        catch ( cool::TagNotFound& e) {
	          ATH_MSG_ERROR ("Delete and retag HEAD failed");
	        }
	      }
	    }
	  }
	}
	return StatusCode::SUCCESS;
    }
    
    catch (std::exception& e) {
        ATH_MSG_ERROR ("*** COOL  exception caught: " << e.what() );
        // << "\n"
        //	    << "***   error code: " << e.code() 
	return StatusCode::FAILURE;
    }

    //return StatusCode::SUCCESS;
}

cool::StorageType::TypeId IOVRegistrationSvc::coralToCoolType(
   const std::string& parname,const std::string& coralName) const {
  // map coral type onto corresponding COOL storage type
  std::string coralType=coralName;
  // check for any overrides
  for (unsigned int i=0;i<m_overrideType.size();++i) {
    if (m_overrideName[i]==parname) {
      coralType=m_overrideType[i];
      ATH_MSG_INFO ("Override default type for attribute " << 
                    parname << " - use " << coralType);
    }
  }
  // FIXME to include all CORAL/COOL types
  // give both the names of CORAL types, and COOL types which may get specified
  // as overrides
  if (coralType=="bool") return cool::StorageType::Bool;
  if (coralType=="UChar" || coralType=="unsigned char") return cool::StorageType::UChar;
  if (coralType=="Int16") return cool::StorageType::Int16;
  if (coralType=="UInt16") return cool::StorageType::UInt16;
  if (coralType=="Int32" || coralType=="int") return cool::StorageType::Int32;
  if (coralType=="UInt32" || coralType=="unsigned int") return cool::StorageType::UInt32;
  if (coralType=="UInt63" || coralType=="unsigned long long") return cool::StorageType::UInt63;
  if (coralType=="Int64" || coralType=="long long") return cool::StorageType::Int64;
  if (coralType=="Float" || coralType=="float") return cool::StorageType::Float;
  if (coralType=="Double" || coralType=="double") return cool::StorageType::Double;
  if (coralType=="String255") return cool::StorageType::String255;
  if (coralType=="String4k" || coralType=="string") return cool::StorageType::String4k;
  if (coralType=="String64k") return cool::StorageType::String64k;
  if (coralType=="String16M") return cool::StorageType::String16M;
  if (coralType=="blob" || coralType=="Blob64k") 
    return cool::StorageType::Blob64k;
  if (coralType=="Blob16M") return cool::StorageType::Blob16M;

  // if we get here, mapping is undefined
  ATH_MSG_FATAL ("No COOL mapping defined for CORAL type " << coralName);
  throw std::exception();
}


//--------------------------------------------------------------------------

uint64_t IOVRegistrationSvc::timeToNano(const unsigned long int timesec) const
{
  // convert time specified in seconds to ns used by COOL
  // use the magic value MAXEVENT to signal full range
  if (timesec==IOVTime::MAXEVENT) {
    return IOVTime::MAXTIMESTAMP;
  } else {
    return static_cast<uint64_t>(timesec)*1000000000;
  }
}

StatusCode IOVRegistrationSvc::buildDescription(const std::string&  identifier,
                                       const std::string&  value,
                                       std::string&        description) const {
  // this routine was originally in IOVDbSvc, moved here as only client
  // buids an XML fragment of form <identifier>value</identifier>
  if (identifier.empty() || value.empty()) {
    ATH_MSG_ERROR ("Identifier or value is null.");
    return StatusCode::FAILURE;
  }
  description = "<"+identifier+">"+value+"</"+identifier+">"+description;
  return StatusCode::SUCCESS;
}

StatusCode IOVRegistrationSvc::splitAddress(const std::string& address,
                                  std::string& address_header,
                                  std::string& address_data ) const {
  // this routine was originally in IOVDbSvc, moved here as only client
  // Deals with address of form
  // <address_header service_type="256" clid="1238547719" /> POOLContainer_CondAttrListCollection][CLID=x
  // return header as part up to and including />, trailer as rest

  std::string::size_type p1=address.find(" />");
  if (p1!=std::string::npos) {
    address_header=address.substr(0,p1+3);
    address_data=address.substr(p1+4);
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
}
