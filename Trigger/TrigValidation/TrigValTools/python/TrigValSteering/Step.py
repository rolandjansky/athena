#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Base classes for steps in Trigger ART tests
'''

import os
import sys
import signal
import subprocess
import time
import re
from enum import Enum
from threading import Timer
from TrigValTools.TrigValSteering.Common import get_logger, art_result, running_in_CI


class Step(object):
    '''Base class for a step of a Trigger ART test'''

    class OutputStream(Enum):
        NO_PRINT = 0
        FILE_ONLY = 1
        STDOUT_ONLY = 2
        FILE_AND_STDOUT = 3

    def __init__(self, name=None):
        self.log = get_logger()
        self.name = name
        self.type = None
        self.executable = None
        self.args = ''
        self.cmd_suffix = None
        self.output_stream = self.OutputStream.FILE_ONLY
        self.log_file_name = None
        self.result = None
        self.auto_report_result = False
        self.required = False
        self.depends_on_previous = False
        self.timeout = None
        self.prmon = False
        self.prmon_interval = 5  # monitoring interval in seconds

    def get_log_file_name(self):
        return self.log_file_name or self.name+'.log'

    def configure(self, test=None):
        '''
        Can be implemented by derived classes.
        Base class implementation only prints the configuration to debug log.
        '''
        self.log.debug(
            'Step configuration:\n-- %s\n',
            '\n-- '.join(['{}: {}'.format(k, v) for k, v in self.__dict__.items()]))

    def report_result(self, result=None, name=None):
        if result is None:
            if self.result is not None:
                result = self.result
            else:
                self.misconfig_abort('report_result was called but result is None')

        if name is None:
            if self.name is not None:
                name = self.name
            else:
                name = 'UnnamedStep'

        art_result(result, name)

    def misconfig_abort(self, error_msg, *args, **kwargs):
        '''
        Print an error message (arguments passed to logging.error),
        report non-zero art-result and exit the process with non-zero code
        '''
        self.log.error('Misconfiguration in %s: '+error_msg, self.name, *args, **kwargs)
        self.report_result(1, 'TestConfig')
        sys.exit(1)

    def __trace_and_kill(self, pid, signal, backtrace_list):
        '''
        Produce a backtrace for a process and its children, then call
        os.killpg on the process. The last argument is a list of strings
        where the first is filled with the backtrace by this function
        (it has to be a list to be mutable).
        '''
        # Produce backtrace for the parent and all children

        try:
            import psutil
            parent = psutil.Process(pid)
            backtrace = ''
            for proc in [parent] + parent.children(recursive=True):
                backtrace += '\nTraceback for {} PID {}:\n'.format(proc.name(), proc.pid)
                backtrace += subprocess.check_output('$ROOTSYS/etc/gdb-backtrace.sh {}'.format(proc.pid),
                                                     stderr=subprocess.STDOUT, shell=True)
        except ImportError:
            # psutil is missing in LCG_96 python3
            backtrace = 'psutil not available; no backtrace generated'

        backtrace_list[0] = backtrace

        # Kill the process
        os.killpg(pid, signal)

    def __execute_with_timeout(self, cmd, timeout_sec):
        '''
        Execute a shell process and kill it if it doesn't finish
        before timeout_sec seconds pass. The implementation is based on
        https://stackoverflow.com/a/10012262 and https://stackoverflow.com/a/4791612
        In addition, a backtrace is produced for the timed out process and its children.
        '''
        proc = subprocess.Popen(cmd, shell=True, preexec_fn=os.setsid)
        backtrace_list = ['']
        timer = Timer(timeout_sec, self.__trace_and_kill,
                      [os.getpgid(proc.pid), signal.SIGKILL, backtrace_list])
        try:
            timer.start()
            proc.communicate()
        finally:
            timer.cancel()

        if proc.returncode == -signal.SIGKILL:
            self.log.error('%s killed on timeout of %s seconds',
                           self.name, self.timeout)
            if (self.output_stream == self.OutputStream.FILE_ONLY or
                    self.output_stream == self.OutputStream.FILE_AND_STDOUT):
                with open(self.get_log_file_name(), 'a') as log_file:
                    log_file.write(
                        'ERROR process killed on timeout '
                        'of {} seconds, command was {}\n'.format(
                            self.timeout, cmd))
                    log_file.write(backtrace_list[0])
            return signal.SIGKILL  # return 9 instead of -9

        return proc.returncode

    def __start_prmon(self):
        self.log.debug('Starting prmon for pid %d', os.getpid())
        prmon_cmd = 'prmon --pid {:d} --interval {:d}'.format(os.getpid(), self.prmon_interval)
        prmon_cmd +=' --filename prmon.{name:s}.txt --json-summary prmon.summary.{name:s}.json'.format(name=self.name)
        return subprocess.Popen(prmon_cmd, shell=True)

    def __stop_prmon(self, prmon_proc):
        self.log.debug('Stopping prmon')
        try:
            prmon_proc.send_signal(signal.SIGUSR1)
            countWait = 0
            while (not prmon_proc.poll()) and countWait < 10:
                time.sleep(0.1)
                countWait += 1
        except OSError as err:
            self.log.warning('Error while stopping prmon: %s', err)
            pass

    def run(self, dry_run=False):
        cmd = '{} {}'.format(self.executable, self.args)
        if self.cmd_suffix:
            cmd += self.cmd_suffix
        if self.output_stream == self.OutputStream.NO_PRINT:
            cmd += ' >/dev/null 2>&1'
        elif self.output_stream == self.OutputStream.FILE_ONLY:
            cmd += ' >{} 2>&1'.format(self.get_log_file_name())
        elif self.output_stream == self.OutputStream.FILE_AND_STDOUT:
            cmd += ' 2>&1 | tee {}; exit ${{PIPESTATUS[0]}}'.format(self.get_log_file_name())
        elif self.output_stream == self.OutputStream.STDOUT_ONLY:
            cmd += ' 2>&1'.format(self.name)

        self.log.info('Running %s step using command:\n%s', self.name, cmd)
        if dry_run:
            self.result = 0
        else:
            if self.prmon:
                prmon_proc = self.__start_prmon()
            if self.timeout:
                self.result = self.__execute_with_timeout(cmd, self.timeout)
            else:
                self.result = subprocess.call(cmd, shell=True)
            if self.prmon:
                self.__stop_prmon(prmon_proc)

        if self.auto_report_result:
            self.report_result()

        # Print full log to stdout for failed steps if running in CI
        if self.required \
                and self.result != 0 \
                and running_in_CI() \
                and self.output_stream==self.OutputStream.FILE_ONLY:
            self.log.error('Step failure while running in CI. Printing full log %s', self.get_log_file_name())
            with open(self.get_log_file_name()) as log_file:
                log=log_file.read()
                print(log)  # noqa: ATL901
                # Print also sub-step logs for transforms
                if self.executable.endswith('_tf.py'):
                    step_matches = re.findall('Logs for (.*) are in (.*)', log)
                    if not step_matches:
                        self.log.warning('Failed to determine sub-step log names, cannot print the full sub-step logs')
                    else:
                        step_log_names = [m[1] for m in step_matches]
                        for step_log_name in step_log_names:
                            if os.path.isfile(step_log_name):
                                with open(step_log_name) as step_log_file:
                                    step_log = step_log_file.read()
                                    self.log.info('Printing sub-step log file %s', step_log_name)
                                    print(step_log)  # noqa: ATL901

        return self.result, cmd


def get_step_from_list(step_name, step_list):
    '''
    Retrieve the first test matching the name from the list. Returns None if
    no match is found.
    '''
    for step in step_list:
        if step.name is not None and step_name in step.name:
            return step
    return None


def get_step_type_from_list(step_type, step_list):
    '''
    Retrieve the first test matching the type from the list. Returns None if
    no match is found.
    '''
    for step in step_list:
        if isinstance(step, step_type):
            return step
    return None
