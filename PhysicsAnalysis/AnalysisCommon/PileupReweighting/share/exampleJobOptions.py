
#by default (i.e. with no config files) the tool will be created in config file making mode

from glob import glob

theApp.EvtMax=10                                        #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
svcMgr.EventSelector.InputCollections=glob(vars().get("FILES","/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1") )   #insert your list of input files here

algseq = CfgMgr.AthSequencer("AthAlgSeq")


#in your c++ code, create a ToolHandle<IPileupReweightingTool>
#the ToolHandle constructor should be given "CP::PileupReweightingTool/myTool" as its string argument
ToolSvc += CfgMgr.CP__PileupReweightingTool("myTool", 
            ConfigFiles=["my.prw.root"],
            DataScaleFactor=1./1.09,
            UnrepresentedDataAction=2, DataScaleFactorUP=1./1.11, DataScaleFactorDOWN=1./1.07,
            LumiCalcFiles=["/usera/will/testareas/ZZ8TeV/ZZAnalysis/share/GRL_v61.None.lumicalc.root"],OutputLevel=VERBOSE)

algseq += CfgMgr.CP__PileupReweightingProvider(Tool=ToolSvc.myTool,OutputLevel=VERBOSE)                                 #adds an instance of your alg to it


include("AthAnalysisBaseComps/SuppressLogging.py")       #Optional include to suppress as much athena output as possible


