#!/usr/bin/env python

import sys
import unittest

from AthenaCommon.CfgGetter import getAlgorithm
from G4AtlasApps.SimFlags import simFlags
from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name
from ISF_Config.ISF_jobProperties import ISF_Flags


class Test_generate_mergeable_collection_name(unittest.TestCase):

    def setUp(self):
        # keep same algorithm instance around for the duration of each test
        self.collection_merger = getAlgorithm('ISF_CollectionMerger')

    def tearDown(self):
        # get rid of current algorithm instance so that next test gets its own
        # instance
        del self.collection_merger

    def test_isISFRunAndHITSMergingRequired_expectBareNameWithSuffixReturned(self):
        simFlags.ISFRun.set_Value(True)
        mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        expected_collection_name = 'aTestCollection_TESTSUFFIX'
        self.assertEqual(expected_collection_name, actual_collection_name)

    def test_isISFRunAndHITSMergingRequiredWithEmptyCollectionMergerAlgorithm_expectCollectionAddedToCollectionMergerAlgorithm(self):
        simFlags.ISFRun.set_Value(True)
        mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        collection_merger = getAlgorithm('ISF_CollectionMerger')
        actual_collection_merger_input = collection_merger.InputPixelHits
        expected_collection_merger_input = ['aTestCollection_TESTSUFFIX']

        self.assertEqual(expected_collection_merger_input,
                         actual_collection_merger_input)

    def test_isISFRunAndNoHITSMergingRequired_expectBareCollectionNameReturned(self):
        simFlags.ISFRun.set_Value(True)
        mergeDict = {'ID':False, 'CALO':True, 'MUON':False}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        expected_collection_name = 'aTestCollection'
        self.assertEqual(expected_collection_name, actual_collection_name)

    def test_isISFRunAndNoHITSMergingRequiredWithEmptyCollectionMergerAlgorithm_expectCollectionMergerAlgorithmUntouched(self):
        simFlags.ISFRun.set_Value(True)
        mergeDict = {'ID':False, 'CALO':True, 'MUON':False}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        collection_merger = getAlgorithm('ISF_CollectionMerger')
        actual_collection_merger_input = collection_merger.InputPixelHits

        expected_collection_merger_input = list()
        self.assertEqual(expected_collection_merger_input,
                         actual_collection_merger_input)

    def test_isNotISFRunAndNoHITSMergingRequired_expectBareCollectioNameReturned(self):
        simFlags.ISFRun.set_Value(False)
        mergeDict = {'ID':False, 'CALO':True, 'MUON':False}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        expected_collection_name = 'aTestCollection'
        self.assertEqual(expected_collection_name, actual_collection_name)

    def test_isNotISFRunAndHITSMergingRequired_expectBareCollectioNameReturned(self):
        simFlags.ISFRun.set_Value(False)
        mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        expected_collection_name = 'aTestCollection'
        self.assertEqual(expected_collection_name, actual_collection_name)

    def test_isNotISFRunAndNoHITSMergingRequired_expectCollectionMergerAlgorithmUntouched(self):
        simFlags.ISFRun.set_Value(False)
        mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        collection_merger = getAlgorithm('ISF_CollectionMerger')
        actual_collection_merger_input = collection_merger.InputPixelHits

        expected_collection_merger_input = list()
        self.assertEqual(expected_collection_merger_input,
                         actual_collection_merger_input)

    def test_isNotISFRunAndHITSMergingRequired_expectCollectionMergerAlgorithmUntouched(self):
        simFlags.ISFRun.set_Value(False)
        mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
        ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)

        bare_collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        region = 'ID'
        actual_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property,
                                                                    region)

        collection_merger = getAlgorithm('ISF_CollectionMerger')
        actual_collection_merger_input = collection_merger.InputPixelHits

        expected_collection_merger_input = list()
        self.assertEqual(expected_collection_merger_input,
                         actual_collection_merger_input)


if __name__ == '__main__':
    unittest.main()
