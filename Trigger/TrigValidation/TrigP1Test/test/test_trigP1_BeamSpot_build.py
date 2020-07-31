#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: BeamSpot update test using athenaHLT
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, Step, ExecStep, CheckSteps
from TrigValTools.TrigValSteering.Input import get_input

# Delete any previous bytestream file
ex_rm = ExecStep.ExecStep('cleanup')
ex_rm.type = 'other'
ex_rm.input = ''
ex_rm.executable = 'rm'
ex_rm.args = '-f beamspot._0001.data'
ex_rm.auto_report_result = False  # Do not set art-result for this step
ex_rm.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step

# Create new bytestream file
ex_bs = ExecStep.ExecStep('create_bs')
ex_bs.type = 'other'
ex_bs.input = ''
ex_bs.executable = 'python'
ex_bs.args = '-m TrigP1Test.BeamSpotUpdate -n 50 -o beamspot %s' % get_input('data').paths[0]

# Run athenaHLT
ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigP1Test/testHLT_beamspot.py'
ex.input = ''
ex.explicit_input = True
ex.args = '-f ./beamspot._0001.data'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex_rm, ex_bs, ex]
# Only keep a few relevant checks
test.check_steps = [chk for chk in CheckSteps.default_check_steps(test)
                    if type(chk) in (CheckSteps.LogMergeStep, CheckSteps.CheckLogStep, CheckSteps.RegTestStep)]

# Add a reference comparison step
refcomp = CheckSteps.RegTestStep('RefComp')
refcomp.regex = 'InDet::InDetBeamSpotReader.*|BeamSpotCondAlg.*'
refcomp.reference = 'TrigP1Test/BeamSpot.ref'
refcomp.required = True # Final exit code depends on this step
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.LogMergeStep, refcomp)

import sys
sys.exit(test.run())
