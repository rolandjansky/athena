#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definition of a Trigger ART test to be configured and executed
'''

import sys
import os
import json
import subprocess
from collections import OrderedDict

from TrigValTools.TrigValSteering.Common import get_logger, art_result, clear_art_summary, package_prefix_dict
from TrigValTools.TrigValSteering.Step import get_step_from_list, get_step_type_from_list


class Test(object):
    '''Definition of a Trigger ART test to be configured and executed'''
    def __init__(self):
        self.log = get_logger()
        self.name = None
        self.package_name = None
        self.art_type = None
        self.exec_steps = []
        self.check_steps = []
        self.dry_run = None
        self.configured = False

    def configure(self):
        # Configure name
        self.configure_name()
        self.log.info("Configuring test %s", self.name)

        # Configure timeout for all steps
        self.configure_timeout()

        # Configure steps
        for step in self.exec_steps:
            step.configure(self)
        for step in self.check_steps:
            self.log.debug('Configuring check step %s', step.name)
            step.configure(self)

        duplicate_names = self.duplicate_step_names()
        if len(duplicate_names) > 0:
            self.configuration_error(
                'Found test steps with duplicate names: '
                '{}'.format(duplicate_names)+' Aborting because this'
                ' configuration could lead to overwriting logs')

        # Configure dry_run option
        if self.dry_run is None:
            # A hook to use dry_run for unit tests
            env = os.environ.get('TRIGVALSTEERING_DRY_RUN')
            if env:
                self.dry_run = True

        self.configured = True

        # Print configuration
        self.log.debug(
            'Test configuration complete:\n-- %s',
            '\n-- '.join(['{}: {}'.format(k, v) for k, v in self.__dict__.items()]))

    def run(self):
        if not self.configured:
            self.configure()

        self.log.info("Running test %s", self.name)

        # Clear the result summary log
        clear_art_summary()

        # Store the executed commands
        commands = OrderedDict()

        # Pre-exec - a useful hook for some workarounds
        pre_exec_cmd = self.pre_exec()
        if len(pre_exec_cmd) > 0:
            commands['pre_exec'] = pre_exec_cmd

        commands['exec_steps'] = []
        commands['check_steps'] = []

        # Run the exec steps
        self.run_steps(self.exec_steps, commands['exec_steps'])

        # Make a summary result code for all exec steps if there are multiple
        if len(self.exec_steps) > 1:
            exec_summary = 0
            for step in self.exec_steps:
                if step.result > exec_summary:
                    exec_summary = step.result
            if exec_summary > 0:
                self.log.info('At least one of the exec steps failed, using the largest code as ExecSummary')
            else:
                self.log.info('All exec steps succeeded')
            art_result(exec_summary, 'ExecSummary')

        # Run the check steps
        self.run_steps(self.check_steps, commands['check_steps'])

        # Dump all commands to JSON
        with open('commands.json', 'w') as outfile:
            json.dump(commands, outfile, indent=4)

        # Create the exit code and message from required steps
        exit_code = 0
        failed_required_steps = []
        for step in self.exec_steps + self.check_steps:
            if step.required and (step.result != 0):
                self.log.debug('Required step %s finished with result %s', step.name, step.result)
                failed_required_steps.append(step.name)
                if abs(step.result) > exit_code:
                    exit_code = abs(step.result)
        exit_msg = 'Test {:s} finished with code {:d}'.format(self.name, exit_code)
        if exit_code == 0:
            exit_msg += ' because all required steps were successful'
        else:
            exit_msg += ' because the following required steps failed: {:s}'.format(str(failed_required_steps))
        self.log.info(exit_msg)
        return exit_code

    def run_steps(self, steps, commands_list):
        previous_code = 0
        for step in steps:
            if previous_code != 0 and step.depends_on_previous:
                    self.log.error('Skipping step %s because previous step(s) failed', step.name)
                    step.result = 1
                    code, cmd = step.result, '# Skipped %s because of earlier failure'.format(step.name)
                    if step.auto_report_result:
                        step.report_result()
                    continue
            code, cmd = step.run(self.dry_run)
            previous_code = code
            commands_list.append(cmd)

    def configure_timeout(self):
        '''Set default timeout values for steps which don't have it set'''
        for exec_step in self.exec_steps:
            if exec_step.timeout is None:
                # 3h for grid tests, 1h for build tests
                exec_step.timeout = 12*3600 if self.art_type == 'grid' else 3600
        for check_step in self.check_steps:
            if check_step.timeout is None:
                # 5 min for all check steps
                check_step.timeout = 5*60

    def configure_name(self):
        filename = os.path.basename(sys.argv[0])
        self.log.debug('Parsing file name %s', filename)
        prefix = 'test_'
        suffix = '.py'
        if not filename.startswith(prefix) or not filename.endswith(suffix):
            self.configuration_error(
                'Test file name {} does not match '.format(filename) +
                'the required pattern {}*{}'.format(prefix, suffix))
        for package_name, package_prefix in package_prefix_dict.items():
            if filename.startswith(prefix+package_prefix):
                self.package_name = package_name
        if self.package_name is None:
            self.configuration_error(
                'Test file name {} could not be matched '.format(filename) +
                'to any of the required package prefixes: {}'.format(
                    package_prefix_dict.values()))
        max_len = 50
        if len(filename) > max_len:
            self.configuration_error(
                'Test file name is too long. The limit is {} '.format(max_len) +
                'characters, but it has {}'.format(len(filename)))
        self.name = filename[len(prefix):-len(suffix)]

    def duplicate_step_names(self):
        d = {}
        for step in self.exec_steps:
            d.setdefault(step.name, 0)
            d[step.name] += 1
        for step in self.check_steps:
            d.setdefault(step.name, 0)
            d[step.name] += 1
        duplicates = [name for name, count in d.items() if count > 1]
        self.log.debug('all steps: %s', d)
        self.log.debug('duplicates: %s', duplicates)
        return duplicates

    def configuration_error(self, message):
        self.log.error(message)
        art_result(1, 'TestConfig')
        sys.exit(1)

    def get_step(self, step_name):
        step = get_step_from_list(step_name, self.exec_steps)
        if step is None:
            step = get_step_from_list(step_name, self.check_steps)
        return step

    def get_step_by_type(self, step_type):
        step = get_step_type_from_list(step_type, self.exec_steps)
        if step is None:
            step = get_step_type_from_list(step_type, self.check_steps)
        return step

    def pre_exec(self):
        '''Extra pre-exec function executed just before the steps'''
        cmd_list = []
        # Create empty POOL File Catalog to avoid incorrect grid failure handling
        if self.art_type == 'grid':
            cmd = 'which art.py >/dev/null 2>&1'
            art_available = subprocess.call(cmd, shell=True)
            if art_available == 0:
                cmd = 'art.py createpoolfile'
                self.log.debug('Executing pre-exec command %s', cmd)
                subprocess.call(cmd, shell=True)
                cmd_list.append(cmd)
        return cmd_list
