
theApp.EvtMax=-1                                         #says how many events to run over. Set to -1 for all events

#import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
import AthenaRootComps.ReadAthenaxAODHybrid
svcMgr.EventSelector.AccessMode=1 #use class access mode ... I know it's wrong, but it should be ok just for the EventInfo information
from glob import glob
defFile = os.environ.get ('ASG_TEST_FILE_MC',
                          '/afs/cern.ch/user/a/asgbase/patspace/xAODs/r7725/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915862._000100.pool.root.1')
svcMgr.EventSelector.InputCollections=glob( vars().get("FILES",defFile).strip() )

if len(svcMgr.EventSelector.InputCollections)==0:
   print("WARNING  >>>>>>>>>>  generatePRW_jobOptions.py:   NO INPUT FILES PROVIDED/FOUND FROM: %s ... this will produce a failure unless you are sending this job to the grid <<<<<<<<<<<<" % vars().get("FILES","/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1").strip())
   

#instantiate a tool without a config file, so that it's in config file making mode ...
ToolSvc += CfgMgr.CP__PileupReweightingTool("auto",ConfigFiles=[],LumiCalcFiles=[],DataScaleFactor=1.0,DataScaleFactorUP=0.,DataScaleFactorDOWN=0.)

#need to configure the weighttool to use TruthEvents, because of ATLASSIM-2989 bug in 20.7
ToolSvc += CfgMgr.McEventWeight("MyWeights",UseTruthEvents=True)
ToolSvc.auto.WeightTool = ToolSvc.MyWeights


algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq += CfgMgr.CP__PileupReweightingProvider(ConfigOutputStream="METADATA",Tool=ToolSvc.auto)                           #adds an instance of your alg to it

include("AthAnalysisBaseComps/SuppressLogging.py")       #Optional include to suppress as much athena output as possible

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["METADATA DATAFILE='my.prw.root' OPT='RECREATE'"]

#use on the grid like this:
#pathena PileupReweighting/generatePRW_jobOptions.py --inDS="etc/,etc/,etc/" --outDS="user.whatever.myprw/" --extOutFile="auto.prw.root"


