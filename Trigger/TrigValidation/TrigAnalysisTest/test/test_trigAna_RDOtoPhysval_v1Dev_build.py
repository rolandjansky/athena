#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD with threads=1 followed by AOD->NTUP_PHYSVAL with serial athena
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

preExec = ';'.join([
  'setMenu=\'Dev_pp_run3_v1_TriggerValidation_prescale\'',
  'from AthenaConfiguration.AllConfigFlags import ConfigFlags',
  'ConfigFlags.Trigger.AODEDMSet=\'AODFULL\'',
])

preExecESDtoAOD = ';'.join(['from JetRec.JetRecFlags import jetFlags',
                            'jetFlags.writeJetsToAOD.set_Value_and_Lock(True)',
                            'jetFlags.detailLevel.set_Value_and_Lock(4)',
                            'from METReconstruction.METRecoFlags import metFlags',
                            'metFlags.WriteMETAssocToOutput.set_Value_and_Lock(True)'])

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.threads = 1
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" --valid=True'
rdo2aod.args += ' --preExec="all:{:s};" "ESDtoAOD:{:s}"'.format(preExec, preExecESDtoAOD)

physval = ExecStep.ExecStep('PhysVal')
physval.type = 'Reco_tf'
physval.input = ''
physval.explicit_input = True
physval.args = '--inputAODFile=AOD.pool.root --outputNTUP_PHYSVALFile=NTUP_PHYSVAL.pool.root --valid=True'
physval.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'

validationFlags = 'doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet'
validationPreExec = 'from TrigEDMConfig import ContainerRemapping_Run2Run3; ContainerRemapping_Run2Run3.remapHLTContainerNames(); '
physval.args += ' --validationFlags="{:s}" --preExec="{:s}"'.format(validationFlags, validationPreExec)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [rdo2aod,physval]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
