# analysis release job options for TrigEgammaMatchingTool
# Authors: 
# Will Buttinger 
# 


#if not 'DIR' in dir():
#     dirtouse='/afs/cern.ch/user/r/rwhite/workspace/public/validation/mc/DC14/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
#else :
#     dirtouse=DIR

outputName = 'Validation_asdf'



import AthenaPoolCnvSvc.ReadAthenaPool
from glob import glob
svcMgr.EventSelector.InputCollections=[os.environ['ROOTCORE_TEST_FILE']]
theApp.EvtMax=100

triggerList = [
                  'HLT_e26_lhtight_iloose',
                  'HLT_e60_lhmedium',
                  'HLT_e17_lhloose',
                  'HLT_e17_loose',
                  'HLT_e14_etcut',
                  ]

cmbList = [ 'HLT_e17_lhloose_mu14', 'HLT_2e17_lhloose','HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo', 'HLT_e9_tight_e4_etcut_Jpsiee']

ToolSvc += CfgMgr.TrigConf__xAODConfigTool("MyConfig")
ToolSvc += CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool",ConfigTool=ToolSvc.MyConfig,TrigDecisionKey="xTrigDecision")

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
 
ToolSvc += CfgMgr.Trig__TrigEgammaMatchingTool("TrigEgammaMatchingTool");
#ToolSvc += CfgMgr.Trig__MatchingTool("TriggerMatchingTool",TrigDecisionTool=ToolSvc.TrigDecisionTool,DefaultMatchingThreshold=0.07)


theJob += CfgMgr.Trig__TrigEgammaMatchingToolTest(TrigEgammaMatchingTool=ToolSvc.TrigEgammaMatchingTool, 
        TriggerList=triggerList, CombinedTriggerList=cmbList,OutputLevel=INFO)
 
#include("TriggerTest/TriggerTestCommon.py")

