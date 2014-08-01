###############################################################
#
# An example of job options file for Baur generation of
#
#Andrea bocci
#Zhijun Liang 
#
# Februar 2008
##
#
#==============================================================
# General Application Configuration options
#--------------------------------------------------------------
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
from AthenaCommon.AppMgr import ServiceMgr
#theApp.setup( MONTECARLO )
include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls  += [ "Baur_i", "TruthExamples" ]
theApp.TopAlg = ["Baur","DumpMC"]
theApp.ExtSvc += ["AtRndmGenSvc"]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = 3
MessageSvc.defaultLimit = 100000
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

# Set run number (default 0 causes problems)
ServiceMgr.EventSelector.RunNumber = 12345
#EventSelector = Service("EventSelector")
#EventSelector.RunNumber = 12345

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

AtRndmGenSvc = Service( "AtRndmGenSvc" )
#AtRndmGenSvc.Seeds = ["HIJING "+str(seed1)+" "+str(seed2), "HIJING_INIT 53240261 53827126"]

Baur = Algorithm( "Baur" )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
include("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
include("GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")




#include( "jobOptions.BaurPythia.py" )

# 2101 == EventInfo
# 133273 == MCTruth (HepMC)
# 54790518 == HijigEventParams
#Stream1 = Algorithm( "Stream1" )
#Stream1.ItemList += [ "2101#*" ]
#Stream1.ItemList += [ "133273#*" ]
#Stream1.ItemList += [ "54790518#*" ]

#Stream1.OutputFile = "fileName.pool.root"
#==============================================================
#
# End of job options file
#
###############################################################
