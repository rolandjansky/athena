#!/bin/env python

##
# @file RandomDozerApp.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
# @brief example CoolDozer application

import sys
sys.argv.append("-b")


dozerPath =  "/afs/cern.ch/atlas/project/RTT/Results/dozer/api/test" 
sys.path = [ dozerPath ] + sys.path
sys.path.append( dozerPath )

from CoolDozer import *
import random

##
# @class RandomDozerApp
# @author Krzysztof Daniel Cibe (Krzysztof.Ciba@NOSPAMagh.edu.pl)
# @brief example application using CoolDozer API for Atlas RTT test framework 
class RandomDozerApp( object ):
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param argDict arguments dictionary
    def __init__( self , argDict = {} ):
        
        self.log = DozerLogger( "RandomDozerApp.log", "RandomDozerApp", toConsole=True )
        self.log = self.log.getLogger( self )
        self.log.toConsole( True )
        print self.log.getFileName()
 
        self.log.info("*** Hi ***")

        self.dry = bool( argDict.get("DryRun", None ) )
        if ( not self.dry ): self.dry = True
 
        self.log.info("getting RTT JobDescriptior from arguments dictionary")
        jobDtor = argDict.get("JobDescriptor", None )
        if ( jobDtor != None ):
            self.log.info("JobDescriptior has been found")

            self.package = jobDtor.package
            self.outputPath = jobDtor.runPath

            self.nightly = jobDtor.paths.branch
            self.install = jobDtor.paths.runType
            self.cmtconfig = jobDtor.paths.cmtCondig
            self.project = jobDtor.paths.topProject
            self.release = jobDtor.paths.release
            self.jobId = "job_1"

        else:
            self.package = "DozerEx"
            self.outputPath = "."
            self.nightly = "bugfix"
            self.install = "build"
            self.cmtconfig = "i686-slc4-gcc34-opt"
            self.project = "offline"
            self.release = "rel_0"
            self.jobId = "job_1"

        self.dozerSchemaConfig = DozerSchemaConfig( self.nightly,
                                                    self.install,
                                                    self.cmtconfig,
                                                    self.project, self.jobId )

        self.rtSchemaPath = self.nightly + "/" + self.install + "/" + self.cmtconfig + "/" + self.project  + "/" + self.jobId
        self.log.info("will use runtime schema path = %s" % self.rtSchemaPath )

        self.db = DozerDB( self.package, "RTT", self.dry, self.log )


    def run( self ):
        
        # writting 
        try:
            isOpened = self.db.openDB() 
        except DozerError, value:
            self.log.epanic( value )
        
        self.rtSchema = None
        if ( isOpened ):

            try:
                self.rtSchema = self.db.getSchema( self.rtSchemaPath )
            except DozerError, value:
                self.log.info("schema hasn't been added to the database yet!")

                            
            # run schemator and get schema from it
            if ( not self.rtSchema ):
                self.schemator()
                self.rtSchema = self.rConfig.get( self.rtSchemaPath )
        
            if ( not self.rtSchema ):
                self.log.panic("no schema for config = '%s' found in db nor self.schemator()!")
                sys.exit(0)
            else:
                self.db += self.rtSchema  
            
            ## write to db
            self.writer() 
        
            ## read data back and make some plots
            self.reader()

            self.db.closeDB()

        pass

    ## creates dummy DozerConfig for RTT
    # @param self "Me, myself and Irene"
    def schemator( self ):
        
        self.rConfig = DozerConfig( "RTT",  self.log )
           
        rChannel = DozerChannel("dummyRandom", "dummy random channel", log=self.log)
       
        rChannel += DozerData("dummy random int", "Int", "dummy unit count", log=self.log )
        rChannel += DozerData("dummy random float", "Float", "dummy float count", log=self.log )

        rSchema = DozerSchema("*", "*", "*", "*",  "*", log=self.log)

        rSchema += rChannel
        
        self.rConfig += rSchema 


    def writer( self ):
        
        if ( "dummyRandom" in self.rtSchema ):
            chRandom = self.rtSchema["dummyRandom"]
            if ( "dummy random int" in chRandom ):
                chRandom["dummy random int"].setValue( random.randrange(100, 110) )
            if ( "dummy random float" in chRandom ):
                chRandom["dummy random float"].setValue( random.gauss( 115.0, 3.0 ) ) 
        
        iov = DozerIOV( log=self.log )
        todayAsRelease = iov.AsRelease()
        since = iov.today()
        until = iov.max() 

        
        if ( "rel_" in self.release ):

            if ( todayAsRelease != self.release ):
                
                since = iov.fromRelease( self.release )
                
                until = since.nextDay()
                since = since()
                
        
        if ( self.db and self.db.isOpened() ):
            self.db.addSchemaPayload( self.rtSchema, since , until )


    ## 
    # @param self "Me, myself and Irene"
    def reader( self ):

        recordSet = None
        if ( "dummyRandom" in self.rtSchema ):
            recordSet = self.db.get( self.db.pathChunk() + "/" + self.rtSchema.pathChunk() + "/" + "dummyRandom" )

            
        if ( recordSet ):

            graph = recordSet.getDozerGraph() 

            graph.trend( "dummy random int", False, "trend_testRandomInt.png")
            graph.histogram( "dummy random int", False, "histo_testRandomInt.png")
           

            graph.trend( "dummy random float", False, "trend_testRandomFloat.png")
            graph.histogram( "dummy random float", False, "histo_testRandomFloat.png")
            


        
## dummy test running 
if __name__ == "__main__":
      print "dummy running RandomDozerApp..."
      testCfg = { "JobDescriptor"    : None,
                  "DryRun"           : True }
      dozerApp = RandomDozerApp( testCfg )
      dozerApp.run()
