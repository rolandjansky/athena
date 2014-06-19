## Config for Py8 tune AU2 with CTEQ6L1
#include("MC12JobOptions/Pythia8_Base_Fragment.py")
include("common/Pythia8_Base_Fragment.py")

topAlg.Pythia8.Commands += [
    "Tune:pp = 5",
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = cteq6ll.LHpdf",
    "MultipartonInteractions:bProfile = 4",
    "MultipartonInteractions:a1 = 0.00",
    "MultipartonInteractions:pT0Ref = 2.13",
    "MultipartonInteractions:ecmPow = 0.21",
    "BeamRemnants:reconnectRange = 2.21",
    "SpaceShower:rapidityOrder=0"]
evgenConfig.tune = "AU2 CTEQ6L1"
