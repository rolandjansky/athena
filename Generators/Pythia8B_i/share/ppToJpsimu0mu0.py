##############################################################
# Job options fragment for pp->J/psi(mu0mu0)X  
##############################################################

# General evgen config options
from Pythia8B_i.Pythia8B_iConf import Pythia8B_i
topAlg+=Pythia8B_i("Pythia8B")
evgenConfig.generators += ["Pythia8B"]
evgenConfig.description = "Inclusive pp->J/psi(mu0mu0) production"
evgenConfig.keywords = ["BPHYS"]
evgenConfig.minevents = 5000

# Hard process
#topAlg.Pythia8B.Commands += ['HardQCD:all = on'] # Uncomment for MSEL1
#topAlg.Pythia8B.Commands += ['HardQCD:gg2bbbar = on'] # Uncomment for MSEL5
#topAlg.Pythia8B.Commands += ['HardQCD:qqbar2bbbar = on'] # Uncomment for MSEL5
topAlg.Pythia8B.Commands += ['PhaseSpace:pTHatMin = 1.']    
topAlg.Pythia8B.Commands += ['ParticleDecays:mixB = off']
topAlg.Pythia8B.Commands += ['HadronLevel:all = on']

# Quarkonia production mode
# This should be used for direct charmonium or bottomonium production
topAlg.Pythia8B.Commands += ['Charmonium:all = on']
#topAlg.Pythia8B.Commands += ['Bottomonium:all = on']
topAlg.Pythia8B.Commands += ['PhaseSpace:pTHatMinDiverge = 0.5']
topAlg.Pythia8B.SuppressSmallPT = True
topAlg.Pythia8B.pT0timesMPI = 1.
topAlg.Pythia8B.numberAlphaS = 3.
topAlg.Pythia8B.useSameAlphaSasMPI = False   
# IMPORTANT: If you run in this mode you MUST also do the following
# (1) HardQCD:all = off or comment out
# (2) Switch off quark selection
# (3) Set NHadronizationLoops to 1
# The final state selections can be modified as required

# Quark cuts
# Apply cuts to b- or c- quarks
# If both are set to true, event is accepted if either b- or c- pass the cuts
# This is to enable the assembly of mixed bb and cc samples
# If one is set to true, event is accepted or rejected on the basis of that quark
# flavour only - the other is ignored. If both are false, no quark cuts are applied 
topAlg.Pythia8B.SelectBQuarks = False
topAlg.Pythia8B.SelectCQuarks = False

# Close all J/psi decays apart from J/psi->mumu
topAlg.Pythia8B.Commands += ['443:onMode = off']
topAlg.Pythia8B.Commands += ['443:2:onMode = on']

# Signal topology - only events containing this sequence will be accepted 
topAlg.Pythia8B.SignalPDGCodes = [443,-13,13]

# Number of repeat-hadronization loops
topAlg.Pythia8B.NHadronizationLoops = 1

# List of B-species - for counting purposes (no effect on generation)
include("Pythia8B_i/BPDGCodes.py")

# Final state selections
topAlg.Pythia8B.TriggerPDGCode = 13
topAlg.Pythia8B.TriggerStatePtCut = [0.0]
topAlg.Pythia8B.TriggerStateEtaCut = [10.5]
topAlg.Pythia8B.MinimumCountPerCut = [2]

#Uncomment for DumpMC print-out
#from TruthExamples.TruthExamplesConf import DumpMC
#topAlg += DumpMC()
