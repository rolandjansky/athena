#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfArgClasses.py
#  @brief Unittests for test_trfArgClasses.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfArgClassesATLAS.py 628398 2014-11-14 12:54:56Z graemes $
#  @note Tests of ATLAS specific file formats (that thus rely on other
#  parts of Athena) live here

import sys
import unittest

from PyJobTransforms.trfLogger import msg

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfArgClasses import *

class argFileEOSTests(unittest.TestCase):
    def test_SimExpansion(self):
        hitsInputs = argFile('root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000[029,200].pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000[614,817].pool.root.5', io='input')
        self.assertEqual(len(hitsInputs.value), 4)
        
    def test_SimGlobStar(self):
        hitsInputs = argFile('root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.*', io='input')
        self.assertEqual(len(hitsInputs.value), 4)        

    def test_SimGlobMatchSingle(self):
        hitsInputs = argFile('root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._??????.pool.root.?', io='input')
        self.assertEqual(len(hitsInputs.value), 4)        

    def test_SimGlobMatchBoth(self):
        hitsInputs = argFile('root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._*.pool.root.?', io='input')
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
            self.assertEqual(esdFile.getMetadata(), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.ESD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'_exists': True, 'run_number': [182796L], 'beam_energy': [3500000.0], 'file_type': 'pool', 'AODFixVersion': '', 'file_size': 17033381L, 'geometry': 'ATLAS-GEO-16-00-01', 'file_guid': '0CABA22E-9096-E011-AE25-0030487C8CE6', 'beam_type': ['collisions'], 'lumi_block': [300L], 'conditions_tag': 'COMCOND-BLKPST-004-00', 'integrity': True, 'nentries': 10L}}) 
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
            self.assertEqual(aodFile.getMetadata(), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.AOD._lb0300._SFO-10._0001.1.10evts.16.6.6.4.pool.root': {'_exists': True, 'run_number': [182796L], 'beam_energy': [3500000.0], 'file_type': 'pool', 'AODFixVersion': '', 'file_size': 4673269L, 'geometry': 'ATLAS-GEO-16-00-01', 'file_guid': '6E1FE6F0-9096-E011-9DDA-0030487C8CE6', 'beam_type': ['collisions'], 'lumi_block': [300L], 'conditions_tag': 'COMCOND-BLKPST-004-00', 'integrity': True, 'nentries': 10L}}) 
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
            print '+++', tagFile.getMetadata()
            self.assertEqual(tagFile.getMetadata(), {'/afs/cern.ch/work/g/graemes/ddm/data12_8TeV.00207865.physics_JetTauEtmiss.merge.TAG.r4065_p1278_tid01030417_00/TAG.01030417._000001.pool.root.1': {'_exists': True, 'run_number': [207865L], 'beam_energy': [], 'file_type': 'tag', 'AODFixVersion': '', 'file_size': 12222088, 'geometry': None, 'file_guid': '3CCAD8D2-9195-5845-857B-550D616962F9', 'beam_type': [], 'lumi_block': [], 'conditions_tag': None, 'integrity': True, 'nentries': 38112L}})
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
            self.assertEqual(rawFile.getMetadata(), {'/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'_exists': True, 'run_number': [191920], 'beam_energy': [0], 'file_type': 'bs', 'AODFixVersion': '', 'file_size': 12998048L, 'geometry': None, 'file_guid': '4A511034-3A53-E111-8745-003048F0E7AE', 'beam_type': ['collisions'], 'lumi_block': [257], 'conditions_tag': None, 'integrity': True, 'nentries': 10}}) 
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
            self.assertEqual(rawFile.getMetadata(), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'_exists': True, 'run_number': [182609], 'beam_energy': [0], 'file_type': 'bs', 'AODFixVersion': '', 'file_size': 10487108L, 'geometry': None, 'file_guid': 'E8B69378-EF91-E011-A6F0-003048CAD388', 'beam_type': ['cosmics'], 'lumi_block': [100], 'conditions_tag': None, 'integrity': True, 'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'_exists': True, 'run_number': [191920], 'beam_energy': [0], 'file_type': 'bs', 'AODFixVersion': '', 'file_size': 12998048L, 'geometry': None, 'file_guid': '4A511034-3A53-E111-8745-003048F0E7AE', 'beam_type': ['collisions'], 'lumi_block': [257], 'conditions_tag': None, 'integrity': True, 'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data': {'_exists': True, 'run_number': [182796], 'beam_energy': [3500], 'file_type': 'bs', 'AODFixVersion': '', 'file_size': 12946296L, 'geometry': None, 'file_guid': 'FABAAD37-B38E-E011-8C1D-003048CAD384', 'beam_type': ['collisions'], 'lumi_block': [300], 'conditions_tag': None, 'integrity': True, 'nentries': 10}}) 
            self.assertEqual(rawFile.getMetadata(metadataKeys = ('nentries',)), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data': {'nentries': 10}})
            self.assertEqual(rawFile.getMetadata(metadataKeys = ('nentries',), files = '/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data'), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'nentries': 10}})

            self.assertEqual(rawFile.prodsysDescription['type'], 'file')
        except OSError:
            # With python 2.7 this should call the self.skip() method
            print >>sys.stderr, 'WARNING Skipping test_argAthenaMultiFileMetadata - stat on AFS test file failed'


if __name__ == '__main__':
    unittest.main()
