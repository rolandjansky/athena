#--------------------------------------------------------------
# Private Application Configuration option
# Using Pythia8
#
# check out both External/Pythia8 and Generators/Pythia8_i . External/Pythia8 is used to link to the Genser Pythia8 
# distribution. You should source setup.sh and gmake first External/Pythia8 and then Generators/Pythia8_i. 
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
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA8 4789899 989240512",
"PYTHIA8_INIT 820021 2347532"]
#
###from AthenaCommon.AlgSequence import AlgSequence
###job=AlgSequence()
#
###from Pythia_i.Pythia_iConf import Pythia
###job +=Pythia()
###Pythia = job.Pythia

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from Pythia8_i.Pythia8_iConf import Pythia8_i
job+=Pythia8_i()
Pythia = job.Pythia8_i

# Generate Z->mu mu


# Z decays into tau+tau-
Pythia.Commands += ["WeakBosonAndParton:qqbar2gmZg = on",
                           "WeakBosonAndParton:qg2gmZq = on",
                           "23:onMode = off", # choose decay modes for Z0 : switch off everything but Z0 -> leptons
                           "23:onIfAny = 15", # 23:onIfAny = 11 13 15, choose tau
##			   "15:onMode = off", # no tau decays
			   ] 
#Pythia.CollisionEnergy = 14000

				   




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
