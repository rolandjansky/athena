/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRDO/RpcPadContainer.h"
#include "EventContainers/SelectAllObject.h"
#include <map>
#include <cassert>
#include <iostream>

RpcPadContainer::RpcPadContainer( unsigned int hashmax)
: IdentifiableContainer<RpcPad>(hashmax) 
{
}

RpcPadContainer::RpcPadContainer( RpcPad_Cache* cache)
: IdentifiableContainer<RpcPad>(cache) 
{
}

//**********************************************************************

const CLID& RpcPadContainer::classID()
{
  return ClassID_traits<RpcPadContainer>::ID();       
}


// Insert a pad.

void RpcPadContainer::push_back(RpcCoinMatrix* /*cm*/) {
}

//**********************************************************************

// Return the total number of cma in the container.

RpcPadContainer::size_type RpcPadContainer::pad_size() const {
  RpcPadContainer::size_type count = 0;
  RpcPadContainer::const_iterator it = begin();
  RpcPadContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

std::ostream& operator<<(std::ostream& lhs, const RpcPadContainer& rhs) {
  lhs << "RpcPadContainer has " << rhs.pad_size() << " pads:" << std::endl;
  typedef SelectAllObject<RpcPadContainer> SELECTOR; 
  SELECTOR sel(&rhs);
  SELECTOR::const_iterator it = sel.begin(); 
  SELECTOR::const_iterator it_end = sel.end(); 

  bool first = true;
  for ( ; it!=it_end;++it)
  { 
      if ( first ) {
        first = false;
      } else {
        lhs << std::endl;
      }
      lhs << "  " << *it;
  }
  return lhs;
}

//**********************************************************************


