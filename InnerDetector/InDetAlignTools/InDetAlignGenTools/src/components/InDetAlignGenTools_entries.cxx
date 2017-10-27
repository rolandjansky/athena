#include "InDetAlignGenTools/InDetAlignDBTool.h"
#include "InDetAlignGenTools/InDetAlignTrackSelTool.h"
#include "InDetAlignGenTools/InDetAlignFillTrack.h"

#include "InDetAlignGenTools/InDetAlignOverlapTool.h"
#include "InDetAlignGenTools/InDetAlignFillSiCluster.h"
#include "InDetAlignGenTools/RefitSiOnlyTool.h"
#include "InDetAlignGenTools/InDetAlignHitQualSelTool.h"


DECLARE_COMPONENT( InDetAlignDBTool )
DECLARE_COMPONENT( InDetAlignTrackSelTool )
DECLARE_COMPONENT( InDetAlignFillTrack )

DECLARE_COMPONENT( InDetAlignOverlapTool )
DECLARE_COMPONENT( InDetAlignFillSiCluster )
DECLARE_COMPONENT( InDetAlignHitQualSelTool )
DECLARE_COMPONENT( InDetAlignment::RefitSiOnlyTool )

