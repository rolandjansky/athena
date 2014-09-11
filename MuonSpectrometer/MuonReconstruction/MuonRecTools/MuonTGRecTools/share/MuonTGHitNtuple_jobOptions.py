#
include("MuonTGRecTools/MuonTGRec_jobOptions.py") 

# load MuonHolesOnTrackTool 
from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonHolesOnTrackTool
MuonHolesOnTrackTool = Muon__MuonHolesOnTrackTool( name= 'MuonHolesOnTrackTool',
                                                   ExtrapolatorName = MuonExtrapolator,
                                                   TrackingGeometryName = 'MuonStandaloneTrackingGeometry') 
ToolSvc += MuonHolesOnTrackTool

#
from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonTGHitNtuple
MuonTGHitNtuple = Muon__MuonTGHitNtuple(  name = 'MuonTGHitNtuple'
                                          )
topSequence += MuonTGHitNtuple
print MuonTGHitNtuple

