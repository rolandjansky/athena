#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger RDO->RDO_TRIG athena test of the Dev_pp_run3_v1 menu with pileup80 ttbar sample
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: prmon*
# art-output: *.check*

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'ttbar_pu80'
ex.threads = 8
ex.concurrent_events = 8
ex.args = '-c "setMenu=\'Dev_pp_run3_v1_TriggerValidation_prescale\';doWriteBS=False;doWriteRDOTrigger=True;setGlobalTag=\'OFLCOND-RUN12-SDR-31\';from IOVDbSvc.CondDB import conddb;conddb.addOverride(\'/PIXEL/PixelModuleFeMask\',\'PixelModuleFeMask-SIM-MC16-000-03\')"'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
