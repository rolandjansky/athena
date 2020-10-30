/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RootCollectionMetadata.h"

#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"

#include <iostream>
using namespace std;

namespace pool {
   namespace RootCollection { 

      static char emptyStr[2] = { (char)0xFF, (char)0x00 }; 

      RootCollectionMetadata::RootCollectionMetadata() :
        m_mode(ICollection::READ),
        m_keyBranch(0),
        m_valBranch(0),
        m_charBuffer(0),
        m_poolOut(0)
      {
         m_hasKeys = false;
         m_tree = 0;
      }

      
      RootCollectionMetadata::~RootCollectionMetadata()
      {
	 delete[] m_charBuffer;
	 delete m_poolOut;
      }
  
        
      void
      RootCollectionMetadata::initialize( TFile* file, pool::ICollection::OpenMode mode )
      {
         const char* const c_treeName = "CollectionMetadata";
         m_poolOut = new coral::MessageStream("RootCollection::Metadata");
         
         m_mode = mode;
         // retrieve the TTree from file 
         if( mode == ICollection::READ || mode == ICollection::UPDATE ) {
            m_tree = dynamic_cast<TTree*>(file->Get(c_treeName));
            if( m_tree ) {
               m_keyBranch = m_tree->GetBranch( "Key" );
               m_valBranch = m_tree->GetBranch( "Value" );
            } else {
               *m_poolOut << coral::Info
			  << " ****** No metadata tree in the collection file"
			  << coral::MessageStream::endmsg;
               if( mode == ICollection::READ )
                  return;
            }
         }

         // create a new TTree
         if( !m_tree ) {
            m_tree = new TTree(c_treeName, file->GetName()); 
            if( m_tree ) {
               m_tree->SetDirectory( file );
               m_keyBranch = m_tree->Branch( "Key", 0, "Key/C" );
               m_valBranch = m_tree->Branch( "Value", 0, "Value/C" );
               *m_poolOut << coral::Info
			 << " ****** Created metadata tree in the collection file"
			 << coral::MessageStream::endmsg;
            }
         }

         if( !m_tree || !m_keyBranch || !m_valBranch ) {
            std::string errorMsg = (m_tree? "failed to set TBranches":"TTree creation failed");
            delete m_tree;
            m_tree = 0;
            *m_poolOut << coral::Error << " ****** " << errorMsg
		      << coral::MessageStream::endmsg;
            throw pool::Exception( errorMsg,
                                   "RootCollectionMetadata::initialize", 
                                   "RootCollection");
         }

      }

            
      unsigned long long
      RootCollectionMetadata::entries() const
      {
	 return m_tree? m_tree->GetEntries() : 0;
      }
	 

      void
      RootCollectionMetadata::readKeys() const
      {
         if( m_tree ) {
	    size_t valsize = m_valBranch->GetLeaf("Value")->GetLen();
	    size_t keysize = m_keyBranch->GetLeaf("Key")->GetLen();
	    size_t buffsize = ( valsize>keysize?  valsize : keysize );
	    if( buffsize < 4100 ) buffsize = 4100;
 	    *m_poolOut << coral::Debug << " Using char buffer of size: " << buffsize
		       << coral::MessageStream::endmsg; 
	    m_charBuffer = new char[buffsize];
            m_keyBranch->SetAddress( m_charBuffer );
	    *m_poolOut << coral::Verbose << " Key TTree entries: " << m_tree->GetEntries()
		       << coral::MessageStream::endmsg; 
            for( int n = 0; n < m_tree->GetEntries(); n++ ) {
               m_keyBranch->GetEntry(n);
               m_keyMap[ m_charBuffer ] = n;
               (*m_poolOut) << coral::Verbose << " readKeys: key " << n+1 << "=" << m_charBuffer
			 << coral::MessageStream::endmsg;
            }
         }
         m_hasKeys = true;
	 *m_poolOut << coral::Debug << " readKeys: found " << m_keyMap.size() << " metadata keys"
		    << coral::MessageStream::endmsg;
      }
              

      bool 
      RootCollectionMetadata::existsKey( const std::string& key ) const
      {
         if( !m_hasKeys )
            readKeys();
         return m_keyMap.find( key ) != m_keyMap.end();
      }

      
      const char* 
      RootCollectionMetadata::getValueForKey( const std::string& key ) const
      {
         if( !m_hasKeys )
            readKeys();
	 *m_poolOut << coral::Verbose << " getValueForKey: " << key
	    	    << coral::MessageStream::endmsg; 
         MetadataKeyMap_t::const_iterator keyI = m_keyMap.find( key );
         if( keyI == m_keyMap.end() )
            return 0;
         m_valBranch->SetAddress( m_charBuffer );
         m_valBranch->GetEntry( keyI->second );
         // protect against ROOT bug? when reading empty strings
         if( !strcmp(m_charBuffer, emptyStr) )
            m_charBuffer[0] = 0;
         return m_charBuffer;
      }

        
      void 
      RootCollectionMetadata::setValueForKey( const std::string& key,
                                              const std::string& val )
      {
         if( m_mode == ICollection::READ ) {
            throw pool::Exception( "Metadata update not allowed in Read-only mode",
                                   "RootCollectionMetadata::setValueForKey", 
                                   "RootCollection");
         }
         if( key.empty() ) {
            throw pool::Exception( "Attempt to set metadata with an empty Key",
                                   "RootCollectionMetadata::setValueForKey", 
                                   "RootCollection");
         } 
         m_keyBranch->SetAddress( (void*)key.c_str() );
         // protect against ROOT bug? dealing with empty strings - write a substitute
         m_valBranch->SetAddress( val.c_str()[0]? (void*)val.c_str() : (void*)emptyStr );
         Long64_t entry = m_tree->GetEntries();
         if( m_tree->Fill() <= 0 )
            throw pool::Exception( "TTree::Fill() failed",           
                                   "RootCollectionMetadata::setValueForKey", 
                                   "RootCollection");
         m_keyMap[ key ] = entry;
         // std::cout << " Stored (" << key <<", " << val << ") as entry " << entry << std::endl;
      }

      
      void 
      RootCollectionMetadata::commit()
      {
         Long64_t bytes = m_tree->AutoSave();
         coral::MessageStream pool_out("RootCollection::MetadataCommit");
         pool_out << coral::Debug << "Bytes written to the Metadata TTree " << bytes << coral::MessageStream::endmsg;
      }

      
      RootCollectionMetadata::RootCollectionMetadataIterator::
      RootCollectionMetadataIterator( const MetadataKeyMap_t::const_iterator& iter,
                                      const RootCollectionMetadata* mdata ) 
            : m_iterator( iter ),
              m_metadata( mdata )
      { }


      bool
      RootCollectionMetadata::RootCollectionMetadataIterator::
      operator==( const ICollectionMetadataIterator& _rhs ) const {
         const RootCollectionMetadataIterator* rhs
            = dynamic_cast<const RootCollectionMetadataIterator*>( &_rhs );
         if( !rhs )
            throw pool::Exception( "Incompatible iterator comparison",           
                                   "RootCollectionMetadataItorator::operator==", 
                                   "RootCollection");
         return m_iterator == rhs->m_iterator;
      }

             
             
      void RootCollectionMetadata::RootCollectionMetadataIterator::
      operator++() { ++m_iterator; }
             

      const std::string&
      RootCollectionMetadata::RootCollectionMetadataIterator::
      key() const
      { return m_iterator->first; }


      const char *
      RootCollectionMetadata::RootCollectionMetadataIterator::
      value() const
      { return m_metadata->getValueForKey(key()); }

        
        
      ICollectionMetadata::const_iterator
      RootCollectionMetadata::
      begin() const
      {
         if( !m_hasKeys )
            readKeys();
         return ICollectionMetadata::const_iterator(
             std::make_unique<RootCollectionMetadataIterator>( m_keyMap.begin(), this ) );
      }

        
      ICollectionMetadata::const_iterator
      RootCollectionMetadata::
      end() const
      {
         if( !m_hasKeys )   // check also here, in case End is evaluated before Begin
            readKeys();
         return ICollectionMetadata::const_iterator(
            std::make_unique<RootCollectionMetadataIterator>( m_keyMap.end(), this ) );
      }
        
   } // end namespace
}
