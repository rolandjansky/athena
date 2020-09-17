# example JO for using the EnhanceMPI plugin
# For testing only - not validated for production yet!!

evgenConfig.description = "Example QCD jets with MPI enhanced with UserHook"
evgenConfig.keywords = ["QCD", "MPI"]
include("MC12JobOptions/Pythia8_AU2_CTEQ6L1_Common.py")

topAlg.Pythia8.UserParams += ["EnhanceMPI:PTCut=20"]

topAlg.Pythia8.Commands += ["HardQCD:all=on",
                            "PhaseSpace:pTHatMin=60"]
topAlg.Pythia8.UserHook = "EnhanceMPI"


