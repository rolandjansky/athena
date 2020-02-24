#!/usr/bin/env python

# art-description: Same as full_menu test from TrigUpgradeTest, but with athenaHLT. Test with detector ROBs removed. 
# art-type: build                                                                  
# art-include: master/Athena                                                       

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.args = '--ros2rob="{\'ROS-TDQ-CTP-00\': [0x770000]}"'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
