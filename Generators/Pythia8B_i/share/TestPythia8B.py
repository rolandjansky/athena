##############################################################
# TestPythia8B.py
# Example job options for Pythia8B  
# Must be run via Generate_trf.py
# Generate_trf.py maxEvents=-1 ecmEnergy=7000.0 firstEvent=1 randomSeed=0 jobConfig=./TestPythia8B.py outputEVNTFile=evnt.pool.root runNumber=999999  
# To run in debug mode:
# Generate_trf.py --athenaopts="--loglevel DEBUG" maxEvents=-1 ecmEnergy=7000.0 firstEvent=1 randomSeed=0 jobConfig=./TestPythia8B.py outputEVNTFile=evnt.pool.root runNumber=999999 postExec='ServiceMgr.MessageSvc.OutputLevel=DEBUG',,'ServiceMgr.MessageSvc.enableSuppression=False'
##############################################################

# General evgen config options
from Pythia8B_i.Pythia8B_iConf import Pythia8B_i
topAlg+=Pythia8B_i("Pythia8B")
evgenConfig.generators += ["Pythia8B"]
evgenConfig.description = "Sample job options for Pythia8B generation"
evgenConfig.keywords = ["BPHYS"]
evgenConfig.minevents = 5

# Hard process
#topAlg.Pythia8B.Commands += ['HardQCD:all = on'] # Uncomment for MSEL1
topAlg.Pythia8B.Commands += ['HardQCD:gg2bbbar = on'] # Uncomment for MSEL5
topAlg.Pythia8B.Commands += ['HardQCD:qqbar2bbbar = on'] # Uncomment for MSEL5
topAlg.Pythia8B.Commands += ['PhaseSpace:pTHatMin = 7.'] # Equivalent of CKIN3    
topAlg.Pythia8B.Commands += ['ParticleDecays:mixB = off']
topAlg.Pythia8B.Commands += ['HadronLevel:all = off']

# Quarkonia production mode
# This should be used for direct charmonium or bottomonium production
#topAlg.Pythia8B.Commands += ['Charmonium:all = on']
#topAlg.Pythia8B.Commands += ['Bottomonium:all = on']
# IMPORTANT: If you run in this mode you MUST also do the following
# (1) HardQCD:all = on
# (2) Switch off quark selection
# (3) Set NHadronizationLoops to 1
# The final state selections can be modified as required

# Quark cuts
topAlg.Pythia8B.SelectBQuarks = True
topAlg.Pythia8B.SelectCQuarks = False
topAlg.Pythia8B.QuarkPtCut = 0.0
topAlg.Pythia8B.AntiQuarkPtCut = 2.5
topAlg.Pythia8B.QuarkEtaCut = 102.5
topAlg.Pythia8B.AntiQuarkEtaCut = 2.5
topAlg.Pythia8B.RequireBothQuarksPassCuts = True
topAlg.Pythia8B.VetoDoubleBEvents = True

# Job options for closing (anti)B decays but opening B(antiB) decays
include ("Pythia8B_i/CloseBDecays.py")
#include ("Pythia8B_i/CloseAntiBDecays.py")

# Job options for opening inclusive B->J/psi decays
#include ("Pythia8B_i/OpenBJpsiDecays.py")

# Example of setting a signal decay (Bs->J/psi phi)
topAlg.Pythia8B.Commands += ['531:87:onMode = on']

# Close all J/psi decays apart from J/psi->mumu
topAlg.Pythia8B.Commands += ['443:onMode = off']
topAlg.Pythia8B.Commands += ['443:2:onMode = on']

# Signal topology - only events containing this sequence will be accepted 
topAlg.Pythia8B.SignalPDGCodes = [531,443,-13,13,333,321,-321]

# Number of repeat-hadronization loops
topAlg.Pythia8B.NHadronizationLoops = 10

# List of B-species - for counting purposes (no effect on generation)
include("Pythia8B_i/BPDGCodes.py")

# Final state selections
topAlg.Pythia8B.TriggerPDGCode = 13
topAlg.Pythia8B.TriggerStatePtCut = [6.0,4.0]
topAlg.Pythia8B.TriggerStateEtaCut = [2.5]
topAlg.Pythia8B.MinimumCountPerCut = [1,2]

#Uncomment for DumpMC print-out
#from TruthExamples.TruthExamplesConf import DumpMC
#topAlg += DumpMC()
