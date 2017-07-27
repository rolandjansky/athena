/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonDigitContainer.cxx

#include "MuonDigitContainer/sTgcDigitContainer.h"
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
sTgcDigitContainer::sTgcDigitContainer()
  : IdentifiableContainer<sTgcDigitCollection>(1578) //Hard coded max size for dictionary generators
{

}


sTgcDigitContainer::sTgcDigitContainer(unsigned int hash_max)
  : IdentifiableContainer<sTgcDigitCollection>(hash_max) 
{

}

//**********************************************************************

// Destructor.

sTgcDigitContainer::~sTgcDigitContainer() {

}

//**********************************************************************

// return the class ID

const CLID& sTgcDigitContainer::classID()
{
  return ClassID_traits<sTgcDigitContainer>::ID(); 
}

//**********************************************************************

// Return the total number of digits in the container.

sTgcDigitContainer::size_type sTgcDigitContainer::digit_size() const {
  sTgcDigitContainer::size_type count = 0;
  sTgcDigitContainer::const_iterator it = begin();
  sTgcDigitContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

ostream& operator<<(ostream& lhs, const sTgcDigitContainer& rhs) {
  lhs << "sTgcDigitContainer has " << rhs.digit_size() << " digits:" << endl;
  typedef SelectAllObject<sTgcDigitContainer> SELECTOR; 
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
