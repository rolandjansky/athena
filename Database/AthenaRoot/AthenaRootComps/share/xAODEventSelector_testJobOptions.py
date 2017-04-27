

#this joboption is used in performance monitoring to compare to POOL

theApp.EvtMax = -1

import os

jps.AthenaCommonFlags.FilesInput = [os.environ.get("ASG_TEST_FILE_MC","/afs/cern.ch/user/a/asgbase/patspace/xAODs/r7725/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915862._000100.pool.root.1")]



import AthenaRootComps.ReadAthenaxAODHybrid


algseq = CfgMgr.AthSequencer("AthAlgSeq")
algseq += CfgMgr.EventCounterAlg(BookkeepOtherMCEventWeights=True)
