/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionExpressionParser.h"
#include "RelationalCollectionNames.h"

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/TokenList.h"

#include "POOLCore/Exception.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"

#include "CoralBase/MessageStream.h"

#include <sstream>
#include <iostream>
using namespace std;


// conversion from C++ to relational operators
// small test version
const string  operatorMap[][2] = {
   { "==", "=" },
   { "&&", " and " },
   { "||", " or " },
   { "", "" }
};

// replace C++ operators
// note - this will reduce any length of "====" into "="
bool replaceOperators( string& str )
{
   const size_t s = str.length();
   for( int i=0; !operatorMap[i][0].empty(); i++ ) {
      const string& patt = operatorMap[i][0];
      const size_t patsize = patt.length();
      if( s >= patsize && str.substr( s - patsize ) == patt ) {
	 str = str.substr( 0, s - patsize ) + operatorMap[i][1];
	 return true;
      }
   }
   return false;
}


std::string
pool::RelationalCollection::RelationalCollectionExpressionParser::parse( 
   const std::string& whereClause,
   const pool::ICollectionDescription& description,
   const std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
   const std::map< std::string, std::string >& linksTableNameForCollectionFragmentName,
   const std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
   const std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
   const std::map< std::string, std::map< std::string, unsigned >* >& mapOfLinkIdForTokenKeyMaps,
   std::vector< std::string >& collectionFragmentNames,
   std::vector< std::string >& linksTableNames,
   std::vector< std::string >& linkIDToOID1MatchingConditions,
   coral::AttributeList* attributeBindData,
   pool::TokenList* tokenBindData,
   coral::AttributeList* whereClauseBindData )
{
   std::ostringstream parsingOutput;

   std::vector< std::string > whereClauseFragments;
   std::string lastWord = "";
   std::string lastNonWord = "";
   bool withinQuotes = false;

   // Parse where clause into variables, values, key words and symbol sequences and store sequentially in a local vector.
   for( std::string::size_type i = 0; i < whereClause.size(); ++i )   {
      char c = whereClause[i];
      if( c == '\'' )     {
         if( withinQuotes ) {
            withinQuotes = false;
         }
         else {
            if( !lastWord.empty() ) {
               whereClauseFragments.push_back( lastWord );
               lastWord = "";
            }
            withinQuotes = true;
         }
         lastNonWord += c;
         continue;
      }

      if( withinQuotes )     {
         lastNonWord += c;
         continue;
      }

      if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
           c == '`' || c == '~' || c == '!' || c == '@' || c == '%' ||
           c == '^' || c == '&' || c == '*' || c == '(' || c == ')' ||
           c == '-' || c == '+' || c == '=' || c == '[' || c == ']' ||
           c == '{' || c == '}' || c == ';' || c == ':' || c == '"' ||
           c == '\\' || c == '|' || c == '<' || c == '>' || c == ',' ||
           c == '.' || c == '?' || c == '/' ) 
      {
         if( !lastWord.empty() )       {
            whereClauseFragments.push_back( lastWord );
            lastWord = "";
         }

         lastNonWord += c;
	 replaceOperators( lastNonWord );
      }
      else {
         if( lastNonWord.size() )  {
            if(  lastNonWord.size() > 2 && lastNonWord.find( " :" ) == ( lastNonWord.size() - 2 ) )        {
               lastWord = ":";
               whereClauseFragments.push_back( lastNonWord.substr(0, lastNonWord.size() - 1 ) );
            }
            else        {
               whereClauseFragments.push_back( lastNonWord );
            }
            lastNonWord = "";
         }

         lastWord += c;
      }
   }

   // Add whatever is left over.
   if( !lastWord.empty() )  {
      whereClauseFragments.push_back( lastWord );
   }
   else if( !lastNonWord.empty() )  {
      whereClauseFragments.push_back( lastNonWord );
   }

   // Create new vector of where clause fragments that use table column names instead of collection column names.
   std::vector< std::string > newWhereClauseFragments;
   unsigned pos = 0;
   for( std::vector< std::string >::const_iterator iWord = whereClauseFragments.begin(); 
        iWord != whereClauseFragments.end(); ++iWord, ++pos )
   {
      // cout << "Parsing: cond: " << *iWord << endl;
      bool	isAttribute(false),  isToken(false);
      // Look for where clause fragment in list of Attribute names.
      std::map< std::string, std::string >::const_iterator it = 
         tableAttributeColumnNameForCollectionAttributeColumnName.find( *iWord );
      if( it != tableAttributeColumnNameForCollectionAttributeColumnName.end() ) {
	 isAttribute = true;
      } else {
	 // Look for where clause fragment in list of Token names.
	 it = tableTokenColumnPrefixForCollectionTokenColumnName.find( *iWord );
	 if( it != tableTokenColumnPrefixForCollectionTokenColumnName.end() ) {
	    isToken = true;
	 }
      }
      // Get name of collection fragment that contains Attribute and add to output vector.
      std::string fragmentName;
      std::string columnName;
      if( isAttribute || isToken ) {
	 fragmentName = description.collectionFragmentName( *iWord );
	 columnName = it->second;
         for (const std::string& name : collectionFragmentNames )
	 {
	    if( name == fragmentName ) {
               // cppcheck-suppress invalidContainerLoop; ok: we break after this
	       collectionFragmentNames.push_back( fragmentName );
	       break;
	    }
	 }
      }

      if( isAttribute ) {
	 newWhereClauseFragments.push_back( columnName );	 
      }
      else if( isToken ) {
         // Get names of data and links tables that contain columns for Token.
         std::string dataTableName = ( dataTableNameForCollectionFragmentName.find( fragmentName ) )->second;
         std::string linksTableName = ( linksTableNameForCollectionFragmentName.find( fragmentName ) )->second;

         // Find names of columns for Token in data table.
         std::string tokenColumnPrefix = columnName;
         std::string oid1ColumnName = tokenColumnPrefix + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
         std::string oid2ColumnName = tokenColumnPrefix + RelationalCollectionNames::oid_2_variableInCollectionDataTable();

         // Find the next where clause fragment.
         std::string nextWord;
         if( whereClauseFragments.size() > pos+1 )   {
            nextWord = whereClauseFragments[pos+1];
         }

         // Check if where clause fragment is part of a Token column condition.
         if( !nextWord.empty() && nextWord[0] == '.' ) {
            // Is Token column condition. Increment iterator and position and get Token column name.
            ++iWord; ++pos;

            // Find name of Token column used for condition.
            std::string columnName;
            if( whereClauseFragments.size() > pos+1 )  {
               // Column name found. Increment iterator and position.
               columnName = whereClauseFragments[pos+1];
               ++iWord; ++pos;

	       bool	isOIDColumn =
		  ( columnName == RelationalCollectionNames::oid_1_variableInCollectionDataTable() ||
		    columnName == RelationalCollectionNames::oid_2_variableInCollectionDataTable() );
               // Check that column name belongs to the data or links table schema for Tokens.
               if ( isOIDColumn ||
                    columnName == RelationalCollectionNames::linkIdVariableInCollectionLinksTable() ||
                    columnName == RelationalCollectionNames::databaseIdVariableInCollectionLinksTable() ||
                    columnName == RelationalCollectionNames::classIdVariableInCollectionLinksTable() ||
                    columnName == RelationalCollectionNames::containerIdVariableInCollectionLinksTable() ||
                    columnName == RelationalCollectionNames::classIdVariableInCollectionLinksTable() ||
                    columnName == RelationalCollectionNames::technologyIdVariableInCollectionLinksTable() )
               {                
                  // Add table name and column name for Token column to new where clause fragment list.
		  newWhereClauseFragments.push_back( isOIDColumn? dataTableName : linksTableName );
                  newWhereClauseFragments.push_back( "." );
                  newWhereClauseFragments.push_back( columnName );

                  // Add additional query conditions if Token column of condition is in links table.
                  if( !isOIDColumn ) {
                     // Add links table name to list of table names to add to query.
		     if( std::find(linksTableNames.begin(), linksTableNames.end(), linksTableName)
			 == linksTableNames.end() ) {
			linksTableNames.push_back( linksTableName );
                     }
                     // Add condition to match Link ID column of links table with OID_1 column of data table.
                     std::string linkIdColumnName = linksTableName + "." + RelationalCollectionNames::linkIdVariableInCollectionLinksTable();
                     std::string condition = " " + oid1ColumnName + " = " + linkIdColumnName + " ";
                     bool foundCondition = false;
                     for( std::vector< std::string >::const_iterator iCondition = linkIDToOID1MatchingConditions.begin();
                          iCondition != linkIDToOID1MatchingConditions.end(); ++iCondition )
                     {
                        if ( *iCondition == condition ) {
                           foundCondition = true;
                           break;
                        }
                     }
                     if ( ! foundCondition ) {
                        linkIDToOID1MatchingConditions.push_back( condition );
                     }
                  }
               } else {
                  std::string errorMsg = "Badly formed Token column condition. Name `" + columnName + 
                     "' is an unrecognized Token column name.";
                  throw pool::Exception( errorMsg,
                                         "RelationalCollectionExpressionParser::parse",
                                         "RelationalCollection" );
               }
            } else {
               std::string errorMsg = "Badly formed Token column condition. No Token column name specified after string `" + 
                  whereClauseFragments[pos] + ".";
               throw pool::Exception( errorMsg,
                                      "RelationalCollectionExpressionParser::parse",
                                      "RelationalCollection" );
            }
         }
         // Check if where clause fragment is part of a Token condition.
         else {
            std::string comparator;
            std::string valueAsString;

            // Check if next where clause fragment to the right contains a comparison operator.
            if ( whereClauseFragments.size() > pos+1 ) {
               comparator = whereClauseFragments[pos+1];
            }
            if ( ( comparator.find( "=" ) < comparator.find( "(" ) && 
                   comparator.find_first_not_of( " =(" ) == comparator.npos ) ||
                 ( comparator.find( "<" ) < comparator.find( "(" ) &&
                   comparator.find_first_not_of( " <(" ) == comparator.npos ) ||
                 ( comparator.find( ">" ) < comparator.find( "(" ) &&
                   comparator.find_first_not_of( " >(" ) == comparator.npos ) )
            {
               // Comparison operator found. Increment iterator and position.
               ++iWord;
               ++pos;

               // Look for the Token value in the next where clause fragment to the right.
               if ( whereClauseFragments.size() > pos+1 ) {
                  // Token value found. Increment iterator and position.
                  valueAsString = whereClauseFragments[pos+1];
                  ++iWord;
                  ++pos;
               }
               else {
                  std::string errorMsg = "Badly formed Token condition. No value found for Token '" + *iWord + "'.";
                  throw pool::Exception( errorMsg,
                                         "RelationalCollectionExpressionParser::parse",
                                         "RelationalCollection" );
               }
            }
            else {
               // Check if next where clause fragment to the left contains a Token comparison operator.
               if( pos > 0 ) {
                  comparator = whereClauseFragments[pos-1];
               }
               if( ( comparator.find( "=" ) > comparator.find( ")" ) && 
                     comparator.find_first_not_of( " =)" ) == comparator.npos ) ||
                   ( comparator.find( "<" ) > comparator.find( ")" ) &&
                     comparator.find_first_not_of( " <)" ) == comparator.npos ) ||
                   ( comparator.find( ">" ) > comparator.find( ")" ) &&
                     comparator.find_first_not_of( " >)" ) == comparator.npos ) )
               {
                  // Comparison operator found. Temporarily remove from new where clause fragment vector.
                  newWhereClauseFragments.pop_back();

                  // Look for Token value.
                  if( pos > 1 ) {
                     // Look for the Token value in the next where clause fragment to the left.
                     valueAsString = whereClauseFragments[pos-2];
                     newWhereClauseFragments.pop_back();
                  }
                  else {
                     std::string errorMsg = "Badly formed Token condition. No value found for Token '" + *iWord + "'.";
                     throw pool::Exception( errorMsg,
                                            "RelationalCollectionExpressionParser::parse",
                                            "RelationalCollection" );
                  }
               }
               else {
                  std::string errorMsg = "Badly formed Token condition. No valid comparison operator found for Token '" + *iWord + "'.";
                  throw pool::Exception( errorMsg,
                                         "RelationalCollectionExpressionParser::parse",
                                         "RelationalCollection" );
               }
            }
         
            // Get Token column values and bind them if necessary.
            unsigned oid1ColumnValue = 0;
            unsigned oid2ColumnValue = 0;
            std::map< std::string, unsigned >* linkIdForTokenKey =
               ( mapOfLinkIdForTokenKeyMaps.find( fragmentName ) )->second;
            if ( !valueAsString.empty() && valueAsString[0] == ':' )
            {
               // Get bind variable for Token.
               std::string bindVariable = valueAsString.substr( 1 );

               // Check if Token bind variable defined in input Token bind data buffer.
               bool foundBindVariable = false;
               for ( pool::TokenList::const_iterator iToken = tokenBindData->begin(); iToken != tokenBindData->end(); ++iToken )
               {
                  if ( iToken.tokenName() == bindVariable ) {
                     foundBindVariable = true;
                  }
               }
               if( !foundBindVariable ) {
                  std::string errorMsg = "Badly formed Token condition. Could not find bind data variable `" +
                     bindVariable + "' in input Token bind data buffer.";
                  throw pool::Exception( errorMsg,
                                         "RelationalCollectionExpressionParser::parse",
                                         "RelationalCollection" );
               }

               // Get Token column values from input bind data.
               oid1ColumnValue = ( linkIdForTokenKey->find( (*tokenBindData)[ bindVariable ].key() ) )->second;
               oid2ColumnValue = static_cast<unsigned>( (*tokenBindData)[ bindVariable ].oid().second );

               // Add Token column values to where clause bind data buffer.
               whereClauseBindData->extend( oid1ColumnName, typeid(unsigned) );
               whereClauseBindData->extend( oid2ColumnName, typeid(unsigned) );
               (*whereClauseBindData)[ oid1ColumnName ].data<unsigned>() = oid1ColumnValue;
               (*whereClauseBindData)[ oid2ColumnName ].data<unsigned>() = oid2ColumnValue;
            }
            else
            {
               // Get Token column values from where clause fragment.
               Token token;
               token.fromString( valueAsString );
               oid1ColumnValue = ( linkIdForTokenKey->find( token.key() ) )->second;
               oid2ColumnValue = static_cast<unsigned>( token.oid().second );
            }

            // Get string representations of Token column values.
            std::string oid1ColumnValueAsString = "";
            std::ostringstream oid1Stream;
            oid1Stream << oid1ColumnValue;
            oid1ColumnValueAsString = oid1Stream.str();
            std::string oid2ColumnValueAsString = "";
            std::ostringstream oid2Stream;
            oid2Stream << oid2ColumnValue;
            oid2ColumnValueAsString = oid2Stream.str();

            // Add Token column condition to new where clause fragment vector.
            newWhereClauseFragments.push_back( oid1ColumnName );
            newWhereClauseFragments.push_back( comparator );
            newWhereClauseFragments.push_back( oid1ColumnValueAsString );
            newWhereClauseFragments.push_back( " AND " );
            newWhereClauseFragments.push_back( oid2ColumnName );
            newWhereClauseFragments.push_back( comparator );
            newWhereClauseFragments.push_back( oid2ColumnValueAsString );
         }
      }
      // Check if where clause fragment is a bind variable.
      else if( iWord->find( ":" ) == 0 )   {
         // Get bind variable for Attribute.
         std::string bindVariable = iWord->substr( 1 );

         // Look for bind variable in input Attribute bind data buffer
         // and add to output Attribute bind data buffer if found.
         bool foundBindVariable = false;
         for( coral::AttributeList::const_iterator iAttribute = attributeBindData->begin();
              iAttribute != attributeBindData->end(); ++iAttribute )
         {
            if ( iAttribute->specification().name() == bindVariable )
            {
               foundBindVariable = true;
               whereClauseBindData->extend( bindVariable,
                                            (*attributeBindData)[ bindVariable ].specification().type() );
               (*whereClauseBindData)[ bindVariable ] = (*attributeBindData)[ bindVariable ];
               break;
            }
         }
         // If bind variable not found in input Attribute bind data buffer verify that it is in input Token data buffer. 
         if ( ! foundBindVariable ) {
            for( pool::TokenList::const_iterator iToken = tokenBindData->begin(); iToken != tokenBindData->end(); ++iToken )
            {
               if( iToken.tokenName() == bindVariable ) {
                  foundBindVariable = true;
               }
            }
            if ( ! foundBindVariable ) {
               std::string errorMsg = "Badly formed bind variable expression. Could not find bind data variable `" + 
                  bindVariable + "' in input Attribute or Token data buffers.";
               throw pool::Exception( errorMsg,
                                      "RelationalCollectionExpressionParser::parse",
                                      "RelationalCollection" );
            }
         }

         // Add ":" and bind data variable to new where clause fragments vector unchanged.
         newWhereClauseFragments.push_back( *iWord );
      } 
      // Add all other types of where clause fragments to new where clause fragments vector unchanged.
      else  {
         // cout << " Query parse:  adding word '" << *iWord << "'" << endl;
         newWhereClauseFragments.push_back( *iWord );
      }
   }

   // Add all new where clause fragments to the output string sequentially.
   for (const std::string& fragment : newWhereClauseFragments )
   {
      parsingOutput << fragment;
   }

   coral::MessageStream log( "pool::RelationalCollectionExpressionParser::parse()" );
   log << coral::Debug
       << "Parsed query: " <<  parsingOutput.str() << coral::MessageStream::endmsg;
   
   return parsingOutput.str();
}

