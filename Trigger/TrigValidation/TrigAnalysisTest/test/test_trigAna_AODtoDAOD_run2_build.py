#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test Run 2 to Run 3 EDM Conversion (Navigation & configg metadata)
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps
input='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/myAOD.pool.root'

aod2daod = ExecStep.ExecStep('AODtoDAOD')
aod2daod.type = 'Reco_tf'
aod2daod.input = ''
#aod2daod.forks = 4
aod2daod.explicit_input = True
aod2daod.args = '--inputAODFile='+input+' --digiSeedOffset1=1 --digiSeedOffset2=1 --outputDAODFile=DAOD.pool.root --reductionConf=PHYS'
aod2daod.args += ' --preExec="default:from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); DetFlags.digitize.all_setOff(); from AthenaMP.AthenaMPFlags import jobproperties as ampjp; ampjp.AthenaMPFlags.ChunkSize=100; from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.doEDMVersionConversion=True;"'
aod2daod.args += ' --postExec="default:from IOVDbSvc.CondDB import conddb; conddb.addFolderSplitOnline(\\\"INDET\\\",\\\"/Indet/Onl/Beampos\\\",\\\"/Indet/Beampos\\\", className=\\\"AthenaAttributeList\\\"); from AthenaCommon.AlgSequence import AthSequencer; condSeq = AthSequencer(\\\"AthCondSeq\\\"); from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg; condSeq += BeamSpotCondAlg( \\\"BeamSpotCondAlg\\\" );"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [aod2daod]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test)

import sys
sys.exit(test.run())
