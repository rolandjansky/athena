# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTTestCase import RTTTestCase
import unittest
import os
class TestCase(RTTTestCase):

    def variableContentTags(self):
        """
        Add the hashstring tag to the variable content
        tag list. This job takes files from the
        chain store, so the content of the file tag depends on
        the (variable) name of the chain store.
        """

        tags = RTTTestCase.variableContentTags(self)
        tags.extend(['directory','hashString'])
        return tags
    
    def testCMLComp(self): self.rttTestXMLComp()
    def testNKeepFiles(self): self.rttTestNTags(self.dom, 'keepfile_newstyle', 16)
        
if __name__ == '__main__':
    from runTestCase import runTestCase
    runTestCase(TestCase)
