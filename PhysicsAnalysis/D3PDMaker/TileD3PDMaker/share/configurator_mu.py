# -*- coding: iso-8859-1 -*-
#
# File: configurator.py
# Author: Marco van Woerden <mvanwoer@cern.ch>
# Date: November 2012
#

cellsname = "AODCellContainer" if input_format=="AOD" else "AllCalo"

####################
### GLOBAL FLAGS ###
####################
from AthenaCommon.GlobalFlags import globalflags as globalFlags
globalFlags.DetGeo                              .set_Value_and_Lock("atlas")                    # atlas, ctbh6, ctbh8, ctb
globalFlags.DataSource                          .set_Value_and_Lock(input_type)         # data source: data, geant3, geant4

from AthenaCommon.BeamFlags import jobproperties as beamFlags
beamFlags.Beam.beamType                         .set_Value_and_Lock(beam_type)

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
ToolSvc.TrigDecisionTool.PublicChainGroups = { "EF": "EF_.*", "L1" : "L1_.*"}
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
ToolSvc += TrackInCaloTool

# VERTEXING TOOL
from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
TrackToVertexTool = Reco__TrackToVertex(name="TrackToVertexTool")
ToolSvc+=TrackToVertexTool

beamFlags.Beam.beamType.set_Value_and_Lock(beam_type)

##############
# ALGORITHMS #
##############

# DEFINE ALGORITHM
def make_cell_filter():

    # PRINTOUT FOR DEBUGGING PURPOSES
    outputCellsName = "SelectedCells"
    inputCellsName  = "AODCellContainer" if input_format=="AOD" else "AllCalo"

    # LOCALLY IMPORT, SET FILTER DETAILS
    from TileD3PDMaker.TileD3PDMakerConf import TileCellFilterAlg
    tileCellFilter                            = TileCellFilterAlg(outputCellsName)
    tileCellFilter.IncludeLAr = "yes" if include_lar else "no"
    tileCellFilter.CellsName                  = inputCellsName
    tileCellFilter.OutputCellsName            = outputCellsName
    tileCellFilter.ExtrapolateInCalo          = TrackInCaloTool
    #tileCellFilter.ExtrapolateToCaloTool      = TrackToCaloTool
    tileCellFilter.TrackType                  = muon_type
    tileCellFilter.FilterConfiguration        = filterConfig

    return tileCellFilter

import D3PDMakerCoreComps
from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig

# TileD3PDMaker CONFIG
import TileD3PDMaker.TileD3PDObject
from TileD3PDMaker.TileD3PDObject          import *

# TRACKS
TileTrackD3PDObject = D3PDObject(make_Track_D3PDVectorFiller, 'tracks_' , 'TracksD3PDObject')
TileTrackD3PDObject.defineBlock(0, 'TrackParticleDump', TileD3PDMaker.TileTrackFillerTool)

# RETRIEVE D3PD OBJECT CONTAINING SELECTED CALOCELLS
TileCellD3PDObject = D3PDObject(make_Cell_D3PDVectorFiller, 'cells_' , 'CellsD3PDObject')
TileCellD3PDObject.defineBlock(0, 'CaloCellDump', TileD3PDMaker.TileCellFillerTool)

TileMBTSD3PDObject = D3PDObject(make_MBTS_D3PDVectorFiller, 'mbts_' , 'MBTSD3PDObject')
TileMBTSD3PDObject.defineBlock(0, 'MBTSDump', TileD3PDMaker.TileMBTSFillerTool)

# EVENT BY EVENT INFORMATION TO STORE 
TileEventD3PDObject = make_SG_D3PDObject( "EventInfo", D3PDMakerFlags.EventInfoSGKey(), 'evt_', "TileEventD3PDObject" )
TileEventD3PDObject.defineBlock(0, 'EventDump', TileD3PDMaker.TileEventFillerTool)

# ASSOCIATIONS
IndexMultiAssociation(parent=TileClusterD3PDObject,assoctool=TileD3PDMaker.TileClusterCellAssociationTool,target="cells_",prefix="clustercells_",level=0,blockname="AssocClusterToCell")
IndexMultiAssociation(parent=TileTrackD3PDObject,assoctool=TileD3PDMaker.TileTrackCellAssociationTool,target="cells_",prefix="trackcells_",level=0,blockname="AssocTrackToCell")

# OTHER CONFIG
import TileD3PDMaker
from TileD3PDMaker.TileD3PDObject          import *
from TileD3PDMaker                                     import *
from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject

# PERFORMANCE MONITORING
if monitor_performance:
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doMonitoring = True
    jobproperties.PerfMonFlags.doFullMon = True
    jobproperties.PerfMonFlags.doPostProcessing = True

from AthenaCommon.AlgSequence import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

## DEFINE FILTER SETTINGS ##
def cell_filterHook(c, **kw):
    cell_filter = make_cell_filter()
    preseq = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
    preseq += cell_filter
    return
    
if filter_cells:
    TileD3PDMaker.TileD3PDObject.TileCellD3PDObject.defineHook(cell_filterHook)
    
## SET ALL ALGORITHM PROPERTIES
from TileD3PDMaker.TileD3PD import mu_D3PD
cellsname = "SelectedCells" if filter_cells else ( "AODCellContainer" if input_format=="AOD" else "AllCalo" )

truthinfo = "GEN_AOD" if input_format=="AOD" else "TruthInfo"

TileCellFillerTool.ExtrapolateInCaloTool  = TrackInCaloTool
TileCellFillerTool.LevelOfDetails         = levelOfDetails
TileCellFillerTool.TrackType              = muon_type
TileMBTSFillerTool.ExtrapolateInCaloTool  = TrackInCaloTool
TileMBTSFillerTool.LevelOfDetails         = levelOfDetails
TileMBTSFillerTool.TrackType              = muon_type
TileMuonFillerTool.ExtrapolateInCaloTool  = TrackInCaloTool
TileMuonFillerTool.ExtrapolateToCaloTool  = TrackToCaloTool
TileMuonFillerTool.TrackToVertexTool      = TrackToVertexTool
TileMuonFillerTool.LevelOfDetails         = levelOfDetails
TileMuonFillerTool.TruthInfo              = truthinfo
TileMuonFillerTool.TrackType              = muon_type
TileMuonFillerTool.CellsName              = cellsname
TileEventFillerTool.TrigDecisionTool      = tdt
TileEventFillerTool.LevelOfDetails        = levelOfDetails
TileMuCellAssociationTool.CellsName       = cellsname
TileMuCellAssociationTool.TrackType       = muon_type
TileMuCellAssociationTool.ExtrapolateInCaloTool = TrackInCaloTool
TileCellMuAssociationTool.TrackType       = muon_type
TileCellMuAssociationTool.ExtrapolateInCaloTool = TrackInCaloTool
alg = mu_D3PD(file = outputFile, tuplename = 'tileD3PD')

print "==============> GLOBAL ATTRIBUTES SET <=============="

## TRIGGER
alg += MuonTriggerBitsD3PDObject ( 2, 'MuonTriggerBits')
alg.MetadataTools += [ LBMetadataConfig() ]

### SET OUTPUT LEVEL OF MESSAGES ###
debugging = False

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()
if debugging: svcMgr.MessageSvc.OutputLevel = 1
else:         svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False
