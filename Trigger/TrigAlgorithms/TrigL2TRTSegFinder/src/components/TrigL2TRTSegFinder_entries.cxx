#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigL2TRTSegFinder/TrigTRTSegFinder.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

DECLARE_ALGORITHM_FACTORY(TrigTRTSegFinder)

DECLARE_FACTORY_ENTRIES(TrigL2TRTSegFinder) {
    DECLARE_ALGORITHM(TrigTRTSegFinder)
}
