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


if not 'FILE' in dir():
      filetouse='AOD.pool.root'
else :
      filetouse=FILE

#if not 'DIR' in dir():
#     dirtouse='/afs/cern.ch/user/r/rwhite/workspace/public/validation/mc/DC14/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
#else :
#     dirtouse=DIR


# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/user/r/rwhite/workspace/egamma/mc/DC14Val/mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3059_s1982_s2008_r5787_tid01572494_00'" test_NavZeeTPAll.py
#listfiles=os.listdir(dirtouse)
finallist=[]
#for ll in listfiles:
finallist.append(filetouse)
#print finallist

athenaCommonFlags.FilesInput=finallist
#athenaCommonFlags.EvtMax=500
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
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
#ToolSvc.TrigDecisionTool.OutputLevel = VERBOSE
# Here we configure the output histogram
# And the athena algorithm, simply a loop over tools
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["Validation_Zee DATAFILE='Validation_Zee.root' OPT='RECREATE'"]
#from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaTDToolTest
#topSequence+=TrigEgammaTDToolTest("TrigEgammaTDToolTest")
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaAnalysisAlg
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavZeeTPCounts,TrigEgammaNavZeeTPPerf,TrigEgammaNavZeeTPEff,TrigEgammaNavZeeTPRes
Res = TrigEgammaNavZeeTPRes(name="NavZeeTPRes",
        ElectronKey="Electrons",
        ProbeTriggerList=['e26_tight_iloose',
            'e24_tight_iloose',
            'e26_lhtight_iloose',
            'e24_tight_iloose_L1EM20VH',
            'e24_medium_iloose_L1EM20VH',
            'e24_lhmedium_iloose_L1EM20VH',
            'e24_tight_iloose_HLTCalo_L1EM20VH',
            'e24_tight_iloose_L2EFCalo_L1EM20VH',
            'e24_lhtight_iloose_L1EM20V',
            'e24_lhtight_iloose_HLTCalo_L1EM20VH',
            'e24_lhtight_iloose_L2EFCalo_L1EM20VH',
            ],
        MinimumTriggerList=["e24_tight","e24_tight_iloose","e26_tight_iloose"],
        TagTrigger="e26_lhtight_iloose",
        OutputLevel=2,
        )
Eff = TrigEgammaNavZeeTPEff(name="NavZeeTPEff",
        ElectronKey="Electrons",
        ProbeTriggerList=['e26_tight_iloose',
            'e24_tight_iloose',
            'e26_lhtight_iloose',
            'e24_tight_iloose_L1EM20VH',
            'e24_medium_iloose_L1EM20VH',
            'e24_lhmedium_iloose_L1EM20VH',
            'e24_tight_iloose_HLTCalo_L1EM20VH',
            'e24_tight_iloose_L2EFCalo_L1EM20VH',
            'e24_lhtight_iloose_L1EM20V',
            'e24_lhtight_iloose_HLTCalo_L1EM20VH',
            'e24_lhtight_iloose_L2EFCalo_L1EM20VH',
            ],
        
        MinimumTriggerList=["e24_tight","e24_tight_iloose","e26_tight_iloose"],
        TagTrigger="e26_lhtight_iloose",
        )
Counts = TrigEgammaNavZeeTPCounts(name="NavZeeTPCounts",
        ElectronKey="Electrons",
        ProbeTriggerList=['e26_tight_iloose',
            'e24_tight_iloose',
            'e26_lhtight_iloose',
            'e24_tight_iloose_L1EM20VH',
            'e24_medium_iloose_L1EM20VH',
            'e24_lhmedium_iloose_L1EM20VH',
            'e24_tight_iloose_HLTCalo_L1EM20VH',
            'e24_tight_iloose_L2EFCalo_L1EM20VH',
            'e24_lhtight_iloose_L1EM20V',
            'e24_lhtight_iloose_HLTCalo_L1EM20VH',
            'e24_lhtight_iloose_L2EFCalo_L1EM20VH',
            ],
        MinimumTriggerList=["e24_tight","e24_tight_iloose","e26_tight_iloose"],
        TagTrigger="e26_lhtight_iloose",
        )

Perf = TrigEgammaNavZeeTPPerf(name="NavZeeTPPerf",
        ElectronKey="Electrons",
        ProbeTriggerList=['e26_tight_iloose',
            'e24_tight_iloose',
            'e26_lhtight_iloose',
            'e24_tight_iloose_L1EM20VH',
            'e24_medium_iloose_L1EM20VH',
            'e24_lhmedium_iloose_L1EM20VH',
            'e24_tight_iloose_HLTCalo_L1EM20VH',
            'e24_tight_iloose_L2EFCalo_L1EM20VH',
            'e24_lhtight_iloose_L1EM20V',
            'e24_lhtight_iloose_HLTCalo_L1EM20VH',
            'e24_lhtight_iloose_L2EFCalo_L1EM20VH',
            ],
        MinimumTriggerList=["e24_tight","e24_tight_iloose","e26_tight_iloose"],
        TagTrigger="e26_lhtight_iloose",
        )

Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[Counts,Perf,Res,Eff])
#Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[Res])

