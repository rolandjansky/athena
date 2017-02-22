###############################################################
# 25/02/2007 Andrzej Olszewski
# jobOptions to run Hydjet generation
# Random number seed setting via nseed
# 15/03/2008 Andrzej Olszewski
# Updated for configurables
#==============================================================
###############################################################
#
# An example of job options file for Hydjet generation of
# Pb + Pb collisions at 5520 GeV/(colliding nucleon pair)
#
#==============================================================
# General Application Configuration options
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
from AthenaCommon.Configurable import Configurable
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

# configuring the Athena application for a 'generator' job
import AthenaCommon.AtlasUnixGeneratorJob

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence()

from Hydjet_i.Hydjet_iConf import Hydjet
topAlg += Hydjet()

from TruthExamples.TruthExamplesConf import PrintHijingPars
topAlg += PrintHijingPars()

StoreGateSvc = svcMgr.StoreGateSvc
StoreGateSvc.Dump = TRUE

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel  = 3
svcMgr.MessageSvc.defaultLimit = 100000

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10

# Set run number (default 0 causes problems)
svcMgr.EventSelector.RunNumber   = 12345
svcMgr.EventSelector.FirstEvent  = 1

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

# Use fixed seeds for reproducibility
seed1 = 123456789
seed1 &= 0xfffffffe

Hydjet = Algorithm( "Hydjet" )
Hydjet.McEventKey = "GEN_EVENT"
Hydjet.Initialize = ["e 5520", "a 208", "nh 20000",
                    #"ifb 0", "bfix 0", # fixed impact [fm] 
                    "ifb 1", "bmin 0", "bmax 30", # impact range [fm]
                    "nhsel 2", "ishad 1", "ptmin 10.0",
                    "ylfl 4.0", "ytfl 1.5", "tf 0.1", "fpart 1",
                    "ienglu 0", "ianglu 0", "t0 1", "tau0 0.1", "nf 0",
                    "mstp 51 7", "mstp 81 0", "mstu 21 1", "paru 14 1.0",
                    "msel 1", "nseed "+str(seed1) ] 
                     
#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )

theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv" ]
PoolSvc = Service( "PoolSvc" )

# Name of the output file
Stream1.OutputFile = "hydjet.test.pbpb5520.r12345.pool.root"

# 2101 == EventInfo
# 133273 == MCTruth (HepMC)
# 54790518 == HijigEventParams
Stream1.ItemList += [ "2101#*" ]
Stream1.ItemList += [ "133273#*" ]
Stream1.ItemList += [ "54790518#*" ]

#==============================================================
#
# End of job options file
#
###############################################################
