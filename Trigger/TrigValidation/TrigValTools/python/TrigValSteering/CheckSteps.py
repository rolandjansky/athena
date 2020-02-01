#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of post-exec check steps in Trigger ART tests
'''

import sys
import os
import re
import subprocess
import json
import six

from TrigValTools.TrigValSteering.Step import Step
from TrigValTools.TrigValSteering.Common import art_input_eos, art_input_cvmfs


class RefComparisonStep(Step):
    '''Base class for steps comparing a file to a reference'''

    def __init__(self, name):
        super(RefComparisonStep, self).__init__(name)
        self.reference = None
        self.input_file = None

    def configure(self, test):
        if self.reference is not None:
            # Do nothing if the reference exists
            if os.path.isfile(self.reference):
                return
            # Try to find the file in DATAPATH
            full_path = subprocess.check_output('find_data.py {}'.format(self.reference), shell=True).strip()
            if os.path.isfile(full_path):
                self.log.debug('%s using reference %s', self.name, full_path)
                self.reference = full_path
                return
            else:
                self.log.warning(
                    '%s failed to find reference %s - wrong path?',
                    self.name, self.reference)
                return

        if self.input_file is None:
            self.log.error('Cannot configure %s because input_file not specified',
                           self.name)
            self.report_result(1, 'TestConfig')
            sys.exit(1)

        branch = os.environ.get('AtlasBuildBranch')  # Available after asetup
        if branch is None:
            branch = os.environ.get('gitlabTargetBranch')  # Available in CI
        if branch is None:
            self.log.warning('Cannot determine the branch name, both variables '
                             'AtlasBuildBranch and gitlabTargetBranch are empty')
            branch = 'UNKNOWN_BRANCH'

        sub_path = '{}/ref/{}/test_{}/'.format(
            test.package_name, branch, test.name)
        ref_eos = art_input_eos + sub_path + self.input_file
        ref_cvmfs = art_input_cvmfs + sub_path + self.input_file
        if os.path.isfile(ref_eos):
            self.log.debug('%s using reference from EOS: %s',
                           self.name, ref_eos)
            self.reference = ref_eos
        elif os.path.isfile(ref_cvmfs):
            self.log.debug('%s using reference from CVMFS: %s',
                           self.name, ref_cvmfs)
            self.reference = ref_cvmfs
        else:
            self.log.warning('%s failed to find reference %s in %s or %s',
                             self.name, sub_path + self.input_file,
                             art_input_eos, art_input_cvmfs)
            self.reference = None

        super(RefComparisonStep, self).configure(test)


class InputDependentStep(Step):
    '''Base class for steps executed only if the input file exists'''

    def __init__(self, name=None):
        super(InputDependentStep, self).__init__(name)
        self.input_file = None

    def run(self, dry_run=False):
        if self.input_file is None:
            self.log.error('%s misconfiguration - no input file specified',
                           self.name)
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, '# (internal) {} -> failed'.format(self.name)

        if not os.path.isfile(self.input_file):
            self.log.debug('Skipping %s because %s does not exist',
                           self.name, self.input_file)
            self.result = 0
            return self.result, '# (internal) {} -> skipped'.format(self.name)

        return super(InputDependentStep, self).run(dry_run)


class LogMergeStep(Step):
    '''Merge several log files into one for post-processing'''

    def __init__(self, name='LogMerge'):
        super(LogMergeStep, self).__init__(name)
        self.log_files = None
        self.extra_log_regex = None
        self.merged_name = 'athena.merged.log'
        self.warn_if_missing = True

    def configure(self, test):
        if self.log_files is None:
            self.log_files = []
            for step in test.exec_steps:
                self.log_files.append(step.name)
        super(LogMergeStep, self).configure(test)

    def process_extra_regex(self):
        if self.extra_log_regex:
            files = os.listdir('.')
            r = re.compile(self.extra_log_regex)
            match_files = filter(r.match, files)
            for f in match_files:
                self.log_files.append(f)

    def merge_logs(self):
        encargs = {} if six.PY2 else {'encoding' : 'utf-8'}
        try:
            with open(self.merged_name, 'w', **encargs) as merged_file:
                for log_name in self.log_files:
                    if not os.path.isfile(log_name):
                        if self.warn_if_missing:
                            self.log.warning('Cannot open %s', log_name)
                            merged_file.write(
                                '### WARNING Missing {} ###\n'.format(log_name))
                        continue
                    with open(log_name, **encargs) as log_file:
                        merged_file.write('### {} ###\n'.format(log_name))
                        for line in log_file:
                            merged_file.write(line)
            return 0
        except OSError as e:
            self.log.error('%s merging failed due to OSError: %s',
                           self.name, e.strerror)
            return 1

    def run(self, dry_run=False):
        self.process_extra_regex()
        self.log.info('Running %s merging logs %s into %s',
                      self.name, self.log_files, self.merged_name)
        if dry_run:
            self.result = 0
        else:
            self.result = self.merge_logs()
        return self.result, '# (internal) {} in={} out={}'.format(self.name, self.log_files, self.merged_name)


class RootMergeStep(Step):
    '''
    Merge root files with hadd. Parameters are:
    input_file - file(s) to be merged
    merged_file - output file name
    rename_suffix - if merged_file exists, it is renamed by adding this suffix
    '''

    def __init__(self, name='RootMerge'):
        super(RootMergeStep, self).__init__(name)
        self.input_file = None
        self.merged_file = None
        self.rename_suffix = None
        self.executable = 'hadd'

    def configure(self, test=None):
        self.args += ' ' + self.merged_file + ' ' + self.input_file
        super(RootMergeStep, self).configure(test)

    def run(self, dry_run=False):
        if os.path.isfile(self.merged_file) and self.rename_suffix:
            old_name = os.path.splitext(self.merged_file)
            new_name = old_name[0] + self.rename_suffix + old_name[1]
            self.executable = 'mv {} {}; {}'.format(self.merged_file, new_name, self.executable)
        return super(RootMergeStep, self).run(dry_run)


class ZipStep(Step):
    '''Compress a large log file'''

    def __init__(self, name='Zip'):
        super(ZipStep, self).__init__(name)
        self.zip_output = None
        self.zip_input = None
        self.executable = 'tar'
        self.args = '-czf'
        self.output_stream = Step.OutputStream.STDOUT_ONLY

    def configure(self, test=None):
        self.args += ' '+self.zip_output+' '+self.zip_input
        # Remove the file after zipping
        self.args += ' && rm ' + self.zip_input
        super(ZipStep, self).configure(test)


class CheckLogStep(Step):
    '''Execute CheckLog looking for errors or warnings in a log file'''

    def __init__(self, name):
        super(CheckLogStep, self).__init__(name)
        self.executable = 'check_log.py'
        self.log_file = None
        self.check_errors = True
        self.check_warnings = False
        self.config_file = None
        self.args = '--showexcludestats'

    def configure(self, test):
        if self.config_file is None:
            if test.package_name == 'TrigUpgradeTest':
                self.config_file = 'checklogTrigUpgradeTest.conf'
            elif test.package_name == 'TrigP1Test':
                self.config_file = 'checklogTrigP1Test.conf'
            elif test.package_name == 'TrigValTools':
                self.config_file = 'checklogTrigValTools.conf'
            else:
                self.config_file = 'checklogTriggerTest.conf'
        if self.log_file is None:
            if len(test.exec_steps) == 1:
                self.log_file = test.exec_steps[0].name+'.log'
            else:
                self.log_file = 'athena.log'
        if self.check_errors:
            self.args += ' --errors'
        if self.check_warnings:
            self.args += ' --warnings'
        if self.check_errors and not self.check_warnings:
            self.output_stream = Step.OutputStream.FILE_AND_STDOUT
            self.auto_report_result = True
            self.required = True

        self.args += ' --config {} {}'.format(self.config_file, self.log_file)

        super(CheckLogStep, self).configure(test)


class RegTestStep(RefComparisonStep):
    '''Execute RegTest comparing a log file against a reference'''

    def __init__(self, name='RegTest'):
        super(RegTestStep, self).__init__(name)
        self.regex = 'REGTEST'
        self.executable = 'regtest.pl'
        self.input_base_name = 'athena'
        self.args += ' --linematch ".*"'
        self.auto_report_result = True
        self.output_stream = Step.OutputStream.FILE_AND_STDOUT

    def configure(self, test):
        self.input_file = self.input_base_name+'.regtest'
        RefComparisonStep.configure(self, test)
        self.args += ' --inputfile {} --reffile {}'.format(self.input_file, self.reference)
        Step.configure(self, test)

    def prepare_inputs(self):
        log_file = self.input_base_name+'.log'
        if not os.path.isfile(log_file):
            self.log.error('%s input file %s is missing', self.name, log_file)
            return False
        encargs = {} if six.PY2 else {'encoding' : 'utf-8'}
        with open(log_file, **encargs) as f_in:
            matches = re.findall('{}.*$'.format(self.regex),
                                 f_in.read(), re.MULTILINE)
            with open(self.input_file, 'w', **encargs) as f_out:
                for line in matches:
                    f_out.write(line+'\n')
        return True

    def rename_ref(self):
        try:
            if self.reference:
                new_name = os.path.basename(self.reference) + '.new'
            else:
                new_name = os.path.basename(self.input_file) + '.new'
            os.rename(self.input_file, new_name)
            self.log.debug('Renamed %s to %s', self.input_file, new_name)
        except OSError:
            self.log.warning('Failed to rename %s to %s',
                             self.input_file, new_name)

    def run(self, dry_run=False):
        if not dry_run and not self.prepare_inputs():
            self.log.error('%s failed in prepare_inputs()', self.name)
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, '# (internal) {} -> failed'.format(self.name)
        if self.reference is None:
            self.log.error('Missing reference for %s', self.name)
            if not dry_run:
                self.rename_ref()
            self.result = 999
            if self.auto_report_result:
                self.report_result()
            return self.result, '# (internal) {} -> failed'.format(self.name)
        retcode, cmd = super(RegTestStep, self).run(dry_run)
        if not dry_run:
            self.rename_ref()
        return retcode, cmd


class RootCompStep(RefComparisonStep):
    '''Execute RootComp comparing histograms against a reference'''

    def __init__(self, name='RootComp'):
        super(RootCompStep, self).__init__(name)
        self.input_file = 'expert-monitoring.root'
        self.executable = 'rootcomp.py'
        self.auto_report_result = True

    def configure(self, test):
        RefComparisonStep.configure(self, test)
        self.args += ' {} {}'.format(self.reference, self.input_file)
        Step.configure(self, test)

    def run(self, dry_run=False):
        if self.reference is None:
            if not os.path.isfile(self.input_file):
                self.log.debug(
                    'Skipping %s because both reference and input are missing',
                    self.name)
                return 0, '# (internal) {} -> skipped'.format(self.name)
            else:  # input exists but reference not
                self.log.error('Missing reference for %s', self.name)
                self.result = 999
                if self.auto_report_result:
                    self.report_result()
                return self.result, '# (internal) {} -> failed'.format(self.name)
        retcode, cmd = super(RootCompStep, self).run(dry_run)
        return retcode, cmd


class PerfMonStep(InputDependentStep):
    '''Execute the PerfMon ntuple post-processing'''

    def __init__(self, name='PerfMon'):
        super(PerfMonStep, self).__init__(name)
        self.input_file = None
        self.executable = 'perfmon.py'
        self.args = '-f 0.90'

    def configure(self, test):
        if not self.input_file:
            num_athenaHLT_steps = sum([1 for step in test.exec_steps if step.type == 'athenaHLT'])
            if num_athenaHLT_steps > 0:
                self.input_file = 'athenaHLT_workers/athenaHLT-01/ntuple.pmon.gz'
            else:
                self.input_file = 'ntuple.pmon.gz'
        self.args += ' '+self.input_file
        super(PerfMonStep, self).configure(test)


class TailStep(Step):
    '''Copy the last N lines of a log file into a separate file'''

    def __init__(self, name='Tail'):
        super(TailStep, self).__init__(name)
        self.log_file = 'athena.log'
        self.output_name = None
        self.executable = 'tail'
        self.num_lines = 5000
        self.output_stream = Step.OutputStream.STDOUT_ONLY

    def configure(self, test):
        if self.output_name is None:
            split = os.path.splitext(self.log_file)
            self.output_name = split[0]+'.tail'
            if len(split) > 1:
                self.output_name += split[1]
        self.args += ' -n {:d}'.format(self.num_lines)
        self.args += ' '+self.log_file
        self.args += ' >'+self.output_name
        super(TailStep, self).configure(test)


class HistCountStep(InputDependentStep):
    '''Execute histSizes.py to count histograms in a ROOT file'''

    def __init__(self, name='HistCount'):
        super(HistCountStep, self).__init__(name)
        self.input_file = 'expert-monitoring.root'
        self.executable = 'histSizes.py'
        self.args = '-t'

    def configure(self, test):
        self.args += ' '+self.input_file
        super(HistCountStep, self).configure(test)


class ChainDumpStep(InputDependentStep):
    '''
    Execute chainDump.py to print trigger counts from histograms to text files
    '''

    def __init__(self, name='ChainDump'):
        super(ChainDumpStep, self).__init__(name)
        self.input_file = 'expert-monitoring.root'
        self.executable = 'chainDump.py'
        self.args = '--json'

    def configure(self, test):
        self.args += ' -f '+self.input_file
        super(ChainDumpStep, self).configure(test)


class TrigTestJsonStep(Step):
    '''Execute trig-test-json.py to create extra-results.json file'''

    def __init__(self, name='TrigTestJson'):
        super(TrigTestJsonStep, self).__init__(name)
        self.executable = 'trig-test-json.py'


class CheckFileStep(InputDependentStep):
    '''
    Execute checkFile and checkxAOD for POOL files.
    executable and input_file can have multiple comma-separated values
    '''

    def __init__(self, name='CheckFile'):
        super(CheckFileStep, self).__init__(name)
        self.input_file = 'AOD.pool.root,ESD.pool.root,RDO_TRIG.pool.root'
        self.executable = 'checkFile.py,checkxAOD.py'
        self.__executables__ = None
        self.__input_files__ = None

    def configure(self, test):
        # Skip the check if all test steps are athenaHLT (no POOL files)
        test_types = [step.type for step in test.exec_steps]
        num_athenaHLT = sum(1 for tt in test_types if tt == 'athenaHLT')
        if num_athenaHLT == test_types:
            self.log.debug('%s will be skipped because all exec steps use athenaHLT')
            self.__executables__ = None
            self.__input_files__ = None
            return
        self.__executables__ = self.executable.split(',')
        self.__input_files__ = self.input_file.split(',')
        super(CheckFileStep, self).configure(test)

    def run(self, dry_run=False):
        ret_codes = []
        commands = []
        for f in self.__input_files__:
            for ex in self.__executables__:
                self.executable = ex
                self.input_file = f
                self.args = f
                ex_base = ex.split('.')[0:-1]
                self.log_file_name = f + '.' + ''.join(ex_base)
                ret, cmd = super(CheckFileStep, self).run(dry_run)
                ret_codes.append(ret)
                commands.append(cmd)

        # Merge executed commands for logging
        merged_cmd = ''
        if len(commands) == 1:
            merged_cmd = commands[0]
        else:
            for cmd in commands:
                if '(internal)' not in cmd:
                    merged_cmd += cmd+'; '
        if len(merged_cmd) == 0:
            merged_cmd = commands[-1]

        return max(ret_codes), merged_cmd


class ZeroCountsStep(Step):
    '''
    Check if all counts are zero.
    input_file can have multiple comma-separated values
    '''

    def __init__(self, name='ZeroCounts'):
        super(ZeroCountsStep, self).__init__(name)
        self.input_file = 'HLTChain.txt,HLTTE.txt,L1AV.txt'
        self.auto_report_result = True
        self.__input_files__ = None

    def configure(self, test=None):
        self.__input_files__ = self.input_file.split(',')

    def check_zero_counts(self, input_file):
        if not os.path.isfile(input_file):
            self.log.debug(
                'Skipping %s for %s because the file does not exist',
                self.name, input_file)
            return -1
        lines_checked = 0
        encargs = {} if six.PY2 else {'encoding' : 'utf-8'}
        with open(input_file, **encargs) as f_in:
            for line in f_in.readlines():
                split_line = line.split()
                lines_checked += 1
                if int(split_line[-1]) != 0:
                    return 0  # at least one non-zero count
        if lines_checked == 0:
            self.log.error('Failed to read counts from %s', input_file)
        return 1  # all counts are zero

    def run(self, dry_run=False):
        results = []
        for input_file in self.__input_files__:
            results.append(self.check_zero_counts(input_file))

        self.result = max(results)
        cmd = '# (internal) {} for {}'.format(self.name, self.__input_files__)
        if self.result < 0:
            cmd = '# (internal) {} -> skipped'.format(self.name)
            self.result = 0
            return self.result, cmd
        self.log.info('Running %s step', self.name)
        if self.auto_report_result:
            self.report_result()
        return self.result, cmd


class MessageCountStep(Step):
    '''Count messages printed inside event loop'''

    def __init__(self, name='MessageCount'):
        super(MessageCountStep, self).__init__(name)
        self.executable = 'messageCounter.py'
        self.log_regex = r'(athena\..*log$|athenaHLT:.*\.out$|^log\..*to.*)'
        self.start_pattern = r'(HltEventLoopMgr|AthenaHiveEventLoopMgr).*INFO Starting loop on events'
        self.end_pattern = r'(HltEventLoopMgr.*INFO All events processed|AthenaHiveEventLoopMgr.*INFO.*Loop Finished)'
        self.info_threshold = None
        self.debug_threshold = None
        self.verbose_threshold = None
        self.other_threshold = None
        self.auto_report_result = True

    def configure(self, test):
        self.args += ' -s "{:s}"'.format(self.start_pattern)
        self.args += ' -e "{:s}"'.format(self.end_pattern)
        if self.info_threshold is None:
            self.info_threshold = test.exec_steps[0].max_events
        if self.debug_threshold is None:
            self.debug_threshold = 0
        if self.verbose_threshold is None:
            self.verbose_threshold = 0
        if self.other_threshold is None:
            self.other_threshold = test.exec_steps[0].max_events
        super(MessageCountStep, self).configure(test)

    def run(self, dry_run=False):
        files = os.listdir('.')
        r = re.compile(self.log_regex)
        log_files = filter(r.match, files)
        self.args += ' ' + ' '.join(log_files)
        auto_report = self.auto_report_result
        self.auto_report_result = False
        ret, cmd = super(MessageCountStep, self).run(dry_run)
        self.auto_report_result = auto_report
        if ret != 0:
            self.log.error('%s failed')
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, cmd
        (num_info, num_debug, num_verbose, num_other) = (0, 0, 0, 0)
        for log_file in log_files:
            json_file = 'MessageCount.{:s}.json'.format(log_file)
            if not os.path.isfile(json_file):
                self.log.warning('%s cannot open file %s', self.name, json_file)
            with open(json_file) as f:
                summary = json.load(f)
                num_info += summary['INFO']
                num_debug += summary['DEBUG']
                num_verbose += summary['VERBOSE']
                num_other += summary['other']
        if num_info > self.info_threshold:
            self.log.info(
                '%s Number of INFO messages %s is higher than threshold %s',
                self.name, num_info, self.info_threshold)
            self.result += 1
        if num_debug > self.debug_threshold:
            self.log.info(
                '%s Number of DEBUG messages %s is higher than threshold %s',
                self.name, num_debug, self.debug_threshold)
            self.result += 1
        if num_verbose > self.verbose_threshold:
            self.log.info(
                '%s Number of VERBOSE messages %s is higher than threshold %s',
                self.name, num_verbose, self.verbose_threshold)
            self.result += 1
        if num_other > self.other_threshold:
            self.log.info(
                '%s Number of "other" messages %s is higher than threshold %s',
                self.name, num_other, self.other_threshold)
            self.result += 1
        if self.auto_report_result:
            self.report_result()
        return self.result, cmd


def default_check_steps(test):
    '''
    Create the default list of check steps for a test. The configuration
    depends on the package name and the type of exec steps (athena or
    athenaHLT or transforms).
    '''

    check_steps = []

    # Log merging
    if len(test.exec_steps) == 1:
        exec_step = test.exec_steps[0]
        if exec_step.type == 'athenaHLT':
            logmerge = LogMergeStep()
            logmerge.merged_name = 'athena.log'
            logmerge.log_files = ['athenaHLT.log']
            nforks = 1 if exec_step.forks is None else exec_step.forks
            for n in range(1, 1+nforks):
                logmerge.log_files.append('athenaHLT:{:02d}.out'.format(n))
                logmerge.log_files.append('athenaHLT:{:02d}.err'.format(n))
            check_steps.append(logmerge)
    else:
        logmerge = LogMergeStep()
        logmerge.merged_name = 'athena.log'
        logmerge.log_files = []
        for exec_step in test.exec_steps:
            logmerge.log_files.append(exec_step.get_log_file_name())
            if exec_step.type == 'athenaHLT':
                logmerge.extra_log_regex = 'athenaHLT:.*(.out|.err)'
        check_steps.append(logmerge)
    log_to_check = None
    log_to_zip = None
    if len(check_steps) > 0 and isinstance(check_steps[-1], LogMergeStep):
        log_to_check = check_steps[-1].merged_name
        log_to_zip = check_steps[-1].merged_name

    # Reco_tf log merging
    step_types = [step.type for step in test.exec_steps]
    if 'Reco_tf' in step_types:
        reco_tf_logmerge = LogMergeStep('LogMerge_Reco_tf')
        reco_tf_logmerge.warn_if_missing = False
        tf_names = ['HITtoRDO', 'RDOtoRDOTrigger', 'RAWtoESD', 'ESDtoAOD',
                    'PhysicsValidation', 'RAWtoALL', 'BSFTKCreator']
        reco_tf_logmerge.log_files = ['log.'+tf_name for tf_name in tf_names]
        reco_tf_logmerge.merged_name = 'athena.merged.log'
        log_to_zip = reco_tf_logmerge.merged_name
        if log_to_check is not None:
            reco_tf_logmerge.log_files.append(log_to_check)
            log_to_check = reco_tf_logmerge.merged_name
        log_to_check = reco_tf_logmerge.merged_name
        check_steps.append(reco_tf_logmerge)

    # Histogram merging for athenaHLT forks
    num_athenaHLT_steps = sum([1 for step in test.exec_steps if step.type == 'athenaHLT'])
    if num_athenaHLT_steps > 0:
        histmerge = RootMergeStep('HistMerge')
        histmerge.merged_file = 'expert-monitoring.root'
        histmerge.input_file = 'athenaHLT_workers/*/expert-monitoring.root expert-monitoring-mother.root'
        histmerge.rename_suffix = '-mother'
        check_steps.append(histmerge)

    # CheckLog for errors
    checklog = CheckLogStep('CheckLog')
    if log_to_check is not None:
        checklog.log_file = log_to_check
    check_steps.append(checklog)

    # CheckLog for warnings
    checkwarn = CheckLogStep('Warnings')
    checkwarn.check_errors = False
    checkwarn.check_warnings = True
    if log_to_check is not None:
        checkwarn.log_file = log_to_check
    check_steps.append(checkwarn)

    # MessageCount
    msgcount = MessageCountStep('MessageCount')
    check_steps.append(msgcount)

    # RegTest
    regtest = RegTestStep()
    if log_to_check is not None:
        regtest.input_base_name = os.path.splitext(log_to_check)[0]
    if 'athenaHLT' in step_types:
        regtest.regex = r'(?:HltEventLoopMgr(?!.*athenaHLT-)|REGTEST)'
    check_steps.append(regtest)

    # Tail (probably not so useful these days)
    tail = TailStep()
    if log_to_check is not None:
        tail.log_file = log_to_check
    check_steps.append(tail)

    # PerfMon
    check_steps.append(PerfMonStep())

    # Histogram-based steps
    check_steps.append(RootCompStep())
    check_steps.append(ChainDumpStep())
    check_steps.append(HistCountStep())

    # ZeroCounts
    check_steps.append(ZeroCountsStep())

    # Extra JSON
    check_steps.append(TrigTestJsonStep())

    # CheckFile
    check_steps.append(CheckFileStep())

    # Zip the merged log (can be large and duplicates information)
    if log_to_zip is not None:
        zip_step = ZipStep()
        zip_step.zip_input = log_to_zip
        zip_step.zip_output = log_to_zip+'.tar.gz'
        check_steps.append(zip_step)

    # return the steps
    return check_steps
