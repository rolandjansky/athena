## Configure Pythia 8 to shower PoWHEG input using Main31 shower veto
include("Pythia8_i/Pythia8_Powheg.py")
genSeq.Pythia8.Commands += [ 'SpaceShower:pTmaxMatch = 2',
                             'TimeShower:pTmaxMatch = 2'  ]

if "UserHooks" in genSeq.Pythia8.__slots__.keys():
  genSeq.Pythia8.UserHooks += ['PowhegMain31']
  genSeq.Pythia8.Commands += ['Powheg:veto = 1']
else:
  genSeq.Pythia8.UserHook = 'Main31'


include("Pythia8_i/Pythia8_ShowerWeights.py")
