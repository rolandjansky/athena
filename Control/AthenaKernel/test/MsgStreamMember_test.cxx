/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  MsgStreamMember_test.cxx
 * @brief Example/Regression test for @class Athena::MsgStreamMember
 *
 * @author Paolo Calafiura
 * $Id: MsgStreamMember_test.cxx,v 1.1 2008-07-14 22:10:14 calaf Exp $
 */
#undef NDEBUG
#include <cassert>
#include "TestTools/initGaudi.h"
#include "AthenaKernel/MsgStreamMember.h"

using namespace Athena;
class Mine {
public:
  Mine() : 
    //this is how you would set m_stream in a (data)obj w/o access to IMessageSvc*
    m_stream(name()) {}
  const std::string& name() const { 
    static const std::string n("Mine"); 
    return n;
  }
  void printIt() {
    m_stream << MSG::ALWAYS << "*** Mine works ***" <<endmsg;
  }
private:
  MsgStreamMember m_stream;
};
class Yours {
public:
  Yours(IMessageSvc* ims) :
    //this would be the typical way to construct m_stream in e.g. a Service
    m_stream(ims, name()) {}
  const std::string& name() const {  
    static const std::string n("Yours"); 
    return n;
  }

  void printIt() {
    m_stream << MSG::ALWAYS << "*** Yours works too ***" <<endmsg;
  }
private:
  Athena::MsgStreamMember m_stream;
};

class Hers {
public:
  Hers(IMessageSvc* ims) {
    //this would be another (less preferred) way to construct m_stream
    m_stream = MsgStreamMember(ims, name());
  }
  const std::string& name() const {  
    static const std::string n("Hers"); 
    return n;
  }

  void printIt() {
    m_stream << MSG::ALWAYS << "*** reporting ***" <<endmsg;
  }
private:
  Athena::MsgStreamMember m_stream;
};

int main() {
  ISvcLocator* pDum;
  assert( Athena_test::initGaudi(pDum) );
  IMessageSvc *pMS(Athena::getMessageSvc());
  assert( pMS );
  //usual nasty trick to get the ref count
  pMS->addRef();
  unsigned int refCount(pMS->release());
  std::cout << "Initial message svc ref count " << refCount << std::endl; 
  {
    Mine my;
    my.printIt();
    Yours you(pMS);
    Yours alsoYours(you);
    alsoYours.printIt();
    Hers her(pMS);
    her.printIt();
  } //my, you, etc destructors called

  MsgStreamMember mm(Athena::Options::Eager, "Foo");

  longlong t0(System::currentTime(System::microSec));
  for (int i=0; i<1000; ++i) MsgStreamMember(Athena::Options::Eager, "Foo");

  //  pMS->addRef();
  //  std::cout << "ref count after eager creation " << pMS->release() << std::endl; 
  longlong t1(System::currentTime(System::microSec));
  for (int i=0; i<10; ++i) mm << MSG::ALWAYS << "bla" << endmsg;

  longlong t2(System::currentTime(System::microSec));

  std::cout << "Wall clock time to create a MsgStreamMember (microsec)" << (t1-t0) * 1e-3 <<std::endl;
  std::cout << "Wall clock time to print bla using MsgStreamMember (microsec)" << (t2-t1) * 0.1 <<std::endl;
  

  pMS->addRef();
  unsigned int refCountAfter(pMS->release());
  std::cout << "Final ref count " << refCountAfter << std::endl; 

  ///FIXME Gaudi bug#74192    assert(refCountAfter == refCount);

  std::cout << "*** MsgStreamMember_test OK ***" <<std::endl;
  return 0;
}
