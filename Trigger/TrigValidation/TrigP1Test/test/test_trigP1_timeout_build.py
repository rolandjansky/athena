#!/usr/bin/env python

# art-description: athenaHLT test of timeout handling
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, Step
import os
import inspect

output_name_base = 'output.test_trigP1_timeout'

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigP1Test/testHLT_timeout.py'
ex.input = 'data'
ex.max_events = 20
ex.args = '--timeout 2000'
ex.args += ' -o ' + output_name_base
ex.perfmon = False  # perfmon with athenaHLT doesn't work at the moment

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = []


# Class for custom post-exec steps
class TrigBSDumpGrepStep(Step.Step):
    '''
    Extra step based on grepping trigbs_dumpHLTContentInBS.py output
    and comparing the line count to expected value
    '''

    def __init__(self, name='TrigBSDumpGrep'):
        super(TrigBSDumpGrepStep, self).__init__(name)
        self.executable = 'trigbs_dumpHLTContentInBS.py'
        self.file_name_base = output_name_base
        self.auto_report_result = True
        self.required = True
        self.regex = ''
        self.comparator = lambda n: n != 0

    def run(self, dry_run=False):
        if dry_run:
            self.log.info('Skipping %s in dry run', self.name)
            return 0, '# (internal) {} -> skipped'.format(self.name)
        file_name = None
        for ls_file in os.listdir('.'):
            if output_name_base in ls_file:
                file_name = ls_file
        if file_name is None:
            self.log.error('%s cannot find the BS output file', self.name)
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, '# (internal) {} -> failed'.format(self.name)
        self.log.debug('%s found BS file %s', self.name, file_name)
        self.args += ' ' + file_name
        self.args += ' | grep "{}" | wc -l'.format(self.regex)

        old_auto_report = self.auto_report_result
        self.auto_report_result = False
        ret, cmd = super(TrigBSDumpGrepStep, self).run(dry_run)
        self.auto_report_result = old_auto_report

        if ret != 0:
            self.log.error('%s failed', self.name)
            if self.auto_report_result:
                self.report_result()
            return self.result, cmd

        if not os.path.isfile(self.get_log_file_name()):
            self.log.error('%s failed, the file %s is missing',
                           self.name, self.get_log_file_name())
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, cmd

        num = None
        with open(self.get_log_file_name()) as log_file:
            num = eval(log_file.read())
        if not self.comparator(num) or num is None:
            self.log.error('%s failed the comparison', self.name)
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, cmd

        compare_str_list = inspect.getsource(self.comparator).split(':')
        compare_str = ''.join(compare_str_list[1:])
        compare_str = compare_str.strip()
        self.log.info('Comparison %s for num=%s gives True', compare_str, num)
        self.result = 0
        if self.auto_report_result:
            self.report_result()
        return self.result, cmd


# Step checking if there is at least one event in HltTimeout debug stream
debug_count_step = TrigBSDumpGrepStep('DebugCount')
debug_count_step.args += '--stag'
debug_count_step.regex = r"\[('HltTimeout', 'debug')\]"
debug_count_step.comparator = lambda num: num > 0
test.check_steps.append(debug_count_step)

# Step checking if number of output events is equal to number of input events
event_count_step = TrigBSDumpGrepStep('EventCount')
event_count_step.args += '--stats'
event_count_step.regex = r'Global_ID'
event_count_step.comparator = lambda num: num == ex.max_events
test.check_steps.append(event_count_step)

import sys
sys.exit(test.run())
