/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcPadContainer.h

#ifndef MUONDIGITCONTAINER_RPCPADCONAINTER_H
#define MUONDIGITCONTAINER_RPCPADCONAINTER_H



#include <vector>
#include <string>
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPad_Cache.h"
//#include "MuonRDO/RpcPadIdHash.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

/** 
	Use IdentifiableContainer with RpcPad
*/ 

class RpcPadContainer
   :public IdentifiableContainer<RpcPad> {

public:  
   RpcPadContainer(unsigned int hashmax) ; 
   RpcPadContainer(RpcPad_Cache* cache) ; 

  ~RpcPadContainer() ; 

  typedef RpcPad::size_type size_type ; 
  typedef IdentifiableContainer<RpcPad> MyBase; 

  /// Convert identifier to idhash
  //unsigned int idToHash(unsigned int id) const;

  // Insert a pad
  void push_back( RpcCoinMatrix* cm);

  size_type pad_size() const ; 

  // IdentifiableContainer is still a DataObject
  // Put CLID here. 
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {
    return classID(); 
  }

private:

    //    RpcPadIdHash&  hashFcn() const;

};

CLASS_DEF(RpcPadContainer,4190,0) 

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const RpcPadContainer& rhs);

#endif


