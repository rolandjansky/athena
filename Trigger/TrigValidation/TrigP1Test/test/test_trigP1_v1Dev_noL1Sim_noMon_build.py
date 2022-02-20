#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: athenaHLT test of the Dev_pp_run3_v1 menu without monitoring (ATR-24655)
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, PyStep
import json

# 1) Dump JSON configuration
ex1 = ExecStep.ExecStep('dumpConfig')
ex1.type = 'athenaHLT'
ex1.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex1.input = 'data'
ex1.args = '-c "setMenu=\'Dev_pp_run3_v1\';"'
ex1.args += ' --dump-config-exit'
ex1.perfmon = False

# 2) Remove all MonTools
def fix_json():
   """Disable monitoring in JSON"""
   with open('HLTJobOptions.json','r') as f:
      data = json.load(f)
      for alg, props in data['properties'].items():
         for pname, pvalue in props.items():
            # Remove GenericMonitoringTool from ToolHandle(Array)
            if 'GenericMonitoringTool' in pvalue:
               data['properties'][alg][pname] = '[]' if '[' in pvalue else ''

   with open('HLTJobOptions.json','w') as f:
      json.dump(data, f, indent=4, sort_keys=True, ensure_ascii=True)

noMon = PyStep.PyStep(fix_json)

# 3) Run from new JSON file
ex2 = ExecStep.ExecStep()
ex2.type = 'athenaHLT'
ex2.use_pickle = True   # do not check existence of job options
ex2.job_options = 'HLTJobOptions.json'
ex2.input = 'data'
ex2.perfmon = False

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex1, noMon, ex2]
# Only keep relevant checks from the defaults
test.check_steps = [chk for chk in CheckSteps.default_check_steps(test)
                    if type(chk) in (CheckSteps.LogMergeStep, CheckSteps.CheckLogStep)]

import sys
sys.exit(test.run())
