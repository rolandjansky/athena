#!/usr/bin/env python

import pickle
import subprocess
import unittest


class TestAtlasG4(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        config_picklefilename = 'AtlasG4_config.pkl'
        command = [
            'AtlasG4_tf.py',
            '--athenaopts', '"--config-only={}"'.format(config_picklefilename),
            '--conditionsTag', 'OFLCOND-MC16-SDR-14',
            '--physicsList', 'FTFP_BERT_ATL',
            '--maxEvents', '2',
            '--postInclude', 'PyJobTransforms/UseFrontier.py',
            '--geometryVersion', 'ATLAS-R2-2016-01-00-01',
            '--DataRunNumber', '284500',
            '--inputEVNTFile', '/afs/cern.ch/atlas/groups/Simulation/EVNT_files/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1',
            '--outputHITSFile', 'Hits.atlasg4.pool.root',
            # would otherwise fail due to missing HITS file:
            '--outputFileValidation', 'False',
        ]
        subprocess.check_call(command)

        with open(config_picklefilename) as picklefile:
            job_config_dict = pickle.load(picklefile)

        cls._job_config_dict = job_config_dict

    def _assert_G4AtlasAlg_property_unordered_equal(self,
                                                    property_name,
                                                    expected_property_value):
        g4atlasalg = self._job_config_dict['G4AtlasAlg']
        actual_property_value_as_str = g4atlasalg[property_name]
        # need to evaluate to obtain actual Python object
        actual_property_value = eval(actual_property_value_as_str)
        expected_property_value_sorted = sorted(expected_property_value)
        actual_property_value_sorted = sorted(actual_property_value)

        failure_message = "G4AtlasAlg.{property} has a different " \
                          "value than expected!\n" \
                          "expected (sorted):\n" \
                          " {expected}\n" \
                          "actual (sorted):\n" \
                          " {actual}".format(
                              property=property_name,
                              expected=expected_property_value_sorted,
                              actual=actual_property_value_sorted)
        self.assertEqual(expected_property_value_sorted,
                         actual_property_value_sorted,
                         failure_message)


    def _assert_G4AtlasAlg_property(self,
                                        property_name,
                                        expected_property_value):
        g4atlasalg = self._job_config_dict['G4AtlasAlg']
        actual_property_value = g4atlasalg[property_name]

        failure_message = "G4AtlasAlg.{property} has a different " \
                          "value than expected!\n" \
                          "expected:\n" \
                          " {expected}\n" \
                          "actual:\n" \
                          " {actual}".format(
                              property=property_name,
                              expected=str(expected_property_value),
                              actual=str(actual_property_value))
        self.assertEqual(str(expected_property_value),
                         str(actual_property_value),
                         failure_message)


    def test___G4AtlasAlg_is_at_end_of_AthAlgSeq(self):
        ath_alg_seqence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ath_alg_seqence_list = eval(ath_alg_seqence_as_str)

        actual_last_ath_alg_sequence_entry = ath_alg_seqence_list[-1]
        expected_last_ath_alg_sequence_entry = "G4AtlasAlg/G4AtlasAlg"
        self.assertEqual(expected_last_ath_alg_sequence_entry,
                         actual_last_ath_alg_sequence_entry)


    def test___G4AtlasAlg_ListOfSetProperties(self):
        expected_list = ['AtRndmGenSvc', 'DetGeoSvc', 'DetStore', 'EvtStore', 'ExtraInputs', 'ExtraOutputs', 'FastSimMasterTool', 'FlagAbortedEvents', 'G4AtlasSvc', 'G4Commands', 'GeoIDSvc', 'InputConverter', 'InputTruthCollection', 'KillAbortedEvents', 'MultiThreading', 'NeededResources', 'OutputTruthCollection', 'PhysicsListTool', 'RandomGenerator', 'RecordFlux', 'ReleaseGeoModel', 'SenDetMasterTool', 'TruthRecordService', 'UserActionSvc', 'UserStore', 'Verbosities']
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
        self._assert_G4AtlasAlg_property('InputTruthCollection', expected_input_truth)


    def test___G4AtlasAlg_OutputTruthCollection_setCorrectly(self):
        expected_output_truth = 'TruthEvent'
        self._assert_G4AtlasAlg_property('OutputTruthCollection', expected_output_truth)


    def test___G4AtlasAlg_FastSimMasterTool_setCorrectly(self):
        expected_tool_name = 'FastSimulationMasterTool/FastSimulationMasterTool'
        self._assert_G4AtlasAlg_property('FastSimMasterTool', expected_tool_name)


    def test___G4AtlasAlg_PhysicsListTool_setCorrectly(self):
        expected_tool_name = 'PhysicsListToolBase/PhysicsListToolBase'
        self._assert_G4AtlasAlg_property('PhysicsListTool', expected_tool_name)


    def test___G4AtlasAlg_SenDetMasterTool_setCorrectly(self):
        expected_tool_name = 'SensitiveDetectorMasterTool/SensitiveDetectorMasterTool'
        self._assert_G4AtlasAlg_property('SenDetMasterTool', expected_tool_name)


    def test___G4AtlasAlg_G4AtlasSvc_setCorrectly(self):
        expected_service_name = 'G4AtlasSvc'
        self._assert_G4AtlasAlg_property('G4AtlasSvc', expected_service_name)


    def test___G4AtlasAlg_UserActionSvc_setCorrectly(self):
        expected_service_name = 'G4UA::UserActionSvc'
        self._assert_G4AtlasAlg_property('UserActionSvc', expected_service_name)


    def test___G4AtlasAlg_DetGeoSvc_setCorrectly(self):
        expected_service_name = 'DetectorGeometrySvc'
        self._assert_G4AtlasAlg_property('DetGeoSvc', expected_service_name)


    def test___G4AtlasAlg_GeoIDSvc_setCorrectly(self):
        expected_service_name = 'ISF_GeoIDSvc'
        self._assert_G4AtlasAlg_property('GeoIDSvc', expected_service_name)


    def test___G4AtlasAlg_AtRndmGenSvc_setCorrectly(self):
        expected_service_name = 'AtDSFMTGenSvc'
        self._assert_G4AtlasAlg_property('AtRndmGenSvc', expected_service_name)


    def test___G4AtlasAlg_TruthRecordService_setCorrectly(self):
        expected_service_name = 'ISF_MC12TruthService'
        self._assert_G4AtlasAlg_property('TruthRecordService', expected_service_name)


    def test___G4AtlasAlg_InputConverter_setCorrectly(self):
        expected_service_name = 'ISF_InputConverter'
        self._assert_G4AtlasAlg_property('InputConverter', expected_service_name)


    def test___G4AtlasAlg_RandomGenerator_setCorrectly(self):
        expected_service_name = 'athena'
        self._assert_G4AtlasAlg_property('RandomGenerator', expected_service_name)


    def test___G4AtlasAlg_G4Commands_setCorrectly(self):
        expected_property_value = [] #TODO this will probably change as the configuration is migrated
        self._assert_G4AtlasAlg_property_unordered_equal('G4Commands', expected_property_value)


    def test___G4AtlasAlg_Verbosities_setCorrectly(self):
        expected_property_value = {} #TODO this will probably change as the configuration is migrated
        self._assert_G4AtlasAlg_property_unordered_equal('Verbosities', expected_property_value)


    def test___G4AtlasAlg_ReleaseGeoModel_setCorrectly(self):
        expected_property_value = True
        self._assert_G4AtlasAlg_property('ReleaseGeoModel', expected_property_value)


    def test___G4AtlasAlg_KillAbortedEvents_setCorrectly(self):
        expected_property_value = True
        self._assert_G4AtlasAlg_property('KillAbortedEvents', expected_property_value)


    def test___G4AtlasAlg_FlagAbortedEvents_setCorrectly(self):
        expected_property_value = False
        self._assert_G4AtlasAlg_property('FlagAbortedEvents', expected_property_value)


    def test___G4AtlasAlg_MultiThreading_setCorrectly(self):
        expected_property_value = False
        self._assert_G4AtlasAlg_property('MultiThreading', expected_property_value)


    def test___G4AtlasAlg_RecordFlux_setCorrectly(self):
        expected_property_value = False
        self._assert_G4AtlasAlg_property('RecordFlux', expected_property_value)


if __name__ == '__main__':
    unittest.main()
