#!/usr/bin/env python

# art-description: Runs the full_menu test writing BS output and then runs BS decoding
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
# art-output: *.check*

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

writeBS = ExecStep.ExecStep("WriteBS")
writeBS.type = 'athenaHLT'
writeBS.job_options = 'TrigUpgradeTest/full_menu.py'
writeBS.input = 'data'
writeBS.args = '-o output'
writeBS.perfmon = False # perfmon with athenaHLT doesn't work at the moment

decodeBS = ExecStep.ExecStep("DecodeBS")
decodeBS.type = 'athena'
decodeBS.job_options = 'TrigUpgradeTest/decodeBS.py'
decodeBS.input = ''
decodeBS.explicit_input = True
decodeBS.args = '--filesInput=output_Child-001._0001.data'
decodeBS.perfmon = False # no need to run PerfMon for this step

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [writeBS, decodeBS]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
