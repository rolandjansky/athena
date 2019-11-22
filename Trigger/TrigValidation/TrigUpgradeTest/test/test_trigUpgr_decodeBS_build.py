#!/usr/bin/env python

# art-description: Runs the full_menu test writing BS output and then runs BS decoding
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

writeBS = ExecStep.ExecStep("WriteBS")
writeBS.type = 'athena'
writeBS.job_options = 'TrigUpgradeTest/full_menu.py'
writeBS.input = 'data'
writeBS.args = '-c "isOnline=True;doWriteBS=True;doWriteRDOTrigger=False;"'

decodeBS = ExecStep.ExecStep("DecodeBS")
decodeBS.type = 'athena'
decodeBS.job_options = 'TriggerJobOpts/decodeBS.py'
decodeBS.input = ''
decodeBS.explicit_input = True
decodeBS.args = '--filesInput=data_test.00360026.Single_Stream.daq.RAW._lb0151._Athena._0000.data'
decodeBS.perfmon = False # no need to run PerfMon for this step

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [writeBS, decodeBS]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
