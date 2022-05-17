#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: Check that the menu and job configuration don't depend on input file
# art-type: build
# art-include: master/Athena
# art-include: 22.0/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

_menu_name = 'PhysicsP1_pp_run3_v1'

def diff_step(refdir, testdir, filename):
    '''Step comparing the config to reference'''
    refname = f'{refdir}/{filename}'
    testname = f'{testdir}/{filename}'
    compare = ExecStep.ExecStep(f'Diff.{filename}.{testdir}.{refdir}')
    compare.type = 'other'
    compare.input = ''
    compare.executable = '(diff'
    compare.args = testname + ' ' + refname
    msg_error = f'ERROR Configuration file {testname} differs from {refname}'
    msg_success = f'SUCCESS Configuration file {testname} is the same as {refname}'
    compare.args += f'; ret=$?; if [ $ret -ne 0 ]; then echo {msg_error}; else echo {msg_success}; fi; exit $ret)'
    compare.prmon = False
    return compare


def gen_config(input_name):
    dirname = input_name or 'nofile'

    # athenaHLT step to generate the job configuration
    ex = ExecStep.ExecStep(dirname)
    ex.type = 'athenaHLT'
    ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
    ex.input = input_name
    ex.args = f'-c "setMenu=\'{_menu_name}\';"'
    ex.args += ' -M --dump-config-exit'
    ex.perfmon = False  # Cannot use PerfMon with -M
    ex.fpe_auditor = False  # Don't want FPEAuditor in SMK for P1

    # Execute the athenaHLT step in a subdirectory
    ex.cmd_prefix = f'(mkdir -p {dirname} && pushd {dirname} && '
    ex.cmd_suffix = ' && popd)'

    steps = [ex]

    # Add step checking the configuration is identical as the 'nofile' version
    if input_name:
        for filename in ['HLTJobOptions.json',f'HLTMenu_{_menu_name}*.json',f'L1Menu_{_menu_name}*.json']:
            steps.append( diff_step('nofile',dirname,filename) )

    return steps

# Define the test
test = Test.Test()
test.art_type = 'build'
test.exec_steps = []
inputs = ['', 'data', 'data_run3']
for input_name in inputs:
    test.exec_steps.extend(gen_config(input_name))

# Only keep relevant checks from the defaults
test.check_steps = [chk for chk in CheckSteps.default_check_steps(test)
                    if type(chk) in [CheckSteps.LogMergeStep, CheckSteps.CheckLogStep]]

import sys
sys.exit(test.run())
