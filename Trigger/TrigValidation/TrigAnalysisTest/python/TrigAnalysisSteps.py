#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of additional validation steps in Trigger ART tests relevant only for TrigAnalysisTest.
The main common check steps are defined in the TrigValSteering.CheckSteps module.
'''

from TrigValTools.TrigValSteering.Step import Step
from TrigValTools.TrigValSteering.ExecStep import ExecStep
from TrigValTools.TrigValSteering.CheckSteps import CheckFileStep, InputDependentStep, LogMergeStep
import os
import re

##################################################
# Additional exec steps
##################################################

class AthenaCheckerStep(ExecStep, InputDependentStep):
    '''
    Run athena job with Trig*Checker algorithm with job options accepting a fileList parameter
    '''
    def __init__(self, name, job_options):
        InputDependentStep.__init__(self, name)
        ExecStep.__init__(self, name)
        # InputDependentStep parameters
        self.input_file = 'AOD.pool.root'
        # ExecStep parameters
        self.type = 'athena'
        self.job_options = job_options
        self.input = ''
        self.explicit_input = True
        self.perfmon = False
        self.prmon = False

    def configure(self, test):
        self.args += ' -c \'fileList=["{:s}"]\''.format(self.input_file)
        return ExecStep.configure(self, test)

    def run(self, dry_run):
        return InputDependentStep.run(self, dry_run)

class TrigDecChecker(AthenaCheckerStep):
    def __init__(self, name='TrigDecChecker', in_file='AOD.pool.root'):
        AthenaCheckerStep.__init__(self, name, 'TrigAnalysisTest/testAthenaTrigAOD_TrigDecTool.py')
        self.input_file = in_file

class TrigEDMChecker(AthenaCheckerStep):
    def __init__(self, name='TrigEDMChecker', in_file='AOD.pool.root'):
        AthenaCheckerStep.__init__(self, name, 'TrigAnalysisTest/testAthenaTrigAOD_TrigEDMCheck.py')
        self.input_file = in_file

class TrigEDMAuxChecker(AthenaCheckerStep):
    def __init__(self, name='TrigEDMAuxChecker', in_file='AOD.pool.root'):
        AthenaCheckerStep.__init__(self, name, 'TrigAnalysisTest/testAthenaTrigAOD_TrigEDMAuxCheck.py')
        self.input_file = in_file

##################################################
# Additional check steps
##################################################

class CheckFileTrigSizeStep(CheckFileStep):
    '''
    Execute checkFileTrigSize_RTT.py for POOL files.
    '''
    def __init__(self, name='CheckFileTrigSize'):
        super(CheckFileTrigSizeStep, self).__init__(name)
        self.input_file = 'AOD.pool.root,ESD.pool.root,RDO_TRIG.pool.root'
        self.executable = 'checkFileTrigSize_RTT.py'

class PhysValWebStep(Step):
    '''
    Execute physval_make_web_display.py to make PhysVal web display from NTUP_PHYSVAL.root
    '''
    def __init__(self, name='PhysValWeb'):
        super(PhysValWebStep, self).__init__(name)
        self.input_file = 'NTUP_PHYSVAL.pool.root'
        self.executable = 'physval_make_web_display.py'
        self.refdir = None
        self.sig = None
        self.args = '--ratio --drawopt HISTPE --refdrawopt HIST --title Test '
        self.auto_report_result = True
        self.timeout = 30*60
        self.required = True  # whether the full test should fail if physval_make_web_display fails
        self.required_no_red = False  # whether the full test should fail if red histograms are found

    def configure(self, test):
        assert self.sig, 'sig is a required parameter'
        outargs = ' --outdir PHYSVAL_WEB/'+self.sig
        dirargs = ' --startpath run_1/HLT/'+self.sig
        self.args += ' '+outargs+' '+dirargs
        super(PhysValWebStep, self).configure(test)

    def get_refdir(self):
        if self.refdir:
            return
        for fname in os.listdir('.'):
            if fname.startswith('ref-') and os.path.isdir(fname):
                self.refdir = fname

    def run(self, dry_run=False):
        if not dry_run and not os.path.exists(self.input_file):
            self.log.error('Input file does not exist: %s', self.input_file)
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, '# (internal) {} -> failed'.format(self.name)

        if not os.path.exists('PHYSVAL_WEB'):
            os.mkdir('PHYSVAL_WEB')

        self.get_refdir()
        if self.refdir:
            ref_file = self.refdir+'/'+self.input_file
            if not dry_run and not os.path.exists(ref_file):
                self.log.warning('Reference file %s does not exist, running without reference', ref_file)
            else:
                self.args += ' --reffile Ref:'+ref_file
        self.args += ' '+self.input_file

        do_report_result = self.auto_report_result
        self.auto_report_result = False  # don't report yet, only after further checks below
        self.result, cmd = super(PhysValWebStep, self).run(dry_run)
        cmd += ' # Plus internal post-exec checks'
        if dry_run:
            if do_report_result:
                self.report_result()
            return self.result, cmd

        fname = 'PHYSVAL_WEB/'+self.sig+'/index.html'
        if not os.path.exists(fname):
            self.log.error('Missing output file %s', fname)
            self.result += 1000
            if do_report_result:
                self.report_result()
            return self.result, cmd

        nred_all = 0
        with open(fname, 'r') as f:
            red_lines = re.findall('Red.*$', f.read(), re.MULTILINE)
            nred = len(red_lines)
            nred_all += nred
            if nred > 0:
                msg = 'Red histograms in display for slice %s: %d'
                if self.required_no_red:
                    self.log.error(msg, self.sig, nred)
                else:
                    self.log.info(msg, self.sig, nred)

        if do_report_result:
            self.report_result(self.result + nred_all)
        if self.required_no_red:
            self.result += nred_all

        return self.result, cmd

##################################################
# Getter functions
##################################################

def trig_analysis_exec_steps(input_file='AOD.pool.root'):
    # TODO: add TrigNavSlimming test
    return [
        TrigDecChecker(in_file=input_file),
        TrigEDMChecker(in_file=input_file),
        TrigEDMAuxChecker(in_file=input_file)]

def trig_analysis_check_steps():
    return [CheckFileTrigSizeStep()]

def add_analysis_steps(test, input_file='AOD.pool.root'):
    analysis_exec_steps = trig_analysis_exec_steps(input_file)
    test.exec_steps.extend(analysis_exec_steps)
    test.check_steps.extend(trig_analysis_check_steps())

    # Add the analysis exec step logs for merging
    logmerge = test.get_step_by_type(LogMergeStep)
    if not logmerge:
        test.log.warning('LogMerge step not found, cannot add TrigAnalysisSteps exec step log files for merging')
    else:
        for step in analysis_exec_steps:
            logmerge.log_files.append(step.get_log_file_name())

def add_physvalweb_steps(test, slice_names, download_step=None):
    # Collect the steps
    steps = [download_step] if download_step else []
    for slice_name in slice_names:
        sliceweb = PhysValWebStep('PhysValWeb'+slice_name)
        sliceweb.sig = slice_name
        steps.append(sliceweb)

    # Add the steps at the beginning of check_steps list
    test.check_steps = steps + test.check_steps

    # Add the step logs for merging
    logmerge = test.get_step_by_type(LogMergeStep)
    if not logmerge:
        test.log.warning('LogMerge step not found, cannot add PhysValWeb step log files for merging')
    else:
        for step in steps:
            logmerge.log_files.append(step.get_log_file_name())
