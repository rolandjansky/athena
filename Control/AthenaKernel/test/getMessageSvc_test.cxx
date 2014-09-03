/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  getMessageSvc_test.cxx
 * @author Paolo Calafiura
 * @brief Regression test for getMessageSvc() 
 *
 * get a MsgSvc pointer out of thin air and use it
 *
 * $Id: getMessageSvc_test.cxx,v 1.5 2008-07-14 22:10:14 calaf Exp $
 */
#undef NDEBUG
#include <cassert>
#include "GaudiKernel/MsgStream.h"
#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"

class Mine {
public:
  void printIt() {
    MsgStream mlog(m_ims, "Mine");
    mlog << MSG::ALWAYS << "*** Mine works ***" <<endmsg;
  }
private:
  Athena::IMessageSvcHolder m_ims;
};
class Yours {
public:
  Yours(IMessageSvc* ims) : m_ims(ims) {}
  void printIt() {
    MsgStream mlog(m_ims.get(), "Yours");
    mlog << MSG::ALWAYS << "*** Yours works too ***" <<endmsg;
  }
private:
  Athena::IMessageSvcHolder m_ims;
};

int main() {
  ISvcLocator* pDum;
  assert( Athena_test::initGaudi(pDum) );
  IMessageSvc *pMS(Athena::getMessageSvc());
  assert( pMS );
  //usual nasty trick to get the ref count
  pMS->addRef();
  unsigned int refCount(pMS->release());
  //check it is a singleton
  IMessageSvc *another(Athena::getMessageSvc());
  assert(another == pMS);
  assert(refCount == pMS->release());
  
  {
    Mine my;
    my.printIt();
    //this simulates what would happen in a Gaudi component
    Yours you(pMS);
    you.printIt();
  } //my, you destructors called
  pMS->addRef();
  //  assert(pMS->release() == refCount);

  MsgStream log(pMS, "getMessageSvc_test");
  log << MSG::ALWAYS << "*** getMessageSvc_test OK ***" <<endmsg;
  return 0;
}
