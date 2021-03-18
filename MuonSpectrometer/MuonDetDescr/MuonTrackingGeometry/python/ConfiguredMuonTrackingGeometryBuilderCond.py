# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#####################################################
# ConfiguredMuonTrackingGeometry module
#
# it inherits from MuonTrackingGeometryBuilder and performs 
# standard configuration
#
######################################################

# import the configurable
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonTrackingGeometryBuilderCond

# define the class
class ConfiguredMuonTrackingGeometryBuilderCond( Muon__MuonTrackingGeometryBuilderCond ):
    # constructor
    def __init__(self,name = 'MuonTrackingGeometryBuilderCond'):
 
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

        # import the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir() :
            ToolSvc = ToolSvc()
        
        from MuonRecExample import MuonAlignConfig  # noqa: F401 (import side-effects)

        from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationTypeBuilder
        MuonStationTypeBuilder= Muon__MuonStationTypeBuilder(name = 'MuonStationTypeBuilder')

        ToolSvc += MuonStationTypeBuilder

        # muon active/passive geometry setup
        from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationBuilderCond
        MuonStationBuilder= Muon__MuonStationBuilderCond(name = 'MuonStationBuilderCond' )
        ToolSvc += MuonStationBuilder

        from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonInertMaterialBuilderCond
        MuonInertMaterialBuilder= Muon__MuonInertMaterialBuilderCond(name = 'MuonInertMaterialBuilderCond')
        ToolSvc += MuonInertMaterialBuilder

        Muon__MuonTrackingGeometryBuilderCond.__init__(self,name,
                                                       EntryVolumeName=TrkDetFlags.MuonSystemEntryVolumeName(),
                                                       ExitVolumeName  = TrkDetFlags.MuonSystemContainerName())
        
