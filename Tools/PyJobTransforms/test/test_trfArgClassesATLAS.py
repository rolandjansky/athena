#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfArgClasses.py
#  @brief Unittests for test_trfArgClasses.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfArgClassesATLAS.py 731518 2016-03-22 07:29:46Z graemes $
#  @note Tests of ATLAS specific file formats (that thus rely on other
#  parts of Athena) live here

import sys
import unittest

from PyJobTransforms.trfLogger import msg

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfArgClasses import *

# Stripped down key list for files which are inputs 
from PyJobTransforms.trfFileUtils import athFileInterestingKeys

class argFileEOSTests(unittest.TestCase):
    def test_SimGlobStar(self):
        hitsInputs = argFile('/eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.*', io='input')
        self.assertEqual(len(hitsInputs.value), 4)        

    def test_SimGlobMatchSingle(self):
        hitsInputs = argFile('/eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._??????.pool.root.?', io='input')
        self.assertEqual(len(hitsInputs.value), 4)        

    def test_SimGlobMatchBoth(self):
        hitsInputs = argFile('/eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._*.pool.root.?', io='input')
        self.assertEqual(len(hitsInputs.value), 4)        


class argPOOLFiles(unittest.TestCase):
    def tearDown(self):
        for f in 'athfile-cache.ascii.gz', 'athfile-infos.ascii':
            try:
                os.unlink(f)
            except OSError:
                pass

    def test_argPOOLFileMetadata_ESD(self):
        try:
            testFile = '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.ESD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root'
            os.stat(testFile)
            esdFile = argPOOLFile(testFile, io = 'input', type='esd')
            self.assertEqual(esdFile.getMetadata(metadataKeys =  tuple(athFileInterestingKeys)), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.ESD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'file_type': 'pool', 'file_guid': '0CABA22E-9096-E011-AE25-0030487C8CE6', 'nentries': 10L, 'file_size': 17033381}})
            esdFile = argPOOLFile(testFile, io = 'output', type='esd')
            self.assertEqual(esdFile.getMetadata(), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.ESD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'_exists': True, 'file_type': 'pool', 'file_guid': '0CABA22E-9096-E011-AE25-0030487C8CE6', 'file_size': 17033381, 'integrity': True, 'nentries': 10L}}) 
            self.assertEqual(esdFile.getMetadata(metadataKeys = ('nentries',)), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.ESD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'nentries': 10}})
            self.assertEqual(esdFile.prodsysDescription['type'],'file')
        except OSError:
            # With python 2.7 this should call the self.skip() method
            print >>sys.stderr, 'WARNING Skipping test_argPOOLFileMetadata_ESD - stat on AFS test file failed'

    def test_argPOOLFileMetadata_AOD(self):
        try:
            testFile = '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.AOD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root'
            os.stat(testFile)
            aodFile = argPOOLFile(testFile, io = 'input', type='aod')
            self.assertEqual(aodFile.getMetadata(metadataKeys = tuple(athFileInterestingKeys)), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.AOD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'file_type': 'pool', 'file_guid': '6E1FE6F0-9096-E011-9DDA-0030487C8CE6', 'nentries': 10L, 'file_size': 4673269}})
            aodFile = argPOOLFile(testFile, io = 'output', type='aod')
            self.assertEqual(aodFile.getMetadata(),{'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.AOD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'_exists': True, 'file_type': 'pool', 'file_guid': '6E1FE6F0-9096-E011-9DDA-0030487C8CE6', 'file_size': 4673269, 'integrity': True, 'nentries': 10L}}) 
            self.assertEqual(aodFile.getMetadata(metadataKeys = ('nentries',)), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.AOD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'nentries': 10}}) 
            self.assertEqual(aodFile.prodsysDescription['type'],'file')
            self.assertTrue(aodFile.prodsysDescription['subtype']=='AOD')
        except OSError:
            # With python 2.7 this should call the self.skip() method
            print >>sys.stderr, 'WARNING Skipping test_argPOOLFileMetadata_AOD - stat on AFS test file failed'

class argTAGFiles(unittest.TestCase):
    def test_argTAGFileMetadata(self):
        try:
            testFile = '/afs/cern.ch/work/g/graemes/ddm/data12_8TeV.00207865.physics_JetTauEtmiss.merge.TAG.r4065_p1278_tid01030417_00/TAG.01030417._000001.pool.root.1'
            os.stat(testFile)
            tagFile = argTAGFile(testFile, io = 'input', type='tag')
            self.assertEqual(tagFile.getMetadata(), {'/afs/cern.ch/work/g/graemes/ddm/data12_8TeV.00207865.physics_JetTauEtmiss.merge.TAG.r4065_p1278_tid01030417_00/TAG.01030417._000001.pool.root.1': {'_exists': True, 'file_type': 'tag', 'file_guid': '3CCAD8D2-9195-5845-857B-550D616962F9', 'file_size': 12222088, 'integrity': True, 'nentries': 38112L}}) 
            self.assertEqual(tagFile.getMetadata(metadataKeys = ('nentries',)), {'/afs/cern.ch/work/g/graemes/ddm/data12_8TeV.00207865.physics_JetTauEtmiss.merge.TAG.r4065_p1278_tid01030417_00/TAG.01030417._000001.pool.root.1': {'nentries': 38112L}})
            self.assertEqual(tagFile.prodsysDescription['type'],'file')
        except OSError:
            # With python 2.7 this should call the self.skip() method
            print >>sys.stderr, 'WARNING Skipping test_argTAGFileMetadata - stat on AFS test file failed'

class argBSFiles(unittest.TestCase):
    def tearDown(self):
        for f in 'athfile-cache.ascii.gz', 'athfile-infos.ascii':
            try:
                os.unlink(f)
            except OSError:
                pass
    
    def test_argBSFileMetadata(self):
        try:
            testFile = '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts'
            os.stat(testFile)
            rawFile = argBSFile(testFile, io = 'input', type='bs')
            self.assertEqual(rawFile.getMetadata(),{'/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'_exists': True, 'file_type': 'bs', 'file_guid': '4A511034-3A53-E111-8745-003048F0E7AE', 'file_size': 12998048, 'integrity': True, 'nentries': 10}}) 
            self.assertEqual(rawFile.getMetadata(metadataKeys = ('nentries',)), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'nentries': 10}})

            self.assertEqual(rawFile.prodsysDescription['type'],'file')
        except OSError:
            # With python 2.7 this should call the self.skip() method
            print >>sys.stderr, 'WARNING Skipping test_argAthenaFileMetadata - stat on AFS test file failed'

    def test_argBSMultiFileMetadata(self):
        try:
            testFiles = ['/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts',
                         '/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data',
                         '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data',
                         ]
            for fname in testFiles:
                os.stat(fname)
            rawFile = argBSFile(testFiles, io = 'input', type = 'bs')
            self.assertEqual(rawFile.getMetadata(),{'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'_exists': True, 'file_type': 'bs', 'file_guid': 'E8B69378-EF91-E011-A6F0-003048CAD388', 'file_size': 10487108, 'integrity': True, 'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'_exists': True, 'file_type': 'bs', 'file_guid': '4A511034-3A53-E111-8745-003048F0E7AE', 'file_size': 12998048, 'integrity': True, 'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data': {'_exists': True, 'file_type': 'bs', 'file_guid': 'FABAAD37-B38E-E011-8C1D-003048CAD384', 'file_size': 12946296, 'integrity': True, 'nentries': 10}}) 
            self.assertEqual(rawFile.getMetadata(metadataKeys = ('nentries',)), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data': {'nentries': 10}})
            self.assertEqual(rawFile.getMetadata(metadataKeys = ('nentries',), files = '/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data'), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'nentries': 10}})

            self.assertEqual(rawFile.prodsysDescription['type'], 'file')
        except OSError:
            # With python 2.7 this should call the self.skip() method
            print >>sys.stderr, 'WARNING Skipping test_argAthenaMultiFileMetadata - stat on AFS test file failed'


if __name__ == '__main__':
    unittest.main()
