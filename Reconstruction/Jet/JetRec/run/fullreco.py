# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# fullreco.py

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
try:
  eosdir=os.environ["EOSDIR"]
except:
  eosdir="root://eosatlas//eos"
if 1:
  # Standard ttbar, 14 TeV, mu=40 sample
  # For ESD and AOD see /afs/cern.ch/work/k/krasznaa/public/xAOD/devval_rel_4
  infile = eosdir + "/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000001.pool.root.1"
elif 0:
  # 28Mar2014: Copied from /afs/cern.ch/work/k/krasznaa/public/xAOD/devval_rel_4/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.devval_rel_3.pool.root
  infile = "/afs/cern.ch/user/d/dadams/pubdata/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.devval_rel_3.pool.root"
else:
  infile = "/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root"

athenaCommonFlags.FilesInput = [infile]

athenaCommonFlags.EvtMax = 10

from RecExConfig.RecFlags import rec
rec.doFloatingPointException = True         # To crash on FPE
#rec.doTau=False
rec.doTrigger=False

if 0:
  # Disable jets.
  rec.doJetMissingETTag = False

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

from JetRec.JetRecFlags import jetFlags
#jetFlags.useTruth = False
#jetFlags.useTopo = False
#jetFlags.useTracks = False
#jetFlags.useMuonSegments = False
#jetFlags.usePflow = False
#jetFlags.applyCalibrationName = "none"
#jetFlags.applyCalibrationName = "jes"
#jetFlags.skipTools = ["comshapes"]
#jetFlags.debug = 3
#jetFlags.additionalTopoGetters += ["gakt3trackget", "gakt4trackget"]
jetFlags.timeJetToolRunner = 2
jetFlags.timeJetRecTool = 2

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
