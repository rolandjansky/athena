#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys,re,bz2,gzip

from math import sqrt
from math import atan2

class WrapperTruth:
    """
    Processes FTK Athena Wrapper output, primarily to get truth information
    Contact person: Joe Tuggle
    """
    def __init__(self,inputFileName=None,verbose=False):
        """
        inputFileName is the file that contains the list of wrapper
        files, e.g., ~/config/input/inputprod_raw_V1213INDEP.list
        verbose will print some output on the progress
        """
        # WrapperTruth internals
        self.verbose=verbose
        self.wrapperList=[]
        self.validTruth=False
        self.fileIdx=0
        self.curFile=None
        self.aLine=""
        # Event info
        self.eventnum=-1
        self.globalEvtNum=-1
        self.validEvent=False
        #Truth info
        self.currentTrueTrk=-1
        self.barcode=-1
        self.curv=0.0
        self.d0=0.0
        self.abd0=0.0
        self.z0=0.0
        self.phi=0.0
        self.ctheta=0.0
        
        if inputFileName is None:
            print "WrapperTruth: WARNING No input files specified."
        if type(inputFileName) is not str:
            print "WrapperTruth: WARNING inputFileName was not a string."
            inputFileName = None

        # Open the input file and make a list of the file names
        # No dCache support yet!
        try:
            f = open(inputFileName,'r')
        except:
            print "WrapperTruth: WARNING Problem opening",inputFileName
        else:
            for line in f:
                self.wrapperList.append(line.rstrip())
        if self.verbose is True:
            print "WrapperTruth: Loaded %s with %d wrapper files."%(inputFileName,len(self.wrapperList))

        # Set the current file
        try:
            if self.wrapperList[self.fileIdx].endswith('bz2'):
                self.curFile = bz2.BZ2File(self.wrapperList[self.fileIdx],'r')
            elif self.wrapperList[self.fileIdx].endswith('gz'):
                self.curFile = gzip.GzipFile(self.wrapperList[self.fileIdx],'r')
            else:
                self.curFile = open(self.wrapperList[self.fileIdx],'r')
        except:
            print "WrapperTruth: WARNING Could not open",self.wrapperList[self.fileIdx]
        else:
            if self.verbose:
                print "WrapperTruth: Opened",self.wrapperList[self.fileIdx] 

    def __str__(self):
        """ stringify operator so that the object can be printed via print statement """
        result="curv %f d0 %f z0 %f ctheta %f phi %f barcode %d"%(self.curv,self.d0,self.z0,self.ctheta,self.phi,self.barcode)
        return result

    def nextline(self):
        """
        Intended only for internal use. Returns the next line of the
        wrapper file, loading more files as necessary.
        """
        try:
            self.aLine = self.curFile.next()
        except:
            self.aLine = ""
        if( self.aLine == "" ): # We've reached the end of the file
            # Open the next one
            if self.nextfile():
                try:
                    self.aLine = self.curFile.next()
                except:
                    self.aLine = ""
                if self.aLine == "":
                    print "WrapperTruth: WARNING %s is empty. Bailing out."%self.wrapperList[self.fileIdx]
                    return ""
            else:
                return ""

        # Now we have a valid file and a line is loaded
        return self.aLine
    
    def nextevent(self):
        """
        Gets the next event from the wrapper files.
        Return value is whether the command completed successfully.
        """
        self.validEvent=False
        self.currentTrueTrk=-1
        self.aLine = self.nextline()
        if self.aLine == "":
            return self.validEvent
        
        # Now we have a valid file and a line is loaded
        while True:
            while re.match( 'F', self.aLine ) is None:
                self.aLine = self.nextline()
                if self.aLine == "":
                    break
            splitline = self.aLine.split()
            if len(splitline) < 2:
                # File done or problem reading
                self.validEvent=False
                return self.validEvent
            try:
                self.eventnum = int(splitline[1])
            except:
                # This would be weird. We have an F but no evt number.
                self.eventnum = -1
            self.globalEvtNum = self.globalEvtNum + 1
            if self.verbose:
                if self.globalEvtNum%1000 == 0:
                    print "Global event %d, local %d"%(self.globalEvtNum,self.eventnum)
            break

        # Now we have a valid event
        self.validEvent=True
        return self.validEvent

    def nexttruth(self):
        """
        Gets the next truth track in the current event
        Return value is the truth index in the event. -1 if no good.
        """

        if not self.validEvent:
            return -1
        self.aLine = self.nextline()
        while re.match( 'T', self.aLine ) is None:
            if re.match( 'L', self.aLine ):
                # The event ended!
                self.validEvent=False
                return -1
            self.aLine = self.nextline()
            if self.aLine == "":
                # Unexpected end of file
                self.validEvent=False
                return -1

        # Now we have a true track
        self.currentTrueTrk = self.currentTrueTrk + 1
        trueparams = self.aLine.split()
        x = float(trueparams[1])
        y = float(trueparams[2])
        self.z0 = float(trueparams[3])
        charge = int(trueparams[4])
        px = float(trueparams[5])
        py = float(trueparams[6])
        pz = float(trueparams[7])
        self.barcode = int(trueparams[12])

        # Since we're dealing with tracks here, go to the next true
        # index if this one isn't a track
        if charge == 0:
            return self.nexttruth()
        x0 = x - charge*py
        y0 = y + charge*px
        self.curv = charge*1.0 / sqrt(px*px+py*py)
        self.d0 = sqrt(x0*x0+y0*y0)*abs(charge)/charge - 1.0/self.curv
        blah = y*px - x*py
        d0sign = 0
        if blah > 0:
            d0sign = 1
        else:
            d0sign = -1
        self.abd0 = sqrt(x*x+y*y)*d0sign
        self.curv = self.curv/2.0 # redifine curvature as semi-curvature
        self.ctheta = pz/sqrt(px*px+py*py);
        self.phi = atan2(-x0*abs(charge)/charge,y0*abs(charge)/charge);
        return self.currentTrueTrk 
    
    def nextfile(self):
        """
        Gets the next file in the series
        Returns whether it succeeded
        """
        if self.fileIdx < (len(self.wrapperList)-1):
            self.curFile.close()
            self.fileIdx = self.fileIdx + 1
            try:
                if self.wrapperList[self.fileIdx].endswith('bz2'):
                    self.curFile = bz2.BZ2File( self.wrapperList[self.fileIdx],'r')
                elif self.wrapperList[self.fileIdx].endswith('gz'):
                    self.curFile = gzip.GzipFile(self.wrapperList[self.fileIdx],'r')
                else:
                    self.curFile = open(self.wrapperList[self.fileIdx],'r')
            except:
                print "WrapperTruth: WARNING Could not open",self.wrapperList[self.fileIdx]
                return False
            else:
                if self.verbose:
                    print "WrapperTruth: Opened",self.wrapperList[self.fileIdx]

        else: # We're already at the end
            self.curFile.close()
            return False

        return True
