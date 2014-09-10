
# muon active/passive geometry setup
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonStationBuilder
MuonStationBuilder= Muon__MuonStationBuilder(name = 'MuonStationBuilder',
                                             BuildBarrelStations = True,     
                                             BuildEndcapStations = True,
                                             BuildCSCStations = True,
                                             BuildTGCStations = True,
                                             ResolveActiveLayers = True )
ToolSvc += MuonStationBuilder

from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonInertMaterialBuilder
MuonInertMaterialBuilder= Muon__MuonInertMaterialBuilder(name = 'MuonInertMaterialBuilder',
                                                         SimplifyGeometry = False,
                                                         SimplifyGeometryToLayers = False )
ToolSvc += MuonInertMaterialBuilder 

# muon tracking geometry builder
from MuonTrackingGeometry.MuonTrackingGeometryConf import Muon__MuonTrackingGeometryBuilder
MuonTrackingGeometryBuilder= Muon__MuonTrackingGeometryBuilder(name = 'MuonTrackingGeometryBuilder')

ToolSvc += MuonTrackingGeometryBuilder

print MuonTrackingGeometryBuilder
