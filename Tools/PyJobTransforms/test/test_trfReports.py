#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @Package test_trfReports.py
#  @brief Unittests for trfReports.py
#  @author graeme.andrew.stewart@cern.ch

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfReports import *

## Unittests for this module
class trfJobReportUnitTests(unittest.TestCase):

    def setUp(self):
        self.report = trfJobReport(parentTrf = None)

    def tearDown(self):
        pass

## Unittests for this module
class trfFileReportUnitTests(unittest.TestCase):

    def setUp(self):
        # In python 2.7 support for multiple 'with' expressions becomes available
        with open('file1', 'w') as f1:
            print('This is test file 1', file=f1)
        with open('file2', 'w') as f2:
            print('Short file 2', file=f2)
        with open('file3', 'w') as f3:
            print(80 * '-', 'Long file 3', 80 * '-', file=f3)

        # For test reports, use manually set GUIDs
        self.mySingleFile = trfArgClasses.argFile(['file1'], name = 'inputTEST_SINGLEFile', io = 'input', guid = {'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B'})
        self.mySingleFileReport = trfFileReport(self.mySingleFile)

        self.myMultiFile = trfArgClasses.argFile(['file1', 'file2', 'file3'], name = 'inputTEST_MULTIFile', io = 'input', guid = {'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'file2': '1368D295-27C6-4A92-8187-704C2A6A5864', 'file3': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9'})
        self.myMultiFileReport = trfFileReport(self.myMultiFile)

    def tearDown(self):
        for f in 'file1', 'file2', 'file3':
            try:
                os.unlink(f)
            except OSError:
                pass

    def test_fileReportPython(self):
        self.assertEqual(self.mySingleFileReport.singleFilePython(filename = 'file1'), {'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1', 'file_size': 20})
        self.assertEqual(self.mySingleFileReport.python(), {'argName': 'inputTEST_SINGLEFile', 'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1', 'file_size': 20}], 'type': None, 'dataset': None})
        self.assertEqual(self.mySingleFileReport.python(type = 'name'), {'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1'}], 'nentries': 'UNDEFINED', 'dataset': None})

    def test_fileReportPythonUnknown(self):
        self.assertRaises(trfExceptions.TransformReportException, self.mySingleFileReport.singleFilePython, filename = 'xxx')

    def test_multiFileReportPython(self):
        self.assertEqual(self.myMultiFileReport.singleFilePython(filename = 'file1'), {'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1', 'file_size': 20})
        self.assertEqual(self.myMultiFileReport.singleFilePython(filename = 'file2'), {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864', 'name': 'file2', 'file_size': 13})
        self.assertEqual(self.myMultiFileReport.singleFilePython(filename = 'file3'), {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9', 'name': 'file3', 'file_size': 174})
        self.assertEqual(self.myMultiFileReport.python(type = 'full'), {'argName': 'inputTEST_MULTIFile', 'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1', 'file_size': 20}, {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864', 'name': 'file2', 'file_size': 13}, {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9', 'name': 'file3', 'file_size': 174}], 'type': None, 'dataset': None})
        self.assertEqual(self.myMultiFileReport.python(type = 'name'), {'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1'}, {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864', 'name': 'file2'}, {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9', 'name': 'file3'}], 'nentries': 'UNDEFINED', 'dataset': None})


class machineReportUnitTests(unittest.TestCase):
    # Following tests don't test content, just that the generation of the objects happens correctly
    def test_machineReportPython(self):
        self.assertTrue(isinstance(machineReport().python(), dict))


class trfExeReportUnitTests(unittest.TestCase):
    def test_exeReportPython(self):
        try:
            from PyJobTransforms.trfExe import transformExecutor
        except ImportError:
            from trfExe import transformExecutor
        exeReport = trfExecutorReport(transformExecutor(name = 'unittest dummy'))
        self.assertTrue(isinstance(exeReport.python(), dict))


class trfReportWriterUnitTests(unittest.TestCase):
    def tearDown(self):
        try:
            os.unlink('testReport')
        except OSError:
            pass

    # These tests are minimal - just check the methods don't raise exceptions
    def test_textReportGenerator(self):
        self.assertTrue(trfReport().writeTxtReport(filename = 'testReport') == None)

    def test_GPickleReportGenerator(self):
        self.assertTrue(trfReport().writeGPickleReport(filename = 'testReport') == None)

    def test_ClassicXMLReportGenerator(self):
        self.assertTrue(trfReport().writeClassicXMLReport(filename = 'testReport') == None)

    def test_JSONReportGenerator(self):
        self.assertTrue(trfReport().writeJSONReport(filename = 'testReport') == None)



class trfClassicXMLReportTest(unittest.TestCase):

    def setUp(self):
        # Initialise a fake transform
        pass


if __name__ == '__main__':
    unittest.main()
