#include "InDetDetDescrExample/ReadSiDetectorElements.h"
#include "InDetDetDescrExample/ReadTRT_DetectorElements.h"
#include "InDetDetDescrExample/TestSiAlignment.h"
#include "InDetDetDescrExample/TestTRT_Alignment.h"
#include "InDetDetDescrExample/InDetUpdateCaches.h"
#include "InDetDetDescrExample/PrintSiElements.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( ReadSiDetectorElements )
DECLARE_ALGORITHM_FACTORY( ReadTRT_DetectorElements )
DECLARE_ALGORITHM_FACTORY( TestSiAlignment )
DECLARE_ALGORITHM_FACTORY( TestTRT_Alignment )
DECLARE_ALGORITHM_FACTORY( InDetUpdateCaches )
DECLARE_ALGORITHM_FACTORY( PrintSiElements )

DECLARE_FACTORY_ENTRIES(InDetDetDescrExample) {
    DECLARE_ALGORITHM( ReadSiDetectorElements )
    DECLARE_ALGORITHM( ReadTRT_DetectorElements )
    DECLARE_ALGORITHM( TestSiAlignment )
    DECLARE_ALGORITHM( TestTRT_Alignment )
    DECLARE_ALGORITHM( InDetUpdateCaches )
    DECLARE_ALGORITHM( PrintSiElements )
}

