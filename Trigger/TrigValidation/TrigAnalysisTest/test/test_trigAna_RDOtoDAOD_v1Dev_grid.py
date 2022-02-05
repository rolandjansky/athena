#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD with threads=4 and AOD->DAOD with multiprocess
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 4
# art-memory: 7000
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

preExec = ';'.join([
  'setMenu=\'Dev_pp_run3_v1_TriggerValidation_prescale\'',
  'from AthenaConfiguration.AllConfigFlags import ConfigFlags',
  'ConfigFlags.Trigger.AODEDMSet=\'AODFULL\'',
])

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.max_events = 100
rdo2aod.threads = 4
rdo2aod.concurrent_events = 4
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG"'
rdo2aod.args += ' --preExec="all:{:s};"'.format(preExec)

aod2daod = ExecStep.ExecStep('AODtoDAOD')
aod2daod.type = 'Reco_tf'
aod2daod.input = ''
aod2daod.forks = 4
aod2daod.explicit_input = True
aod2daod.args = '--inputAODFile=AOD.pool.root --sharedWriter=True --runNumber=300001 --digiSeedOffset1=1 --digiSeedOffset2=1 --outputDAODFile=DAOD.pool.root --reductionConf=PHYS'
aod2daod.args += ' --preExec="default:from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); DetFlags.digitize.all_setOff(); DetFlags.simulate.all_setOff(); from AthenaMP.AthenaMPFlags import jobproperties as ampjp; ampjp.AthenaMPFlags.ChunkSize=100;"'
aod2daod.args += ' --postExec="default:from IOVDbSvc.CondDB import conddb; conddb.addFolderSplitOnline(\\\"INDET\\\",\\\"/Indet/Onl/Beampos\\\",\\\"/Indet/Beampos\\\", className=\\\"AthenaAttributeList\\\"); from AthenaCommon.AlgSequence import AthSequencer; condSeq = AthSequencer(\\\"AthCondSeq\\\"); from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg; condSeq += BeamSpotCondAlg( \\\"BeamSpotCondAlg\\\" );"'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod, aod2daod]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test)

import sys
sys.exit(test.run())
