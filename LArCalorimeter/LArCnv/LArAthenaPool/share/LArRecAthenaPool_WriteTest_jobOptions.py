from AthenaCommon.DetFlags import DetFlags

DetFlags.detdescr.all_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()
DetFlags.makeRIO.LAr_setOn()
DetFlags.makeRIO.Tile_setOn()


doCaloNoise=False; 
readG3= False
doLArCellDirect = True; 

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.Luminosity.set_zero()

if readG3:
    GlobalFlags.DataSource.set_geant3()
    GlobalFlags.InputFormat.set_zebra()
else:
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.InputFormat.set_pool()




if readG3:
	theApp.setup( ZEBRA )
	EventSelector = Service( "EventSelector" )
	EventSelector.firstRun=1; 
	EventSelector.lastRun=100000; 
	EventSelector.calos = FALSE
	EventSelector.muons = FALSE
	EventSelector.indet = FALSE
else:
	include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
	EventSelector = Service( "EventSelector" )
	EventSelector.InputCollections = [ "/afs/cern.ch/atlas/offline/data/testfile/dc2.002885.pyt_z_ee.g4dig805._0001.pool.root" ]

include ("RecExCommon/AllDet_detDescr.py") 

if readG3:
        if doLArCellDirect:
            # LArHit->LArCell
            include( "CaloRec/CaloCellMakerFromHit_MC_jobOptions.py" )

            # comment this line to switch off the noise in TIle calo
            TileInfoLoader = Algorithm( "TileInfoLoader" )
            TileInfoLoader.TileNoise            = doCaloNoise
            TileInfoLoader.TileZeroSuppress         = FALSE


            ToolSvc.cellbuilderEM.WithNoise=doCaloNoise # switch noiseon in e.m
            ToolSvc.cellbuilderHEC.WithNoise=doCaloNoise # switch noiseon in HEC
            ToolSvc.cellbuilderFCal.WithNoise=doCaloNoise # switch noiseon in FCAL
        else:
            # LArHit->LArDigit->LArRawChannel->LArCell
            include( "CaloRec/CaloCellMakerFromRaw_MC_jobOptions.py" )
            digitmaker1.NoiseOnOff = doCaloNoise
else:
        # one single file for all calorimeter cell building
        # check on DetFlags yet to be implemented inside
        include( "CaloRec/CaloCellMakerFromRaw_MCG4_jobOptions.py" )


include( "LArClusterRec/LArCluster_jobOptions.py" )

include( "LArClusterRec/LArCluster_MultiSize_jobOptions.py" )

include( "CaloRec/CaloCluster_jobOptions.py" )

from CaloIdentifier import SUBCALO

theApp.Dlls += [ "CaloTools" ]

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


include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
Stream1 = AthenaPoolOutputStream( "Stream1" )

theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
theApp.Dlls   += [ "TileEventAthenaPoolPoolCnv" ]
Stream1.ItemList+=["LArClusterContainer#*"]
Stream1.ItemList+=["LArCellContainer#*"]
Stream1.ItemList+=["EventInfo#*"]
Stream1.ItemList+=["CaloCellContainer#AllCalo"]
#Stream1.ItemList+=["CaloTowerContainer#LArTowerEM"]
#Stream1.ItemList+=["CaloTowerContainer#CombinedTower"]
Stream1.ItemList+=["CaloClusterContainer#CombinedCluster"]
Stream1.OutputFile="LArRecTest.root" 
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump =TRUE

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 2
#  Ntuple output 
theApp.Dlls   += [ "RootHistCnv" ]
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output =  [ "FILE1 DATAFILE='larcluster_write.root' OPT='NEW'" ]
theApp.HistogramPersistency="ROOT"
# Histogram output, if any. 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.root"

# write compact CaloCell with this switch. 
#  ToolSvc.CaloCompactCellTool.WriteCompactCell=True;
