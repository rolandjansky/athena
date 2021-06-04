#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of exec steps (main job) in Trigger ART tests
'''

import os
import re

from TrigValTools.TrigValSteering.Step import Step
from TrigValTools.TrigValSteering.Input import is_input_defined, get_input
from TrigValTools.TrigValSteering.Common import check_job_options


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
        self.use_pickle = False
        self.imf = True
        self.perfmon = True
        self.fpe_auditor = True
        self.costmon = False
        self.malloc = False
        self.prmon = True
        self.config_only = False
        self.auto_report_result = True
        self.required = True
        self.depends_on_previous = True

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
        self.log.debug('Configuring type for step %s', self.name)
        # Check if type or executable is specified
        if self.type is None and self.executable is None:
            self.misconfig_abort('Cannot configure a step without specified type or executable')

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
            self.misconfig_abort('Cannot determine type of this step')

        if self.executable.endswith('_tf.py'):
            def del_env(varname):
                if varname in os.environ:
                    del os.environ[varname]

            # Ensure no log duplication for transforms
            os.environ['TRF_NOECHO'] = '1'
            del_env('TRF_ECHO')

            # We don't use the Reco_tf auto-configuration for MP/MT transforms,
            # instead we pass our parameters of choice to athenaopts
            del_env('ATHENA_NPROC_NUM')
            del_env('ATHENA_CORE_NUMBER')

    def configure_input(self):
        self.log.debug('Configuring input for step %s', self.name)
        if self.input is None:
            self.misconfig_abort(
                'Input not provided for this step. To configure'
                'a step without input, use an empty string')

        # Step with no input
        if len(self.input) == 0:
            return

        # Try to interpret input as keyword
        if is_input_defined(self.input):
            self.input_object = get_input(self.input)
            if self.input_object is None:
                self.misconfig_abort('Failed to load input with keyword %s', self.input)
            return

        # Try to interpret explicit paths
        input_paths = self.input.split(',')
        for path in input_paths:
            if not os.path.isfile(path):
                self.misconfig_abort('The provided input does not exist: %s', self.input)
        self.log.debug('Using explicit input: %s', self.input)

    def configure_job_options(self):
        '''Check job options configuration'''
        self.log.debug('Configuring job options for step %s', self.name)
        if self.type == 'other':
            self.log.debug('Skipping job options check for step.type=other')
            return
        if self.use_pickle:
            self.log.debug('Skipping job options check for step running from a pickle file')
            return

        if self.type.endswith('_tf'):
            if self.job_options is None:
                return
            else:
                self.misconfig_abort('Transform %s does not accept job options', self.type)
        elif self.job_options is None or len(self.job_options) == 0:
            self.misconfig_abort('Job options not provided for this step')
        # Check if job options exist
        if check_job_options(self.job_options):
            self.log.debug('Job options file exists: %s', self.job_options)
        else:
            self.misconfig_abort('Failed to find job options file %s', self.job_options)

    def add_precommand(self, precommand):
        if self.type == 'athena':
            precommand_arg_names = ['-c ', '--command = ', '--command ']
        elif self.type == 'athenaHLT':
            precommand_arg_names = ['-c ', '--precommand = ', '--precommand ']
        elif self.type in ['Reco_tf', 'Trig_reco_tf']:
            precommand_arg_names = ['--preExec ', '--preExec = ']
        else:
            self.log.warning('add_precommand() undefined for ExecStep with type="%s"', self.type)
            return

        if not precommand.endswith(';'):
            precommand += ';'

        opt_pattern_str = '('
        for arg_name in precommand_arg_names:
            if len(opt_pattern_str) > 1:
                opt_pattern_str += '|'
            opt_pattern_str += r'{a:s}".*"|{a:s}\'.*\''.format(a=arg_name.replace(' ', r'\s*'))
        opt_pattern_str += ')'
        opt_pattern = re.compile(opt_pattern_str)
        match = re.search(opt_pattern, self.args)
        if not match:
            self.args += ' {:s}"{:s}"'.format(precommand_arg_names[0], precommand)
            return
        opt_match = match.group(0)

        # Refine the match to avoid matching '--preExec "foo" --postExec "bar"'
        refine_pattern = re.compile(r'(--\w*\s*"|--\w*\s*\'|--\w*="|--\w*=\')')
        refine_matches = re.findall(refine_pattern, opt_match)
        if len(refine_matches) > 1:
            opt_match = opt_match[0:opt_match.find(refine_matches[1])]

        old_cmd_pattern = re.compile(r'(".*"|\'.*\')')
        old_cmd_match = re.search(old_cmd_pattern, opt_match)
        if not old_cmd_match:
            self.misconfig_abort('Failed to add precommand ' + precommand + ' to step ' + self.name)
        old_cmd = old_cmd_match.group(0)



        # Transform case
        if self.type.endswith('_tf'):
            new_cmd = '--preExec {:s} "{:s}" '.format(old_cmd, precommand)
            self.args = self.args.replace(opt_match, new_cmd)
            return


        # athena(HLT) case
        old_cmd = old_cmd[1:-1]
        new_cmd = old_cmd
        if not new_cmd.endswith(';'):
            new_cmd += ';'
        new_cmd += precommand
        self.args = self.args.replace(old_cmd, new_cmd)

    def add_hlt_jo_modifier(self, modifier):
        '''
        Same as add_precommand but checks if using HLT job options where the modifier is applicable
        and prepends the transform step name in case of transforms
        '''
        if self.type in ['athena', 'athenaHLT'] and 'runHLT_standalone' not in self.job_options:
            self.log.debug('Skip adding modifier %s to step %s because it does not use runHLT_standalone job options',
                           modifier, self.name)
            return
        elif self.type in ['Reco_tf', 'Trig_reco_tf']:
            if 'inputBS_RDOFile' in self.args:
                modifier = 'BSRDOtoRAW:' + modifier
            elif 'outputRDO_TRIGFile' in self.args or 'doRDO_TRIG' in self.args:
                modifier = 'RDOtoRDOTrigger:' + modifier
            else:
                self.log.debug('Skip adding modifier %s to step %s because it is a transform which does not run Trigger',
                            modifier, self.name)
                return

        return self.add_precommand(modifier)

    def configure_args(self, test):
        self.log.debug('Configuring args for step %s', self.name)
        if self.args is None:
            self.args = ''
        athenaopts = ''

        # Disable prmon for Reco_tf because it is already started inside the transform
        if self.type == 'Reco_tf':
            self.prmon = False

        # Disable perfmon for multi-fork jobs as it cannot deal well with them
        if self.forks and self.forks > 1:
            self.perfmon = False

        # Append imf/perfmon
        if self.type != 'other':
            if self.imf:
                athenaopts += ' --imf'
            if self.perfmon:
                athenaopts += ' --perfmon'
            if self.malloc:
                athenaopts += " --stdcmalloc "

        # Enable CostMonitoring/FPEAuditor
        if self.type != 'other':
            if self.costmon:
                self.add_hlt_jo_modifier('forceCostMonitoring=True')
            if self.fpe_auditor:
                self.add_hlt_jo_modifier('fpeAuditor=True')

        # Run config-only if requested
        if self.config_only :

            if self.type == 'athenaHLT' or (self.type == "other" and self.executable == "athenaHLT.py") :
                athenaopts += ' --dump-config-exit'

            elif self.type == 'athena' or self.type == 'Reco_tf' or (self.type == "other" and self.executable == "athena.py") :
                athenaopts += ' --config-only=' + self.name + '.pkl'

            # No current support if it isn't clear exactly what's being run
            # This includes Trig_reco_tf and 'other' where the executable is not known
            else :
                self.misconfig_abort('Cannot determine what config-only option is needed. Consider adding the appropriate flag to "args" instead.')

        # Default threads/concurrent_events/forks
        if test.package_name == 'TrigP1Test' and self.type == 'athenaHLT':
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
                    self.max_events = 100
                else:
                    self.max_events = 20
            else:
                self.max_events = 1000

        # Set prmon interval based on max events
        if self.prmon:
            if self.max_events <= 100:
                self.prmon_interval = 5
            else:
                self.prmon_interval = 10

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
                    input_str = ' --file='.join(self.input_object.paths)
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
                    self.misconfig_abort(
                        'Cannot build inputXYZFile string for transform '
                        ' from explicit input path. Use input=\'\' and '
                        'specify the input explicitly in args')
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
