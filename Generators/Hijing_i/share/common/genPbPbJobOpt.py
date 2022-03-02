###############################################################
#
# An example of job options file for Hijing generation of
# Pb + Pb collisions at 5520 GeV/(colliding nucleon pair)
#
# Brian Cole
# Mikhail Leltchouk
# Andrzej Olszewski
#
# February 2007
#
# more examples in https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasHICSC2007
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
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence()

from Hijing_i.Hijing_iConf import Hijing
topAlg += Hijing()

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

import random

# Generate seeds
#
seed1 = random.randint(2<<15-1,1<<31-1)
seed2 = random.randint(2<<15-1,1<<31-1)
seed1 &= 0xfffffffe
seed2 &= 0xfffffffe

if not hasattr(svcMgr, 'AtRndmGenSvc'):
    from RngComps.RngCompsConf import AtRndmGenSvc
    svcMgr += AtRndmGenSvc()
    
svcMgr.AtRndmGenSvc.Seeds = \
       ["HIJING "+str(seed1)+" "+str(seed2), "HIJING_INIT 53240261 53827126"]

Hijing = Algorithm( "Hijing" )
Hijing.Initialize = ["efrm 5520.", "frame CMS", "proj A", "targ A",
                                         "iap 208", "izp 82", "iat 208", "izt 82",
                     # Simulation of minimum-bias events
                                         "bmin 0.", "bmax 20.",
                     # Maximum pt of hard or semihard scatterings in GeV,
                     # if negative (or commented as below) there is no up-limit.
                     ###                 "hipr1 9 10.",
                     # Turns OFF jet quenching:
                                         "ihpr2 4 0",
                     # Jan24,06 turns ON decays charm and  bottom but not pi0, lambda, ...
                                         "ihpr2 12 2",
                     # Turns ON retaining of particle history - truth information:
                                         "ihpr2 21 1"]

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

#--- Stream1 ---
Stream1 = AthenaPoolOutputStream( "Stream1" )

theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv" ]
PoolSvc = Service( "PoolSvc" )

# 2101 == EventInfo
# 133273 == MCTruth (HepMC)
# 54790518 == HijigEventParams
Stream1.ItemList += [ "2101#*" ]
Stream1.ItemList += [ "133273#*" ]
Stream1.ItemList += [ "54790518#*" ]

Stream1.OutputFile = "hijing.pool.root"
#==============================================================
#
# End of job options file
#
###############################################################
