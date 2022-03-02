###############################################################
#
# 28/02/2006 Andrzej Olszewski
# jobOptions to run Hijing generation and AddFlowByShifting
# in a single job producing container GEN_EVENT
# with flow modified particles
# Now with "ihpr2 12 2"
# - turned ON decays charm and bottom but not pi0, lambda, ...;
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Hijing_i.Hijing_iConf import Hijing
job += Hijing()

from FlowAfterburner.FlowAfterburnerConf import AddFlowByShifting
job += AddFlowByShifting()

theApp.Dlls   += [ "Hijing_i", "FlowAfterburner" ]
theApp.TopAlg  = [ "Hijing" ]
theApp.TopAlg += [ "AddFlowByShifting" ]
#theApp.ExtSvc += [ "AtRndmGenSvc" ]

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from RngComps.RngCompsConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()
svcMgr.AtRndmGenSvc.Seeds = ["HIJING 4789891 989240511",
                      "HIJING_INIT 889223461 78782321"]
#AtRndmGenSvc.ReadFromFile = true;

Hijing = job.Hijing
Hijing.McEventKey = "HIJING_EVENT"
Hijing.Initialize = ["efrm 5520", "frame CMS", "proj A", "targ A",
                    "iap 208", "izp 82", "iat 208", "izt 82",
                    "bmin  4.9316", "bmax 6.0374", "ihpr2 4 0", # turn off quenching
                    "ihpr2 18 1", "hipr1 7 5.36", # turn on b production
                    "ihpr2 12 2", "ihpr2 21 1"] # turn on (some) decays
                                                # and history



AddFlowByShifting = job.AddFlowByShifting
AddFlowByShifting.McTruthKey    = "HIJING_EVENT"
AddFlowByShifting.McFlowKey     = "GEN_EVENT"

#"jjia_minbias_new", "jjia_minbias_new_v2only", "fixed_vn", "fixed_v2", "jjia_minbias_old", "ao_test", "custom"
AddFlowByShifting.FlowFunctionName="jjia_minbias_new"
AddFlowByShifting.FlowImplementation="exact" # "approximate" , "exact"
AddFlowByShifting.FlowFluctuations  =False # True, False

AddFlowByShifting.RandomizePhi  = 0

AddFlowByShifting.FlowEtaSwitch = 0
AddFlowByShifting.FlowMinEtaCut = 0
AddFlowByShifting.FlowMaxEtaCut = 6.5

AddFlowByShifting.FlowPtSwitch  = 0
AddFlowByShifting.FlowMinPtCut  = 0
AddFlowByShifting.FlowMaxPtCut  = 2.0

AddFlowByShifting.custom_v1=0.0000;
AddFlowByShifting.custom_v2=0.0500;
AddFlowByShifting.custom_v3=0.0280;
AddFlowByShifting.custom_v4=0.0130;
AddFlowByShifting.custom_v5=0.0045;
AddFlowByShifting.custom_v6=0.0015;
AddFlowByShifting.FlowBSwitch   = 0




# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel               = 3
svcMgr.MessageSvc.defaultLimit              = 10000
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000

# Set run number (default 0 causes problems)
svcMgr.EventSelector.RunNumber = 1001

#--------------------------------------------------------------
# AthenaRoot Persistency
#--------------------------------------------------------------
include( 'GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py' )
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
# 2101   == EventInfo
# 133273 == MC Truth (HepMC)
# 54790518 == HijigEventParams
Stream1.ItemList += [ "2101#*" ]
Stream1.ItemList += [ "133273#*" ]
Stream1.ItemList += [ "54790518#*" ]
Stream1.OutputFile = "hijing.flow.10-15.pool.root"

#==============================================================
#
# End of job options file
#
###############################################################
