#!/bin/env python
##
# @file RandomDozerAppStandalone.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
# @brief example CoolDozer application

import sys
sys.argv.append("-b")

sys.rtt = True

from CoolDozer import *
import random
##
# @class RandomDozerAppStandalone::RandomDozerAppStandalone
# @author Krzysztof Daniel Cibe (Krzysztof.Ciba@NOSPAMagh.edu.pl)
# @brief example application using CoolDozer API for Atlas RTT test framework 
class RandomDozerAppStandalone( object ):
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param argDict arguments dictionary
    def __init__( self , argDict = {} ):
        
        self.log = DozerLogger( "RandomDozerAppStandalone.log", "RandomDozerAppStandalone", toConsole=True )
        self.log = self.log.getLogger( self )
        self.log.toConsole( True )
        print self.log.getFileName()
 
        self.log.info("*** Hi ***")

        self.dry = False
        self.log.info("getting RTT JobDescriptior from arguments dictionary")
        jobDtor = argDict.get("JobDescriptor", None )

        if ( jobDtor != None ):
            self.log.info("JobDescriptior has been found")
            self.package    = jobDtor.package
            self.outputPath = jobDtor.runPath
            self.nightly    = jobDtor.paths.branch # nightly
            self.install    = jobDtor.paths.runType # run type 
            self.cmtconfig  = jobDtor.paths.cmtConfig # cmt config value
            self.project    = jobDtor.paths.topProject # project name
            self.release    = jobDtor.paths.release # current release
            self.jobId      = jobDtor.trendId # jobId
        else:
            self.package = "CoolDozerTEST_09"
            self.outputPath = "."
            self.nightly = "dev"
            self.install = "build"
            self.cmtconfig = "i686-slc4-gcc34-opt"
            self.project = "AtlasProduction"
            self.release = "rel_0"
            self.jobId = "job_2"

        self.dozerSchemaConfig = DozerSchemaConfig( self.nightly,
                                                    self.install,
                                                    self.cmtconfig,
                                                    self.project,
                                                    self.jobId )

        self.rtSchemaPath = self.nightly + "/" + self.install + "/" + self.cmtconfig + "/" + self.project  + "/" + self.jobId
        self.log.info("will use runtime schema path = %s" % self.rtSchemaPath )

        # create DozerDB instance
        self.db = DozerDB( self.package, "RTT", False, "PROD", self.log )


    ## Run Forest! Run! 
    # @param self "Me, myself and Irene"
    def run( self ):
        
        ## opening DB
        try:
            isOpened = self.db.openDB() 
        except DozerError, value:
            self.log.epanic( value )
            return 1
        
        self.rtSchema = None
        if ( isOpened ):
            if ( not self.rtSchema ):
                self.log.info("getting schema from DozerConfig instance using schemator() ")
                self.schemator()
                self.rtSchema = self.rConfig.get( self.rtSchemaPath )
        
            if ( not self.rtSchema ):
                self.log.panic("no schema for config = '%s' found in db nor self.schemator()!")
                return 1
            
            ## write to db
            self.writer() 
        
            ## read data back and make some plots
            self.reader()

            ## close DB
            self.db.closeDB()
            return 0
        return 1
        

    ## creates dummy DozerConfig for RTT
    # @param self "Me, myself and Irene"
    def schemator( self ):
        
        ## DozerConfig instance for RTT framework
        self.rConfig = DozerConfig( "RTT",  self.log )
           
        ## DozerChannel for storing dummy data
        rChannel = DozerChannel("dummyRandom", "dummy random channel")
       
        ## adding dummy int and float data to the channel
        rChannel += DozerData("dummy random int", "Int", "dummy unit count" )
        rChannel += DozerData("dummy random int;error", "Int", "dummy unit count" )
        rChannel += DozerData("dummy random float", "Float", "dummy float count")
        rChannel += DozerData("dummy random float;uperror", "Float", "dummy float count")
        rChannel += DozerData("dummy random float;downerror", "Float", "dummy float count")

        ## the most generic schema
        rSchema = DozerSchema("*", "*", "*", "*", "*" )

        ## adding dummy DozerChannel to DozerSchema
        rSchema += rChannel
        
        ## adding generic schema to the DozerConfig
        self.rConfig += rSchema 

    ## db writer
    # @param self "me, myself and Irene"
    # @brief storing dummy random numbers in the db
    def writer( self ):

        ## check if dummyRandom channel is in schema
        if ( "dummyRandom" in self.rtSchema ):
            
            ## get the channel
            chRandom = self.rtSchema["dummyRandom"]
            
            ## set the values
            if ( "dummy random int" in chRandom ):
                chRandom["dummy random int"].setValue( random.randrange(100, 110) )
            if ( "dummy random float" in chRandom ):
                chRandom["dummy random float"].setValue( random.gauss( 115.0, 3.0 ) ) 
            if (  "dummy random int;error" in chRandom ):
                chRandom["dummy random int;error"].setValue( random.randrange(5, 10) )
            if ( "dummy random float;uperror" in chRandom ):
                chRandom["dummy random float;uperror"].setValue( random.randrange( 5, 10 ) )
            if ( "dummy random float" in chRandom ):
                chRandom["dummy random float;downerror"].setValue( random.randrange( 5, 10 ) ) 
            
        
        iov = DozerIOV( )
        since = iov.today()
        until = iov.max() 
                
        ## check is db is opened, then store schema in it
        #if ( self.db and self.db.isOpened() ):
        self.db.addSchemaPayload( self.rtSchema, since, until )
            

    ## db reader and plotter
    # @param self "Me, myself and Irene"
    def reader( self ):

        ## this will be DozerRecordSet
        recordSet = None
        
        ## get the RecordSet made of "dummyRandom" DozerChannel instances  
        if ( "dummyRandom" in self.rtSchema ):
            recordSet = self.db.get( self.db.pathChunk() + "/" + self.rtSchema.pathChunk() + "/" + "dummyRandom", since=DozerIOV().lastNDays(30) )
            
            for dates in recordSet:
                since = DozerIOV(dates[0]).AsISO()
                until = DozerIOV(dates[1]).AsISO()
                self.log.info( "since=" + since + " until=" + until + " channel=" + str(recordSet[dates]) )

            ## get the grapher 
            graph = recordSet.getDozerGraph() 
            self.log.info( str(graph.plotables()) )

            ## make trend and histo plots for "dummy random int" DozerData
            graph.trend( "dummy random int", False, "trend_testRandomInt.png")
            graph.histogram( "dummy random int", False, "histo_testRandomInt.png")
           
            ## make trend and histo plots for "dummy random float" DozerData
            graph.trend( "dummy random float", False, "trend_testRandomFloat.png")
            graph.histogram( "dummy random float", False, "histo_testRandomFloat.png")
            


        
## dummy test running 
if __name__ == "__main__":
      print "dummy running RandomDozerApp..."
      testCfg = { "JobDescriptor"    : None,
                  "DryRun"           : False }
      dozerApp = RandomDozerAppStandalone( testCfg )
      dozerApp.run()
