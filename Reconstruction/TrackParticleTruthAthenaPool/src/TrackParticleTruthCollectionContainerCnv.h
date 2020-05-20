/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCONTCNV_H
#define TRACKPARTICLETRUTHCOLLECTIONCONTCNV_H

/*
  @brief POOL converter for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"

class TrackParticleTruthCollectionContainer_tlp3;

typedef TrackParticleTruthCollectionContainer_tlp3 TrackParticleTruthCollectionContainerPERS;

typedef T_AthenaPoolCustomCnv<TrackParticleTruthCollectionContainer, TrackParticleTruthCollectionContainerPERS> TrackParticleTruthCollectionContainerCnvBase;

class TrackParticleTruthCollectionContainerCnv_tlp1;
class TrackParticleTruthCollectionContainerCnv_tlp2;
class TrackParticleTruthCollectionContainerCnv_tlp3;


class TrackParticleTruthCollectionContainerCnv : public TrackParticleTruthCollectionContainerCnvBase
{
  friend class CnvFactory<TrackParticleTruthCollectionContainerCnv>;
protected:
   TrackParticleTruthCollectionContainerCnv(ISvcLocator* svcloc);
   ~TrackParticleTruthCollectionContainerCnv();
  virtual TrackParticleTruthCollectionContainer* createTransient();
  virtual TrackParticleTruthCollectionContainerPERS* createPersistent(TrackParticleTruthCollectionContainer*);
private:
  TrackParticleTruthCollectionContainerCnv_tlp1* m_converter_p1;
  TrackParticleTruthCollectionContainerCnv_tlp2* m_converter_p2;
  TrackParticleTruthCollectionContainerCnv_tlp3* m_converter_p3;
};

#endif
