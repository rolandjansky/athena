# Default job options for TrigEgammaMatchingTool
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# 

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os

if not 'DIR' in dir():
     dirtouse='/afs/cern.ch/user/r/rwhite/workspace/public/validation/mc/DC14/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
else :
     dirtouse=DIR

outputName = 'Validation_asdf'

# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/user/r/rwhite/workspace/egamma/mc/DC14Val/mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3059_s1982_s2008_r5787_tid01572494_00'" test_NavZeeTPAll.py
listfiles=os.listdir(dirtouse)
finallist=[]
for ll in listfiles:
      finallist.append(dirtouse+'/'+ll)
#print finallist

athenaCommonFlags.FilesInput=finallist
athenaCommonFlags.EvtMax=100
#athenaCommonFlags.EvtMax=-1
rec.readAOD=True
# switch off detectors
rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False
rec.doEgamma=False
rec.doTrigger = True
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
triggerList = [
                  #'e26_tight_iloose',
                  #'e26_lhtight_iloose',
                  #'e26_lhtight_cutd0dphideta_iloose',
                  #'e26_lhtight_iloose_HLTCalo',
                  #'e26_lhvloose_L1EM20VH',
                  #'e26_vloose_L1EM20VH',
                  #'e60_lhmedium',
                  #'e60_lhmedium_HLTCalo',
                  #'e60_lhmedium_cutd0dphideta',
                  #'e60_lhvloose',
                  'e17_lhloose',
                  'e17_loose',
                  #'e13_etcut_trkcut',
                  #"g20_loose",
                  #"g20_tight",
                  "g25_loose",
                  #"g25_medium",
                  #"g35_loose",
                  #"g35_medium",
                  ]
include ("RecExCommon/RecExCommon_topOptions.py")
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
 
from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool
matchtool = Trig__TrigEgammaMatchingTool("MatchingTool");
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += matchtool
 
from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingToolTest
alg = Trig__TrigEgammaMatchingToolTest()
alg.TrigEgammaMatchingTool = matchtool
alg.TriggerList = triggerList
alg.OutputLevel = DEBUG
theJob += alg
 
include("TriggerTest/TriggerTestCommon.py")

