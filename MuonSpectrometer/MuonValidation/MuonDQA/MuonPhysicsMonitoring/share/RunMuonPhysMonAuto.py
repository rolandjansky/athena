from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput = [ "/afs/cern.ch/user/m/muoncomm/w0/MuonDQA/giannis/mc09_7TeV.108496.Pythia_directJpsimu0mu0.merge.AOD.e540_s765_s767_r1250_r1260_tid131990_00/AOD.131990._000042.pool.root.1" ]
athenaCommonFlags.FilesInput = [ "/afs/cern.ch/user/m/muoncomm/w0/MuonDQA/giannis/data10_7TeV.00153565.physics_MuonswBeam.merge.AOD.f251_m466/data10_7TeV.00153565.physics_MuonswBeam.merge.AOD.f251_m466._lb0965-lb0984._0001.1", "/afs/cern.ch/user/m/muoncomm/w0/MuonDQA/giannis/data10_7TeV.00153599.physics_MuonswBeam.merge.AOD.f251_m466/data10_7TeV.00153599.physics_MuonswBeam.merge.AOD.f251_m466._lb0504-lb0524._0001.1" ]

useGRL = True ##################################

from RecExConfig.RecFlags import rec

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
if useGRL:
    GlobalFlags.DataSource.set_data() ## Needed for REAL DATA, not MC
from IOVDbSvc.CondDB import conddb

rec.doTrigger.set_Value_and_Lock(False) # leave false; nothing to do with trigger analysis  
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock (False)

OutputLevel = WARNING
doPerfMon = False
doWriteAOD = False

# main jobOption - must always be included
include("RecJobTransforms/UseOracle.py") # DB access
include ("RecExCommon/RecExCommon_topOptions.py")

theApp.EvtMax = -1 # number of event to process

# Stops writing of monitoring ntuples (big files)
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False

# main jobOption

#-----------------------------------------------------
#set up the trigger
#-----------------------------------------------------
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter('ReadPool')

########Setup monitoring ################
from GaudiSvc.GaudiSvcConf import THistSvc
THistSvc = Service ("THistSvc")
THistSvc.Output += ["MuonPhysMon DATAFILE='MuonPhysMon.root' OPT='RECREATE'"]
THistSvc.Output += ["HIST DATAFILE='MuonPhysMon.root' OPT='RECREATE'"]

from AthenaMonitoring.DQMonFlags import DQMonFlags

if not 'DQMonFlags' in dir():
    print "DQMonFlags not yet imported - I import them now"
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    
DQMonFlags.monManEnvironment = "tier0"
DQMonFlags.monManFileKey = "MuonPhysMon"
#DQMonFlags.useTrigger = True 

if DQMonFlags.monManEnvironment() == 'tier0':
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()  
  
  if useGRL == True:
    #-------------------------------------------------------------
    # Good run selection mechanism
    #-------------------------------------------------------------
    ## Configure the goodrunslist selector tool
    from GoodRunsLists.GoodRunsListsConf import *
    ToolSvc += GoodRunsListSelectorTool()
    GoodRunsListSelectorTool.GoodRunsListVec = [ 'MyLBCollection.xml' ]
    ## GRL selector
    from GoodRunsListsUser.GoodRunsListsUserConf import *
    topSequence += GRLTriggerSelectorAlg('GRLTriggerAlg1')
    #### NOTE - variable above MUST match name inside GRL xml file
    #job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = ['MyLBCollection'] 
    topSequence.GRLTriggerAlg1.GoodRunsListArray = ['muon_7TeV'] 

  from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

  from MuonRecExample.MuonRecFlags import muonRecFlags

  from AthenaMonitoring.DQMonFlags import DQMonFlags  
  DQMonFlags.doMuonRawMon=False
  DQMonFlags.doMuonSegmentMon=False
  DQMonFlags.doMuonTrackMon=False
  DQMonFlags.doMuonAlignMon=False  
  #include ("AthenaMonitoring/DataQualitySteering_jobOptions.py")

  
  from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
  MuonDQADetFlags.doMuonMonitoring = True
  # --- MuonPhysics Monitoring options
  MuonDQADetFlags.doMuonPhysMon = True
  MuonDQADetFlags.doMuTrkMon = False 
  MuonDQADetFlags.doZmumuMon = False

  muonOutputLevel=INFO

  if MuonDQADetFlags.doMuonPhysMon():
      include ("MuonPhysicsMonitoring/MuonPhysicsDQA_options.py")
  if MuonDQADetFlags.doMuTrkMon():
      include ("MuonPhysicsMonitoring/MuonTrackPhysicsDQA_options.py")


