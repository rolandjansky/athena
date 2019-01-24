/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonDigitContainer.cxx

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRDO/RpcPadContainer.h"
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


//**********************************************************************

// Default constructor.

RpcPadContainer::RpcPadContainer( unsigned int hashmax)
: IdentifiableContainer<RpcPad>(hashmax) 
{
  // std::cout<<"RpcPadContainer ctor ["<<this<<"]"<<std::endl;
}

RpcPadContainer::RpcPadContainer( RpcPad_Cache* cache)
: IdentifiableContainer<RpcPad>(cache) 
{
  // std::cout<<"RpcPadContainer ctor ["<<this<<"]"<<std::endl;
}

//**********************************************************************

// Destructor.

RpcPadContainer::~RpcPadContainer() {
  // std::cout<<"RpcPadContainer dtor ["<<this<<"]"<<std::endl;

}

//**********************************************************************
/// Convert identifier to idhash
//unsigned int 
//RpcPadContainer::idToHash(unsigned int id) const
//{
//    return (hashFcn()(Identifier(id)));
//}

//**********************************************************************
//RpcPadIdHash&
//RpcPadContainer::hashFcn() const
//{
//    static RpcPadIdHash hashFcn;
//    return(hashFcn);
//}


//**********************************************************************

const CLID& RpcPadContainer::classID()
{
  return ClassID_traits<RpcPadContainer>::ID();       
  //    static const CLID CLID_RpcPadContainer= 4190; 
  //    return  CLID_RpcPadContainer ; 
}


// Insert a pad.

void RpcPadContainer::push_back(RpcCoinMatrix* /*cm*/) {

  // IMessageSvc*  msgSvc;
  // ISvcLocator* svcLoc = Gaudi::svcLocator( );
  // svcLoc->service( "MessageSvc", msgSvc );
  // MsgStream log(msgSvc, "MuonDigitContainer" );
  // 
  // Identifier padId = cm->identify(); 
  // 
  // MyBase::const_iterator it = MyBase::indexFind(padId);
  // if(it!=MyBase::end()) {
  //   const RpcPad* const_pad = (*it)  ;
  //   RpcPad * pad = const_cast<RpcPad*>(const_pad);
  //   pad->push_back(cm); 
  //  } else
  //  {
  //    log << MSG::ERROR <<" RpcPadContainer: Pad does not exist "<<endmsg;
  //  } 
  //  return ; 
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

ostream& operator<<(ostream& lhs, const RpcPadContainer& rhs) {
  lhs << "RpcPadContainer has " << rhs.pad_size() << " pads:" << endl;
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
        lhs << endl;
      }
      lhs << "  " << *it;
  }
  return lhs;
}

//**********************************************************************


