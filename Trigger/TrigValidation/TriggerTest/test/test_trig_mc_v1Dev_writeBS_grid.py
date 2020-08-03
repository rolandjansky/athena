#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger RDO->BS athena test of the Dev_pp_run3_v1 menu
# art-type: grid
# art-include: master/Athena
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

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Step

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'ttbar'
ex.threads = 1
# LS2_v1 soon to be renamed to Dev_pp_run3_v1
ex.args = '-c "setMenu=\'LS2_v1\';doWriteBS=True;doWriteRDOTrigger=False;"'

checkBS = Step.Step("CheckBS")
checkBS.executable = 'trigbs_dumpHLTContentInBS_run3.py'
checkBS.args = ' --l1 --hlt --hltres --stag --sizeSummary'
checkBS.args += ' `find . -name \'*Single_Stream.daq.RAW.*Athena.*.data\' | tail -n 1`'
checkBS.required = True
checkBS.auto_report_result = True

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = [checkBS] + CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
