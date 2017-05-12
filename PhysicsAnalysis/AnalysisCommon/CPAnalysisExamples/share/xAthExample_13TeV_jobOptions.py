
# Set up the file reading: Reads example 13TeV file
FNAME="/afs/cern.ch/atlas/project/PAT/xAODs/r5787/mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD.e2928_s1982_s2008_r5787_r5853_tid01597980_00/AOD.01597980._000098.pool.root.1"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
theJob = CfgMgr.AthSequencer("AthAlgSeq")


# Add the test algorithm:
theJob += CfgMgr.xAthExample()

#configure all the tools (all kept by the ToolSvc in this example)
#need to know if isData or not ... do a file peek with athfile:
from PyUtils import AthFile
af = AthFile.fopen(ServiceMgr.EventSelector.InputCollections[0])
isData = 'IS_SIMULATION' not in af.fileinfos['evt_type']


ToolSvc += CfgMgr.JetCleaningTool("JetCleaning",OutputLevel=DEBUG,CutLevel="MediumBad")

configFile = "JES_Prerecommendation2015_Feb2015.config"

if isData:
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibLCTopo",JetCollection="AntiKt4LCTopo",
            ConfigFile=configFile,
            CalibSequence="JetArea_Residual_EtaJES",IsData=isData)
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibEMTopo",JetCollection="AntiKt4EMTopo",
            ConfigFile=configFile,
            CalibSequence="JetArea_Residual_EtaJES",IsData=isData)
else:
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibLCTopo",JetCollection="AntiKt4LCTopo",
            ConfigFile=configFile,
            CalibSequence="JetArea_Residual_EtaJES",IsData=isData)
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibEMTopo",JetCollection="AntiKt4EMTopo",
            ConfigFile=configFile,
            CalibSequence="JetArea_Residual_EtaJES",IsData=isData)

ToolSvc += CfgMgr.met__METRebuilder("metRebuilder",JetColl="AntiKt4LCCalibJets", 
                                    OutputTotal="Final",SoftTermType="TrackSoftTerm",OutputContainer="MET_CalibFinal")

#configure the output histsvc file
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='myfile.root' OPT='RECREATE'"]


#other job options
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=-1) #disables the eventloop printout, the algorithm has its own
