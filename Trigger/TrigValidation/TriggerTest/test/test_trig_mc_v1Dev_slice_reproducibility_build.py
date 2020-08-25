#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Compares results of a slice chains when running in full menu and when running alone
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.


from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Step
import json
import os


def hist_rename_pre_exec(file_name):
    pre_exec = ';'.join([
        "from GaudiSvc.GaudiSvcConf import THistSvc",
        "from AthenaCommon.AppMgr import ServiceMgr as svcMgr",
        "svcMgr += THistSvc()",
        "svcMgr.THistSvc.Output+=[\\\"EXPERT DATAFILE='{:s}' OPT='RECREATE'\\\"]".format(file_name),
    ])
    pre_exec += ';'
    return pre_exec


def generate_steps(slice_name = None):
    name = slice_name or 'FullMenu'
    # athena
    ex = ExecStep.ExecStep(name)
    ex.type = 'athena'
    ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
    ex.input = 'ttbar'
    ex.threads = 1
    hist_file_name = 'onlinemon_{:s}.root'.format(name)
    pre_exec = hist_rename_pre_exec(hist_file_name)
    if slice_name:
        pre_exec += 'doEmptyMenu=True;do{:s}Slice=True;'.format(slice_name)
    ex.args = '-c "setMenu=\'LS2_v1\';doWriteBS=False;doWriteRDOTrigger=False;{:s}"'.format(pre_exec)
    # chainDump
    cd = ExecStep.ExecStep('ChainDump' + name)
    cd.type = 'other'
    cd.executable = 'chainDump.py'
    cd.input = ''
    cd.args = '-f {:s} --json ChainDump.{:s}.json'.format(hist_file_name, name)
    cd.auto_report_result = False

    return [ex, cd]


class CompareSlicesToFullMenuStep( Step.Step ):
    def __init__( self, name='CompareSlicesToFullMenu' ):
        super( CompareSlicesToFullMenuStep, self ).__init__( name )
        self.ref_name = 'FullMenu'
        self.slice_names = None
        self.required = True
        self.auto_report_result = True

    def compare_counts(self, data, ref_key, slice_key, log_file):
        all_good = True
        for count_type in ['HLTChain', 'HLTDecision']:
            counts_ref = data[ref_key][count_type]['counts']
            counts_slice = data[slice_key][count_type]['counts']
            for item_name in counts_slice.keys():
                if not item_name.startswith('HLT_'):
                    continue  # Skip 'All', streams and groups
                slice_count = counts_slice[item_name]['count']
                ref_count = counts_ref[item_name]['count']
                if slice_count != ref_count:
                    all_good = False
                    log_file.write('ERROR {:s} count difference {:s}, {:s}: {:d}, {:s}: {:d}\n'.format(
                        count_type, item_name, slice_key, slice_count, ref_key, ref_count))
                else:
                    log_file.write('INFO {:s} count matches {:s}, {:s}: {:d}, {:s}: {:d}\n'.format(
                        count_type, item_name, slice_key, slice_count, ref_key, ref_count))
        return all_good

    def fail_run(self, cmd):
        cmd += ' -> failed'
        self.result = 1
        self.report_result()
        return self.result, cmd

    def run( self, dry_run=False ):
        self.log.info( 'Running %s comparing %s with slices %s ',
                       self.name, self.ref_name, str( self.slice_names ) )

        # Command to report in commands.json
        cmd = '# (internal) {}'.format(self.name)

        if dry_run:
            self.result = 0
            return self.result, cmd+' -> skipped'

        error = False
        counts_data = {}
        with open(self.get_log_file_name(), 'w') as log_file:
            for key in [self.ref_name] + self.slice_names:
                file_name = 'ChainDump.'+key+'.json'
                if not os.path.isfile(file_name):
                    log_file.write('ERROR the counts file {:s} does not exist\n'.format(file_name))
                    error = True
                    if key in slice_names:
                        slice_names.remove(key)
                    continue
                with open(file_name, 'r') as json_file:
                    counts_data[key] = json.load(json_file)
            if self.ref_name not in counts_data.keys():
                log_file.write('ERROR reference not loaded, cannot compare anything')
                return self.fail_run(cmd)
            for key in self.slice_names:
                same = self.compare_counts(counts_data, self.ref_name, key, log_file)
                if same:
                    log_file.write('INFO Counts for {:s} are consistent with {:s}\n'.format(key, self.ref_name))
                else:
                    log_file.write('ERROR Counts for {:s} differ from {:s}\n'.format(key, self.ref_name))
                    error = True

        if error:
            return self.fail_run(cmd)
        else:
            self.result = 0
            self.report_result()
            return self.result, cmd


# Test configuration
slice_names = ['Bjet', 'Bphysics', 'Egamma', 'Jet', 'MET', 'Muon', 'Tau']

test = Test.Test()
test.art_type = 'build'
test.exec_steps = generate_steps()  # Full menu
for name in slice_names:
    test.exec_steps.extend(generate_steps(name))

cross_check = CompareSlicesToFullMenuStep()
cross_check.slice_names = slice_names

merge_log = CheckSteps.LogMergeStep()
merge_log.merged_name = 'athena.merged.log'
merge_log.log_files = [ step.get_log_file_name() for step in test.exec_steps ]
merge_log.log_files.append(cross_check.get_log_file_name())

check_log = CheckSteps.CheckLogStep('CheckLog')
check_log.log_file = merge_log.merged_name

test.check_steps = [ cross_check, merge_log, check_log ]

import sys
sys.exit(test.run())
