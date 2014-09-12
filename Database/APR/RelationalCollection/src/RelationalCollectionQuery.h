/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONQUERY_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONQUERY_H

#include "CollectionBase/ICollectionQuery.h"

#include <map>
#include <set>
#include <vector>


namespace coral {
  class ISessionProxy;
  class IQuery;
}

namespace pool {

  class ICollectionDescription;

  namespace RelationalCollection {

    /** 
     * @class RelationalCollectionQuery RelationalCollectionQuery.h src/RelationalCollectionQuery.h
     *
     * Implementation of the ICollectionQuery interface used to query a collection.
     */
    class RelationalCollectionQuery : virtual public pool::ICollectionQuery
    {
    public:
      /**
       * Constructor: Selects the primary key and the event reference Token (if one exists for the top
       * level collection fragment) and adds the data table of the top level collection fragment to 
       * the query table list.
       *
       * @param session Reference to current database access session.
       * @param description Specification of collection properties.
       * @param dataTableNameForCollectionFragmentName Map of data table names using names of corresponding collection fragment names as keys.
       * @param linksTableNameForCollectionFragmentName Map of links table names using names of corresponding collection fragment names as keys.
       * @param tableTokenColumnPrefixForCollectionTokenColumnName Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
       * @param tableAttributeColumnNameForCollectionAttributeColumnName Map of table column names for Attributes using names of corresponding collection columns as keys.
       * @param tableAttributeList List of all Attributes defined by collection but using table column names.
       * @param mapOfLinkIdForTokenKeyMaps Map of maps of link table ID's that use corresponding Token prefixes as keys, using collection fragment names as keys.
       * @param mapOfTokenKeyForLinkIdMaps Map of maps of Token prefixes that use corresponding link table ID's as keys, using collection fragment names as keys.
       * @param readPrimaryKey - if true (the default) add primary key to the query select list - don't change
       */
      RelationalCollectionQuery( 
        coral::ISessionProxy& session,
        const pool::ICollectionDescription& description,
        const std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
        const std::map< std::string, std::string >& linksTableNameForCollectionFragmentName,
        const std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
        const std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
        const coral::AttributeList& tableAttributeList,
        const std::map< std::string, std::map< std::string, unsigned >* >& mapOfLinkIdForTokenKeyMaps,
        const std::map< std::string, std::map< unsigned, std::string >* >& mapOfTokenKeyForLinkIdMaps,
	bool readPrimaryKey = true
	);

      /// Default destructor.
      virtual ~RelationalCollectionQuery();

      RelationalCollectionQuery (const RelationalCollectionQuery&) = delete;
      RelationalCollectionQuery& operator= (const RelationalCollectionQuery&) = delete;

      /**
       * Adds a column to the query select list.
       *
       * @param columnName Name of column to select.
       */
      virtual void addToOutputList( const std::string& columnName );

      /**
       * Adds one or more columns to the query select list.
       *
       * @param columnNames Names of columns to select.
       */
      virtual void addToOutputList( const std::vector<std::string>& columnNames );

      /// Adds all Attribute columns to the query select list.
      virtual void selectAllAttributes();

      /// Adds all Token columns to the query select list.
      virtual void selectAllTokens();

      /// Adds all Token and Attribute columns to the query select list.
      virtual void selectAll();

      /**
       * Adds the data table associated with a collection fragment to the query table list.
       *
       * @param fragmentName Name of collection fragment to add.
       */
      virtual void addToCollectionFragmentList( const std::string& fragmentName );

      /**
       * Adds the data tables associated with one or more collection fragments to the query 
       * table list.
       *
       * @param fragmentNames Names of collection fragments to add.
       */
      virtual void addToCollectionFragmentList( const std::vector< std::string >& fragmentNames );

      /// Adds all collection fragments to the query table list.
      virtual void addAllCollectionFragments();

      /**
       * Sets the predicates of the query. The predicates may involve conditions on Attributes,
       * Tokens or individual columns of a Token in a data or links table. In the case of
       * Token column conditions the name of the column should be provided by the user in
       * form "<Token name>.<column name>" where "<column name>" is the actual name of the
       * column in the associated data or links table. If bind variables are used they should
       * be preceded by the symbol ":" in the where clause and the bind data should be
       * provided as Attribute or Token lists that use the bind variables as their names.
       * For Token column conditions the input Attribute list should be used for this purpose,
       * using "<column name>" as the Attribute name. If desired, the where clause can be
       * constructed in fragments by repeated calls to `setCondition'. All necessary table names,
       * primary key matching conditions and links table ID to data table ID matching conditions
       * are added to the final query string internally before the query is executed.
       *
       * @param whereClause The predicates of the query.
       * @param attributeBindData Attribute and Token column condition data to bind if bind variables are used in query.
       * @param tokenBindData Token condition data to bind if bind variables are used in query.
       */
      virtual void setCondition( const std::string& whereClause,
                                 coral::AttributeList* attributeBindData = 0,
                                 pool::TokenList* tokenBindData = 0 );

      /// Returns the where clause of the query.
      virtual const std::string& whereClause() const;

      /**
       * Sets the cache size used to store the query result.
       *
       * @param Number of rows stored in cache before cache is flushed.
       */
      virtual void setRowCacheSize( int rowCacheSize );

      /// Processes the query and returns a cursor over the query result.
      virtual pool::ICollectionCursor& execute();

      /**
       * Tell the query to not include the primary event reference
       * in the result by default (it can still beselected manually)
       *
       * @param skip if true (the default) then skip the primary event reference
       */
    
      virtual void skipEventReference( bool = true );


   protected:
      /**
       * Adds a Token column to the query select list.
       *
       * @param columnName Name of Token column to select.
       */
      virtual void addToTokenOutputList( const std::string& columnName );

      /**
       *   Adds an Attribute to the query select list.
       */
      virtual void addToAttributeOutputList( const std::string& columnName );

      /// Construct the WHERE part of the query
      virtual void	prepareQueryCondition();

      /// create underlying query object. set WHERE and FROM query parts
      virtual void	prepareQuery();

      /// Add primary key of the top level collection fragment to the query select list
      virtual void 	addPrimaryKeyToSelect();
      
    protected:
      /// Specification of collection properties.
      const pool::ICollectionDescription& m_description;

      /// Map of data table names using names of corresponding collection fragment names as keys.
      const std::map< std::string, std::string >& m_dataTableNameForCollectionFragmentName;

      /// Map of links table names using names of corresponding collection fragment names as keys.
      const std::map< std::string, std::string >& m_linksTableNameForCollectionFragmentName;

      /// Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
      const std::map< std::string, std::string >& m_tableTokenColumnPrefixForCollectionTokenColumnName;

      /// Map of table column names for Attributes using names of corresponding collection columns as keys.
      const std::map< std::string, std::string >& m_tableAttributeColumnNameForCollectionAttributeColumnName;

      /// List of all Attributes defined by collection but using table column names.
      const coral::AttributeList& m_tableAttributeList;
  
      /// Map of maps of link table ID's that use corresponding Token prefixes as keys, using collection fragment names as keys.
      const std::map< std::string, std::map< std::string, unsigned >* >& m_mapOfLinkIdForTokenKeyMaps;

      /// Map of maps of Token prefixes that use corresponding link table ID's as keys, using collection fragment names as keys.
      const std::map< std::string, std::map< unsigned, std::string >* >& m_mapOfTokenKeyForLinkIdMaps;

      /// Reference to table query processing object.
      coral::IQuery* m_query;

      /// Name of data table of top level collection fragment.
      std::string m_dataTableName;

      /// Row buffer to contain the results of the query.
      coral::AttributeList* m_outputDataBuffer;

      /// Names of Token columns selected by query.
      std::set< std::string > m_selectedTokenColumnNames;

      /// Names of Attribute columns selected by query.
      std::vector< std::string > m_selectedAttributeColumnNames;

      /// Collection fragments for which to include associated data tables in query.
      std::set< std::string > m_collectionFragmentNames;

      /// Link tables for which where clause contains explicit conditions on links table columns.
      std::vector< std::string > m_linksTableNames;

      /// Where clause fragments to match links table link ID's with OID_1's of associated data tables.
      std::vector< std::string > m_linkIDToOID1MatchingConditions;

      /// Where clause of query.
      std::string m_whereClause;

      /// Data provided to query if bind variables are used in where clause.
      coral::AttributeList* m_whereClauseBindData;

      /// Row buffer to contain Tokens selected by query.
      pool::TokenList* m_outputTokenList;

      /// Row buffer to contain Attributes selected by query.
      coral::AttributeList* m_outputAttributeList;

      /// Cursor to navigate over results of query.
      pool::ICollectionCursor* m_cursor;

      /// the session
      coral::ISessionProxy&	m_session;
      
      /// query returned rows bundling
      unsigned		m_rowCacheSize;

      /// If true add primary table key to the query selection list
      bool		m_readPrimaryKey;
      
      /// If false, the primary event reference is added always to the query result
      bool              m_skipEventRef;
    };
  }
}

#endif


