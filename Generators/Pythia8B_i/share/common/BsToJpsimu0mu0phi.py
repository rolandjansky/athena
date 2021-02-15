##############################################################
# BsToJpsimu0mu0phi.py
# Job options for Pythia8B_i generation of Bs->J/psi(mu0mu)phi(KK)   
# Must be run via Generate_trf.py
##############################################################

# General evgen config options
from Pythia8B_i.Pythia8B_iConf import Pythia8B_i
topAlg+=Pythia8B_i("Pythia8B")
evgenConfig.generators += ["Pythia8B"]
evgenConfig.description = "Signal Bs->J/psi(mu0mu)phi(KK)"
evgenConfig.keywords = ["BPHYS"]
evgenConfig.minevents = 10

# Hard process
topAlg.Pythia8B.Commands += ['HardQCD:all = on'] # Uncomment for MSEL1
#topAlg.Pythia8B.Commands += ['HardQCD:gg2bbbar = on'] # Uncomment for MSEL5
#topAlg.Pythia8B.Commands += ['HardQCD:qqbar2bbbar = on'] # Uncomment for MSEL5
topAlg.Pythia8B.Commands += ['PhaseSpace:pTHatMin = 8.'] # Equivalent of CKIN3    
topAlg.Pythia8B.Commands += ['ParticleDecays:mixB = off']
topAlg.Pythia8B.Commands += ['HadronLevel:all = off']

# Quark cuts
topAlg.Pythia8B.SelectBQuarks = True
topAlg.Pythia8B.SelectCQuarks = False
topAlg.Pythia8B.QuarkPtCut = 0.0
topAlg.Pythia8B.AntiQuarkPtCut = 6.0
topAlg.Pythia8B.QuarkEtaCut = 102.5
topAlg.Pythia8B.AntiQuarkEtaCut = 3.5
topAlg.Pythia8B.RequireBothQuarksPassCuts = True
topAlg.Pythia8B.VetoDoubleBEvents = True

# Job options for closing B decays 
include ("Pythia8B_i/CloseBDecays.py")

# Signal decay (Bs->J/psi phi)
topAlg.Pythia8B.Commands += ['531:87:onMode = on']

# Close all J/psi decays apart from J/psi->mumu
topAlg.Pythia8B.Commands += ['443:onMode = off']
topAlg.Pythia8B.Commands += ['443:2:onMode = on']

# Signal topology - only events containing this sequence will be accepted 
topAlg.Pythia8B.SignalPDGCodes = [531,443,-13,13,333,321,-321]
#topAlg.Pythia8B.SignalPtCuts = [0.0,0.0,2.5,2.5,0.0,0.0,0.0]
#topAlg.Pythia8B.SignalEtaCuts = [102.5,102.5,2.5,2.5,102.5,102.5,102.5]
#topAlg.Pythia8B.UserSelection = 'BJPSIPHI_TRANS'

# Number of repeat-hadronization loops
topAlg.Pythia8B.NHadronizationLoops = 6 

# List of B-species - for counting purposes (no effect on generation)
include("Pythia8B_i/BPDGCodes.py")

# Final state selections
topAlg.Pythia8B.TriggerPDGCode = 13
topAlg.Pythia8B.TriggerStatePtCut = [0.0]
topAlg.Pythia8B.TriggerStateEtaCut = 102.5
topAlg.Pythia8B.MinimumCountPerCut = [1]

#Uncomment for DumpMC print-out
#from TruthExamples.TruthExamplesConf import DumpMC
#topAlg += DumpMC()
