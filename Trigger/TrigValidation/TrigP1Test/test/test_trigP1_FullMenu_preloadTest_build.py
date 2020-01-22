#!/usr/bin/env python
#
# art-description: Run athenaHLT to test preloaded partition running
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Input, Test, Step, ExecStep, CheckSteps

# Input file
f = Input.get_input('data').paths[0].encode('ascii', 'ignore')

# Delete any previous bytestream file
ex_rm = ExecStep.ExecStep('cleanup')
ex_rm.type = 'other'
ex_rm.input = ''
ex_rm.executable = 'rm'
ex_rm.args = '-f raw._0001.data'
ex_rm.auto_report_result = False  # Do not set art-result for this step
ex_rm.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step

# Create new bytestream file with future run number and renumber LBs
ex_bs = ExecStep.ExecStep('create_bs')
ex_bs.type = 'other'
ex_bs.input = ''
ex_bs.executable = 'trigbs_modifyEvent.py'
ex_bs.args = '-n 50 --runNumber 999999 --incLB 3 --firstLB 4 -o raw %s' % f

# Run athenaHLT forcing run and timestamp of original data file
import eformat
from TrigCommon import AthHLT
run = eformat.EventStorage.pickDataReader(f).runNumber()
sor = AthHLT.get_sor_params(run)['SORTime']

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = ''
ex.explicit_input = True
ex.args = '-f ./raw._0001.data -C "from AthenaCommon.AppMgr import ServiceMgr; ServiceMgr.HltEventLoopMgr.forceRunNumber=%d; ServiceMgr.HltEventLoopMgr.forceStartOfRunTime=%d" -R 999999 --sor-time=now --detector-mask=all' % (run, sor)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex_rm, ex_bs, ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
