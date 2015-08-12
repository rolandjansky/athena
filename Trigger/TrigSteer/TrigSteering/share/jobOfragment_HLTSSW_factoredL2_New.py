include.block("TrigSteering/jobOfragment_HLTSSW_factoredL2_New.py")
##--------------------------------------------------
# TrigSteering Algorithm
#-------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

from TrigSteering.TrigSteeringConf import HLT__TrigSteer
TrigSteer_L2 = HLT__TrigSteer( "TrigSteer_L2" )
TopAlg += TrigSteer_L2


#theApp.TopAlg += [ "HLT::TrigSteer/TrigSteer_L2" ]
#TrigSteer_L2 = Algorithm( "HLT::TrigSteer/TrigSteer_L2" )
TrigSteer_L2.OutputLevel=VERBOSE
TrigSteer_L2.HLTLevel="L2"

from TrigSteering.TestingTrigSteeringConfig import TestingLvl1Converter
TrigSteer_L2.LvlConverterTool = TestingLvl1Converter('Lvl1Converter')

from TrigSteering.TestingTrigSteeringConfig import TestingResultBuilder
TrigSteer_L2.ResultBuilder = TestingResultBuilder()

from TrigNavigation.TrigNavigationConfig import TestingHLTNavigationOffline
Navigation = TestingHLTNavigationOffline()
TrigSteer_L2.Navigation = Navigation



# load configrables of HLT algorithms
# the HLTConfigSvc must already be declared and setup
l2algs, efalgs = HLTConfigSvc.getAlgorithmsByLevel()
print "INFO Will add L2 algorithms to TrigSteer_L2: ", l2algs

for name,instance in Configurable.allConfigurables.iteritems():
    if instance.getFullName() in l2algs:
        TrigSteer_L2 += instance
        print "INFO added: ", instance.getFullName()
