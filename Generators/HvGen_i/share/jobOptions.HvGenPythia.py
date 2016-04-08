###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.Logging import logging

evgenLog = logging.getLogger('csc_evgen')

# special setup for event generation
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

# remember output level I set

evgenMsgLevel = INFO

# other includes needed by the physics file
include( "PartPropSvc/PartPropSvc.py" )

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

svcMgr.MessageSvc.OutputLevel = evgenMsgLevel

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
atRndmGenSvc = svcMgr.AtRndmGenSvc
AtRndmGenSvc.Seeds = ["PYTHIA 330020611 841000366", "PYTHIA_INIT 824021 3247532"]

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()

from TruthExamples.TruthExamplesConf import PrintMC
topAlg += PrintMC()
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
Pythia = topAlg.Pythia
Pythia.PythiaCommand = ["pyinit user hvgen"]
# decay particles anyware
Pythia.PythiaCommand += ["pydat1 parj 71 20000."]

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
#
#include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
#theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv" ]
#Stream1 = Algorithm( "Stream1" )
#Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
#
#Stream1.OutputFile = "HV.pool.root"
#
#==============================================================
#
# End of job options file
#
###############################################################
