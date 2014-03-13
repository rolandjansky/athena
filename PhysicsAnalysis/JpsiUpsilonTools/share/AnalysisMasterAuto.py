############################################################
# AnalysisMasterAuto.py
# These are the master job options for running your analysis
# Include your own job options as shown below ensuring you
# do not overwrite things set here
# RUNS AUTOCONFIG - no need to set tags
############################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ "/afs/cern.ch/atlas/maxidisk/d129/realData/data11_7TeV.00180225.physics_Muons.merge.DAOD_ONIAMUMU.f368_m812_p551_p552/DAOD_ONIAMUMU.334686._000022.pool.root.1" ]
#athenaCommonFlags.PoolInputQuery.set_Value_and_Lock("") # Needed for TAG jobs (as is)
from RecExConfig.RecFlags import rec

rec.doTrigger.set_Value_and_Lock(False) # leave false; nothing to do with trigger analysis  

# include your algorithm job options here
rec.UserAlgs.set_Value_and_Lock("RunJpsiExample.py")

# Output log setting; this is for the framework in general
# You may over-ride this in your job options for your algorithm
rec.OutputLevel.set_Value_and_Lock(INFO);

# Write settings; keep all of these to false.
# Control the writing of your own n-tuple in the alg's job options
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock (False)
rec.doWriteTAG.set_Value_and_Lock (False)
rec.doHist.set_Value_and_Lock (False)

# These 2 lines are needed for the AODFix mechanism
rec.readRDO=False
rec.doESD=False

# main jobOption - must always be included
#include("RecJobTransforms/UseOracle.py") # DB access
include ("RecExCommon/RecExCommon_topOptions.py")
# Following 3 lines needed for TAG jobs (as is)
#svcMgr.EventSelector.RefName= "StreamAOD"
#svcMgr.EventSelector.CollectionType="ExplicitROOT"
#svcMgr.EventSelector.Query = "" 

theApp.EvtMax = -1 # number of event to process

# Stops writing of monitoring ntuples (big files)
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False

