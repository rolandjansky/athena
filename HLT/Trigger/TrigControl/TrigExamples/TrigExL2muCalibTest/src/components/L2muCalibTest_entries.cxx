#include "TrigExL2muCalibTest/L2muCalibTest.h"
#include "TrigExL2muCalibTest/TrigPartialEBFilter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( L2muCalibTest )
DECLARE_ALGORITHM_FACTORY( TrigPartialEBFilter )

DECLARE_FACTORY_ENTRIES(TrigExL2muCalibTest) {
    DECLARE_ALGORITHM( L2muCalibTest );
    DECLARE_ALGORITHM( TrigPartialEBFilter );
}
