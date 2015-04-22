# Default job options for TrigEgammaAnalysisTools
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# Denis Damazio <denis.damazio@cern.ch
# 
# Tool and algorithm configuration done using egamma Factories
# Default configurations found in TrigEgammaAnalysisToolsConfig

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os

if not 'DIR' in dir():
     dirtouse='/afs/cern.ch/work/j/jolopezl/datasets/valid1.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3099_s2578_r6220_tid05203475_00'
else :
     dirtouse=DIR

if not 'NOV' in dir():
    nov=-1
else :
    nov=NOV


# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/work/j/jolopezl/datasets/valid1.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3099_s2578_r6220_tid05203475_00'" -c "NOV=50" test_ZeeElectronLowPtSupportingTrigAnalysis.py
# where NOV is the number of events to run
listfiles=os.listdir(dirtouse)
finallist=[]
for ll in listfiles:
      finallist.append(dirtouse+'/'+ll)
#print finallist

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
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
#ToolSvc.TrigDecisionTool.OutputLevel = VERBOSE
# Here we configure the output histogram
# And the athena algorithm, simply a loop over tools
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["Validation_Zee_HighPtPhysicsTriggers DATAFILE='Validation_Zee_HighPtPhysicsTriggers.root' OPT='RECREATE'"]
#ServiceMgr.THistSvc.Output += ["zee DATAFILE='zee.root' OPT='RECREATE'"]
#from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaTDToolTest
#topSequence+=TrigEgammaTDToolTest("TrigEgammaTDToolTest")
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaAnalysisAlg
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavZeeTPCounts,TrigEgammaNavZeeTPPerf,TrigEgammaNavZeeTPEff,TrigEgammaNavZeeTPRes,TrigEgammaNavZeeTPNtuple,TrigEgammaNavZeeTPIneff,TrigEgammaEmulationTool
from TrigEgammaAnalysisTools.TrigEgammaProbelist import * # to import probelist

probelist = probeListHighPtPhysicsTriggers

Res = TrigEgammaNavZeeTPRes(name="NavZeeTPRes",
        ElectronKey="Electrons",
        ProbeTriggerList=probelist,
        TagTrigger="e26_lhtight_iloose",
        OutputLevel=2,
        File="Validation_Zee_HighPtPhysicsTriggers",
        )
Eff = TrigEgammaNavZeeTPEff(name="NavZeeTPEff",
        ElectronKey="Electrons",
        ProbeTriggerList=probelist,
        TagTrigger="e26_tight_iloose",
        File="Validation_Zee_HighPtPhysicsTriggers",
        )
Counts = TrigEgammaNavZeeTPCounts(name="NavZeeTPCounts",
        ElectronKey="Electrons",
        ProbeTriggerList=probelist,
        TagTrigger="e26_tight_iloose",
        File="Validation_Zee_HighPtPhysicsTriggers",
        )

Ineff = TrigEgammaNavZeeTPIneff(name="NavZeeTPIneff",
        ElectronKey="Electrons",
        ProbeTriggerList=probelist,
        TagTrigger="e26_lhtight_iloose",
        File="Validation_Zee_HighPtPhysicsTriggers",
        )

Perf = TrigEgammaNavZeeTPPerf(name="NavZeeTPPerf",
#        File = "ttbar",
        OutputLevel = 2,
        ElectronKey="Electrons",
        ProbeTriggerList=probelist,
        TagTrigger="e26_lhtight_iloose",
        File="Validation_Zee_HighPtPhysicsTriggers",
        )

Ntuple = TrigEgammaNavZeeTPNtuple(name="NavZeeTPNtuple",
        ElectronKey="Electrons",
        ProbeTriggerList=probelist,
        TagTrigger="e26_lhtight_iloose",
        doRinger=False, # if its true, we will save only tes with ringer.
        File="Validation_Zee_HighPtPhysicsTriggers",
        )


Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[Counts,Eff,Perf,TrigEgammaEmulationTool(OutputLevel=2)])
#Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[Perf]) # for Wprime