#include "InDetAlignGenTools/InDetAlignDBTool.h"
#include "InDetAlignGenTools/InDetAlignTrackSelTool.h"
#include "InDetAlignGenTools/InDetAlignFillTrack.h"
//**
#include "InDetAlignGenTools/InDetAlignOverlapTool.h"
#include "InDetAlignGenTools/InDetAlignFillSiCluster.h"
#include "InDetAlignGenTools/RefitSiOnlyTool.h"
#include "InDetAlignGenTools/InDetAlignHitQualSelTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY( InDetAlignDBTool )
DECLARE_TOOL_FACTORY( InDetAlignTrackSelTool )
DECLARE_TOOL_FACTORY( InDetAlignFillTrack )
//**
DECLARE_TOOL_FACTORY( InDetAlignOverlapTool )
DECLARE_TOOL_FACTORY( InDetAlignFillSiCluster )
DECLARE_TOOL_FACTORY( InDetAlignHitQualSelTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDetAlignment, RefitSiOnlyTool )

DECLARE_FACTORY_ENTRIES(InDetAlignGenTools) {
    DECLARE_TOOL( InDetAlignDBTool );
    DECLARE_TOOL( InDetAlignTrackSelTool );
    DECLARE_TOOL( InDetAlignFillTrack );
    //**
    DECLARE_TOOL( InDetAlignOverlapTool );
    DECLARE_TOOL( InDetAlignFillSiCluster );
    DECLARE_TOOL( InDetAlignHitQualSelTool );
    DECLARE_NAMESPACE_TOOL( InDetAlignment, RefitSiOnlyTool );
}
