##############################################################
# bb->J/psi(mu4mu4)X  
##############################################################

# General evgen config options
from Pythia8B_i.Pythia8B_iConf import Pythia8B_i
topAlg+=Pythia8B_i("Pythia8B")
evgenConfig.generators += ["Pythia8B"]
evgenConfig.description = "Inclusive bb->J/psi(mu4mu4) production"
evgenConfig.keywords = ["BPHYS"]
evgenConfig.minevents = 500

# Hard process
topAlg.Pythia8B.Commands += ['HardQCD:all = on'] # Uncomment for MSEL1
#topAlg.Pythia8B.Commands += ['HardQCD:gg2bbbar = on'] # Uncomment for MSEL5
#topAlg.Pythia8B.Commands += ['HardQCD:qqbar2bbbar = on'] # Uncomment for MSEL5
topAlg.Pythia8B.Commands += ['PhaseSpace:pTHatMin = 6.'] # Equivalent of CKIN3
topAlg.Pythia8B.Commands += ['ParticleDecays:mixB = off']
topAlg.Pythia8B.Commands += ['HadronLevel:all = off']

# Quark cuts
topAlg.Pythia8B.SelectBQuarks = True
topAlg.Pythia8B.SelectCQuarks = False
topAlg.Pythia8B.QuarkPtCut = 0.0
topAlg.Pythia8B.AntiQuarkPtCut = 4.0
topAlg.Pythia8B.QuarkEtaCut = 102.5
topAlg.Pythia8B.AntiQuarkEtaCut = 2.5
topAlg.Pythia8B.RequireBothQuarksPassCuts = True
topAlg.Pythia8B.VetoDoubleBEvents = True
topAlg.Pythia8B.UserSelection = 'BJPSIINCLUSIVE'

# Job options for closing (anti)B decays but opening B(antiB) decays
include ("Pythia8B_i/CloseBDecays.py")
#include ("Pythia8B_i/CloseAntiBDecays.py")

# Job options for opening inclusive B->J/psi decays
include ("Pythia8B_i/OpenBJpsiDecays.py")

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
topAlg.Pythia8B.TriggerStatePtCut = [4.0]
topAlg.Pythia8B.TriggerStateEtaCut = [2.5]
topAlg.Pythia8B.MinimumCountPerCut = [2]


#Uncomment for DumpMC print-out
#from TruthExamples.TruthExamplesConf import DumpMC
#topAlg += DumpMC()
