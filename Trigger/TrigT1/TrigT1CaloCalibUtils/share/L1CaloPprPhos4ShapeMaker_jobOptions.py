from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from TrigT1CaloCalibUtils.TrigT1CaloCalibUtilsConf import L1CaloPprPhos4ShapeMaker
phos4job = L1CaloPprPhos4ShapeMaker( "L1CaloPprPhos4ShapeMaker" )
phos4job.outputDirectory = "/tmp/childers"
job += phos4job

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#MessageSvc = Service( "MessageSvc" )
#MessageSvc.OutputLevel = ERROR


from RecExConfig.RecFlags import rec
rec.readRDO.set_Value_and_Lock(True)
rec.doLArg.set_Value_and_Lock(False)
rec.doTile.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(True)

disableRecJobOpts = ('doWriteESD','doTruth', 'doAOD','doAODCaloCells',
          'doAODall','doCBNT','CBNTAthenaAware','doPerfMon','doHist',
          'doDetailedPerfMon','doWriteAOD','doWriteTAG','doInDet','doMuon','doJetMissingETTag',
          'doEgamma','doMuonCombined','doTau','doMonitoring','readESD','doWriteBS','doDPD',
          'doWriteTAGCOM','doPyDump','doWriteESD','doFileMetaData','doCheckDictionary','doPersint','doJiveXML',
          'Commissioning','doHeavyIon','doDumpPoolInputContent','doEdmMonitor')
for p in disableRecJobOpts:
   getattr(rec, p).set_Value_and_Lock(False)

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType = 'cosmics'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(['/dataDisk/phos4scans/lar/00173146/data11_calib.00173146.calibration_L1CaloPprPhos4ScanPars.daq.RAW._lb0000._SFI-LVL1-1._0001.data'])

athenaCommonFlags.EvtMax.set_Value_and_Lock(-1)

# For calibration runs you must set the run number
# to an ATLAS run that occured near to the calibration run
# this allows for some databases to be setup properly
#Service("IOVDbSvc").forceRunNumber=148787
#Service("IOVDbSvc").forceLumiblockNumber=1
#Service("IOVDbSvc").OutputLevel=2

include ("RecExCommon/RecExCommon_topOptions.py")

include ("TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py")

# Setup database file for L1Calo
# from IOVDbSvc.CondDB import conddb 
# conddb.addFolder('','<dbConnection>oracle://INTR;schema=ATLAS_COOLONL_TRIGGER;dbname=L1CALO</dbConnection> /TRIGGER/L1Calo/V1/Calibration/Calib1/PprChanCalib <tag>HEAD</tag>')
# conddb.addFolder('','<dbConnection>oracle://INTR;schema=ATLAS_COOLONL_TRIGGER;dbname=L1CALO</dbConnection> /TRIGGER/L1Calo/V1/Configuration/ReadoutConfig <tag>HEAD</tag>')


from AthenaCommon.AppMgr import ServiceMgr
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr+=PoolSvc(SortReplicas=True)
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
ServiceMgr+=DBReplicaSvc(UseCOOLSQLite=False)
