#!/usr/bin/env python

# art-description: Runs the full_menu test writing BS output and then runs BS decoding
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

writeBS = ExecStep.ExecStep("WriteBS")
writeBS.type = 'athenaHLT'
writeBS.job_options = 'TriggerJobOpts/runHLT_standalone.py'
writeBS.input = 'data'
writeBS.args = '-o output'

decodeBS = ExecStep.ExecStep("DecodeBS")
decodeBS.type = 'athena'
decodeBS.job_options = 'TriggerJobOpts/decodeBS.py'
decodeBS.input = ''
decodeBS.explicit_input = True
decodeBS.args = '--filesInput=`find . -name \'*_HLTMPPy_output.*.data\' | tail -n 1`'
decodeBS.perfmon = False # no need to run PerfMon for this step

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [writeBS, decodeBS]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
