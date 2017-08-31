#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigParticleCreation/TrigParticleCreator.h"
#include "InDetTrigParticleCreation/TrigTrackingxAODCnv.h"
#include "../TrigTrackingxAODCnvMT.h"
#include "InDetTrigParticleCreation/TrigVertexxAODCnv.h"
#include "TrkTrack/Track.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigParticleCreator )
DECLARE_ALGORITHM_FACTORY( TrigTrackingxAODCnv )
DECLARE_ALGORITHM_FACTORY( TrigTrackingxAODCnvMT )
DECLARE_ALGORITHM_FACTORY( TrigVertexxAODCnv )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( InDetTrigParticleCreation )
{
    DECLARE_ALGORITHM( TrigParticleCreator )
    DECLARE_ALGORITHM( TrigTrackingxAODCnv )
    DECLARE_ALGORITHM( TrigTrackingxAODCnvMT )
    DECLARE_ALGORITHM( TrigVertexxAODCnv )
}
