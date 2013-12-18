# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest,tempfile
import sys,os

__all__ = [ ]

debug = False
fake = False

def main():
    # remove options that unittest.main does not like
    myopts = [ '-d', '-n' ]
    testargv = [ arg for arg in sys.argv if not arg in myopts ]
    unittest.main( argv=testargv, testRunner=unittest.TextTestRunner(verbosity=2) )


class TestCase(unittest.TestCase):
    def shortDescription(self):
        return '%s: %s' % (self.__class__.__name__, unittest.TestCase.shortDescription(self))


    def setUp(self):
        """Prepare for temporary files"""
        # handle some command line options
        if '-d' in sys.argv:
            self.debug = True
        else:
            self.debug = False
        if '-n' in sys.argv:
            self.fake = True
        else:
            self.fake = False

        # prepare for temporary files
        self.startupdir = os.getcwd()
        self.tmpdir = tempfile.mkdtemp()


    def tearDown(self):
        """Clean up temporary files and go back to startup directory"""
        if not hasattr(self,'tmpdir'): return
        self.recursiveTmpRemove()
        if os.path.isdir(self.tmpdir):
            if self.debug: print "Removing directory %s" % self.tmpdir
            os.rmdir(self.tmpdir)
        os.chdir(self.startupdir)


    def makeTmpDir(self,d=None):
        """Create a temporary directory, with an optional subdirectory d.
        Returns the full path of the directory."""
        # no absolute paths
        if d and os.path.isabs(d): raise IOError('Absolute path not allowed for temporary files')
        if not d or d == os.curdir: return self.tmpdir
        # create temporary subdir
        fulldir = os.path.join(self.tmpdir,d)
        if not os.path.isdir(fulldir):
            if self.debug: print "Creating temporary directory %s" % fulldir
            if not self.fake: os.makedirs(fulldir)

        return fulldir


    def recursiveTmpRemove(self,fileOrDir=None):
        if not self.tmpdir or not os.path.isdir(self.tmpdir): return
        if fileOrDir is None: fileOrDir = self.tmpdir
        if fileOrDir.startswith(self.tmpdir):
            full = fileOrDir
        else:
            if os.path.isabs(fileOrDir): raise IOError('Absolute path not allowed for temporary files')
            full = os.path.join(self.tmpdir,fileOrDir)    

        if os.path.isdir(full):
            for f in os.listdir(full): self.recursiveTmpRemove(os.path.join(full,f))
            if self.debug: print "Removing directory %s" % full
            if not self.fake: os.rmdir(full)
        else:
            if self.debug: print "Removing file %s" % full
            if not self.fake: os.remove(full)



    def removeTmpDir(self,d):
        """Default: remove all tmp dirs"""
        if d and os.path.isabs(d): raise IOError('Absolute path not allowed for temporary files')
        if d != os.curdir:
            fulldir = os.path.join(self.tmpdir, d)
            if os.path.isdir(fulldir):
                if self.debug: print "Removing temporary directory %s" % fulldir
                if not self.fake: os.rmdir(fulldir)


    def cdTmpDir(self,d=None):
        """Go to the subdirectory in the temporary area"""
        if d and d != os.curdir:
            fulldir = os.path.join( self.tmpdir, d )
        else:
            fulldir = self.tmpdir
            
        os.chdir(fulldir)


    def fullTmpFile(self,filename):
        if os.path.isabs(filename): raise IOError('Absolute path not allowed for temporary files')
        return os.path.join( self.tmpdir, filename )
        

    def createTmpFile(self,filename):
        """Create a dummy file in the temporary directory. Returns full path of the file."""
        if os.path.isabs(filename): raise IOError('Absolute path not allowed for temporary files')
        dirname = os.path.dirname( filename )
        self.makeTmpDir(dirname)
        fullpath = os.path.join( self.tmpdir, filename )
        if not os.path.exists(fullpath):
            if self.debug: print "Creating temporary file %s" % fullpath
            if not self.fake:
                nf = open(fullpath,'w')
                nf.write( filename )
                nf.close()
        return fullpath

    

    def removeTmpFiles(self,files):
        if not files: return
        # for compatibility with single filename
        if type(files) == type(''): files = [ files ]
        for f in files:
            fullfile = os.path.join( self.tmpdir, f )
            if os.path.isfile(fullfile):
                if self.debug: print "Removing temporary file %s" % fullfile
                if not self.fake: os.remove(fullfile)


    def recursiveListTmpFiles(self,dir=None,indent=''):
        if dir is None: dir = self.tmpdir
        if indent == '':
            line = dir + os.sep
        else:
            line = indent + os.path.basename(dir) + os.sep
        print line
        newindent = ' '*(len(line)-1)
        for f in os.listdir(dir):
            full = os.path.join(dir,f)
            if os.path.isdir(full):
                self.recursiveListTmpFiles(full,newindent)
            else:
                print newindent + f
        

    def listTmpFiles(self,title):
        # move starting newline to beginning of line
        if title.startswith(os.linesep):
            mess = "\n%s temporary files %s:" % (self.__class__.__name__, title[1:])
        else:
            mess = "%s temporary files %s:" % (self.__class__.__name__, title)
        print mess
        if not self.tmpdir or not os.path.isdir(self.tmpdir): return
        self.recursiveListTmpFiles()


       
        

