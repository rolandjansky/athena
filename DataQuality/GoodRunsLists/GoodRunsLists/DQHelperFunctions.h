/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GoodRunsLists/TGoodRunsList.h"
#include "TString.h"

namespace DQ
{
  void SetXMLFile(const TString& filename);
  Root::TGoodRunsList& GetGRL();
  bool PassRunLB(int runnr, int lbnr);
}

