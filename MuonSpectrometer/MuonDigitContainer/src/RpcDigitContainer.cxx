/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonDigitContainer.cxx

#include "MuonDigitContainer/RpcDigitContainer.h"
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

RpcDigitContainer::RpcDigitContainer(unsigned int hash_max)
: IdentifiableContainer<RpcDigitCollection>(hash_max) 
{

}

//**********************************************************************

// Destructor.

RpcDigitContainer::~RpcDigitContainer() {

}

//**********************************************************************

 const CLID& RpcDigitContainer::classID()    
{
  return ClassID_traits<RpcDigitContainer>::ID();       
  //      static const CLID CLID_RpcDigitContainer= 4192; 
  //      return  CLID_RpcDigitContainer ; 
}


//**********************************************************************

// Return the total number of digits in the container.

RpcDigitContainer::size_type RpcDigitContainer::digit_size() const {
  RpcDigitContainer::size_type count = 0;
  RpcDigitContainer::const_iterator it = begin();
  RpcDigitContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

ostream& operator<<(ostream& lhs, const RpcDigitContainer& rhs) {
  lhs << "RpcDigitContainer has " << rhs.digit_size() << " digits:" << endl;
  typedef SelectAllObject<RpcDigitContainer> SELECTOR; 
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
