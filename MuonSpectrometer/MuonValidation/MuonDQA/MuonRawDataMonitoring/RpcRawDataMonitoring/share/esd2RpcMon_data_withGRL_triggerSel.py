#example of personal topOptions
#
# in 20.1.5.12 for run00272531f611
# run with athena.py esd2RpcMon_fromMyTopOptions.py > log_esd2RpcMon_fromMyTopOptions & 
#
# to use it  
# RecExCommon_links.sh  # once in a given directory
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)

#set input file. Note that this triggers autoconfiguration automatically (see RecExCommonAutoConfiguration wiki)
# here a MC RDO is specified, but this can also be a data BS, an ESD AOD or TAG
#if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
#    athenaCommonFlags.FilesInput=["LFN:top_GEO-02-01-00_RDO_extract.pool"] 

# input is DESDM_MCP
athenaCommonFlags.FilesInput=["/einstein2/stefania/atlas/RunII_rpc/Perf/test/data15_13TeV.00281411.physics_Main.merge.DESDM_MCP.f629_m1508._0460.1"]
#athenaCommonFlags.FilesInput=["/tmp/stefspa/esdMuons100_trf.pool.root"]
#athenaCommonFlags.FilesInput=["/tmp/stefspa/esdGeantino100k_trf.pool.root"]
#athenaCommonFlags.FilesInput=["/tmp/stefspa/esdFromMaxRecoGeantino.pool.root"]
#athenaCommonFlags.FilesInput=["/tmp/stefspa/esd.pool.root"]
#athenaCommonFlags.FilesInput=["/afs/cern.ch/user/s/stefspa/atlas/SW/RPCdigTest/runtrf/ESD.pool.root"]
#athenaCommonFlags.FilesInput=["/afs/cern.ch/work/s/stefspa/ESD_GeantinoHits_test125k_condRun00281411.pool.root"]
#athenaCommonFlags.FilesInput=["/afs/cern.ch/work/s/stefspa/ESD_GeantinoHits_test100kFixed_avEffStep5.pool.root"]
#athenaCommonFlags.FilesInput=["/afs/cern.ch/user/s/stefspa/myAfsLinks/stefspaWork/group.det-muon.147407.PowhegPythia8.e3099_s2621.rel201411.RPCDigi010514.nomod.20150930.v01_EXT0/group.det-muon.6582837.EXT0._001189.ESD.pool.root"]
#athenaCommonFlags.FilesInput=["/afs/cern.ch/user/s/stefspa/atlas/SW/RPCdigTest/run/tryDigPrdOnly/restart/try/ESD.pool.root"]
#"root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00278734/data15_13TeV.00278734.physics_Main.recon.ESD.f628/data15_13TeV.00278734.physics_Main.recon.ESD.f628._lb0005._SFO-6._0001_003.1"]
#
#/eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00272531/data15_13TeV.00272531.physics_Main.merge.DESDM_MCP.f611_m1467/data15_13TeV.00272531.physics_Main.merge.DESDM_MCP.f611_m1467._0011.1"]
#athenaCommonFlags.FilesInput=["/tmp/stefspa/testCrash/group.det-muon.147407.PowhegPythia8.e3099_s2621.rel201411.RPCDigi010512.Only.20150913.v01_EXT0.42808390/group.det-muon.6472803.EXT0._001141.ESD.pool.root"]

# input is ESD !!                                                                                                                                                                  
#athenaCommonFlags.FilesInput=["root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00272531/data15_13TeV.00272531.physics_Main.recon.ESD.f611/data15_13TeV.00272531.physics_Main.recon.ESD.f611._lb0130._SFO-6._0001._003.1"]
##
#
#/eos/atlas/atlastier0/rucio/data15_comm/physics_Main/00273391/data15_comm.00273391.physics_Main.recon.ESD.f611/data15_comm.00273391.physics_Main.recon.ESD.f611._lb0820._SFO-6._0001._003.1"]
#
#"root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_cos/physics_CosmicMuons/00274784/data15_cos.00274784.physics_CosmicMuons.recon.ESD.x345/data15_cos.00274784.physics_CosmicMuons.recon.ESD.x345._lb0439._SFO-ALL._0001_000.1"]
#
#"root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00271595/data15_13TeV.00271595.physics_Main.merge.DESDM_MCP.f611_m1467/data15_13TeV.00271595.physics_Main.merge.DESDM_MCP.f611_m1467._0074.1"]
#
#root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_comm/physics_Main/00266766/data15_comm.00266766.physics_Main.recon.ESD.f594/data15_comm.00266766.physics_Main.recon.ESD.f594._lb0017._SFO-ALL._0001.1"]#/eos/atlas/atlastier0/rucio/data15_comm/physics_Main/00265573/data15_comm.00265573.physics_Main.recon.ESD.f581/data15_comm.00265573.physics_Main.recon.ESD.f581._lb0175._SFO-ALL._0001.1"]

#athenaCommonFlags.jp.AthenaCommonFlags.EvtMax=-1   # number of events to process run on all file
#athenaCommonFlags.EvtMax=200000   # number of events to process run on all file
#athenaCommonFlags.EvtMax=-1   # number of events to process run on all File
athenaCommonFlags.EvtMax=1000   # number of events to process run on all file


rec.doTrigger=True
recAlgs.doTrigger=True
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True
## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel = INFO



# switch off writing of standard reco output 
rec.doWriteAOD=False
rec.doWritexAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doFileMetaData=False

rec.doAOD=False
rec.doESD=False
rec.doAODCaloCells=False
rec.doAODall=False
rec.doDPD=False


# switch off detectors
rec.doForwardDet=False
# rec.doInDet=False # this will generate a meaningless FATAL error 
rec.doCalo=False
# rec.doMuon=False
rec.doEgamma=False
rec.doJetMissingETTag=False
rec.doTau=False
rec.doBTagging=False
rec.doTagRawSummary=False
rec.doLucid=False
#rec.doTrigger = False; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched of)
rec.doMuonCombined=False
rec.doTruth=False

# ** Turn PerfMon off for valgrind usage **
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)
#rec.doNameAuditor=True
rec.doNameAuditor=False



# include my own algorithm(s). These jobo will be included at the end of topSequence configuration
# rec.UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ]
# these lines will be executed at the end of topSequence configuration
# rec.UserExecs=[ "from MyPackage.myAlgConf import myAlg","topSequence+=myAlg()" ]
# these lines will be executed after RecExCommon_flags.py has been imported, last chance for flag modification
# note that flag locking mechanism has a better chance to yield consistent configuration
# see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerFlags
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# rec.UserFlags=[ "AODFlags.FastSimulation=False" ] 


# autoconfiguration might trigger undesired feature
# rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
# rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
# rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
# rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
# rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG
#rec.doDumpTES=True

rec.doMonitoring.set_Value_and_Lock(True)
from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.monManEnvironment.set_Value_and_Lock('tier0ESD')#produces run directories
#DQMonFlags.monManEnvironment.set_Value_and_Lock('user')#doesn't produce run directories

DQMonFlags.doInDetAlignMon.set_Value_and_Lock(False)
#disable all monitoring other than alignment (too susceptible to crashes)
DQMonFlags.doPixelMon.set_Value_and_Lock(False)
DQMonFlags.doSCTMon.set_Value_and_Lock(False)
DQMonFlags.doTRTMon.set_Value_and_Lock(False)
DQMonFlags.doInDetGlobalMon.set_Value_and_Lock(False)
DQMonFlags.doGlobalMon.set_Value_and_Lock(False)
DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
DQMonFlags.doCTPMon.set_Value_and_Lock(False)
DQMonFlags.doHLTMon.set_Value_and_Lock(False)
DQMonFlags.doTRTElectronMon.set_Value_and_Lock(False)
DQMonFlags.doTileMon.set_Value_and_Lock(False)
DQMonFlags.doLArMon.set_Value_and_Lock(False)
DQMonFlags.doCaloMon.set_Value_and_Lock(False)
DQMonFlags.doEgammaMon.set_Value_and_Lock(False)
DQMonFlags.doMissingEtMon.set_Value_and_Lock(False)
DQMonFlags.doJetMon.set_Value_and_Lock(False)
DQMonFlags.doTauMon.set_Value_and_Lock(False)
DQMonFlags.doJetTagMon.set_Value_and_Lock(False)
DQMonFlags.doLucidMon.set_Value_and_Lock(False)
DQMonFlags.doInDetPerfMon.set_Value_and_Lock(False)
# muon monitoring 
DQMonFlags.doMuonSegmentMon.set_Value_and_Lock(False)
DQMonFlags.doMuonTrackMon.set_Value_and_Lock(False)
DQMonFlags.doMuonAlignMon.set_Value_and_Lock(False)
DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)
DQMonFlags.doMuonTrkPhysMon.set_Value_and_Lock(False)
DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(False)
# muon raw monitoring 
DQMonFlags.doMuonRawMon.set_Value_and_Lock(True)
# muon raw monitoring - all det off but RPC and RPCL1
from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
MuonDQADetFlags.doMDTMon.set_Value_and_Lock(False)
MuonDQADetFlags.doCSCMon.set_Value_and_Lock(False)
MuonDQADetFlags.doTGCMon.set_Value_and_Lock(False)
MuonDQADetFlags.doMDTRPCMon.set_Value_and_Lock(False)
MuonDQADetFlags.doMDTTGCL1Mon.set_Value_and_Lock(False)
MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(False)
#
MuonDQADetFlags.doRPCMon.set_Value_and_Lock(True)
MuonDQADetFlags.doRPCL1Mon.set_Value_and_Lock(True)


###############
### try here GRL selection 
# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.GoodRunsListVec = [ 'data15_13TeV.periodAllYear_DetStatus-v71-pro19-06_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns.xml' ]
GoodRunsListSelectorTool.PassThrough = False
## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
#job = AlgSequence()
seq = AthSequencer("AthMasterSeq")
## AthMasterSeq is always executed before the top sequence, and is configured such that
## any follow-up sequence (eg. top sequence) is not executed in case GRLTriggerAlg1 does
## not pass the event
## In short, the sequence AthMasterSeq makes sure that all algs in the job sequence
## are skipped when an event gets rejects
from GoodRunsListsUser.GoodRunsListsUserConf import *
seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
seq.GRLTriggerAlg1.GoodRunsListArray = ['PHYS_StandardGRL_All_Good_25ns']        ## pick up correct name from inside xml file!
#seq.GRLTriggerAlg1.TriggerSelectionRegistration = 'L1_MBTS_1' ## set this to your favorite trigger, eg. L1_MBTS_1_1
#seq.GRLTriggerAlg1.OutputLevel=DEBUG
###############


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


# user modifier of properties should come here
#topSequence.myAlg.someProperty=3.14
#
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.IgnoreTagDifference=True
#
from RpcRawDataMonitoring.RpcRawDataMonitoringConf import RpcRawDataValAlg
# the following true to have strip profiles 
RpcRawDataValAlg.doCoolDB = True
#
from RpcRawDataMonitoring.RpcRawDataMonitoringConf import RPCStandaloneTracksMon
# the following true to have tomography  
# RPCStandaloneTracksMon.doRadiography = True
# the following true to have per strip timing and efficiency                                                                                         
RPCStandaloneTracksMon.doCoolDB      = True
RPCStandaloneTracksMon.StandAloneMatchedWithTrack = True
RPCStandaloneTracksMon.selectTriggerChainGroup   = True
RPCStandaloneTracksMon.deSelectTriggerChainGroup = False
#RPCStandaloneTracksMon.OutputLevel=DEBUG
#to select HLT muon triggers ------------------------------
#RPCStandaloneTracksMon.triggerChainGroupRegExp='HLT_mu.*'
#to select ortogonal (to muons) triggers ------------------------------
RPCStandaloneTracksMon.triggerChainGroupRegExp='HLT_(e[0-9]*_loose_L1EM[0-9]*|e[0-9]*_lhmedium_L1EM[0-9]*VH|g[0-9]*_loose|2g[0-9]*_tight|tau[0-9]*_medium1_tracktwo|j[0-9]*_320eta490|j[0-9]*|[0-9]j[0-9]*|xe[0-9]*|j[0-9]*_xe[0-9]*|j[0-9]*_bmedium_split|j[0-9]*_bloose_split)'


#svcMgr.MessageSvc.OutputLevel=VERBOSE
svcMgr.MessageSvc.defaultLimit=9999999
#RPCStandaloneTracksMon.OutputLevel=DEBUG

