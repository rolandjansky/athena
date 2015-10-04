# Job options used for L2/EF merging tests
# The input file is the default BSRDO file
#
#trigtest.pl --test AthenaTrigRDO_L2EFMerging --run AthenaTrigRDO_L2EFMerging --conf TriggerTest.conf

import os
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf


####################################
# Generic setup
####################################
#EvtMax=20
#ReadBS=True
#testCurrentMenu=True
#testPhysicsV5=True
#testLS1V1=True
rerunLVL1=True

OutputLevel=WARNING
HLTOutputLevel=WARNING
#SkipEvents=0
#setPrescale="None"
doValidation=False
#chainOrderedDown=True

#BSRDOInput='root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data12_8TeV.00212967.physics_EnhancedBias.merge_eb_zee_zmumu_cc.RAW._lb0291._SFO-5._0001.data'

#used externally with Athena
#useCONDBR2=False
#setGlobalTag='COMCOND-HLTP-004-03'
# athena setDetDescr='ATLAS-R1-2012-02-01-00'
#setDetDescr='ATLAS-R1-2012-02-00-00'
#testPhysicsV5=True

#####################################
# configure the menu
#####################################
#emptyMenu=True
#testEgamma=True
#testJet=True
#testMuon=True
#testTau=True
#testBjet=True
#testBphysics=True
#testMET=True
#testMinBias=True
#testCombined=True
##testCalib=True
#testCosmic=True
#testGeneric=True
#testMonitor=True
#testBeamSpot=True
#testStreaming=True
#removeSignatures={'JetSlice':["L2_fj10_a4TTem_fj10000_c4cchad_L1RD0", "L2_j10_a4TThad_j100000_c4cchad_L1RD0", "L2_j10000_a4cchad_L1RD0", "L2_j10000_c4cchad_L1RD0"]}
#removeSignatures={'MonitorSlice':["cost_monitor"]}
#########################################
# enable new ROBDataProviderSvc for RTT tests
# first generate the rob-ros-map file
#generate-rob-ros-map-from-data.py BSRDOInput > rob-ros-map.py
#include("TrigROBDataProviderSvc/TrigROBDataProviderSvc.py")
include("TrigROBDataProviderSvc/TrigROBDataProviderSvc_RTT.py")
#########################################
include("TriggerTest/rob-ros-map.py")

#when did this come in - check in SVN Trac
#TriggerFlags.triggerMenuSetup="Physics_pp_v5"

#####################################################
include('TriggerRelease/runHLT_standalone.py')
#####################################################


####################################
#svcMgr.EventSelector.SkipEvents = SkipEvents 
####################################

### configure the steering
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

def configureSteering(trigSteer):
	trigSteer.enableRerun=False
	trigSteer.enableRobRequestPreparation=True
	trigSteer.cachingMode=0
	trigSteer.EBstrategy=1 #call EB at the end of all chains	
	
	from TrigSteerMonitor.TrigSteerMonitorConfig import TrigROBMoniConfig
	TrigSteerMonROB = [  TrigROBMoniConfig() ]	
	trigSteer.MonTools += TrigSteerMonROB

	trigSteer.OutputLevel=INFO
	#for algo in trigSteer.getChildren(): 
	#	algo.OutputLevel = WARNING
		#	print trigSteer


configureSteering(topSequence.TrigSteer_HLT)


# additional DEBUG flags:
#ServiceMgr.HLTConfigSvc.OutputLevel=DEBUG
#ServiceMgr.TrigConfigSvc.OutputLevel=VERBOSE
#ToolSvc.TrigDataAccess.OutputLevel=VERBOSE

ServiceMgr.ROBDataProviderSvc.OutputLevel=WARNING
ServiceMgr.MessageSvc.debugLimit=0
ServiceMgr.MessageSvc.warningLimit=5000
#ServiceMgr.MessageSvc.infoLimit=0

print " End of configuration for testAthenaL2EFMerging"


# MSG::NIL  	(=0) DEFAULT
# MSG::VERBOSE  (=1)
# MSG::DEBUG	(=2)
# MSG::INFO	(=3)
# MSG::WARNING  (=4)
# MSG::ERROR	(=5)
# MSG::FATAL    (=6)
# MSG::ALWAYS   (=7)



