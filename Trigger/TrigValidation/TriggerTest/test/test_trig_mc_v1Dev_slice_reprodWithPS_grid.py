#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Compares results of a slice chains when running in full menu and when running alone with other slices disabled by prescaling
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


from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Step, PyStep
import yaml
import json
import glob

_slice_dict = {
    'Bjet': ['BW:BJet'],
    'Bphysics': ['BW:Bphysics', 'BW:BphysicsElectron'],
    'Egamma': ['BW:Egamma','BW:Electron','BW:Photon'],
    'Jet': ['BW:Jet'],
    'MET': ['BW:MET','BW:MultiMET'],
    'Muon': ['BW:Muon'],
    'Tau': ['BW:Tau'],
    'Other': ['BW:BeamSpot','BW:Detector','BW:DISCARD','BW:MinBias','BW:Other','BW:ZeroBias']
}


class CopyStep(ExecStep.ExecStep):
    '''Simply call cp in_file out_file'''
    def __init__(self, name, in_file, out_file):
        super(CopyStep, self).__init__(name)
        self.type = 'other'
        self.input = ''
        self.executable = 'cp'
        self.args = ' {:s} {:s}'.format(in_file, out_file)
        self.auto_report_result = False  # Do not set art-result for this step
        self.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step
        self.prmon = False


class MoveStep(ExecStep.ExecStep):
    '''Simply call mv in_file out_file'''
    def __init__(self, name, in_file, out_file):
        super(MoveStep, self).__init__(name)
        self.type = 'other'
        self.input = ''
        self.executable = 'mv'
        self.args = ' {:s} {:s}'.format(in_file, out_file)
        self.auto_report_result = False  # Do not set art-result for this step
        self.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step
        self.prmon = False


class UpdatePrescalesStep(ExecStep.ExecStep):
    '''Prescale out all chains except those in the given groups'''
    def __init__(self, name, group_names):
        super(UpdatePrescalesStep, self).__init__(name)
        self.type = 'other'
        self.input = ''
        self.executable = 'trigmenu_modify_prescale_json.py'
        self.args = ' -p HLTPrescalesSet_*.json -m HLTMenu_*.json'
        for group in group_names:
            self.args += ' -g ' + group
        self.auto_report_result = False  # Do not set art-result for this step
        self.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step
        self.prmon = False


class StripCountsStep(PyStep.PyStep):
    def __init__(self, name, counts_file, prescales_file):
        super(StripCountsStep, self).__init__(remove_disabled_counts, name=name, counts_file=counts_file, ps_file_pattern=prescales_file)
        self.auto_report_result = False  # Do not set art-result for this step
        self.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step
        self.prmon = False


def remove_disabled_counts(counts_file, ps_file_pattern):
    ps_files = glob.glob(ps_file_pattern)
    if not ps_files:
        raise RuntimeError('Prescales file ' + ps_file_pattern + ' not found')
    ps_file = ps_files[0]
    with open(counts_file, 'r') as fp:
        counts = yaml.safe_load(fp)
    with open(ps_file) as fp:
        prescales = json.load(fp)
    new_counts = dict()
    for chain in counts:
        if prescales['prescales'][chain]['enabled']:
            new_counts[chain] = counts[chain]
    with open(counts_file, 'w') as fp:
        yaml.dump(new_counts, fp)


def generate_config_steps():
    # athena
    ex = ExecStep.ExecStep('ConfigOnly')
    ex.config_only = True
    ex.type = 'athena'
    ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
    ex.input = 'ttbar'
    ex.threads = 1
    ex.max_events = 100
    ex.args = '-c "setMenu=\'Dev_pp_run3_v1\';doWriteBS=False;doWriteRDOTrigger=False;"'
    ex.perfmon = False
    # Make a copy of the default prescales file
    copy_ps = CopyStep('CopyPrescales.Default', 'HLTPrescalesSet*.json', 'prescales_Default.json')
    return [ex, copy_ps]


def generate_exec_steps(slice_name = None):
    name = slice_name or 'FullMenu'
    steps = []
    if slice_name:
        # Reset prescales to FullMenu ones and update them for the slice
        copy_ps = CopyStep('CopyPrescales.'+name, 'prescales_Default.json', 'HLTPrescalesSet*.json')
        upd_ps = UpdatePrescalesStep('UpdatePrescales.'+name, _slice_dict[name])
        steps.extend([copy_ps, upd_ps])

    # athena
    ex = ExecStep.ExecStep(name)
    ex.type = 'athena'
    ex.input = ''
    ex.use_pickle = True
    ex.job_options = 'athena.ConfigOnly.pkl'
    ex.threads = 1
    ex.max_events = 100
    ex.depends_on_previous = False if slice_name else True
    ex.perfmon = False

    # Rename the online histograms file
    hist_file_name = 'onlinemon_{:s}.root'.format(name)
    mv_hist = MoveStep('RenameHistFile.' + name, 'expert-monitoring.root', hist_file_name)

    # chainDump
    counts_file_name = 'ChainDump.{:s}.yml'.format(name)
    cd = ExecStep.ExecStep('ChainDump.' + name)
    cd.type = 'other'
    cd.executable = 'chainDump.py'
    cd.input = ''
    cd.args = '-f {:s} --yaml {:s}'.format(hist_file_name, counts_file_name)
    cd.auto_report_result = False
    cd.prmon = False

    # Remove zero-counts of disabled chains
    strip = StripCountsStep('StripCounts.' + name, counts_file_name, 'HLTPrescalesSet*.json')

    steps.extend([ex, mv_hist, cd, strip])
    return steps


def generate_chaincomp_step(slice_name):
    step = CheckSteps.ChainCompStep('ChainComp.'+slice_name)
    step.args += ' --matchingOnly'
    step.required = True
    step.input_file = 'ChainDump.{:s}.yml'.format(slice_name)
    step.reference = 'ChainDump.FullMenu.yml'
    step.explicit_reference = True
    return step


# Test configuration
test = Test.Test()
test.art_type = 'grid'
test.exec_steps = generate_config_steps()
test.exec_steps.extend(generate_exec_steps())  # Full menu
for name in _slice_dict:
    test.exec_steps.extend(generate_exec_steps(name))

merge_log = CheckSteps.LogMergeStep()
merge_log.merged_name = 'athena.merged.log'
merge_log.log_files = [ step.get_log_file_name() for step in test.exec_steps if step.output_stream != Step.Step.OutputStream.STDOUT_ONLY]

check_log = CheckSteps.CheckLogStep('CheckLog')
check_log.log_file = merge_log.merged_name

chain_comp_steps = [generate_chaincomp_step(name) for name in _slice_dict]

test.check_steps = [ merge_log, check_log ]
test.check_steps.extend(chain_comp_steps)

import sys
sys.exit(test.run())
