/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AtlasRegionName.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "AtlasDetDescr/AtlasRegionHelper.h"

namespace AtlasDetDescr {

  const char * AtlasRegionHelper::getName( int region ) {
    return getName( AtlasDetDescr::AtlasRegion( region ) );
  }

  const char * AtlasRegionHelper::getName( const AtlasDetDescr::AtlasRegion region ) {

    if      ( region == AtlasDetDescr::fAtlasID      ) return "AtlasID";
    else if ( region == AtlasDetDescr::fAtlasForward ) return "AtlasForward";
    else if ( region == AtlasDetDescr::fAtlasCalo    ) return "AtlasCalo";
    else if ( region == AtlasDetDescr::fAtlasMS      ) return "AtlasMS";
    else if ( region == AtlasDetDescr::fAtlasCavern  ) return "AtlasCavern";
    else                                               return "UndefinedAtlasRegion";

  }

} // end of namespace

