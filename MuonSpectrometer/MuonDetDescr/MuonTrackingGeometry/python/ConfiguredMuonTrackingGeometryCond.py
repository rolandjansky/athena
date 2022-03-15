# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AppMgr import ToolSvc

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

from MuonRecExample import MuonAlignConfig  # noqa: F401 (import side-effects)


from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationTypeBuilder
MuonStationTypeBuilder= Muon__MuonStationTypeBuilder(name = 'MuonStationTypeBuilder')
ToolSvc += MuonStationTypeBuilder

# muon active/passive geometry setup
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationBuilderCond
MuonStationBuilderCond = Muon__MuonStationBuilderCond(name = 'MuonStationBuilderCond')
MuonStationBuilderCond.StationTypeBuilder = MuonStationTypeBuilder
ToolSvc += MuonStationBuilderCond

from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonInertMaterialBuilderCond
MuonInertMaterialBuilderCond= Muon__MuonInertMaterialBuilderCond(name = 'MuonInertMaterialBuilderCond')
ToolSvc += MuonInertMaterialBuilderCond

# muon tracking geometry builder
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonTrackingGeometryBuilderCond
MuonTrackingGeometryBuilderCond= Muon__MuonTrackingGeometryBuilderCond(name = 'MuonTrackingGeometryBuilderCond')
MuonTrackingGeometryBuilderCond.EntryVolumeName = TrkDetFlags.MuonSystemEntryVolumeName()
MuonTrackingGeometryBuilderCond.ExitVolumeName  = TrkDetFlags.MuonSystemContainerName()
MuonTrackingGeometryBuilderCond.OutputLevel     = TrkDetFlags.MuonBuildingOutputLevel()
ToolSvc += MuonTrackingGeometryBuilderCond

from MuonRecExample.MuonStandalone import MuonStationsInterSectAlg 
MuonStationsInterSectAlg()
      