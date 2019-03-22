#!/usr/bin/env python

import pickle
import subprocess
import unittest


class TestFullG4(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        config_picklefilename = 'FullG4_config.pkl'
        command = [
            'Sim_tf.py',
            '--simulator', 'FullG4',
            '--athenaopts', '"--config-only={}"'.format(config_picklefilename),
            '--conditionsTag', 'default:OFLCOND-RUN12-SDR-19',
            '--geometryVersion', 'default:ATLAS-R2-2015-03-01-00_VALIDATION',
            '--inputEVNTFile', '/afs/cern.ch/atlas/groups/Simulation/EVNT_files/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1',
            '--outputHITSFile', 'Hits.pool.root',
            '--maxEvents', '2',
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


    def test___SimKernel_is_at_end_of_AthAlgSeq(self):
        ath_alg_sequence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ignore_Algs = ['EventInfoTagBuilder/EventInfoTagBuilder']
        ath_alg_sequence_list = [ alg for alg in eval(ath_alg_sequence_as_str) if alg not in ignore_Algs ]

        actual_last_ath_alg_sequence_entry = ath_alg_sequence_list[-1]
        expected_last_ath_alg_sequence_entry = "ISF::SimKernel/ISF_Kernel_FullG4"
        self.assertEqual(expected_last_ath_alg_sequence_entry,
                         actual_last_ath_alg_sequence_entry)


    def test___BeamEffectsAlg_is_before_SimKernel_in_AthAlgSeq(self):
        ath_alg_sequence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ignore_Algs = ['EventInfoTagBuilder/EventInfoTagBuilder']
        ath_alg_sequence_list = [ alg for alg in eval(ath_alg_sequence_as_str) if alg not in ignore_Algs ]
        actual_last_ath_alg_sequence_entry = ath_alg_sequence_list[1] #Position 0 should be the timer alg
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
        expected_ISFRun = True
        self._assert_Algorithm_property_equal('BeamEffectsAlg', 'ISFRun', expected_ISFRun)


    def test___BeamEffectsAlg_GenEventManipulators_setCorrectly(self):
        expected_property_value = ['Simulation::GenEventValidityChecker/GenEventValidityChecker','Simulation::GenEventVertexPositioner/GenEventVertexPositioner']
        self._assert_Algorithm_property_ordered_equal('BeamEffectsAlg', 'GenEventManipulators', expected_property_value)


    def test___Geo2G4Svc_GetTopTransform_setCorrectly(self):
        expected_property_value = True
        self._assert_Algorithm_property_equal('Geo2G4Svc', 'GetTopTransform', expected_property_value)


    def test___SimKernel_ListOfSetProperties(self):
        expected_list = ['BeamPipeSimulationSelectors', 'CaloSimulationSelectors', 'CavernSimulationSelectors', 'DetStore', 'DoCPUMonitoring', 'DoMemoryMonitoring', 'EventFilterTools', 'EvtStore', 'ExtraInputs', 'ExtraOutputs', 'IDSimulationSelectors', 'InputConverter', 'InputHardScatterCollection', 'MSSimulationSelectors', 'MemoryMonitoringTool', 'NeededResources', 'OutputHardScatterTruthCollection', 'ParticleBroker', 'QuasiStablePatcher', 'TruthRecordService']
        simkernel = self._job_config_dict['ISF_Kernel_FullG4']
        actual_list = simkernel.keys()
        expected_property_value_sorted = sorted(expected_list)
        actual_property_value_sorted = sorted(actual_list)

        failure_message = "simkernel.keys() has a different " \
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


    def test___SimKernel_TruthRecordService_setCorrectly(self):
        expected_service_name = 'ISF_MC12TruthService'
        self._assert_Algorithm_property_equal('ISF_Kernel_FullG4', 'TruthRecordService', expected_service_name)


    def test___SimKernel_InputConverter_setCorrectly(self):
        expected_service_name = 'ISF_InputConverter'
        self._assert_Algorithm_property_equal('ISF_Kernel_FullG4', 'InputConverter', expected_service_name)


    def test___SimKernel_OutputHardScatterTruthCollection_setCorrectly(self):
        expected_output_truth = 'TruthEvent'
        self._assert_Algorithm_property_equal('ISF_Kernel_FullG4', 'OutputHardScatterTruthCollection', expected_output_truth)


    def test___SimKernel_InputHardScatterCollection_setCorrectly(self):
        expected_output_truth = 'BeamTruthEvent'
        self._assert_Algorithm_property_equal('ISF_Kernel_FullG4', 'InputHardScatterCollection', expected_output_truth)


    def test___SimKernel_BeamPipeSimulationSelectors_setCorrectly(self):
        expected_property_value = ['ISF::DefaultSimSelector/ISF_FullGeant4Selector']
        self._assert_Algorithm_property_ordered_equal('ISF_Kernel_FullG4', 'BeamPipeSimulationSelectors', expected_property_value)


    def test___SimKernel_IDSimulationSelectors_setCorrectly(self):
        expected_property_value = ['ISF::DefaultSimSelector/ISF_FullGeant4Selector']
        self._assert_Algorithm_property_ordered_equal('ISF_Kernel_FullG4', 'IDSimulationSelectors', expected_property_value)


    def test___SimKernel_CaloSimulationSelectors_setCorrectly(self):
        expected_property_value = ['ISF::DefaultSimSelector/ISF_FullGeant4Selector']
        self._assert_Algorithm_property_ordered_equal('ISF_Kernel_FullG4', 'CaloSimulationSelectors', expected_property_value)


    def test___SimKernel_MSSimulationSelectors_setCorrectly(self):
        expected_property_value = ['ISF::DefaultSimSelector/ISF_FullGeant4Selector']
        self._assert_Algorithm_property_ordered_equal('ISF_Kernel_FullG4', 'MSSimulationSelectors', expected_property_value)


    def test___SimKernel_CavernSimulationSelectors_setCorrectly(self):
        expected_property_value = ['ISF::DefaultSimSelector/ISF_DefaultParticleKillerSelector']
        self._assert_Algorithm_property_ordered_equal('ISF_Kernel_FullG4', 'CavernSimulationSelectors', expected_property_value)


    def test___GeoModelSvc_DetectorTools_setCorrectly(self):
        expected_property_value = ['PixelDetectorTool/PixelDetectorTool', 'SCT_DetectorTool/SCT_DetectorTool', 'TRT_DetectorTool/TRT_DetectorTool', 'InDetServMatTool/InDetServMatTool', 'LArDetectorToolNV/LArDetectorToolNV', 'TileDetectorTool/TileDetectorTool', 'MuonDetectorTool/MuonDetectorTool', 'BeamPipeDetectorTool/BeamPipeDetectorTool']
        self._assert_Algorithm_property_ordered_equal('GeoModelSvc', 'DetectorTools', expected_property_value)


    def test___AGDDtoGeoSvc_Builders_setCorrectly(self):
        expected_property_value = ['MuonAGDDTool/MuonSpectrometer']
        self._assert_Algorithm_property_ordered_equal('AGDDtoGeoSvc', 'Builders', expected_property_value)


if __name__ == '__main__':
    unittest.main()
