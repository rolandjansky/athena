/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness_Tight_20091117.h"
#include "MissingETGoodness/TSelectionSetArchive.h"
#include "CxxUtils/unused.h"

ClassImp(MET::EtmissGoodness_Tight_20091117)


MET::EtmissGoodness_Tight_20091117::EtmissGoodness_Tight_20091117()
 : TSelectionSet(MET::Tight,20091117)
{
}


Bool_t
MET::EtmissGoodness_Tight_20091117::CreateSelection() 
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
  const bool UNUSED(dummy) =
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::Tight,20091117,"MET::EtmissGoodness_Tight_20091117");
}

