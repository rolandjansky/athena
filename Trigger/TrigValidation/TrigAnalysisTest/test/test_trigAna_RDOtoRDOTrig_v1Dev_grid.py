#!/usr/bin/env python

# art-description: Test of the RDOtoRDOTrigger transform with threads=1
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

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps
import os

# To run single-process transform on MCORE sites
if 'ATHENA_NPROC_NUM' in os.environ:
    del os.environ['ATHENA_NPROC_NUM']

preExec = ';'.join([
  'setMenu=\'LS2_v1_TriggerValidation_mc_prescale\'',
  'from TriggerJobOpts.TriggerFlags import TriggerFlags',
  'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
])

ex = ExecStep.ExecStep()
ex.type = 'Reco_tf'
ex.input = 'ttbar'
ex.threads = 1
ex.args = '--outputRDO_TRIGFile=RDO_TRIG.pool.root'
ex.args += ' --preExec="all:{:s};"'.format(preExec)

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test, input_file='RDO_TRIG.pool.root')

import sys
sys.exit(test.run())
