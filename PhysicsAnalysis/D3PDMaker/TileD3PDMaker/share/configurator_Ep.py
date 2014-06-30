# -*- coding: iso-8859-1 -*-
#
# File: configurator.py
# Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
# Date: June 2013 
#

cellsname = "AODCellContainer" if input_format=="AOD" else "AllCalo"

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

######################
### MAGNETIC FIELD ###
######################
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)

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

##############################
### PRE-DEFINED JOBOPTIONS ###
##############################
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "BFieldAth/BFieldAth_jobOptions.py" )
include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
from __main__ import AtlasTrackingGeometrySvc 

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

# LUMIBLOCK TOOLS #
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.OutputLevel = FATAL

from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuTool
ToolSvc += LumiBlockMuTool("LumiBlockMuTool")

from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool("LumiBlockMetaDataTool",calcLumi=True,storeXMLFiles=False)
LumiBlockMetaDataTool.applyDQCuts = True
LumiBlockMetaDataTool.OutputLevel = FATAL

# TRIGGER DECISION TOOL #
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc += tdt
ToolSvc.TrigDecisionTool.OutputLevel=ERROR
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
ToolSvc.TrigDecisionTool.PublicChainGroups = {}
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList=['ds']

# ATLAS EXTRAPOLATOR #
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator(name = 'egammaExtrapolator')
theAtlasExtrapolator.DoCaloDynamic = False 

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
ToolSvc += AtlasMaterialEffectsUpdator 
NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
NoElossMaterialEffectsUpdator.EnergyLoss = False
ToolSvc += NoElossMaterialEffectsUpdator

MyUpdators = []
MyUpdators += [NoElossMaterialEffectsUpdator] # FOR INNER DETECTOR
MyUpdators += [NoElossMaterialEffectsUpdator] # FOR CALORIMETER
MyUpdators += [NoElossMaterialEffectsUpdator] # FOR MUON SPECTROMETER

MySubUpdators = []
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # FOR INNER DETECTOR
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # FOR CALORIMETER
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # FOR MUON SPECTROMETER

theAtlasExtrapolator.MaterialEffectsUpdators = MyUpdators
theAtlasExtrapolator.SubMEUpdators = MySubUpdators
ToolSvc+=theAtlasExtrapolator

# EXTRAPOLATE TO CALORIMETER
from TileD3PDMaker.TileD3PDMakerConf import Extrapolator
TrackToCaloTool = Extrapolator(name = "theTrackToCalo",ExtrapolatorHandle = theAtlasExtrapolator)
ToolSvc += TrackToCaloTool

# TRACK IN CALORIMETER TOOLS
include("TrackInCaloTools/TrackInCaloTools_jobOptions.py")
include("CaloTrkMuIdTools/CaloTrkMuIdTools_jobOptions.py")

from TileD3PDMaker.TileD3PDMakerConf import TrackTools
TrackInCaloTool = TrackTools()
TrackInCaloTool.CaloCellContainer = "AODCellContainer" if input_format=="AOD" else "AllCalo"
#TrackInCaloTool.ExtrapolateToCaloTool = TrackToCaloTool
ToolSvc += TrackInCaloTool

# VERTEXING TOOL
from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
TrackToVertexTool = Reco__TrackToVertex(name="TrackToVertexTool")
ToolSvc+=TrackToVertexTool

beamFlags.Beam.beamType.set_Value_and_Lock(beam_type)

# ISOLATION TOOL
from TrackIsolationTools.TrackIsolationToolsConf import TrackIsolationTool
NewTrackIsolationTool = TrackIsolationTool()
ToolSvc += NewTrackIsolationTool

print "==============> TOOL ATTRIBUTES SET <=============="

##################
### D3PD Stuff ###
##################

# I'm not sure we need this stuff here anymore
#cellsname = "SelectedCells" if filter_cells else ( "AODCellContainer" if input_format=="AOD" else "AllCalo" )
#tracksname = "SelectedTracks" if filter_tracks else "TrackParticleCandidate" 
clustersname =  "CaloCalTopoCluster"

from AthenaCommon.AlgSequence import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import D3PDObject
algSequence = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )

import TileD3PDMaker
from TileD3PDMaker import TileD3PDObject
# TRACKS
#from TileD3PDObject import make_Track_D3PDVectorFiller
from TileD3PDMaker import TileTrackFillerTool
from TileD3PDMaker.TileD3PDMakerConf import TileTrackFilterAlg

#Define track filter
filteredTracksName = "SelectedTracks"
inputTracksName  = "TrackParticleCandidate"
tileTrackFilter                            = TileTrackFilterAlg(filteredTracksName)
tileTrackFilter.InputTracksName            = inputTracksName
tileTrackFilter.OutputTracksName           = filteredTracksName
tileTrackFilter.cut_p                      = cutP
tileTrackFilter.cut_eta                    = cutEta
tileTrackFilter.cut_sizeofcone             = cutSizeOfCone
tileTrackFilter.track_iso_tool             = NewTrackIsolationTool

#Define track filler
TileTrackFillerTool.LevelOfDetails        = levelOfDetails
TileTrackFillerTool.ExtrapolateInCaloTool = TrackInCaloTool
TileTrackFillerTool.CellsName             = cellsname
TileTrackFillerTool.track_iso_tool        = NewTrackIsolationTool
TileTrackFillerTool.TrackToVertexTool     = TrackToVertexTool

#Tell D3PDMaker to use filter and filler
TileTrackD3PDObject = D3PDObject(TileD3PDMaker.TileD3PDObject.make_Track_D3PDVectorFiller, 'tracks_' , 'TracksD3PDObject')
TileTrackD3PDObject.defineBlock(0, 'TrackParticleDump', TileD3PDMaker.TileTrackFillerTool)
algSequence += tileTrackFilter

# CELLS
#from TileD3PDObject import make_Cell_D3PDVectorFiller
from TileD3PDMaker.TileD3PDMakerConf import TileCellFilterAlg
from TileD3PDMaker import TileCellFillerTool
    
#Define cell filter
outputCellsName = "SelectedCells"
inputCellsName  = "AODCellContainer" if input_format=="AOD" else "AllCalo"
tileCellFilter                            = TileCellFilterAlg(outputCellsName)
tileCellFilter.IncludeLAr                 = "yes" if include_lar else "no"
tileCellFilter.InputContainer             = filteredTracksName
tileCellFilter.CellsName                  = inputCellsName
tileCellFilter.OutputCellsName            = outputCellsName
tileCellFilter.ExtrapolateInCalo          = TrackInCaloTool
tileCellFilter.FilterConfiguration        = filterConfig
tileCellFilter.ConeSize                   = cutSizeOfCone

#Defile cell filler
TileCellFillerTool.ExtrapolateInCaloTool  = TrackInCaloTool
TileCellFillerTool.LevelOfDetails         = 2
TileCellFillerTool.FilterConfiguration    = filterConfig

#Tell D3PDMaker to use filter and filler
TileCellD3PDObject = D3PDObject(TileD3PDMaker.TileD3PDObject.make_Cell_D3PDVectorFiller, 'cells_' , 'CellsD3PDObject')
TileCellD3PDObject.defineBlock(0, 'CaloCellDump', TileD3PDMaker.TileCellFillerTool)
algSequence += tileCellFilter

# CLUSTERS
#from TileD3PDObject import make_Cluster_D3PDVectorFiller
from TileD3PDMaker import TileCaloClusterFillerTool

#Defile cluster filler
TileCaloClusterFillerTool.CaloClustersName      = clustersname
TileCaloClusterFillerTool.LevelOfDetails        = levelOfDetails
TileCaloClusterFillerTool.ExtrapolateInCaloTool = TrackInCaloTool

#Tell D3PDMaker to user filler
TileClusterD3PDObject = D3PDObject(TileD3PDMaker.TileD3PDObject.make_Cluster_D3PDVectorFiller, 'clusters_' , 'ClustersD3PDObject')
TileClusterD3PDObject.defineBlock(0, 'CaloClusterDump', TileD3PDMaker.TileCaloClusterFillerTool)

# EVENT
from TileD3PDMaker import TileEventFillerTool

#Defile cluster filler
TileEventFillerTool.TrigDecisionTool      = tdt
TileEventFillerTool.LevelOfDetails        = 1

#Tell D3PDMaker to user filler
from D3PDMakerCoreComps.D3PDObject                      import make_SG_D3PDObject
TileEventD3PDObject = make_SG_D3PDObject( "EventInfo", D3PDMakerFlags.EventInfoSGKey(), 'evt_', "TileEventD3PDObject" )
TileEventD3PDObject.defineBlock(0, 'EventDump', TileD3PDMaker.TileEventFillerTool)


# Setup association tools: so that we can find out what is connected to what
from D3PDMakerCoreComps.IndexMultiAssociation           import IndexMultiAssociation

from TileD3PDMaker import TileClusterCellAssociationTool
TileClusterCellAssociationTool.CellsName             = cellsname
TileClusterCellAssociationTool.ExtrapolateInCaloTool = TrackInCaloTool

IndexMultiAssociation(parent=TileClusterD3PDObject,
                      assoctool=TileD3PDMaker.TileClusterCellAssociationTool,
                      target="cells_",
                      prefix="clustercells_",
                      level=0,
                      blockname="AssocClusterToCell")

from TileD3PDMaker import TileTrackCellAssociationTool
TileTrackCellAssociationTool.CellsName             = cellsname 
TileTrackCellAssociationTool.ExtrapolateInCaloTool = TrackInCaloTool 

IndexMultiAssociation(parent=TileTrackD3PDObject,
                      assoctool=TileD3PDMaker.TileTrackCellAssociationTool,
                      target="cells_",
                      prefix="trackcells_",
                      level=0,
                      blockname="AssocTrackToCell")

from TileD3PDMaker import TileTrackClusterAssociationTool
TileTrackClusterAssociationTool.ClustersName          = clustersname
TileTrackClusterAssociationTool.cut_sizeofcone        = cutSizeOfCone
TileTrackClusterAssociationTool.ExtrapolateInCaloTool = TrackInCaloTool

IndexMultiAssociation(parent=TileTrackD3PDObject,
                      assoctool=TileD3PDMaker.TileTrackClusterAssociationTool,
                      target="clusters_",
                      prefix="trackclusters_",
                      level=0,
                      blockname="AssocTrackToCluster")


# Finally create an output stream and specify what is dumped

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream( 'tileD3PD', outputFile )
alg += TileTrackD3PDObject(10)
alg += TileCellD3PDObject(10)
alg += TileClusterD3PDObject(10)
alg += TileEventD3PDObject(10)

print "==============> D3PD ATTRIBUTES SET <=============="

###################
### OTHER STUFF ###
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
