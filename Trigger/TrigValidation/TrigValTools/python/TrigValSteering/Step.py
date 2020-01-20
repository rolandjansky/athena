#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Base classes for steps in Trigger ART tests
'''

import os
import sys
import signal
import subprocess
from enum import Enum
from threading import Timer
from TrigValTools.TrigValSteering.Common import get_logger, art_result


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
        self.output_stream = self.OutputStream.FILE_ONLY
        self.log_file_name = None
        self.result = None
        self.auto_report_result = False
        self.required = False
        self.timeout = None

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
                self.log.error('report_result was called but result is None')
                self.report_result(1, 'TestConfig')
                sys.exit(1)

        if name is None:
            if self.name is not None:
                name = self.name
            else:
                name = 'UnnamedStep'

        art_result(result, name)

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

    def run(self, dry_run=False):
        cmd = '{} {}'.format(self.executable, self.args)
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
            if self.timeout:
                self.result = self.__execute_with_timeout(cmd, self.timeout)
            else:
                self.result = subprocess.call(cmd, shell=True)
        if self.auto_report_result:
            self.report_result()
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
