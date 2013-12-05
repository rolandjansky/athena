/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AtlasRegionHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ATLASDETDESCR_ATLASREGIONHELPER_H
#define ATLASDETDESCR_ATLASREGIONHELPER_H 1

#include "AtlasDetDescr/AtlasRegion.h"

namespace AtlasDetDescr {

 /** @class AtlasRegionHelper
   
     A simple helper class (functional) to translate AtlasDetDesc::AtlasRegion enums
     into a string description.

     @author Elmar.Ritsch -at- cern.ch
   */

   class AtlasRegionHelper {       
     public:
       static const char * getName( int region );
       static const char * getName( AtlasDetDescr::AtlasRegion region );

     private:
       AtlasRegionHelper();

   };

} // end of namespace

#endif // ATLASDETDESCR_ATLASREGIONHELPER_H
