/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRK_PARTICLE_TRUTH_COLL_CNTNR_TLP3 
#define __TRK_PARTICLE_TRUTH_COLL_CNTNR_TLP3 

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include <vector>
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_p3.h"

class TrackParticleTruthCollectionContainerCnv_tlp3;


class TrackParticleTruthCollectionContainer_tlp3 {

  friend class TrackParticleTruthCollectionContainerCnv_tlp3;

public:

  TrackParticleTruthCollectionContainer_tlp3();
  virtual ~TrackParticleTruthCollectionContainer_tlp3();

private:

  std::vector<TrackParticleTruthCollectionContainer_p3> m_trackPartTruthCollConts;

};

#endif 
