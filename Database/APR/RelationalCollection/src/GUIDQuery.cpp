/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GUIDQuery.h"
#include "RelationalCollectionNames.h"

#include "PersistentDataModel/Token.h"

#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISessionProperties.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/MessageStream.h"

#include "POOLCore/Exception.h"

#include "CollectionBase/ICollectionDescription.h"

#include <iostream>
using namespace std;


namespace pool {
   namespace RelationalCollection {

      // convert empty Tokens into zeroes
      string  guidFromToken( const Token& token ) {
         return token.dbID().toString();
      }

      
      void GUIDQuery::readGUIDs()
      {
         m_guids.clear();
         coral::MessageStream log( "pool::RelationalCollection::readGUIDs()" );

         if( m_session.properties().flavorName() != "Oracle" || m_selectedTokenColumnNames.size() <= 1 ) {
            // simple version for a single token, or non-oracle database
            for( std::set<std::string>::const_iterator
                    tokenNameI = m_selectedTokenColumnNames.begin(),
                    end = m_selectedTokenColumnNames.end();
                 tokenNameI != end; ++tokenNameI ) {
               log << coral::Debug
                   << "Querying GUIDs for token " << *tokenNameI << coral::MessageStream::endmsg;
               readGUIDs( *tokenNameI );
            }
            return;
         }

         // Oracle version  - single query for all tokens, using CASE syntax

         prepareQueryCondition();
         prepareQuery();

         // Create a subquery that produces a sequence (1,2,.., <number of tokens>)
         // used in the main query to iterate over the tokens in a single row
         string tokenSubQueryName = "token_seq";
         string tokenNumName = "token_num";
         coral::IQueryDefinition& subquery = m_query->defineSubQuery(tokenSubQueryName);
         subquery.addToTableList( RelationalCollectionNames::nameOfCollectionDescriptionsTable() );
         coral::AttributeList	bindings;
         bindings.extend(tokenNumName, typeid(unsigned));
         bindings[tokenNumName].data<unsigned>() = m_selectedTokenColumnNames.size();
         log << coral::Debug << "Selecting " << bindings[tokenNumName].data<unsigned>() << " tokens" << coral::MessageStream::endmsg;
         subquery.setCondition("rownum<= :"+tokenNumName, bindings);
         subquery.addToOutputList("rownum", tokenNumName);

         m_query->addToTableList( tokenSubQueryName );

         // create the case statements for the main query - one switch per token
         // it will extract the token link reference and token name
         std::ostringstream sqlTokID;
         std::ostringstream sqlTokName;
         sqlTokID << "case";
         sqlTokName << "case";
         int	tokenN = 1;
         for( std::set<std::string>::const_iterator
                 tokenNameI = m_selectedTokenColumnNames.begin(),
                 end = m_selectedTokenColumnNames.end();
              tokenNameI != end; ++tokenNameI )
         {
            string oid1ColumnName = m_tableTokenColumnPrefixForCollectionTokenColumnName.find( *tokenNameI )->second + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
            sqlTokID << " when " << tokenSubQueryName << "." << tokenNumName << "=" <<tokenN
                     << " then " << oid1ColumnName;
            sqlTokName << " when " << tokenSubQueryName << "." << tokenNumName << "=" <<tokenN
                       << " then " << "'" << *tokenNameI << "'";
            tokenN++;
         }
         sqlTokID << " end "; 
         sqlTokName << " end "; 
         m_query->addToOutputList( sqlTokID.str(), "TokenLink" );
         m_outputDataBuffer->extend( "TokenLink", typeid(unsigned) );

         m_query->addToOutputList( sqlTokName.str(), "TokenName" );
         m_outputDataBuffer->extend( "TokenName", typeid(string) );

         // Execute query and retrieve cursor for navigation over result.
         m_query->defineOutput( *m_outputDataBuffer );
         m_query->setDistinct();
	 
         coral::ICursor& cursor = m_query->execute();

         // map token link IDs into GUIDs using the link tables retrieved when opening the collection
         Token token;
         while( cursor.next() )  {
            unsigned linkId = cursor.currentRow()[0].data< unsigned >();
            const string& tokenName =  cursor.currentRow()[1].data< string >();
            const string& fragmentName = m_description.collectionFragmentName( tokenName );
            std::map< unsigned, std::string >* tokenKeyForLinkId = m_mapOfTokenKeyForLinkIdMaps.find( fragmentName )->second;
            token.fromString( tokenKeyForLinkId->find( linkId )->second );
            m_guids.insert( make_pair(guidFromToken(token), tokenName) );
            //cout << token.dbID() << "  " << tokenName << endl;
         }
      }

      
  
      void GUIDQuery::readGUIDs( const std::string& tokenName )
      {
         prepareQueryCondition();
         prepareQuery();
	   
         // Form the OID_1 column name of the Token.
         std::string oid1ColumnName = m_tableTokenColumnPrefixForCollectionTokenColumnName.find( tokenName ) ->second
            + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
     
         m_query->addToOutputList( oid1ColumnName );
         m_outputDataBuffer->extend( oid1ColumnName, typeid(unsigned) );

         // Execute query and retrieve cursor for navigation over result.
         m_query->defineOutput( *m_outputDataBuffer );
         m_query->setDistinct();
         coral::ICursor& cursor = m_query->execute();
      
         // map token link IDs into GUIDs using the link tables retrieved when opening the collection
         string fragmentName = m_description.collectionFragmentName( tokenName );
         std::map< unsigned, std::string >* tokenKeyForLinkId = m_mapOfTokenKeyForLinkIdMaps.find( fragmentName )->second;
         Token token;
         while( cursor.next() )  {
            unsigned linkId = cursor.currentRow()[0].data< unsigned >();
            token.fromString( tokenKeyForLinkId->find( linkId )->second );
            m_guids.insert( make_pair(guidFromToken(token),tokenName) );
         }

      }


      
      void	GUIDQuery::addToAttributeOutputList( const std::string& columnName ) 
      {  
         std::string errorMsg( "Attribute `" + columnName + "' is not a Token" );
         throw pool::Exception( errorMsg,
                                "RelationalCollectionGUIDQuery::addToOutputList",
                                "RelationalCollection" );
      }	

      
      const GUIDQuery::CountedGroupedGUIDs&
      GUIDQuery::getGroupedGUIDs()
      {
         m_groupedGUIDs.clear();
         coral::MessageStream log( "pool::RelationalCollection::getGroupedGUIDs()" );

         prepareQueryCondition();
         prepareQuery();

         /* create query in the form:
            SELECT COUNT(*), tokens GROUP BY tokens
            it will extract the token link references */
         m_query->addToOutputList( "count(*)", "count" );
         m_outputDataBuffer->extend( "count", typeid(unsigned) );
         string sep, groupby;
         for( std::set<std::string>::const_iterator
                 tokenNameI = m_selectedTokenColumnNames.begin(),
                 end = m_selectedTokenColumnNames.end();
              tokenNameI != end; ++tokenNameI )
         {
            string oid1ColumnName = m_tableTokenColumnPrefixForCollectionTokenColumnName.find( *tokenNameI )->second + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
            m_query->addToOutputList( oid1ColumnName, *tokenNameI );
            m_outputDataBuffer->extend( *tokenNameI, typeid(unsigned) );
            groupby += sep + oid1ColumnName;
            sep = ",";
            m_groupedGUIDs.tokenNames.push_back( *tokenNameI );
         }
         m_query->groupBy( groupby );

         // Execute query and retrieve cursor for navigation over result.
         m_query->defineOutput( *m_outputDataBuffer ); 
         coral::ICursor& cursor = m_query->execute();

         // map token link IDs into GUIDs using the link tables retrieved when opening the collection
         Token token;
         while( cursor.next() )  {
            countedGUIDGroup_t   row;
            row.first = cursor.currentRow()[0].data< unsigned >();
            for( std::set<std::string>::const_iterator
                    tokenNameI = m_selectedTokenColumnNames.begin(),
                    end = m_selectedTokenColumnNames.end();
                 tokenNameI != end; ++tokenNameI )
            {
               unsigned linkId = cursor.currentRow()[*tokenNameI].data< unsigned >();
               const string& fragmentName = m_description.collectionFragmentName( *tokenNameI );
               std::map< unsigned, std::string >* tokenKeyForLinkId = m_mapOfTokenKeyForLinkIdMaps.find( fragmentName )->second;
               token.fromString( tokenKeyForLinkId->find( linkId )->second );
               row.second.push_back( guidFromToken(token) );
            }
            // check for duplicated rows - result of an old linktable insert bug
            // if found, just sum up the count
            bool found(false);
            for( vector< countedGUIDGroup_t >::iterator rowi = m_groupedGUIDs.groupedGUIDRows.begin(),
                    end = m_groupedGUIDs.groupedGUIDRows.end();  rowi != end; ++rowi ) {
               bool match(true);
               for( unsigned t = 0; t < rowi->second.size(); t++ ) {
                  if( row.second[t] != rowi->second[t] ) {
                     match = false; break;
                  }
               }
               if( match ) {
                  rowi->first += row.first;
                  found = true;
                  break;
               }
            }
            if( !found )
               m_groupedGUIDs.groupedGUIDRows.push_back( row ); 
         }
         return m_groupedGUIDs;
      }
      
      
   }
}



