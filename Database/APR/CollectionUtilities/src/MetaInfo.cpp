/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "CollectionBase/CollectionBaseNames.h"
#include "CoralBase/MessageStream.h"

#include <algorithm>
#include <iostream>

#include "CollectionUtilities/MetaInfo.h"
#include "CollectionUtilities/GenericMetaHandler.h"
#include "CollectionUtilities/CollMetaRegistry.h"

/**
 * @class MetaInfo
 * 
 * @brief MetaInfo is an concretization of a CmdLineArgs2
 *   to process metadata-related arguments
 *   Now also deals with metadata copying
 */

#define corENDL coral::MessageStream::endmsg

using namespace std;
using namespace pool;

MetaInfo::MetaInfo( const std::string& logname ) :
      CmdLineArgs2( logname )
      , m_noMetadata( true )
      , m_copyMode( ICollMetaHandler::merge )
      , m_log( logname )
{
   m_markers.insert( std::make_pair("-metadata",    ArgQual(2,true,false)) );
   m_markers.insert( std::make_pair("-copy",        ArgQual(0)) );
   m_markers.insert( std::make_pair("-extract",     ArgQual(0)) );
   m_markers.insert( std::make_pair("-merge",       ArgQual(0)) );
   m_markers.insert( std::make_pair("-nometadata",  ArgQual(0)) );
   m_markers.insert( std::make_pair("-metaopt",     ArgQual(1,true,false)) );
   m_markers["-metadata"].desc << "<metadata key> <metadata value> " << std::endl
                               << "List of space separated key-value pairs of metadata entries "
                               << "that will be added to the output collection(s). "
                               << "Keys are automatically wildcarded, so key='DOH' "
                               << "includes '*DOH*' key entries. DEFAULT=keepAll";
   m_markers["-copy"].desc << "Make exact copy of 1 source collection ";
   m_markers["-extract"].desc << "Combine N source collections, prepend existing metadata keys "
                              << "with source collection name. "
                              << "Add new metadata without prefix, include metadata for: "
                              << "InputCollections, OutputCollections, Query, ExtractNode, TimeBegin, TimeEnd. "
                              << "(useful for creating standalone destination collections)";
   m_markers["-merge"].desc << "Combine N source collections, prepend existing metadata keys "
                            << "with source collection name. "
                            << "Add user supplied metadata with source collection name prefix. " 
                            << "(useful for appending to exiting destination collections). This is the DEFAULT. ";
   m_markers["-nometadata"].desc << "Ignore metadata. "
                                 << "(will still accept new metadata from the command line -metadata option)"
                                 << "Same as -metaopt 'NONE' (DEPRECATED)";
   m_markers["-metaopt"].desc << "Space-separated list of input metadata keys to retain in output. \n"
                              << "DEFAULT='NONE' (same as -nometadata), keyword 'ALL' also available.";

   setArgQuals(m_markers);
   m_genericHandler = new GenericMetaHandler();
}


MetaInfo::~MetaInfo()
{
   delete m_genericHandler;
}



bool
MetaInfo::evalArgs(std::vector<std::string>& argv)
{
   if( !CmdLineArgs2::evalArgs(argv) )
      return false;

   vector<string> newmetadata;
   if( getOpts("-metadata", newmetadata) ) {
      for( size_t i = 0; i < newmetadata.size(); i+=2 ) {
	 const string& key = newmetadata[i], val = newmetadata[i+1];
         m_cmdlineMetadata[ key ] = val;
      }
   } 

   if( this->hasQual("-copy") ) {
      m_copyMode = ICollMetaHandler::copy;
   }
   if ( this->hasQual("-extract")) {
      m_copyMode = ICollMetaHandler::extract;
   }
   if ( this->hasQual("-merge")) {
      m_copyMode = ICollMetaHandler::merge;
   }

   vector<string> keepkeys;
   if( getOpts("-metaopt", keepkeys) ) {
      if      ( keepkeys[0] == "ALL" )  m_noMetadata=false;
      else if ( keepkeys[0] == "NONE" ) m_noMetadata=true;
      else {
         m_noMetadata=true;
         for( size_t i = 0; i < keepkeys.size() ; ++i) {
            // Check for any wildcarding
            std::string::size_type sep = keepkeys[i].find("*");
            if (sep == std::string::npos) {
               //std::cerr << "Keeping " << keepkeys[i] << std::endl;
               m_wildkeepers.push_back(std::make_pair(keepkeys[i],""));
            }
            else { 
               // std::cerr << "Found a wildcard" << std::endl;
               std::pair<std::string,std::string> key;
               key.first = keepkeys[i].substr(0,sep);
               key.second = keepkeys[i].substr(sep+1,keepkeys[i].size());
               if (key.first.find("*")==std::string::npos && 
                   key.second.find("*")==std::string::npos) m_wildkeepers.push_back(key);
               else std::cerr << "Multiple wildcards not supported, disregarding " << keepkeys[i] << std::endl;
            }
         }
      }
   }
   
   else if ( this->hasQual("-nometadata")) {
      m_noMetadata = true;
   }

   return true;
}


/*
void 
MetaInfo::addMetaEntry(std::string key, std::string val)
{
   // overwrites existing key values (by design)
   // In case this is not good, we need to expand API
   m_metadata[ key ] = val;
}
*/



bool MetaInfo::shouldCopy(string key)
{
   if( !m_noMetadata )
      return true;
   
   bool keep=false;
   if( m_wildkeepers.size() ) {
      for (unsigned int i=0; i < m_wildkeepers.size(); ++i) {
         std::pair<std::string,std::string> test = m_wildkeepers[i];
         if (!test.first.empty() && test.second.size()) {
            //std::cout << "Look " << test.first << " && " << test.second << " in " << key << std::endl;
            if (key.find(test.first)  != std::string::npos && 
                key.find(test.second) != std::string::npos) keep=true;
         }
         else if (!test.first.empty()) {
            //std::cout << "Look " << test.first << " in " << key << std::endl;
            if (key.find(test.first)  != std::string::npos) keep=true;
         }
         else if (!test.second.empty()) {
            //std::cout << "Look " << test.second << " in " << key << std::endl;
            if (key.find(test.second) != std::string::npos) keep=true;
         }
      }
   }
   return keep;
}


bool 
MetaInfo::checkMetadata( std::vector<pool::ICollection*> inputs,
			 std::vector<pool::ICollection*> outputs )
{
   if( m_noMetadata || m_copyMode != ICollMetaHandler::merge ) {
      return true;
   }
   for( vector<ICollection*>::const_iterator input = inputs.begin(),
	   end = inputs.end(); input != end; ++input )
   {
      ICollection* const &srcColl = *input;
      ICollectionMetadata &srcMeta =  srcColl->metadata();
      string srcCollName = srcColl->description().name();
      if( srcMeta.existsKey("Name") ) {
	 srcCollName = srcMeta.getValueForKey("Name");
	 m_log << coral::Debug << "Input collection "
	       << srcColl->description().name()
	       << " has internal metadata Name=" << srcCollName << corENDL;
      }

      long long sm_n = srcMeta.entries();
      for( vector<ICollection*>::const_iterator output = outputs.begin(),
	      end = outputs.end(); output != end; ++output )
      {
	 ICollection* const	&destColl = *output;
	 ICollectionMetadata &dstMeta =  destColl->metadata();
	 long long dm_n = dstMeta.entries();

	 bool exists = false;
	 if( sm_n * 10000 < dm_n ) {
	    m_log << coral::Info << "Destination collection metadata has " << dm_n << " entries. "
		  << " Checking for KEY conflicts with single queries." << corENDL;
	    for( ICollectionMetadata::const_iterator it = srcMeta.begin(),
		    end = srcMeta.end(); it != end; ++it ) {
	       if( dstMeta.existsKey(  srcCollName + "|" + it.key() ) ) {
		  exists = true;
		  break;
	       }
	    }	    
	 } else {
	    for( ICollectionMetadata::const_iterator it = dstMeta.begin(),
		    end = dstMeta.end(); it != end; ++it ) {
	       if( it.key().compare(0, srcCollName.length()+1, srcCollName+"|" ) == 0 ) {
		  exists = true;
		  break;
	       }
	    }
	 }
	 if( exists ) {
	    m_log << coral::Warning << "*** Metadata from source collection "
		  << srcCollName << " already exists in destination collection "
		  << destColl->description().name()
		  << ". It will NOT be copied to any collection." << corENDL;
	    return false;
	 }
      }
   }
   return true;
}



void MetaInfo::addMetaEntry ATLAS_NOT_THREAD_SAFE
                           (ICollMetaHandler::MetaKey key,
                            ICollMetaHandler::MetaKey val,
                            ICollMetaHandler::ProvNode srcCollection)
{
   // std::cout << "+++    key= " << key << "; val = " << val << std::endl;
   ICollMetaHandler* h = m_genericHandler;
   CollMetaRegistry* metareg = CollMetaRegistry::Instance();
   if (metareg==0) {
      std::cout << "Could not retrieve CollMetaRegistry" << std::endl;
      return;
   }
   // see if there is a special handler for this key
   CollMetaRegistry::Container::const_iterator it = metareg->seek(key);
   if( it != metareg->end() )
      h = it->second;
   h->process(m_copyMode, val, m_metadata, srcCollection, key);
}



void
MetaInfo::readMetadata ATLAS_NOT_THREAD_SAFE ( std::vector<pool::ICollection*> inputs )
{
   for( vector<ICollection*>::const_iterator input = inputs.begin(),
	   end = inputs.end(); input != end; ++input )
   {
      ICollection* const &srcColl = *input;
      ICollectionMetadata &srcMeta =  srcColl->metadata();
      string srcCollName = srcColl->description().name();
      if( srcMeta.existsKey("Name") ) {
	 srcCollName = srcMeta.getValueForKey("Name");
      }

      for( ICollectionMetadata::const_iterator sit = srcMeta.begin(),
              end = srcMeta.end(); sit != end; ++sit )
      {
         const string &key = sit.key();
         if( shouldCopy(key) )
            addMetaEntry( key, sit.value(), srcCollName );
      }
      m_log << coral::Info << "Source metadata size after processing collection "
            << srcCollName << " is " <<  m_metadata.size() << corENDL;

      // add new metadata from the commandline
      //  ignores -nometadata option  m_cmdlineMetadata[
      for( MetaIterator i = m_cmdlineMetadata.begin(), end = m_cmdlineMetadata.end();
	   i != end; ++i ) {
         const string &key = i->first;
         if( shouldCopy(key) ) {
            addMetaEntry( key, i->second );
            m_log << coral::Debug << "adding commandline metadata " << key
                  << "=" << i->second << corENDL;
         }
      }      
   } 
}


void
MetaInfo::writeMetadata ATLAS_NOT_THREAD_SAFE (std::vector<pool::ICollection*> outputs)
{
   // write all metadata
   for( vector<ICollection*>::const_iterator output = outputs.begin(),
	   end = outputs.end(); output != end; ++output )
   {
      ICollection* const &destColl = *output;
      for( MetaInfo::MetaIterator mit = this->begin(), end = this->end(); mit != end; ++mit ) {
         const string &key = mit->first;
	 if (!destColl->metadata().existsKey( key )) {
	    destColl->metadata().setValueForKey(key, mit->second);
	 } else {
            // merge values for existing keys
	    if( key == CollectionBaseNames::CollIDMdataKey() )
               continue;  // ignore the GUID entry
            string dstval( destColl->metadata().getValueForKey(key) );
            // first add it to the container
            addMetaEntry(key, dstval);
            // then just take it from the container
            destColl->metadata().setValueForKey( key, m_metadata[key] );
	 }
         m_log << coral::Debug <<  "Metadata insert key=" <<  key << ", value len="
               <<  m_metadata[key].length() << corENDL;
      }
   }
}

