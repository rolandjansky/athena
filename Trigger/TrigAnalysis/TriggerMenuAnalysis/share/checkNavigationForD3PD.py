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
athenaCommonFlags.EvtMax=200

# from AthenaCommon.GlobalFlags import GlobalFlags
# GlobalFlags.DetGeo.set_atlas()
from RecExConfig.RecFlags  import rec
rec.readESD=True
rec.readRDO=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.AutoConfiguration = ['everything']
rec.doTruth=False

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly = True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [
    '/pcatr-srv1/data/files/fwinkl/mc10_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e598_s933_s946_d513.pool.root', 
    #'/scratch/hh/lustre/atlas/monthly/tkohno/data/test/data11_7TeV.00179771.physics_JetTauEtmiss.merge.AOD.f362_m801._lb0132-lb0133._0001.1',
    #'/tmp/tkohno/bjet/data11_7TeV.00183391.physics_JetTauEtmiss.merge.AOD.f384_m878._lb0252-lb0253._0001.1',

    #'/scratch/hh/lustre/atlas/monthly/tkohno/data/test/data11_7TeV.00186729.physics_Egamma.merge.AOD.f394_m934._lb1000-lb1002._0001.1',
    # Same run 186729
    #'/scratch/hh/lustre/atlas/monthly/tkohno/data/AOD.513456._000336.pool.root.1',
    # problem with TrigPassBits
    '/scratch/hh/lustre/atlas/monthly/tkohno/data/test/AOD.322310._000006.pool.root.1',

    # bjet D3PD problem (multi-bjet, multi-RoI vertexing)
    #'/scratch/hh/lustre/atlas/monthly/tkohno/data/bjetD3PD/ESD.after.pool_741.root',
    # one from 17.1.X.Y-VAL,rel_4 (01.02.2012)
    #'/scratch/hh/lustre/atlas/monthly/tkohno/data/test/ESD.pool.root', 
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

## from TrigDecision.TrigDecisionConfig import TrigDecisionTool
#from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
#tdt = Trig__TrigDecisionTool('TrigDecisionTool')
#ToolSvc += tdt

## To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod:
#from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.configurationSourceList = ['ds']
#TriggerFlags.configForStartup = 'HLTofflineNoL1Thr'

## set up trigger config service
#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#cfg =  TriggerConfigGetter()
#

#------------------------------------------------------------------------
# My job
#------------------------------------------------------------------------
from AthenaCommon.AppMgr import AlgSequence, ServiceMgr, ToolSvc
job = AlgSequence.AlgSequence()
from TriggerMenuAnalysis.TriggerMenuAnalysisConfig import *
job += HltNaviChecker()
ToolSvc += RoILinksCnvTool()
ToolSvc.RoILinksCnvTool.Chains_Electron = [
    'L2_e.*', 'EF_e.*', 
    'L2_2e.*', 'EF_2e.*', 
    'L2_3e.*', 'EF_3e.*', 
    ]
ToolSvc.RoILinksCnvTool.Chains_Bjet = [
    'L2_mu.*L1J.*matched', 'EF_mu.*L1J.*matched', 
    'L2_mu.*j.*matched', 'EF_mu.*j.*matched',
    'L2_b.*', 'EF_b.*', 
    'L2_2b.*', 'EF_2b.*', 
    'L2_3b.*', 'EF_3b.*', 
    ]

job.HltNaviChecker.ChainNames = [
    'L2_mu20', 'EF_mu20', 
    'L2_mu4_j10_a4tc_EFFS', 'EF_mu4_j10_a4tc_EFFS',
    'EF_mu4_L1J20_matched',
    'EF_mu4_L1J30_matched', 
    'L2_mu4_L1J10_matched', 'EF_mu4_L1J10_matched',
    'L2_mu4_L1J75_matched', 'EF_mu4_L1J75_matched',
    'L2_mu4_j95_L1matched', 'EF_mu4_j135_a4tc_EFFS_L1matched',
    'L2_e20_medium', 'EF_e20_medium',
    #
    'L2_b10_IDTrkNoCut', 'EF_b10_IDTrkNoCut', 
    'L2_b10_L2Star_IDTrkNoCut', 'EF_b10_L2Star_IDTrkNoCut', 
    'L2_b10_medium_4L1J10', 'EF_b10_medium_4L1J10', 
    'L2_2b10_medium_3L1J10', 'EF_2b10_medium_3L1J10', 
    ]
ToolSvc.RoILinksCnvTool.OutputLevel = DEBUG

ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
ToolSvc.Navigation.OutputLevel = VERBOSE
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = VERBOSE
# ServiceMgr.ToolSvc.Navigation.OutputLevel = VERBOSE
# ServiceMgr.StoreGateSvc.Dump = True


print ServiceMgr
print ToolSvc

#ToolSvc.TrigDecisionTool.Navigation.OutputLevel = DEBUG

print 'topAlgos: '
print job


