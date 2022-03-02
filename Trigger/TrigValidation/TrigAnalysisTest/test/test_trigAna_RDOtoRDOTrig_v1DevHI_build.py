#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of the RDOtoRDOTrigger transform with threads=1
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

preExec = ';'.join([
  'setMenu=\'Dev_HI_run3_v1\'',
  'doWriteBS=False',
  'doWriteRDOTrigger=True',
  'forceEnableAllChains=True',
  'from AthenaConfiguration.AllConfigFlags import ConfigFlags',
  'ConfigFlags.Trigger.AODEDMSet=\'AODFULL\'',
])

ex = ExecStep.ExecStep()
ex.type = 'Reco_tf'
ex.input = 'pbpb'
ex.threads = 1
ex.args = '--outputRDO_TRIGFile=RDO_TRIG.pool.root'
ex.args += ' --ignorePatterns "Py:Configurable.+attempt to add a duplicate.+"'
ex.args += ' --preExec="all:{:s};"'.format(preExec)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

chaindump = test.get_step("ChainDump")
chaindump.args = '--json --yaml ref_RDOtoRDOTrig_v1DevHI_build.new'

refcomp = CheckSteps.ChainCompStep("CountRefComp")
refcomp.input_file = 'ref_RDOtoRDOTrig_v1DevHI_build.new'
refcomp.args += ' --patch'
refcomp.reference_from_release = True # installed from TrigAnalysisTest/share
refcomp.required = True

CheckSteps.add_step_after_type(test.check_steps, CheckSteps.ChainDumpStep, refcomp)

import sys
sys.exit(test.run())
