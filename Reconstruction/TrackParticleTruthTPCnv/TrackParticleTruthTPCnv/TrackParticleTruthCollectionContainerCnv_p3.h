/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_P3
#define  __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_P3

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_p3.h"



class TrackParticleTruthCollectionContainerCnv_p3: 
  public T_AthenaPoolTPCnvBase<TrackParticleTruthCollectionContainer,
			       TrackParticleTruthCollectionContainer_p3>
{
public:
  TrackParticleTruthCollectionContainerCnv_p3();
  
  virtual ~TrackParticleTruthCollectionContainerCnv_p3();
  
  virtual void transToPers(const TrackParticleTruthCollectionContainer *transVect, 
			   TrackParticleTruthCollectionContainer_p3 *persVect,
			   MsgStream &msg );
    
  virtual void persToTrans(const TrackParticleTruthCollectionContainer_p3 *persVect, 
			   TrackParticleTruthCollectionContainer *transVect, 
			   MsgStream &msg );
};

#endif
