#include "InDetAlignGenTools/InDetAlignDBTool.h"
#include "InDetAlignGenTools/InDetAlignTrackSelTool.h"
#include "InDetAlignGenTools/InDetAlignFillTrack.h"

#include "InDetAlignGenTools/InDetAlignOverlapTool.h"
#include "InDetAlignGenTools/InDetAlignFillSiCluster.h"
#include "InDetAlignGenTools/RefitSiOnlyTool.h"
#include "InDetAlignGenTools/InDetAlignHitQualSelTool.h"


DECLARE_TOOL_FACTORY( InDetAlignDBTool )
DECLARE_TOOL_FACTORY( InDetAlignTrackSelTool )
DECLARE_TOOL_FACTORY( InDetAlignFillTrack )

DECLARE_TOOL_FACTORY( InDetAlignOverlapTool )
DECLARE_TOOL_FACTORY( InDetAlignFillSiCluster )
DECLARE_TOOL_FACTORY( InDetAlignHitQualSelTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDetAlignment, RefitSiOnlyTool )

