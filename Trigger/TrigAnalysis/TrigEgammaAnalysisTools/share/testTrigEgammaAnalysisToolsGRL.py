# Job options for standalone and Tier0 running of AnalysisTools 
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# 
# Tool and algorithm configuration done using egamma Factories
# Default configurations found in TrigEgammaAnalysisToolsConfig

# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/work/j/jolopezl/datasets/valid1.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3099_s2578_r6220_tid05203475_00'" -c "NOV=50" test_ZeeElectronLowPtSupportingTrigAnalysis.py
# where NOV is the number of events to run

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag="CONDBR2-BLKPA-2014-00"
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os

# Use the parser
import argparse
#print 'Setup parser'
parser = argparse.ArgumentParser()
#parser.add_argument("--file", action="store",help="input file name")
#parser.add_argument("--dirname", action="store",help="input directory")
#parser.add_argument("--nev",default=-1,action=store,type=int,help="number of events to process")
#parser.add_argument("--output",default="Validation",action=store,help="output file name")
#parser.add_argument("--t0",default=False,action=store_true,help="run T0 monitoring")
#parser.add_argument("--tagTrigger",default="e26_lhtight_iloose",action=store,help="Tag trigger to TP")
#parser.add_argument("--useMCMenu",default=False,action=store_true,help="Use MC menu to generate trigger list")
#parser.add_argument("--usePhysicsMenu",default=False,action=store_true,help="Use MC menu to generate trigger list")

#print 'Set some variables for job'
dirtouse = str()

finallist=[]
#print 'Now parser'
#args = parser.parse_args()
#print 'Now setup filelist'
#finallist.append(args.file)
#print 'Or set the directory'
#dirtouse=args.dirname
#nov=args.nev
#while( dirtouse.endswith('/') ) :
#    dirtouse= dirtouse.rstrip('/')
#    listfiles=os.listdir(dirtouse)
#    for ll in listfiles:
#        finallist.append(dirtouse+'/'+ll)
#outputName = args.output
#tagItem = args.Tagtrigger

if 'FILE' in dir() :
     finallist.append(FILE)
elif 'DIR' in dir() :
     dirtouse=DIR       
     while( dirtouse.endswith('/') ) :
          dirtouse= dirtouse.rstrip('/')
     listfiles=os.listdir(dirtouse)
     for ll in listfiles:
          finallist.append(dirtouse+'/'+ll)

if 'NOV' in dir():
    nov=NOV
else :
    nov=-1

if 'OUTPUT' in dir():
    outputName = OUTPUT
elif 'DOTIER0' in dir():
    outputName = ''
else:
    outputName = 'Validation'

if('TAG' in dir()):
    tagItem = TAG 
else: 
    tagItem = 'e26_tight_iloose'


athenaCommonFlags.FilesInput=finallist
athenaCommonFlags.EvtMax=nov
#athenaCommonFlags.EvtMax=-1
rec.readAOD=True
# switch off detectors
rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False
rec.doEgamma=False
rec.doTrigger = True; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched off)
rec.doMuon=False
rec.doMuonCombined=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doDPD=False
rec.doTruth=False


# autoconfiguration might trigger undesired feature
rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# TDT
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.GoodRunsListVec = ['data15_13TeV.periodAllYear_DetStatus-v63-pro18-01_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml']
GoodRunsListSelectorTool.OutputLevel = DEBUG
# Set base path for monitoring/validation tools        
basePath = '/HLT/Egamma/'

if 'DOTIER0' in dir():
    from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
    topSequence = AlgSequence()
    seq = AthSequencer("AthMasterSeq")
    from GoodRunsListsUser.GoodRunsListsUserConf import *
    seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
    seq.GRLTriggerAlg1.GoodRunsListArray = ['PHYS_StandardGRL_All_Good']        ## pick up correct name from inside xml file!
    
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    topSequence += AthenaMonManager( "HLTMonManager")
    HLTMonManager = topSequence.HLTMonManager

    ################ Mon Tools #################

    #Global HLTMonTool

    from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
    HLTMon = HLTMonTool(name               = 'HLTMon',
                   histoPathBase      = "HLT");


    #ToolSvc += HLTMon;
    HLTMonManager.AthenaMonTools += [ HLTMon ];
    
    from TrigEgammaAnalysisTools import TrigEgammaMonToolConfig
    TrigEgammaMonTool = TrigEgammaMonToolConfig.TrigEgammaMonTool()
    HLTMonManager.AthenaMonTools += [ TrigEgammaMonTool ]
    HLTMonManager.FileKey = "GLOBAL"

elif 'DO50ns' in dir():
    from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
    topSequence = AlgSequence()
    seq = AthSequencer("AthMasterSeq")
    from GoodRunsListsUser.GoodRunsListsUserConf import *
    seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
    seq.GRLTriggerAlg1.GoodRunsListArray = ['PHYS_StandardGRL_All_Good']        ## pick up correct name from inside xml file!

    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    topSequence += AthenaMonManager( "HLTMonManager")
    HLTMonManager = topSequence.HLTMonManager

    ################ Mon Tools #################

    #Global HLTMonTool

    from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
    HLTMon = HLTMonTool(name               = 'HLTMon',
                   histoPathBase      = "HLT");


    #ToolSvc += HLTMon;
    HLTMonManager.AthenaMonTools += [ HLTMon ];
    
    from TrigEgammaAnalysisTools import TrigEgammaMonToolConfig50ns
    TrigEgammaMonTool = TrigEgammaMonToolConfig50ns.TrigEgammaMonTool()
    HLTMonManager.AthenaMonTools += [ TrigEgammaMonTool ]
    HLTMonManager.FileKey = "GLOBAL"
elif 'DOPHYSVAL' in dir():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    monMan= AthenaMonManager( "PhysValMonManager")
    monMan.FileKey = "PhysVal"
    topSequence += monMan

    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
    
    ServiceMgr.THistSvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]
    from TrigEgammaAnalysisTools import TrigEgammaPhysValMonToolConfig
    TrigEgammaPhysValMonToolConfig.TrigEgammaPhysValMonTool()
    monMan.AthenaMonTools += [ "TrigEgammaPhysValMonTool" ]
else:
   print "No job configured, options DOPHYSVAL=True or DOTIER0=True" 
    
