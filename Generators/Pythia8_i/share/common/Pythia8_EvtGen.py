## Run EvtGen afterburner on top of Pythia 8
assert hasattr(genSeq, "Pythia8")
include("EvtGen_i/EvtGen_Fragment.py")
evgenConfig.auxfiles += ['inclusiveP8DsDPlus.pdt']
#genSeq.EvtInclusiveDecay.pdtFile = "inclusiveP8.pdt"
genSeq.EvtInclusiveDecay.pdtFile = "inclusiveP8DsDPlus.pdt"

# FHerwig has problems with omega b* (5334), so not present in the base EvtGen fragment.  Add it here.
genSeq.EvtInclusiveDecay.whiteList+=[-5334, 5334]


