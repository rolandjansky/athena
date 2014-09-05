/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONGUIDQUERY_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONGUIDQUERY_H

// Disable warning C4250 on Windows (inheritance via dominance)
#ifdef WIN32
#pragma warning ( disable : 4250 )
#endif

/*
 * @author  Marcin.Nowak@cern.ch
 */


#include "RelationalCollectionQuery.h"
#include "CollectionBase/ICollectionRelationalExtensions.h"

#include <set>


namespace pool {
   namespace RelationalCollection {

      
      class GUIDQuery : public RelationalCollectionQuery,
                        virtual public ICollectionGUIDQuery
      {
     public:
        GUIDQuery( coral::ISessionProxy& session,
                   const pool::ICollectionDescription& description,
                   const std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
                   const std::map< std::string, std::string >& linksTableNameForCollectionFragmentName,
                   const std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
                   const std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
                   const coral::AttributeList& tableAttributeList,
                   const std::map< std::string, std::map< std::string, unsigned >* >& mapOfLinkIdForTokenKeyMaps,
                   const std::map< std::string, std::map< unsigned, std::string >* >& mapOfTokenKeyForLinkIdMaps )
              : RelationalCollectionQuery(
                 session, description,
                 dataTableNameForCollectionFragmentName,
                 linksTableNameForCollectionFragmentName,
                 tableTokenColumnPrefixForCollectionTokenColumnName,
                 tableAttributeColumnNameForCollectionAttributeColumnName,
                 tableAttributeList,
                 mapOfLinkIdForTokenKeyMaps,
                 mapOfTokenKeyForLinkIdMaps,
                 false /* do not add primary key */ )
        { }

        /// Query all GUIDs that were requested using AddToOutputList()
        void      readGUIDs();

        /// Query GUIDs for a specific Token.  Results are accumulated in the GUID Set
        void      readGUIDs( const std::string& tokenName );
	
        /// Provide access to the GUID set.  
        virtual const GUIDSet&	getGUIDs() 	{ return m_guids; }

        virtual const CountedGroupedGUIDs&   getGroupedGUIDs();

     protected:
        GUIDSet                 m_guids;
        CountedGroupedGUIDs     m_groupedGUIDs;
	
        // hide methods not supposed to be called
     protected:
        /// this method does not make sense in a GUID query
        void	addToAttributeOutputList( const std::string& columnName );

        /// not useable since there is no special Cursor (yet?) for this type of query
        ICollectionCursor& execute() { return RelationalCollectionQuery::execute(); }
      };

   }
}
#endif
