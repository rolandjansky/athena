#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of the RDOtoRDOTrigger transform with threads=1
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

preExec = ';'.join([
  'setMenu=\'LS2_v1_TriggerValidation_prescale\'',
  'from TriggerJobOpts.TriggerFlags import TriggerFlags',
  'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
  'doRuntimeNaviVal=True', # Perform runtime graph vaidation in this test
])

ex = ExecStep.ExecStep()
ex.type = 'Reco_tf'
ex.input = 'ttbar'
ex.threads = 1
ex.args = '--outputRDO_TRIGFile=RDO_TRIG.pool.root'
ex.args += ' --preExec="all:{:s};"'.format(preExec)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Add a step comparing counts against a reference
chaindump = test.get_step("ChainDump")
chaindump.args = '--json --yaml ref_RDOtoRDOTrig_v1Dev_build.new'
refcomp = CheckSteps.ChainCompStep("CountRefComp")
refcomp.input_file = 'ref_RDOtoRDOTrig_v1Dev_build.new'
refcomp.required = True # Final exit code depends on this step
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.ChainDumpStep, refcomp)

import sys
sys.exit(test.run())
