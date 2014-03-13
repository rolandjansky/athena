/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/SelectionTemplate.h"
#include "MissingETGoodness/TSelectionSetArchive.h"

ClassImp(MET::SelectionTemplate)


MET::SelectionTemplate::SelectionTemplate()
 : TSelectionSet(MET::QUALITY,VERSION)
{
}


Bool_t
MET::SelectionTemplate::CreateSelection() 
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
  const bool dummy =
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::QUALITY,VERSION,"MET::SelectionTemplate");
}

