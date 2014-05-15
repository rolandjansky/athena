/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonDigitContainer.cxx

#include "MuonDigitContainer/MdtDigitContainer.h"
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

MdtDigitContainer::MdtDigitContainer(unsigned int hash_max)
: IdentifiableContainer<MdtDigitCollection>(hash_max) 
{

}

//**********************************************************************

// Destructor.

MdtDigitContainer::~MdtDigitContainer() {

}

//**********************************************************************

// return the class ID

const CLID& MdtDigitContainer::classID()
{
  return ClassID_traits<MdtDigitContainer>::ID();       
  //      static const CLID CLID_MdtDigitContainer= 4191; 
  //      return  CLID_MdtDigitContainer ; 
}

//**********************************************************************

// Return the total number of digits in the container.

MdtDigitContainer::size_type MdtDigitContainer::digit_size() const {
  MdtDigitContainer::size_type count = 0;
  MdtDigitContainer::const_iterator it = begin();
  MdtDigitContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

ostream& operator<<(ostream& lhs, const MdtDigitContainer& rhs) {
  lhs << "MdtDigitContainer has " << rhs.digit_size() << " digits:" << endl;
  typedef SelectAllObject<MdtDigitContainer> SELECTOR; 
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
