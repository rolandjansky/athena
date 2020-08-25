#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This is not an ART test. This is a unit test of the framework used for
# steering Trigger ART tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Common
import logging

import os
if not os.path.exists ('test_unit_trigvalsteering'):
    os.mkdir ('test_unit_trigvalsteering')
os.chdir ('test_unit_trigvalsteering')

Common.trigvalsteering_logging_level = logging.DEBUG
Common.package_prefix_dict['TrigValTools']='unit_'

test = Test.Test()

for test_type in ['athena','athenaHLT','Reco_tf','Trig_reco_tf']:
    ex = ExecStep.ExecStep('Test_'+test_type)
    ex.type = test_type
    if '_tf' not in test_type:
        ex.job_options = 'AthExJobOptions/AthExJobOptions_MinimalJobOptionsNoEvt.py'
    ex.input = ''
    ex.args = '--help'
    test.exec_steps.append(ex)

test.art_type = 'build'
test.check_steps = CheckSteps.default_check_steps(test)

regtest_ref_text = [
    '### athena.log ###',
    '### athena.Test_athena.log ###',
    '### athenaHLT.Test_athenaHLT.log ###',
    '### Reco_tf.Test_Reco_tf.log ###',
    '### Trig_reco_tf.Test_Trig_reco_tf.log ###',
]

with open('regtest.ref','w') as f:
    f.write('\n'.join(regtest_ref_text))

refcomp = CheckSteps.RegTestStep('RefComp')
refcomp.input_base_name = 'athena.merged'
refcomp.regex = '###'
refcomp.reference = 'regtest.ref'
refcomp.required = True
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.LogMergeStep, refcomp)

import sys
sys.exit(test.run())
