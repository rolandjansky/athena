#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import myunittest
from PyJobTransformsCore.TransformConfig import *

class TransfromConfigTest(myunittest.TestCase):
    def setUp(self):
        self.config = TransformConfig()

    def testAddAttribute(self):
        """add a member to the config"""
        self.config.addAttribute( String( "myString", "myStringDefault", "myStringDoc" ) )
        print ""
        print self.config
        self.config.myString = "myStringValue"
        print self.config


if __name__ == "__main__":
    myunittest.main()

