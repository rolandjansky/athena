/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONEXPRESSIONPARSER_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONEXPRESSIONPARSER_H

#include <string>
#include <vector>
#include <map>


namespace coral {
  class AttributeList;
}

namespace pool {

  class ICollectionDescription;
  class TokenList;

  namespace RelationalCollection {

    /**
     * @class RelationalCollectionExpressionParser RelationalCollectionExpressionParser.h src/RelationalCollectionExpressionParser.h
     *
     * A utility class for parsing a user submitted where clause.
     *
     */

    class RelationalCollectionExpressionParser
    {
    public:
      /**
       * Parses a user provided where clause by converting Attribute and Token names of a collection
       * to their corresponding data table column names. The where clause may also contain conditions
       * on the individual columns used to form a Token in the data and links tables. In that case the
       * user should provide the column name in the form "<Token name>.<column name>" where "<column name>"
       * refers to the actual name of the column in the data or links table of the Token in this case.
       * If bind data is provided in an Attribute or Token input data buffer then the same names should
       * be used in the where clause but preceded by the ":" symbol. Token column condition bind data should
       * be included in the Attribute input data buffer.
       *
       * @param whereClause Where clause provided by user.
       * @param description Specification of collection properties.
       * @param dataTableNameForCollectionFragmentName Map of data table names using names of corresponding collection fragment names as keys.
       * @param linksTableNameForCollectionFragmentName Map of links table names using names of corresponding collection fragment names as keys.
       * @param tableTokenColumnPrefixForCollectionTokenColumnName Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
       * @param tableAttributeColumnNameForCollectionAttributeColumnName Map of table column names for Attributes using names of corresponding collection columns as keys.
       * @param mapOfLinkIdForTokenKeyMaps Map of maps of link table ID's that use corresponding Token prefixes as keys, using collection fragment names as keys.
       * @param collectionFragmentNames Names of collection fragments needed by query. Returned by reference.
       * @param linksTableNames Names of links tables needed by query. Returned by reference.
       * @param linkIDToOID1MatchingConditions Links table to data table matching conditions needed by query. Returned by reference. 
       * @param attributeBindData Attribute input bind data.
       * @param tokenBindData Token input bind data.
       * @param whereClauseBindData Data buffer containing all bind data needed by query. Returned by reference.
       */
      static std::string parse(
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
                  coral::AttributeList* whereClauseBindData );
    };
  
  }

}

#endif
