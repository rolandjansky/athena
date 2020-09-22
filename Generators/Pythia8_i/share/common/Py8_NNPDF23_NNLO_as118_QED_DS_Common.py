## Config for Py8 with NNPDF23 QED pdf

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
  "PDF:pSet= LHAPDF6:NNPDF23_nnlo_as_0118_qed",
    "SpaceShower:rapidityOrder = on",
    "SigmaProcess:alphaSvalue = 0.140",
    "SpaceShower:pT0Ref = 1.56",
    "SpaceShower:pTmaxFudge = 0.91",
    "BeamRemnants:unresolvedHadron=2",
    "SpaceShower:pTdampFudge = 1.05",
    "SpaceShower:alphaSvalue = 0.127",
    "TimeShower:alphaSvalue = 0.127",
    "SpaceShower:pTdampMatch = 1",
    "BeamRemnants:primordialKThard = 0.1",
    "BeamRemnants:primordialKTsoft=0.1",
    "PartonLevel:MPI = off",
    "PartonLevel:ISR = on",
    "SpaceShower:pTmaxMatch = 1",
    "SpaceShower:pTmaxFudge = 1.",
    "TimeShower:pTmaxMatch = 1",
    "TimeShower:pTmaxFudge = 1.",
    "SpaceShower:MEcorrections = off",
     "TimeShower:MEcorrections = off",
     "TimeShower:globalRecoil = on",
     "TimeShower:weightGluonToQuark = 1", 
     "BeamRemnants:primordialKTremnant = 0.1"            
]

evgenConfig.tune = "NNPDF23_QED"

#EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")

