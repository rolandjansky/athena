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
    * Spezialization of RootTreeContainer, Read only port for R21!
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
      RootTreeIndexContainer(IOODatabase* idb);

      /// Standard destructor
      virtual ~RootTreeIndexContainer();

      /// Find object by object identifier and load it into memory
      /** @param  call      [IN]   Callback to load data
      * @param  oid      [OUT]   Object OID
      * @param  mode      [IN]   Object access mode
      *
      * @return Status code indicating success or failure.
      */
      virtual DbStatus loadObject(DataCallBack* call,
                                Token::OID_t& oid,
                                DbAccessMode  mode);
   };
}
#endif //POOL_ROOTTREEINDEXCONTAINER_H
