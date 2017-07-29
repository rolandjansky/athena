/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRDO/MdtCsmContainer.h"
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

MdtCsmContainer::MdtCsmContainer()
: IdentifiableContainer<MdtCsm>(hashFcn().max()) 
{
}

//**********************************************************************

MdtCsmContainer::MdtCsmContainer(unsigned int hashmax)
: IdentifiableContainer<MdtCsm>(hashmax) 
{
}

//**********************************************************************

// Destructor.

MdtCsmContainer::~MdtCsmContainer() {

}

//**********************************************************************
/// Convert identifier to idhash
unsigned int 
MdtCsmContainer::idToHash(unsigned int id) const
{
    return (hashFcn()(Identifier(Identifier32(id))));
}

//**********************************************************************
const MdtCsmIdHash& 
MdtCsmContainer::hashFcn()
{
    static const MdtCsmIdHash hashFcn;
    return (hashFcn);
}


//**********************************************************************

const CLID& MdtCsmContainer::classID()    
{
  return ClassID_traits<MdtCsmContainer>::ID();       
  //      static const CLID CLID_MdtCsmContainer= 4187; 
  //      return CLID_MdtCsmContainer ; 
}


// Return the total number of digits in the container.

MdtCsmContainer::size_type MdtCsmContainer::digit_size() const {
  MdtCsmContainer::size_type count = 0;
  MdtCsmContainer::const_iterator it = begin();
  MdtCsmContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

ostream& operator<<(ostream& lhs, const MdtCsmContainer& rhs) {
  lhs << "MdtCsmContainer has " << rhs.digit_size() << " digits:" << endl;
  typedef SelectAllObject<MdtCsmContainer> SELECTOR; 
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
