# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#####################################################
# ConfiguredMuonTrackingGeometry module
#
# it inherits from MuonTrackingGeometryBuilder and performs 
# standard configuration
#
######################################################
# import the include statement
from AthenaCommon.Include import Include, IncludeError, include
 
# import the configurable
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonTrackingGeometryBuilder

# define the class
class ConfiguredMuonTrackingGeometryBuilder( Muon__MuonTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'MuonTrackingGeometryBuilder'):
 
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

        # import the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir() :
            ToolSvc = ToolSvc()

        from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationTypeBuilder
        MuonStationTypeBuilder= Muon__MuonStationTypeBuilder(name = 'MuonStationTypeBuilder')

        ToolSvc += MuonStationTypeBuilder

        # muon active/passive geometry setup
        from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationBuilder
        MuonStationBuilder= Muon__MuonStationBuilder(name = 'MuonStationBuilder' )
        ToolSvc += MuonStationBuilder

        from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonInertMaterialBuilder
        MuonInertMaterialBuilder= Muon__MuonInertMaterialBuilder(name = 'MuonInertMaterialBuilder')
        ToolSvc += MuonInertMaterialBuilder 

        Muon__MuonTrackingGeometryBuilder.__init__(self,name,\
                                                   EntryVolumeName=TrkDetFlags.MuonSystemEntryVolumeName(),\
                                                   ExitVolumeName  = TrkDetFlags.MuonSystemContainerName())
