# Job options used for L2/EF merging tests
# The input file is the default BSRDO file
#
# LVL2 only: -c 'EvtMax=10;doEF=False'
# EF only:   -c 'EvtMax=10;doLVL2=False'
# HLT only:  -c 'EvtMax=10;doHLT=True'
#trigtest.pl --test AthenaTrigRDO_L2EFMerging_nomerge --run AthenaTrigRDO_L2EFMerging_nomerge --conf TriggerTest.conf

import os
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf


####################################
# Generic setup
####################################
EvtMax=20
ReadBS=True
#testCurrentMenu=True
#testPhysicsV5=True
testLS1V1=True
OutputLevel=DEBUG
HLTOutputLevel=DEBUG
SkipEvents=0
setPrescale="None"
doValidation=False
chainOrderedDown=True
BSRDOInput='root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data12_8TeV.00212967.physics_EnhancedBias.merge_eb_zee_zmumu_cc.RAW._lb0291._SFO-5._0001.data'
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
include("TriggerTest/rob-ros-map.py")
#########################################


#####################################################
include('TriggerRelease/runHLT_standalone.py')
#####################################################


####################################
svcMgr.EventSelector.SkipEvents = SkipEvents 
####################################

### configure the steering
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

def configureSteering(trigSteer):
	trigSteer.OutputLevel=DEBUG
#VERBOSE
	trigSteer.enableRerun=False
	for algo in trigSteer.getChildren(): 
		algo.OutputLevel = DEBUG
#INFO
	print trigSteer

if TriggerFlags.doHLT():
	topSequence.TrigSteer_HLT.enableRobRequestPreparation=True
	topSequence.TrigSteer_HLT.cachingMode=0
	topSequence.TrigSteer_HLT.EBstrategy=1 #call EB at the end of all chains	
	configureSteering(topSequence.TrigSteer_HLT)
else: 
	if TriggerFlags.doEF():
		configureSteering(topSequence.TrigSteer_EF)
	if TriggerFlags.doLVL2():
		topSequence.TrigSteer_L2.enableRobRequestPreparation=True
		configureSteering(topSequence.TrigSteer_L2)


# additional DEBUG flags:
#ServiceMgr.HLTConfigSvc.OutputLevel=DEBUG
#ServiceMgr.TrigConfigSvc.OutputLevel=VERBOSE
#ToolSvc.TrigDataAccess.OutputLevel=VERBOSE

ServiceMgr.ROBDataProviderSvc.OutputLevel=WARNING
#ServiceMgr.MessageSvc.debugLimit=0
ServiceMgr.MessageSvc.warningLimit=0
#ServiceMgr.MessageSvc.infoLimit=0

print " End of configuration for testAthenaL2EFMerging"


# MSG::NIL  	(=0) DEFAULT
# MSG::VERBOSE  (=1)
# MSG::DEBUG	(=2)
# MSG::INFO	    (=3)
# MSG::WARNING  (=4)
# MSG::ERROR	(=5)
# MSG::FATAL    (=6)
# MSG::ALWAYS   (=7)


