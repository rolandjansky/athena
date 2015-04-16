/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_METAINFO
#define UTILITIES_COLLECTION_METAINFO

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"
#include "CollectionUtilities/ICollMetaHandler.h"

#include "CollectionBase/ICollection.h"

#include <vector>
#include <map>
#include <string>

/**********************************************************

Metainfo is an concretization of a CmdLineArgs2
  to process metadata-related arguments
Now also deals with metadata copying

**********************************************************/

namespace pool
{
   
   class MetaInfo : public CmdLineArgs2
   {
  public:
     /// Constructors
     MetaInfo( const std::string& logname = "MetaInfo" );

     virtual            ~MetaInfo();

     MetaInfo (const MetaInfo&) = delete;
     MetaInfo& operator= (const MetaInfo&) = delete;

     /// process options
     virtual bool       evalArgs(std::vector<std::string>& argv);

     /// if -nometadata option was given
     virtual bool       noMetadata() const;

     /// add metadata entry [key, value]
     void 		addMetaEntry(ICollMetaHandler::MetaKey key, 
                                     ICollMetaHandler::MetaValue val, 
                                     ICollMetaHandler::ProvNode srcCollection="SELF");
 
     /// how many metadata entries do we have
     virtual size_t	nMeta() const;

     /// the copy mode requested
     ICollMetaHandler::CopyMode	copyMode() const;

     /// check for conflicts between input and output collections before copying metadata
     virtual bool       checkMetadata( std::vector<pool::ICollection*> inputs,
				       std::vector<pool::ICollection*> outputs );

     /// read metadata from source collections
     virtual void       readMetadata( std::vector<pool::ICollection*> inputs );

     /// write metadata to destination collections
     virtual void       writeMetadata(std::vector<pool::ICollection*> outputs);
   
     typedef  std::map< std::string, std::string >::const_iterator  MetaIterator;
     virtual MetaIterator       begin() const;
     virtual MetaIterator       end() const;
  
  protected:
     /// checks if this metadata item should be copied
     bool       shouldCopy( std::string key );
     
     /// do not copy source metadata
     bool      	m_noMetadata;
     // Copymode: affects how metadata is being copied
     ICollMetaHandler::CopyMode  	m_copyMode;
     
     /// List of metadata to keep
     std::vector<std::pair<std::string,std::string> > m_wildkeepers;

     /// The accumulated metadata entries.  No key duplication possible
      ICollMetaHandler::CollectionMetadata	m_metadata;

     /// Commandline metadata
     ICollMetaHandler::CollectionMetadata	m_cmdlineMetadata;  

     ICollMetaHandler* m_genericHandler;

     /// options list
     QualList 	m_markers;

     mutable coral::MessageStream m_log;
   };



   inline bool  MetaInfo::noMetadata()  const
   {
      return m_noMetadata;
   }

   inline MetaInfo::MetaIterator  MetaInfo::begin()  const
   {
      return m_metadata.begin();
   }

   inline MetaInfo::MetaIterator  MetaInfo::end()  const
   {
      return m_metadata.end();
   }


   inline size_t  MetaInfo::nMeta()  const
   {
      return m_metadata.size();
   }


   inline  ICollMetaHandler::CopyMode  MetaInfo::copyMode()  const
   {
      return m_copyMode;
   }

} // end pool namespace

#endif  

