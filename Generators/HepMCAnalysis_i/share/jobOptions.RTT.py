## ###############################################################
## #
## # JOB OPTIONS FILE.
## # for the run in RTT
## #==============================================================
## #--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

from TruthExamples.TruthExamplesConf import *
from PyJobTransformsCore.runargs import *
runArgs = RunArguments()

evtMax = 100000
try:
  evtMax = EvtMax
except NameError:
  pass

CMSEnergy = 7000.
try:
  CMSEnergy = ecmEnergy
except NameError:
  pass

import random
seedOffset = random.randint( 1, 100000000 )
try:
  seedOffset = SeedOffset        # some fixed value at the beginning 
except NameError:
  pass
try:
  seedOffset = SeedOffset0        # some fixed value at the beginning 
except NameError:
  pass

seedOffset1 = random.randint( 1, 100000000 )
try:
  seedOffset1 = SeedOffset1        # some fixed value at the beginning 
except NameError:
  pass

seedOffset2 = random.randint( 1, 100000000 )
try:
  seedOffset2 = SeedOffset2        # some fixed value at the beginning 
except NameError:
  pass


filePostfix = ""
try:
  postfix = Postfix        # some fixed value at the beginning 
  filePostfix = "_%s" % seedOffset
except NameError:
  pass

outputFileName="RTT.MC11.106070.PythiaZeeJet_Ptcut%s.root" % filePostfix 
try:
  outputFileName = OutputFileName
except NameError:
  pass


print "Seed offset: ", seedOffset, seedOffset1, seedOffset2 
print "Output file name: ", outputFileName



#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = evtMax
runArgs.ecmEnergy = CMSEnergy
runArgs.outputEVNTFile = "EVNT." + outputFileName
from AthenaPoolCnvSvc.WriteAthenaPool import *
StreamEVGEN = AthenaPoolOutputStream("StreamEVGEN",runArgs.outputEVNTFile)

include("MC11JobOptions/MC11.106070.PythiaZeeJet_Ptcut.py")
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA OFFSET %s 390020611 821000366" % seedOffset,
                                 "PYTHIA_INIT 820021 2347532"]
ServiceMgr.AtRndmGenSvc.Seeds = ["PHOTOS OFFSET %s 0020611 8210366" % seedOffset1,
                                 "PHOTOS_INIT 80021 47532"]
ServiceMgr.AtRndmGenSvc.Seeds = ["TAUOLA OFFSET %s 30020611 1000366" % seedOffset2,
                                 "TAUOLA_INIT 843321 27532"]

# start my own
from HepMCAnalysis_i.HepMCAnalysis_iConfig import HepMCAnalysis_i

myHepMCAnalysis = HepMCAnalysis_i("HepMCAnalysis_i", file = outputFileName)
myHepMCAnalysis.JetAnalysis=True
#myHepMCAnalysis.WplusJetAnalysis=True
myHepMCAnalysis.ZAnalysis=True
#myHepMCAnalysis.ZtautauAnalysis=True
#myHepMCAnalysis.WtaunuAnalysis=True
#myHepMCAnalysis.ttbarAnalysis=True
#myHepMCAnalysis.bbbarAnalysis=True
myHepMCAnalysis.UEAnalysis=True
myHepMCAnalysis.EtmissAnalysis=True
#myHepMCAnalysis.ElasScatAnalysis=True
#myHepMCAnalysis.UserAnalysis=True

topAlg += myHepMCAnalysis
# end my own

#
# End of job options file
#
###############################################################
