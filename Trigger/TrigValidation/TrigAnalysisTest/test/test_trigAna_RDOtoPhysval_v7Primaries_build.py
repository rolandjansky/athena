#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD->NTUP_PHYSVAL with serial athena (legacy trigger)
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

preExec = ';'.join([
  'from TriggerJobOpts.TriggerFlags import TriggerFlags',
  'TriggerFlags.triggerMenuSetup=\'Physics_pp_v7_primaries\'',
  'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
])

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" --valid=True'
rdo2aod.args += ' --preExec="all:{:s};"'.format(preExec)

physval = ExecStep.ExecStep('PhysVal')
physval.type = 'Reco_tf'
physval.input = ''
physval.explicit_input = True
physval.args = '--inputAODFile=AOD.pool.root --outputNTUP_PHYSVALFile=NTUP_PHYSVAL.pool.root --valid=True'

validationFlags = 'doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet'
physval.args += ' --validationFlags="{:s}"'.format(validationFlags)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [rdo2aod,physval]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
