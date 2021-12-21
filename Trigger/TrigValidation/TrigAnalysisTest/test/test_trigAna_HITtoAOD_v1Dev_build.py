#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of transform HITS->RDO followed by RDO->RDO_TRIG,ESD,AOD
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Input

hit2rdo = ExecStep.ExecStep('HITtoRDO')
hit2rdo.type = 'Reco_tf'
hit2rdo.input = 'ttbar_HITS'
hit2rdo.args = '--outputRDOFile=RDO.pool.root'
hit2rdo.max_events = 10

pu_low = Input.get_input('pileup_low')
pu_high = Input.get_input('pileup_high')
hit2rdo.args += ' --inputLowPtMinbiasHitsFile=' + pu_low.paths[0]
hit2rdo.args += ' --inputHighPtMinbiasHitsFile=' + pu_high.paths[0]
hit2rdo.args += ' --numberOfCavernBkg="0"'
hit2rdo.args += ' --numberOfHighPtMinBias="0.15520183"'
hit2rdo.args += ' --numberOfLowPtMinBias="59.3447981771"'
hit2rdo.args += ' --pileupFinalBunch="6"'
hit2rdo.args += ' --jobNumber="1"'

hit2rdo.args += ' --preExec "HITtoRDO:userRunLumiOverride={\'run\':300000, \'startmu\':40.0, \'endmu\':70.0, \'stepmu\':1.0, \'startlb\':1, \'timestamp\': 1500000000};ScaleTaskLength=0.1"'
hit2rdo.args += ' --preInclude "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_muRange.py"'
hit2rdo.args += ' --postInclude="h2r:LArROD/LArSuperCellEnable.py"' # to be removed when Run3 geometry RDO is integrated in Trigger ART tests

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = ''
rdo2aod.explicit_input = True
rdo2aod.threads = 1
rdo2aod.args = '--inputRDOFile=RDO.pool.root --outputRDO_TRIGFile=RDO_TRIG.pool.root --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root'
rdo2aod.args += ' --preExec="setMenu=\'LS2_v1_TriggerValidation_prescale\'"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [hit2rdo, rdo2aod]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
