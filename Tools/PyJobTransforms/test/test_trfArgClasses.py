#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfArgClasses.py
#  @brief Unittests for test_trfArgClasses.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfArgClasses.py 604265 2014-06-30 14:53:32Z graemes $

import os
import sys
import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfArgClasses import *

## Unittests for this module
class trfArgumentTests(unittest.TestCase):

    def setUp(self):
        self.message = 'a string'
        
        self.a = argument()
        self.b = argument()
        self.c = argument(self.message)
        self.d = argument(self.message, runarg = False)

    ## Test basic properties of arguments
    def test_argumentInitialValue(self):
        self.assertEqual(self.a.value, None)
        self.assertEqual(self.c.value, self.message)

    def test_argumentCmp(self):
        self.assertEqual(self.a, self.b)
        self.assertNotEqual(self.a, self.c)
                
    def test_argumentSetValue(self):
        self.a.value = self.message
        self.assertEqual(self.a.value, self.message)
        self.assertEqual(self.a, self.c)

    def test_argumentRunarg(self):
        self.assertEqual(self.a.isRunarg, True)
        self.assertEqual(self.d.isRunarg, False)

class trfArgStringTests(unittest.TestCase):
    
    def setUp(self):
        self.message = 'Flibbertigibbet'
        self.aFloat = 1922
        
        self.a = argString()
        self.b = argString(self.message)
        self.c = argString(self.aFloat)
        self.d = argString(self.message, runarg = False)
        self.veg = argString('potato', choices = ['potato', 'carrot', 'broccoli', 'onion'], runarg = True)
        
    ## Test basic properties of arguments
    def test_argStringInitialValue(self):
        self.assertEqual(self.a.value, '')
        self.assertEqual(self.b.value, self.message)
        self.assertEqual(self.c.value, str(self.aFloat))

    def test_argStringCmp(self):
        self.assertEqual(self.b, self.d)
        self.assertNotEqual(self.b, self.c)
                
    def test_argStringSetValue(self):
        self.a.value = self.message
        self.assertEqual(self.a.value, self.message)
        self.assertEqual(self.a, self.b)

    def test_argStringRunarg(self):
        self.assertEqual(self.a.isRunarg, True)
        self.assertEqual(self.d.isRunarg, False)

    def test_argStringprodsysDescription(self):
        self.assertEqual(self.a.prodsysDescription['type'],'string')
        
    def test_argStringChoices(self):
        self.assertEqual(self.veg.value, 'potato')
        self.veg.value = 'onion'
        self.assertEqual(self.veg.value, 'onion')
        
    def test_argStringChoicesBad(self):
        self.assertRaises(trfExceptions.TransformArgException, argString, 'orange', 
                          choices = ['potato', 'carrot', 'broccoli', 'onion'])

class trfArgIntTests(unittest.TestCase):

    def setUp(self):
        self.anInt1 = 1968
        self.anInt2 = -1066
        self.aValidIntString = '45'
        
        self.a = argInt()
        self.b = argInt(self.anInt1)
        self.c = argInt(self.anInt2)
        self.d = argInt(self.anInt1, runarg = False)
        self.e = argInt(self.aValidIntString)
        
    ## Test basic properties of arguments
    def test_argIntInitialValue(self):
        self.assertEqual(self.a.value, 0)
        self.assertEqual(self.b.value, self.anInt1)
        self.assertEqual(self.c.value, self.anInt2)
        self.assertEqual(self.e.value, int(self.aValidIntString))

    def test_argIntCmp(self):
        self.assertEqual(self.b, self.d)
        self.assertNotEqual(self.b, self.c)
                
    def test_argIntSetValue(self):
        self.a.value = self.anInt1
        self.assertEqual(self.a.value, self.anInt1)
        self.assertEqual(self.a, self.b)

    def test_argIntRunarg(self):
        self.assertEqual(self.a.isRunarg, True)
        self.assertEqual(self.d.isRunarg, False)
        
    def test_argIntSetBadValue(self):
        self.assertRaises(trfExceptions.TransformArgException, argInt, 'rubbish')
        self.assertRaises(trfExceptions.TransformArgException, argInt, '1.141')

    def test_argIntprodsysDescription(self):
        self.assertEqual(self.a.prodsysDescription['type'],'INT') 

class trfArgBoolTests(unittest.TestCase):
    
    def setUp(self):
        self.a = argBool()
        self.b = argBool(True)
        self.c = argBool(False, runarg = False)
        self.d = argBool(True, runarg = False)
        
    ## Test basic properties of arguments
    def test_argBoolInitialValue(self):
        self.assertEqual(self.a.value, False)
        self.assertEqual(self.b.value, True)
        self.assertEqual(self.c.value, False)

    def test_argBoolCmp(self):
        self.assertEqual(self.b, self.d)
        self.assertNotEqual(self.b, self.c)
                
    def test_argBoolSetValue(self):
        self.a.value = True
        self.assertEqual(self.a.value, True)
        self.assertEqual(self.a, self.b)

    def test_argBoolRunarg(self):
        self.assertEqual(self.a.isRunarg, True)
        self.assertEqual(self.d.isRunarg, False)

    def test_argBoolprodsysDescription(self):
        self.assertEqual(self.a.prodsysDescription['type'],'bool')
        
class trfArgFloatTests(unittest.TestCase):
    
    def setUp(self):
        self.aFloat1 = 1922.67
        self.aFloat2 = -1066.45
        self.aValidFloatString = '3.14'
        
        self.a = argFloat()
        self.b = argFloat(self.aFloat1)
        self.c = argFloat(self.aFloat2)
        self.d = argFloat(self.aFloat1, runarg = False)
        self.e = argFloat(self.aValidFloatString)
        self.f = argFloat(self.aFloat1, min = 1000, max = 2000)
        
    ## Test basic properties of arguments
    def test_argFloatInitialValue(self):
        self.assertEqual(self.a.value, 0.0)
        self.assertEqual(self.b.value, self.aFloat1)
        self.assertEqual(self.c.value, self.aFloat2)
        self.assertEqual(self.e.value, float(self.aValidFloatString))

    def test_argFloatCmp(self):
        self.assertEqual(self.b, self.d)
        self.assertNotEqual(self.b, self.c)
                
    def test_argFloatSetValue(self):
        self.a.value = self.aFloat1
        self.assertEqual(self.a.value, self.aFloat1)
        self.assertEqual(self.a, self.b)

    def test_argFloatRunarg(self):
        self.assertEqual(self.a.isRunarg, True)
        self.assertEqual(self.d.isRunarg, False)
        
    def test_argFloatSetBadValue(self):
        self.assertRaises(trfExceptions.TransformArgException, argFloat, 'rubbish')

    def test_argFloatMinMax(self):
        # This code does not work until python2.7
#        with self.assertRaises(trfExceptions.TransformArgException):
#            self.f.value = 2100
#        with self.assertRaises(trfExceptions.TransformArgException):
#            self.f.value = 900
        self.assertRaises(trfExceptions.TransformArgException, argFloat, value = 67, max = 60)
        self.assertRaises(trfExceptions.TransformArgException, argFloat, value = 67, min = 100)

    def test_argFloatprodsysDescription(self):
        self.assertEqual(self.f.prodsysDescription['type'],'float')
        self.assertEqual(self.f.prodsysDescription['min'],self.f._min)
        self.assertEqual(self.f.prodsysDescription['max'],self.f._max)

class argListTests(unittest.TestCase):
    def test_basicList(self):
        lst = argList('bob,sally,jack')
        self.assertEqual(len(lst.value), 3)
        
    def test_emptySupression(self):
        l_nosup = argList('bob,,sally,jack', supressEmptyStrings = False)
        l_sup = argList('bob,,sally,jack', supressEmptyStrings = True)
        self.assertEqual(len(l_sup.value), 3)
        self.assertEqual(len(l_nosup.value), 4)
        
    def test_emptyList(self):
        lst = argList('', supressEmptyStrings = True)
        self.assertEqual(lst.value, [])


class argIntListTests(unittest.TestCase):
    def test_basicIntList(self):
        lst = argIntList('56,89,-100')
        self.assertEqual(len(lst.value), 3)
        self.assertEqual(lst.value[2], -100)
        
    def test_emptyIntListSupression(self):
        lst = argIntList('23,,-50,99', supressEmptyStrings = True)
        self.assertEqual(len(lst.value), 3)

    def test_emptyIntList(self):
        lst = argIntList('', supressEmptyStrings = True)
        self.assertEqual(lst.value, [])

    def test_illegalIntList(self):
        self.assertRaises(trfExceptions.TransformArgException, argIntList, value = "notAnInt")


class argKeyFloatValueListTests(unittest.TestCase):
    def test_basicKFVList(self):
        lst = argKeyFloatValueList('ESD:123.6,AOD:456.1,TAG:0,HIST:-1')
        self.assertEqual(len(lst.value), 4)
        self.assertEqual(lst.value['ESD'], 123.6)
        self.assertEqual(lst.value['AOD'], 456.1)
        self.assertEqual(lst.value['TAG'], 0)
        self.assertEqual(lst.value['HIST'], -1)
        
    def test_emptyKFVSupression(self):
        lst = argKeyFloatValueList('ESD:123,AOD:456,TAG:0,,HIST:-1', supressEmptyStrings = True)
        self.assertEqual(len(lst.value), 4)
        self.assertEqual(lst.value['ESD'], 123)
        self.assertEqual(lst.value['AOD'], 456)
        self.assertEqual(lst.value['TAG'], 0)
        self.assertEqual(lst.value['HIST'], -1)

    def test_emptyKFVList(self):
        lst = argKeyFloatValueList('', supressEmptyStrings = True)
        self.assertEqual(lst.value, {})

    def test_illegalKFVList(self):
        self.assertRaises(trfExceptions.TransformArgException, argKeyFloatValueList, value = "foo:bar")
        self.assertRaises(trfExceptions.TransformArgException, argKeyFloatValueList, value = "foobar")


class argPickleDictTests(unittest.TestCase):
    # Write me!
    pass


class argSubstepTests(unittest.TestCase):
    def test_substepBasic(self):
        ss = argSubstep('free biscuits')
        self.assertEqual(ss.value, {'all': 'free biscuits'})

    def test_substepBasicAll(self):
        ss = argSubstep('all:free biscuits')
        self.assertEqual(ss.value, {'all': 'free biscuits'})

    def test_substepFromList(self):
        ss = argSubstep(['free biscuits', 'fruit:apple', 'drink:lemonade'])
        self.assertEqual(ss.value, {'all': 'free biscuits', 'fruit': 'apple', 'drink': 'lemonade'})

    def test_substepFromDict(self):
        d = {'all': 'foo', 'fruit': 'bar', 'drink': 'baz'}
        ss = argSubstep(d)
        self.assertEqual(ss.value, d)
        
    def test_substepFloat(self):
        ss = argSubstepFloat("1.2")
        self.assertEqual(ss.value, {'all': 1.2})
    
    def test_substepFloatMinMax(self):
        ss = argSubstepFloat("1.2",min=1,max=2)
        self.assertEqual(ss.value, {'all': 1.2})
        
    def test_substepFloatMinMaxBroken(self):
        self.assertRaises(trfExceptions.TransformArgException, argSubstepFloat, "1.2", min=0., max=1. )
    
    def test_substepInt(self):
        ss = argSubstepInt("4711")
        self.assertEqual(ss.value, {'all': 4711})
        
    def test_substepEqualsSeparator(self):
        ss = argSubstep("free=juice", separator='=')
        self.assertEqual(ss.value, {"free": "juice"})


class argSubstepListTests(unittest.TestCase):
    def test_substepBasic(self):
        ss = argSubstepList('free biscuits')
        self.assertEqual(ss.value, {'all': ['free biscuits']})

    def test_substepBasicAll(self):
        ss = argSubstepList('all:free biscuits')
        self.assertEqual(ss.value, {'all': ['free biscuits']})

    def test_substepList(self):
        ss = argSubstepList(['all:free biscuits', 'fruit:apple', 'fruit:pear', 'drink:lemonade', 'first:lay table', 'default:mince'])
        self.assertEqual(ss.value, {'all': ['free biscuits'], 'fruit': ['apple', 'pear'], 'drink': ['lemonade'], 
                                    'first': ['lay table'], 'default': ['mince']})
        self.assertEqual(ss.returnMyValue(name='fruit'), ['free biscuits', 'apple', 'pear'])
        self.assertEqual(ss.returnMyValue(name='entree'), ['free biscuits', 'mince'])
        self.assertEqual(ss.returnMyValue(name='entree', first=True), ['free biscuits', 'lay table'])

    def test_substepListAll(self):
        ss = argSubstepList(['all:free biscuits', 'fruit:apple', 'fruit:pear', 'drink:lemonade', 'free beer'])
        self.assertEqual(ss.value, {'all': ['free biscuits', 'free beer'], 'fruit': ['apple', 'pear'], 'drink': ['lemonade']})
        
    def test_substepListSplitter(self):
        ss = argSubstepList(['all:free,biscuits', 'fruit:apple', 'fruit:pear', 'drink:lemonade', 'first:lay,table', 'default:mince'], splitter=',')
        self.assertEqual(ss.value, {'all': ['free', 'biscuits'], 'fruit': ['apple', 'pear'], 'drink': ['lemonade'], 
                                    'first': ['lay', 'table'], 'default': ['mince']})
        self.assertEqual(ss.returnMyValue(name='fruit'), ['free', 'biscuits', 'apple', 'pear'])
        self.assertEqual(ss.returnMyValue(name='entree'), ['free', 'biscuits', 'mince'])
        self.assertEqual(ss.returnMyValue(name='entree', first=True), ['free', 'biscuits', 'lay', 'table'])

    def test_substepFromDict(self):
        d = {'all': ['free biscuits', 'will come'], 'fruit': ['apple', 'pear'], 'drink': ['dishwater']}
        ss = argSubstepList(d)
        self.assertEqual(ss.value, d)

    def test_substepBroken(self):
        self.assertRaises(trfExceptions.TransformArgException, argSubstepList, value={'key' : 'value'})


class argSteeringTests(unittest.TestCase):
    def test_basicStrSet(self):
        steer = argSubstepSteering('RAWtoESD:out-RDO')
        self.assertEqual(len(steer.value), 1)
        self.assertEqual(steer.value, {'RAWtoESD': [('out', '-', 'RDO')]})

    def test_multiStrSet(self):
        steer = argSubstepSteering('RAWtoESD:out-RDO,out+RDO_TRIG')
        self.assertEqual(len(steer.value), 1)
        self.assertEqual(steer.value, {'RAWtoESD': [('out', '-', 'RDO'), ('out', '+', 'RDO_TRIG')]})

    def test_multiStrSet2(self):
        steer = argSubstepSteering(['RAWtoESD:out-RDO,out+RDO_TRIG', 'ESDtoAOD:in+JUNK'])
        self.assertEqual(len(steer.value), 2)
        self.assertEqual(steer.value, {'RAWtoESD': [('out', '-', 'RDO'), ('out', '+', 'RDO_TRIG')], 'ESDtoAOD': [('in', '+', 'JUNK')]})


class argFileTests(unittest.TestCase):
    def setUp(self):
        # In python 2.7 support for multiple 'with' expressions becomes available
        with open('file1', 'w') as f1:
            print >>f1, 'This is test file 1'
        with open('file2', 'w') as f2:
            print >>f2, 'Short file 2'
        with open('file3', 'w') as f3:
            print >>f3, 80*'-', 'Long file 3', 80*'-'
        with open('prefix.prodsysfile._001.suffix.1', 'w') as f1:
            print >>f1, 'This is prodsys test file 1'
        with open('prefix.prodsysfile._002.suffix.4', 'w') as f2:
            print >>f2, 'Short prodsys file 2'
        with open('prefix.prodsysfile._003.suffix.7', 'w') as f3:
            print >>f3, 80*'-', 'Long prodsys file 3', 80*'-'
            
        self.mySingleFile = argFile(['file1'], io='output')
        self.myMultiFile = argFile(['file1', 'file2', 'file3'], io='input')
        self.myManualGUIDSingleFile = argFile(['file1'], io='output', guid={'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B'})
        self.myManualGUIDMultiFile = argFile(['file1', 'file2', 'file3'], io='input', guid={'file1': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'file2': '1368D295-27C6-4A92-8187-704C2A6A5864', 'file3': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9'})

    def tearDown(self):
        for f in ('file1', 'file2', 'file3', 'prefix.prodsysfile._001.suffix.1', 'prefix.prodsysfile._002.suffix.4', 
                  'prefix.prodsysfile._003.suffix.7'):
            try:
                os.unlink(f)
            except OSError:
                pass

    def test_argFileInitialValue(self):
        self.assertEqual(self.mySingleFile.value, ['file1',])
        self.assertEqual(self.myMultiFile.value, ['file1', 'file2', 'file3'])
        
    def test_athFileValueSetter(self):
        self.mySingleFile.value = 'theAmazingFile'
        self.assertEqual(self.mySingleFile.value, ['theAmazingFile',])
        self.myMultiFile.value = ('file3', 'file2')
        self.assertEqual(self.myMultiFile.value, ['file3', 'file2'])
        # Next test fails because input files glob to nothing
        self.assertRaises(trfExceptions.TransformArgException, self.mySingleFile.__init__, value=['fileXXX', 'fileYYY'])
        
    def test_argFileGlob(self):
        myInput = argFile('file?', io='input')
        # Use set comparison as glob order is not guaranteed
        self.assertEqual(set(myInput.value), set(['file1', 'file2', 'file3']))    

    def test_argFileProdsysGlob(self):
        myInput = argFile('prefix.prodsysfile._[001,002,003].suffix', io='input')
        self.assertEqual(myInput.value, ['prefix.prodsysfile._001.suffix.1', 'prefix.prodsysfile._002.suffix.4', 
                                         'prefix.prodsysfile._003.suffix.7'])
        
    def test_argFileIO(self):
        self.assertEqual(self.mySingleFile.io, 'output')
        self.assertEqual(self.myMultiFile.io, 'input')
        
    def test_argFileDataset(self):
        withDataset = argFile('fakeDatasetName#file1')
        self.assertEqual(withDataset.dataset, 'fakeDatasetName')
    
    def test_argFileMetadata(self):
        # Can't test all metadata directly now we added a GUID generator
        self.assertTrue(cmpMetadata(self.mySingleFile.getMetadata(), {'file1': {'_exists': True, 'file_guid': 'D6F5F632-4EA6-4EA6-9A78-9CF59C247094', 'integrity': True, 'file_size': 20}}))
        self.assertTrue(cmpMetadata(self.myMultiFile.getMetadata(),{'file3': {'_exists': True, 'file_guid': 'CAB26113-8CEC-405A-BEDB-9B1CFDD96DA8', 'integrity': True, 'file_size': 174}, 'file2': {'_exists': True, 'file_guid': '23310685-BBC3-4385-9870-1622D15B4C2D', 'integrity': True, 'file_size': 13}, 'file1': {'_exists': True, 'file_guid': '9E3EDD48-978E-4441-97B9-F3FF4241DE80', 'integrity': True, 'file_size': 20}}))         

    def test_argFileMetadataWithGUID(self):
        self.assertEqual(self.myManualGUIDSingleFile.getMetadata(), {'file1': {'_exists': True, 'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'integrity': True, 'file_size': 20}})
        self.assertEqual(self.myManualGUIDMultiFile.getMetadata(), {'file3': {'_exists': True, 'file_guid': 'F5BA4602-6CA7-4111-B3C7-CB06486B30D9', 'integrity': True, 'file_size': 174}, 'file2': {'_exists': True, 'file_guid': '1368D295-27C6-4A92-8187-704C2A6A5864', 'integrity': True, 'file_size': 13}, 'file1': {'_exists': True, 'file_guid': '05ACBDD0-5F5F-4E2E-974A-BBF4F4FE6F0B', 'integrity': True, 'file_size': 20}}) 

    def test_argFileprodsysDescription(self):
        self.assertEqual(self.mySingleFile.prodsysDescription['type'],'file')
        
    def test_argFileDuplicateRemoval(self):
        self.myMultiFile.value = ('file1', 'file1', 'file3', 'file2', 'file3', 'file1')
        self.assertEqual(self.myMultiFile.value, ['file1', 'file3', 'file2']) # Base order preserved
        
    ## @brief Test isCached() method
    def test_argFileMetadataCache(self):
        self.assertFalse(self.myMultiFile.isCached(metadataKeys = ['file_size']))
        self.myMultiFile.getMetadata(metadataKeys = ['file_size'])
        self.assertTrue(self.myMultiFile.isCached(metadataKeys = ['file_size']))
        
    ## @brief Test that we can manually set metadata items
    def test_argFileSetMetadata(self):
        self.myMultiFile._setMetadata(files=None, metadataKeys={'file_size': 1234567, '_exists': True})
        self.assertEqual(self.myMultiFile.getSingleMetadata('file1', 'file_size'), 1234567)

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
            for file in testFiles:
                os.stat(file)
            rawFile = argBSFile(testFiles, io = 'input', type = 'bs')
            self.assertEqual(rawFile.getMetadata(), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'_exists': True, 'run_number': [182609], 'beam_energy': [0], 'file_type': 'bs', 'AODFixVersion': '', 'file_size': 10487108L, 'geometry': None, 'file_guid': 'E8B69378-EF91-E011-A6F0-003048CAD388', 'beam_type': ['cosmics'], 'lumi_block': [100], 'conditions_tag': None, 'integrity': True, 'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'_exists': True, 'run_number': [191920], 'beam_energy': [0], 'file_type': 'bs', 'AODFixVersion': '', 'file_size': 12998048L, 'geometry': None, 'file_guid': '4A511034-3A53-E111-8745-003048F0E7AE', 'beam_type': ['collisions'], 'lumi_block': [257], 'conditions_tag': None, 'integrity': True, 'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data': {'_exists': True, 'run_number': [182796], 'beam_energy': [3500], 'file_type': 'bs', 'AODFixVersion': '', 'file_size': 12946296L, 'geometry': None, 'file_guid': 'FABAAD37-B38E-E011-8C1D-003048CAD384', 'beam_type': ['collisions'], 'lumi_block': [300], 'conditions_tag': None, 'integrity': True, 'nentries': 10}}) 
            self.assertEqual(rawFile.getMetadata(metadataKeys = ('nentries',)), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts': {'nentries': 10}, '/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data': {'nentries': 10}})
            self.assertEqual(rawFile.getMetadata(metadataKeys = ('nentries',), files = '/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data'), {'/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data': {'nentries': 10}})

            self.assertEqual(rawFile.prodsysDescription['type'],'file')
        except OSError:
            # With python 2.7 this should call the self.skip() method
            print >>sys.stderr, 'WARNING Skipping test_argAthenaMultiFileMetadata - stat on AFS test file failed'


if __name__ == '__main__':
    unittest.main()
