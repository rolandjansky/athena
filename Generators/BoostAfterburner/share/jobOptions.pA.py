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
Hijing = topAlg.Hijing
Hijing.Initialize = ["efrm 2750.", "frame CMS", "proj P", "targ A",
                    "iap 1", "izp 1", "iat 208", "izt 82",
# simulation of minimum-bias events
                     "bmin 0", "bmax 10",
# turns OFF jet quenching:
                     "ihpr2 4 0",
# Jan24,06 turns ON decays charm and  bottom but not pi0, lambda, ... 
                     "ihpr2 12 2",
# turns ON retaining of particle history - truth information:
                     "ihpr2 21 1",
# turning OFF string radiation
                     "ihpr2 1 0",
# change minimum pt for hard scatterings, default is 2 GeV
                     "hipr1 8 4"
                     ]

from BoostAfterburner.BoostAfterburnerConf import BoostEvent
topAlg+=BoostEvent()
BoostEvent=topAlg.BoostEvent
BoostEvent.BetaZ=0.5
BoostEvent.McOutputKey="BOOSTED_EVENT"
BoostEvent.OutputLevel=2


#if not hasattr(topAlg,'CBNT_AthenaAware'):
#    from CBNT_Athena.CBNT_AthenaAwareCfg import CBNT_AthenaAware
#    theCBNT_AthenaAware = CBNT_AthenaAware()
#    CBNT_AthenaAware = topAlg.CBNT_AthenaAware

#from CBNT_Truth.CBNTAA_TruthCfg import CBNTAA_Truth
#theCBNTAA_Truth = CBNTAA_Truth(MaxNbParticles = 100000 ,
#                               MaxNbVertices = 10000 ,
#                               PtMin = -1. * GeV ,
#                               EtaMax= -1)

#theCBNTAA_Truth.McEventsName=BoostEvent.McOutputKey
#from CBNT_Truth.CBNT_TruthConf import CBNT_TruthSelector
#theSelector= CBNT_TruthSelector( "All" ,
#                                 PtMin = -1. * GeV ,
#                                 EtaMax = -7 ,
#                                 OptionAll = True ,
#                                 Enable = True)

#theCBNTAA_Truth +=theSelector									   
#CBNT_AthenaAware += theCBNTAA_Truth
#from CBNT_Truth.CBNT_TruthConf import CBNTAA_HijingEventParams
#CBNT_AthenaAware +=CBNTAA_HijingEventParams()

#from GaudiSvc.GaudiSvcConf import THistSvc
#svcMgr+=THistSvc()
#tHistSvc = svcMgr.THistSvc
#tHistSvc.Output = ["AANT DATAFILE='"+"hijing.cbnt.root"+"' OPT='UPDATE'"]

#from AnalysisTools.AnalysisToolsConf import AANTupleStream
#theApp.TopAlg += [ "AANTupleStream" ]
#AANTupleStream = Algorithm( "AANTupleStream" )
#AANTupleStream.ExtraRefNames = [ ]
#AANTupleStream.OutputName = "hijing1.cbnt.root"
#AANTupleStream.WriteInputDataHeader = False
#AANTupleStream.ExistDataHeader = False

#topAlg+=CBNT_AthenaAware
#topAlg+=AANTupleStream


theApp.ExtSvc += ["AtRndmGenSvc"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel  = 4
svcMgr.MessageSvc.defaultLimit = 100000
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp.EvtMax = 5000
theApp.EvtMax = 5

# Set run number (default 0 causes problems)
svcMgr.EventSelector.RunNumber = 12345

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
if not hasattr(svcMgr, 'AtRndmGenSvc'):
    from RngComps.RngCompsConf import AtRndmGenSvc
    svcMgr += AtRndmGenSvc()
    
svcMgr.AtRndmGenSvc.Seeds = ["HIJING 4789899 989240512", "HIJING_INIT 889223465 78782321"]
