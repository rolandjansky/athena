#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
import myunittest
from PyJobTransformsCore.TransformConfig import TransformConfig, ListOfStrings

sys.tracebacklimit = 0


class DerivedTransformConfig(TransformConfig):
    __slots__ = ()
    keywords = ListOfStrings("Search keywords")

    def __init__(self, name=None):
        super(DerivedTransformConfig, self).__init__(name)

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)


# This test case mimics a set of use cases to validate the base class behavior of "UniqueList" class.
# Test methods are named in the format testPrecondition_ExpectedResults
class UniqueListTestCase(myunittest.TestCase):
    def setUp(self):
        self.dummyConfig = DerivedTransformConfig('DummyTransformConfig')

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


# This test case mimics a set of use cases to validate the "Allowed Values" feature of ListOfStrings class.
# Test methods are named in the format testPrecondition_ExpectedResults
class AllowedValuesTestCase(myunittest.TestCase):
    def setUp(self):
        self.dummyTrfConfig = DerivedTransformConfig('DummyTransformConfig')


if __name__ == "__main__":
    myunittest.main()
