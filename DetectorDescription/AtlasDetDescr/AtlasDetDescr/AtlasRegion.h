/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AtlasRegion.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ATLASDETDESCR_ATLASREGION_H
#define ATLASDETDESCR_ATLASREGION_H 1

#include <assert.h>

// use these macros to check a given AtlasRegion for its validity
#define validAtlasRegion(region) ( (region<AtlasDetDescr::fNumAtlasRegions) && (region>=AtlasDetDescr::fFirstAtlasRegion) )
#define assertAtlasRegion(region) ( assert(validAtlasRegion(region)) )

namespace AtlasDetDescr {

 /** @enum AtlasRegion
   
     A simple enum of ATLAS regions and sub-detectors.

     @author Elmar.Ritsch -at- cern.ch
   */

   enum AtlasRegion {       
        // Unset
            fUndefinedAtlasRegion = 0,
        // first Geometry element in enum, used in e.g. loops
            fFirstAtlasRegion     = 1,
        // ATLAS Detector setup: geometrical ones
            fAtlasID              = 1,
            fAtlasForward         = 2,
            fAtlasCalo            = 3,
            fAtlasMS              = 4,
            fAtlasCavern          = 5,
        // number of defined GeoIDs
            fNumAtlasRegions      = 6
   };

} // end of namespace

#endif // ATLASDETDESCR_ATLASREGION
