include.block("TrigSteering/jobOfragment_HLTSSW_factoredEF_New.py")
#--------------------------------------------------
# TrigSteering Algorithm
#-------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

from TrigSteering.TrigSteeringConf import HLT__TrigSteer
TrigSteer_EF = HLT__TrigSteer( "TrigSteer_EF" )
TopAlg += TrigSteer_EF


#theApp.TopAlg += [ "HLT::TrigSteer/TrigSteer_EF" ]
#TrigSteer_EF = Algorithm( "HLT::TrigSteer/TrigSteer_EF" )
TrigSteer_EF.OutputLevel=VERBOSE
TrigSteer_EF.HLTLevel="EF"

from TrigSteering.TestingTrigSteeringConfig import TestingLvl2Converter
TrigSteer_EF.LvlConverterTool = TestingLvl2Converter()

from TrigSteering.TestingTrigSteeringConfig import TestingResultBuilder
TrigSteer_EF.ResultBuilder = TestingResultBuilder()

from TrigNavigation.TrigNavigationConfig import TestingHLTNavigationOffline
Navigation = TestingHLTNavigationOffline()
TrigSteer_EF.Navigation = Navigation

# load configrables of HLT algorithms
# the HLTConfigSvc must already be declared and setup
l2algs, efalgs = HLTConfigSvc.getAlgorithmsByLevel()
print "INFO Will add EF algorithms to TrigSteer_EF: ", efalgs

for name,instance in Configurable.allConfigurables.iteritems():
    if instance.getFullName() in efalgs:
        TrigSteer_EF += instance
        print "INFO added: ", instance.getFullName()
