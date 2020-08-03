# DetDescrVersion="Rome-Final"
DetDescrVersion="ATLAS-DC3-02"

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
GlobalFlags.InputFormat.set_pool()
GlobalFlags.DetGeo.set_atlas()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

# input 
include ("AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py")
EventSelector = Service("EventSelector") 
PoolSvc = Service("PoolSvc") 

if not 'inputFile'  in dir() : 
  inputFile="cosmic.digit.pool.root"

if not 'outputFile'  in dir() : 
  outputFile="cosmic.reco.pool.root"

print " input file = ", inputFile 
print " output file = ", outputFile 
EventSelector.InputCollections = [inputFile] 

# Calo DetDescr
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "LArCondCnv/LArCondCnv_G4_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

# CaloCell 
include( "CaloRec/CaloCellMaker_config.py" )
include( "CaloRec/CaloCellMaker_jobOptions.py" )
include( "CaloRec/CaloTopoCluster_jobOptions.py" )
include( "CaloRec/EMTopoCluster_jobOptions.py" )

EMTopoClusterMaker.EMTopoCluster.ClusterEtorAbsEtCut            = .1*GeV
CaloTopoClusterMaker.TopoCluster.CalorimeterNames=[ "TILE" ]


# slim cells 
theApp.TopAlg +=["CaloClusterCellSlimmer/slimmer"]
slimmer = Algorithm("slimmer")
slimmer.ClusterContainerNames = ["EMTopoCluster","CaloTopoCluster"]
slimmer.CellContainerName = "ClusterOnly" 


theApp.Dlls +=["LArAthenaPoolPoolCnv"] 
theApp.Dlls +=["TileEventAthenaPoolPoolCnv"] 
theApp.Dlls +=["GeneratorObjectsAthenaPoolPoolCnv"] 
theApp.Dlls +=["G4SimAthenaPOOLPoolCnv"] 
theApp.Dlls +=["RecAthenaPoolPoolCnv"] 
theApp.Dlls +=["TruthExamples"] 
theApp.Dlls +=["LArRawUtils", "LArROD"] 

#theApp.TopAlg += ["DumpMC"] 
#DumpMC = Algorithm("DumpMC") 
#DumpMC.MCEventKey = "TruthEvent" 

# jamie boyd - 1/6/2012 remove all reference to CBNT without testing
#CBNT_Athena = Algorithm( "CBNT_Athena" )

doCaloMuId=True 
doLAr=True
include ("LArMuId/LArMuId_jobOptions.py")
LArMuIdAlgorithm.StartFromTracks = False 
LArMuIdAlgorithm.OutputLevel = DEBUG 

include ("TileMuId/TileMuId_cosmics_jobOptions.py")
#CBNT_Athena.Members += [ "CBNT_TileMu" ]

theApp.EvtMax = 1000
MessageSvc.OutputLevel = INFO
MessageSvc.infoLimit  = 10000000 
MessageSvc.debugLimit  = 10000000 
# EventSelector.OutputLevel = DEBUG 

# CBNT 
#include( "CBNT_Athena/CBNT_Athena_jobOptions.py" )
#include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
#include( "RecExCommon/CBNT_Truth_jobOptions.py" )
# include ("CaloRec/CaloCell_CBNT_jobOptions.py" )
#include ("CaloRec/EMTopoCluster_CBNT_jobOptions.py" )
#include ("CaloRec/CaloTopoCluster_CBNT_jobOptions.py" )
#include ("CommissionUtils/CBNT_TTR_jobOptions.py")

#CBNT_Athena.NtupleLocID="/FILE1/CBNT/cosmic"

# include the cells in CaloCluster.
#CBNT_EMClusterTopo.AddCells = True
#CBNT_EMClusterTopo.AddCellDetails = True

#CBNT_CaloClusterTopo.AddCells = True
#CBNT_CaloClusterTopo.AddCellDetails = True

NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='cosmic_reco_ntuple.root' OPT='NEW'"]
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"

theApp.OutStream += [ "Stream1" ]
theApp.OutStreamType = "AthenaOutputStream"
Stream1 = Algorithm( "Stream1" )
Stream1.EvtConversionSvc = "AthenaPoolCnvSvc"
# allows objects to be copied in output even if not read in
Stream1 = Algorithm( "Stream1" )
Stream1.ItemList +=["EventInfo#*"] 

Stream1.ItemList +=["McEventCollection#*"] 
Stream1.ItemList +=["CaloCellContainer#ClusterOnly"] 

Stream1.ItemList +=["CaloCellLinkContainer#CaloTopoCluster_Slimmed_Link" ]
Stream1.ItemList +=["CaloCellLinkContainer#EMTopoCluster_Slimmed_Link" ]
Stream1.ItemList +=["CaloCellLinkContainer#LArMuClusterCandidates_Link" ]


Stream1.ItemList +=["CaloShowerContainer#CaloTopoCluster_Slimmed_Data" ]
Stream1.ItemList +=["CaloShowerContainer#EMTopoCluster_Slimmed_Data" ]
Stream1.ItemList +=["CaloShowerContainer#LArMuClusterCandidates_Data" ]

Stream1.ItemList +=["INavigable4MomentumCollection#EMTopoCluster_Slimmed"]
Stream1.ItemList +=["INavigable4MomentumCollection#CaloTopoCluster_Slimmed"]

Stream1.ItemList +=["CaloClusterContainer#EMTopoCluster_Slimmed"] 
Stream1.ItemList +=["CaloClusterContainer#CaloTopoCluster_Slimmed"] 

Stream1.ItemList +=["TileMuContainer#*"] 

Stream1.ItemList += ["TimedTrackRecordCollection#*"]
Stream1.OutputFile = outputFile
