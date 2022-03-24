#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of additional validation steps in Trigger ART tests relevant only for TrigP1Test.
The main common check steps are defined in the TrigValSteering.CheckSteps module.
'''

from TrigValTools.TrigValSteering import Step, CheckSteps
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


class ExtractExpertMonitoring(CheckSteps.InputDependentStep):
    '''
    Step which extracts the EXPERT directory from an online monitoring file
    produced by OH server into an offline-like expert-monitoring.root
    '''
    def __init__(self, name='ExtractExpertMonitoring'):
        super(ExtractExpertMonitoring, self).__init__(name)
        self.input_file = None
        self.path_prefix = None
        self.executable = 'rootcp'
        self.args = '--recreate -r'
        self.output_stream = Step.Step.OutputStream.STDOUT_ONLY

    def configure(self, test):
        self.args += ' {:s}:{:s}/HLT-Histogramming/*/EXPERT/* expert-monitoring.root'.format(self.input_file, self.path_prefix or '')
        super(ExtractExpertMonitoring, self).configure(test)


def default_check_steps_OHMon(test, hist_path):
    steps = []
    # Extract expert-monitoring.root file from OH server output
    extract_hist = ExtractExpertMonitoring()
    hist_path_split = hist_path.split(':')
    if len(hist_path_split) > 1:
        extract_hist.input_file = hist_path_split[0]
        extract_hist.path_prefix = hist_path_split[1]
    else:
        extract_hist.input_file = hist_path
    steps.append(extract_hist)
    # Default check steps
    steps.extend(CheckSteps.default_check_steps(test))
    # Remove histogram merging step
    matches = [step for step in steps if step.name == 'HistMerge']
    for hm_step in matches:
        steps.remove(hm_step)
    return steps

def filterBS(stream_name):
    '''Extract ByteStream data for a given stream from a file with multiple streams'''
    from TrigValTools.TrigValSteering import ExecStep
    from TrigValTools.TrigValSteering.Common import find_file
    filterStep = ExecStep.ExecStep('FilterBS_'+stream_name)
    filterStep.type = 'other'
    filterStep.executable = 'trigbs_extractStream.py'
    filterStep.input = ''
    filterStep.args = '-s ' + stream_name + ' ' + find_file('*_HLTMPPy_output.*.data')
    return filterStep

def decodeBS(stream_name):
    '''Deserialise HLT data from ByteStream and save to an ESD file'''
    from TrigValTools.TrigValSteering import ExecStep
    from TrigValTools.TrigValSteering.Common import find_file
    decodeStep = ExecStep.ExecStep('DecodeBS_'+stream_name)
    decodeStep.type = 'athena'
    decodeStep.job_options = 'TriggerJobOpts/decodeBS.py'
    decodeStep.input = ''
    decodeStep.explicit_input = True
    decodeStep.args = '--filesInput='+find_file('*'+stream_name+'*._athenaHLT*.data')
    decodeStep.perfmon = False  # no need to run PerfMon for this step
    return decodeStep

