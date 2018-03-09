#!/usr/bin/env python

import pickle
import subprocess
import unittest


class TestATLFASTII(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        config_picklefilename = 'ATLFASTII_config.pkl'
        command = [
            'Sim_tf.py',
            '--simulator', 'ATLFASTII',
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

    def _assert_ISF_CollectionMerger_property_unordered_equal(self,
                                                              property_name,
                                                              expected_property_value):
        collection_merger = self._job_config_dict['ISF_CollectionMerger']
        actual_property_value_as_str = collection_merger[property_name]
        # need to evaluate to obtain actual Python object
        actual_property_value = eval(actual_property_value_as_str)

        expected_property_value_sorted = sorted(expected_property_value)
        actual_property_value_sorted = sorted(actual_property_value)

        failure_message = "ISF_CollectionMerger.{property} has a different " \
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

    def test___ISF_Collection_merger_is_at_end_of_AthAlgSeq(self):
        ath_alg_seqence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ath_alg_seqence_list = eval(ath_alg_seqence_as_str)

        actual_last_ath_alg_sequence_entry = ath_alg_seqence_list[-1]
        expected_last_ath_alg_sequence_entry = "ISF::CollectionMerger/ISF_CollectionMerger"
        self.assertEqual(expected_last_ath_alg_sequence_entry,
                         actual_last_ath_alg_sequence_entry)

    def test___ISF_Collection_merger_InputBCMHits_contains_only_G4_collection(self):
        expected_input_hits = ['BCMHits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputBCMHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputBLMHits_contains_only_G4_collection(self):
        expected_input_hits = ['BLMHits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputBLMHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputCSCHits_contains_only_G4_collection(self):
        expected_input_hits = ['CSC_Hits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputCSCHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputLArEMBHits_contains_G4_and_FastCaloSim_collections(self):
        expected_input_hits = ['LArHitEMB_G4', 'LArHitEMB_FastCaloSim']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputLArEMBHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputLArEMECHits_contains_G4_and_FastCaloSim_collections(self):
        expected_input_hits = ['LArHitEMEC_G4', 'LArHitEMEC_FastCaloSim']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputLArEMECHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputLArFCALHits_contains_G4_and_FastCaloSim_collections(self):
        expected_input_hits = ['LArHitFCAL_G4', 'LArHitFCAL_FastCaloSim']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputLArFCALHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputLArHECHits_contains_G4_and_FastCaloSim_collections(self):
        expected_input_hits = ['LArHitHEC_G4', 'LArHitHEC_FastCaloSim']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputLArHECHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputMBTSHits_contains_only_G4_collection(self):
        expected_input_hits = ['MBTSHits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputMBTSHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputMDTHits_contains_only_G4_collection(self):
        expected_input_hits = ['MDT_Hits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputMDTHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputPixelHits_contains_only_G4_collection(self):
        expected_input_hits = ['PixelHits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputPixelHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputRPCHits_contains_only_G4_collection(self):
        expected_input_hits = ['RPC_Hits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputRPCHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputSCTHits_contains_only_G4_collection(self):
        expected_input_hits = ['SCT_Hits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputSCTHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputTGCHits_contains_only_G4_collection(self):
        expected_input_hits = ['TGC_Hits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputTGCHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputTRTUncompressedHits_contains_only_G4_collection(self):
        expected_input_hits = ['TRTUncompressedHits_G4']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputTRTUncompressedHits',
            expected_input_hits)

    def test___ISF_Collection_merger_InputTileHits_contains_G4_and_FastCaloSim_collections(self):
        expected_input_hits = ['TileHitVec_G4', 'TileHitVec_FastCaloSim']
        self._assert_ISF_CollectionMerger_property_unordered_equal(
            'InputTileHits',
            expected_input_hits)


if __name__ == '__main__':
    unittest.main()
