##############################################################
# Common job options for direct Pythia8B charmonium production
##############################################################

# Hard process
genSeq.Pythia8B.Commands += ['PhaseSpace:pTHatMin = 1.'] # Equivalent of CKIN3
genSeq.Pythia8B.Commands += ['ParticleDecays:mixB = off']
genSeq.Pythia8B.Commands += ['HadronLevel:all = off']

# Quarkonia production mode
genSeq.Pythia8B.Commands += ['Charmonium:all = on']
genSeq.Pythia8B.Commands += ['PhaseSpace:pTHatMinDiverge = 0.5']
genSeq.Pythia8B.SuppressSmallPT = True
genSeq.Pythia8B.pT0timesMPI = 1.
genSeq.Pythia8B.numberAlphaS = 3.
genSeq.Pythia8B.useSameAlphaSasMPI = False
genSeq.Pythia8B.SelectBQuarks = False
genSeq.Pythia8B.SelectCQuarks = False
genSeq.Pythia8B.VetoDoubleBEvents = False
genSeq.Pythia8B.VetoDoubleCEvents = False

# Number of repeat-hadronization loops
genSeq.Pythia8B.NHadronizationLoops = 1

# List of B-species - for counting purposes (no effect on generation)
#include("Pythia8B_i/BPDGCodes.py") NB: Not working in MC15, is it a problem??

