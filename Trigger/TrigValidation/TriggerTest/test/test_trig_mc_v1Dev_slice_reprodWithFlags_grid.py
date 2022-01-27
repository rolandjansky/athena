#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Compares results of a slice chains when running in full menu and when running alone with other slices disabled by doXYZFlag=False
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.yml
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: prmon*
# art-output: *.check*


from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps


def hist_rename_pre_exec(file_name):
    pre_exec = ';'.join([
        "from GaudiSvc.GaudiSvcConf import THistSvc",
        "from AthenaCommon.AppMgr import ServiceMgr as svcMgr",
        "svcMgr += THistSvc()",
        "svcMgr.THistSvc.Output+=[\\\"EXPERT DATAFILE='{:s}' OPT='RECREATE'\\\"]".format(file_name),
    ])
    pre_exec += ';'
    return pre_exec


def generate_exec_steps(slice_name = None):
    name = slice_name or 'FullMenu'
    # athena
    ex = ExecStep.ExecStep(name)
    ex.type = 'athena'
    ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
    ex.input = 'ttbar'
    ex.threads = 1
    ex.max_events = 100
    hist_file_name = 'onlinemon_{:s}.root'.format(name)
    pre_exec = hist_rename_pre_exec(hist_file_name)
    if slice_name:
        pre_exec += 'doEmptyMenu=True;do{:s}Slice=True;'.format(slice_name)
    ex.args = '-c "setMenu=\'Dev_pp_run3_v1\';doWriteBS=False;doWriteRDOTrigger=False;{:s}"'.format(pre_exec)
    # chainDump
    cd = ExecStep.ExecStep('ChainDump' + name)
    cd.type = 'other'
    cd.executable = 'chainDump.py'
    cd.input = ''
    cd.args = '-f {:s} --yaml ChainDump.{:s}.yml'.format(hist_file_name, name)
    cd.auto_report_result = False
    cd.prmon = False

    return [ex, cd]


def generate_chaincomp_step(slice_name):
    step = CheckSteps.ChainCompStep('ChainComp.'+slice_name)
    step.args += ' --matchingOnly'
    step.required = True
    step.input_file = 'ChainDump.{:s}.yml'.format(slice_name)
    step.reference = 'ChainDump.FullMenu.yml'
    step.explicit_reference = True
    return step


# Test configuration
slice_names = ['Bjet', 'Bphysics', 'Egamma', 'Jet', 'MET', 'Muon', 'Tau']

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = generate_exec_steps()  # Full menu
for name in slice_names:
    test.exec_steps.extend(generate_exec_steps(name))

merge_log = CheckSteps.LogMergeStep()
merge_log.merged_name = 'athena.merged.log'
merge_log.log_files = [ step.get_log_file_name() for step in test.exec_steps ]

check_log = CheckSteps.CheckLogStep('CheckLog')
check_log.log_file = merge_log.merged_name

chain_comp_steps = [generate_chaincomp_step(name) for name in slice_names]

test.check_steps = [ merge_log, check_log ]
test.check_steps.extend(chain_comp_steps)

import sys
sys.exit(test.run())
