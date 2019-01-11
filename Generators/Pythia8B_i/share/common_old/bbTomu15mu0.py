##############################################################
# bb->mu15mu0X  
##############################################################

# General evgen config options
from Pythia8B_i.Pythia8B_iConf import Pythia8B_i
topAlg+=Pythia8B_i("Pythia8B")
evgenConfig.generators += ["Pythia8B"]
evgenConfig.description = "Inclusive bb->mu15mu0 production"
evgenConfig.keywords = ["BPHYS"]
evgenConfig.minevents = 50

# Hard process
topAlg.Pythia8B.Commands += ['HardQCD:all = on'] # Uncomment for MSEL1
#topAlg.Pythia8B.Commands += ['HardQCD:gg2bbbar = on'] # Uncomment for MSEL5
#topAlg.Pythia8B.Commands += ['HardQCD:qqbar2bbbar = on'] # Uncomment for MSEL5
topAlg.Pythia8B.Commands += ['PhaseSpace:pTHatMin = 7.']    
topAlg.Pythia8B.Commands += ['ParticleDecays:mixB = off']
topAlg.Pythia8B.Commands += ['HadronLevel:all = off']

# Quark cuts
topAlg.Pythia8B.SelectBQuarks = True
topAlg.Pythia8B.SelectCQuarks = False
topAlg.Pythia8B.QuarkPtCut = 7.0
topAlg.Pythia8B.AntiQuarkPtCut = 7.0
topAlg.Pythia8B.QuarkEtaCut = 4.5
topAlg.Pythia8B.AntiQuarkEtaCut = 4.5
topAlg.Pythia8B.RequireBothQuarksPassCuts = False
topAlg.Pythia8B.VetoDoubleBEvents = True

# Number of repeat-hadronization loops
topAlg.Pythia8B.NHadronizationLoops = 10

# List of B-species - for counting purposes (no effect on generation)
include("Pythia8B_i/BPDGCodes.py")

# Final state selections
topAlg.Pythia8B.TriggerPDGCode = 13
topAlg.Pythia8B.TriggerStatePtCut = [15.0,0.0]
topAlg.Pythia8B.TriggerStateEtaCut = [2.5]
topAlg.Pythia8B.MinimumCountPerCut = [1,2]


#Uncomment for DumpMC print-out
#from TruthExamples.TruthExamplesConf import DumpMC
#topAlg += DumpMC()
