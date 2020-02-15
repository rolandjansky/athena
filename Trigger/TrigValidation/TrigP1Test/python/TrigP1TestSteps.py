#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of additional validation steps in Trigger ART tests relevant only for TrigP1Test.
The main common check steps are defined in the TrigValSteering.CheckSteps module.
'''

from TrigValTools.TrigValSteering import Step
import os
import inspect


class TrigBSDumpGrepStep(Step.Step):
    '''
    Extra step based on grepping trigbs_dumpHLTContentInBS.py output
    and comparing the line count to expected value
    '''

    def __init__(self, name='TrigBSDumpGrep'):
        super(TrigBSDumpGrepStep, self).__init__(name)
        self.executable = 'trigbs_dumpHLTContentInBS_run3.py'
        self.file_name_base = 'output'
        self.auto_report_result = True
        self.required = True
        self.regex = ''
        self.comparator = lambda n: n > 0

    def run(self, dry_run=False):
        if dry_run:
            self.log.info('Skipping %s in dry run', self.name)
            self.result = 0
            return self.result, '# (internal) {} -> skipped'.format(self.name)
        file_name = None
        for ls_file in os.listdir('.'):
            if  self.file_name_base in ls_file:
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
