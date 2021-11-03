/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// DetectorParameters
// 30-Jun-2003 Bill Seligman (WGS)

// This routine implements a modified version of the singleton
// pattern.  It's initialized with a pointer to some class that
// inherits from this one; thereafter that pointer can be accessed
// from any routine in the simulation.

// Note: we're using the LArGeo namespace, which is why LArGeo:: is in
// front of so many names.

#include "LArGeoCode/VDetectorParameters.h"
#include <iostream>
#include <stdlib.h>


CxxUtils::CachedUniquePtr<LArGeo::VDetectorParameters> LArGeo::VDetectorParameters::s_instance;

void LArGeo::VDetectorParameters::SetInstance(std::unique_ptr<VDetectorParameters> p)
{
  s_instance.set (std::move (p));
}

const LArGeo::VDetectorParameters* LArGeo::VDetectorParameters::GetInstance()
{
  const VDetectorParameters* p = s_instance.get();
  if(p==0)
      std::cerr << "LArGeo::VDetectorParameters::GetInstance was called, but "
		<< "the pointer to the detector-parameter access object is 0."
		<< std::endl;

  return p;
}
