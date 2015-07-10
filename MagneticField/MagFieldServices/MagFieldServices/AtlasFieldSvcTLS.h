/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AtlasFieldSvcTLS.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MAGFIELDSERVICES_ATLASFIELDSVCTLS_H
#define MAGFIELDSERVICES_ATLASFIELDSVCTLS_H 1

// MagField includes
#include "BFieldCond.h"
#include "BFieldZone.h"
#include "BFieldMeshZR.h"

namespace MagField {

  /** @class AtlasFieldSvcTLS

      Thread-local storage for AtlasFieldSvcTLS interface.

      @author Elmar.Ritsch -at- cern.ch
      @author Robert.Langenberg -at- cern.ch
    */

  struct AtlasFieldSvcTLS {

    /** Constructor */
    AtlasFieldSvcTLS() : isInitialized(false), cond(0), cache(), cacheZR() { ; }

    /** Is the current AtlasFieldSvcTLS object properly initialized */
    bool                           isInitialized;

    /** Pointer to the conductors in the current field zone (biot-savart component) */
    const std::vector<BFieldCond> *cond;

    /** The field caches */
    BFieldCache                    cache;
    BFieldCacheZR                  cacheZR;
  };

}

#endif //> !MAGFIELDSERVICES_ATLASFIELDSVCTLS_H
