#include "LArCondUtils/LArFecLvTempDcsTool.h"
#include "LArCondUtils/LArHVToolMC.h" 
#include "LArCondUtils/LArHVToolDB.h" 
#include "LArCondUtils/LArFEBTempTool.h"
#include "LArCondUtils/LArHVPathologyDbAlg.h"
#include "LArCondUtils/LArHV2Ntuple.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY( LArFecLvTempDcsTool )
DECLARE_TOOL_FACTORY( LArHVToolMC )
DECLARE_TOOL_FACTORY( LArHVToolDB )
DECLARE_TOOL_FACTORY( LArFEBTempTool )
DECLARE_ALGORITHM_FACTORY( LArHVPathologyDbAlg )
DECLARE_ALGORITHM_FACTORY( LArHV2Ntuple )

DECLARE_FACTORY_ENTRIES(LArCondUtils) {
    DECLARE_TOOL( LArFecLvTempDcsTool )
    DECLARE_TOOL( LArHVToolMC )
    DECLARE_TOOL( LArHVToolDB )
    DECLARE_TOOL( LArFEBTempTool )
    DECLARE_ALGORITHM( LArHVPathologyDbAlg )
    DECLARE_ALGORITHM( LArHV2Ntuple )
}
