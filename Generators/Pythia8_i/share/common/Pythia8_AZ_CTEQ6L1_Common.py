## Config for Py8 tune AZ with CTEQ6L1
include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
#    "PDF:pSet=LHAPDF6:cteq6l1",
    "BeamRemnants:primordialKThard =  1.713",
    "SpaceShower:pT0Ref = 0.586",
    "SpaceShower:alphaSvalue = 0.12374",
    "MultipartonInteractions:pT0Ref = 2.18"]

rel = os.popen("echo $AtlasVersion").read()

genSeq.Pythia8.Commands +=  ["PDF:pSet=LHAPDF6:cteq6l1"]

evgenConfig.tune = "AZ CTEQ6L1"





