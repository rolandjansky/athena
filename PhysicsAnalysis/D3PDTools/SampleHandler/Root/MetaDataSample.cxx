/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <SampleHandler/MetaDataSample.h>

//
// method implementations
//

ClassImp (SH::MetaDataSample)

namespace SH
{
  MetaDataSample ::
  MetaDataSample (const std::string& val_name)
    : name (val_name), unknown (true), isData (-1), running (-1),
      luminosity (-1), crossSection (-1), nevents (-1), kfactor (-1),
      filterEfficiency (-1)
  {
  }
}
