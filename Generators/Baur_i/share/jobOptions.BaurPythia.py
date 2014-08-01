###############################################################
#
# Job options file
#
#==============================================================
#---------------------------------------------------------------
# Convert Baur Output
#---------------------------------------------------------------
include("ConvertToMadgraph.py")

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
theApp.EvtMax = 10
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
Pythia = topAlg.Pythia
Pythia.PythiaCommand = ["pyinit user madgraph"]


from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "baur.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
###############################################################


#
###############################################################
#from AthenaCommon.AppMgr import ServiceMgr
#ServiceMgr.EventSelector.RunNumber = runArgs.runNumber
#ServiceMgr.EventSelector.FirstEvent = runArgs.firstEvent
#----------------------------------------------------------------
# Ntuple service output
#----------------------------------------------------------------

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
