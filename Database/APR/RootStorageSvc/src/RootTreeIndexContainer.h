/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */

#ifndef POOL_ROOTTREEINDEXCONTAINER_H
#define POOL_ROOTTREEINDEXCONTAINER_H 1

#include "RootTreeContainer.h"

// Forward declarations
class TTree;
class TBranch;

namespace pool {
  /** @class RootTreeIndexContainer RootTreeIndexContainer.h src/RootTreeIndexContainer.h
    *
    * Description:
    * Spezialization of RootTreeContainer
    * ROOT specific implementation of Database container.
    * Since objects in root are stored in "trees with branches",
    * this object corresponds to a tuple (Tree/Branch), where
    * each object type (determined by the location of the transient
    * object within the data store) is accessed by an index number
    * stored inside its tree.
    *
    * @author  N.Dulin, P.v.Gemmeren
    * @date    8/1/2018
    * @version 1.0
    */

   class RootTreeIndexContainer : public RootTreeContainer {
   public:
      /// Standard constructor
      RootTreeIndexContainer();

      RootTreeIndexContainer (const RootTreeIndexContainer&) = delete;
      RootTreeIndexContainer& operator= (const RootTreeIndexContainer&) = delete;

      /// Standard destructor
      virtual ~RootTreeIndexContainer();

      /// Number of entries within the container
      virtual long long int nextRecordId();

      /// Find object by object identifier and load it into memory
      /** @param  ptr    [IN/OUT]  ROOT-style address of the pointer to object
        * @param  shape     [IN]   Object type
        * @param  oid      [OUT]   Object OID
        *
        * @return Status code indicating success or failure.
        */
      virtual DbStatus loadObject( void** ptr, ShapeH shape, 
                                   Token::OID_t& oid);
      
      /// Commit single entry to container
      virtual DbStatus writeObject(ActionList::value_type&);

      /// Execute Transaction action
      virtual DbStatus transAct(Transaction::Action action);

   private:
      /// Pointer to index branch
      TBranch* m_index_ref;
      TBranch* m_index_foreign;
      /// Index multiplier (e.g. pid - ppid), fill in c'tor
      int m_index_multi;
      /// Index (64 bit)
      long long int* m_index;
   };
}
#endif //POOL_ROOTTREEINDEXCONTAINER_H
