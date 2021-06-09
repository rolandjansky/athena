/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_COLLUTILITIES_COLLAPPENDBASE_H
#define POOL_COLLUTILITIES_COLLAPPENDBASE_H

/**
 * @file CollAppendBase.h
 * @brief Base class header for utilities that copy or merge POOL collections
 * @author Marcin.Nowak@cern.ch
 * $Id: 
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionDataEditor.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "CollectionBase/CollectionBaseNames.h"

#include "CoralBase/MessageStream.h"
#include "CoralBase/AttributeList.h"

#include "CollectionUtilities/DstInfo.h"
#include "CollectionUtilities/Progress.h"
#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/QueryInfo.h"
#include "CollectionUtilities/CatalogInfo.h"


#include <string>
#include <map>
#include <set>


namespace pool
{
   class MetaInfo;
   class CollectionService;
   
   class ATLAS_NOT_THREAD_SAFE CollAppendBase
   {
  public:

     CollAppendBase(const std::string& name="CollAppend");
     virtual ~CollAppendBase();

     CollAppendBase (const CollAppendBase&) = delete;
     CollAppendBase& operator= (const CollAppendBase&) = delete;

     virtual int  execute( std::vector<std::string> argv_v );

  protected:
     virtual bool init( std::vector<std::string> argv_v );
     virtual void chkExistingDst( std::vector<bool>& existVec);

     virtual std::vector< pool::ICollection* >
     openDestCollections( pool::CollectionDescription& destDescription );

     virtual pool::ICollection*
     openDestCollection( const std::string& name, const std::string& type, const std::string& connect );

     virtual pool::ICollection*
     createDestCollection( const pool::ICollectionDescription& destDescription );

     virtual pool::CollectionDescription 
     buildDstDesc(const pool::ICollectionDescription& sourceDesc,
		  const pool::TokenList &tokens,
		  const coral::AttributeList &attribs,
                  const std::string& queryopt );

     virtual pool::ICollection*
     openSrcCollection( const std::string& name, const std::string& type, const std::string& connect );

     virtual std::string
     readCollectionGUID( pool::ICollection* collection );

     virtual void 		copyData();

     /// supply user specialization of MetaInfo class for metadata processing - BEFORE init()
     virtual void               setMetaInfo( MetaInfo* minfo );
     virtual void 		readMetadata();
     virtual void 		addMetadata();
     virtual void	 	writeMetadata();

     virtual void	 	closeCollections();
     virtual void	 	finalize();

  
     std::vector< pool::ICollection* > 	m_srcCollections;
     std::vector< pool::ICollection* > 	m_destCollections;

     std::string 	m_thisProgram;

     std::string 	m_provName;
     std::string 	m_provCLID;
     Token 	        m_provToken;

     // used to mark provenance columns removed from the source
     bool         	m_provCollExists;
     std::set<std::string>     m_removedTokens;
  
     bool      	        m_noAttrib;
     // output modifiers
     int           	m_numEvents;
     int         	m_numEventsPerCommit;
     int          	m_numEventsPerPrint;
     int          	m_numRowsCached;
     bool         	m_extendProv;
     bool 		m_ignoreSchemaDiff;

     time_t       	m_starttime, m_endtime;
     // src collection info
     std::vector<int>   m_srcCountVec;
     unsigned int 	m_evtCounterTotal;

     // for adding attributes
     coral::AttributeList       m_addedAttributes;
     TokenList          m_addedReferences;
 
     // Classes with shared cli keys and their argument properties
     CatalogInfo 	m_catinfo;
     QueryInfo   	m_queryinfo; 
     DstInfo     	m_dstinfo;
     SrcInfo     	m_srcinfo; 
     MetaInfo*          m_metainfo;
     Progress    	m_progress; 

     bool		m_committed;
     pool::CollectionService* m_collectionService;
     coral::MessageStream m_log;

     // Vector of args
     Args2Container     m_argsVec;

     bool               m_initialized;
   };

} // end pool namespace

#endif
