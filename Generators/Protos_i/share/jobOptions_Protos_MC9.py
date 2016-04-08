###############################################################
#
# JOB OPTIONS FILE. THIS FILE IS TO DEMONSTRATE HOW THE USER CAN
# CHANGE THE PYTHIA PARAMETERS. IT DOES NOT NECESARILY COMPLY TO
# THE ATLAS DEFAULTS FOR PHYSICS EVENT GENERATION.
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

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
# ServiceManager.AtRndmGenSvc.ReadFromFile = true;


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
topAlg=AlgSequence("TopAlg")
#from Pythia_i.Pythia_iConf import Pythia

include ( "MC09JobOptions/MC9_Pythia_Common.py" )

Pythia.PythiaCommand = ["pyinit user protos",
                        "pydat1 parj 90 20000.",
                        "pydat3 mdcy 15 1 0"
]

# ... Tauola
include ( "MC09JobOptions/MC9_Tauola_Fragment.py" )

# ... Photos
include ( "MC09JobOptions/MC9_Photos_Fragment.py" )

#from TruthExamples.TruthExamplesConf import DumpMC
#topAlg += DumpMC()

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "protos.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]

#
# End of job options file
#
###############################################################
