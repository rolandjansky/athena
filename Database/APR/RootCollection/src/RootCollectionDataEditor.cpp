/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RootCollection.h"
#include "RootCollectionDataEditor.h"

#include "PersistentDataModel/Token.h"

#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/ICollectionColumn.h"


#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"

#include "TBranch.h"

//#include <iostream>
//using namespace std;

const unsigned int c_maxLengthOfStrings = 5000;


pool::RootCollection::RootCollectionDataEditor::RootCollectionDataEditor(
   const pool::ICollectionDescription& description,
   TTree *tree,
   coral::MessageStream &log
//   const pool::CollectionRowBuffer& collectionRowBuffer,
   ) :
      m_description( description ),
      m_tree( tree ),
      m_poolOut( &log )
{
   clearRowBuffers();
}



pool::CollectionRowBuffer&
pool::RootCollection::RootCollectionDataEditor::rowBuffer()
{
   return m_collectionRowBuffer;
}


pool::CollectionRowBuffer&
pool::RootCollection::RootCollectionDataEditor::emptyRowBuffer()
{
   clearRowBuffers();
   return m_collectionRowBuffer;
}



void
pool::RootCollection::RootCollectionDataEditor::clearRowBuffers()
{
   // Create empty collection and data table row buffers
   pool::TokenList                 tokenList;
   coral::AttributeList         attributeList;
   for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )
   {
      std::string         fragmentName = m_description.collectionFragment( i ).name();
      for( int j = 0; j < m_description.numberOfTokenColumns( fragmentName ); j++ ) {
         const std::string& collectionColumnName = m_description.tokenColumn( j, fragmentName ).name();
         tokenList.extend( collectionColumnName );
      }
      for( int j = 0; j < m_description.numberOfAttributeColumns( fragmentName ); j++ ) {
         const std::string& collectionColumnName = m_description.attributeColumn( j, fragmentName ).name();
         const std::string& columnType = m_description.attributeColumn( j, fragmentName ).type();
         attributeList.extend( collectionColumnName, columnType );
      }
   }
   m_collectionRowBuffer.setTokenList( tokenList );
   m_collectionRowBuffer.setAttributeList( attributeList );
}



void
pool::RootCollection::RootCollectionDataEditor::
insertRow( const pool::CollectionRowBuffer& inputRowBuffer, bool )
{
   std::map< std::string, TBranch* > branchByName;

   const TObjArray* branches = m_tree->GetListOfBranches();
   Int_t nbranches = branches->GetEntriesFast();
   for(int i = 0; i < nbranches; ++i) {
      TBranch* branch = (TBranch*)branches->UncheckedAt(i);
      branchByName[ branch->GetName() ] = branch;
      // cout << "RootCollection:AddRow: TTree Branch " <<  branch->GetName() << " at " << branch << endl;
   }

   std::vector<std::string> stringBuffer;

   for( pool::TokenList::const_iterator iToken = inputRowBuffer.tokenList().begin();
         iToken != inputRowBuffer.tokenList().end(); ++iToken )  {
      stringBuffer.push_back( iToken->toString() );
      branchByName[ iToken.tokenName() ]->SetAddress( (void*)stringBuffer.back().c_str() );
   }
   for( coral::AttributeList::const_iterator iAttribute = inputRowBuffer.attributeList().begin();
        iAttribute != inputRowBuffer.attributeList().end(); ++iAttribute ) {
      if( iAttribute->specification().type() == typeid(std::string) ) {
         const std::string        &str = iAttribute->data<std::string>();
         if( str.length()+1 >= c_maxLengthOfStrings ) {
          throw pool::Exception( "String is too long",
                                 "RootCollectionDataEditor:insertRow", 
                                 "RootCollection" );
         }
         branchByName[ iAttribute->specification().name() ]->SetAddress( (void*)str.c_str() );
      } else {
         branchByName[ iAttribute->specification().name() ]->SetAddress( (void*)iAttribute->addressOfData() );
      }
   }
   
   if( m_tree->Fill() <= 0 )
      throw pool::Exception( "TTree::Fill() failed",
                             "RootCollectionDataEditor:insertRow", 
                             "RootCollection" );
   *m_poolOut << coral::Debug << "Collection size=" << m_tree->GetEntries() << coral::MessageStream::endmsg;
}



int
pool::RootCollection::RootCollectionDataEditor::
updateRows( coral::AttributeList* /* attributeSetList */,
            TokenList* /* tokenSetList */,
            const std::string& /* whereClause */,
            coral::AttributeList* /* attributeBindData */,
            pool::TokenList* /* tokenBindData */ )
{
   throw pool::Exception( "Method not implemented ",
                          "RootCollectionDataEditor::updateRows", 
                          "RootCollection");
   return 0;
}



int
pool::RootCollection::RootCollectionDataEditor::
deleteRows( const std::string& /* whereClause */,
            coral::AttributeList* /* attributeBindData */,
            pool::TokenList* /* tokenBindData */,
            bool  )
{
   throw pool::Exception( "Method not implemented ",
                          "RootCollectionDataEditor::deleteRows", 
                          "RootCollection");
   return 0;  
}

