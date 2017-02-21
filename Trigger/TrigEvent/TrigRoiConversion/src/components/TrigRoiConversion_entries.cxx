// $Id: TrigRoiConversion_entries.cxx 781704 2016-11-01 17:50:14Z krasznaa $

// Gaudi include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../RoiWriter.h"

DECLARE_ALGORITHM_FACTORY( RoiWriter )

DECLARE_FACTORY_ENTRIES( TrigRoiConversion ) {
    DECLARE_ALGORITHM( RoiWriter )
}
