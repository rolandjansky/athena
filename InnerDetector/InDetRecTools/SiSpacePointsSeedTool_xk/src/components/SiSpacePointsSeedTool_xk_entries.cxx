#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ATLxk.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_LowMomentum.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_BeamGas.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_Cosmic.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_HeavyIon.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_Trigger.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ITK.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ITKNew.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_TrkSeeded.h"
#include "SiSpacePointsSeedTool_xk/RoISeedTool.h"
#include "SiSpacePointsSeedTool_xk/FileRoISeedTool.h"
#include "SiSpacePointsSeedTool_xk/ZWindowRoISeedTool.h"
#include "SiSpacePointsSeedTool_xk/TruthHSRoISeedTool.h"
#include "SiSpacePointsSeedTool_xk/RandomRoISeedTool.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_ATLxk      )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_LowMomentum)
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_BeamGas    )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_Cosmic     )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_HeavyIon   )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_Trigger    )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_ITK        )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_ITKNew     )
DECLARE_TOOL_FACTORY( SiSpacePointsSeedMaker_TrkSeeded  )
DECLARE_TOOL_FACTORY( RoISeedTool                       )
DECLARE_TOOL_FACTORY( FileRoISeedTool                       )
DECLARE_TOOL_FACTORY( ZWindowRoISeedTool                )
DECLARE_TOOL_FACTORY( TruthHSRoISeedTool                )
DECLARE_TOOL_FACTORY( RandomRoISeedTool                )

DECLARE_FACTORY_ENTRIES( SiSpacePointsSeedTool_xk )
{
	DECLARE_TOOL( SiSpacePointsSeedMaker_ATLxk      )
	DECLARE_TOOL( SiSpacePointsSeedMaker_LowMomentum)
	DECLARE_TOOL( SiSpacePointsSeedMaker_BeamGas    )
	DECLARE_TOOL( SiSpacePointsSeedMaker_Cosmic     )
	DECLARE_TOOL( SiSpacePointsSeedMaker_HeavyIon   )
	DECLARE_TOOL( SiSpacePointsSeedMaker_Trigger    )
	DECLARE_TOOL( SiSpacePointsSeedMaker_ITK        )
	DECLARE_TOOL( SiSpacePointsSeedMaker_ITKNew     )
	DECLARE_TOOL( SiSpacePointsSeedMaker_TrkSeeded  )
	DECLARE_TOOL( RoISeedTool                       )
	DECLARE_TOOL( FileRoISeedTool                   )
	DECLARE_TOOL( ZWindowRoISeedTool                )
	DECLARE_TOOL( TruthHSRoISeedTool                )
	DECLARE_TOOL( RandomRoISeedTool                 )
}





