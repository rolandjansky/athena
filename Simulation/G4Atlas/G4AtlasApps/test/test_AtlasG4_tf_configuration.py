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
        collection_merger = self._job_config_dict['G4AtlasAlg']
        actual_property_value_as_str = collection_merger[property_name]
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


    def test___G4AtlasAlg_is_at_end_of_AthAlgSeq(self):
        ath_alg_seqence_as_str = self._job_config_dict['AthAlgSeq']['Members']
        # need to evaluate to obtain actual Python object
        ath_alg_seqence_list = eval(ath_alg_seqence_as_str)

        actual_last_ath_alg_sequence_entry = ath_alg_seqence_list[-1]
        expected_last_ath_alg_sequence_entry = "G4AtlasAlg/G4AtlasAlg"
        self.assertEqual(expected_last_ath_alg_sequence_entry,
                         actual_last_ath_alg_sequence_entry)


    ## def test___ISF_Collection_merger_InputBCMHits_contains_only_G4_collection(self):
    ##     expected_input_hits = ['BCMHits_G4']
    ##     self._assert_G4AtlasAlg_property_unordered_equal(
    ##         'InputBCMHits',
    ##         expected_input_hits)


if __name__ == '__main__':
    unittest.main()
