#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
import myunittest
from PyJobTransformsCore.TransformConfig import TransformConfig, TransformConfigError, ListOfStrings

sys.tracebacklimit = 0


class TransformConfigKeywords(TransformConfig):
    __slots__ = ()
    keywords = ListOfStrings("Search keywords")

    def __init__(self, name=None):
        super(TransformConfigKeywords, self).__init__(name)

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)


# This test case mimics a set of use cases to validate the base class behavior of "UniqueList" class.
# Test methods are named in the format testPrecondition_ExpectedResults
class UniqueListTestCase(myunittest.TestCase):
    def setUp(self):
        self.dummyConfig = TransformConfigKeywords('DummyTransformConfig')

    def testSetEmptyList_ReturnsEmptyList(self):
        self.dummyConfig.keywords = []
        self.assertListEqual(self.dummyConfig.keywords, [])

    def testSetOneUniqueValue_ReturnsListWithSameValue(self):
        self.dummyConfig.keywords = ["QCD"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD"])

    def testSetOneRepeatedValue_ReturnsListWithOneValue(self):
        self.dummyConfig.keywords = ["QCD", "QCD", "QCD"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD"])

    def testSetMultipleUniqueValues_ReturnsSameValues_KeepsSequence(self):
        self.dummyConfig.keywords = ["QCD", "minBias", "SM"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD", "minBias", "SM"])

    def testSetMultipleRepeatedValues_ReturnsUniqueValues_KeepsSequence(self):
        self.dummyConfig.keywords = ["QCD", "minBias", "minBias", "SM", "SM"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD", "minBias", "SM"])


# This class mimics a transform configuration with restricted keywords.
class TransformConfigAllowedKeywords(TransformConfig):
    __slots__ = ()
    keywords = ListOfStrings("Search keywords", allowedValues=["QCD", "minBias", "SM"])

    def __init__(self, name=None):
        super(TransformConfigAllowedKeywords, self).__init__(name)

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)


# This test case mimics a set of use cases to validate the "Allowed Values" feature of ListOfStrings class.
# Test methods are named in the format testPrecondition_ExpectedResults
class AllowedValuesTestCase(myunittest.TestCase):
    def setUp(self):
        self.dummyConfig = TransformConfigAllowedKeywords('DummyTransformConfig')

    def testSetEmptyList_ReturnsEmptyList(self):
        self.dummyConfig.keywords = []
        self.assertListEqual(self.dummyConfig.keywords, [])

    def testSetOneAllowedValue_ReturnsListWithSameValue(self):
        self.dummyConfig.keywords = ["QCD"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD"])

    def testSetOneAllowedRepeatedValue_ReturnsListWithSingleValue(self):
        self.dummyConfig.keywords = ["QCD", "QCD", "QCD"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD"])

    def testSetMultipleAllowedAndUniqueValues_ReturnsSameValues_KeepsSequence(self):
        self.dummyConfig.keywords = ["QCD", "minBias", "SM"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD", "minBias", "SM"])

    def testSetMultipleAllowedAndRepeatedValues_ReturnsUniqueValues_KeepsSequence(self):
        self.dummyConfig.keywords = ["QCD", "minBias", "minBias", "SM", "SM"]
        self.assertListEqual(self.dummyConfig.keywords, ["QCD", "minBias", "SM"])

    def testSetOneValueNotAllowed_ThrowsTransformConfigError(self):
        with self.assertRaises(TransformConfigError):
            self.dummyConfig.keywords = ["InvalidKeyword"]

    def testSetOneValueNotAllowedAndRepeated_ThrowsTransformConfigError(self):
        with self.assertRaises(TransformConfigError):
            self.dummyConfig.keywords = ["InvalidKeyword", "InvalidKeyword"]

    def testSetMultipleValuesNotAllowedAndUnique_ThrowsTransformConfigError(self):
        with self.assertRaises(TransformConfigError):
            self.dummyConfig.keywords = ["InvalidKeyword1", "InvalidKeyword2"]

    def testSetMultipleValuesNotAllowedAndRepeated_ThrowsTransformConfigError(self):
        with self.assertRaises(TransformConfigError):
            self.dummyConfig.keywords = ["InvalidKeyword1", "InvalidKeyword1", "InvalidKeyword2", "InvalidKeyword2"]

if __name__ == "__main__":
    myunittest.main()
