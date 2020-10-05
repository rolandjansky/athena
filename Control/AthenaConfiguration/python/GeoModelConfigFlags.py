# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD, GetDetDescrInfo

def createGeoModelConfigFlags():
    gcf=AthConfigFlags()
    gcf.addFlag('GeoModel.Layout', 'atlas') # replaces global.GeoLayout
    gcf.addFlag("GeoModel.AtlasVersion", lambda prevFlags : (prevFlags.Input.Files and prevFlags.Input.Files != ["_ATHENA_GENERIC_INPUTFILE_NAME_"] and GetFileMD(prevFlags.Input.Files).get("GeoAtlas",None)) or "ATLAS-R2-2016-01-00-01") #
    gcf.addFlag("GeoModel.Align.Dynamic", lambda prevFlags : (not prevFlags.Detector.Simulate and not prevFlags.Input.isMC))
    gcf.addFlag("GeoModel.StripGeoType", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('StripGeoType',"GMX")) # Based on CommonGeometryFlags.StripGeoType
    gcf.addFlag("GeoModel.Run", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('Run',"RUN2")) # Based on CommonGeometryFlags.Run (InDetGeometryFlags.isSLHC replaced by GeoModel.Run=="RUN4")
    gcf.addFlag("GeoModel.Type", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('GeoType',"UNDEFINED")) # Geometry type in {ITKLoI, ITkLoI-VF, etc...}
    gcf.addFlag("GeoModel.IBLLayout", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('IBLlayout',"UNDEFINED")) # IBL layer layout  in {"planar", "3D", "noIBL", "UNDEFINED"}
    return gcf
