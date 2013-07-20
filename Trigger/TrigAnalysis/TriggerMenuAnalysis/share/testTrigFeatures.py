#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#
# doESD, DetDescrVersion and other flags 
# needs be set before the include, since several
# secondary flags are configured according to that one
#

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.EvtMax=100

# from AthenaCommon.GlobalFlags import GlobalFlags
# GlobalFlags.DetGeo.set_atlas()
from RecExConfig.RecFlags  import rec
rec.readESD=True
rec.readRDO=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doCBNT=False
rec.AutoConfiguration = ['everything']
rec.doTruth=False

#DetDescrVersion = 'ATLAS-GEO-08-00-01'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [
    '/scratch/hh/lustre/atlas/monthly/tkohno/data/test/data11_7TeV.00179771.physics_JetTauEtmiss.merge.AOD.f362_m801._lb0132-lb0133._0001.1',
    ]
#---------------------------------------------------------------------
# user modifier should come here
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Set up trigger configuration service and metadata service it relies on,
# for analysis job without RecExCommon set up trigger decision tool
#---------------------------------------------------------------------
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# from TrigDecision.TrigDecisionConfig import TrigDecisionTool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool('TrigDecisionTool')
ToolSvc += tdt

# To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod:
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']
TriggerFlags.configForStartup = 'HLTofflineNoL1Thr'

# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

from AthenaCommon.AppMgr import AlgSequence, ServiceMgr

#------------------------------------------------------------------------
# My job
#------------------------------------------------------------------------

#TriggerFlags.NtupleProductionFlags.FileNames.forSlice
job = AlgSequence.AlgSequence()
from TriggerMenuAnalysis.TriggerMenuAnalysisConfig import *
job += TestNewTDT()
job.TestNewTDT.ChainGroupNames = [
    'EF_j10_a4tc_EFFS', 
    'EF_j15_a4tc_EFFS', 
    'EF_j20_a4tc_EFFS', 
    'L2_j35', 'EF_j40_a4tc_EFFS', 
    'L2_j70', 'EF_j75_a4tc_EFFS',
    'L2_mu4_j10_a4tc_EFFS', 'EF_mu4_j10_a4tc_EFFS',
    'EF_mu4_L1J20_matched',
    'EF_mu4_L1J30_matched', 
    'L2_mu4_L1J10_matched', 'EF_mu4_L1J10_matched',
    ]
job.TestNewTDT.OutputLevel = DEBUG


ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = INFO
# ServiceMgr.ToolSvc.Navigation.OutputLevel = VERBOSE
# ServiceMgr.StoreGateSvc.Dump = True

print ServiceMgr
print ServiceMgr.ToolSvc

print 'topAlgos: '
print job


