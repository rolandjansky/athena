/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcDigitContainer.h

#ifndef MUONDIGITCONTAINER_TGCDIGITCONAINTER_H
#define MUONDIGITCONTAINER_TGCDIGITCONAINTER_H

// David Adams
// November 2001

// Ketevi A. Assamagan, September 27 2002

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
//   TgcModuleId bil11(T1E,1,4);
//   TgcModuleId bml11(T4E,-1,10);
//   TgcModuleId bol11(T1F,1,1);
//   // Insert some digits.
//   digs.push_back(TgcDigit(TgcChannelId(bil11, 1, 0, 1)));
//   digs.push_back(TgcDigit(TgcChannelId(bil11, 2, 1, 2)));
//   digs.push_back(TgcDigit(TgcChannelId(bml11, 3, 0, 5)));
//   // Check subcontainers.
//   assert( digs.digits(bil11).size() == 2 );
//   assert( digs.digits(bml11).size() == 1 );
//   assert( digs.digits(bol11).size() == 0 );

/** 
	Use IdentifiableContainer with TgcDigitCollection
*/ 

#include <vector>
#include <string>
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 


class TgcDigitContainer
   :public IdentifiableContainer<TgcDigitCollection> 
{
public:  
   TgcDigitContainer(unsigned int hash_max) ; 
  ~TgcDigitContainer() ; 

  typedef TgcDigitCollection::size_type size_type ; 
  typedef IdentifiableContainer<TgcDigitCollection> MyBase; 

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

CLASS_DEF(TgcDigitContainer,4193,0) 

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const TgcDigitContainer& rhs);

#endif


