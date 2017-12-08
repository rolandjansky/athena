# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root"]

#attempting to get Zmumu file #1
#athenaCommonFlags.FilesInput=["root://eosatlas//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/e0/5c/RDO.06718162._000011.pool.root.1"]

#Zmumu file #2 (500 events)
#athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/64/3c/RDO.07497175._000001.pool.root.1"]

#ttbar file that also works on PRTT (6-17-16)
#athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/60/11/RDO.07497143._000001.pool.root.1"]

#new k_short sample (7-21-16)...
#athenaCommonFlags.FilesInput = ["../rootfile_storage/mc15_13TeV.422016.ParticleGun_single_K0s_PtFlat1_10.recon.RDO.e4459_s2726_r7059/RDO.07275539._000001.pool.root.1"]

#new photon conversion sample (8-18-16)
#athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch///eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/54/d2/RDO.06634780._000001.pool.root.1"]

#better photon conversion sample (10-3-16)
athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/e6/ca/RDO.07275499._000001.pool.root.1"]

#Control the number of events, -1 means "ALL"
athenaCommonFlags.EvtMax = -1

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

from RecExConfig.RecFlags import rec
rec.doTrigger=False

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSlimming.set_Value_and_Lock(False)          #7-21-16: normally False, set to True for testing
InDetFlags.doBackTracking.statusOn = True
InDetFlags.doTrtSegments.statusOn = True                 #8-8-16: new line added to enable backtracking
InDetFlags.doTRTStandalone.set_Value_and_Lock(False)      #10-3-16: Was False, now True
#from JetRec.JetRecFlags import jetFlags
#jetFlags.applyCalibration = False 

#Custom kalman vertex updator
#from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__KalmanVertexUpdator
#myVertexUpdator = Trk__KalmanVertexUpdator()
#ToolSvc += myVertexUpdator

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#ToolSvc.TileCellBuilder.maskBadChannels=False   #8-8-16: Commented out because it caused some sort of error

#Enable debug
#InDetVxFitterTool.OutputLevel=VERBOSE
#InDetPriVxFinderTool.OutputLevel=VERBOSE
