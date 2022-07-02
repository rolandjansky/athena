#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test Run 2 to Run 3 EDM Conversion (Navigation & config metadata)
# art-type: build
# art-include: master/Athena
# art-include: 22.0/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

aod2daod = ExecStep.ExecStep('AODtoDAOD')
aod2daod.type = 'Reco_tf'
aod2daod.input = 'data_run2_reco_run3_AOD'
aod2daod.explicit_input = True
aod2daod.args = ' --reductionConf=PHYS --outputDAODFile=DAOD.pool.root'
aod2daod.args += ' --preExec="from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.doEDMVersionConversion=True;"'
aod2daod.args += ' --postExec="from OutputStreamAthenaPool.MultipleStreamManager import MSMgr; daod=MSMgr.GetStream(\\\"StreamDAOD_PHYS\\\"); daod.AddItem(\\\"xAOD::TrigCompositeContainer#HLTNav_R2ToR3Summary\\\"); daod.AddItem(\\\"xAOD::TrigCompositeAuxContainer#HLTNav_R2ToR3SummaryAux.\\\"); daod.AddItem(\\\"TrigRoiDescriptorCollection\\\"); daod.AddItem(\\\"xAOD::TrigEMCluster\\\"); daod.AddItem(\\\"xAOD::TrigEMClusterContainer\\\"); daod.AddItem(\\\"xAOD::TrigRingerRings\\\"); daod.AddItem(\\\"xAOD::TrigRingerRingsContainer\\\"); daod.AddItem(\\\"xAOD::TrigRNNOutput\\\"); daod.AddItem(\\\"xAOD::TrigRNNOutputContainer\\\"); daod.AddItem(\\\"xAOD::CaloClusterContainer\\\"); daod.AddItem(\\\"xAOD::L2StandAloneMuonContainer\\\"); daod.AddItem(\\\"xAOD::L2StandAloneMuonAuxContainer\\\"); daod.AddItem(\\\"xAOD::L2CombinedMuonContainer\\\"); daod.AddItem(\\\"xAOD::L2CombinedMuonAuxContainer\\\"); daod.AddItem(\\\"xAOD::L2IsoMuonContainer\\\"); daod.AddItem(\\\"xAOD::MuonContainer\\\"); daod.AddItem(\\\"xAOD::MuonAuxContainer\\\"); daod.AddItem(\\\"xAOD::TauJetContainer\\\"); daod.AddItem(\\\"xAOD::ElectronContainer\\\"); daod.AddItem(\\\"xAOD::PhotonContainer\\\"); daod.AddItem(\\\"xAOD::JetContainer\\\"); daod.AddItem(\\\"xAOD::BTaggingContainer\\\"); daod.AddItem(\\\"xAOD::BTagVertexContainer\\\"); daod.AddItem(\\\"xAOD::JetElementContainer\\\"); daod.AddItem(\\\"xAOD::TrigMissingET\\\"); daod.AddItem(\\\"xAOD::TrigBphysContainer\\\"); from DerivationFrameworkJetEtMiss.JetCommon import swapAlgsInSequence;swapAlgsInSequence(topSequence,\'jetalg_ConstitModCorrectPFOCSSKCHS_GPFlowCSSK\', \'UFOInfoAlgCSSK\');"'
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [aod2daod]
test.check_steps = CheckSteps.default_check_steps(test)
import sys
sys.exit(test.run())
