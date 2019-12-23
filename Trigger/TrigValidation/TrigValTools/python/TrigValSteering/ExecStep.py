#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of exec steps (main job) in Trigger ART tests
'''

import sys
import os
import subprocess

from TrigValTools.TrigValSteering.Step import Step
from TrigValTools.TrigValSteering.Input import is_input_defined, get_input


class ExecStep(Step):
    '''
    Step executing the main job of a Trigger ART test. This can be either
    athena or athenaHLT or a transform. There can be several ExecSteps in
    one Test
    '''

    def __init__(self, name=None):
        super(ExecStep, self).__init__(name)
        self.input = None
        self.input_object = None
        self.job_options = None
        self.threads = None
        self.concurrent_events = None
        self.forks = None
        self.max_events = None
        self.skip_events = None
        self.imf = True
        self.perfmon = True
        self.auto_report_result = True
        self.required = True

    def construct_name(self):
        if self.name and self.type == 'other':
            return self.name
        name = self.type if self.type else self.executable
        if self.name:
            name += '.'+self.name
        return name

    def get_log_file_name(self):
        if self.log_file_name is not None:
            return self.log_file_name
        else:
            return self.construct_name()+'.log'

    def configure(self, test):
        # Construct the name
        self.name = self.construct_name()
        self.log_file_name = self.name+'.log'
        self.log.debug('Configuring step %s', self.name)

        # Parse configuration
        self.configure_type()
        self.configure_input()
        self.configure_job_options()
        self.configure_args(test)

        super(ExecStep, self).configure(test)

    def configure_type(self):
        # Check if type or executable is specified
        if self.type is None and self.executable is None:
            self.log.error('Cannot configure a step without specified type '
                           'or executable')
            self.report_result(1, 'TestConfig')
            sys.exit(1)

        # Configure executable from type
        known_types = ['athena', 'athenaHLT', 'Reco_tf', 'Trig_reco_tf']
        if self.type in known_types:
            if self.executable is not None:
                self.log.warning('type=%s was specified, so executable=%s '
                                 'will be ignored and deduced automatically '
                                 'from type', self.type, self.executable)
            self.executable = '{}.py'.format(self.type)
        elif self.type == 'other' or self.type is None:
            self.type = 'other'
        else:
            self.log.error('Cannot determine type of step %s', self.name)
            self.report_result(1, 'TestConfig')
            sys.exit(1)

    def configure_input(self):
        if self.input is None:
            self.log.error(
                'Input not provided for step %s. To configure '
                'a step without input, use an empty string',
                self.name)
            self.report_result(1, 'TestConfig')
            sys.exit(1)

        # Step with no input
        if len(self.input) == 0:
            return

        # Try to interpret input as keyword
        if is_input_defined(self.input):
            self.input_object = get_input(self.input)
            if self.input_object is None:
                self.log.error('Failed to load input with keyword %s', self.input)
                self.report_result(1, 'TestConfig')
                sys.exit(1)
            return

        # Try to interpret explicit paths
        input_paths = self.input.split(',')
        for path in input_paths:
            if not os.path.isfile(path):
                self.log.error('The provided input does not exist: %s', self.input)
                self.report_result(1, 'TestConfig')
                sys.exit(1)
        self.log.debug('Using explicit input: %s', self.input)

    def configure_job_options(self):
        '''Check job options configuration'''
        if self.type == 'other':
            self.log.debug('Skipping job options check for step.type=other')
            return

        if self.type.endswith('_tf'):
            if self.job_options is None:
                return
            else:
                self.log.error('Transform %s does not accept job options',
                               self.type)
                self.report_result(1, 'TestConfig')
                sys.exit(1)
        elif self.job_options is None or len(self.job_options) == 0:
            self.log.error('Job options not provided for step %s', self.name)
            self.report_result(1, 'TestConfig')
            sys.exit(1)
        # Check if job options exist
        if os.path.isfile(self.job_options):
            self.log.debug('Job options file exists: %s', self.job_options)
            return
        # Try to find the file in PATH
        get_files_output = subprocess.check_output(
            'get_files -jo -list {}'.format(self.job_options), shell=True)
        if 'nothing found' in get_files_output.decode():
            self.log.error('Failed to find job options file %s', self.name)
            self.report_result(1, 'TestConfig')
            sys.exit(1)
        self.log.debug('Job options file exists: %s', self.job_options)

    def configure_args(self, test):
        if self.args is None:
            self.args = ''
        athenaopts = ''

        # Append imf/perfmon
        if self.type != 'other':
            if self.imf:
                athenaopts += ' --imf'
            if self.perfmon:
                athenaopts += ' --perfmon'

        # Default threads/concurrent_events/forks
        if test.package_name == 'TrigUpgradeTest':
            if self.threads is None:
                self.threads = 1
        elif test.package_name == 'TrigP1Test' and self.type == 'athenaHLT':
            if self.threads is None:
                self.threads = 1
            if self.concurrent_events is None:
                self.concurrent_events = 1
            if self.forks is None:
                self.forks = 1

        # Append threads/concurrent_events/forks
        if self.threads is not None:
            athenaopts += ' --threads={}'.format(self.threads)
        if self.concurrent_events is not None:
            athenaopts += ' --concurrent-events={}'.format(
                self.concurrent_events)
        if self.forks is not None:
            athenaopts += ' --nprocs={}'.format(self.forks)

        # Append athenaopts
        athenaopts = athenaopts.strip()
        if self.type.endswith('_tf'):
            self.args += ' --athenaopts="{}"'.format(athenaopts)
        else:
            self.args += ' '+athenaopts

        # Default max events
        if self.max_events is None:
            if test.art_type == 'build':
                if test.package_name == 'TrigP1Test':
                    self.max_events = 50
                else:
                    self.max_events = 20
            else:
                self.max_events = 1000

        # Append max/skip events
        if self.type == 'athena':
            self.args += ' --evtMax={}'.format(self.max_events)
        elif self.type == 'athenaHLT':
            self.args += ' --number-of-events={}'.format(self.max_events)
        elif self.type.endswith('_tf'):
            self.args += ' --maxEvents={}'.format(self.max_events)
        if self.skip_events is not None:
            if self.type == 'athena':
                self.args += ' --skipEvents={}'.format(self.skip_events)
            elif self.type == 'athenaHLT':
                self.args += ' --skip-events={}'.format(self.skip_events)
            elif self.type.endswith('_tf'):
                self.args += ' --skipEvents={}'.format(self.skip_events)

        # Append input
        if len(self.input) > 0:
            if self.input_object is not None:
                if self.type == 'athenaHLT':
                    # athenaHLT can only take one input file
                    input_str = self.input_object.paths[0]
                else:
                    input_str = ','.join(self.input_object.paths)
            else:
                input_str = self.input
            if self.type == 'athena':
                self.args += ' --filesInput={}'.format(input_str)
            elif self.type == 'athenaHLT':
                self.args += ' --file={}'.format(input_str)
            elif self.type.endswith('_tf'):
                if self.input_object is None:
                    self.log.error(
                        'Cannot build inputXYZFile string for transform '
                        ' from explicit input path. Use input=\'\' and '
                        'specify the input explicitly in args')
                    self.report_result(1, 'TestConfig')
                    sys.exit(1)
                if self.type == 'Trig_reco_tf' and '--prodSysBSRDO True' in self.args:
                    self.args += ' --inputBS_RDOFile={}'.format(input_str)
                else:
                    self.args += ' --input{}File={}'.format(
                        self.input_object.format, input_str)


        # Append job options
        if self.job_options is not None:
            self.args += ' '+self.job_options

        # Strip extra whitespace
        self.args = self.args.strip()
