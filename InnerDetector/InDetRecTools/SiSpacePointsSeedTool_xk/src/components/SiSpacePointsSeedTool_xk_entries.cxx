
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ATLxk.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_LowMomentum.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_BeamGas.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_Cosmic.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_HeavyIon.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_Trigger.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ITK.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_ATLxk      )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_LowMomentum)
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_BeamGas    )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_Cosmic     )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_HeavyIon   )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_Trigger    )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_ITK        )

DECLARE_FACTORY_ENTRIES( SiSpacePointsSeedTool_xk )
{
	DECLARE_TOOL( SiSpacePointsSeedMaker_ATLxk      )
	DECLARE_TOOL( SiSpacePointsSeedMaker_LowMomentum)
	DECLARE_TOOL( SiSpacePointsSeedMaker_BeamGas    )
	DECLARE_TOOL( SiSpacePointsSeedMaker_Cosmic     )
	DECLARE_TOOL( SiSpacePointsSeedMaker_HeavyIon   )
	DECLARE_TOOL( SiSpacePointsSeedMaker_Trigger    )
	DECLARE_TOOL( SiSpacePointsSeedMaker_ITK        )
}





