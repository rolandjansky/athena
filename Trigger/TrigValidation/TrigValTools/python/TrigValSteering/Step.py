#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Base classes for steps in Trigger ART tests
'''

import sys
import subprocess
from enum import Enum

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

    def get_log_file_name(self):
        return self.log_file_name or self.name+'.log'

    def configure(self):
        '''Can be implemented by derived classes'''
        pass

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
