###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# import message levels (INFO/ERROR/...)
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr    import theApp
from AthenaCommon.AppMgr    import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 5
    pass
theApp.EvtMax = EVTMAX

from RngComps.RngCompsConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc(
    Seeds = [ "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
              "JIMMY 390020611 821000366", "JIMMY_INIT 820021 2347532",
              "HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532"
              ]
    )

####################
# Generate the event
####################
if not 'GENERATOR' in dir():
    GENERATOR = "Pythia"
if not 'PROCESS' in dir():
    PROCESS = "ttbar"
from McParticleTests.tests import makeGenEvents
topSequence += makeGenEvents( genName    = GENERATOR,
                              genProcess = PROCESS,
                              cfgGenName = "EvGen" )

########
# Create the AOD McEventCollection
#
from McParticleAlgs.JobOptCfg import McAodBuilder
topSequence += McAodBuilder()
topSequence.McAodBuilder.DoFiltering = False
topSequence.McAodBuilder.CnvTool.McEvents             = "GEN_EVENT"
topSequence.McAodBuilder.CnvTool.TruthParticlesOutput = "SpclMC"

if not 'ALGMODE' in dir():
    ALGMODE='cpp'
    pass
if ALGMODE=='py':
    from McParticleTests.Lib import PyMcAodSymLinkTests as McAodSymLinkTests
else:
    from McParticleTests.McParticleTestsConf import McAodSymLinkTests
topSequence += McAodSymLinkTests( OutputLevel = DEBUG )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream", noTag=True)
outStream.ItemList  = [ "EventInfo#*"]
outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]
outStream.ItemList += [ "TruthParticleContainer#SpclMC" ]

if 'OUTPUT' not in dir():
  OUTPUT = "mc.aod.symlinktests.pool"
  pass
outStream.OutputFile = OUTPUT
outStream.EvtConversionSvc = "AthenaPoolCnvSvc"

svcMgr.MessageSvc.defaultLimit = 4000000
svcMgr.MessageSvc.OutputLevel  = ERROR
#==============================================================
#
# End of job options file
#
###############################################################
