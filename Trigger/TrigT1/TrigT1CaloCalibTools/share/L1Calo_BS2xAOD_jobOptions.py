# Input file(s)
InFiles = [
  "/afs/cern.ch/work/j/juraj/public/2015/TileCIS/252890/data15_calib.00252890.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0001.data"
  #"/afs/cern.ch/work/j/juraj/public/2015/TileCIS/251619/data15_calib.00251619.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0001.data"
   #"/afs/cern.ch/work/h/hristova/public/M7/248370/data14_cos.00248370.physics_L1Calo.merge.RAW._lb0007._SFO-ALL._0001.1"
]

runNumber = ""
for i in InFiles:
  runNumber += i.partition("calib.")[2].partition(".calib")[0]
  pass


#configuration of globalflags... do this FIRST so that anything else you do that might load conddb will be ready for it
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'data'
globalflags.DatabaseInstance = 'CONDBR2'
globalflags.InputFormat = 'bytestream'
globalflags.DetDescrVersion ="ATLAS-R2-2015-02-00-00"
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("CONDBR2-ES1PA-2015-01")


# Output file
OutFile = "xAOD.L1Calo."+runNumber+".pool.root"
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", OutFile )

# Decorate trigger towers with Calo Cells?
# This should be setup better
decorateTT = True

# Number of events (-1 == all events)
nEvents = -1

# Tell Athena about the input file(s)
from ByteStreamCnvSvc import ReadByteStream
svcMgr.EventSelector.Input = InFiles

topSequence = CfgMgr.AthSequencer("AthAlgSeq")

# Add L1Calo xAOD to output
include("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")

xaodStream.AddItem( "xAOD::EventInfo#*" )
xaodStream.AddItem( "xAOD::EventAuxInfo#*" )
sgKeyTriggerTowers = "xAODTriggerTowers"
xaodStream.AddItem( "xAOD::TriggerTowerContainer#%s" % sgKeyTriggerTowers )
xaodStream.AddItem( "xAOD::TriggerTowerAuxContainer#%sAux." % sgKeyTriggerTowers )


if decorateTT:
  from AthenaCommon.DetFlags import DetFlags
  DetFlags.detdescr.all_setOff() 
  DetFlags.detdescr.Calo_setOn()
  include("RecExCond/AllDet_detDescr.py")
  include("CaloRec/CaloRec_jobOptions.py")
  # temp for now - want to put this next line into CaloRec/CaloRec_jobOptions.py
  include("TrigT1CaloCalibTools/DecorateL1CaloTriggerTowers_prodJobOFragment.py")
  pass


# Do some additional tweaking:
theApp.EvtMax = nEvents
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000