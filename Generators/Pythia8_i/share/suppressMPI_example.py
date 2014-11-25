# example JO for using the SuppressMPI plugin
# For testing only - not validated for production yet!!

evgenConfig.description = "Example QCD jets with MPI suppressed with UserHook"
evgenConfig.keywords = ["QCD", "MPI"]
include("MC12JobOptions/Pythia8_AU2_CTEQ6L1_Common.py")

topAlg.Pythia8.UserParams += ["SuppressMPI:PTCut=20"]

topAlg.Pythia8.Commands += ["HardQCD:all=on",
                            "PhaseSpace:pTHatMin=60"]
topAlg.Pythia8.UserHook = "SuppressMPI"


