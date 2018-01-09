/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RegistrationStreamLCGTool.cxx
 *
 * @brief AthAlgTool which takes references provided by RegStream 
 * finds the appropriate CORAL object in storegate and stores 
 * them in a POOL collection. 
 *
 * $Id: RegistrationStreamLCGTool.cxx,v 1.25 2009-05-08 20:24:15 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "PersistentDataModel/Token.h"
#include "PoolSvc/IPoolSvc.h"

// Framework include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/GaudiException.h"

// POOL include files
#include "POOLCore/Exception.h"
#include "CollectionBase/CollectionFactory.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/CollectionBaseNames.h"
#include "CollectionBase/CollectionRowBuffer.h"  // replacing attributelist
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionDataEditor.h"
#include "CollectionBase/ICollectionSchemaEditor.h"
#include "FileCatalog/IFileCatalog.h"

// CORAL include files
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Date.h"
#include "CoralBase/TimeStamp.h"

#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <ctime>

#include "DBDataModel/CollectionMetadata.h"

#include "RegistrationStreamLCGTool.h"

// Standard Constructor
RegistrationStreamLCGTool::RegistrationStreamLCGTool(
    const std::string& type, 
    const std::string& name,
    const IInterface* parent 
) 
    :
    AthAlgTool(type,name,parent),
    m_storeGateSvc( "StoreGateSvc",        name ),
    m_metadataStore( "StoreGateSvc/MetaDataStore",        name ),
    m_poolSvc( "PoolSvc", name ),
    m_collection(0),
    m_events(0)
{
    m_cmdKeys.push_back("Input");
    // declare interface
    declareInterface<IRegistrationStreamTool>( this );

    declareProperty("CollectionType",     m_collectionType = "ExplicitROOT", 
                    "ExplicitROOT, ExplicitRAL"); 
    declareProperty("Connection",         m_connection,
                    "CORAL type URL or alias"); 
    declareProperty("OutputCollection",   m_outputCollection="",
                    "Name of collection. NOTE: will append .root for ExplicitROOT");
    declareProperty("CollectionOpenMode", m_openMode="CREATE_AND_OVERWRITE",
                    "READ, CREATE, CREATE_AND_OVERWRITE");
    declareProperty("Slave",              m_slave=true, 
                    "Default = true = take type,connection,name,mode from parent stream");
    declareProperty("StoreName",          m_storeName="StoreGateSvc",       "EXPERTS");
    declareProperty("PrimaryKey",         m_primKeyAtts, "primary key attributes, ALL must be present");
}

// Standard Destructor
RegistrationStreamLCGTool::~RegistrationStreamLCGTool()   {
}

// initialize data writer
StatusCode 
RegistrationStreamLCGTool::initialize() 
{
    StatusCode status(StatusCode::FAILURE);

    m_events=0;
    
    ATH_MSG_DEBUG("In initialize ");

    // set up the SG service:
    m_storeGateSvc = ServiceHandle<StoreGateSvc>( m_storeName, name() );
    StatusCode sc = m_storeGateSvc.retrieve();
    if ( !sc.isSuccess() ) {
        ATH_MSG_ERROR("Could not locate default store");
	return sc;
    }
    else {
        ATH_MSG_DEBUG("Found " << m_storeName << " store.");
    }
    assert( m_storeGateSvc );

    // set up the metadata store:
    m_metadataStore = ServiceHandle<StoreGateSvc>( "StoreGateSvc/MetaDataStore", name() );
    sc = m_metadataStore.retrieve();
    if ( !sc.isSuccess() ) {
        ATH_MSG_ERROR("Could not locate metadata store");
	return sc;
    }
    else {
        ATH_MSG_DEBUG("Found metadata store.");
    }
    assert( m_metadataStore );

    // get PoolSvc
    status = m_poolSvc.retrieve();
    if (!status.isSuccess()) {
        ATH_MSG_ERROR(" Cant get PoolSvc");
	return(StatusCode::FAILURE);
    } 
    else {
        ATH_MSG_DEBUG(" Got PoolSvc");
    }

    // Check whether properties need to be take from stream
    if (m_slave.value()) overrideProperties();
         
    ATH_MSG_DEBUG("End initialize ");
  
    StatusCode basestat = AthAlgTool::initialize();
    if (status.isSuccess() && basestat.isSuccess()) return StatusCode::SUCCESS;
    else return StatusCode::FAILURE;
}


StatusCode 
RegistrationStreamLCGTool::initCollection() 
{

    ATH_MSG_DEBUG("initCollection");

    // Check/Decode properties set by job options and then 
    // create collection from call to PoolSvc as:
    //  create(collectionType, 
    //         connection,            // connection string
    //         collectionName, 
    //         openMode);
    //
    // argument         possible values
    //
    // collectionType -  "RootCollection", "MySQLCollection", "MySQLltCollection", "RelationalCollection"
    // connection  -     <connect string>
    // collectionName  - <CollectionName>
    // 
    // openMode can be CREATE, CREATE_AND_OVERWRITE (default), UPDATE, or 
    //   READ (not appropriate for RegStrm)


    // Set CollectionType and connection for the collection factory
    std::string collectionType = m_collectionType;
    std::string connection;
    if (collectionType == "ExplicitROOT") {
	collectionType = "RootCollection";
    }
    else if( (collectionType == "ExplicitMySQL") ||
	     (collectionType == "ExplicitMySQLlt") ||
	     (collectionType == "ExplicitRAL") )
    {
       collectionType = "RelationalCollection";
       connection = m_connection;
        // If connectionString is empty, use default of TAGDB 
        // Can override by new entry in local dblookup.xml if needed
       if(connection.size() == 0) {
           msg(MSG::WARNING) << "No connection string provided for relational collection access, "
                 << "will try using default alias TAGDB" << endmsg;
           connection = "TAGDB";
	}
	// Check that the connection string is ok
	if (connection.find("mysql:") != 0 &&
            connection.find("oracle:") != 0) {
            msg(MSG::WARNING)
                  << "Database connectionString did not begin with \"mysql:\" or \"oracle:\""
                  << ", ASSUMING alias and passing"
                  << endmsg;
	    //return(StatusCode::FAILURE);
	}
    }
    else {
	// Invalid collection type
	msg(MSG::ERROR) << "RegStream<x>.CollectionType must be one of: "
	                << "ExplicitROOT (default), ExplicitMySQL, ExplicitMySQLlt or ExplicitRAL. Found: " 
	                << m_collectionType
	                << endmsg;
	return(StatusCode::FAILURE);
    }

    // CollectionName must be non-empty
    if (m_outputCollection.value().size() == 0) {
	msg(MSG::ERROR) << "RegStream<y>.CollectionName must be provided " 
	    << endmsg;
	return(StatusCode::FAILURE);
    }

    // openMode must be correct
    pool::ICollection::OpenMode openMode;
    if (m_openMode.value() == "READ") {
	msg(MSG::ERROR) << "RegStream<x>.CollectionOpenMode cannot be READ. "
	    << "Possible values are: CREATE, CREATE_AND_OVERWRITE (default), or UPDATE " 
	    << endmsg;
	return(StatusCode::FAILURE);
    }
    else if (m_openMode.value() == "CREATE") {
	openMode = pool::ICollection::CREATE;
    }
    else if (m_openMode.value() == "CREATE_AND_OVERWRITE") {
	openMode = pool::ICollection::CREATE_AND_OVERWRITE;
    }
    else if (m_openMode.value() == "UPDATE") {
	openMode = pool::ICollection::UPDATE;
    }
    else {
	msg(MSG::ERROR) << "Invalide RegStream<x>.CollectionOpenMode. "
	    << "Possible values are: CREATE, CREATE_AND_OVERWRITE (default), or UPDATE " 
	    << endmsg;
	return(StatusCode::FAILURE);
    }
    

    ATH_MSG_DEBUG("Collection name: " << m_outputCollection);
    ATH_MSG_DEBUG("Collection type " << collectionType);
    ATH_MSG_DEBUG("Collection open mode " << m_openMode);


    // Create a collection for either MySQL or ROOT
    if (collectionType != "ImplicitCollection") {
       try {
           m_collection = m_poolSvc->createCollection(collectionType,
                                                      connection,
                                                      m_outputCollection.value(),
                                                      openMode);
       }
       catch (pool::Exception& e) {
           msg(MSG::ERROR) << "::initCollection: Caught exception from Pool collection creation. Message: " 
                           << e.what()
                           << endmsg;
           return(StatusCode::FAILURE);
       }
       if (m_collection==0) {
          ATH_MSG_ERROR("::initCollection: Zero collection ptr from Pool collection creation. Message: ");
          return(StatusCode::FAILURE);
       }
    }

    // prepare the new collection attributes


    return StatusCode::SUCCESS;
}


StatusCode
RegistrationStreamLCGTool::finalize()
{
    ATH_MSG_DEBUG("In finalize: " << m_events);
    return AthAlgTool::finalize();
}


// terminate data writer
StatusCode 
RegistrationStreamLCGTool::commit() 
{
    ATH_MSG_INFO("Collection Events output: " << m_events);

    // Commit collection
    if (m_collection==0) {
        ATH_MSG_DEBUG("Do initCollection in finalize for m_collection = 0");
        ATH_MSG_DEBUG(" ==> Probably had 0 accepted events");
        StatusCode status = initCollection();
        if (!status.isSuccess()) {
            ATH_MSG_ERROR(" Cannot create collection");
            return(StatusCode::FAILURE);
        }
        else {
            ATH_MSG_DEBUG(" Created collection");
        }
    }

    // Commit collection
    if( m_collection ) {
       ATH_MSG_DEBUG("pointer to collection is not null");
     
       // Register collection
       try {
          bool overwrite = false;
          if (m_openMode.value()=="CREATE_AND_OVERWRITE") overwrite = true;
          m_poolSvc->registerExistingCollection(m_collection,overwrite,true);
       }
       catch (pool::Exception& e) {
          ATH_MSG_INFO("Unable to register collection: " << e.what());
       }

       // Add metadata to collection
       addMetadata();

       try {
	  m_collection->commit();
       } catch (pool::Exception& e) {
	  ATH_MSG_ERROR("unable to commit collection commit: " << e.what() );
	  return(StatusCode::FAILURE);
       }
       ATH_MSG_DEBUG("Committed collection ");

       m_collection->close();
       ATH_MSG_DEBUG(" Closed collection ");
       delete m_collection;  m_collection = 0;
    }
    else {
       ATH_MSG_WARNING("Collection never initialized");
    }
    return AthAlgTool::finalize();
}


// Work entry point
StatusCode 
RegistrationStreamLCGTool::fill(std::vector< std::pair<std::string, std::string> >& refs,
                                std::string tagKey) 
{
    return this->fillAtt(refs,tagKey);
}

// Misdirection method to allow inheritance of interface
StatusCode 
RegistrationStreamLCGTool::fillAtt(std::vector< std::pair<std::string, std::string> >& refs,
                                   std::string tagKey) 
{
    // Here we add to a collection a set of refs built from the
    // DataHeaders and the tag as an AttributeList. The DataHeaders
    // and AthenaAttributeList are all specified in the itemList.

    if (m_events==0) {
        ATH_MSG_DEBUG("outcoll = " << m_outputCollection.value() );
        StatusCode status = initCollection();
        if (!status.isSuccess()) {
 	    ATH_MSG_ERROR(" Cannot create collection");
 	    return(StatusCode::FAILURE);
        } 
        else {
 	    ATH_MSG_DEBUG(" Created collection");
        }
    }

    ATH_MSG_DEBUG("RSTool::fill dumping inputs");
    ATH_MSG_DEBUG("tagKey = " << tagKey);
    ATH_MSG_DEBUG("refs");
    for (std::vector< std::pair<std::string,std::string> >::iterator it = refs.begin(); 
                                                     it != refs.end(); ++it) {
        ATH_MSG_DEBUG(it->first << " " << it->second);
    }

    // Keep track of used names
    std::set<std::string> used;

    // Check that collection was initialized
    //
    if( !m_collection ) {
	    ATH_MSG_ERROR("Collection has not been initialized! ");
	    return  StatusCode::FAILURE;
    }

    if (refs.size() == 0) {
        ATH_MSG_ERROR("fill, ref container empty");
	return(StatusCode::FAILURE);
    }

    // Now set event reference name to process tag of DH
    std::string prefName;
    if (refs[0].first.size() >0) prefName = refs[0].first + "_ref";
    else prefName = m_collection->description().eventReferenceColumnName();
    if ( prefName != m_collection->description().eventReferenceColumnName() ) {
       try {
          m_collection->schemaEditor().setEventReferenceColumnName(prefName);
          used.insert(prefName);
          ATH_MSG_DEBUG("Setting primary ref to " << prefName);
       }
       catch( const pool::Exception& e ) {
          ATH_MSG_WARNING("Unable to set primary ref name, taking default");
          ATH_MSG_WARNING("due to exception " << e.what() );
       }
    }

    //---------
    // DEFINE TOKEN ATTRIBUTES
    // Since the token columns start at one and may remain at one, use 
    //   the only available marker to check if extra token columns need to be defined
    //   CAVEATS:
    //      - will be based on first dataheader
    //      - Any new stages in following headers will be disregarded
    //
    //static StatusCode sc_backNavStat = StatusCode::FAILURE;
    if (m_events==0) {
        //
        // Get back nav names by reading provenance record in first DataHeader
        std::vector< std::pair<std::string,std::string> >::iterator itref = refs.begin();
        // Increment first to avoid first record, even though used is in place
        for (std::vector< std::pair<std::string,std::string> >::iterator itr = ++itref; 
             itr != refs.end(); 
             ++itr) 
        {
            std::string name = itr->first + "_ref";
            ATH_MSG_DEBUG("Event " << m_events); 
            // Maintain name uniqueness for column names
            if ( used.find(name)==used.end() ) {
                try {
                    m_collection->schemaEditor().insertTokenColumn( name ); 
                    used.insert(name);
                    ATH_MSG_DEBUG("Provenance key " << name << " added to spec");
                }
                catch (const pool::Exception& e) {
                    ATH_MSG_DEBUG("Provenance stage " << name << " already in token spec");
                }
            }
            else {
                ATH_MSG_DEBUG("ref name " << name << " already used");
            }
        }
    }

    ATH_MSG_DEBUG( "CollDesc after token addition ");
    if (msg().level() <= MSG::DEBUG) dumpDesc();

    // If there was no attributelist in the itemlist, issue a warning
    //   otherwise try to retrieve the appropriate one from storegate
    const AthenaAttributeList* attributes=0;
    if (tagKey == "notthere") {
        ATH_MSG_WARNING("Could not find Attribute List in itemlist, will continue with tokens only ");
	//return  StatusCode::FAILURE;
    }
    else {
        // AttributeLists should have been filled by other Algorithms and put in StoreGate
        //   Choose the one for the tagKey from the itemlist
        StatusCode retrieveStatus = m_storeGateSvc->retrieve(attributes, tagKey);
	if (retrieveStatus.isFailure()) {
	   ATH_MSG_ERROR("Could not retrieve attributes. Key/tagname " << tagKey);
	   return  StatusCode::FAILURE;
        }
	else {
	   ATH_MSG_DEBUG("Retrieved attribute list with Key/tagname " << tagKey);
	}

	ATH_MSG_DEBUG("Attribute list");
        if (msg().level() <= MSG::VERBOSE) dumpLCGList(attributes);

        // If no attribute columns, then define them based on retrieved attributelist 
        // 
        if (m_collection->description().numberOfAttributeColumns()==0) {
	    ATH_MSG_DEBUG("add spec to collection");
            ATH_MSG_DEBUG("Collection has " << m_collection->description().numberOfColumns() << " total columns");
            ATH_MSG_DEBUG("Collection has " << m_collection->description().numberOfAttributeColumns() << " metadata columns");
            ATH_MSG_DEBUG("Collection has " << m_collection->description().numberOfTokenColumns() << " token columns");

            //---------
            // DEFINE METADATA ATTRIBUTES
            //
            // Use columns in retrieved attributelist
            //
            coral::AttributeList::const_iterator first1 = attributes->begin();
            coral::AttributeList::const_iterator last1  = attributes->end();
            for (; first1 != last1; ++first1) {
               std::string name = (*first1).specification().name();
               std::string typeName = (*first1).specification().typeName();
               try {
	           m_collection->schemaEditor().insertColumn( name, typeName );
               }
               catch (const pool::Exception& e) {
                   msg(MSG::ERROR) << "Unable to add column " << name 
                                   << " to " << m_collection->description().name() << endmsg;
               }
            }
            if (m_primKeyAtts.size()>0) {
               ATH_MSG_INFO("Setting Primary key to " << m_primKeyAtts);
               try {
                  m_collection->schemaEditor().setUniqueConstraint("PrimKey",m_primKeyAtts);
               }
               catch (const pool::Exception& e) {
                  ATH_MSG_WARNING("Unable to set primary key due to " << e.what());
               }
            }
            else {ATH_MSG_INFO("No Primary Key defined");}
	    ATH_MSG_DEBUG("CollDesc after attribute addition");
            if (msg().level() <= MSG::DEBUG) dumpDesc();

	    if (m_collection->description().numberOfAttributeColumns()==0) {
	        ATH_MSG_ERROR(" Attribute list specification is empty");
	        return(StatusCode::FAILURE);
	    } 
        } 
    }  // end of else for attributelist
   

    //---------
    // FILL ACTUAL COLLECTION DATA
    // 
    // Add tokens plus attributes
    try {
        // a CollectionRowBuffer has two elements: attributeList, tokenList
        pool::CollectionRowBuffer &row = m_collection->dataEditor().rowBuffer();
        // set attribute list first
        if(attributes!=0) row.attributeList() = attributes->coralList();
        ATH_MSG_DEBUG("Set the row buffer attributelist to a size=" << row.attributeList().size() ); 
        // now grab primary token from refs array and put it first in the tokenList
        // MN: not using the refnames[0] name, but the default token column
        row.tokenList()[ m_collection->description().eventReferenceColumnName() ].fromString( refs[0].second );
        // now do something similar to add in the provenance tokens
        std::vector< std::pair<std::string,std::string> >::iterator ir;
        for( ir = refs.begin(); ir != refs.end(); ++ir ) {
 	   try {
              std::string brefName(ir->first+"_ref");
	      if (brefName!=prefName) row.tokenList()[ brefName ].fromString( ir->second );
	   } catch( const pool::Exception& e ) {
              msg(MSG::ERROR) << " Failed to set Token "
                  << ir->second << " for provenance stage "
	          << ir->first 
		  << " error: " << e.what() << endmsg;
           }
        }

        m_collection->dataEditor().insertRow(row);

    } catch (const pool::Exception& e) {
        ATH_MSG_ERROR( 
	    "Caught exception from collection add of Pool attributes. Message: " 
  	    << e.what()
        );
	return(StatusCode::FAILURE);
    }

    m_events++;
    
    return StatusCode::SUCCESS;
}

void RegistrationStreamLCGTool::dumpDesc(int fragid) const
{
    // Dump token columns first
    ATH_MSG_DEBUG("Token Attributes for fragment " << fragid);
    for (int i=0; i< m_collection->description().numberOfTokenColumns(fragid); ++i) {
        std::string tcolname = m_collection->description().tokenColumn(i,fragid).name();
        std::string tcoltype = m_collection->description().tokenColumn(i,fragid).type();
        ATH_MSG_DEBUG("Name: " << tcolname << "   Type: " << tcoltype);
    }

    // Dump  metadata attributes
    ATH_MSG_DEBUG(" Metadata Attributes for fragment " << fragid);
    for (int i=0; i< m_collection->description().numberOfAttributeColumns(fragid); ++i) {
        std::string tcolname = m_collection->description().attributeColumn(i,fragid).name();
        std::string tcoltype = m_collection->description().attributeColumn(i,fragid).type();
        ATH_MSG_DEBUG("Name: " << tcolname << "   Type: " << tcoltype);
    }
}

void RegistrationStreamLCGTool::dumpLCGList(const AthenaAttributeList* attributes) const
{
    this->dumpAttList(attributes);
}

void RegistrationStreamLCGTool::dumpAttribute(const coral::Attribute& attrib) const
{
    std::string name = attrib.specification().name();
    std::string typeName = attrib.specification().typeName();
    const std::type_info& type = attrib.specification().type();
    if ( type == typeid(bool) ) {
        const bool* value = static_cast<const bool*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(char) ) {
        const char* value = static_cast<const char*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(unsigned char) ) {
        const unsigned char* value = 
            static_cast<const unsigned char*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(short) ) {
        const short* value = static_cast<const short*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(unsigned short) ) {
        const unsigned short* value = 
            static_cast<const unsigned short*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(int) ) {
        const int* value = static_cast<const int*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(unsigned int) ) {
        const unsigned int* value = 
            static_cast<const unsigned int*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(long) ) {
        const long* value = static_cast<const long*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(unsigned long) ) {
        const unsigned long* value = 
            static_cast<const unsigned long*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(long long) ) {
        const long long* value = static_cast<const long long*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(unsigned long long) ) {
        const unsigned long long* value = static_cast<const unsigned long long*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(float) ) {
        const float* value = static_cast<const float*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(double) ) {
        const double* value = static_cast<const double*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(long double) ) {
        const long double* value = 
            static_cast<const long double*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(std::string) ) {
        const std::string* value = 
            static_cast<const std::string*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " value " << *value); 
    }
    else if ( type == typeid(coral::Blob) ) {
        const coral::Blob* value = 
            static_cast<const coral::Blob*>( attrib.addressOfData() );
        ATH_MSG_DEBUG(" name " << name << " type " << typeName << " size " << value->size() );
    }
    else if ( type == typeid(coral::Date) ) {
        const coral::Date* value = 
            static_cast<const coral::Date*>( attrib.addressOfData() );
        msg(MSG::DEBUG) << " name " << name
                          << " type " << typeName
                          << "values: "
                          << " year " << value->year()
                          << " month " << value->month()
                          << " day " << value->day()
                          << endmsg;
    }
    else if ( type == typeid(coral::TimeStamp) ) {
        const coral::TimeStamp* value = 
            static_cast<const coral::TimeStamp*>( attrib.addressOfData() );
        msg(MSG::DEBUG) << " name " << name
                          << " type " << typeName
                          << " values: "
                          << " year " << value->year()
                          << " month " << value->month()
                          << " day " << value->day()
                          << " hour " << value->hour()
                          << " minute " << value->minute()
                          << " second " << value->second()
                          << " nanosecond " << value->nanosecond()
                          << endmsg;
    }
    else
    {
        ATH_MSG_ERROR("Unrecognized attribute type: " << typeName);
    }
}

// Provide the CLID that contains the metadata used by the Tool
const CLID& RegistrationStreamLCGTool::listID() 
{
    return this->attListID();
}

// Misdirection method to allow inheritance of interface
const CLID& RegistrationStreamLCGTool::attListID() 
{
    return ClassID_traits<AthenaAttributeList>::ID();
}

// Override properties of tool with values from stream for overlapping properties
void RegistrationStreamLCGTool::overrideProperties()
{
   const IProperty* propertyServer = dynamic_cast<const IProperty*>(this->parent());
   if (propertyServer != 0) {
      StringProperty streamProperty("CollectionType", "");
      StatusCode status = propertyServer->getProperty(&streamProperty);
      if (status.isSuccess() && streamProperty.value() != "") {
         m_collectionType = streamProperty.value();
         ATH_MSG_DEBUG("streamProperty CollectionType = " << m_collectionType.value() );
      }
      streamProperty = StringProperty("Connection", "");
      status = propertyServer->getProperty(&streamProperty);
      if (status.isSuccess()) {
         m_connection = streamProperty.value();
         ATH_MSG_DEBUG("streamProperty Connection = " << m_connection.value() );
      }
      streamProperty = StringProperty("OutputCollection", "");
      status = propertyServer->getProperty(&streamProperty);
      if (status.isSuccess() && streamProperty.value() != "") {
         m_outputCollection = streamProperty.value();
         ATH_MSG_DEBUG("streamProperty OutputCollection = " << m_outputCollection.value() );
      }
      streamProperty = StringProperty("CollectionOpenMode", "");
      status = propertyServer->getProperty(&streamProperty);
      if (status.isSuccess() && streamProperty.value() != "") {
         m_openMode = streamProperty.value();
         ATH_MSG_DEBUG("streamProperty CollectionOpenMode = " << m_openMode.value() );
      }
      streamProperty = StringProperty("StoreName", "");
      status = propertyServer->getProperty(&streamProperty);
      if (status.isSuccess() && streamProperty.value() != "") {
         m_storeName = streamProperty.value();
         ATH_MSG_DEBUG("streamProperty StoreName = " << m_storeName.value() );
      }
   }
   else {
      ATH_MSG_WARNING("Could not retrieve PropertyServer");
   }
}

void
RegistrationStreamLCGTool::addMetadata()
{
   // First get a handle to the MetaDataStore

   ServiceHandle<StoreGateSvc> metadataStore("StoreGateSvc/MetaDataStore", this->name());
   StatusCode status = metadataStore.retrieve();
   if (status.isSuccess()) {
      ATH_MSG_DEBUG("Found metadata store, MetaDataStore");   
      //ATH_MSG_DEBUG(metadataStore->dump());   
   }

   // Second grab data from requested CollectionMetadataContainers in MetaDataStore

   // If no entry contains * as a key, take the list
   if ( std::find(m_cmdKeys.begin(),m_cmdKeys.end(),"*") == m_cmdKeys.end() ) {
      const CollectionMetadataContainer* cmdbeg = nullptr;
      //const DataHandle<CollectionMetadata> cmdend;
      std::vector<std::string>::iterator cmd_it = m_cmdKeys.begin();
   
      // Loop over the explicit keys since there is no * key
      while (cmd_it != m_cmdKeys.end()) {
         //status = metadataStore->retrieve(cmdbeg,cmdend);
         std::string key(*cmd_it);
         ATH_MSG_DEBUG("Looking for key "<<key);
         if (metadataStore->contains<CollectionMetadataContainer>(key)) {
            status = metadataStore->retrieve(cmdbeg,key);
            if (status.isSuccess()) {
               ATH_MSG_DEBUG("Found metadata object for "<<key<<" in MetaDataStore");   
               // Loop over the contents of each container and store it in m_collection
               for (CollectionMetadataContainer::const_iterator cit = cmdbeg->begin();
                    cit != cmdbeg->end(); cit++) { 
                  CollectionMetadata::const_iterator it = (*cit)->begin();
                  while (it != (*cit)->end()) {
                     ATH_MSG_DEBUG("key = " << it->first << "; value = " << it->second);
                     m_collection->metadata().setValueForKey(it->first,it->second);
                     ++it;
                  }
               } // cit
            } 
         } // if contains
         //else {ATH_MSG_INFO("Could not find CollectionMetadata in MetaDataStore for key = " << key);}
         ++cmd_it;
      } // cmd_it
   }
   // or take all CollectionMetadataContainers
   else {
      const DataHandle<CollectionMetadataContainer> cmdbeg;
      const DataHandle<CollectionMetadataContainer> cmdend;
      status = metadataStore->retrieve(cmdbeg,cmdend);
      if (status.isSuccess()) {
         for (; cmdbeg != cmdend; ++cmdbeg) {
            ATH_MSG_DEBUG("Found CollectionMetadataContainer with key = " << cmdbeg.key());
            ATH_MSG_DEBUG("Found CollectionMetadataContainer with size = " << cmdbeg->size());
            // Loop over the contents of each container and store it in m_collection
            for (CollectionMetadataContainer::const_iterator cit = cmdbeg->begin();
                 cit != cmdbeg->end(); cit++) { 
               CollectionMetadata::const_iterator it = (*cit)->begin();
               while (it != (*cit)->end()) {
                  ATH_MSG_DEBUG("key = " << it->first << "; value = " << it->second);
                  m_collection->metadata().setValueForKey(it->first,it->second);
                  ++it;
               }
            }
         }
      }
   }

   // Third, put in any generic metadata for bookkeeping

   // Add the name from the description
   m_collection->metadata().setValueForKey("Name",m_collection->description().name());

   // Now get a timestamp
   time_t tm = time(NULL);
   std::stringstream temp;
   //temp << ctime(&tm);
   temp << tm;
   if (temp.str().size() > 0) {
      m_collection->metadata().setValueForKey("UnixTimestamp",temp.str());
   }
}

void RegistrationStreamLCGTool::setCollMetadataKeys(const std::vector<std::string>& keys)
{
   std::vector<std::string>::const_iterator it = keys.begin();
   while (it != keys.end()) {
      m_cmdKeys.push_back(*it);
      ++it;
   }
}
