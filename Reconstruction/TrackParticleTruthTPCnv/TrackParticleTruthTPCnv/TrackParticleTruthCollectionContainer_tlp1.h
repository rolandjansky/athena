/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRK_PARTICLE_TRUTH_COLL_CNTNR_TLP1 
#define __TRK_PARTICLE_TRUTH_COLL_CNTNR_TLP1 

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include <vector>
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_p1.h"

class TrackParticleTruthCollectionContainerCnv_tlp1;


class TrackParticleTruthCollectionContainer_tlp1 {

  friend class TrackParticleTruthCollectionContainerCnv_tlp1;

public:

  TrackParticleTruthCollectionContainer_tlp1();
  virtual ~TrackParticleTruthCollectionContainer_tlp1();

private:

  std::vector<TrackParticleTruthCollectionContainer_p1> m_trackPartTruthCollConts;

};

#endif 
