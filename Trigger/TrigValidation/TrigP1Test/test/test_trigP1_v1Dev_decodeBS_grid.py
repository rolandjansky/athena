#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: Runs athenaHLT writing BS output and then runs BS decoding
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
# art-memory: 7000

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

##################################################
# Helper functions to build the test steps
##################################################
from TrigP1Test.TrigP1TestSteps import filterBS, decodeBS

##################################################
# Test definition
##################################################

# Run standard athenaHLT BS->BS job
writeBS = ExecStep.ExecStep("WriteBS")
writeBS.type = 'athenaHLT'
writeBS.job_options = 'TriggerJobOpts/runHLT_standalone.py'
writeBS.input = 'data'
writeBS.threads = 4
writeBS.concurrent_events = 4
writeBS.args = '-o output'
writeBS.args += ' -c "setMenu=\'Dev_pp_run3_v1_TriggerValidation_prescale\';doL1Sim=True;rewriteLVL1=True;"'  
writeBS.args += ' --dump-config-reload'

# Extract and decode physics_Main
filterMain = filterBS("Main")
decodeMain = decodeBS("Main")

# Extract and decode calibration_CostMonitoring
filterCost = filterBS("CostMonitoring")
decodeCost = decodeBS("CostMonitoring")
decodeCost.args += ' -c "ModuleID=1"'

# Test definition with updated CheckFile inputs
test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [writeBS, filterMain, decodeMain, filterCost, decodeCost]
test.check_steps = CheckSteps.default_check_steps(test)
test.get_step('CheckFile').input_file = 'ESD.pool.root,ESD.Module1.pool.root'

import sys
sys.exit(test.run())
