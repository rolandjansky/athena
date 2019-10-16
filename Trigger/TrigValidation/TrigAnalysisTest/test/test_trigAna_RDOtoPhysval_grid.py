#!/usr/bin/env python

# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD->NTUP_PHYSVAL with serial athena (legacy trigger)
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
import os

# To run single-process transform on MCORE sites
if 'ATHENA_NPROC_NUM' in os.environ:
    del os.environ['ATHENA_NPROC_NUM']

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" --valid=True'
rdo2aod.args += ' --preExec="all:from TriggerJobOpts.TriggerFlags import TriggerFlags; TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\");"'

physval = ExecStep.ExecStep('PhysVal')
physval.type = 'Reco_tf'
physval.input = ''
physval.explicit_input = True
physval.args = '--inputAODFile=AOD.pool.root --outputNTUP_PHYSVALFile=NTUP_PHYSVAL.pool.root --valid=True'

validationFlags = 'doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet'
physval.args += ' --validationFlags="{:s}"'.format(validationFlags)

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod,physval]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
