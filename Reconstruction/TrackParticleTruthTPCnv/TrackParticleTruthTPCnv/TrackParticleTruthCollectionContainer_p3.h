/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  __TRK_PARTICLE_TRUTH_COLL_CNTNR_P3 
#define  __TRK_PARTICLE_TRUTH_COLL_CNTNR_P3 

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrackParticleTruthCollectionContainer_p3: public std::vector<TPObjRef>
{
};

#endif

