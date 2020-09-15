# Common fragment for generation of 1 Pythia8 event, when producing LHE output only
# the number of events is set to 1 in two places: as Pythia param: Main:numberOfEvents = 1 and postSeq.CountHepMC.RequestedOutput=1 (the later one is stronger)
# It is needed to make the transform run
# we don't modify nEventsPerJob anymore, which now corresponds to the number of events in the output lhe file
# No output is kept from this generation
## Base config for Pythia8

from __main__ import opts
import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts

from Pythia8_i.Pythia8_iConf import Pythia8_i
genSeq += Pythia8_i("Pythia8")

genSeq.Pythia8.Commands += [
    "Main:timesAllowErrors = 500",
    "Main:numberOfEvents = 1",### first place for number of dummy events
    "6:m0 = 172.5",
    "23:m0 = 91.1876",
    "23:mWidth = 2.4952",
    "24:m0 = 80.399",
    "24:mWidth = 2.085",
    "StandardModel:sin2thetaW = 0.23113",
    "StandardModel:sin2thetaWbar = 0.23146",
    "ParticleDecays:limitTau0 = on",
    "ParticleDecays:tau0Max = 10.0"]

genSeq.Pythia8.Commands += [
    "Tune:pp = 5",
    "MultipartonInteractions:bProfile = 4",
    "MultipartonInteractions:a1 = 0.03",
    "MultipartonInteractions:pT0Ref = 1.90",
    "MultipartonInteractions:ecmPow = 0.30",
    "PDF:pSet=LHAPDF6:MSTW2008lo68cl",
    "ColourReconnection:range=2.28",
    "SoftQCD:inelastic = on",
    "SpaceShower:rapidityOrder=0"]

### second place for number of dummy events
if not hasattr(postSeq, "CountHepMC"):
    postSeq += CountHepMC()
postSeq.CountHepMC.RequestedOutput = 1

### we don't modify nEventsPerJob, it'll correspond to the number of events in the output lhe file
#evgenConfig.nEventsPerJob = 1

### deleting TestHepMC for lhe-only production
del testSeq.TestHepMC




