# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# @author Jon Burr
# adapted from GeneratorAlgorithmsTest_jobOptions.py

# Only test on 'mc'
import AthenaRootComps.ReadAthenaxAODHybrid
theApp.EvtMax = 500
svcMgr.EventSelector.InputCollections = [os.getenv("ASG_TEST_FILE_MC")]

from AsgAnalysisAlgorithms.AsgAnalysisAlgorithmsTest import makePileupSequence
algseq = makePileupSequence()
print(algseq)

athAlgSeq += algseq

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += [
    "ANALYSIS DATAFILE='PileupAlgorithmsTest.hist.root' OPT='RECREATE'"
]