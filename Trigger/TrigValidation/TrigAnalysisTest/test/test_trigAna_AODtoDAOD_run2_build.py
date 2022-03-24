#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test Run 2 to Run 3 EDM Conversion (Navigation & config metadata)
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

aod2daod = ExecStep.ExecStep('AODtoDAOD')
aod2daod.type = 'Reco_tf'
aod2daod.input = 'data_run2_reco_run3_AOD'
aod2daod.explicit_input = True
aod2daod.args = ' --reductionConf=PHYS --outputDAODFile=DAOD.pool.root'
aod2daod.args += ' --preExec="from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.doEDMVersionConversion=True;"'
aod2daod.args += ' --postExec="from OutputStreamAthenaPool.MultipleStreamManager import MSMgr; daod=MSMgr.GetStream(\\\"StreamDAOD_PHYS\\\"); daod.AddItem(\\\"xAOD::TrigCompositeContainer#HLTNav_R2ToR3Summary\\\"); daod.AddItem(\\\"xAOD::TrigCompositeAuxContainer#HLTNav_R2ToR3SummaryAux.\\\"); "'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [aod2daod]
test.check_steps = CheckSteps.default_check_steps(test)
import sys
sys.exit(test.run())
