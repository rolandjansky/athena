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


data = {'noreco': [';', ';', ';']}  # in the lists there are the events
data['emclusters'] = ['eta:1,phi:1,et:2000; eta:1,phi:-1.2,et:3500;',
                      'eta:0.5,phi:0,et:1000; eta:1,phi:-1.2,et:3500;',
                      'eta:-0.6,phi:1.7,et:5000;']

for name, d in data.iteritems():
    with open(name+".dat", "w") as f:
        for event in d:
            f.write(event)
            f.write("\n")
        




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
def reco(name, FileName, Output):
    return HLTTest__TestRecoAlg("R_"+name, OutputLevel = DEBUG, FileName=FileName, Output=Output)

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
def hypo(name):
    return HLTTest__TestHypoAlg("H_"+name, OutputLevel = DEBUG)

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
allFilters={}
def seqFilter(name, Inputs=[], Outputs=[]):
    global allFilters
    f = HLTTest__TestRoRSeqFilter("F_"+name, OutputLevel = DEBUG, Inputs=Inputs, Outputs=Outputs)
    if "Step1" in name: # so that we see events running through, will be gone once L1 emulation is included
        f.AlwaysPass = True        
    allFilters[name] = f
    return f

def pick(name):
    global allFilters
    return allFilters[name]

stepNo = 0
steps[stepNo] += seqFilter( "Step1MU", Inputs=["L1MU"], Outputs=["step0MU"])
steps[stepNo] += seqFilter( "Step1MU_E", Inputs=["L1MU", "L1EM"], Outputs=["step0MU","step0EM"])
steps[stepNo] += seqFilter( "Step1EM", Inputs=["L1EM"], Outputs=["step0EM"])


stepNo += 1 
em1 = stepSeq( "em1", pick("Step1EM"), [ reco("EMRoIs", FileName="noreco.dat", Output="EMRoIs"),
                                         reco("CaloClustering", FileName="emclusters.dat", Output="EMClusters"),
                                         hypo("Step1ElGamHypo") ] )
steps[stepNo] += em1

# mu1 = stepSeq("mu1", pick("Step1MU"), [ reco("MURoIs", "noreco.dat"), reco("muMSRecAlg", "msmu.dat"), hypo("Step1MuHypo") ] )
# steps[stepNo] += mu1

# mue1 = stepSeq("mue1", pick("Step1MU_E"), [ pick("CaloClustering"), reco("muMSRecAlg", "msmu.dat"), hypo("Step1MuEHypo") ])
# steps[stepNo] += mue1


# stepNo += 1
# steps[stepNo] += seqFilter("Step2MU", Inputs=["step1MUDecisions"], Outputs=["step2MU"] )
# steps[stepNo] += seqFilter("Step2MU_E", Inputs=["step1MUEDecisions"], Outputs=["step2MUE"] )
# steps[stepNo] += seqFilter("Step2E", Inputs=["step1EDecisions"], Outputs=["step2E"] )
# steps[stepNo] += seqFilter("Step2G", Inputs=["step1GDecisions"], Outputs=["step2G"])

# stepNo += 1
# mu2 = stepSeq("mu2", pick("Step2MU"), [ reco("TrkRoIs", "noreco.dat"), reco("TrigFastTrackFinder", "tracks.dat"),  reco("MuonRecAlg", "mucomb.dat"), hypo("Step2MuHypo") ] )
# steps[stepNo] += mu2

# mue2 = stepSeq("mue2", pick("Step2MU_E"), [ pick("TrkRoIs", "noreco.dat"), pick("TrigFastTrackFinder"), reco("MuonRecAlg", "mucomb.dat"),  reco("ElectronRecAlg"), hypo("Step2MuEHypo") ])
# steps[stepNo] += mue2

# e2 = stepSeq( "e2" , pick("Step2E"), [ pick("TrkRoIs"), pick("TrigFastTrackFinder"), reco("ElectronRecAlg", "electrons.dat"),hypo("Step2EHypo") ] )
# steps[stepNo] += e2

# g2 = stepSeq("g2", pick("Step2G"), [ reco("GRoIs", "noreco.dat"), reco("PhotonRecAlg", "photons.dat"), hypo("Step2PhotonHypo") ])
# steps[stepNo] += g2
# stepNo += 1

theApp.EvtMax = 3




