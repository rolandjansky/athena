## Config for Py8 tune AZNLO with CTEQ6L1
## changed to Pythia8.2 standard for pdfs, for Pythia8.1 was PDF:LHAPDFset = cteq6ll.LHpdf
include("Pythia8_i/Pythia8_Base_Fragment.py")
include('Pythia8_i/Pythia8_Powheg_Main31.py')

evgenConfig.tune = "AZNLO CTEQ6L1"

genSeq.Pythia8.Commands += [
    "Tune:pp = 5",
    "BeamRemnants:primordialKThard = 1.74948",
    "SpaceShower:alphaSorder = 2",
    "SpaceShower:alphaSvalue = 0.118",
    "SpaceShower:pT0Ref = 1.923589",
    "MultipartonInteractions:pT0Ref = 2.002887"
    ]



if "UserHooks" in genSeq.Pythia8.__slots__.keys():

  genSeq.Pythia8.Commands += ['Powheg:NFinal = 1',
                              'Powheg:pTHard = 0',
                              'Powheg:pTdef = 2',
                              "PDF:pSet=LHAPDF6:cteq6l1"]
else:

  genSeq.Pythia8.UserModes += ['Main31:NFinal = 1',
                               'Main31:pTHard = 0',
                               'Main31:pTdef = 2']
  
  genSeq.Pythia8.Commands +=  ["PDF:useLHAPDF = on",
                               "PDF:LHAPDFset = cteq6l1"]

