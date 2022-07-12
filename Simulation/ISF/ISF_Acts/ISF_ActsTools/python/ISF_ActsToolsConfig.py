# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def getActsFatrasSimTool(name="ISF_ActsFatrasSimTool", **kwargs):

    from ActsGeometry.ActsGeometryConf import ActsTrackingGeometrySvc
    trkGeomSvc = ActsTrackingGeometrySvc("ActsTrackingGeometrySvc")
    trkGeomSvc.UseMaterialMap = True
    trkGeomSvc.MaterialMapInputFile = "/eos/project-a/acts/public/MaterialMaps/ATLAS/material-maps.json" # TO BE moved to conditions area on CVMFS 
    trkGeomSvc.BuildSubDetectors = [
      "Pixel",
      "SCT",
      "TRT"
    ]

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += trkGeomSvc

    from ISF_ActsTools.ISF_ActsToolsConf import ISF__ActsFatrasSimTool
    return ISF__ActsFatrasSimTool(name, **kwargs)
