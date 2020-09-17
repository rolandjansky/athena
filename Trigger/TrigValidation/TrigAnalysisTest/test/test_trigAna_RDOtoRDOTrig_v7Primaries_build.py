#!/usr/bin/env python

# art-description: Test of the RDOtoRDOTrigger transform with serial athena (legacy trigger)
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps

preExec = ';'.join([
  'from TriggerJobOpts.TriggerFlags import TriggerFlags',
  'TriggerFlags.triggerMenuSetup=\'Physics_pp_v7_primaries\'',
  'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
])

ex = ExecStep.ExecStep()
ex.type = 'Reco_tf'
ex.input = 'ttbar'
ex.args = '--outputRDO_TRIGFile=RDO_TRIG.pool.root'
ex.args += ' --preExec="all:{:s};"'.format(preExec)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test, input_file='RDO_TRIG.pool.root')

import sys
sys.exit(test.run())
