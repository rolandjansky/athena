/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// VDetectorParameters
// 30-Jun-2003 Bill Seligman

// Interface class that defines how the different detector-parameter
// methods return values to the simulation.

// Purpose: This class isolates the method of storing detector
// parameters from the simulation itself.  Detector parameters might
// be stored as constants, as entries in a MySQL database, or in some
// other form; they might be stored in AGE structures or in some other
// form.  This interface separates the details of how the parameters
// are stored from the simulation's geometry and hit-calculation
// routines.

#ifndef LARGEOCODE_VDETECTORPARAMETERS_H
#define LARGEOCODE_VDETECTORPARAMETERS_H

#include "CxxUtils/CachedUniquePtr.h"
#include "CxxUtils/checker_macros.h"
#include <string>
#include <limits.h>

namespace LArGeo {

  class VDetectorParameters {

  public:

    virtual ~VDetectorParameters() {};

    // This class uses a modified version of the singleton pattern.
    // It has to be explicitly initialized with a "SetInstance" call,
    // whose parameter is some class that inherits from
    // VDetectorParameters and implements the GetValue method.
    // Thereafter, any routine in the simulation can use the result of
    // "GetInstance" to find out how to get its parameters.

    static void SetInstance(std::unique_ptr<VDetectorParameters>);
    static const VDetectorParameters* GetInstance();

    // This is the general access method for obtaining valued from
    // some source of detector parameters.  THe paramter is identified
    // by some string.  Optionally, up to five integers can be used
    // (typically to index an array of some sort).  The default value
    // of these integers, INT_MIN, comes from limits.h; it's the
    // minimum possible integer value that can be represented on the
    // computer.

    virtual double GetValue(const std::string&, 
			    const int i0 = INT_MIN,
			    const int i1 = INT_MIN,
			    const int i2 = INT_MIN,
			    const int i3 = INT_MIN,
			    const int i4 = INT_MIN ) const = 0;

  protected:
    VDetectorParameters() {};

  private:
    static CxxUtils::CachedUniquePtr<VDetectorParameters> s_instance ATLAS_THREAD_SAFE;

  };

} // namespace LArGeo

#endif // LARGEOCODE_VDETECTORPARAMETERS_H
