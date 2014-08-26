############################################################
# BPhysAnalysisMasterAuto.py
# These are the master job options for running your analysis
# Include your own job options as shown below ensuring you
# do not overwrite things set here
# RUNS AUTOCONFIG - no need to set tags
############################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput = [ "/afs/cern.ch/atlas/maxidisk/d129/realData/data11_7TeV.00190689.physics_Muons.merge.DAOD_ONIAMUMU.f411_m1014_p809_p811_tid594772_00/DAOD_ONIAMUMU.594772._000010.pool.root.1" ]
#athenaCommonFlags.FilesInput = [ "/tmp/mskinner/valid1.105013.J4_pythia_jetjet.recon.AOD.e815_s1310_s1300_r3176_tid728127_00/AOD.728127._000036.pool.root.1" ]
#athenaCommonFlags.FilesInput = [ "/tmp/mskinner/mc11_7TeV.126389.Sherpa2DP20.merge.AOD.e1028_s1310_s1300_r3108_r3109_tid658500_00/AOD.658500._000008.pool.root.1" ]
#athenaCommonFlags.FilesInput = [ "/tmp/mskinner/mc11_7TeV.152613.GGM_gl_wino_700_680_newgfilter_herwigpp_susy.merge.AOD.e1078_s1372_s1370_r3108_r3109_tid708730_00/AOD.708730._000001.pool.root.1" ]
#athenaCommonFlags.FilesInput = [ "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/AOD.728127._000036.pool.root.1" ]
#athenaCommonFlags.FilesInput = [ "tmpRDO.pool.root" ]
#athenaCommonFlags.FilesInput = [ "PileupDigiRTTOutput.RDO.pool.root" ]
#athenaCommonFlags.FilesInput = [ "tmp.ESD.pool.root" ]
athenaCommonFlags.FilesInput = [ "AOD.728127._000036.pool.root.1" ]

#athenaCommonFlags.PoolInputQuery.set_Value_and_Lock("") # Needed for TAG jobs (as is)
from RecExConfig.RecFlags import rec

rec.doTrigger.set_Value_and_Lock(False) # leave false; nothing to do with trigger analysis  

# include your algorithm job options here
#rec.UserAlgs.set_Value_and_Lock("PileupRTT/PileupMCEventCollectionChecker_jobOptions.py")
#rec.UserAlgs.set_Value_and_Lock("/afs/cern.ch/user/m/mskinner/athena/17.2.0.2/Simulation/PileupRTT/share/PileupValidation_test.py")
rec.UserAlgs.set_Value_and_Lock("PileupRTT/PileupValidation_test.py")

# Output log setting; this is for the framework in general
# You may over-ride this in your job options for your algorithm
rec.OutputLevel.set_Value_and_Lock(INFO);

# Write settings; keep all of these to false.
# Control the writing of your own n-tuple in the alg's job options
rec.doCBNT.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock (False)
rec.doAOD.set_Value_and_Lock (False)

rec.doWriteESD.set_Value_and_Lock (False)
rec.doWriteAOD.set_Value_and_Lock (False)
rec.doWriteTAG.set_Value_and_Lock (False)
rec.doHist.set_Value_and_Lock (False)

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

