###############################################################
#
# Job options file for Pyquench_i
#
# Brian Cole, Nathan Grau, Mikhail Leltchouk, Nevis, April 2006
#==============================================================

# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = FATAL

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Set run number (default 0 causes problems)
#EventSelector = Service("EventSelector")
ServiceMgr.EventSelector.RunNumber = 1040

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYQUENCH 47895451 989454505", "PYQUENCH_INIT 489234453 71565781"]
# AtRndmGenSvc.ReadFromFile = true;

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Pyquench_i.Pyquench_iConf import Pyquench
job += Pyquench()
job.Pyquench.OutputLevel = 3
job.Pyquench.nuclearA = 208
job.Pyquench.minBiasYN = False
job.Pyquench.bimpact = 0.
job.Pyquench.ptmin = 100.
job.Pyquench.sqrtSNN = 5520.

# Number of events to be processed (default is 10)
theApp.EvtMax = 50

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
include("GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")

include("AthenaSealSvc/AthenaSealSvc_joboptions.py" )
AthenaSealSvc.CheckDictionary = TRUE


from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
#Stream1.ForceRead=TRUE
Stream1.OutputFile = "r1040b0pyquench.root"
# 2101 = EventInfo
# 133273 = MCTruth (HepMC)
Stream1.ItemList += [ "2101#*", "133273#*" ]

#==============================================================
#
# End of job options file
#
###############################################################
