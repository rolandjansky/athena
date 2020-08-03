#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: athenaHLT test of the Dev_pp_run3_v1 menu imitating partition with preloaded data at P1
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Input, Test, Step, ExecStep
from TrigP1Test import TrigP1TestSteps

# Input file
f = str(Input.get_input('data').paths[0])

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
ex_bs.args = '-n 50 --runNumber 999999 --incLB 6 --firstLB 4 --eventsPerLB=3 -o raw %s' % f

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
ex.args = '-f ./raw._0001.data'
ex.args += ' -c "setMenu=\'LS2_v1\';"'  # LS2_v1 to be renamed to Dev_pp_run3_v1
ex.args += ' -C "from AthenaCommon.AppMgr import ServiceMgr; ServiceMgr.HltEventLoopMgr.forceRunNumber=%d; ServiceMgr.HltEventLoopMgr.forceStartOfRunTime=%d"' % (run, sor)
ex.args += ' -R 999999 --sor-time=now --detector-mask=all'
ex.args += ' -M --dump-config'  # For SMK generation
ex.perfmon = False  # Cannot use PerfMon with -M

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex_rm, ex_bs, ex]
test.check_steps = TrigP1TestSteps.default_check_steps_OHMon(test, 'r0000999999_athenaHLT_HLT-Histogramming.root:run_999999/lb_-1')

import sys
sys.exit(test.run())
