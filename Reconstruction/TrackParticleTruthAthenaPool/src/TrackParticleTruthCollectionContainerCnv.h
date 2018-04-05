/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCONTCNV_H
#define TRACKPARTICLETRUTHCOLLECTIONCONTCNV_H

/*
  @brief POOL converter for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"
#include "ParticleTruth/TrackParticleTruthVector.h"

class TrackParticleTruthCollectionContainer_tlp2;

typedef TrackParticleTruthCollectionContainer_tlp2 TrackParticleTruthCollectionContainerPERS;

typedef T_AthenaPoolCustomCnv<TrackParticleTruthCollectionContainer, TrackParticleTruthCollectionContainerPERS> TrackParticleTruthCollectionContainerCnvBase;

class TrackParticleTruthCollectionContainerCnv_tlp1;
class TrackParticleTruthCollectionContainerCnv_tlp2;


class TrackParticleTruthCollectionContainerCnv : public TrackParticleTruthCollectionContainerCnvBase
{
  friend class CnvFactory<TrackParticleTruthCollectionContainerCnv>;
protected:
public:
   TrackParticleTruthCollectionContainerCnv(ISvcLocator* svcloc);
   ~TrackParticleTruthCollectionContainerCnv();
protected:
  virtual TrackParticleTruthCollectionContainer* createTransient();
  virtual TrackParticleTruthCollectionContainerPERS* createPersistent(TrackParticleTruthCollectionContainer*);
private:
  TrackParticleTruthCollectionContainerCnv_tlp1* m_converter_p1;
  TrackParticleTruthCollectionContainerCnv_tlp2* m_converter_p2;
};

#endif
