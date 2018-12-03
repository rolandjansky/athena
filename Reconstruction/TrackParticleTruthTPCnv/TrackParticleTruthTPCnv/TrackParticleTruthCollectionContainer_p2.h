/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  __TRK_PARTICLE_TRUTH_COLL_CNTNR_P2 
#define  __TRK_PARTICLE_TRUTH_COLL_CNTNR_P2 

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrackParticleTruthCollectionContainer_p2: public std::vector<TPObjRef>
{
};

#endif

