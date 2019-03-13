/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/getMessageSvc.h"

using namespace Athena;
using std::cerr;
using std::endl;

/// Set this to force off the warning messages from getMessageSvc
/// (in unit tests, for example).
std::atomic<bool> Athena::getMessageSvcQuiet;

IMessageSvc* Athena::getMessageSvc( bool quiet ) { return getMessageSvc( Options::Lazy, quiet ); }
IMessageSvc* Athena::getMessageSvc( const Options::CreateOptions opt, bool quiet ) {
  IMessageSvc* pSvc(0);
  const bool createIf( opt == Athena::Options::Eager );
  if (!(Gaudi::svcLocator()->service("MessageSvc", pSvc, createIf)).isSuccess() &&
      !quiet &&
      !Athena::getMessageSvcQuiet)
  {
    cerr << "Athena::getMessageSvc: WARNING MessageSvc not found, will use std::cout" << endl;
  }
  return pSvc;
}

void Athena::reportMessage (IMessageSvc* ims, const std::string &source, int type, const std::string &message) {
  if (ims) ims->reportMessage(source, type, message);
}

int Athena::outputLevel(const IMessageSvc* ims, const std::string &source) {
  if (ims) return ims->outputLevel(source);
  else return MSG::INFO;
}

void Athena::setOutputLevel(IMessageSvc* ims, const std::string &source, int level) {
  if(ims) ims->setOutputLevel(source, level);
}

IMessageSvcHolder::IMessageSvcHolder(IMessageSvc *ims) : m_ims(ims) {
  assert(m_ims);
  m_ims->addRef(); //take ownership till we go out of scope
}

IMessageSvcHolder::IMessageSvcHolder(const IMessageSvcHolder& rhs) :
  m_ims(rhs.m_ims) 
{
  if (m_ims) m_ims->addRef(); //take ownership till we go out of scope
}

IMessageSvcHolder& 
IMessageSvcHolder::operator=(const IMessageSvcHolder& rhs) {
  if (this != & rhs && m_ims != rhs.m_ims) {
    if (m_ims) m_ims->release(); //give up our IMessageSvc*
    m_ims = rhs.m_ims;
    if (m_ims) m_ims->addRef(); //take ownership till we go out of scope
  }
  return *this;
}

IMessageSvcHolder::IMessageSvcHolder( const Options::CreateOptions opt ) :
  m_ims(0)
{
  if (opt == Athena::Options::Eager) m_ims = getMessageSvc(opt);
}

IMessageSvcHolder::~IMessageSvcHolder() {
  if (m_ims) m_ims->release();
}

IMessageSvc*
IMessageSvcHolder::get() {
  if (!m_ims) m_ims = getMessageSvc();
  return m_ims;
}
