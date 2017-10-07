## Example job option script to run an event generator with FixHepMC filtering
## Author: Andy Buckley <andy.buckley@cern.ch>

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

## Configure and add an event generator to the alg seq
from Pythia8_i.Pythia8_iConf import Pythia8_i
topAlg += Pythia8_i("Pythia8")
topAlg.Pythia8.CollisionEnergy = 7000.0
topAlg.Pythia8.Commands += ['HardQCD:all = on', 'PhaseSpace:pTHatMin = 30.0']

## Now include FixHepMC
include("EvgenProdTools/EnableFixHepMC.py")
#topAlg.FixHepMC.OutputLevel = DEBUG
