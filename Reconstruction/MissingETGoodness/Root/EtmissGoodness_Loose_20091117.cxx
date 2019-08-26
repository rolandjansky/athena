/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness_Loose_20091117.h"
#include "MissingETGoodness/TSelectionSetArchive.h"

ClassImp(MET::EtmissGoodness_Loose_20091117)


MET::EtmissGoodness_Loose_20091117::EtmissGoodness_Loose_20091117()
 : TSelectionSet(MET::Loose,20091117)
{
}


Bool_t
MET::EtmissGoodness_Loose_20091117::CreateSelection() 
{
  Bool_t status(kTRUE);

  // Add selections here.
  // ...

  return status;
}


/*-------------------------------------------------------------------------
 *  Register the selection, automatically done when loading the library
 *-------------------------------------------------------------------------*/
namespace
{
  [[maybe_unused]]
  const bool dummy =
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::Loose,20091117,"MET::EtmissGoodness_Loose_20091117");
}

