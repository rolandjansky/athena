#!/usr/bin/env python

# art-description: Test of transform HITS->RDO with serial athena followed by RDO->RDO_TRIG with threads=1
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Input
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps

hit2rdo = ExecStep.ExecStep('HITtoRDO')
hit2rdo.type = 'Reco_tf'
hit2rdo.input = 'ttbar_HITS'
hit2rdo.args = '--outputRDOFile=RDO.pool.root'

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

rdo2rdotrig = ExecStep.ExecStep('RDOtoRDOTrigger')
rdo2rdotrig.type = 'Reco_tf'
rdo2rdotrig.input = ''
rdo2rdotrig.explicit_input = True
rdo2rdotrig.threads = 1
rdo2rdotrig.args = '--inputRDOFile=RDO.pool.root --outputRDO_TRIGFile=RDO_TRIG.pool.root'
rdo2rdotrig.args += ' --preExec="setMenu=\'LS2_v1_TriggerValidation_mc_prescale\'"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [hit2rdo, rdo2rdotrig]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test, input_file='RDO_TRIG.pool.root')

import sys
sys.exit(test.run())
