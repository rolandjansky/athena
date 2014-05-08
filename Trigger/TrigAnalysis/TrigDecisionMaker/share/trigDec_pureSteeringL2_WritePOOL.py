########################
# Framework
########################

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10

########################
# Generate config XML files
########################

include("pureNewSteering_menu.py")

########################
# L2 Steering:
########################

from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
log.info("setting up TestingTrigSteer_L2 configurable:")
hltSteer_L2 = TestingTrigSteer_L2('hltSteer_L2', hltFile='pureNewSteering_menu.xml', lvl1File='')

topSequence += hltSteer_L2
hltSteer_L2.Navigation.ClassesToPayload += [ 'TrigRoiDescriptor' ]

########################
# EF Steering:
########################

from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
log.info("setting up TestingTrigSteer_EF configurable:")
hltSteer_EF = TestingTrigSteer_EF('hltSteer_EF')

topSequence += hltSteer_EF
hltSteer_EF.Navigation.ClassesToPayload += [ 'TrigRoiDescriptor' ]

########################
# TrigDecision Makers
########################

theApp.Dlls += [ "TrigDecisionMaker" ]
theApp.TopAlg += [ "TrigDec::TrigDecisionMaker/TrigDecMaker" ]
theApp.TopAlg += [ "TrigDec::TrigAODConfigDataMaker/TrigAODCfgMaker" ]

########################
# POOL
########################

import AthenaPoolCnvSvc.WriteAthenaPool

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#Explicitly specify the output file catalog
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalog1.xml"

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()
svcMgr.AthenaPoolCnvSvc.CommitInterval = 10;

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "HLT.root"; # ** mandatory parameter ** // The output file name
Stream1.ItemList += [ "TrigDec::TrigDecision#TrigDecision" ]
Stream1.ItemList += [ "TrigConf::HLTAODConfigData#AODConfig" ]
Stream1.ItemList += [ "TrigConf::Lvl1AODConfigData#AODConfig" ]
Stream1.ItemList += [ "TrigConf::Lvl1AODPrescaleConfigData#AODConfig" ]

theApp.Dlls += [ "TrigEventAthenaPoolPoolCnv" ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.PoolSvc.OutputLevel = 2 
svcMgr.AthenaPoolCnvSvc.OutputLevel = 2
Stream1.OutputLevel = 2
