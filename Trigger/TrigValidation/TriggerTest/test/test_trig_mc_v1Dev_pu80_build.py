#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger RDO->RDO_TRIG athena test of the Dev_pp_run3_v1 menu with pileup80 ttbar sample
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'ttbar_pu80'
ex.threads = 1
precommand = ''.join([
  "setMenu='Dev_pp_run3_v1_TriggerValidation_prescale';",  
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
    "setGlobalTag='OFLCOND-RUN12-SDR-31';from IOVDbSvc.CondDB import conddb;conddb.addOverride('/PIXEL/PixelModuleFeMask','PixelModuleFeMask-SIM-MC16-000-03');conddb.addOverride('/TRT/Calib/PID_NN', 'TRTCalibPID_NN_v1')"
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
