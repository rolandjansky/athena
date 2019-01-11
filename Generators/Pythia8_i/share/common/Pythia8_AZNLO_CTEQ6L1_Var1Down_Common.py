## Config for Py8 tune AZNLO with CTEQ6L1
include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
        "Tune:pp = 5",
#        "PDF:useLHAPDF = on",
        "BeamRemnants:primordialKThard = 1.780",
        "SpaceShower:alphaSorder = 2",
        "SpaceShower:alphaSvalue = 0.118",
        "SpaceShower:pT0Ref = 1.928",
        "MultipartonInteractions:pT0Ref = 2.002887"
        ]

# Deal with cteq6l1/ll pdf naming in Pythia8 release
rel = os.popen("echo $AtlasVersion").read()
print "Atlas release " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
  if 'Pythia8-01' in ver[:50]:
    genSeq.Pythia8.Commands += [
      "PDF:useLHAPDF = on",
      "PDF:LHAPDFset = cteq6ll.LHpdf"
      ]
  else:
    genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:cteq6l1"]
else:
  genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:cteq6l1"]

evgenConfig.tune = "AZNLO CTEQ6L1"

# needs Pythia8 Main31 matching
include('Pythia8_i/Pythia8_Powheg_Main31.py')

genSeq.Pythia8.UserModes += ['Main31:NFinal = 1',
                             'Main31:pTHard = 0',
                             'Main31:pTdef = 2'
                             ]
