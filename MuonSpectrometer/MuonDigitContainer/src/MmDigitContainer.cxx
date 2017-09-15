/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonDigitContainer.cxx

#include "MuonDigitContainer/MmDigitContainer.h"
#include "EventContainers/SelectAllObject.h"
#include <map>
#include <cassert>
#include <iostream>

using std::ostream;
using std::endl;

//**********************************************************************
// Local definitions
//**********************************************************************

//**********************************************************************
// Member functions.
//**********************************************************************

// Default constructor.


MmDigitContainer::MmDigitContainer(unsigned int hash_max)
  : IdentifiableContainer<MmDigitCollection>(hash_max) 
{

}

//**********************************************************************

// Destructor.

MmDigitContainer::~MmDigitContainer() {

}

//**********************************************************************

// return the class ID

const CLID& MmDigitContainer::classID()
{
  return ClassID_traits<MmDigitContainer>::ID();       
  //      static const CLID CLID_MmDigitContainer= 4191; 
  //      return  CLID_MmDigitContainer ; 
}

//**********************************************************************

// Return the total number of digits in the container.

MmDigitContainer::size_type MmDigitContainer::digit_size() const {
  MmDigitContainer::size_type count = 0;
  MmDigitContainer::const_iterator it = begin();
  MmDigitContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

ostream& operator<<(ostream& lhs, const MmDigitContainer& rhs) {
  lhs << "MmDigitContainer has " << rhs.digit_size() << " digits:" << endl;
  typedef SelectAllObject<MmDigitContainer> SELECTOR; 
  SELECTOR sel(&rhs);
  SELECTOR::const_iterator it = sel.begin(); 
  SELECTOR::const_iterator it_end = sel.end(); 

  bool first = true;
  for ( ; it!=it_end;++it)
    { 
      if ( first ) {
        first = false;
      } else {
        lhs << endl;
      }
      lhs << "  " << *it;
    }
  return lhs;
}

//**********************************************************************
