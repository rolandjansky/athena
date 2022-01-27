#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Runs athenaHLT writing BS output and then runs BS decoding
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigValTools.TrigValSteering.Common import find_file

##################################################
# Helper functions to build the test steps
##################################################

def filterBS(stream_name):
    '''Extract ByteStream data for a given stream from a file with multiple streams'''
    filterStep = ExecStep.ExecStep('FilterBS_'+stream_name)
    filterStep.type = 'other'
    filterStep.executable = 'trigbs_extractStream.py'
    filterStep.input = ''
    filterStep.args = '-s ' + stream_name + ' ' + find_file('*_HLTMPPy_output.*.data')
    return filterStep

def decodeBS(stream_name):
    '''Deserialise HLT data from ByteStream and save to an ESD file'''
    decodeStep = ExecStep.ExecStep('DecodeBS_'+stream_name)
    decodeStep.type = 'athena'
    decodeStep.job_options = 'TriggerJobOpts/decodeBS.py'
    decodeStep.input = ''
    decodeStep.explicit_input = True
    decodeStep.args = '--filesInput='+find_file('*'+stream_name+'*._athenaHLT*.data')
    decodeStep.perfmon = False  # no need to run PerfMon for this step
    return decodeStep

##################################################
# Test definition
##################################################

# Run standard athenaHLT BS->BS job
writeBS = ExecStep.ExecStep("WriteBS")
writeBS.type = 'athenaHLT'
writeBS.job_options = 'TriggerJobOpts/runHLT_standalone.py'
writeBS.input = 'data'
writeBS.max_events = 50
precommand = ''.join([
  "setMenu='Dev_pp_run3_v1_TriggerValidation_prescale';",
  "doL1Sim=True;",
  "rewriteLVL1=True;",
  "doRuntimeNaviVal=True;",
])
writeBS.args = '-c "{:s}"'.format(precommand)
writeBS.args += ' -o output'
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
test.art_type = 'build'
test.exec_steps = [writeBS, filterMain, decodeMain, filterCost, decodeCost]
test.check_steps = CheckSteps.default_check_steps(test)
test.get_step('CheckFile').input_file = 'ESD.pool.root,ESD.Module1.pool.root'

# Overwrite default MessageCount settings
# We are trying to lower the limits step by step
# Ultimately there should be no per-event messages
msgcount = test.get_step("MessageCount")
msgcount.thresholds = {
  'WARNING': 4000,  # TODO: Fix the warnings and decrease the limit, ATR-23548, ATR-22942
  'INFO': 600,
  'other': 100
}
msgcount.required = True # make the test exit code depend on this step

# Add a step comparing counts against a reference
chaindump = test.get_step("ChainDump")
chaindump.args = '--json --yaml ref_v1Dev_decodeBS_build.new'
refcomp = CheckSteps.ChainCompStep("CountRefComp")
refcomp.input_file = 'ref_v1Dev_decodeBS_build.new'
refcomp.args += ' --patch'
refcomp.reference_from_release = True # installed from TrigP1Test/share
refcomp.required = True # Final exit code depends on this step
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.ChainDumpStep, refcomp)

import sys
sys.exit(test.run())
