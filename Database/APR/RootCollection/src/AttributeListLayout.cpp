/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RootCollection.h"
#include "RootCollection/AttributeListLayout.h"

#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/CollectionFragment.h"
#include "CollectionBase/CollectionColumn.h"

#include "CoralBase/MessageStream.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

ClassImp(AttributeListLayout)

#include <iostream> 
using namespace pool;
using namespace std;
   
AttributeListLayout::AttributeListLayout()
{}

AttributeListLayout::~AttributeListLayout()
{}

AttributeListLayout::AttributeListLayout( const pool::ICollectionDescription& desc )
{
   for( int f_id = 0; f_id < desc.numberOfCollectionFragments(); f_id++ ) {

      for( int col_id = 0; col_id < desc.numberOfTokenColumns( f_id ); col_id++ ) {
         const ICollectionColumn&        column = desc.tokenColumn(col_id, f_id);
         m_layout.push_back( make_pair( column.name(), column.type() ) );
         m_annotations.push_back( column.annotation() );
      }

         
      for( int col_id = 0; col_id < desc.numberOfAttributeColumns( f_id ); col_id++ ) {
         const ICollectionColumn& column = desc.attributeColumn(col_id, f_id);
         m_layout.push_back( make_pair( column.name(), column.type() ) );
         m_annotations.push_back( column.annotation() );
      }
   }
   m_eventRefColumnName = desc.eventReferenceColumnName();
}


void AttributeListLayout::fillDescription( pool::CollectionDescription& desc )
{
   if( m_eventRefColumnName.empty() ) {
      m_eventRefColumnName = RootCollection::RootCollection::c_tokenBranchName;
   }
   desc.setEventReferenceColumnName( m_eventRefColumnName );
      
   size_t        annots_n = m_annotations.size();
   for( size_t i = 0; i < m_layout.size(); i++ ) {
      std::string column_name = m_layout[i].first;
      if( !i && column_name == "EVENT_REFERENCE" ) {
         // apparently some older collections stored "EVENT_REFERENCE" in the description
         column_name = m_eventRefColumnName;
      }
      const std::string& annotation = ( i<annots_n? m_annotations[i] : "" ); 
      desc.insertColumn( column_name, m_layout[i].second, annotation );
   }
}



void AttributeListLayout::print()const{
  coral::MessageStream log( "AttributeListLayout" );
  typedef std::vector< std::pair<std::string, std::string> >::const_iterator Iter;
  for(Iter iter=m_layout.begin();
      iter!=m_layout.end();
      ++iter)
    log  << coral::Info << iter->first << " \t"
         << iter->second << coral::MessageStream::endmsg;
}

