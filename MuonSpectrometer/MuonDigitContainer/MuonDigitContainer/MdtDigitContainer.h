/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtDigitContainer.h

#ifndef MUONDIGITCONTAINER_MDTDIGITCONAINTER_H
#define MUONDIGITCONTAINER_MDTDIGITCONAINTER_H

// David Adams
// November 2001
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
//   MdtModuleId bil11(BIL,1,1);
//   MdtModuleId bml11(BML,1,1);
//   MdtModuleId bol11(BOL,1,1);
//   // Insert some digits.
//   digs.push_back(MdtDigit(MdtChannelId(bil11, 0, 0,  8), 101));
//   digs.push_back(MdtDigit(MdtChannelId(bil11, 0, 1, 16), 101));
//   digs.push_back(MdtDigit(MdtChannelId(bml11, 1, 0, 12),  72));
//   // Check subcontainers.
//   assert( digs.digits(bil11).size() == 2 );
//   assert( digs.digits(bml11).size() == 1 );
//   assert( digs.digits(bol11).size() == 0 );

/** 
	Use IdentifiableContainer with MdtDigitCollection
*/ 

#include <vector>
#include <string>
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 


class MdtDigitContainer
   :public IdentifiableContainer<MdtDigitCollection> 
{
public:  
   MdtDigitContainer(unsigned int hash_max) ; 
  ~MdtDigitContainer() ; 

  typedef MdtDigitCollection::size_type size_type ; 
  typedef IdentifiableContainer<MdtDigitCollection> MyBase; 

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

CLASS_DEF(MdtDigitContainer,4191,0) 

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const MdtDigitContainer& rhs);

#endif


