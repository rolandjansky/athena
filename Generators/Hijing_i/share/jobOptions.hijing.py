###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
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
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence()

from Hijing_i.Hijing_iConf import Hijing
topAlg += Hijing()
from TruthExamples.TruthExamplesConf import DumpMC
topAlg += DumpMC()

theApp.ExtSvc += ["AtRndmGenSvc"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel  = 4
svcMgr.MessageSvc.defaultLimit = 100000
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5

# Set run number (default 0 causes problems)
svcMgr.EventSelector.RunNumber = 12345

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
if not hasattr(svcMgr, 'AtRndmGenSvc'):
    from AthenaServices.AthenaServicesConf import AtRndmGenSvc
    svcMgr += AtRndmGenSvc()
    
svcMgr.AtRndmGenSvc.Seeds = ["HIJING 4789899 989240512", "HIJING_INIT 889223465 78782321"]
#svcMgr.AtRndmGenSvc.ReadFromFile = True

# Hijing = Algorithm( "Hijing" )
# Hijing.Initialize = ["efrm 200", "frame CMS", "proj A", "targ A", 
#                      "iap 197", "izp 79", "iat 197", "izt 79"]
# Hijing.wide = FALSE # Allow the random events to be anywhere in the beampipe
# Hijing.randomizeVertices = TRUE # Randomize vertices within the beampipe - default x,y = 0,0
# Hijing.selectVertex = TRUE # Allows manual vertex settings. Please set x, y, and z. 
# Hijing.x = 10.0 #mm - X vertex setting - overridden by randomization.
# Hijing.y = 10.0 #mm - Y vertex setting - overridden by randomization.
# Hijing.z = 10000.0 #mm - Z vertex setting - overridden by randomization.
# Hijing.vertexOffsetCut = 1.0E-7 # default
# Hijing.partonStoreMinPt = 5.0 # default
# Hijing.keepSpectators = true # default
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
