# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
## postInclude to add acts refitting algorithm
 
print ("entering configurations...")

# this is needed to create the ACTS Tracking Geometry Tool if not yet created
# and added to the ToolSvc

from ActsGeometry.ActsGeometryConf import ActsTrackingGeometryTool

class ConfiguredActsRefittingTrackingGeometry( ActsTrackingGeometryTool ) :   
  # constructor
  def __init__(self,name):
    
    subDetectors = []
    subDetectors += ["Pixel"]
    subDetectors += ["SCT"]
    subDetectors += ["TRT"]
    subDetectors += ["Calo"]

    from ActsGeometry.ActsGeometryConf import ActsTrackingGeometrySvc
    ActsTrackingGeometrySvc = ActsTrackingGeometrySvc(name = "ActsTrackingGeometrySvc",
                                                      BuildSubDetectors=subDetectors)
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    ActsTrackingGeometrySvc.CaloVolumeBuilder = CompFactory.ActsCaloTrackingVolumeBuilder()                                                  
    ActsTrackingGeometrySvc.UseMaterialMap = True
    ActsTrackingGeometrySvc.BuildBeamPipe = True
    ActsTrackingGeometrySvc.MaterialMapInputFile = "/eos/project-a/acts/public/MaterialMaps/ATLAS-material-maps.json"
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += ActsTrackingGeometrySvc

    from ActsGeometry.ActsGeometryConf import NominalAlignmentCondAlg
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "NominalAlignmentCondAlg"):
      condSeq += NominalAlignmentCondAlg(name = "NominalAlignmentCondAlg")
    
    ActsTrackingGeometryTool.__init__(self,
                                      name,
                                      ActsTrackingGeometrySvc=ActsTrackingGeometrySvc)

actsReFittingTrackingGeometry = ConfiguredActsRefittingTrackingGeometry(name = "ActsReFittingTrackingGeometryTool")
ToolSvc += actsReFittingTrackingGeometry

actsExtrapolationTool = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool", TrackingGeometryTool = actsReFittingTrackingGeometry, MaxSteps = 10000)

from ActsGeometry.ActsGeometryConf import ActsATLASConverterTool
actsAtlasConverter = ActsATLASConverterTool(name="ActsATLASConverterTool", TrackingGeometryTool = actsReFittingTrackingGeometry)
ToolSvc+=actsAtlasConverter

import InDetRecExample.TrackingCommon as TrackingCommon

from ActsTrkFitting.ActsTrkFittingConf import ActsReFitterAlg    
from ActsTrkFitting.ActsTrkFittingConf import ActsKalmanFitter

summary = TrackingCommon.getInDetTrackSummaryToolNoHoleSearch()

actsKF = ActsKalmanFitter(name = "ActsKalmanFitter",
                           ExtrapolationTool    = actsExtrapolationTool,
                           TrackingGeometryTool = actsReFittingTrackingGeometry,
                           ATLASConverterTool   = actsAtlasConverter,
                           SummaryTool          = summary
)
ToolSvc+=actsKF

actsfitter = ActsReFitterAlg(name = "fitter",
                           ActsKalmanFitter    = actsKF,
                           TrackName           = "ResolvedTracks"
)

actsfitter.OutputLevel = INFO

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence+=actsfitter

print ("exiting configurations... all done! Bye!")
