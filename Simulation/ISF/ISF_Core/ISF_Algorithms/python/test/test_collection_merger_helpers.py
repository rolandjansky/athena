#!/usr/bin/env python

from mock import Mock
import sys
import unittest

from AthenaCommon.CfgGetter import getAlgorithm
from G4AtlasApps.SimFlags import simFlags
from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


class Test_generate_mergeable_collection_name(unittest.TestCase):

    def setUp(self):
        # keep same algorithm instance around for the duration of each test
        self.collection_merger = getAlgorithm('ISF_CollectionMerger')

    def tearDown(self):
        # get rid of current algorithm instance so that next test gets its own
        # instance
        del self.collection_merger

    def test_isISFRun_expectNameAndSuffixInReturnedCollectionName(self):
        simFlags.ISFRun.set_Value(True)

        collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        actual_collection_name = generate_mergeable_collection_name(collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property)

        expected_collection_name = 'aTestCollection_TESTSUFFIX'
        self.assertEqual(expected_collection_name, actual_collection_name)

    def test_isISFRunWithEmptyCollectionMergerAlgorithm_expectCollectionAddedToCollectionMergerAlgorithm(self):
        simFlags.ISFRun.set_Value(True)

        collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        actual_collection_name = generate_mergeable_collection_name(collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property)

        collection_merger = getAlgorithm('ISF_CollectionMerger')
        actual_collection_merger_input = collection_merger.InputPixelHits
        expected_collection_merger_input = ['aTestCollection_TESTSUFFIX']

        print('collection_merger in test', collection_merger)
        self.assertEqual(expected_collection_merger_input,
                         actual_collection_merger_input)

    def test_isNotISFRun_expectOnlyNameInReturnedCollectionName(self):
        simFlags.ISFRun.set_Value(False)

        collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        actual_collection_name = generate_mergeable_collection_name(collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property)

        expected_collection_name = 'aTestCollection'
        self.assertEqual(expected_collection_name, actual_collection_name)

    def test_isNotISFRun_expectCollectionMergerAlgorithmUntouched(self):
        simFlags.ISFRun.set_Value(False)

        collection_name = 'aTestCollection'
        collection_suffix = '_TESTSUFFIX'
        merger_input_property = 'PixelHits'
        actual_collection_name = generate_mergeable_collection_name(collection_name,
                                                                    collection_suffix,
                                                                    merger_input_property)

        collection_merger = getAlgorithm('ISF_CollectionMerger')
        actual_collection_merger_input = collection_merger.InputPixelHits

        expected_collection_merger_input = list()
        self.assertEqual(expected_collection_merger_input,
                         actual_collection_merger_input)


if __name__ == '__main__':
    unittest.main()
