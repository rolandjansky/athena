/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_RELATIONALEXTENSIONS_H
#define COLLECTIONBASE_RELATIONALEXTENSIONS_H

#include "CollectionBase/ICollectionQuery.h"

#include <set>
#include <vector>

namespace pool {

  /** 
   * @class 
   * @author Marcin.Nowak@cern.ch
   */
   
   class ICollectionGUIDQuery : virtual public ICollectionQuery {
  public:
     typedef  std::set< std::pair<std::string, std::string> >  GUIDSet;

     typedef std::pair< unsigned, std::vector< std::string > > countedGUIDGroup_t;
     struct CountedGroupedGUIDs {
       std::vector< std::string >         tokenNames;
       std::vector< countedGUIDGroup_t >  groupedGUIDRows;

       void clear() { tokenNames.clear(); groupedGUIDRows.clear(); }
     };

     
     /// read GUIDs for all Tokens from the database
     virtual void	readGUIDs() = 0;
     /// read GUIDs for one Token (tokenName)  from the database
     virtual void	readGUIDs( const std::string& tokenName ) = 0;
     /// return reference to a set of all GUIDs read so far
     virtual const GUIDSet&	getGUIDs() = 0;

     virtual const CountedGroupedGUIDs&   getGroupedGUIDs() = 0;

     virtual		~ICollectionGUIDQuery() {}
   };


   
   class ICollectionRelationalExtensions
   {
  public:
     
     virtual ICollectionGUIDQuery* newGUIDQuery() const = 0;
   };


}

#endif

