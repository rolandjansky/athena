#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys

import myunittest
from PyJobTransformsCore.basic_trfarg import *
from PyJobTransformsCore.trfutil import *

class TestInputTarFileArg(myunittest.TestCase):
    def setUp(self):
        """setup temporary files to work with.
        Strategy:
        Create a temporary directory (given by system).
        In this directory, create 2 subdirectories: 'src' and 'dest'.
        Put all the source files (tar's) into the 'src' subdir,
        run the tests from the temporary dir, and gives the 'dest'
        subdir as a destination dir for the TarZipFile object.
        The input tar files will be given counted from the current dir,
        so with the 'dest' prefix.
        """
        myunittest.TestCase.setUp(self)
        if self.debug: self.listTmpFiles(os.linesep + "BEFORE SETUP")
        self.srcdir = 'src'
        self.destdir = 'dest'
        self.srcfulldir  = self.makeTmpDir( self.srcdir )
        self.destfulldir = self.makeTmpDir( self.destdir )
        self.arg = InputTarFileArg(help="Testing InputTarFileArg",destdir=self.destdir)
        # setup temporary filenames
        self.prefix = 'test'
        self.tarfile = "%s.tar" % self.prefix
        self.targzfile = "%s.tar.gz" % self.prefix
        self.tgzfile = "%s.tgz" % self.prefix
        self.files = [ "%s.dat" % (self.prefix), "%s.txt" % (self.prefix) ]
        # create the files
        for f in self.files: self.createTmpFile( os.path.join( self.srcdir, f ) )
        if self.debug: self.listTmpFiles("AFTER SETUP")


    def tearDown(self):
        """remove temporary files"""
        if self.debug: self.listTmpFiles("BEFORE TEARDOWN")
        myunittest.TestCase.tearDown(self)
        if self.debug: self.listTmpFiles("AFTER TEARDOWN")


    def createTarFile(self):
        """Creates a tarfile containing the files in self.files.
        It returns the full path of the tarfile."""
        subtar = os.path.join( self.srcdir, self.tarfile )
        # remove existing tarfile
        self.removeTmpFiles( subtar )
        # make new one
        fulltar = self.fullTmpFile( subtar )
        tarargs = "%s" % (' '.join(self.files))
        tardir = os.path.dirname( fulltar )
        cmds = [ "cd %s ; tar -cf %s %s" % (tardir, self.tarfile, tarargs) ]
        for cmd in cmds:
            if self.debug: print cmd
            if not self.fake:
                stat = os.system( cmd )
                if stat != 0:
                    raise OSError( "shell command %s FAILED (exit=%d)" % (cmd,stat) )

        return fulltar
       

    def createTarGzFile(self):
        """Creates a zipped tarfile containing the files in self.files"""
        # create the tarfile
        fulltar = self.createTarFile()
        # zip it
        tardir = os.path.dirname( fulltar )
        subtargz = os.path.join( self.srcdir, self.targzfile )
        fulltargz = self.fullTmpFile( subtargz )
        targzdir = os.path.dirname( fulltargz )
        assert( tardir == targzdir )
        cmds = [ "cd %s ; gzip %s" % (tardir, self.tarfile) ]
        for cmd in cmds:
            if self.debug: print cmd
            if not self.fake:
                stat = os.system( cmd )
                if stat != 0:
                    raise OSError( "shell command %s FAILED (exit=%d)" % (cmd,stat) )
       
        return fulltargz


    def createTgzFile(self):
        subtgz = os.path.join( self.srcdir, self.tgzfile )
        # remove existing tarfile
        self.removeTmpFiles( subtgz )
        # make and register new one
        fulltgz = self.fullTmpFile( subtgz )
        tarargs = "%s" % (' '.join(self.files))
        tardir = os.path.dirname( fulltgz )
        cmds = [ "cd %s ; tar -czf %s %s" % (tardir, self.tgzfile, tarargs) ]
        for cmd in cmds:
            if self.debug: print cmd
            if not self.fake:
                stat = os.system( cmd )
                if stat != 0:
                    raise OSError( "shell command %s FAILED (exit=%d)" % (cmd,stat) )
        
        return fulltgz


    def contentsAndExtraction(self,archive):
        # setup & execute
        arg = self.arg
        self.cdTmpDir()
        infile = os.path.join(self.srcdir,archive)
        arg.setValue(infile)
        found = arg.filelist()
        arg.extract()
        # test the contents
        found.sort()
        self.files.sort()
        self.assertEqual( found, self.files )
        # test the extraction
        for f in self.files:
            self.failUnless( os.path.exists( os.path.join( self.destfulldir, f ) ),
                             "File %s not extracted from archive %s" % (f,infile) )


    def testTarFile(self):
        """Test a non-zipped tarfile"""
        self.createTarFile()
        self.contentsAndExtraction( self.tarfile )

        
    def testTarGzFile(self):
        """Test a gzipped tarfile"""
        self.createTarGzFile()
        self.contentsAndExtraction( self.targzfile )


    def testTgzFile(self):
        """Test a tgz tarfile"""
        self.createTgzFile()
        self.contentsAndExtraction( self.tgzfile )





class TestInputFileListArg(myunittest.TestCase):
    def setUp(self):
        self.arg = InputFileListArg("Testing InputFileListArg",'default',
                                    FileType(type='root',contents='test|TEST') )

    
    def checkConversion(self,valIn,valOut):
        self.assertEqual( self.arg.toPython(valIn), valOut )
        
    
    def assertException(self,valsIn):
        for valIn in valsIn:
            self.assertRaises( TransformArgumentError, self.arg.toPython, valIn ) 



class TestInputFileListArg_GoodInput(TestInputFileListArg):
    def testSingleFilename(self):
        """single filename (i.e. without [])"""
        valIn = "prefix_only.suffix"
        valOut = [ valIn ]
        self.checkConversion( valIn, valOut )


    def testSimpleList(self):
        """list of simple filenames"""
        valIn = "prefix1_only.suffix1,prefix2_only.suffix2,prefix3_only.suffix3"
        valOut = valIn.split(FileList.listSep)
        self.checkConversion( valIn, valOut )


    def testSingleNumber(self):
        """Single number in []"""
        valIn = "prefix.[001].suffix"
        valOut = [ "prefix.001.suffix" ]
        self.checkConversion( valIn, valOut )
        

    def testSingleNumberFirstInList(self):
        """Single number in [], first in list"""
        valIn = "prefix.[001].suffix,filename2,filename3"
        valOut = [ "prefix.001.suffix", "filename2", "filename3" ]
        self.checkConversion( valIn, valOut )
        

    def testSingleNumberMiddleInList(self):
        """Single number in [], middle in list"""
        valIn = "filename1,prefix.[001].suffix,filename2"
        valOut = [ "filename1", "prefix.001.suffix", "filename2" ]
        self.checkConversion( valIn, valOut )
        

    def testSingleNumberLastInList(self):
        """Single number in [], last in list"""
        valIn = "filename1,filename2,prefix.[001].suffix"
        valOut = [ "filename1", "filename2", "prefix.001.suffix" ]
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


    def testRangeAndListFirstInList(self):
        """Coded range and list of files, first in list"""
        valIn = "prefix.[17-19,24,28].suffix,filename1,filename2"
        valOut = [ "prefix.17.suffix", "prefix.18.suffix" ,"prefix.19.suffix", "prefix.24.suffix",
                   "prefix.28.suffix", "filename1", "filename2" ]
        self.checkConversion( valIn, valOut )

        
    def testRangeAndListMiddleInList(self):
        """Coded range and list of files, middle in list"""
        valIn = "filename1,prefix.[17-19,24,28].suffix,filename2"
        valOut = [ "filename1", "prefix.17.suffix", "prefix.18.suffix" ,"prefix.19.suffix", "prefix.24.suffix",
                   "prefix.28.suffix", "filename2" ]
        self.checkConversion( valIn, valOut )
        

    def testRangeAndListLastInList(self):
        """Coded range and list of files, last in list"""
        valIn = "filename1,filename2,prefix.[17-19,24,28].suffix"
        valOut = [ "filename1", "filename2", "prefix.17.suffix", "prefix.18.suffix" ,"prefix.19.suffix", "prefix.24.suffix",
                   "prefix.28.suffix" ]
        self.checkConversion( valIn, valOut )
        

    def testAllTogether(self):
        """All kinds of combinations"""
        valIn = "filename1,prefix.[17-19].suffix,prefix.[24,28].suffix" + \
        ",filename2,prefix.[32-34].suffix,filename3"
        valOut = [ "filename1", "prefix.17.suffix", "prefix.18.suffix" ,"prefix.19.suffix",
                   "prefix.24.suffix", "prefix.28.suffix", "filename2",
                   "prefix.32.suffix", "prefix.33.suffix", "prefix.34.suffix", "filename3" ]
        self.checkConversion( valIn, valOut )


    def testTwoRanges(self):
        """Two ranges"""
        valIn = "prefix.[30-32,35-36].suffix"
        valOut = [ "prefix.30.suffix", "prefix.31.suffix" ,"prefix.32.suffix",
                   "prefix.35.suffix", "prefix.36.suffix" ]
        self.checkConversion( valIn, valOut )



class InputFileListArg_BadInput(TestInputFileListArg):
    def testCommaOnly(self):
        """comma only"""
        badargs = [ "," ]
        self.assertException( badargs )


    def testEmptyFieldBegin(self):
        """Empty field begin"""
        badargs = [ ",file1" ]
        self.assertException( badargs )


    def testEmptyFieldMiddle(self):
        """Empty field middle"""
        badargs = [ "file1,,file2" ]
        self.assertException( badargs )


    def testEmptyFieldEnd(self):
        """Empty field end"""
        badargs = [ "file1," ]
        self.assertException( badargs )
       
    
    def testMissingOpenBracket(self):
        """Missing ["""
        badargs = [ "prefix.4-7].suffix" ]
        self.assertException( badargs )


    def testMissingCloseBracket(self):
        """Missing ]"""
        badargs = [ "prefix.[4-7.suffix" ]
        self.assertException( badargs )


    def testEmptyBrackets(self):
        """Empty brackets: []"""
        badargs = [ "prefix.[].suffix" ]
        self.assertException( badargs )


    def testSpuriousCharacter(self):
        """Spurious character"""
        badargs = [ "prefix.[4.7].suffix","prefix.[4x7].suffix",  "prefix.[4X7].suffix", "prefix.[4$7].suffix",
                    "prefix.[4%7].suffix", "prefix.[4#7].suffix" ]
        self.assertException( badargs )


    def testMissingListEntryBegin(self):
        """Missing number begin [,5]"""
        badargs = [ "prefix.[,5].suffix", "prefix.[,5,7].suffix", "prefix.[,5,7-8].suffix" ]
        self.assertException( badargs ) 


    def testMissingListEntryEnd(self):
        """Missing number end [8,]"""
        badargs = [ "prefix.[8,].suffix", "prefix.[6,8,].suffix", "prefix.[4-6,8,].suffix" ]
        self.assertException( badargs )


    def testMissingListEntryMiddle(self):
        """Missing number middle [7,,8]"""
        badargs = [ "prefix.[7,,8].suffix", "prefix.[7,8,,10].suffix", "prefix.[7,,9,10].suffix",
                    "prefix.[2-4,,8].suffix", "prefix.[7,,8-10].suffix" ]
        self.assertException( badargs )


    def testMissingRangeBegin(self):
        """Missing start number of range"""
        badargs = [ "prefix.[-8].suffix", "prefix.[-8].suffix", "prefix.[-8].suffix", "prefix.[-8,10].suffix",
                    "prefix.[4,-8].suffix", "prefix.[4,-8,10].suffix" ]
        self.assertException( badargs )


    def testMissingRangeEnd(self):
        """Missing end number of range"""
        badargs = [ "prefix.[8-].suffix", "prefix.[8-,10].suffix", "prefix.[4,8-].suffix",
                    "prefix.[4,8-,10].suffix", "prefix.[8-,10-12].suffix" ]
        self.assertException( badargs )


    def testRangeDoubleDash(self):
        """Double dash: --"""
        badargs = [ "prefix.[6--8].suffix", "prefix.[6--8,10].suffix", "prefix.[4,6--8].suffix", "prefix.[4,6--8,10].suffix" ]
        self.assertException( badargs )
        


    def testRangeWrongOrder(self):
        """Ranga has lower end then start"""
        badargs = [ "prefix.[8-7].suffix", "prefix.[8-06].suffix" ]
        self.assertException( badargs )



    def testNegativeNumbers(self):
        """Negative numbers"""
        badargs = [ "prefix.[-4].suffix", "prefix.[-4,6].suffix", "prefix.[6,-4].suffix", "prefix.[-4-7].suffix",
                    "prefix.[4--7].suffix", "prefix.[-7--4].suffix","prefix.[2,4--7].suffix","prefix.[4--7,2].suffix" ]   
        self.assertException( badargs )


    

if __name__ == "__main__":
    myunittest.main()

