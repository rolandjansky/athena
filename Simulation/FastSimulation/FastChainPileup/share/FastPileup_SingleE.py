# FastPileup.py - set up fast pileup emulation using in-time generator
# Olivier Arnaez, 17/12/15

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

from EvgenJobTransforms.EvgenConfig import evgenConfig

evgenConfig.description = "Single electron generator"
evgenConfig.keywords = ["SM"]

from FastChainPileup.FastChainPileupConf import MultiParticleGunPileup
genSeq += MultiParticleGunPileup("PartGun")
evgenConfig.generators += ["ParticleGun"]

from EvgenProdTools.EvgenProdToolsConf import FixHepMC
genSeq+=FixHepMC(McEventKey=gen_pu,McEventsR=gen_pu,OutputLevel=INFO)

genSeq.PartGun.McEventKey=gen_pu
genSeq.PartGun.McEventsR=gen_pu
# genSeq.PartGun.OutputLevel=DEBUG

genSeq.PartGun.NCollPerEvent=-1
genSeq.PartGun.PileupProfile=pileUpProfile
genSeq.PartGun.MultBCID=[1.]
# genSeq.PartGun.HistFile='profile.root'

randomSeed=0
if hasattr(runArgs, "randomSeed"):
    randomSeed=runArgs.randomSeed

from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc("AtRndmGenSvc")
ServiceMgr.AtRndmGenSvc.Seeds = [ "PYTHIA8 OFFSET " + str(randomSeed) + " 14789899 989240512", "ParticleGun 820021 2347532" ]

topSeq+=genSeq
