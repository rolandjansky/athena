/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness_Medium_20091117.h"
#include "MissingETGoodness/TSelectionSetArchive.h"
#include "CxxUtils/unused.h"

ClassImp(MET::EtmissGoodness_Medium_20091117)


MET::EtmissGoodness_Medium_20091117::EtmissGoodness_Medium_20091117()
 : TSelectionSet(MET::Medium,20091117)
{
}


Bool_t
MET::EtmissGoodness_Medium_20091117::CreateSelection() 
{
  Bool_t status(kTRUE);

  MET::TSelection foo(100,1000);
  foo.AddCut("foo>200");
  foo.AddCut("bar<20");

  MET::TSelection bar(1100,11000);
  bar.AddCut("foo>2000");
  bar.AddCut("bar<200");

  this->AddSelection(foo);
  this->AddSelection(bar);

  return status;
}


/*-------------------------------------------------------------------------
 *  Register the selection, automatically done when loading the library
 *-------------------------------------------------------------------------*/
namespace
{
  const bool UNUSED(dummy) =
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::Medium,20091117,"MET::EtmissGoodness_Medium_20091117");
}

