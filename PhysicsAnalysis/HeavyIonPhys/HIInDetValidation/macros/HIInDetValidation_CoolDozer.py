# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Based on RTTRandomDozerApp by K. Ciba

import sys
sys.argv.append("-b")
sys.rtt = True

from CoolDozer import *
import logging

class HIInDetValidation_CoolDozer( RTTDozerApp ):

    def __init__( self, argDict = {}):        
        ## call base class constructor
        RTTDozerApp.__init__(self, argDict)
        #super( RTTDozerApp, self ).__init__( argDict )
        self.algoNames = ("Track_Loose", "Track_Tight");

    def schema( self ):
        self.info("will construct generic DozerSchema")

        schema = DozerSchema()

        effChannel = DozerChannel( "efficiency", "efficiency of tracks reconstruction")
        for algo in self.algoNames:
            effChannel += DozerData(algo, "Double", "%" )
            effChannel += DozerData(algo+";error", "Double", "%" )
        schema += effChannel

        fakeChannel = DozerChannel( "fakerate", "fake rate of tracks reconstruction")
        for algo in self.algoNames:
            fakeChannel += DozerData(algo, "Double", "%" )
            fakeChannel += DozerData(algo+";error", "Double", "%" )
        schema += fakeChannel

        #resChannel = DozerChannel( "resolution", "resolutions of algorithms")
        #for algo in self.algoNames:
        #    resChannel += DozerData(algo, "Double", "%" )
        #schema += resChannel

        return schema

    def writer( self ):
        import glob

        #get truth efficiency
        filelista = glob.glob("efficiency_HIInDetValidation.txt")
        if ( len(filelista) == 0 ):
            return
        infile = file(filelista[0])

        while infile:
            line = infile.readline()
            if (line == ''):
                break
            if (line.find('eff') > -1):
                words = line.split()
                for algo in self.algoNames:
                    if (words[0] == algo):
                        self.dozerSchema["efficiency"][algo].setValue( float(words[3]) )
                        self.dozerSchema["efficiency"][algo+";error"].setValue( float(words[6]) )

        #fake ratios
        filelistb = glob.glob("fakes_HIInDetValidation.txt")
        if ( len(filelistb) == 0 ):
            return
        infileb = file(filelistb[0])

        while infileb:
            lineb = infileb.readline()
            if (lineb == ''):
                break                        
            if (lineb.find('fake') > -1):
                wordsb = lineb.split()
                for algo in self.algoNames:
                    if (wordsb[0] == algo):
                        self.dozerSchema["fakerate"][algo].setValue( float(wordsb[4]) )
                        self.dozerSchema["fakerate"][algo+";error"].setValue( float(wordsb[7]) )

## dummy standalone run outside RTT
if __name__ == "__main__":
    print "Dummy test running..."

    ##
    # @class Paths
    # @brief fake RTT Paths
    class Paths:
        def __init__(self):
            self.release    = "rel_0"
            self.branch     = "devval"
            self.runType    = "build"
            self.cmtConfig  = "i686-slc4-gcc34-opt"
            self.topProject = "AtlasAnalysis"
            self.packageTag = "HIInDetValidation-00-00-05"
    ##
    # @class Desc
    # @brief fake RTT JobDescriptor for testing
    class Desc:
        def __init__(self):
            self.package    = "HIInDetValidation"
            self.trendId    = "testJob"
            self.runPath    = "."
            self.paths      = Paths()
            self.log        = "dummy.log"

    testCfg = { "JobDescriptor"    : Desc(),
                "LastNDays"        : 30,
                "Fit"              : True, 
                "Histos"           : False, 
                "DryRun"           : False,
                "Style"            : "Dozer" }

    testDozerApp = HIInDetValidation_CoolDozer( testCfg )
    testDozerApp.run()
