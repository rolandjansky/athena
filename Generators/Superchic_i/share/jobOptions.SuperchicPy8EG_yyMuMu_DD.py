evgenConfig.description = "SuperChic4 MC gamma + gamma pp collisions at 13000 GeV to 2 muons with dissociation on both side"
evgenConfig.keywords = ["2photon","2muon","dissociation"]
#evgenConfig.weighting = 0
evgenConfig.contact = ["william.patrick.mccormack.iii@cern.ch"]

if not os.path.exists('inputs'):
    os.makedirs('inputs')
if not os.path.exists('evrecs'):
    os.makedirs('evrecs')
if not os.path.exists('outputs'):
    os.makedirs('outputs')

from Superchic_i.SuperChicUtils import *

#class with the superchic initialization parameters.  Please see SuperChicUtils for a complete list of tunable parameters.
Init = SuperChicConfig()

hasECM = hasattr(runArgs,"ecmEnergy")
if hasECM:
    ecm = int(runArgs.ecmEnergy)
    Init.rts = str(ecm)+"d0" # set the COM collision energy (in fortran syntax)
Init.isurv = "4"                  # Model of soft survival (from 1 -> 4, corresponding to arXiv:1306.2149)
Init.intag = "'in5'"              # for input files
Init.PDFname = "'MMHT2014lo68cl'" # PDF set name
Init.PDFmember = "0"              # PDF member
Init.proc = "57"                  # Process number (59 = gg->gg, 56 = gg->ee, 68 = gg->a->gg ); Please consult Superchic Manual https://superchic.hepforge.org/
Init.beam = "'prot'"              # Beam type ('prot', 'ion')
Init.outtg = "'out'"              # for output file name
Init.sfaci = ".true."             # Include soft survival effects
Init.diff = "'dd'"                # interaction: elastic ('el'), single ('sd','sda','sdb') and double ('dd') dissociation.
hasSeed = hasattr(runArgs,"randomSeed")
if hasSeed:
    Init.iseed  = str(int(runArgs.randomSeed))
Init.genunw  = ".true."
hasNev = hasattr(runArgs,"maxEvents")
if hasNev:
    Init.nev = str(runArgs.maxEvents)
Init.ymin  = "-2.4d0"              # Minimum object rapidity Y_X
Init.ymax  = "2.4d0"               # Maximum object rapidity Y_X
Init.mmin  = "6d0"                 # Minimum object mass M_X
Init.mmax  = "500d0"               # Maximum object mass M_X
Init.gencuts  = ".true."           # Generate cuts below
Init.ptxmax  = "100d0"         #cut on proton pt
Init.ptamin  = "3.0d0"             # Minimum pT of outgoing object a 
Init.ptbmin  = "3.0d0"             # Minimum pT of outgoing object b 
Init.etaamin  = "-2.4d0"           # Minimum eta of outgoing object a
Init.etaamax   = "2.4d0"           # Maximum eta of outgoing object a
Init.etabmin  = "-2.4d0"           # Minimum eta of outgoing object b
Init.etabmax   = "2.4d0"           # Maximum eta of outgoing object b
Init.acoabmax  = "100d0"


SuperChicRun(Init)


evgenConfig.generators += ["Pythia8"]

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
  "PDF:pSet= LHAPDF6:NNPDF23_nnlo_as_0118_qed",
    "SpaceShower:rapidityOrder = on",
    "SigmaProcess:alphaSvalue = 0.140",
    "SpaceShower:pT0Ref = 1.56",
    "SpaceShower:pTmaxFudge = 0.91",
    "SpaceShower:pTdampFudge = 1.05",
    "SpaceShower:alphaSvalue = 0.127",
    "TimeShower:alphaSvalue = 0.127",
    "SpaceShower:pTdampMatch = 1",
    "BeamRemnants:primordialKThard = 1.88",
    "PartonLevel:MPI = off",
    "PartonLevel:FSR = on",
    "SpaceShower:dipoleRecoil = on",
    "SpaceShower:pTmaxMatch = 2",
    "SpaceShower:QEDshowerByQ = off"
]

evgenConfig.tune = "NNPDF23_QED"

#EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")

include("Pythia8_i/Pythia8_ShowerWeights.py")

assert hasattr(genSeq, "Pythia8")
fileName = "evrecs/evrec"+Init.outtg[1:-1]+".dat"
genSeq.Pythia8.LHEFile = fileName
genSeq.Pythia8.CollisionEnergy = int(runArgs.ecmEnergy)
