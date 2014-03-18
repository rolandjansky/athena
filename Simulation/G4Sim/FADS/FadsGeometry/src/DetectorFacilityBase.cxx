/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/DetectorFacilityBase.h"
#include "FadsGeometry/DetectorFacilityCatalog.h"
#include "FadsUtilities/Tokenizer.h"

#ifdef HAVE_NEW_IOSTREAMS
#include <iostream>
#else
#include <iostream.h>
#endif

namespace FADS {

  DetectorFacilityBase::DetectorFacilityBase(std::string n)
  {
    FADS::Tokenizer t(":",n);
    name=t[0];
    // DetectorFacilityCatalog *dfs=DetectorFacilityCatalog::GetInstance();
    // cout<<"Registering detector "<<n<<endl;
    // dfs->RegisterDetector(this);
  }

} // end namespace
