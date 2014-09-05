#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfReports.py
#  @brief Unittests for trfReports.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfReports.py 610120 2014-08-04 13:34:16Z volkmer $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfReports import *

## Unittests for this module
class trfJobReportUnitTests(unittest.TestCase):

    def setUp(self):
        self.report = trfJobReport(parentTrf = None)
        self.__compressionInput = {'input': [{'dataset': None,
                                              'nentries': 1541,
                                              'subFiles': [{'file_guid': '1CE618EF-AF8A-E111-A3C0-003048D37440',
                                                            'name': 'data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1'}],
                                              'type': 'BS'}],
                                   'output': [{'argName': 'outputAODFile',
                                               'dataset': None,
                                               'subFiles': [{'AODFixVersion': '',
                                                             'beam_energy': [4000000.0],
                                                             'beam_type': ['collisions'],
                                                             'conditions_tag': 'COMCOND-BLKPA-006-01',
                                                             'file_guid': 'C9D861B6-9180-2642-9914-879FC1EA44DF',
                                                             'file_size': 3461628L,
                                                             'file_type': 'pool',
                                                             'geometry': 'ATLAS-GEO-20-00-01',
                                                             'integrity': True,
                                                             'lumi_block': [423L],
                                                             'name': 'my.AOD.pool.root',
                                                             'nentries': 10L,
                                                             'run_number': [201556L]}],
                                               'type': 'AOD'},
                                              {'argName': 'outputESDFile',
                                               'dataset': None,
                                               'subFiles': [{'AODFixVersion': '',
                                                             'beam_energy': [4000000.0],
                                                             'beam_type': ['collisions'],
                                                             'conditions_tag': 'COMCOND-BLKPA-006-01',
                                                             'file_guid': '4CD89E10-42DD-E34C-BF1F-545CEE19E4FD',
                                                             'file_size': 26391289L,
                                                             'file_type': 'pool',
                                                             'geometry': 'ATLAS-GEO-20-00-01',
                                                             'integrity': True,
                                                             'lumi_block': [423L],
                                                             'name': 'my.ESD.pool.root',
                                                             'nentries': 10L,
                                                             'run_number': [201556L]}],
                                               'type': 'ESD'}]}
        self.__compressionOutput = {'input': [{'dataset': None,
                                                'nentries': 1541,
                                                'subFiles': [{'file_guid': '1CE618EF-AF8A-E111-A3C0-003048D37440',
                                                              'name': 'data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1'}],
                                                'type': 'BS'}],
                                     'output': {'common': {'AODFixVersion': '',
                                                           'beam_energy': [4000000.0],
                                                           'beam_type': ['collisions'],
                                                           'conditions_tag': 'COMCOND-BLKPA-006-01',
                                                           'dataset': None,
                                                           'file_type': 'pool',
                                                           'geometry': 'ATLAS-GEO-20-00-01',
                                                           'integrity': True,
                                                           'lumi_block': [423L],
                                                           'nentries': 10L,
                                                           'run_number': [201556L]},
                                                'files': [{'argName': 'outputAODFile',
                                                           'subFiles': [{'file_guid': 'C9D861B6-9180-2642-9914-879FC1EA44DF',
                                                                         'file_size': 3461628L,
                                                                         'name': 'my.AOD.pool.root'}],
                                                           'type': 'AOD'},
                                                          {'argName': 'outputESDFile',
                                                           'subFiles': [{'file_guid': '4CD89E10-42DD-E34C-BF1F-545CEE19E4FD',
                                                                         'file_size': 26391289L,
                                                                         'name': 'my.ESD.pool.root'}],
                                                           'type': 'ESD'}]}}

    def tearDown(self):
        pass

    def test_compressedFileReport(self):
        compressed = self.report._compressFileDict(self.__compressionInput)
        self.assertEqual(compressed, self.__compressionOutput)


    def test_compressedFileReportOneMissing(self):
        del self.__compressionInput['output'][0]['subFiles'][0]['nentries']
        del self.__compressionOutput['output']['common']['nentries']
        self.__compressionOutput['output']['files'][1]['subFiles'][0]['nentries'] = 10L

        compressed = self.report._compressFileDict(self.__compressionInput)
        self.assertEqual(compressed, self.__compressionOutput)

    def test_compressedFileReportTypeDiff(self):
        self.__compressionInput['output'][0]['subFiles'][0]['nentries'] = 10.0
        del self.__compressionOutput['output']['common']['nentries']
        self.__compressionOutput['output']['files'][0]['subFiles'][0]['nentries'] = 10.0
        self.__compressionOutput['output']['files'][1]['subFiles'][0]['nentries'] = 10L

        compressed = self.report._compressFileDict(self.__compressionInput)
        self.maxDiff = None
        self.assertEqual(compressed, self.__compressionOutput)

    def test_compressedFileReportNoCompression(self):
        del self.__compressionInput['output'][0]['subFiles'][0]['AODFixVersion']
        del self.__compressionInput['output'][0]['subFiles'][0]['beam_energy']
        del self.__compressionInput['output'][0]['subFiles'][0]['beam_type']
        del self.__compressionInput['output'][0]['subFiles'][0]['conditions_tag']
        del self.__compressionInput['output'][0]['subFiles'][0]['file_type']
        del self.__compressionInput['output'][0]['subFiles'][0]['integrity']
        del self.__compressionInput['output'][0]['subFiles'][0]['lumi_block']
        del self.__compressionInput['output'][0]['subFiles'][0]['run_number']
        del self.__compressionInput['output'][0]['subFiles'][0]['geometry']
        del self.__compressionInput['output'][0]['subFiles'][0]['nentries']
        del self.__compressionInput['output'][0]['dataset']

        compressed = self.report._compressFileDict(self.__compressionInput)
        self.assertEqual(compressed, self.__compressionInput)


    def test_compressedFileReportSubFileCompression(self):
        subFilesCompressed = {'argName': 'inputTEST_MULTIFileCompress',
                             'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B',
                                           'file_size': 20,
                                           'name': 'file1',
                                           'niceness': 1234567},
                                          {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864',
                                           'file_size': 13,
                                           'name': 'file2',
                                           'niceness': 1234567},
                                          {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9',
                                           'file_size': 174,
                                           'name': 'file3'}],
                             'type': None}
        self.__compressionInput['input'].append(subFilesCompressed)

        compressed = self.report._compressFileDict(self.__compressionInput)
        self.assertEqual(compressed['output'], self.__compressionOutput['output'])
        self.assertEqual(compressed['input'], self.__compressionInput['input'])

    def test_compressedFileReportSubFileCompression2(self):
        subFilesCompressedInput = {'argName': 'inputTEST_MULTIFileCompress',
                                   'common': {'generator': 'Pythia',
                                              'integrity': True
                                              },
                                   'dataset': None,
                                   'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B',
                                                 'file_size': 20,
                                                 'name': 'file1',
                                                 'niceness': 1234567},
                                                {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864',
                                                 'file_size': 13,
                                                 'name': 'file2',
                                                 'niceness': 1234567},
                                                {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9',
                                                 'file_size': 174,
                                                 'niceness': 1234567,
                                                 'name': 'file3'}],
                                   'type': None}
        subFilesCompressedInputCompressed = {'common': {'dataset': None,
                                                        'generator': 'Pythia'
                                                        },
                                             'files': [{'nentries': 1541,
                                                        'subFiles': [{'file_guid': '1CE618EF-AF8A-E111-A3C0-003048D37440',
                                                                      'name': 'data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1'}],
                                                        'type': 'BS'},
                                                       {'argName': 'inputTEST_MULTIFileCompress',
                                                        'common': {'integrity': True},
                                                        'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B',
                                                                      'file_size': 20,
                                                                      'name': 'file1',
                                                                      'niceness': 1234567},
                                                                     {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864',
                                                                      'file_size': 13,
                                                                      'name': 'file2',
                                                                      'niceness': 1234567},
                                                                     {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9',
                                                                      'file_size': 174,
                                                                      'name': 'file3',
                                                                      'niceness': 1234567}],
                                                        'type': None}]}

        self.__compressionInput['input'][0]['generator'] = 'Pythia'
        self.__compressionInput['input'].append(subFilesCompressedInput)
        self.__compressionOutput['input'] = subFilesCompressedInputCompressed

        compressed = self.report._compressFileDict(self.__compressionInput)
        self.assertEqual(compressed, self.__compressionOutput)


## Unittests for this module
class trfFileReportUnitTests(unittest.TestCase):

    def setUp(self):
        # In python 2.7 support for multiple 'with' expressions becomes available
        with open('file1', 'w') as f1:
            print >> f1, 'This is test file 1'
        with open('file2', 'w') as f2:
            print >> f2, 'Short file 2'
        with open('file3', 'w') as f3:
            print >> f3, 80 * '-', 'Long file 3', 80 * '-'

        # For test reports, use manually set GUIDs
        self.mySingleFile = trfArgClasses.argFile(['file1'], name = 'inputTEST_SINGLEFile', io = 'input', guid = {'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B'})
        self.mySingleFileReport = trfFileReport(self.mySingleFile)

        self.myMultiFile = trfArgClasses.argFile(['file1', 'file2', 'file3'], name = 'inputTEST_MULTIFile', io = 'input', guid = {'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'file2': '1368D295-27C6-4A92-8187-704C2A6A5864', 'file3': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9'})
        self.myMultiFileReport = trfFileReport(self.myMultiFile)

        self.myMultiFileCompress1 = trfArgClasses.argFile(['file1', 'file2', 'file3'], name = 'inputTEST_MULTIFileCompress', io = 'input', guid = {'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'file2': '1368D295-27C6-4A92-8187-704C2A6A5864', 'file3': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9'})
        self.myMultiFileCompress1._setMetadata(files = None, metadataKeys = {'niceness': 1234567, 'generator': 'Pythia'})
        self.myMultiFileCompressReport1 = trfFileReport(self.myMultiFileCompress1)

        self.myMultiFileCompress2 = trfArgClasses.argFile(['file1', 'file2', 'file3'], name = 'inputTEST_MULTIFileCompress', io = 'input', guid = {'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'file2': '1368D295-27C6-4A92-8187-704C2A6A5864', 'file3': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9'})
        self.myMultiFileCompress2._setMetadata(files = ['file1', 'file2'], metadataKeys = {'niceness': 1234567, 'generator': 'Pythia'})
        self.myMultiFileCompress2._setMetadata(files = ['file3'], metadataKeys = {'generator': 'Pythia'})
        self.myMultiFileCompressReport2 = trfFileReport(self.myMultiFileCompress2)

    def tearDown(self):
        for f in 'file1', 'file2', 'file3':
            try:
                os.unlink(f)
            except OSError:
                pass

    def test_fileReportPython(self):
        self.assertEqual(self.mySingleFileReport.singleFilePython(filename = 'file1'), {'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'integrity': True, 'name': 'file1', 'file_size': 20})
        self.assertEqual(self.mySingleFileReport.python(), {'argName': 'inputTEST_SINGLEFile', 'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'integrity': True, 'name': 'file1', 'file_size': 20}], 'type': None, 'dataset': None})
        self.assertEqual(self.mySingleFileReport.python(type = 'name'), {'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1'}], 'nentries': 'UNDEFINED', 'dataset': None})

    def test_fileReportPythonUnknown(self):
        self.assertRaises(trfExceptions.TransformReportException, self.mySingleFileReport.singleFilePython, filename = 'xxx')

    def test_multiFileReportPython(self):
        self.assertEqual(self.myMultiFileReport.singleFilePython(filename = 'file1'), {'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'integrity': True, 'name': 'file1', 'file_size': 20})
        self.assertEqual(self.myMultiFileReport.singleFilePython(filename = 'file2'), {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864', 'integrity': True, 'name': 'file2', 'file_size': 13})
        self.assertEqual(self.myMultiFileReport.singleFilePython(filename = 'file3'), {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9', 'integrity': True, 'name': 'file3', 'file_size': 174})
        self.assertEqual(self.myMultiFileReport.python(type = 'full'), {'argName': 'inputTEST_MULTIFile', 'common' : {'integrity' : True}, 'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1', 'file_size': 20}, {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864', 'name': 'file2', 'file_size': 13}, {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9', 'name': 'file3', 'file_size': 174}], 'type': None, 'dataset': None})
        self.assertEqual(self.myMultiFileReport.python(type = 'name'), {'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'name': 'file1'}, {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864', 'name': 'file2'}, {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9', 'name': 'file3'}], 'nentries': 'UNDEFINED', 'dataset': None})


    def test_CommonBlockCleanUp(self):
        compressedResult1 = {'argName': 'inputTEST_MULTIFileCompress',
                             'common': {'generator': 'Pythia', 'integrity': True, 'niceness': 1234567},
                             'dataset': None,
                             'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B',
                                           'file_size': 20,
                                           'name': 'file1'},
                                          {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864',
                                           'file_size': 13,
                                           'name': 'file2'},
                                          {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9',
                                           'file_size': 174,
                                           'name': 'file3'}],
                             'type': None}

        compressedResult2 = {'argName': 'inputTEST_MULTIFileCompress',
                             'common': {'generator': 'Pythia', 'integrity': True},
                             'dataset': None,
                             'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B',
                                           'file_size': 20,
                                           'name': 'file1',
                                           'niceness': 1234567},
                                          {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864',
                                           'file_size': 13,
                                           'name': 'file2',
                                           'niceness': 1234567},
                                          {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9',
                                           'file_size': 174,
                                           'name': 'file3'}],
                             'type': None}

        metaDataIgnored = {'dataset': None,
                           'nentries': 'UNDEFINED',
                           'subFiles': [{'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B',
                                         'name': 'file1'},
                                        {'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864',
                                         'name': 'file2'},
                                        {'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9',
                                         'name': 'file3'}]}

        self.assertEqual(self.myMultiFileCompressReport1.python(type = 'full'), compressedResult1)
        self.assertEqual(self.myMultiFileCompressReport2.python(type = 'full'), compressedResult2)
        self.assertEqual(self.myMultiFileCompressReport1.python(type = 'name'), metaDataIgnored)
        self.assertEqual(self.myMultiFileCompressReport2.python(type = 'name'), metaDataIgnored)


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
