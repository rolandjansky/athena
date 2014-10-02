# ---------------------------------------------------------------------- 
# JO file for RTT
# ---------------------------------------------------------------------- 

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

#from TruthExamples.TruthExamplesConf import *
from EvgenProdTools/EvgenProdToolsConfig import TestHepMC
from PyJobTransformsCore.runargs import *
runArgs = RunArguments()

outputFileName="RTT.MC12.108295.Herwigpp_UEEE3_CTEQ6L1_Zmumu.root"
print "Output file name: ", outputFileName

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg") 

from EvgenJobTransforms.EvgenConfig import *

# ----------------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()

# Add HepMCAnalysis algorithms
from HepMCAnalysis_i.HepMCAnalysis_iConfig import HepMCAnalysis_i

myHepMCAnalysis = HepMCAnalysis_i("HepMCAnalysis_i", file = outputFileName)
myHepMCAnalysis.JetAnalysis=True
myHepMCAnalysis.ZAnalysis=True
myHepMCAnalysis.EtmissAnalysis=True

topAlg += myHepMCAnalysis

