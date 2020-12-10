#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Multi-threaded test of transform RDO->RDO_TRIG->ESD->AOD followed by HLT monitoring step with Run-3 DQ framework
# art-type: build
# art-include: master/Athena
# art-athena-mt: 4
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

preExec = ';'.join([
  'setMenu=\'LS2_v1_TriggerValidation_prescale\'',
  'from TriggerJobOpts.TriggerFlags import TriggerFlags',
  'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
])

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.threads = 4
rdo2aod.concurrent_events = 4
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" --valid=True'
rdo2aod.args += ' --preExec="all:{:s};"'.format(preExec)

dq = ExecStep.ExecStep('Run3DQ')
dq.type = 'other'
dq.executable = 'Run3DQTestingDriver.py'
dq.input = ''
dq.args = '--threads=4'
dq.args += ' --dqOffByDefault'
dq.args += ' Input.Files="[\'AOD.pool.root\']" DQ.Steering.doHLTMon=True'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [rdo2aod,dq]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp']]:
    step.input_file = 'ExampleMonitorOutput.root'

import sys
sys.exit(test.run())
