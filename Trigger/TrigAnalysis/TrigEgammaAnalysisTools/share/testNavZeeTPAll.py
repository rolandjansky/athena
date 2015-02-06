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

#if not 'DIR' in dir():
#     dirtouse='valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
#else :
#     dirtouse=DIR

dirtouse='/afs/cern.ch/user/r/rwhite/workspace/xAOD/DC14/19.1.1.5/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'

# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/user/r/rwhite/workspace/egamma/mc/DC14Val/mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3059_s1982_s2008_r5787_tid01572494_00'" Nav_ZeeTPVal.py
listfiles=os.listdir(dirtouse)
finallist=[]
for ll in listfiles:
      finallist.append(dirtouse+'/'+ll)
#print finallist

athenaCommonFlags.FilesInput=finallist
athenaCommonFlags.EvtMax=1000
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

# Here we configure the output histogram
# And the athena algorithm, simply a loop over tools
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["Validation_Zee DATAFILE='Validation_Zee.root' OPT='RECREATE'"]

from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaAnalysisAlg
TrigEgammaAnalysisAlg()

