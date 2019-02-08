#!/usr/bin/env python

import pickle
import subprocess
import unittest
import os


class TestAtlasG4Cosmics(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        config_picklefilename = 'AtlasG4Cosmics_config.pkl'
        command = [
            'AtlasG4_tf.py',
            '--athenaopts', '"--config-only={}"'.format(config_picklefilename),
            '--conditionsTag', 'OFLCOND-MC16-SDR-14',
            '--physicsList', 'FTFP_BERT_ATL',
            '--maxEvents', '2',
            '--postInclude', 'PyJobTransforms/UseFrontier.py',
            '--geometryVersion', 'ATLAS-R2-2016-01-00-01',
            '--DataRunNumber', '284500',
            '--outputHITSFile', 'Hits.atlasg4.pool.root',
            '--CosmicFilterVolume', 'Calo',
            '--CosmicFilterVolume2', 'NONE',
            '--CosmicPtSlice', 'NONE',
            '--outputEVNT_TRFile', 'test.TR.pool.root',
            '--beamType', 'cosmics',
            # would otherwise fail due to missing HITS file:
            '--outputFileValidation', 'False',
        ]
        subprocess.check_call(command)

        with open(config_picklefilename) as picklefile:
            job_config_dict = pickle.load(picklefile)

        cls._job_config_dict = job_config_dict


    def _assert_Algorithm_property_unordered_equal(self,
                                                   algorithm_name,
                                                   property_name,
                                                   expected_property_value):
        algconfigdict = self._job_config_dict[algorithm_name]
        actual_property_value_as_str = algconfigdict[property_name]
        # need to evaluate to obtain actual Python object
        actual_property_value = eval(actual_property_value_as_str)
        expected_property_value_sorted = sorted(expected_property_value)
        actual_property_value_sorted = sorted(actual_property_value)

        failure_message = "{algorithm}.{property} has a different " \
                          "value than expected!\n" \
                          "expected (sorted):\n" \
                          " {expected}\n" \
                          "actual (sorted):\n" \
                          " {actual}".format(
                              algorithm=algorithm_name,
                              property=property_name,
                              expected=expected_property_value_sorted,
                              actual=actual_property_value_sorted)
        self.assertEqual(expected_property_value_sorted,
                         actual_property_value_sorted,
                         failure_message)


    def _assert_Algorithm_property_ordered_equal(self,
                                                 algorithm_name,
                                                 property_name,
                                                 expected_property_value):
        algconfigdict = self._job_config_dict[algorithm_name]
        actual_property_value_as_str = algconfigdict[property_name]
        # need to evaluate to obtain actual Python object
        actual_property_value = eval(actual_property_value_as_str)

        failure_message = "{algorithm}.{property} has a different " \
                          "value than expected!\n" \
                          "expected:\n" \
                          " {expected}\n" \
                          "actual:\n" \
                          " {actual}".format(
                              algorithm=algorithm_name,
                              property=property_name,
                              expected=str(expected_property_value),
                              actual=str(actual_property_value))
        self.assertEqual(expected_property_value,
                         actual_property_value,
                         failure_message)


    def _assert_Algorithm_property_equal(self,
                                   algorithm_name,
                                   property_name,
                                   expected_property_value):
        algconfigdict = self._job_config_dict[algorithm_name]
        actual_property_value = algconfigdict[property_name]

        failure_message = "{algorithm}.{property} has a different " \
                          "value than expected!\n" \
                          "expected:\n" \
                          " {expected}\n" \
                          "actual:\n" \
                          " {actual}".format(
                              algorithm=algorithm_name,
                              property=property_name,
                              expected=str(expected_property_value),
                              actual=str(actual_property_value))
        self.assertEqual(str(expected_property_value),
                         str(actual_property_value),
                         failure_message)


    def test___CosmicGenerator_is_at_start_of_AthAlgSeq(self):
        ath_alg_seqence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ath_alg_seqence_list = eval(ath_alg_seqence_as_str)
        actual_last_ath_alg_sequence_entry = ath_alg_seqence_list[1] # xAODMaker::EventInfoCnvAlg/xAODMaker::EventInfoCnvAlg is in position 0 in master now.
        expected_last_ath_alg_sequence_entry = "CosmicGenerator/CosmicGenerator"
        self.assertEqual(expected_last_ath_alg_sequence_entry,
                         actual_last_ath_alg_sequence_entry)


    def test___CosmicGenerator_emin_setCorrectly(self):
        expected_property_value = 10000
        self._assert_Algorithm_property_equal('CosmicGenerator', 'emin', expected_property_value)


    def test___CosmicGenerator_emax_setCorrectly(self):
        expected_property_value = 5000000
        self._assert_Algorithm_property_equal('CosmicGenerator', 'emax', expected_property_value)


    def test___CosmicGenerator_IPx_setCorrectly(self):
        expected_property_value = 0.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'IPx', expected_property_value)


    def test___CosmicGenerator_IPy_setCorrectly(self):
        expected_property_value = 0.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'IPy', expected_property_value)


    def test___CosmicGenerator_IPz_setCorrectly(self):
        expected_property_value = 0.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'IPz', expected_property_value)


    def test___CosmicGenerator_ctcut_setCorrectly(self):
        expected_property_value = 0.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'ctcut', expected_property_value)


    def test___CosmicGenerator_xvert_low_setCorrectly(self):
        expected_property_value = -200000.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'xvert_low', expected_property_value)


    def test___CosmicGenerator_xvert_hig_setCorrectly(self):
        expected_property_value = 200000.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'xvert_hig', expected_property_value)


    def test___CosmicGenerator_yvert_val_setCorrectly(self):
        expected_property_value = 98300.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'yvert_val', expected_property_value)


    def test___CosmicGenerator_zvert_low_setCorrectly(self):
        expected_property_value = -200000.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'zvert_low', expected_property_value)


    def test___CosmicGenerator_zvert_hig_setCorrectly(self):
        expected_property_value = 200000.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'zvert_hig', expected_property_value)


    def test___CosmicGenerator_PartPropSvc_setCorrectly(self):
        expected_property_value = 'PartPropSvc'
        self._assert_Algorithm_property_equal('CosmicGenerator', 'PartPropSvc', expected_property_value)


    def test___CosmicGenerator_AtRndmGenSvc_setCorrectly(self):
        expected_property_value = 'AtDSFMTGenSvc'
        self._assert_Algorithm_property_equal('CosmicGenerator', 'AtRndmGenSvc', expected_property_value)


    def test___CosmicGenerator_OptimizeForCavern_setCorrectly(self):
        expected_property_value = True
        self._assert_Algorithm_property_equal('CosmicGenerator', 'OptimizeForCavern', expected_property_value)


    def test___CosmicGenerator_Radius_setCorrectly(self):
        expected_property_value = 10000.0
        self._assert_Algorithm_property_equal('CosmicGenerator', 'Radius', expected_property_value)


    def test___BeamEffectsAlg_is_before_G4AtlasAlg_in_AthAlgSeq(self):
        ath_alg_seqence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ath_alg_seqence_list = eval(ath_alg_seqence_as_str)
        actual_last_ath_alg_sequence_entry = ath_alg_seqence_list[3] # Position 1 is the Sim Timer Alg, xAODMaker::EventInfoCnvAlg/xAODMaker::EventInfoCnvAlg is in position 0 in master now
        expected_last_ath_alg_sequence_entry = "Simulation::BeamEffectsAlg/BeamEffectsAlg"
        self.assertEqual(expected_last_ath_alg_sequence_entry,
                         actual_last_ath_alg_sequence_entry)


    def test___BeamEffectsAlg_InputTruthCollection_setCorrectly(self):
        expected_input_truth = 'GEN_EVENT'
        self._assert_Algorithm_property_equal('BeamEffectsAlg', 'InputMcEventCollection', expected_input_truth)


    def test___BeamEffectsAlg_OutputTruthCollection_setCorrectly(self):
        expected_output_truth = 'BeamTruthEvent'
        self._assert_Algorithm_property_equal('BeamEffectsAlg', 'OutputMcEventCollection', expected_output_truth)


    def test___BeamEffectsAlg_ISFRun_setCorrectly(self):
        expected_ISFRun = False
        self._assert_Algorithm_property_equal('BeamEffectsAlg', 'ISFRun', expected_ISFRun)


    def test___BeamEffectsAlg_GenEventManipulators_setCorrectly(self):
        expected_property_value = ['Simulation::GenEventValidityChecker/GenEventValidityChecker']
        self._assert_Algorithm_property_ordered_equal('BeamEffectsAlg', 'GenEventManipulators', expected_property_value) #TODO should check order


    def test___G4AtlasAlg_is_at_end_of_AthAlgSeq(self):
        ath_alg_seqence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ath_alg_seqence_list = eval(ath_alg_seqence_as_str)
        actual_last_ath_alg_sequence_entry = ath_alg_seqence_list[-1]
        expected_last_ath_alg_sequence_entry = "G4AtlasAlg/G4AtlasAlg"
        self.assertEqual(expected_last_ath_alg_sequence_entry,
                         actual_last_ath_alg_sequence_entry)


    def test___G4AtlasAlg_ListOfSetProperties(self):
        expected_list = ['AtRndmGenSvc', 'DetGeoSvc', 'DetStore', 'EvtStore', 'ExtraInputs', 'ExtraOutputs', 'FastSimMasterTool', 'FlagAbortedEvents', 'G4AtlasSvc', 'G4Commands', 'GeoIDSvc', 'InputConverter', 'InputTruthCollection', 'KillAbortedEvents', 'MultiThreading', 'NeededResources', 'OutputTruthCollection', 'PhysicsListTool', 'RandomGenerator', 'RecordFlux', 'ReleaseGeoModel', 'SenDetMasterTool', 'TruthRecordService', 'UserActionSvc', 'Verbosities']
        g4atlasalg = self._job_config_dict['G4AtlasAlg']
        actual_list = g4atlasalg.keys()
        expected_property_value_sorted = sorted(expected_list)
        actual_property_value_sorted = sorted(actual_list)

        failure_message = "G4AtlasAlg.keys() has a different " \
                          "value than expected!\n" \
                          "expected (sorted):\n" \
                          " {expected}\n" \
                          "actual (sorted):\n" \
                          " {actual}".format(
                              expected=expected_property_value_sorted,
                              actual=actual_property_value_sorted)
        self.assertEqual(expected_property_value_sorted,
                         actual_property_value_sorted,
                         failure_message)


    def test___G4AtlasAlg_InputTruthCollection_setCorrectly(self):
        expected_input_truth = 'BeamTruthEvent'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'InputTruthCollection', expected_input_truth)


    def test___G4AtlasAlg_OutputTruthCollection_setCorrectly(self):
        expected_output_truth = 'TruthEvent'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'OutputTruthCollection', expected_output_truth)


    def test___G4AtlasAlg_FastSimMasterTool_setCorrectly(self):
        expected_tool_name = 'FastSimulationMasterTool/FastSimulationMasterTool'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'FastSimMasterTool', expected_tool_name)


    def test___G4AtlasAlg_PhysicsListTool_setCorrectly(self):
        expected_tool_name = 'PhysicsListToolBase/PhysicsListToolBase'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'PhysicsListTool', expected_tool_name)


    def test___G4AtlasAlg_SenDetMasterTool_setCorrectly(self):
        expected_tool_name = 'SensitiveDetectorMasterTool/SensitiveDetectorMasterTool'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'SenDetMasterTool', expected_tool_name)


    def test___G4AtlasAlg_G4AtlasSvc_setCorrectly(self):
        expected_service_name = 'G4AtlasSvc'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'G4AtlasSvc', expected_service_name)


    def test___G4AtlasAlg_UserActionSvc_setCorrectly(self):
        expected_service_name = 'G4UA::UserActionSvc'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'UserActionSvc', expected_service_name)


    def test___G4AtlasAlg_DetGeoSvc_setCorrectly(self):
        expected_service_name = 'DetectorGeometrySvc'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'DetGeoSvc', expected_service_name)


    def test___G4AtlasAlg_GeoIDSvc_setCorrectly(self):
        expected_service_name = 'ISF_GeoIDSvc'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'GeoIDSvc', expected_service_name)


    def test___G4AtlasAlg_AtRndmGenSvc_setCorrectly(self):
        expected_service_name = 'AthRNGSvc'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'AtRndmGenSvc', expected_service_name)


    def test___G4AtlasAlg_TruthRecordService_setCorrectly(self):
        expected_service_name = 'ISF_MC12TruthService'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'TruthRecordService', expected_service_name)


    def test___G4AtlasAlg_InputConverter_setCorrectly(self):
        expected_service_name = 'ISF_InputConverter'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'InputConverter', expected_service_name)


    def test___G4AtlasAlg_RandomGenerator_setCorrectly(self):
        expected_service_name = 'athena'
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'RandomGenerator', expected_service_name)


    def test___G4AtlasAlg_G4Commands_setCorrectly(self):
        expected_property_value = ['/run/verbose 2'] #TODO this will probably change as the configuration is migrated
        self._assert_Algorithm_property_unordered_equal('G4AtlasAlg', 'G4Commands', expected_property_value)


    def test___G4AtlasAlg_Verbosities_setCorrectly(self):
        expected_property_value = {} #TODO this will probably change as the configuration is migrated
        self._assert_Algorithm_property_unordered_equal('G4AtlasAlg', 'Verbosities', expected_property_value)


    def test___G4AtlasAlg_ReleaseGeoModel_setCorrectly(self):
        expected_property_value = True
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'ReleaseGeoModel', expected_property_value)


    def test___G4AtlasAlg_KillAbortedEvents_setCorrectly(self):
        expected_property_value = True
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'KillAbortedEvents', expected_property_value)


    def test___G4AtlasAlg_FlagAbortedEvents_setCorrectly(self):
        expected_property_value = False
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'FlagAbortedEvents', expected_property_value)


    def test___G4AtlasAlg_MultiThreading_setCorrectly(self):
        expected_property_value = False
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'MultiThreading', expected_property_value)


    def test___G4AtlasAlg_RecordFlux_setCorrectly(self):
        expected_property_value = False
        self._assert_Algorithm_property_equal('G4AtlasAlg', 'RecordFlux', expected_property_value)


    def test___GeoModelSvc_DetectorTools_setCorrectly(self):
        expected_property_value = ['PixelDetectorTool/PixelDetectorTool', 'SCT_DetectorTool/SCT_DetectorTool', 'TRT_DetectorTool/TRT_DetectorTool', 'InDetServMatTool/InDetServMatTool', 'LArDetectorToolNV/LArDetectorToolNV', 'TileDetectorTool/TileDetectorTool', 'MuonDetectorTool/MuonDetectorTool', 'BeamPipeDetectorTool/BeamPipeDetectorTool', 'CavernInfraDetectorTool/CavernInfraDetectorTool']
        self._assert_Algorithm_property_ordered_equal('GeoModelSvc', 'DetectorTools', expected_property_value)


    def test___AGDDtoGeoSvc_Builders_setCorrectly(self):
        expected_property_value = ['MuonAGDDTool/MuonSpectrometer']
        self._assert_Algorithm_property_ordered_equal('AGDDtoGeoSvc', 'Builders', expected_property_value)


if __name__ == '__main__':
    dirname = 'cosmics_config_test'
    os.system ('rm -rf ' + dirname)
    os.mkdir (dirname)
    os.chdir (dirname)
    unittest.main()
