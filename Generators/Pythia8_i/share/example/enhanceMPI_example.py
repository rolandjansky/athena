# example JO for using the EnhanceMPI plugin
# For testing only - not validated for production yet!!

evgenConfig.description = "Example QCD jets with MPI enhanced with UserHook"
evgenConfig.keywords = ["QCD", "MPI"]
include('MC15JobOptions/nonStandard/Pythia8_A14_NNPDF23LO_Common.py')


genSeq.Pythia8.Commands += ["HardQCD:all=on",
                            "PhaseSpace:pTHatMin=60",
                            "EnhanceMPI:PTCut=20"]
genSeq.Pythia8.UserHooks += ["EnhanceMPI"]


