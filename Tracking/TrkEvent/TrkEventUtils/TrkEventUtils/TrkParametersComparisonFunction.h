/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkParametersComparisonFunction, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTUTILS_TRACKPARAMETERSORTER_H
#define TRKEVENTUTILS_TRACKPARAMETERSORTER_H

#include "TrkEventUtils/ComparisonFunction.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

  /** 
    @class TrkParametersComparisonFunction 
    @brief std::binary_function for parameter position comparisons,
           now implemented as hidden template with ComparisonFunction<TrackParameters>.
    
    @author Andreas.Salzburger@cern.ch
     
    */

   typedef ComparisonFunction<TrackParameters> TrkParametersComparisonFunction; 
  
} // end of namespace

#endif

