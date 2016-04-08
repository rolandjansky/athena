# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTTestCase import RTTTestCase
import unittest
import os

class TestCase(RTTTestCase):

    def testXMLComp(self):            self.rttTestXMLComp()

    def testKeepfileDiff(self): self.rttKeepfileDiff()
    
    def variableContentTags(self):
        """
        Add the hashstring tag to the variable content
        tag list. This job takes files from the
        chain store, so the content of the file tag depends on
        the (variable) name of the chain store.
        """

        tags = RTTTestCase.variableContentTags(self)
        tags.extend(['directory', 'mtime'])
        # the size of the output file changes by a few bytes from
        # run to run. Why?
        tags.append('size')
        return tags


if __name__ == '__main__':
    from runTestCase import runTestCase
    runTestCase(TestCase)
