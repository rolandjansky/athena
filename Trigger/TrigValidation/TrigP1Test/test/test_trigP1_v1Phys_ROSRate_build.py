#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger athenaHLT test for ROS rates estimate with the Physics_pp_run3_v1 menu
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Step

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.max_events = 200
ex.args = '-c "setMenu=\'Physics_pp_run3_v1\';"'
ex.args += ' --ros2rob /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/ATLASros2rob2018-r22format.py'

ros2json = CheckSteps.InputDependentStep("RosRateToJson")
ros2json.executable = 'ros-hitstats-to-json.py'
ros2json.input_file = 'ros_hitstats_reject.txt'
ros2json.output_stream = Step.Step.OutputStream.STDOUT_ONLY

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)
test.check_steps.append(ros2json)

import sys
sys.exit(test.run())
