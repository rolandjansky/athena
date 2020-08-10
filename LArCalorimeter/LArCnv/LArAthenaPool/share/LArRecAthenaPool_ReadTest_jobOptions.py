###############################################################
#
# Job options file
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags

DetFlags.detdescr.all_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()
DetFlags.makeRIO.LAr_setOn()
DetFlags.makeRIO.Tile_setOn()

doCaloNoise=False;
readG3= False

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()

if readG3:
  GlobalFlags.DataSource.set_geant3()
else: 
  GlobalFlags.DataSource.set_geant4()

GlobalFlags.InputFormat.set_pool()
GlobalFlags.Luminosity.set_zero()


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )


include ("RecExCommon/AllDet_detDescr.py")


#include( "TileDetMgrDetDescrCnv/TileDetMgrDetDescrCnv_jobOptions.py" )
#include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )



#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 200
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 2
#ApplicationMgr.Dlls   += { "LArClusterRec" };
#ApplicationMgr.Dlls   += { "LArCellRec" };

theApp.Dlls += [ "CaloTools" ]
theApp.Dlls += [ "CaloRec" ]

from CaloIdentifier import SUBCALO

include( "CBNT_Athena/CBNT_Athena_jobOptions.py" )
theApp.TopAlg += [ "CBNT_Athena/CBNT_Calo" ]
CBNT_Calo = Algorithm( "CBNT_Calo" )
CBNT_Calo.NtupleLocID="/FILE1/CALO/168"

CBNT_Calo.Members  += [ "CBNT_CaloCell/CBNT_LArCell" ]
CBNT_LArCell = Algorithm( "CBNT_LArCell" )
CBNT_LArCell.CaloNums=[SUBCALO.LAREM, SUBCALO.LARHEC,SUBCALO.LARFCAL]

CBNT_Calo.Members += [  "CBNT_CaloCluster/CBNT_LArCluster" ]
CBNT_LArCluster = Algorithm( "CBNT_LArCluster" )
CBNT_LArCluster.ClusterColl="LArClusterEM"

# include( "LArClusterRec/CBNT_LArCluster_jobOptions.py" )
# include( "LArClusterRec/CBNT_LArCluster_MultiSize_jobOptions.py" )

theApp.Dlls                 += [ "LArAthenaPoolPoolCnv" ]
theApp.Dlls                 += [ "TileEventAthenaPoolPoolCnv" ]
EventSelector = Service( "EventSelector" )
EventSelector.InputCollection        = "LArRecTest.root"
PoolSvc = Service( "PoolSvc" )
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump=False
#  Ntuple output
theApp.Dlls   += [ "RootHistCnv" ]
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='larcluster_read.root' OPT='NEW'"]
theApp.HistogramPersistency="ROOT"
# Histogram output, if any.
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.root"

