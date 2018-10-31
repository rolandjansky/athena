###############################################################
#
# Ampt_i job options file
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

from Ampt_i.Ampt_iConf import Ampt
topAlg += Ampt("Ampt")

theApp.ExtSvc += ["AtRndmGenSvc"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel  = 3
svcMgr.MessageSvc.defaultLimit = 100000

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 3

# Set run number (default 0 causes problems)
svcMgr.EventSelector.RunNumber = 12345

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
    
import random

seed1 = random.randint(2<<15-1,1<<31-1)
seed2 = random.randint(2<<15-1,1<<31-1)
seed1 &= 0xfffffffe
seed2 &= 0xfffffffe

if not hasattr(svcMgr, 'AtRndmGenSvc'):
    from AthenaServices.AthenaServicesConf import AtRndmGenSvc
    svcMgr += AtRndmGenSvc()

#svcMgr.AtRndmGenSvc.Seeds = ["AMPT "+str(seed1)+" "+str(seed2), "AMPT_INIT 889223465 78782321"]
svcMgr.AtRndmGenSvc.Seeds = ["AMPT 4789899 989240512", "AMPT_INIT 889223461 78782327"]

topAlg.Ampt.EFRM = 200.;
topAlg.Ampt.Bmin = 7.;
topAlg.Ampt.Bmax = 14.;
topAlg.Ampt.writeAmpt = False;

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv" ]
PoolSvc = Service( "PoolSvc" )
### 2101 == EventInfo
### 133273 == MCTruth (HepMC)
### 54790518 == HijigEventParams
Stream1.ItemList += [ "2101#*" ]
Stream1.ItemList += [ "133273#*" ]
Stream1.ItemList += [ "54790518#*" ]
Stream1.OutputFile = "ampt.pool.root"

#==============================================================
#
# End of job options file
#
###############################################################

