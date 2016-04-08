#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Merge several input pool files into a single output file."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf

from subprocess import Popen, PIPE, STDOUT, check_call, CalledProcessError

class MergePoolJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        JobTransform.__init__(self,
                              authors = [ Author('David Cote', 'david.cote@cern.ch') ] ,
                              skeleton='PATJobTransforms/skeleton.MergePool.py' ,
                              help = __doc__ ,
                              lastInChain=lastInChain)

        if not isinstance(inDic,dict):
            raise TypeError("inDic has %s but should be a dictionary." %type(inDic))


        from PATJobTransforms.ConfigDicUtils import AutoConfigureFromDic
        self.inDic=inDic
        # Look to see if we should use the fast hybrid POOL merger (default)
        if self.inDic.get('fastPoolMerge', 'true').lower() == 'true':
            print "Using hybrid merge - will skip all events with athena and merge later."
            # This needs to become a run option, so set it early
            self.inDic['skipEvents'] = 10000000
            # This must be really set to trigger the correct JO fragments for fast merge 
            self.inDic['fastPoolMerge'] = 'true'
        AutoConfigureFromDic(self,inDic)
                
        self._addPostRunAction(self, prepend=True)


    def postRunAction(self):
        # Run fast merge as a post run action to the main transform
        print "Executing postRunActions for MergePoolJobTransform"
        if self.inDic.get('fastPoolMerge', 'true').lower() == 'true':
            print "Now doing hybrid event merge"
            self.fastMerge()


    def fastMerge(self):
        filelist = []
        outputfile = self._outputFiles[0].value()
        for file in self._inputFiles:
            if file:
                value = file.value()
                if type(value).__name__ == 'list':
                    filelist += value
            else:
                filelist.append(value)
        print "Files to Merge: %s" % filelist

        # First run mergePOOL.exe to get events.pool
        cmd = ['mergePOOL.exe', '-o', 'events.pool.root']
        for file in filelist:
            cmd.extend(['-i', file])
        cmd.extend(['-e', 'MetaData', '-e', 'MetaDataHdrDataHeaderForm', '-e', 'MetaDataHdrDataHeader', '-e', 'MetaDataHdr'])
        print "Will execute hybrid merge step 1: %s" % cmd

        p = Popen(cmd, stdout=PIPE, stderr=STDOUT, close_fds=True)
        while p.poll() is None:
            line = p.stdout.readline()
            if line:
                print "mergePOOL.exe Report: %s" % line.strip()
        rc = p.returncode
        print "1st mergePOOL (event data) finished with code %s" % rc
        if rc == 1:
            print "mergePOOL.exe finished with unknown status (upgrade your RootFileTools to a newer version)"
        elif rc != 0:
            raise TransformError("mergePOOL.exe (event merge) encountered a problem",error='TRF_MERGEERR') 

        # Second merge with metadata.pool to produce final output
        cmd = ['mergePOOL.exe', '-o', 'events.pool.root', '-i', outputfile]
        print "Will execute hybrid merge step 2: %s" % cmd
        
        p = Popen(cmd, stdout=PIPE, stderr=STDOUT, close_fds=True)
        while p.poll() is None:
            line = p.stdout.readline()
            if line:
                print "mergePOOL.exe Report: %s" % line.strip()
        rc = p.returncode
        print "2nd mergePOOL (metadata) finished with code %s" % rc
        if rc == 1:
            print "mergePOOL.exe finished with unknown status (upgrade your RootFileTools to a newer version) - assuming all is ok"
        elif rc != 0:
            raise TransformError("mergePOOL.exe (final merge) encountered a problem",error='TRF_MERGEERR') 

        # Finish hybrid merge by moving the full file to the final output location
        shutil.move('events.pool.root', outputfile)

        # Now fix the metadata, which has been left by POOL as the _stub_ file's metadata
        # so it has the wrong GUID in the PFC
        print 'Now fixing metadata in PFC for %s' % outputfile
        try:
            check_call(['FCdeletePFN', '-p', outputfile])
            correctGUID = None
            p = Popen(['pool_extractFileIdentifier.py', outputfile], stdout=PIPE, stderr=STDOUT, close_fds=True, bufsize=1)
            while p.poll() is None:
                line = p.stdout.readline()
                words = line.split()
                if len(words) >= 2 and outputfile in words[1]:
                    correctGUID = words[0]
            if correctGUID == None or p.returncode != 0:
                raise TransformError("pool_extractFileIdentifier.py failed to get merged file GUID", error='TRF_MERGEERR')
            print 'GUID is %s' % correctGUID
            check_call(['FCregisterPFN', '-p', outputfile, '-t', 'ROOT_All', '-g', correctGUID]) 
        except CalledProcessError, e:
            print 'Attempt to fix PFC with new merged file information failed: %s' % e


    def matchEvents(self):
        # Switch between ESD and AOD merging 
        if 'outputAODFile' in self.inDic:
            return self.matchEventsExpectEqual("inputAODFile","outputAODFile")
        if 'outputESDFile' in self.inDic:
            return self.matchEventsExpectEqual("inputESDFile","outputESDFile")


# Python executable
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = MergePoolJobTransform(inDic)          
    sys.exit(trf.exeArgDict(inDic).exitCode())
