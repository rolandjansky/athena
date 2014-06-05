#include "TestLArDetDescr/TestCaloDDE.h"
#include "TestLArDetDescr/TestCaloGeom.h"
#include "TestLArDetDescr/TestLArTT.h"
#include "TestLArDetDescr/TestLArMaterial.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TestCaloDDE )
DECLARE_ALGORITHM_FACTORY( TestCaloGeom )
DECLARE_ALGORITHM_FACTORY( TestLArTT )
DECLARE_ALGORITHM_FACTORY( TestLArMaterial )

DECLARE_FACTORY_ENTRIES(TestLArDetDescr) {
    DECLARE_ALGORITHM( TestCaloDDE );
    DECLARE_ALGORITHM( TestCaloGeom );
    DECLARE_ALGORITHM( TestLArTT );
    DECLARE_ALGORITHM( TestLArMaterial );
}
