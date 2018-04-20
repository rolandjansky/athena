#include "GeomACTS/ActsExtrapolation.h"
#include "GeomACTS/ActsWriteTrackingGeometry.h"
#include "GeomACTS/ActsMaterialMapping.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GeomACTS/TrackingGeometrySvc.h"
#include "GeomACTS/ExtrapolationTool.h"
#include "GeomACTS/ObjWriterTool.h"
#include "GeomACTS/ExCellWriterSvc.h"
#include "GeomACTS/MaterialTrackWriterSvc.h"

DECLARE_COMPONENT( ActsExtrapolation )
DECLARE_COMPONENT( ActsWriteTrackingGeometry )
DECLARE_COMPONENT( ActsMaterialMapping )
DECLARE_COMPONENT( Acts::TrackingGeometrySvc )
DECLARE_COMPONENT( Acts::ExtrapolationTool )
DECLARE_COMPONENT( Acts::ObjWriterTool )
DECLARE_COMPONENT( Acts::ExCellWriterSvc )
DECLARE_COMPONENT( Acts::MaterialTrackWriterSvc )
