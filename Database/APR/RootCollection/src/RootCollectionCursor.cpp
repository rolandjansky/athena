/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RootCollectionCursor.h"

#include "CoralBase/Attribute.h"
#include "POOLCore/Exception.h"


#include <iostream>

pool::RootCollection::RootCollectionCursor::
RootCollectionCursor(
   const pool::ICollectionDescription& description,
   const pool::CollectionRowBuffer& collectionRowBuffer,
   TTree *tree,
   const TEventList *evl 
   )
      :
      m_description( description ),
      m_collectionRowBuffer( collectionRowBuffer ),
      m_eventList(evl),
      m_idx(-1),
      m_entries( evl? evl->GetN() : tree->GetEntries() ),
      m_dummyRef( false )
{
   for( coral::AttributeList::iterator attrI = m_collectionRowBuffer.attributeList().begin();
        attrI != m_collectionRowBuffer.attributeList().end();
        ++attrI ) {
      std::string        branchName = attrI->specification().name();
      TBranch* branch = tree->GetBranch( branchName.c_str() );
      if( !branch ) {
         std::string errorMsg = "Failed to retrieve TBranch " + branchName + " from the CollectionTree";
         throw pool::Exception( errorMsg, "RootCollectionCursor()", "RootCollection");
      }
      if( attrI->specification().type() == typeid(std::string) ) {
         branch->SetAddress( m_charBuffer );
         m_attrBranches.push_back( std::make_pair(branch, &attrI->data<std::string>()) );
      } else {
         branch->SetAddress( attrI->addressOfData() );         
         m_attrBranches.push_back( std::make_pair( branch, (std::string*)0 ) );
      }
   }

   if( m_description.eventReferenceColumnName() == "DummyRef" )  {
      // Athena "fake" collection with no Tokens
      m_dummyRef = true;
      return;  
   }
   
   for( pool::TokenList::iterator tokenI = m_collectionRowBuffer.tokenList().begin();
        tokenI != m_collectionRowBuffer.tokenList().end();
        ++tokenI ) {
      TBranch* branch = tree->GetBranch( tokenI.tokenName().c_str() );
      if( !branch ) {
         std::string errorMsg = "Failed to retrieve TBranch " + tokenI.tokenName() + " from the CollectionTree";
         throw pool::Exception( errorMsg, "RootCollectionCursor()", "RootCollection");
      }
      branch->SetAddress( m_charBuffer );
      m_tokenBranches.push_back( std::make_pair(branch, &*tokenI) );
   }
}


pool::RootCollection::RootCollectionCursor::~RootCollectionCursor()
{
   RootCollectionCursor::close();
   if(m_eventList) delete m_eventList;
}


void
pool::RootCollection::RootCollectionCursor::close()
{
}


bool
pool::RootCollection::RootCollectionCursor::next()
{
   if( ++m_idx >= m_entries ) {
      return false;
   }

   Long64_t entry( m_eventList? m_eventList->GetEntry(m_idx) : m_idx );

   // read attributes
   for( AttrBranchVector_t::const_iterator branchI = m_attrBranches.begin(); branchI != m_attrBranches.end(); ++branchI) {
      branchI->first->GetEntry(entry);
      if( branchI->second ) {
         // copy the read string from character buffer to std::string of the coral attribute
         *branchI->second = m_charBuffer;
      }
   }
   // read tokens
   for( TokenBranchVector_t::const_iterator branchI = m_tokenBranches.begin(); branchI != m_tokenBranches.end(); ++branchI) {
      branchI->first->GetEntry(entry);
      branchI->second->fromString( m_charBuffer );
   }

  
   // Get iterator over current row.
//  coral::AttributeList::const_iterator iData = m_cursor.currentRow().begin();

/* 
  cout << " * Cursor next(), values: " << endl;
  for( ; iData != m_cursor.currentRow().end(); ++iData ) {
      std::cout << "[";
      iData->toOutputStream( std::cout );
      std::cout << "] ";
  }
  cout << endl;  
  iData = m_cursor.currentRow().begin();
*/
  
  return true;
}


const pool::CollectionRowBuffer& 
pool::RootCollection::RootCollectionCursor::currentRow() const
{
  return m_collectionRowBuffer;
}


bool
pool::RootCollection::RootCollectionCursor::seek(long long int position)
{
   if( position >= m_entries ) {
      return false;
   }

   m_idx = position-1;

  return true;
}


int
pool::RootCollection::RootCollectionCursor::size()
{
  return m_entries;
}


const Token& 
pool::RootCollection::RootCollectionCursor::eventRef() const
{
   static const Token dummyToken;
   if( m_dummyRef )  return dummyToken; 
   return m_collectionRowBuffer.tokenList()[ m_description.eventReferenceColumnName() ];
}


