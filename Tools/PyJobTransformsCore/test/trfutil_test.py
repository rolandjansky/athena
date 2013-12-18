#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import myunittest
import os


from PyJobTransformsCore.trfutil import *


class TestStringNumberList(myunittest.TestCase):
    def setUp(self):
        self.obj = StringNumberList()

    
    def checkConversion(self,valIn,valOut):
        self.assertEqual( self.obj.convertStringList(valIn), valOut )
        
    
    def assertNone(self,valsIn):
        for valIn in valsIn:
            self.assertEqual( self.obj.convertStringList(valIn), None ) 


class StringNumberList_GoodInput(TestStringNumberList):
    def testSingleFilename(self):
        """single filename (i.e. without [])"""
        valIn = "prefix_only.suffix"
        valOut = [ valIn ]
        self.checkConversion( valIn, valOut )


    def testSingleNumber(self):
        """Single number in []"""
        valIn = "prefix.[001].suffix"
        valOut = [ "prefix.001.suffix" ]
        self.checkConversion( valIn, valOut )
        

    def testRangeSizeOne(self):
        """Range of size 1"""
        valIn = "prefix.[02-2].suffix"
        valOut = [ "prefix.02.suffix" ]
        self.checkConversion( valIn, valOut )
        
        
    def testRange(self):
        """Range of files"""
        valIn = "prefix.[3-0005].suffix"
        valOut = [ "prefix.0003.suffix", "prefix.0004.suffix", "prefix.0005.suffix" ]
        self.checkConversion( valIn, valOut )


    def testList(self):
        """List of files"""
        valIn = "prefix.[6,7,8].suffix"
        valOut = [ "prefix.6.suffix" , "prefix.7.suffix", "prefix.8.suffix" ]
        self.checkConversion( valIn, valOut )


    def testListAndRange(self):
        """List and Range of files"""
        valIn = "prefix.[9,10,14-16].suffix"
        valOut = [ "prefix.09.suffix", "prefix.10.suffix" ,"prefix.14.suffix", "prefix.15.suffix", "prefix.16.suffix" ]
        self.checkConversion( valIn, valOut )


    def testRangeAndList(self):
        """Range and list of files"""
        valIn = "prefix.[17-19,24,28].suffix"
        valOut = [ "prefix.17.suffix", "prefix.18.suffix" ,"prefix.19.suffix", "prefix.24.suffix", "prefix.28.suffix" ]
        self.checkConversion( valIn, valOut )


    def testTwoRanges(self):
        """Two ranges"""
        valIn = "prefix.[30-32,35-36].suffix"
        valOut = [ "prefix.30.suffix", "prefix.31.suffix" ,"prefix.32.suffix",
                   "prefix.35.suffix", "prefix.36.suffix" ]
        self.checkConversion( valIn, valOut )



class StringNumberList_BadInput(TestStringNumberList):
    def testMissingOpenBracket(self):
        """Missing ["""
        badargs = [ "prefix.4-7].suffix" ]
        self.assertNone( badargs )


    def testMissingCloseBracket(self):
        """Missing ]"""
        badargs = [ "prefix.[4-7.suffix" ]
        self.assertNone( badargs )


    def testEmptyBrackets(self):
        """Empty brackets: []"""
        badargs = [ "prefix.[].suffix" ]
        self.assertNone( badargs )


    def testSpuriousCharacter(self):
        """Spurious character"""
        badargs = [ "prefix.[4.7].suffix","prefix.[4x7].suffix",  "prefix.[4X7].suffix", "prefix.[4$7].suffix",
                    "prefix.[4%7].suffix", "prefix.[4#7].suffix" ]
        self.assertNone( badargs )


    def testMissingListEntryBegin(self):
        """Missing number begin [,5]"""
        badargs = [ "prefix.[,5].suffix", "prefix.[,5,7].suffix", "prefix.[,5,7-8].suffix" ]
        self.assertNone( badargs ) 


    def testMissingListEntryEnd(self):
        """Missing number end [8,]"""
        badargs = [ "prefix.[8,].suffix", "prefix.[6,8,].suffix", "prefix.[4-6,8,].suffix" ]
        self.assertNone( badargs )


    def testMissingListEntryMiddle(self):
        """Missing number middle [7,,8]"""
        badargs = [ "prefix.[7,,8].suffix", "prefix.[7,8,,10].suffix", "prefix.[7,,9,10].suffix",
                    "prefix.[2-4,,8].suffix", "prefix.[7,,8-10].suffix" ]
        self.assertNone( badargs )


    def testMissingRangeBegin(self):
        """Missing start number of range"""
        badargs = [ "prefix.[-8].suffix", "prefix.[-8].suffix", "prefix.[-8].suffix", "prefix.[-8,10].suffix",
                    "prefix.[4,-8].suffix", "prefix.[4,-8,10].suffix" ]
        self.assertNone( badargs )


    def testMissingRangeEnd(self):
        """Missing end number of range"""
        badargs = [ "prefix.[8-].suffix", "prefix.[8-,10].suffix", "prefix.[4,8-].suffix",
                    "prefix.[4,8-,10].suffix", "prefix.[8-,10-12].suffix" ]
        self.assertNone( badargs )


    def testRangeDoubleDash(self):
        """Double dash: --"""
        badargs = [ "prefix.[6--8].suffix", "prefix.[6--8,10].suffix", "prefix.[4,6--8].suffix", "prefix.[4,6--8,10].suffix" ]
        self.assertNone( badargs )
        


    def testRangeWrongOrder(self):
        """Ranga has lower end then start"""
        badargs = [ "prefix.[8-7].suffix", "prefix.[8-06].suffix" ]
        self.assertNone( badargs )



    def testNegativeNumbers(self):
        """Negative numbers"""
        badargs = [ "prefix.[-4].suffix", "prefix.[-4,6].suffix", "prefix.[6,-4].suffix", "prefix.[-4-7].suffix",
                    "prefix.[4--7].suffix", "prefix.[-7--4].suffix","prefix.[2,4--7].suffix","prefix.[4--7,2].suffix" ]   
        self.assertNone( badargs )


class TestLoadTransforms(myunittest.TestCase):
    def setUp(self):
        self.trfName = "argstest_trf.py"


    def testLoadDefault(self):
        """Load default JobTransform"""
        self.failUnless( len( load_transforms(self.trfName) ) >= 1, "default does not load any" )


    def testLoadAtLeastOne(self):
        """Load at least one JobTransform"""
        names = [ r".*test$" ]
        for name in names:
            self.failUnless(  len( load_transforms(self.trfName, name) ) >= 1, "name \"%s\" does not load any" % (name) )


    def testLoadNone(self):
        """Load no JobTransforms"""
        names = [ "does not exist", r"^test.*" ]
        for name in names:
            self.failUnless(  len( load_transforms(self.trfName, name) ) == 0, "name \"%s\" should load nothing" % (name) )
       

    def testLoadFromSubDir(self):
        """Load transform in a subdirectory"""
        oldDir = os.getcwd()
        subdir = os.path.basename(oldDir)
        os.chdir(os.pardir)
        try:
            self.failUnless( len( load_transforms(os.path.join(subdir,self.trfName)) ) >= 1, "default in subdir does not load any" )
        finally:
            os.chdir(oldDir)



    def testLoadFromSideDir(self):
        """Load transform in a side-directory"""
        oldDir = os.getcwd()
        subdir = os.path.basename(oldDir)
        os.chdir(os.path.join(os.pardir,'python'))
        try:
            self.failUnless( len( load_transforms(os.path.join(os.pardir,subdir,self.trfName)) ) >= 1, "default in side-dir does not load any" )
        finally:
            os.chdir(oldDir)



    def testLoadFromPythonPath(self):
        """Load transform from pythonpath"""
        oldDir = os.getcwd()
        subdir = os.path.basename(oldDir)
        basedir = os.path.dirname(oldDir)
        os.chdir(os.path.join(os.pardir,'python'))
        try:
            inserted = False
            if not basedir in sys.path:
                sys.path.insert(1,basedir)
                inserted = True
            self.failUnless( len( load_transforms(os.path.join(subdir,self.trfName)) ) >= 1, "default in pythonpath does not load any" )
            if inserted: sys.path.remove(basedir)
        finally:
            os.chdir(oldDir)
        

        
if __name__ == "__main__":
    myunittest.main()



