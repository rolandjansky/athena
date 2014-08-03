###############################################################
#
# Job options file
#
#==============================================================

# You can use this jobO like that:
# athena -c 'MCEVENTKEY="GEN_EVENT"; POOL_OUTPUT="my.pool.root"' AnalysisExamples/WriteTruthParticles_jobOptions.py

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle on the sequence of top-level algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle on the service manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the application manager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 5
    pass
theApp.EvtMax = EVTMAX

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc(
    "AtRndmGenSvc",
    Seeds = [
       "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
       "JIMMY 390020611 821000366", "JIMMY_INIT 820021 2347532",
       "HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532",
       ]
    )

####################
# Generate the event
####################
from Pythia_i.Pythia_iConf import Pythia

topSequence += Pythia(
    "Pythia",
    PythiaCommand = [
        # generate ttbar events
        "pysubs msel 6",

        #selection of the subprocess gg -> ttbar
        "pysubs msub 81 1",
        "pysubs msub 82 1",
        ]
    )

###############################
# Dump the event from Generator
###############################
if not 'MCEVENTKEY' in dir():
    MCEVENTKEY = "GEN_EVENT"
    pass
# uncomment this if you want to dump the McEventCollection on-screen
## from TruthExamples.TruthExamplesConf import DumpMC
## topSequence += DumpMC("GenMcEvent",
##                       McEventKey = MCEVENTKEY)

###############################
# Convert a McEventCollection
# into a TruthParticleContainer
###############################
from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
from AnalysisExamples.AnalysisExamplesConf import WriteTruthParticles
topSequence += WriteTruthParticles(
    "WriteTruthParticles",
    CnvTool = TruthParticleCnvTool( "CnvTool",
                                    McEvents = MCEVENTKEY,
                                    TruthParticlesOutput = "My"+MCEVENTKEY )
    )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

if not 'POOL_OUTPUT' in dir():
    POOL_OUTPUT = "mc.aod.pool"
    pass

outStream = AthenaPoolOutputStream("OutStream")
outStream.ItemList  = [
    "EventInfo#*",
    "McEventCollection#*",
    "TruthParticleContainer#*",
    ]
outStream.OutputFile = POOL_OUTPUT

#==============================================================
#
# End of job options file
#
###############################################################
