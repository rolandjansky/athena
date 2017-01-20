from AthenaCommon.AthenaCommonFlags import jobproperties as jp
acf = jp.AthenaCommonFlags
acf.FilesInput = [ 'root://eosatlas//eos/atlas/atlasdatadisk/rucio/mc16_13TeV/79/34/AOD.09730552._000001.pool.root.1' ] #mc16_13TeV.423000.ParticleGun_single_electron_egammaET.merge.AOD.e3566_s2955_r8668_r8633
acf.EvtMax = 500
include( "AthenaPython/read_file.py" )

from RecExConfig.RecFlags  import rec
rec.doTrigger.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False);
rec.doDetailedPerfMon.set_Value_and_Lock(False);
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False);
from PerfMonComps.PerfMonFlags import jobproperties;
jobproperties.PerfMonFlags.doMonitoring.set_Value_and_Lock(False);

## to run on old geometry
# from GeoModelSvc.GeoModelSvcConf import GeoModelSvc;
# GeoModelSvc.IgnoreTagSupport=True
 
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

include('CaloCalibHitRec/CalibHitToCaloCellTool_jobOptions.py')

# myCalibHitToCaloCellTool_DM = CalibHitToCaloCellTool(name = "myCalibHitToCaloCellTool_DM",
#                                                   CalibHitContainers = [ 'LArCalibrationHitDeadMaterial' ],
#                                                   OutputLevel = DEBUG)
# ToolSvc += myCalibHitToCaloCellTool_DM


from CaloCalibHitRec.CaloCalibHitRecConf import CalibHitToCaloCellAlg

myCalibHitToCaloCellAlg_LAr = CalibHitToCaloCellAlg( #OutputLevel = DEBUG,
                                  CalibHitToCaloCellToolName = CalibHitToCaloCellTool_LAr )
topSequence += myCalibHitToCaloCellAlg_LAr

# myCalibHitToCaloCellAlg_Tile = CalibHitToCaloCellAlg( #OutputLevel = DEBUG,
#                                   CalibHitToCaloCellToolName = myCalibHitToCaloCellTool_Tile )
# topSequence += myCalibHitToCaloCellAlg_Tile

# myCalibHitToCaloCellAlg_DM = CalibHitToCaloCellAlg( OutputLevel = DEBUG,
#                                 CalibHitToCaloCellToolName = myCalibHitToCaloCellTool_DM,
#                                 TruthCaloCellsOutputName = "TruthCellsDM",
#                                 TruthCaloClustersOutputName = "TruthClustersDM",
#                                 CaloCellMakerTools = [ theCaloCellContainerFinalizerTool ] )
# topSequence += myCalibHitToCaloCellAlg_DM

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
aodStream = MSMgr.NewPoolStream( "StreamAOD", "myAOD.withTruthClusters.pool.root" )
aodStream.EvtConversionSvc     ="AthenaPoolCnvSvc"
aodStream.AddItem( "CaloCellContainer#*")
aodStream.AddItem( "CaloCompactCellContainer#*")
aodStream.AddItem( "CaloCalibrationHitContainer#*")
aodStream.AddItem( "xAOD::CaloClusterContainer#*")
aodStream.AddItem( "xAOD::CaloClusterAuxContainer#*")
aodStream.AddItem( "CaloClusterCellLinkContainer#*")
aodStream.AddItem( "xAOD::TrackParticleContainer#In*")
aodStream.AddItem( "xAOD::TrackParticleAuxContainer#*")
aodStream.AddItem( "xAOD::TruthParticleContainer#*")
aodStream.AddItem( "xAOD::TruthParticleAuxContainer#*")
aodStream.AddItem( "xAOD::ElectronContainer#*")
aodStream.AddItem( "xAOD::ElectronAuxContainer#*")
aodStream.Print()

ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_SPLITLEVEL='99'" ]
# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"

# Do some additional tweaking:
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
# ServiceMgr.AthenaSealSvc.OutputLevel = VERBOSE
