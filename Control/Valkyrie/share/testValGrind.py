###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 1000
    pass
theApp.EvtMax = EVTMAX

if not hasattr(svcMgr, 'AtRndmGenSvc'):
    from RngComps.RngCompsConf import AtRndmGenSvc
    svcMgr += AtRndmGenSvc()
    
svcMgr.AtRndmGenSvc.Seeds =[
    "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
    "JIMMY 390020611 821000366", "JIMMY_INIT 820021 2347532",
    "HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532"
    ]

###############################
# Load Valgrind service
###############################
from Valkyrie.ValkyrieConf import ValgrindSvc
svcMgr += ValgrindSvc( OutputLevel = VERBOSE )
theApp.CreateSvc += [ svcMgr.ValgrindSvc.getFullJobOptName() ]

###############################
# Create some 4-vectors
###############################
from AthExThinning.AthExThinningAlgsConf import AthExThinning__CreateData
topSequence += AthExThinning__CreateData(
    "CreateData",
    ParticlesOutput = "Particles",
    NbrOfParticles  = 10,
    OutputLevel = DEBUG
    )

svcMgr.MessageSvc.defaultLimit = 4000000
#svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################
