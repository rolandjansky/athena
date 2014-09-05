/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONQUERY_H
#define COLLECTIONBASE_ICOLLECTIONQUERY_H

#include <string>
#include <vector>


namespace coral {
  class AttributeList;
}

namespace pool {

  class ICollectionCursor;
  class TokenList;

  /** 
   * @class ICollectionQuery ICollectionQuery.h CollectionBase/ICollectionQuery.h
   *
   * An interface used to query a collection.
   */
  class ICollectionQuery
  {
  public:
    /// Default destructor.
    virtual ~ICollectionQuery() {}

    /**
     * Adds a column to the query select list.
     *
     * @param columnName Name of Attribute column to select.
     */
    virtual void addToOutputList( const std::string& columnName ) = 0;

    /**
     * Adds one or more columns to the query select list.
     *
     * @param columnNames Names of Attribute columns to select.
     */
    virtual void addToOutputList( const std::vector<std::string>& columnNames ) = 0;

    /// Adds all Attribute columns to the query select list.
    virtual void selectAllAttributes() = 0;

    /// Adds all Token columns to the query select list.
    virtual void selectAllTokens() = 0;

    /// Adds all Token and Attribute columns to the query select list.
    virtual void selectAll() = 0;

    /**
     * Adds the data table associated with a collection fragment to the query table list.
     *
     * @param fragmentName Name of collection fragment to add.
     */
    virtual void addToCollectionFragmentList( const std::string& fragmentName ) = 0;

    /**
     * Adds the data tables associated with one or more collection fragments to the query 
     * table list.
     *
     * @param fragmentNames Names of collection fragments to add.
     */
    virtual void addToCollectionFragmentList( const std::vector< std::string >& fragmentNames ) = 0;

    /// Adds all collection fragments to the query table list.
    virtual void addAllCollectionFragments() = 0;

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
                               TokenList* tokenBindData = 0 ) = 0;

    /// Returns the where clause of the query.
    virtual const std::string& whereClause() const = 0;

    /**
     * Sets the cache size used to store the query result.
     *
     * @param rowCacheSize Number of rows stored in cache before cache is flushed.
     */
    virtual void setRowCacheSize( int rowCacheSize ) = 0;

    /// Processes the query and returns a cursor over the query result.
    virtual pool::ICollectionCursor& execute() = 0;

   /**
     * Tell the query to not include the primary event reference
     * in the result by default (it can still beselected manually)
     * Only relevant for explicit collections with attributes
     *
     * @param skip if true (the default) then skip the primary event reference
     */
    
    virtual void skipEventReference( bool = true ) {}
  };

}

#endif


