#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of the RDOtoRDOTrigger transform with Dev menu
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 4
# art-memory: 6000
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

preExec = ';'.join([
  'setMenu=\'LS2_v1_TriggerValidation_prescale\'',
  'from TriggerJobOpts.TriggerFlags import TriggerFlags',
  'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
  'doRuntimeNaviVal=True', # Perform runtime graph vaidation in this test
])

ex = ExecStep.ExecStep()
ex.type = 'Reco_tf'
ex.input = 'ttbar'
ex.max_events = 800
ex.threads = 4
ex.concurrent_events = 4
ex.args = '--outputRDO_TRIGFile=RDO_TRIG.pool.root'
ex.args += ' --preExec="all:{:s};"'.format(preExec)

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
