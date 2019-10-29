#!/usr/bin/env python

# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD with threads=1 followed by AOD->NTUP_PHYSVAL with serial athena
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
import os

# To run single-process transform on MCORE sites
if 'ATHENA_NPROC_NUM' in os.environ:
    del os.environ['ATHENA_NPROC_NUM']

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.threads = 1
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" --valid=True'
rdo2aod.args += ' --preExec="all:from TriggerJobOpts.TriggerFlags import TriggerFlags; TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\");"'
rdo2aod.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'

tzmon = ExecStep.ExecStep('Tier0Mon')
tzmon.type = 'other'
tzmon.executable = 'Run3DQTestingDriver.py'
tzmon.input = ''
tzmon.explicit_input = True
tzmon.args = '--dqOffByDefault Input.Files="[\'AOD.pool.root\']" DQ.Steering.doHLTMon=True'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod,tzmon]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp']]:
    step.input_file = 'ExampleMonitorOutput.root'

import sys
sys.exit(test.run())
