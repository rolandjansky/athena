#include "../SimKernel.h"
#include "../SimKernelMT.h"
#include "../CollectionMerger.h"
#include "../SimHitTreeCreator.h"
#ifndef SIMULATIONBASE
#include "../SimEventFilter.h"
#endif // SimEventFilter currently will not compile in the AthSimulation Project
#include "../RenameHitCollectionsAlg.h"

DECLARE_COMPONENT( ISF::SimKernel )
DECLARE_COMPONENT( ISF::SimKernelMT )
DECLARE_COMPONENT( ISF::CollectionMerger )
DECLARE_COMPONENT( ISF::SimHitTreeCreator )
#ifndef SIMULATIONBASE
DECLARE_COMPONENT( ISF::SimEventFilter )
#endif // SimEventFilter currently will not compile in the AthSimulation Project
DECLARE_COMPONENT( ISF::RenameHitCollectionsAlg )
