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


#print 'Set some variables for job'
dirtouse = str()

finallist=[]

if 'FILE' in dir() :
     finallist.append(FILE)

athenaCommonFlags.FilesInput=finallist
athenaCommonFlags.EvtMax=-1
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


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan= AthenaMonManager( "PhysValMonManager")
monMan.FileKey = "PhysVal"
topSequence += monMan

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
    
ServiceMgr.THistSvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]


from AthenaCommon.AppMgr import ToolSvc
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool
from TrigEgammaAnalysisTools.TrigEgammaProbelist import default

probelist = default

basePath = '/Trigger/HLT/Egamma/'
tagItems = [
    'e24_lhmedium_iloose_L1EM18VH',
    'e24_lhmedium_iloose_L1EM20VH',
    'e24_lhtight_iloose'
    'e26_lhtight_iloose',
    #Primary cut-based electron triggers
    'e24_medium_iloose_L1EM18VH',
    'e24_medium_iloose_L1EM20VH',
    'e24_tight_iloose',
    'e26_tight_iloose'
    ]
Analysis = TrigEgammaNavAnalysisTool(name='NavAnalysis',
        DirectoryPath=basePath+'Analysis',
        TriggerList=probelist, 
        File="PhysVal")
    
TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
        DirectoryPath=basePath+'TPAnalysis',
        TriggerList=probelist, 
        File="PhysVal",
        TagTriggerList=tagItems,
        OutputLevel=0)
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaPhysValMonTool
TrigEgammaPhysValMonTool = TrigEgammaPhysValMonTool( name = "TrigEgammaPhysValMonTool", 
        histoPathBase=basePath,
        Tools=[
            "TrigEgammaNavAnalysisTool/NavAnalysis",
            "TrigEgammaNavTPAnalysisTool/NavTPAnalysis",
            ])

ToolSvc += TrigEgammaPhysValMonTool
monMan.AthenaMonTools += [ "TrigEgammaPhysValMonTool" ]
    
