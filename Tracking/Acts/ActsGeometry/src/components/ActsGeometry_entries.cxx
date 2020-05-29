/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsExtrapolationAlg.h"
#include "ActsGeometry/ActsWriteTrackingGeometry.h"
#include "ActsGeometry/ActsWriteTrackingGeometryTransforms.h"
#include "ActsGeometry/ActsTrackingGeometrySvc.h"
#include "ActsGeometry/ActsExtrapolationTool.h"

#include "ActsGeometry/ActsMaterialMapping.h"
#include "ActsGeometry/ActsSurfaceMappingTool.h"
// #include "ActsGeometry/ActsVolumeMappingTool.h"
#include "ActsGeometry/ActsObjWriterTool.h"
//#include "ActsGeometry/ActsExCellWriterSvc.h"
#include "ActsGeometry/ActsMaterialTrackWriterSvc.h"

//#include "ActsGeometry/GeomShiftCondAlg.h"
#include "ActsGeometry/NominalAlignmentCondAlg.h"
#include "ActsGeometry/ActsAlignmentCondAlg.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"

#include "ActsGeometry/ActsPropStepRootWriterSvc.h"
#include "ActsGeometry/ActsCaloTrackingVolumeBuilder.h"
#include "ActsGeometry/ActsMaterialStepConverterTool.h"
#include "ActsGeometry/ActsMaterialJsonWriterTool.h"

DECLARE_COMPONENT( ActsExtrapolationAlg )
DECLARE_COMPONENT( ActsWriteTrackingGeometry )
DECLARE_COMPONENT( ActsWriteTrackingGeometryTransforms )
DECLARE_COMPONENT( ActsTrackingGeometrySvc )
DECLARE_COMPONENT( ActsExtrapolationTool )

DECLARE_COMPONENT( ActsMaterialMapping )
DECLARE_COMPONENT( ActsSurfaceMappingTool )
// DECLARE_COMPONENT( ActsVolumeMappingTool )
DECLARE_COMPONENT( ActsObjWriterTool )
//DECLARE_COMPONENT( ActsExCellWriterSvc )
DECLARE_COMPONENT( ActsMaterialTrackWriterSvc )
DECLARE_COMPONENT( ActsMaterialStepConverterTool )
DECLARE_COMPONENT( ActsMaterialJsonWriterTool )

//DECLARE_COMPONENT( GeomShiftCondAlg )
DECLARE_COMPONENT( NominalAlignmentCondAlg )
DECLARE_COMPONENT( ActsTrackingGeometryTool )

DECLARE_COMPONENT( ActsPropStepRootWriterSvc )
DECLARE_COMPONENT( ActsAlignmentCondAlg )
DECLARE_COMPONENT( ActsCaloTrackingVolumeBuilder )
