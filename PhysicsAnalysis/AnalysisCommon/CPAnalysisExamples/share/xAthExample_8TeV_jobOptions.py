
# Set up the file reading: reads 8TeV files from a folder
from glob import glob
files=glob("/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/*")

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = files

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

configFile = "JES_Full2012dataset_Preliminary_MC14.config"

if isData:
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibLCTopo",JetCollection="AntiKt4LCTopo",
            ConfigFile=configFile,
            CalibSequence="EtaJES",IsData=isData)
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibEMTopo",JetCollection="AntiKt4EMTopo",
            ConfigFile=configFile,
            CalibSequence="EtaJES",IsData=isData)
else:
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibLCTopo",JetCollection="AntiKt4LCTopo",
            ConfigFile=configFile,
            CalibSequence="EtaJES",IsData=isData)
   ToolSvc += CfgMgr.JetCalibrationTool("JetCalibEMTopo",JetCollection="AntiKt4EMTopo",
            ConfigFile=configFile,
            CalibSequence="EtaJES",IsData=isData)

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
