#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of reconstruction of Run-1 data with trigger
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Step
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps
import os
import re

ex = ExecStep.ExecStep()
ex.type = 'Reco_tf'
ex.input = 'data_run1'
ex.args = '--outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root --outputHISTFile=HIST.root'
ex.args += ' --autoConfiguration="everything"'
ex.args += ' --conditionsTag "all:COMCOND-BLKPA-RUN1-07"'
ex.args += ' --preExec "all:DQMonFlags.doCTPMon=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doHLTMon=False;" "RAWtoESD:TriggerFlags.doMergedHLTResult=False;"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test)

# Custom check step for non-zero number of containers
class ZeroContainersStep(Step.Step):
    '''Check if number of HLT xAOD containers in AOD is greater than zero'''

    def __init__(self, name='ZeroContainers'):
        super(ZeroContainersStep, self).__init__(name)
        self.auto_report_result = True
        self.required = True
        self.log_file = 'AOD.pool.root.checkxAOD'
        self.pattern = 'HLT_xAOD__'
        self.output_stream = Step.Step.OutputStream.STDOUT_ONLY

    def run(self, dry_run=False):
        if dry_run:
            self.result = 0
            return self.result, '# (internal) {} -> skipped'.format(self.name)
        self.log.info('Running %s step', self.name)
        if not os.path.isfile(self.log_file):
            self.log.error('File %s does not exist', self.log_file)
            self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, '# (internal) {} -> failed'.format(self.name)
        with open(self.log_file) as f:
            matches = re.findall(self.pattern+'.*$', f.read(), re.MULTILINE)
            self.log.info('%s found %d containers matching pattern %s', self.name, len(matches), self.pattern)
            if len(matches) > 0:
                self.result = 0
            else:
                self.result = 1
            if self.auto_report_result:
                self.report_result()
            return self.result, '# (internal) {}'.format(self.name)

test.check_steps.append(ZeroContainersStep())

import sys
sys.exit(test.run())
