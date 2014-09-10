# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AppMgr import ToolSvc

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationTypeBuilder
MuonStationTypeBuilder= Muon__MuonStationTypeBuilder(name = 'MuonStationTypeBuilder')
ToolSvc += MuonStationTypeBuilder

# muon active/passive geometry setup
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationBuilder
MuonStationBuilder= Muon__MuonStationBuilder(name = 'MuonStationBuilder')
MuonStationBuilder.StationTypeBuilder = MuonStationTypeBuilder
ToolSvc += MuonStationBuilder

from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonInertMaterialBuilder
MuonInertMaterialBuilder= Muon__MuonInertMaterialBuilder(name = 'MuonInertMaterialBuilder')
ToolSvc += MuonInertMaterialBuilder 

# muon tracking geometry builder
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonTrackingGeometryBuilder
MuonTrackingGeometryBuilder= Muon__MuonTrackingGeometryBuilder(name = 'MuonTrackingGeometryBuilder')
MuonTrackingGeometryBuilder.EntryVolumeName = TrkDetFlags.MuonSystemEntryVolumeName()
MuonTrackingGeometryBuilder.ExitVolumeName  = TrkDetFlags.MuonSystemContainerName()
MuonTrackingGeometryBuilder.OutputLevel     = TrkDetFlags.MuonBuildingOutputLevel()
ToolSvc += MuonTrackingGeometryBuilder

#print MuonTrackingGeometryBuilder
