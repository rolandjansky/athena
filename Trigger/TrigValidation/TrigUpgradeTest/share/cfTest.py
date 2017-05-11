###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Configure the scheduler
from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.ShowDataFlow=True
svcMgr.ForwardSchedulerSvc.ShowControlFlow=True

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

def parOR(name, subs=[]):
    seq = AthSequencer( name )
    seq.ModeOR = True
    seq.Sequential = False
    seq.StopOverride = True
    for s in subs:
        seq += s
    return seq
    
def seqAND(name, subs=[]):
    seq = AthSequencer( name )
    seq.ModeOR = False
    seq.Sequential = True
    seq.StopOverride = False
    for s in subs:
        seq += s
    return seq

def stepSeq(name, filterAlg, rest):
    stepReco = parOR(name+"_reco", rest)
    stepAnd = seqAND(name, [ filterAlg, stepReco ])
    return stepAnd



TopHLTSeq = seqAND("TopHLTSeq")
job += TopHLTSeq

L1UnpackingSeq = parOR("L1UnpackingSeq")
TopHLTSeq += L1UnpackingSeq

steps = [ parOR("step%i" % i) for i in range(5)]
HLTChainsSeq  = seqAND("HLTChainsSeq", steps)
TopHLTSeq += HLTChainsSeq # in principle we do not need the HLTChainsSeq 



from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
def reco(name):
    return HLTTest__TestRecoAlg("R_"+name, OutputLevel = DEBUG)

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
def hypo(name):
    return HLTTest__TestHypoAlg("H_"+name, OutputLevel = DEBUG)

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
def seqFilter(name):
    return HLTTest__TestRoRSeqFilter("F_"+name, OutputLevel = DEBUG)

stepNo = 0
steps[stepNo] += seqFilter("Step1MU")
steps[stepNo] += seqFilter("Step1MU_E")
steps[stepNo] += seqFilter("Step1EM")
steps[stepNo] += seqFilter("Step2MU")

stepNo += 1 
mu1 = stepSeq("mu1", seqFilter("Step1MU"), [ reco("MURoIs"), reco("muMSRecAlg"), hypo("Step1MuHypo") ] )
steps[stepNo] += mu1

mue1 = stepSeq("mue1", seqFilter("Step1MU_E"), [ reco("CaloClustering"), reco("muMSRecAlg"), hypo("Step1MuEHypo") ])
steps[stepNo] += mue1

em1 = stepSeq("em1", seqFilter("Step1EM"), [ reco("EMRoIs"), reco("CaloClustering"), hypo("Step1ElGamHypo") ])
steps[stepNo] += em1

stepNo += 1
steps[stepNo] += seqFilter("Step2MU")
steps[stepNo] += seqFilter("Step2MU_E")
steps[stepNo] += seqFilter("Step2E")
steps[stepNo] += seqFilter("Step2G")

stepNo += 1
mu2 = stepSeq("mu2", seqFilter("Step2MU"), [ reco("TrkRoIs"), reco("TrigFastTrackFinder"),  reco("MuonRecAlg"), hypo("Step2MuHypo") ] )
steps[stepNo] += mu2

mue2 = stepSeq("mue2", seqFilter("Step2MU_E"), [ reco("TrkRoIs"), reco("TrigFastTrackFinder"), reco("MuonRecAlg"),  reco("ElectronRecAlg"), hypo("Step2MuEHypo") ])
steps[stepNo] += mue2

e2 = stepSeq( "e2" , seqFilter("Step2E"), [ reco("TrkRoIs"), reco("TrigFastTrackFinder"), reco("ElectronRecAlg"),hypo("Step2EHypo") ] )
steps[stepNo] += e2

g2 = stepSeq("g2", seqFilter("Step2G"), [ reco("GRoIs"), reco("PhotonRecAlg"), hypo("Step2PhotonHypo") ])
steps[stepNo] += g2
stepNo += 1

theApp.EvtMax = 5




