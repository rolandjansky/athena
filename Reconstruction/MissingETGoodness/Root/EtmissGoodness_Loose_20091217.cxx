/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness_Loose_20091217.h"
#include "MissingETGoodness/TSelectionSetArchive.h"
#include "CxxUtils/unused.h"

ClassImp(MET::EtmissGoodness_Loose_20091217)


MET::EtmissGoodness_Loose_20091217::EtmissGoodness_Loose_20091217()
 : TSelectionSet(MET::Loose,20091217)
{
}


Bool_t
MET::EtmissGoodness_Loose_20091217::CreateSelection() 
{
  Bool_t status(kTRUE);

  // Add selections here.
  // ...

/*
  // Example:

  MET::TSelection foo(100,1000); // runrange
  foo.AddCut("runNr>200");
  foo.AddCut("bar<20");

  MET::TSelection bar(1100,11000);
  bar.AddCut("runNr>2000");
  bar.AddCut("bar<200");
*/

  return status;
}


/*-------------------------------------------------------------------------
 *  Register the selection, automatically done when loading the library
 *-------------------------------------------------------------------------*/
namespace
{
  const bool UNUSED(dummy) =
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::Loose,20091217,"MET::EtmissGoodness_Loose_20091217");
}

