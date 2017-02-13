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
AtRndmGenSvc.Seeds = ["HERWIG 330020611 841000366", "HERWIG_INIT 824021 3247532"]

from Herwig_i.Herwig_iConf import Herwig
topAlg += Herwig()
# ... Main generator : Herwig
Herwig = topAlg.Herwig
Herwig.HerwigCommand = [ "iproc mcatnlo",
                       "modpdf 10550","autpdf HWLHAPDF",
                       "jmueo 1",
                        "msflag 1",
                       "jmbug 0",
                       "ispac 2",
                       "qspac 2.5", 
                       "ptrms 1.2", 
                       "ptjim 3.13",
                       "jmrad 73 1.82",
                       "prsof 0",
                       "clpow 1.20",
                       "pltcut 0.0000000000333",
                       "ptmin 10.",
                       "mixing 1",        
                       "maxpr 5"  ]

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

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
