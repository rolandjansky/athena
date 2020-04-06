/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetectorDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef READOUTGEOMETRYBASE_DETECTORDESIGN_H
#define READOUTGEOMETRYBASE_DETECTORDESIGN_H

// base class
#include "GeoModelKernel/RCBase.h"

/** @class DetectorDesign

   Base class for the detector design classes for ITk and HGTD.
   These hold the local description of the detector elements which are
   shared by a number of detector elements.

 */

class DetectorDesign: public RCBase {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    // Constructor
    DetectorDesign();

    // Destructor
    ~DetectorDesign();

};

#endif // READOUTGEOMETRYBASE_DETECTORDESIGN_H
