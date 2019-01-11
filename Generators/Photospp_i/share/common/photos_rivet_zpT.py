evgenConfig.description = "Test of Pythia 8 + Photos++ in Z pT"
evgenConfig.keywords = ["QED", "Z"]

include("MC12JobOptions/Pythia8_AU2_CTEQ6L1_Common.py")
include("MC12JobOptions/PhotosPythia8_Fragment.py")

# Uncomment to turn exponentiation mode off
# Most physics channels should leave the default mode to on (i.e. leave this commented out)
#topAlg.Photospp.ExponentiationMode=False

topAlg.Pythia8.Commands += ["WeakSingleBoson:ffbar2gmZ=on"]
topAlg.Pythia8.Commands += ["23:onMode=off", "23:onIfAny=11"]

evgenConfig.minevents = 25000

from Rivet_i.Rivet_iConf import Rivet_i
rivet = Rivet_i("Rivet")
rivet.Analyses = ["ATLAS_2011_S9131140"]

topAlg += rivet

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["Rivet DATAFILE='Rivet.root' OPT='RECREATE'"]
