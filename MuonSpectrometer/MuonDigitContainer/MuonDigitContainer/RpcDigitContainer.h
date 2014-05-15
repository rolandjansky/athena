/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcDigitContainer.h

#ifndef MUONDIGITCONTAINER_RPCDIGITCONAINTER_H
#define MUONDIGITCONTAINER_RPCDIGITCONAINTER_H

// David Adams
// November 2001

// Ketevi A. Assamagan, September 25 2002

//
// Muon digit container.
//
// This container provides access to muon digits and provides
// a mechanism for recording them.
//
// Converters will enable reading from raw data, zebra and
// conventional StoreGate technologies (Objectivty, root).
// They will also enable writing to all but zebra.
//
// Usage:
//
//   // Create module ID's.
//   RpcModuleId bil11(BIL,1,1);
//   RpcModuleId bml11(BML,1,1);
//   RpcModuleId bol11(BOL,1,1);
//   // Insert some digits.
//   digs.push_back(RpcDigit(RpcChannelId(bil11, 1, 2, 1, 1, 0, 6)));
//   digs.push_back(RpcDigit(RpcChannelId(bil11, 2, 1, 2, 2, 1, 2)));
//   digs.push_back(RpcDigit(RpcChannelId(bml11, 1, 3, 1, 1, 0, 10)));
//   // Check subcontainers.
//   assert( digs.digits(bil11).size() == 2 );
//   assert( digs.digits(bml11).size() == 1 );
//   assert( digs.digits(bol11).size() == 0 );

/** 
	Use IdentifiableContainer with RpcDigitCollection
*/ 

#include <vector>
#include <string>
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 


class RpcDigitContainer
   :public IdentifiableContainer<RpcDigitCollection> 
{
public:  
   RpcDigitContainer(unsigned int hash_max) ; 
  ~RpcDigitContainer() ; 

  typedef RpcDigitCollection::size_type size_type ; 
  typedef IdentifiableContainer<RpcDigitCollection> MyBase; 

  size_type digit_size() const ; 

  // IdentifiableContainer is still a DataObject
  // Put CLID here. 
  static const CLID& classID();

   /** return class ID */
   virtual const CLID& clID() const
    {
      return classID(); 
    }

 private:


};

CLASS_DEF(RpcDigitContainer,4192,0) 

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const RpcDigitContainer& rhs);

#endif



