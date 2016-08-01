#Skeleton joboption for a simple analysis job

theApp.EvtMax=1000                                         #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
inputfile = os.environ.get("ASG_TEST_FILE_MC", "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r6630/mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.AOD.e3601_s2576_s2132_r6630_tid05358812_00/AOD.05358812._000010.pool.root.1")
svcMgr.EventSelector.InputCollections=[inputfile]

#optional: configure the matching tool
ToolSvc += CfgMgr.Trig__MatchingTool("MyMatchingTool",OutputLevel=DEBUG)

algseq = CfgMgr.AthSequencer("AthAlgSeq")                              #gets the main AthSequencer
algseq += CfgMgr.TestMatchingToolAlg()                                 #adds an instance of your alg to it

#svcMgr.MessageSvc.infoLimit=9999999999
#svcMgr.MessageSvc.debugLimit=9999999999

include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above
