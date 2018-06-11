#include "ActsGeometry/ActsExtrapolation.h"
#include "ActsGeometry/ActsWriteTrackingGeometry.h"
#include "ActsGeometry/ActsMaterialMapping.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ActsGeometry/TrackingGeometrySvc.h"
#include "ActsGeometry/ExtrapolationTool.h"
#include "ActsGeometry/ObjWriterTool.h"
#include "ActsGeometry/ExCellWriterSvc.h"
#include "ActsGeometry/MaterialTrackWriterSvc.h"

DECLARE_COMPONENT( ActsExtrapolation )
DECLARE_COMPONENT( ActsWriteTrackingGeometry )
DECLARE_COMPONENT( ActsMaterialMapping )
DECLARE_COMPONENT( Acts::TrackingGeometrySvc )
DECLARE_COMPONENT( Acts::ExtrapolationTool )
DECLARE_COMPONENT( Acts::ObjWriterTool )
DECLARE_COMPONENT( Acts::ExCellWriterSvc )
DECLARE_COMPONENT( Acts::MaterialTrackWriterSvc )
