/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <SampleHandler/MetaDataQuery.h>

//
// method implementations
//

ClassImp (SH::MetaDataQuery)

namespace SH
{
  void MetaDataQuery ::
  addSample (const MetaDataSample& sample)
  {
    samples.push_back (sample);
  }
}
