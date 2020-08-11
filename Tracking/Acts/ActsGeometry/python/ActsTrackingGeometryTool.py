# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##################################################################################
# The ActsTrackingGeometryTool fragment
#
# usage: 
#   include('ActsGeometry/ActsTrackingGeometryTool.py')
##################################################################################

from __future__ import print_function

# import the DetFlags for the setting
from AthenaCommon.DetFlags import DetFlags

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

# import the Extrapolator configurable
from ActsGeometry.ActsGeometryConf import ActsTrackingGeometryTool
   
class ConfiguredActsTrackingGeometry( ActsTrackingGeometryTool ) :   
  # constructor
  def __init__(self,name):
    
    subDetectors = []
    if DetFlags.pixel_on():
      subDetectors += ["Pixel"]
    if DetFlags.SCT_on():
      subDetectors += ["SCT"]
    if DetFlags.TRT_on():
      subDetectors += ["TRT"]
    if DetFlags.Calo_on():
      subDetectors += ["Calo"]
        
    from ActsGeometry.ActsGeometryConf import ActsTrackingGeometrySvc
    actsTrackingGeometrySvc = ActsTrackingGeometrySvc(name = "ActsTrackingGeometrySvc",
                                                      BuildSubDetectors = subDetectors)
    
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += actsTrackingGeometrySvc
    
    
    from ActsGeometry.ActsGeometryConf import NominalAlignmentCondAlg
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "NominalAlignmentCondAlg"):
      condSeq += NominalAlignmentCondAlg(name = "NominalAlignmentCondAlg")
    
    ActsTrackingGeometryTool.__init__(self,
                                      name,
                                      ActsTrackingGeometrySvc=actsTrackingGeometrySvc)
    
##################################################################################    

# now create the instance
from AthenaCommon.AppMgr import ToolSvc 
if not hasattr(ToolSvc, "ActsTrackingGeometryTool"):
  actsTrackingGeometryTool = ConfiguredActsTrackingGeometry(name = "ActsTrackingGeometryTool")
  # add it to the ServiceManager
  ToolSvc += actsTrackingGeometryTool

