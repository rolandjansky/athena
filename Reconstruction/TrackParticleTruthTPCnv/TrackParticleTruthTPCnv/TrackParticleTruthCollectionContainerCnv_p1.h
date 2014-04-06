/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_P1
#define  __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_P1

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_p1.h"



class TrackParticleTruthCollectionContainerCnv_p1: 
  public T_AthenaPoolTPCnvBase<TrackParticleTruthCollectionContainer,
			       TrackParticleTruthCollectionContainer_p1>
{
public:
  TrackParticleTruthCollectionContainerCnv_p1();
  
  virtual ~TrackParticleTruthCollectionContainerCnv_p1();
  
  virtual void transToPers(const TrackParticleTruthCollectionContainer *transVect, 
			   TrackParticleTruthCollectionContainer_p1 *persVect,
			   MsgStream &msg );
    
  virtual void persToTrans(const TrackParticleTruthCollectionContainer_p1 *persVect, 
			   TrackParticleTruthCollectionContainer *transVect, 
			   MsgStream &msg );
};

#endif
