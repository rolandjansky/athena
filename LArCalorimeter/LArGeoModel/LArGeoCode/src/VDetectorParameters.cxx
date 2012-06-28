/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


LArGeo::VDetectorParameters* LArGeo::VDetectorParameters::s_instance = 0;

void LArGeo::VDetectorParameters::SetInstance(VDetectorParameters* p)
{
  if(s_instance!=0) {
    /* -- Consider moving these printouts to MsgSvc
    if(p==0) 
      std::cerr << "LArGeo::VDetectorParameters::SetInstance warning: "
		<< "database-parameter access objet is being deleted."
		<< std::endl;
    else
      std::cerr << "LArGeo::VDetectorParameters::SetInstance warning: "
		<< "database-parameter access object is being re-defined."
		<< std::endl;
    */
    delete s_instance;
  }

  s_instance = p;
}

LArGeo::VDetectorParameters* LArGeo::VDetectorParameters::GetInstance()
{
  if(s_instance==0)
      std::cerr << "LArGeo::VDetectorParameters::GetInstance was called, but "
		<< "the pointer to the detector-parameter access object is 0."
		<< std::endl;

  return s_instance;
}
