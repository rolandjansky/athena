from AthenaCommon import Logging
logger = Logging.logging.getLogger("Superchic_i")

## Shower config for Pythia8 with Superchic for elastic production

from Pythia8_i.Pythia8_iConf import Pythia8_i
genSeq += Pythia8_i("Pythia8")
evgenConfig.generators += ["Pythia8"]

## Control storing LHE in the HepMC record
if "StoreLHE" in genSeq.Pythia8.__slots__.keys():
    print "Pythia8_Base_Fragment.py: DISABLING storage of LHE record in HepMC by default. Please re-enable storage if desired"
    genSeq.Pythia8.StoreLHE = False

genSeq.Pythia8.LHEFile = genSeq.SuperChicConfig.outputLHEFile()
genSeq.Pythia8.CollisionEnergy = int(runArgs.ecmEnergy)

testSeq.TestHepMC.MaxTransVtxDisp = 1000000
testSeq.TestHepMC.MaxVtxDisp      = 1000000000
