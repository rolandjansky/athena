#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# art-description: FTAG Validation plots run on mc16 AOD file
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 4
# art-memory: 7000
# art-output: *.root
# art-output: *.log
# art-output: *log.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps

preExec = ';'.join([
  'setMenu=\'Dev_pp_run3_v1_TriggerValidation_prescale\'',
  'from AthenaConfiguration.AllConfigFlags import ConfigFlags',
  'ConfigFlags.Trigger.AODEDMSet=\'AODFULL\'',
  'from JetRec.JetRecFlags import jetFlags; jetFlags.writeJetsToAOD = True',
])

rdo2aod = ExecStep.ExecStep("rdo2aod")
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.max_events = 800
rdo2aod.threads = 4
rdo2aod.concurrent_events = 4
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG"'
rdo2aod.args += ' --preExec="all:{:s};"'.format(preExec)


aod2hist_offline = ExecStep.ExecStep("aod2hist_offline")
aod2hist_offline.type = 'athena'
aod2hist_offline.job_options = "FlavourTaggingTests/PhysicsVariablePlots_Configuration.py"
aod2hist_offline.input = ''
aod2hist_offline.max_events = -1
aod2hist_offline.threads = 4
aod2hist_offline.concurrent_events = 4
aod2hist_offline.args  = "-c \""
aod2hist_offline.args += "from AthenaConfiguration.AllConfigFlags import ConfigFlags; "
aod2hist_offline.args += "ConfigFlags.Input.Files='AOD.pool.root'.split(','); "
aod2hist_offline.args += "ConfigFlags.Input.isMC=True; "
aod2hist_offline.args += "ConfigFlags.Output.HISTFileName='Output.FTAGValidation.PhysicsVariablePlots.pool.root'; "
aod2hist_offline.args += "\""

aod2hist_online = ExecStep.ExecStep("aod2hist_online")
aod2hist_online.type = 'athena'
aod2hist_online.job_options = "FlavourTaggingTests/PhysicsTriggerVariablePlots_Configuration.py"
aod2hist_online.input = ''
aod2hist_online.max_events = -1
aod2hist_online.threads = 4
aod2hist_online.concurrent_events = 4
aod2hist_online.args  = "-c \""
aod2hist_online.args += "from AthenaConfiguration.AllConfigFlags import ConfigFlags; "
aod2hist_online.args += "ConfigFlags.Input.Files='AOD.pool.root'.split(','); "
aod2hist_online.args += "ConfigFlags.Input.isMC=True; "
aod2hist_online.args += "ConfigFlags.Output.HISTFileName='Output.FTAGValidation.PhysicsTriggerVariablePlots.pool.root'; "
aod2hist_online.args += "\""

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod, aod2hist_offline, aod2hist_online]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test)

import sys
sys.exit(test.run())



