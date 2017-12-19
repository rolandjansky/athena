/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "ELFElt.h"
#include "ELFCont.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
#include <cassert>


#include "SGTools/TestStore.h"
using namespace SGTest;


void ELFElt::setLink (const std::string& key, int ndx)
{
  m_link.resetWithKeyAndIndex (key, ndx);
}


const ELFElt* ELFElt::ptr() const
{
  return *m_link;
}


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";

  const int nelt = 5;
  ELFCont* cont = new ELFCont;
  store.record (cont, "elf");
  for (int i=0; i < nelt; i++) {
    cont->push_back (new ELFElt(i));
    (*cont)[i]->setLink ("elf", nelt-1-i);
  }

  for (int i=0; i < nelt; i++) {
    assert ((*cont)[i]->ptr()->x() == nelt-1-i);
  }
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  Athena::getMessageSvcQuiet = true;
  initTestStore();

  test1 (store);
  return 0;
}
