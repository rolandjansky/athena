###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING,ERROR
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 5
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc(
    Seeds = [ "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
              "JIMMY 390020611 821000366", "JIMMY_INIT 820021 2347532",
              "HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532"
              ]
    #, ReadFromFile = True
    )

###############################
# Create some 4-vectors
###############################
from AthExThinning.AthExThinningAlgsConf import AthExThinning__CreateData
topSequence += AthExThinning__CreateData(
    "CreateData",
    ParticlesOutput = "Particles",
    NbrOfParticles  = 10,
    OutputLevel = DEBUG )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool as wap
outStream = wap.AthenaPoolOutputStream("StreamUSR", noTag=True)
outStream.ItemList  = [ "EventInfo#*"]
outStream.ItemList += [ "AthExParticles#*" ]
outStream.ItemList += [ "AthExDecay#*" ]
outStream.ItemList += [ "AthExElephantino#*" ]

if not 'OUTPUT' in dir():
  OUTPUT = "my.data.pool"
  pass
outStream.OutputFile = OUTPUT

svcMgr += CfgMgr.MessageSvc( defaultLimit = 4000000 )
#svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################
