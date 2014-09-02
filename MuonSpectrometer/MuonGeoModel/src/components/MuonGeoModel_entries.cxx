#include "MuonGeoModel/MuonDetectorTool.h"
//#include "MuonGeoModel/ShieldDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(MuonDetectorTool)
//DECLARE_TOOL_FACTORY(ShieldDetectorTool)

DECLARE_FACTORY_ENTRIES(MuonGeoModel) {
    DECLARE_ALGTOOL  ( MuonDetectorTool )
        //    DECLARE_ALGTOOL  ( ShieldDetectorTool )
}
 

