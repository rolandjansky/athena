#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of post-exec check steps in Trigger ART tests
'''

import os
import re
import subprocess
import json
import glob

from TrigValTools.TrigValSteering.Step import Step, get_step_from_list
from TrigValTools.TrigValSteering.Common import art_input_eos, art_input_cvmfs, running_in_CI

class RefComparisonStep(Step):
    '''Base class for steps comparing a file to a reference'''

    def __init__(self, name):
        super(RefComparisonStep, self).__init__(name)
        self.reference = None
        self.ref_test_name = None
        self.input_file = None
        self.explicit_reference = False  # True if reference doesn't exist at configuration time

    def configure(self, test):
        if self.reference and self.ref_test_name:
            self.misconfig_abort('Both options "reference" and "ref_test_name" used. Use at most one of them.')

        if not self.ref_test_name:
            self.ref_test_name = test.name

        if self.reference is not None:
            # Do nothing if the reference will be produced later
            if self.explicit_reference:
                return super(RefComparisonStep, self).configure(test)
            # Do nothing if the reference exists
            if os.path.isfile(self.reference):
                return super(RefComparisonStep, self).configure(test)
            # Try to find the file in DATAPATH
            full_path = subprocess.check_output('find_data.py {}'.format(self.reference), shell=True).decode('utf-8').strip()
            if os.path.isfile(full_path):
                self.log.debug('%s using reference %s', self.name, full_path)
                self.reference = full_path
                return super(RefComparisonStep, self).configure(test)
            else:
                self.log.warning(
                    '%s failed to find reference %s - wrong path?',
                    self.name, self.reference)
                return super(RefComparisonStep, self).configure(test)

        if self.input_file is None:
            self.misconfig_abort('input_file not specified')

        branch = os.environ.get('AtlasBuildBranch')  # Available after asetup
        if not branch:
            branch = os.environ.get('gitlabTargetBranch')  # Available in CI
        if not branch:
            jobName = os.environ.get('JOB_NAME')  # Available in nightly build system (ATR-21836)
            if jobName:
                branch = jobName.split('_')[0].split('--')[0]
        if not branch:
            msg = 'Cannot determine the branch name, all variables are empty: AtlasBuildBranch, gitlabTargetBranch, JOB_NAME'
            if self.required:
                self.misconfig_abort(msg)
            else:
                self.log.warning(msg)
                branch = 'UNKNOWN_BRANCH'

        sub_path = '{}/ref/{}/test_{}/'.format(
            test.package_name, branch, self.ref_test_name)
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

        return super(RefComparisonStep, self).configure(test)


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
        # Protect against infinite loop
        if self.merged_name in self.log_files:
            self.misconfig_abort(
                'output log name %s is same as one of the input log names.'
                ' This will lead to infinite loop, aborting.', self.merged_name)
        super(LogMergeStep, self).configure(test)

    def process_extra_regex(self):
        if self.extra_log_regex:
            files = os.listdir('.')
            r = re.compile(self.extra_log_regex)
            match_files = filter(r.match, files)
            for f in match_files:
                self.log_files.append(f)

    def merge_logs(self):
        try:
            with open(self.merged_name, 'w', encoding='utf-8') as merged_file:
                for log_name in self.log_files:
                    if not os.path.isfile(log_name):
                        if self.warn_if_missing:
                            self.log.warning('Cannot open %s', log_name)
                            merged_file.write(
                                '### WARNING Missing {} ###\n'.format(log_name))
                        continue
                    with open(log_name, encoding='utf-8') as log_file:
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
        # Sort log files by modification time
        self.log_files.sort(key=lambda f : os.path.getmtime(f) if os.path.isfile(f) else 0)
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
        file_list_to_check = self.input_file.split()
        if os.path.isfile(self.merged_file) and self.rename_suffix:
            old_name = os.path.splitext(self.merged_file)
            new_name = old_name[0] + self.rename_suffix + old_name[1]
            self.executable = 'mv {} {}; {}'.format(self.merged_file, new_name, self.executable)
            if new_name in file_list_to_check:
                file_list_to_check.remove(new_name)
                file_list_to_check.append(self.merged_file)
        self.log.debug('%s checking if the input files exist: %s', self.name, str(file_list_to_check))
        for file_name in file_list_to_check:
            if len(glob.glob(file_name)) < 1:
                self.log.warning('%s: file %s requested to be merged but does not exist', self.name, file_name)
                self.result = 1
                return self.result, '# (internal) {} in={} out={} -> failed'.format(self.name, self.input_file, self.merged_file)
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
        # The following three are updated in configure() if not set
        self.required = None
        self.auto_report_result = None
        self.output_stream = None

    def configure(self, test):
        if self.config_file is None:
            if test.package_name == 'TrigP1Test':
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

        errors_only = self.check_errors and not self.check_warnings
        if self.output_stream is None:
            self.output_stream = Step.OutputStream.FILE_AND_STDOUT if errors_only else Step.OutputStream.FILE_ONLY
        if self.auto_report_result is None:
            self.auto_report_result = errors_only
        if self.required is None:
            self.required = errors_only

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
        with open(log_file, encoding='utf-8') as f_in:
            matches = re.findall('({}.*).*$'.format(self.regex),
                                 f_in.read(), re.MULTILINE)
            with open(self.input_file, 'w', encoding='utf-8') as f_out:
                for line in matches:
                    linestr = str(line[0]) if type(line) is tuple else line
                    f_out.write(linestr+'\n')
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
        if running_in_CI():
            # drawing the diff output may be slow and is not needed for CI
            self.args += ' --noRoot --noPS'
        self.args += ' {} {}'.format(self.reference, self.input_file)
        Step.configure(self, test)

    def run(self, dry_run=False):
        if self.reference is None:
            if not os.path.isfile(self.input_file):
                self.log.debug(
                    'Skipping %s because both reference and input are missing',
                    self.name)
                self.result = 0
                return self.result, '# (internal) {} -> skipped'.format(self.name)
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


class DownloadRefStep(Step):
    '''Execute art.py download to get results from previous days'''

    def __init__(self, name='DownloadRef'):
        super(DownloadRefStep, self).__init__(name)
        self.executable = 'art.py'
        self.args = 'download'
        self.artpackage = None
        self.artjobname = None
        self.timeout = 20*60
        self.required = True
        self.auto_report_result = True

    def configure(self, test):
        if not self.artpackage:
            self.artpackage = test.package_name
        if not self.artjobname:
            self.artjobname = 'test_'+test.name+'.py'
        self.args += ' '+self.artpackage+' '+self.artjobname
        super(DownloadRefStep, self).configure(test)


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
        self.required = True
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
        with open(input_file, encoding='utf-8') as f_in:
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
        self.log_regex = r'(athena\.(?!.*tail).*log$|athenaHLT:.*\.out$|^log\..*to.*)'
        self.skip_logs = []
        self.start_pattern = r'(HltEventLoopMgr|AthenaHiveEventLoopMgr).*INFO Starting loop on events'
        self.end_pattern = r'(HltEventLoopMgr.*INFO All events processed|AthenaHiveEventLoopMgr.*INFO.*Loop Finished)'
        self.print_on_fail = None
        self.thresholds = {}
        self.auto_report_result = True

    def configure(self, test):
        self.args += ' -s "{:s}"'.format(self.start_pattern)
        self.args += ' -e "{:s}"'.format(self.end_pattern)
        if self.print_on_fail is None:
            self.print_on_fail = self.required
        if self.print_on_fail:
            self.args += ' --saveAll'
        if 'WARNING' not in self.thresholds:
            self.thresholds['WARNING'] = 0
        if 'INFO' not in self.thresholds:
            self.thresholds['INFO'] = test.exec_steps[0].max_events
        if 'DEBUG' not in self.thresholds:
            self.thresholds['DEBUG'] = 0
        if 'VERBOSE' not in self.thresholds:
            self.thresholds['VERBOSE'] = 0
        if 'other' not in self.thresholds:
            self.thresholds['other'] = test.exec_steps[0].max_events
        super(MessageCountStep, self).configure(test)

    def run(self, dry_run=False):
        files = os.listdir('.')
        r = re.compile(self.log_regex)
        log_files = [f for f in filter(r.match, files) if f not in self.skip_logs]
        self.args += ' ' + ' '.join(log_files)
        auto_report = self.auto_report_result
        self.auto_report_result = False
        ret, cmd = super(MessageCountStep, self).run(dry_run)
        self.auto_report_result = auto_report
        if ret != 0:
            self.log.error('%s failed', self.name)
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, cmd

        for log_file in log_files:
            json_file = 'MessageCount.{:s}.json'.format(log_file)
            if self.print_on_fail:
                all_json_file = 'Messages.{:s}.json'.format(log_file)
            if not os.path.isfile(json_file):
                self.log.warning('%s cannot open file %s', self.name, json_file)
            with open(json_file) as f:
                summary = json.load(f)
                for level, threshold in self.thresholds.items():
                    if summary[level] > threshold:
                        self.result += 1
                        self.log.info(
                            '%s Number of %s messages %s in %s is higher than threshold %s',
                            self.name, level, summary[level], log_file, threshold)
                        if self.print_on_fail:
                            self.log.info('%s Printing all %s messages from %s', self.name, level, log_file)
                            with open(all_json_file) as af:
                                all_msg = json.load(af)
                                for msg in all_msg[level]:
                                    print(msg.strip())  # noqa: ATL901

        if self.auto_report_result:
            self.report_result()
        return self.result, cmd


def produces_log(step):
    '''
    Helper function checking whether a Step output_stream value
    indicates that it will produce a log file
    '''
    return step.output_stream == Step.OutputStream.FILE_ONLY or \
           step.output_stream == Step.OutputStream.FILE_AND_STDOUT


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
        if exec_step.type == 'athenaHLT' and produces_log(exec_step):
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
            if not produces_log(exec_step):
                continue
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
    reco_tf_steps = [step for step in test.exec_steps if step.type in ['Reco_tf', 'Trig_reco_tf']]
    if len(reco_tf_steps) > 0:
        reco_tf_logmerge = LogMergeStep('LogMerge_Reco_tf')
        reco_tf_logmerge.warn_if_missing = False
        tf_names = ['HITtoRDO', 'RDOtoRDOTrigger', 'RAWtoESD', 'ESDtoAOD',
                    'PhysicsValidation', 'RAWtoALL', 'BSRDOtoRAW']
        reco_tf_logmerge.log_files = ['log.'+tf_name for tf_name in tf_names]
        if not get_step_from_list('LogMerge', check_steps):
            for step in reco_tf_steps:
                reco_tf_logmerge.log_files.append(step.get_log_file_name())
        reco_tf_logmerge.extra_log_regex = r'athfile-.*\.log\.txt'
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
    for logmerge in [step for step in check_steps if isinstance(step, LogMergeStep)]:
        msgcount.skip_logs.append(logmerge.merged_name)
    check_steps.append(msgcount)

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


def add_step_after_type(step_list, ref_type, step_to_add):
    '''
    Insert step_to_add into step_list after the last step of type ref_type.
    If the list has no steps of type ref_type, append step_to_add at the end of the list.
    '''
    index_to_add = -1
    for index, step in enumerate(step_list):
        if isinstance(step, ref_type):
            index_to_add = index+1
    if index_to_add > 0:
        step_list.insert(index_to_add, step_to_add)
    else:
        step_list.append(step_to_add)
