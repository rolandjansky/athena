# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTTestCase import RTTTestCase
import unittest
import os

class TestCase(RTTTestCase):

    def testXMLComp(self): self.rttTestXMLComp()

    def testKeepfileDiff(self): self.rttKeepfileDiff()

    def variableContentTags(self):
        vcTags = RTTTestCase.variableContentTags(self)
        vcTags.append('errors')
        return vcTags
    
    def test1ErrorTag(self): self.rttTestNTags(self.dom, 'errors', 1)

if __name__ == '__main__':

    from runTestCase import runTestCase
    runTestCase(TestCase)
