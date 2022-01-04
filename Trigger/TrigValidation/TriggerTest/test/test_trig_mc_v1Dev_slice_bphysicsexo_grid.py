#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger RDO->RDO_TRIG athena test of the b-physics slice in Dev_pp_run3_v1 menu (Wtaunu_3mu input)
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 4
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
ex.input = 'mubphysics'
ex.threads = 4
ex.concurrent_events = 4
ex.args = '-c "setMenu=\'Dev_pp_run3_v1\';doEmptyMenu=True;doBphysicsSlice=True;doMuonSlice=True;doWriteBS=False;doWriteRDOTrigger=True;setGlobalTag=\'OFLCOND-RUN12-SDR-31\';from IOVDbSvc.CondDB import conddb;conddb.addOverride(\'/PIXEL/PixelModuleFeMask\',\'PixelModuleFeMask-SIM-MC16-000-03\')"'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
