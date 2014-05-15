/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscDigitContainer.h

#ifndef MUONDIGITCONTAINER_CSCDIGITCONAINTER_H
#define MUONDIGITCONTAINER_CSCDIGITCONAINTER_H

// Ketevi A. Assamagan
// September 30 2002

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
//   CscModuleId css(CSS,1,1);
//   CscModuleId csl(CSL,1,1);
//   // Insert some digits.
//   digs.push_back(CscDigit(CscChannelId(css, 2, 3, 0, 30)));
//   digs.push_back(CscDigit(CscChannelId(csl, 1, 1, 1, 10)));
//   digs.push_back(CscDigit(CscChannelId(csl, 1, 2, 0, 10)));
//   // Check subcontainers.
//   assert( digs.digits(css).size() == 1 );
//   assert( digs.digits(csl).size() == 2 );
//   assert( digs.digits(bol11).size() == 0 );

/** 
	Use IdentifiableContainer with CscDigitCollection
*/ 

#include <vector>
#include <string>
#include "MuonDigitContainer/CscDigitCollection.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 


class CscDigitContainer
   :public IdentifiableContainer<CscDigitCollection> 
{
public:  
   CscDigitContainer(unsigned int hash_max) ; 
  ~CscDigitContainer() ; 

  typedef CscDigitCollection::size_type size_type ; 
  typedef IdentifiableContainer<CscDigitCollection> MyBase; 

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

CLASS_DEF(CscDigitContainer,4194,0) 

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const CscDigitContainer& rhs);

#endif


