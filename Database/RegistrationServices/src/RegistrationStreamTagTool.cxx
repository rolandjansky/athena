/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RegistrationStreamTagTool.cxx
 *
 * @brief AlgTool which takes references provided by RegStream 
 * finds the appropriate CORAL object in storegate and stores 
 * them in a POOL collection. 
 *
 * $Id: RegistrationStreamTagTool.cxx,v 1.12 2008-12-08 17:14:29 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolUtilities/TagAthenaAttributeList.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "PoolSvc/IPoolSvc.h"
#include "PersistentDataModel/Token.h"

// Framework include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/GaudiException.h"

// POOL include files
#include "POOLCore/Exception.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/CollectionBaseNames.h"
#include "CollectionBase/CollectionRowBuffer.h"  // replacing attributelist
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionDataEditor.h"
#include "CollectionBase/ICollectionSchemaEditor.h"

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
#include <typeinfo>

#include "RegistrationStreamTagTool.h"

// Standard Constructor
RegistrationStreamTagTool::RegistrationStreamTagTool(
    const std::string& type, 
    const std::string& name,
    const IInterface* parent 
) : RegistrationStreamLCGTool(type,name,parent)
{
    declareProperty("FragmentByGroup",      m_FragmentByGroup=false,
                    "Default = false; If true, then collection fragments formed for each GRP=");
}

// Standard Destructor
RegistrationStreamTagTool::~RegistrationStreamTagTool()   
{}

// Work entry point
StatusCode 
RegistrationStreamTagTool::fillAtt(std::vector< std::pair<std::string, std::string> >& refs,
                                   std::string tagKey) 
{
    // Here we add to a collection a set of refs built from the
    // DataHeaders and the tag as an AttributeList. The DataHeaders
    // and TagAthenaAttributeList are all specified in the itemList.

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
    if (refs[0].first.size() > 0) prefName = refs[0].first + "_ref";
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

    ATH_MSG_DEBUG("CollDesc after token addition ");
    if (msg().level() <= MSG::DEBUG) dumpDesc();

    // If there was no attributelist in the itemlist, issue a warning
    //   otherwise try to retrieve the appropriate one from storegate
    const TagAthenaAttributeList* attributes=0;
    if (tagKey == "notthere") {
        ATH_MSG_WARNING("Could not find TagAthenaAttributeList in itemlist, will continue with tokens only ");
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
        if (msg().level() <= MSG::VERBOSE) dumpTagList(attributes);

        // If no attribute columns, then define them based on retrieved attributelist 
        // 
        if (m_collection->description().numberOfAttributeColumns()==0) {
	    ATH_MSG_DEBUG("add spec to collection");
            ATH_MSG_DEBUG(
                "Collection has " << m_collection->description().numberOfColumns() << " total columns"
            );
            ATH_MSG_DEBUG(
                "Collection has " << m_collection->description().numberOfAttributeColumns() 
                 << " metadata columns"
            );
            ATH_MSG_DEBUG(
                "Collection has " 
                 << m_collection->description().numberOfTokenColumns() 
                 << " token columns"
            );

            //---------
            // DEFINE METADATA ATTRIBUTES
            //
            // Use columns in retrieved attributelist
            //
            AthenaAttributeListSpecification::const_iterator first1 = attributes->getDesc()->begin();
            AthenaAttributeListSpecification::const_iterator last1  = attributes->getDesc()->end();
            for (; first1 != last1; ++first1) {
               std::string name = (*first1).name();
               std::string typeName = (*first1).typeName();
               std::string info = (*first1).attributeType().extraInfo();
               if (m_FragmentByGroup) {
                  std::string group = (*first1).attributeType().group();
                  bool fragmentGood = true;
                  // If fragment does not exist, add it
                  if (!m_collection->exists(group)) {
                     ATH_MSG_INFO("Fragment " << group << " does not exist, will try adding it");
                     try {
                        m_collection->schemaEditor().addCollectionFragment(group);
                     }
                     catch (const pool::Exception& e) {
                        ATH_MSG_ERROR("Unable to add fragment " << group << " due to " << e.what() );
                        fragmentGood = false;
                     }
                  }
                  if (fragmentGood) {
                     // Ok, safe to add column to fragment
                     try {
	                m_collection->schemaEditor().insertColumn( name, typeName, info, group );
                     }
                     catch (const pool::Exception& e) {
                        msg(MSG::ERROR) << "Unable to add column " << name 
                                          << " to " << m_collection->description().name() 
                                          << " due to " << e.what() << endmsg;
                     }
                  }
               }
               // Don't do fragments
               else {
                  try {
	             m_collection->schemaEditor().insertColumn( name, typeName, info );
                  }
                  catch (const pool::Exception& e) {
                     msg(MSG::ERROR) << "Unable to add column " << name 
                                       << " to " << m_collection->description().name() 
                                       << " due to " << e.what() << endmsg;
                  }
               }
            }
            // Check for primary key
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

	    ATH_MSG_DEBUG("CollDesc after attribute addition ");
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
        msg(MSG::ERROR) 
	    << "Caught exception from collection add of Pool attributes. Message: " 
  	    << e.what() << endmsg;
	return(StatusCode::FAILURE);
    }

    m_events++;
    
    return StatusCode::SUCCESS;
}

void RegistrationStreamTagTool::dumpTagList(const TagAthenaAttributeList* attributes) const
{
    dumpAttList(attributes);
}

// Provide the CLID that contains the metadata used by the Tool
const CLID& RegistrationStreamTagTool::attListID() 
{
    return ClassID_traits<TagAthenaAttributeList>::ID();
}
