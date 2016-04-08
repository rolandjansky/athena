#!/bin/env python
##
# @file RTTRandomDozerApp.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief example class for simplified RTT-CoolDozer long term monitoring
#

import sys
sys.argv.append("-b")

sys.path = [ "/afs/cern.ch/atlas/project/RTT/prod/Results/dozer/api/latest" ] + sys.path

sys.rtt = True

from CoolDozer import *
import random
import logging

##
# @class RTTRandomDozerApp::RTTRandomDozerApp
# @extends python::RTTDozerApp::RTTDozerApp
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test class for simplified RTT-CoolDozer monitoring
class RTTRandomDozerApp( RTTDozerApp ):

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param argDict required by RTT
    def __init__( self, argDict = {}):        
        ## call base class constructor
        super( RTTRandomDozerApp, self ).__init__( argDict )
        
    ## DozerSchema creator
    # @param self "Me, myself and Irene"
    # @return DozerSchema instance
    def schema( self ):
        self.info("will construct generic DozerSchema")

        schema = DozerSchema()
        infoChannel = DozerChannel( "package info", "holds SVN tag")
        tagData = DozerData("svn tag", "String" )
        infoChannel += tagData

        rndChannel = DozerChannel( "random channel", "name says it all")
        rndInt = DozerData("rndInt", "Int", "N" )
        rndIntErr = DozerData("rndInt;error", "Int", "N")
        rndDouble = DozerData("rndDouble", "Double", "GeV" )

        rndChannel += rndInt
        rndChannel += rndIntErr
        rndChannel += rndDouble
        rndChannel += DozerData("rndDouble;uperror", "Double", "GeV" )
        rndChannel += DozerData("rndDouble;downerror", "Double", "GeV" )

        newChannel = DozerChannel("my new channel", "new channel description")
        newChannel += DozerData( "my name", "String" )
        newChannel += DozerData( "today" , "String")

        iChannel = DozerChannel("dates", "days of monitoring" )
        iChannel += DozerData("today", "String")
        iChannel += DozerData("stamp", "Long")
        
        schema += infoChannel
        schema += rndChannel
        schema += newChannel
        schema += iChannel



        return schema

    ## writing a new record to db - filling DozerSchema 
    # @param self "Me, myself and Irene"
    # @return None
    def writer( self ):
        self.dozerSchema["package info"]["svn tag"].setValue( str(self.packageTag) )
        self.dozerSchema["random channel"]["rndInt"].setValue( random.randint( 100,130 ) )
        self.dozerSchema["random channel"]["rndInt;error"].setValue( random.randint( 10,20 ) )
        self.dozerSchema["random channel"]["rndDouble"].setValue( random.uniform( 100, 130 ) )
        self.dozerSchema["random channel"]["rndDouble;uperror"].setValue( random.uniform( 15,20 ) )
        self.dozerSchema["random channel"]["rndDouble;downerror"].setValue( random.uniform( 5,10 ) )
        self.dozerSchema["my new channel"]["my name"].setValue( "Krzysztof Ciba" )
        self.dozerSchema["my new channel"]["today"].setValue( DozerIOV.today().AsISO() )
        self.dozerSchema["dates"]["today"].setValue( DozerIOV.today().AsISO() )
        self.dozerSchema["dates"]["stamp"].setValue( DozerIOV.today().AsUnix()  )


## dummy standalone run outside RTT
if __name__ == "__main__":
    print "Dummy test running..."

    ##
    # @class Paths
    # @brief fake RTT Paths
    class Paths:
        def __init__(self):
            self.release    = "rel_1"
            self.branch     = "dev"
            self.runType    = "build"
            self.cmtConfig  = "i686-slc4-gcc34-opt"
            self.topProject = "AtlasProduction"
            self.packageTag = "CoolDozer-00-00-35"
    ##
    # @class Desc
    # @brief fake RTT JobDescriptor for testing
    class Desc:
        def __init__(self):
            self.package    = "CoolDozer"
            self.trendId    = "testJob_evo"
            self.runPath    = "."
            self.paths      = Paths()
            self.log        = "dummy.log"
               
    testCfg = { "JobDescriptor"    : Desc(),
                "LastNDays"        : 60,
                "Fit"              : True, 
                "Histos"           : True, 
                "DryRun"           : True,
                "Style"            : "Seagull" }

    rttRandomDozerApp = RTTRandomDozerApp( testCfg )
    rttRandomDozerApp.run()
   
