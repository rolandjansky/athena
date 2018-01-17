# Job options for standalone and Tier0 running of AnalysisTools 
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# 
# Tool and algorithm configuration done using egamma Factories
# Default configurations found in TrigEgammaAnalysisToolsConfig

# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/work/j/jolopezl/datasets/valid1.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3099_s2578_r6220_tid05203475_00'" -c "NOV=50" test_ZeeElectronLowPtSupportingTrigAnalysis.py
# where NOV is the number of events to run

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AppMgr import ToolSvc

import os

#print 'Set some variables for job'
dirtouse = str()

finallist=[]

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

if 'RTT' in dir():
    rttfile='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/'+RTT+'/x86_64-slc6-gcc49-opt/offline/TrigEgammaValidation/RDOtoAOD_MC_transform_Zee_25ns_pileup/AOD.Zee.25ns.pileup.pool.root'
    finallist.append(rttfile)

if 'GRL' in dir():
    grl=GRL


athenaCommonFlags.FilesInput=finallist
athenaCommonFlags.EvtMax=nov
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
rec.doWriteAOD=True
rec.doWriteESD=False
rec.doDPD=False
rec.doTruth=False

# autoconfiguration might trigger undesired feature
rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(True) # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(True) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
MessageSvc.debugLimit = 20000000
MessageSvc.infoLimit  = 20000000
# TDT
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Use GRL selection 
if 'GRL' in dir():
    # Configure the goodrunslist selector tool
    from GoodRunsLists.GoodRunsListsConf import *
    ToolSvc += GoodRunsListSelectorTool() 
    GoodRunsListSelectorTool.GoodRunsListVec = [grl]
    GoodRunsListSelectorTool.OutputLevel = DEBUG
    from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
    topSequence = AlgSequence()
    seq = AthSequencer("AthFilterSeq")
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

ToolSvc += HLTMon;
HLTMonManager.AthenaMonTools += [ "HLTMonTool/HLTMon" ];
    
from TrigEgammaAnalysisTools import TrigEgammaMonToolConfig
TrigEgammaMonToolConfig.TrigEgammaMonTool()
HLTMonManager.AthenaMonTools += [ "TrigEgammaMonTool/HLTEgammaMon" ]
HLTMonManager.FileKey = "GLOBAL"


    
