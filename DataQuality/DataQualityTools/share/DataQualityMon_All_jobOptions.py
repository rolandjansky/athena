# ****************************************
# Application configuration for monitoring
# ****************************************

doAlign=True
doMet=True
doJet=True
doEgamma=True
doPixel=False
doTile=False



## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "PrimaryManager" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = OutputLevel

svcMgr.MessageSvc.OutputLevel = WARNING

# ************************
# Monitoring configuration
# ************************

if not hasattr(svcMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

## Setup the output file(s):
THistSvc = Service( "THistSvc" )

## The string "TestMon" in the argument below is the 'FileKey'
## used by Athena to access the output file internally
THistSvc.Output += ["MonHist DATAFILE='Monitor.root' OPT='RECREATE'"]
 

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
AtlasExtrapolator = AtlasExtrapolator()
ToolSvc += AtlasExtrapolator

from AthenaCommon.AppMgr import ToolSvc as toolSvc

## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
monMan = Algorithm( "PrimaryManager" )

## FileKey must match that given to THistSvc
monMan.FileKey = "MonHist"

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True
monMan.DataType            = "userDefined"
monMan.Environment         = "user"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

#------------------#
# MET monitoring   #
#------------------#
if (doMet):
    monMan.AthenaMonTools += [ "METMonTool/METMonTool" ]
    toolSvc.METMonTool.ProcessNEvents = 1000
    toolSvc.METMonTool.PreScale       = 0
    toolSvc.METMonTool.jetColKey = "Cone4TowerJets"

#------------------#
# Jet monitoring   #
#------------------#
if (doJet):
    include("DataQualityTools/JetMonitoring.py")


#-------------------#
# egamma monitoring #
#-------------------#
if (doEgamma):
    monMan.AthenaMonTools += [ "egammaMonTool/egMonTool" ]
## set the monitored containers
    toolSvc.egMonTool.ElectronContainer 	= "ElectronCollection"
    toolSvc.egMonTool.PhotonContainer 	= "PhotonCollection"
    toolSvc.egMonTool.EMTrackMatchContainer = "egDetailContainer"
    toolSvc.egMonTool.EMShowerContainer 	= "egDetailContainer"
    toolSvc.egMonTool.EMConvertContainer 	= "egDetailContainer"

#----------------------#
# Alignment Monitoring #
#----------------------#
if (doAlign):
    monMan.AthenaMonTools += [ "IDAlignMonGenericTracks"]
    monMan.AthenaMonTools += [ "IDAlignMonZmumu"]
    monMan.AthenaMonTools += [ "IDAlignMonEfficiencies"]
    monMan.AthenaMonTools += [ "IDAlignMonTruthComparison"]
    monMan.AthenaMonTools += [ "IDAlignMonResiduals"]
    #monMan.AthenaMonTools += [ "IDAlignMonCosmics"]
    #monMan.AthenaMonTools += [ "IDAlignMonElectrons"]
    #monMan.AthenaMonTools += [ "IDAlignMonJpsiUpsilon"]

if (doTile):    
    monMan.AthenaMonTools += [ "TileMonitoring" ]
    
#------------------#
# pixel monitoring #
#------------------#
if (doPixel):
#    monMan.AthenaMonTools += [ "PixelSpacePointsMon" ]
    monMan.AthenaMonTools += [ "PixelRDOMon" ]
    monMan.AthenaMonTools += [ "PixelClusterMon" ]
    monMan.AthenaMonTools += [ "PixelTrackMon" ]
    monMan.AthenaMonTools += [ "PixelTimingMon" ]
#    toolSvc.PixelSpacePointsMon.Pixel_SpacePointContainerName = "PixelSpacePoints"
    toolSvc.PixelRDOMon.RDOName = "PixelRDOs"
    toolSvc.PixelRDOMon.doA = True
    toolSvc.PixelRDOMon.doB = True
    toolSvc.PixelRDOMon.doC = True
    toolSvc.PixelClusterMon.Pixel_SiClusterContainerName = "PixelClusters"
#    toolSvc.TrackTool.TrackName = "SCT_Cosmic_Tracks"
    toolSvc.PixelTrackMon.TrackName = "Tracks"
    toolSvc.PixelTimingMon.RDOName = "PixelRDOs"
    toolSvc.PixelTimingMon.EvtMax = 15000   #number of events to show in the time profile
    toolSvc.PixelTimingMon.TimeStep = 100   #number of events per bin



