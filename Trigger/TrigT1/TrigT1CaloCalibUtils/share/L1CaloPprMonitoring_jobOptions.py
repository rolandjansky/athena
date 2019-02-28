### Output file
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='output.root' OPT='RECREATE'"]

### Input files
DIR='/dataDisk/jhofmann/DESDs/data12_8TeV.00203027.physics_JetTauEtmiss.merge.DESD_CALJET.f443_m1144/*'

from glob import glob
filelist = glob(DIR)

### Number of events (-1 = all)
nEvents = 20
useGRL = True;

from TrigT1CaloCalibUtils.TrigT1CaloCalibUtilsConf import L1CaloPprMonitoring
pprMonitoringJob = L1CaloPprMonitoring("L1CaloPprMonitoring")

### General settings that will be applied to each and every analysis job
pprMonitoringJob.ppmADCMinValue = 60
pprMonitoringJob.lumiBlockMax=2000
pprMonitoringJob.doFineTimePlots = 1
pprMonitoringJob.doPedestalPlots = 1
pprMonitoringJob.doEtCorrelationPlots = 1

from RecExConfig.RecFlags import rec
rec.readESD.set_Value_and_Lock(True)
rec.doCalo.set_Value_and_Lock(True)

from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

#set to false, in case you don't provide a goodrunslist
if useGRL == False:
  # add LumiBlockMetaDataTool to ToolSvc and configure
  from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
  ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
  LumiBlockMetaDataTool.calcLumi = True # False by default
  LumiBlockMetaDataTool.storeXMLFiles = True
  LumiBlockMetaDataTool.applyDQCuts = True
  LumiBlockMetaDataTool.OutputLevel = DEBUG

  # add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
  from AthenaServices.AthenaServicesConf import MetaDataSvc
  svcMgr += MetaDataSvc( "MetaDataSvc" )
  svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

  # Configure the goodrunslist selector tool
  from GoodRunsLists.GoodRunsListsConf import *
  from GoodRunsListsUser.GoodRunsListsUserConf import *
  ToolSvc += GoodRunsListSelectorTool()
  GoodRunsListSelectorTool.OutputLevel = DEBUG
#  GoodRunsListSelectorTool.GoodRunsListVec = ['../grl/PeriodA.GRL.xml',
 #                                             '../grl/PeriodB.GRL.xml',
  #                                            '../grl/PeriodC.GRL.xml',
   #                                           '../grl/PeriodD.GRL.xml',
    #                                          '../grl/PeriodE.GRL.xml',
     #                                         '../grl/PeriodF.GRL.xml',
      #                                        '../grl/PeriodG.GRL.xml',
       #                                       '../grl/PeriodH.GRL.xml',
        #                                      '../grl/PeriodI.GRL.xml']
  GoodRunsListSelectorTool.GoodRunsListVec = ['grl/data11_7TeV.periodAllYear_DetStatus-v36-pro10_CoolRunQuery-00-04-08_All_Good.xml','grl/data12_8TeV.periodAllYear_HEAD_All_Good.xml']
  GoodRunsListSelectorTool.PassThrough = False
  seq = AthSequencer("AthMasterSeq")
  seq += GRLTriggerSelectorAlg('MyLBCollection')


disableRecJobOpts = ('doAOD', 'doAODCaloCells', 'doAODall', 'doAlfa', 'doApplyAODFix', 'doCBNT', 'doCheckDictionary',
                    'doCheckJOT',    'doDPD', 'doDetStatus', 'doDetailedAuditor', 'doDetailedPerfMon', 'doDumpMC',
                    'doDumpPoolInputContent', 'doDumpProperties', 'doDumpTDS', 'doDumpTES', 'doESD', 'doEdmMonitor',
                    'doEgamma', 'doFileMetaData', 'doFloatingPointException', 'doForwardDet', 'doHeavyIon', 'doHist', 'doInDet',
                    'doJetMissingETTag', 'doJiveXML', 'doLowPt', 'doLucid', 'doMinimalRec', 'doMonitoring', 'doMuon',
                    'doMuonCombined', 'doNameAuditor', 'doPerfMon', 'doPersint', 'doPyDump', 'doRestrictedESD', 'doSGAuditor',
                    'doShowSizeStatistics', 'doTagRawSummary', 'doTau', 'doTimeLimit', 'doTrigger', 'doTruth',
                    'doVP1', 'doWriteAOD', 'doWriteBS', 'doWriteESD', 'doWriteRDO', 'doWriteTAG', 'doWriteTAGCOM', 'doZdc',
                    'readAOD', 'readRDO', 'readTAG')
                    
for p in disableRecJobOpts:
   getattr(rec, p).set_Value_and_Lock(False)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolESDInput.set_Value_and_Lock(filelist)
athenaCommonFlags.EvtMax.set_Value_and_Lock(nEvents)

rec.AutoConfiguration=['everything']

from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
job += pprMonitoringJob

include('RecExCommon/RecExCommon_topOptions.py')

### Database access
include("TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py")
from IOVDbSvc.CondDB import conddb
conddb.addFolder('TRIGGER','/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib')
conddb.addFolder('TRIGGER','/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels')
conddb.addFolder('TRIGGER','/TRIGGER/Receivers/Conditions/VgaDac')
conddb.addFolder('TRIGGER','/TRIGGER/Receivers/Factors/CalibGains')
#Connect also to the sqlite file containting the fine time references and the calibration factors
#comment out the following two lines, if database is not present, otherwise Athena won't run
if pprMonitoringJob.doFineTimePlots:
    svcMgr.IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=../share/calibJuly.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/References/FineTimeReferences"]


#### L1Config and tools
#from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
#LVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')
#LVL1ConfigSvc.XMLFile = "LVL1config_Physics_pp_v1.xml"
#ServiceMgr += LVL1ConfigSvc

StatusCodeSvc.AbortOnError=False
