/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRK_PARTICLE_TRUTH_COLL_CNTNR_TLP2 
#define __TRK_PARTICLE_TRUTH_COLL_CNTNR_TLP2 

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include <vector>
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_p2.h"

class TrackParticleTruthCollectionContainerCnv_tlp2;


class TrackParticleTruthCollectionContainer_tlp2 {

  friend class TrackParticleTruthCollectionContainerCnv_tlp2;

public:

  TrackParticleTruthCollectionContainer_tlp2();
  virtual ~TrackParticleTruthCollectionContainer_tlp2();

private:

  std::vector<TrackParticleTruthCollectionContainer_p2> m_trackPartTruthCollConts;

};

#endif 
