# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# FastPileup.py - set up fast pileup emulation using in-time generator
# Richard Hawkings, Vladimir Lyubushkin, 23/4/15

# modified on 4/9/15 from MC15 joboptions file
# MC15.361037.Pythia8_A2MSTW2008LO_minbias_inelastic_low.py
# but without filter to remove highpT component

if 'pileUpProfile' not in dir():
    fast_chain_log.info("Configuring default pileup profile for MC12")
    include("FastChainPileup/FastPileup_mc12_v2.py")
else:
    fast_chain_log.info("Using previously configured pileup profile")

fast_chain_log.info("Configuring fast chain pileup")

# setup event generation sequence
gen_pu='GEN_EVENT_PU'
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
import AthenaCommon.AppMgr as acam
acam.athMasterSeq+=AlgSequence('EvgenGenSeq')
genSeq=acam.athMasterSeq.EvgenGenSeq

# setup Pythia8 to run multiple times per event
from EvgenJobTransforms.EvgenConfig import evgenConfig
# following copied from MC15.361037.Pythia8_A2MSTW2008LO_minbias_inelastic_low.py
# but modified to use the MultiPy8Pileup as the algorithm

evgenConfig.description = "Inelastic minimum bias, with the A2 MSTW2008LO tune"
evgenConfig.keywords = ["QCD", "minBias","SM"]

# following copied from nonStandard/Pythia8/Pythia8_A2_MSTW2008LO_Common.py
# modified to use MultiPy8Pileup

from FastChainPileup.FastChain_jobProperties import FastChain_Flags

from FastChainPileup.FastChainPileupConf import MultiPy8Pileup
genSeq += MultiPy8Pileup("Pythia8")
evgenConfig.generators += ["Pythia8"]

# base configuration from common/Pythia8/Pythia8_Base_Fragment.py
genSeq.Pythia8.Commands += [
    "Main:timesAllowErrors = 500",
    "6:m0 = 172.5",
    "23:m0 = 91.1876",
    "23:mWidth = 2.4952",
    "24:m0 = 80.399",
    "24:mWidth = 2.085",
    "StandardModel:sin2thetaW = 0.23113",
    "StandardModel:sin2thetaWbar = 0.23146",
    "ParticleDecays:limitTau0 = on",
    "ParticleDecays:tau0Max = 10.0"]

# nonStandard/Pythia8_A2_MSTW2008LO_Common.py configuration
genSeq.Pythia8.Commands += [
    "Tune:pp = 5",
    "PDF:pSet=LHAPDF6:MSTW2008lo68cl",
    "MultipartonInteractions:bProfile = 4",
    "MultipartonInteractions:a1 = 0.03",
    "MultipartonInteractions:pT0Ref = 1.90",
    "MultipartonInteractions:ecmPow = 0.30",
    "ColourReconnection:range = 2.28",
    "SpaceShower:rapidityOrder=0"]
evgenConfig.tune = "A2 MSTW2008LO"

# MC15.361067 specific configuration
genSeq.Pythia8.Commands += ["SoftQCD:inelastic = on"]

## end of copies from Pythia initialisation files

genSeq.Pythia8.CollisionEnergy=13000. # how to set this automatically?

from EvgenProdTools.EvgenProdToolsConf import FixHepMC
genSeq+=FixHepMC(McEventKey=gen_pu,McEventsR=gen_pu,McEventsRW=gen_pu,OutputLevel=INFO)

genSeq.Pythia8.McEventKey=gen_pu
genSeq.Pythia8.McEventsR=gen_pu
genSeq.Pythia8.McEventsRW=gen_pu
# genSeq.Pythia8.OutputLevel=DEBUG

genSeq.Pythia8.NCollPerEvent=-1
genSeq.Pythia8.PileupProfile=pileUpProfile
#genSeq.Pythia8.MultBCID=[1.]
genSeq.Pythia8.MultBCID=FastChain_Flags.FastChainBCID()
# genSeq.Pythia8.HistFile='profile.root'

fast_chain_log.info("Check FastChainBCID Value");
fast_chain_log.info(FastChain_Flags.FastChainBCID.get_Value())


randomSeed=0
if hasattr(runArgs, "randomSeed"):
    randomSeed=runArgs.randomSeed

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc("AtRndmGenSvc")
ServiceMgr.AtRndmGenSvc.Seeds = [ "PYTHIA8 OFFSET " + str(randomSeed) + " 14789899 989240512", "PYTHIA8_INIT 820021 2347532" ]

topSeq+=genSeq
