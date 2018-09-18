#include "ActsGeometry/ActsExtrapolationAlg.h"
#include "ActsGeometry/ActsWriteTrackingGeometry.h"
#include "ActsGeometry/ActsTrackingGeometrySvc.h"
#include "ActsGeometry/ActsExtrapolationTool.h"
#include "ActsGeometry/ObjWriterTool.h"
#include "ActsGeometry/ExCellWriterSvc.h"
#include "ActsGeometry/MaterialTrackWriterSvc.h"

#include "ActsGeometry/GeomShiftCondAlg.h"
#include "ActsGeometry/NominalAlignmentCondAlg.h"
#include "ActsGeometry/ActsAlignedExtrapAlg.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"

DECLARE_COMPONENT( ActsExtrapolationAlg )
DECLARE_COMPONENT( ActsWriteTrackingGeometry )
DECLARE_COMPONENT( ActsTrackingGeometrySvc )
DECLARE_COMPONENT( ActsExtrapolationTool )
DECLARE_COMPONENT( Acts::ObjWriterTool )
DECLARE_COMPONENT( Acts::ExCellWriterSvc )
DECLARE_COMPONENT( Acts::MaterialTrackWriterSvc )

DECLARE_COMPONENT( GeomShiftCondAlg )
DECLARE_COMPONENT( ActsAlignedExtrapAlg )
DECLARE_COMPONENT( NominalAlignmentCondAlg )
DECLARE_COMPONENT( ActsTrackingGeometryTool )
