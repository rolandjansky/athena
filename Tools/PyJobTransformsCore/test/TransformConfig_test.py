#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
import myunittest
from PyJobTransformsCore.TransformConfig import TransformConfig, ListOfStrings
sys.tracebacklimit=0

class DerivedTransformConfig(TransformConfig):
    __slots__ = ()
    keywords = ListOfStrings("Search keywords for a dummy dataset")

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)


class DerivedTransformConfigTest(myunittest.TestCase):
    def testListOfStringsSetValues(self):
        dummyConfig = DerivedTransformConfig('DummyTransformConfig')
        dummyConfig.keywords = ["QCD", "minBias", "SM"]
        self.assertListEqual(dummyConfig.keywords, ["QCD", "minBias", "SM"])
        print(dummyConfig.keywords)


if __name__ == "__main__":
    myunittest.main()

