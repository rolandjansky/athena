from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

include ("AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py")

EventSelector = Service("EventSelector") 

if not 'inputFile'  in dir() : 
 inputFile="cosmic.reco.pool.root"

EventSelector.InputCollections = [inputFile]

# Calo DetDescr
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )



theApp.Dlls +=["LArAthenaPoolPoolCnv"] 
theApp.Dlls +=["TileEventAthenaPoolPoolCnv"] 
theApp.Dlls +=["GeneratorObjectsAthenaPoolPoolCnv"] 
theApp.Dlls +=["TruthExamples"] 

# add user Dlls and TopAlg.
# theApp.Dlls +=["XYZ"] 
# theApp.TopAlg = ["UVW"] 

# CBNT 
# jamie boyd - 1/6/2012 - removing all reference to CBNT - but not testing!
#include( "CBNT_Athena/CBNT_Athena_jobOptions.py" )
#include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
#include( "RecExCommon/CBNT_Truth_jobOptions.py" )
#include ("CaloRec/EMTopoCluster_CBNT_jobOptions.py" )
#include ("CaloRec/CaloTopoCluster_CBNT_jobOptions.py" )

#CBNT_Athena.NtupleLocID="/FILE1/CBNT/cosmic"

# include the cells in CaloCluster.
#CBNT_EMClusterTopo.AddCells = True
#CBNT_EMClusterTopo.AddCellDetails = True

#CBNT_CaloClusterTopo.AddCells = True
#CBNT_CaloClusterTopo.AddCellDetails = True

theApp.EvtMax = 100000
MessageSvc.OutputLevel = INFO

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "cosmic.ana.hist.root" 

NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='cosmic.ana.ntuple.root' OPT='NEW'"]

