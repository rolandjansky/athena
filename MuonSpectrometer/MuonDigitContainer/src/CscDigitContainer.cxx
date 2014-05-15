/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonDigitContainer.cxx

#include "MuonDigitContainer/CscDigitContainer.h"
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

CscDigitContainer::CscDigitContainer(unsigned int hash_max)
: IdentifiableContainer<CscDigitCollection>(hash_max) 
{

}

//**********************************************************************

// Destructor.

CscDigitContainer::~CscDigitContainer() {

}

//**********************************************************************

// return the class ID

const CLID& CscDigitContainer::classID()
{
  return ClassID_traits<CscDigitContainer>::ID();       
  //      static const CLID CLID_CscDigitContainer= 4194; 
  //      return  CLID_CscDigitContainer ; 
}

//**********************************************************************

// Return the total number of digits in the container.

CscDigitContainer::size_type CscDigitContainer::digit_size() const {
  CscDigitContainer::size_type count = 0;
  CscDigitContainer::const_iterator it = begin();
  CscDigitContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

ostream& operator<<(ostream& lhs, const CscDigitContainer& rhs) {
  lhs << "CscDigitContainer has " << rhs.digit_size() << " digits:" << endl;
  typedef SelectAllObject<CscDigitContainer> SELECTOR; 
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
