#!/usr/bin/env python

# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD with threads=1 followed by AOD->NTUP_PHYSVAL with serial athena
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.threads = 1
rdo2aod.args = '--AMI=q221 --outputAODFile=AOD.pool.root --steering="doRDO_TRIG"'

physval = ExecStep.ExecStep('PhysVal')
physval.type = 'Reco_tf'
physval.input = ''
physval.explicit_input = True
physval.args = '--AMI=q221 --inputAODFile=AOD.pool.root --outputNTUP_PHYSVALFile=NTUP_PHYSVAL.pool.root'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [rdo2aod,physval]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
