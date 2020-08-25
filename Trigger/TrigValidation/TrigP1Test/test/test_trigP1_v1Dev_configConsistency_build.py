#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Compare athenaHLT configuration with Dev_pp_run3_v1 menu and different threads/slots/forks settings
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Step


def make_exec_steps(nforks, nthreads, nslots):
    id_str = '{:d}f_{:d}s_{:d}t'.format(nforks, nthreads, nslots)

    # Step dumping athenaHLT config
    ex = ExecStep.ExecStep('Config.'+id_str)
    ex.type = 'athenaHLT'
    ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
    ex.input = 'data'
    ex.forks = nforks
    ex.threads = nthreads
    ex.concurrent_events = nslots
    ex.perfmon = False
    ex.prmon = False
    ex.args = '--dump-config-exit'
    ex.args += ' -c "setMenu=\'LS2_v1\';"'  # LS2_v1 soon to be renamed to Dev_pp_run3_v1

    # Step renaming the config dump
    rename = ExecStep.ExecStep('RenameConfigDump.'+id_str)
    rename.type = 'other'
    rename.input = ''
    rename.executable = 'mv'
    rename.args = 'HLTJobOptions.json HLTJobOptions.'+id_str+'.json'
    rename.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # no log file for this step
    rename.auto_report_result = False  # no art-result for this step

    # Step comparing the config to reference (hard-coded name with 1 fork, 1 thread, 1 slot)
    refname = 'HLTJobOptions.1f_1s_1t.json'
    testname = 'HLTJobOptions.'+id_str+'.json'
    compare = ExecStep.ExecStep('CompareConfig.'+id_str)
    compare.type = 'other'
    compare.input = ''
    compare.executable = '(diff'
    compare.args = testname + ' ' + refname
    msg_base = 'Configuration with forks/threads/slots = {:d}/{:d}/{:d}'.format(nforks, nthreads, nslots)
    msg_error = 'ERROR ' + msg_base + ' differs from configuration with 1/1/1'
    msg_success = 'SUCCESS ' + msg_base + ' is the same as with 1/1/1'
    compare.args += '; ret=$?; if [ $ret -ne 0 ]; then echo "{:s}"; else echo "{:s}"; fi; exit $ret)'.format(msg_error, msg_success)

    return [ex, rename, compare]


# Configure the test with different forks/threads/slots combinations
test = Test.Test()
test.art_type = 'build'
test.exec_steps = []
test.exec_steps.extend(make_exec_steps(1, 1, 1))
test.exec_steps.extend(make_exec_steps(2, 1, 1))
test.exec_steps.extend(make_exec_steps(1, 2, 1))
test.exec_steps.extend(make_exec_steps(1, 2, 2))
test.exec_steps.extend(make_exec_steps(4, 8, 1))
test.exec_steps.extend(make_exec_steps(8, 4, 2))

# Select a few relevant check steps from defaults
test.check_steps = [chk for chk in CheckSteps.default_check_steps(test)
                    if type(chk) in (CheckSteps.LogMergeStep, CheckSteps.CheckLogStep, CheckSteps.ZipStep)]

import sys
sys.exit(test.run())
