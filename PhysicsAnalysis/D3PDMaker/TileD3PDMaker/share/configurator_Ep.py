# -*- coding: iso-8859-1 -*-
#
# File: configurator_muons.py
# Author: Marco van Woerden <mvanwoer@cern.ch>
# Author: Gang Qin <gang.qin@cern.ch>
# Author: Carlos Solans <carlos.solans@cern.ch>
# Author: Archil Durglishvili <archil.durglishvili@cern.ch>
# Date: June 2013 
# Reviewed: Nov 2014

####################
### GLOBAL FLAGS ###
####################
from AthenaCommon.GlobalFlags import globalflags as globalFlags
globalFlags.DetGeo				.set_Value_and_Lock("atlas")			# atlas, ctbh6, ctbh8, ctb
globalFlags.DataSource				.set_Value_and_Lock(input_type)		# data source: data, geant3, geant4

from AthenaCommon.BeamFlags import jobproperties as beamFlags
beamFlags.Beam.beamType				.set_Value_and_Lock(beam_type)

print "==============> GLOBAL FLAGS SET <=============="

#######################
### POOL CONVERTORS ###
#######################
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.EventSelector.InputCollections = inputFiles
ServiceMgr.EventSelector.SkipEvents = skipEvt

print "==============> POOL CONVERTORS SET <=============="

################
### DATABASE ###
################
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
DBReplicaSvc = DBReplicaSvc(UseCOOLSQLite=True)
ServiceMgr += DBReplicaSvc
ServiceMgr.PoolSvc.SortReplicas=True

#include ("TileGeoModel/TileGeoModel_jobOptions.py")

from RDBAccessSvc.RDBAccessSvcConf import RDBAccessSvc
include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py")
RDBAccessSvc = Service( "RDBAccessSvc" )
RDBAccessSvc.HostName = "atlas_dd"
theApp.Dlls     += ["RDBAccessSvc"]
theApp.ExtSvc   += ["RDBAccessSvc"]

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
theApp.EvtMax = EvtMax
athenaCommonFlags.FilesInput.set_Value_and_Lock(inputFiles)
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)

print "==============> DATABASE SET <=============="

####################################
### PHYSICS RECONSTRUCTION FLAGS ###
####################################
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(input_format=="ESD")
rec.readAOD.set_Value_and_Lock(input_format=="AOD")
from RecExConfig.InputFilePeeker import inputFileSummary
include("RecExCond/RecExCommon_flags.py")
include("RecExCond/AllDet_detDescr.py")

from AthenaCommon.AthenaCommonFlags import jobproperties as jp

print "==============> PHYSICS RECONSTRUCTION FLAGS SET <=============="

###########################
### CONDITIONS DATABASE ###
###########################
include( "IOVDbSvc/IOVRecExCommon.py" )
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalFlags.ConditionsTag())
if not conddb.folderRequested('/SCT/DCS/MODTEMP'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
if not conddb.folderRequested('/SCT/DCS/HV'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/HV")
if not conddb.folderRequested('/PIXEL/PixReco'):
    conddb.addFolder("PIXEL_OFL","/PIXEL/PixReco")
    
print "==============> ADDITIONAL DATABASE FOLDERS <=============="
############################
### DETECTOR DESCRIPTION ###
############################

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.Calo_setOn()
include("RecExCond/AllDet_detDescr.py")

# GEOMODEL IS NEEDED TO BUILD GEOMETRY
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
svcMgr.GeoModelSvc.DetectorTools["TileDetectorTool"].GeometryConfig = "FULL"
svcMgr.DetectorStore.Dump = False
include("RecExCond/AllDet_detDescr.py")

######################
### MAGNETIC FIELD ###                              
######################
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)


##############################
### PRE-DEFINED JOBOPTIONS ###
##############################
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
#############
### TOOLS ###
#############
# ALGORITHM SEQUENCE
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.EventPrintoutInterval = 200

from AthenaCommon.AlgSequence import AlgSequence
topSequencer = AlgSequence()

from AthenaCommon.AlgSequence import AthSequencer
Sequencer = AthSequencer("TileD3PDMaker")
Sequencer.StopOverride = False
topSequencer += Sequencer

# ATLAS EXTRAPOLATOR #
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
ToolSvc += AtlasExtrapolator()

# VERTEXING TOOL
from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
TrackToVertexTool = Reco__TrackToVertex(name="TrackToVertexTool")
ToolSvc+=TrackToVertexTool

# ISOLATION TOOL
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool()
ToolSvc += TrackIsoTool

# TRACK TOOLS
from TileD3PDMaker.TileD3PDMakerConf import TrackTools
TrackTools = TrackTools()
TrackTools.TrackIsolationTool = TrackIsoTool
ToolSvc += TrackTools

print "==============> TOOL ATTRIBUTES SET <=============="

##################
### D3PD Stuff ###
##################

from AthenaCommon.AlgSequence import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import D3PDObject
algSequence = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )

from D3PDMakerCoreComps import SGDataVectorGetterTool, VectorFillerTool
import D3PDMakerCoreComps
import TileD3PDMaker

# FILTERS #####################

from TileD3PDMaker.TileD3PDMakerConf import TileEopFilterAlg
tileEopFilter                   = TileEopFilterAlg("TileTrackFilter")
tileEopFilter.InputTracks       = "InDetTrackParticles"
tileEopFilter.OutputTracks      = "SelectedTracks"
tileEopFilter.InputClusters     = "CaloCalTopoCluster"
tileEopFilter.OutputClusters    = "SelectedClusters"
tileEopFilter.InputCells        = CellsContainer
tileEopFilter.OutputCells       = "SelectedCells"
tileEopFilter.TrackMomCut       = 2000
tileEopFilter.TrackEtaCut       = 2.
tileEopFilter.TrackIsoCut       = 0.4
tileEopFilter.TrackPtrelCut     = 0.15
tileEopFilter.TrackClusterR     = 0.6
tileEopFilter.TrackCellR        = 0.2
tileEopFilter.TrackTools        = TrackTools
tileEopFilter.DumpLArCells      = False
algSequence += tileEopFilter

# FILLERS #####################

# Event Filler
from TileD3PDMaker import TileEventFillerTool
TileEventFillerTool.LevelOfDetails    = 1

from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
TileEventD3PDObject = make_SG_D3PDObject( "xAOD::EventInfo_v1", D3PDMakerFlags.EventInfoSGKey(), 'evt_', "TileEventD3PDObject" )
TileEventD3PDObject.defineBlock(0, 'EventDump', TileD3PDMaker.TileEventFillerTool)

# Track Filler
from TileD3PDMaker import TileTrackFillerTool
TileTrackFillerTool.LevelOfDetails        = 1
TileTrackFillerTool.TrackTools            = TrackTools
TileTrackFillerTool.TrackParType          = 1 ;	 #track parameters per 0: sampling 1: layer
TileTrackFillerTool.TrackToVertexTool     = TrackToVertexTool

def tileTrackFiller(name, prefix, object_name):
    tileTrackGetter = SGDataVectorGetterTool('track_getter',
                                             TypeName = 'xAOD::TrackParticleContainer_v1',
                                             SGKey = 'SelectedTracks')
    return VectorFillerTool(name,
                            Prefix = prefix,
                            Getter = tileTrackGetter,
                            ObjectName = 'trackparticles',
                            SaveMetadata = D3PDMakerFlags.SaveObjectMetadata())
TileTrackD3PDObject = D3PDObject(tileTrackFiller, 'tracks_' , 'TracksD3PDObject')
TileTrackD3PDObject.defineBlock(0, 'TrackParticleDump', TileTrackFillerTool)

# Cluster Filler
from TileD3PDMaker import TileClusterFillerTool
def caloClusterFiller(name, prefix, object_name):
    caloClusterGetter = SGDataVectorGetterTool('cluster_getter',
                                               TypeName = 'xAOD::CaloClusterContainer_v1',
                                               SGKey = 'SelectedClusters')
    return VectorFillerTool(name,
                            Prefix = prefix,
                            Getter = caloClusterGetter,
                            ObjectName = 'caloclusters',
                            SaveMetadata = D3PDMakerFlags.SaveObjectMetadata())
TileClusterD3PDObject = D3PDObject(caloClusterFiller, 'clusters_' , 'ClustersD3PDObject')
TileClusterD3PDObject.defineBlock(0, 'CaloClusterDump', TileClusterFillerTool)

#Define cell filler
from TileD3PDMaker import TileCellFillerTool
TileCellFillerTool.TrackTools             = TrackTools
TileCellFillerTool.LevelOfDetails         = 2
def tileCellFiller(name, prefix, object_name):
    tilecell_getter = SGDataVectorGetterTool('cell_getter',
                                             TypeName = 'CaloCellContainer',
                                             SGKey = 'SelectedCells')
    return VectorFillerTool(name,
                            Prefix = prefix,
                            Getter = tilecell_getter,
                            ObjectName = 'calocells',
                            SaveMetadata = D3PDMakerFlags.SaveObjectMetadata())
TileCellD3PDObject = D3PDObject(tileCellFiller, 'cells_' , 'CellsD3PDObject')
TileCellD3PDObject.defineBlock(0, 'CaloCellDump', TileCellFillerTool)

# Index associations ###################################

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
from TileD3PDMaker import TileAssocFillerTool

def trackClusterFiller(name):
    return TileAssocFillerTool(name,
                                  ContainerName = 'TrackClusters',
                                  BranchName = '_index')
trackClusterD3PDObject = make_Void_D3PDObject( 'tracks_trackclusters', 'TrackClusterD3PDObject' )
trackClusterD3PDObject.defineBlock(0, 'TrackClusterDump', trackClusterFiller)

def trackCellFiller(name):
    return TileAssocFillerTool(name,
                                  ContainerName = 'TrackCells',
                                  BranchName = '_index')
trackCellD3PDObject = make_Void_D3PDObject( 'tracks_trackcells', 'TrackCellD3PDObject' )
trackCellD3PDObject.defineBlock(0, 'TrackCellDump', trackCellFiller)

def clusterCellFiller(name):
    return TileAssocFillerTool(name,
                                  ContainerName = "ClusterCells",
                                  BranchName = "_index")
clusterCellD3PDObject = make_Void_D3PDObject( 'clusters_clustercells', 'ClusterCellD3PDObject' )
clusterCellD3PDObject.defineBlock(0, 'ClusterCellDump', clusterCellFiller)

# Finally create an output stream and specify what is dumped

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream( 'tileD3PD', outputFile )
alg += TileEventD3PDObject(10)
alg += TileTrackD3PDObject(10)
alg += TileCellD3PDObject(10)
alg += TileClusterD3PDObject(10)
alg += trackClusterD3PDObject(10)
alg += trackCellD3PDObject(10)
alg += clusterCellD3PDObject(10)


print "==============> D3PD ATTRIBUTES SET <=============="

###################
###?OTHER STUFF ###
###################

## TRIGGER
#from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject
#alg += MuonTriggerBitsD3PDObject ( 2, 'MuonTriggerBits')
#from EventCommonD3PDMaker.LBMetadataConfig import LBMetadataConfig
#alg.MetadataTools += [ LBMetadataConfig() ]

# PERFORMANCE MONITORING
if monitor_performance:
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doMonitoring = True
    jobproperties.PerfMonFlags.doFullMon = True
    jobproperties.PerfMonFlags.doPostProcessing = True

# SET OUTPUT LEVEL OF MESSAGES
debugging = False

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()
if debugging: svcMgr.MessageSvc.OutputLevel = 1
else:         svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

print "==============> OTHER ATTRIBUTES SET <=============="
