#--------------------------------------------------------------
# Private Application Configuration option
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512",
"PYTHIA_INIT 820021 2347532"]
#
from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
#
from Pythia_i.Pythia_iConf import Pythia
job +=Pythia()
Pythia = job.Pythia

# Generate Z->mu mu
Pythia.PythiaCommand += [ "pysubs msel 0",         # Users decay choice.
                          "pydat1 parj 90 20000",   # Turn off FSR.
                          "pydat3 mdcy 15 1 0",     # Turn off tau decays.
#
# Z production:
                          "pysubs msub 1 1",        # Create Z bosons.
                          "pysubs ckin 1 60.0",     # Lower invariant mass.
			  "pypars mstp 43 2",
                          "pydat3 mdme 174 1 0",
                          "pydat3 mdme 175 1 0",
                          "pydat3 mdme 176 1 0",
                          "pydat3 mdme 177 1 0",
                          "pydat3 mdme 178 1 0",
                          "pydat3 mdme 179 1 0",
                          "pydat3 mdme 182 1 1",    # Switch for Z->ee.
                          "pydat3 mdme 183 1 0",
                          "pydat3 mdme 184 1 0",    # Switch for Z->mumu.
                          "pydat3 mdme 185 1 0",
                          "pydat3 mdme 186 1 1",    # Switch for Z->tautau.
                          "pydat3 mdme 187 1 0"]
 
Pythia.useAtlasPythiaTune08=True





from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()

#set-up tauola
from Tauolapp_i.Tauolapp_iConf import TauolaAlg
TauolaAlg=TauolaAlg( "TauolaAlg" )
### #TauolaAlg.decay_mode_same(1); #tau- decay to electron + neutrinos
### #TauolaAlg.decay_mode_opposite(2); #tau+ decays to muon + neutrinos
TauolaAlg.decay_mode_same = 1; #tau- decay to electron + neutrinos
TauolaAlg.decay_mode_opposite = 2; #tau+ decays to muon + neutrinos
TauolaAlg.decay_particle = 15; # decay particle
job += TauolaAlg

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "pythia-tauola.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#GEN_EVENT' ]
###############################################################

#include( "PartPropSvc/PartPropSvc.py" )
#include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
