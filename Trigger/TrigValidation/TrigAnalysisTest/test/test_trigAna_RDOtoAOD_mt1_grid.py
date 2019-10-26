#!/usr/bin/env python

# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD with threads=1
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

rdo2aod = ExecStep.ExecStep()
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.threads = 1
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG"'
rdo2aod.args += ' --preExec="all:from TriggerJobOpts.TriggerFlags import TriggerFlags; TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\");"'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
