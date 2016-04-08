#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## 
# @file DozerRecordSet.py 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Implementation of DozerRecordSet class.
#

import xml.dom.minidom
from types import *
import unittest

import DozerObject
from DozerUtils import DozerLogger, DozerError
from DozerIOV import DozerIOV
from DozerChannel import DozerChannel
from DozerData import DozerData
from DozerGraph import DozerGraph, DozerStyle

## 
# @class DozerRecordSet
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Holder for CoolDozer database payload for one channel.
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
# <ul>
# <li> <h4>Constructing</h4>
#
# When database is opened (by calling python::DozerDB::DozerDB::openDB) you can get the payload by calling python::DozerDB::DozerDB::get 
# function specifying the full path to the database channel you want to read and two points in time (since and until). 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#Python 2.5.4 (r254:67916, Jan 26 2009, 14:34:54) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-10)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> from CoolDozer import *
#>>> db = DozerDB("CoolDozer", "RTT", False)
#>>> recordSet = db.get("/CoolDozer/dev/build/i686-slc4-gcc34-opt/AtlasProduction/rttRandomDozerApp_RTTRandomDozerApp/random channel", DozerIOV().lastNDays(2), DozerIOV.today())
#DozerDB     : DEBUG    path='/CoolDozer/dev/build/i686-slc4-gcc34-opt/AtlasProduction/rttRandomDozerApp_RTTRandomDozerApp/random channel' 
#normpath='/CoolDozer/dev/build/i686-slc4-gcc34-opt/AtlasProduction/rttRandomDozerApp_RTTRandomDozerApp/random channel'
#DozerDB     : DEBUG    will try to change directory to /CoolDozer/dev/build/i686-slc4-gcc34-opt/AtlasProduction/rttRandomDozerApp_RTTRandomDozerApp
#DozerDB     : DEBUG    channel 'random channel' found in path '/CoolDozer/dev/build/i686-slc4-gcc34-opt/AtlasProduction/rttRandomDozerApp_RTTRandomDozerApp'
#DozerDB     : DEBUG    channelId=2 since=2009-10-20 00:00:00 until=2009-10-22 00:00:00
#DozerRecordSet: INFO     constructing DozerRecordSet since='2009-10-20 00:00:00' until='2009-10-22 00:00:00'
#DozerStyle  : DEBUG    setting ROOT TStyle to Default
#DozerChannel: DEBUG    constructing DozerChannel name='random channel' desc='name says it all' uuid='becf3892-7005-43c9-988c-21bad813f168'
#DozerData   : DEBUG    constructing DozerData name='rndInt;error' type='Int' unit='N' mime=''
#DozerChannel: DEBUG    adding DozerData name='rndInt;error' to channel
#DozerData   : DEBUG    constructing DozerData name='rndInt' type='Int' unit='N' mime=''
#DozerChannel: DEBUG    adding DozerData name='rndInt' to channel
#DozerData   : DEBUG    constructing DozerData name='rndDouble' type='Double' unit='GeV' mime=''
#DozerChannel: DEBUG    adding DozerData name='rndDouble' to channel
#DozerData   : DEBUG    constructing DozerData name='rndDouble;downerror' type='Double' unit='GeV' mime=''
#DozerChannel: DEBUG    adding DozerData name='rndDouble;downerror' to channel
#DozerData   : DEBUG    constructing DozerData name='rndDouble;uperror' type='Double' unit='GeV' mime=''
#DozerChannel: DEBUG    adding DozerData name='rndDouble;uperror' to channel
#DozerRecordSet: DEBUG    extending 'until' timestamp to 2009-10-23 00:00:00
#DozerRecordSet: DEBUG    adding point in time ['2009-10-22 00:00:00', '2009-10-23 00:00:00') => channel name='random channel'
#>>> recordSet
#<CoolDozer.DozerRecordSet.DozerRecordSet object at 0xf53cc0ac>
#>>> print recordSet
#DozerRecordSet since=2009-10-20 00:00:00 until=2009-10-23 00:00:00 package=CoolDozer channel=random channel framework=RTT 
#nightly=dev install=build cmtconfig=i686-slc4-gcc34-opt project=AtlasProduction jobId=rttRandomDozerApp_RTTRandomDozerApp
#</pre></b>
#</div></div></div>
#
#<div><div class="dozerInfo"><div class="dozerInfoLabel">Notice</div><div class="dozerInfoText">
# You can read only one channel at a time. 
#</div></div></div>
#
# <li><h4> Looping </h4>
#
# DozerRecordSet has got full python dictionary interface, but with sorted keys. The key in this dictionary is a 2-items tuple (<b>since</b>, <b>until</b>) where
# <b>since</b> and <b>until</b> are longint Cool timestamps. The data behind this key are DozerChannel instances for which the above defined interval is valid.
# So, looping and retreiving of DozerData values is trivial: 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> for dates in recordSet: print "since= " + DozerIOV(dates[0]).AsISO() + " until=" + DozerIOV(dates[1]).AsISO() + " rndInt="+ str(recordSet[dates]["rndInt"].value())
#... 
#since= 2009-10-22 00:00:00 until=2009-10-23 00:00:00 rndInt=102
#</pre></b>
#</div></div></div>
#
# <li> <h4>Data plotting</h4>
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> plotter = recordSet.getDozerGraph()                                    # [1] constructing DozerGraph object
#DozerGraph  : DEBUG    adding data name='rndInt;error' type='Int' value='12' since='2009-10-22 00:00:00' to plotter 
#DozerGraph  : DEBUG    adding data name='rndInt' type='Int' value='102' since='2009-10-22 00:00:00' to plotter 
#DozerGraph  : DEBUG    adding data name='rndDouble' type='Double' value='103.008981253' since='2009-10-22 00:00:00' to plotter 
#DozerGraph  : DEBUG    adding data name='rndDouble;downerror' type='Double' value='5.28879160282' since='2009-10-22 00:00:00' to plotter 
#DozerGraph  : DEBUG    adding data name='rndDouble;uperror' type='Double' value='16.8931509211' since='2009-10-22 00:00:00' to plotter 
#>>> plotter.plotables()                                                    # [2] listing what from this channel could be prestened on plot
#['rndInt;error', 'rndInt', 'rndDouble', 'rndDouble;downerror', 'rndDouble;uperror']
#>>> plotter.trend("rndInt", True)                                          # [3] and now plotting of a trend
#DozerGraph  : DEBUG    will use DozerData 'rndInt;error' as +/- error
#DozerGraph  : DEBUG    will save TCanvas to file rndInt.png
#DozerStyle  : DEBUG    setting ROOT TStyle to Dozer
#Error in <TCanvas::Range>: illegal world coordinates range: x1=0.000000, y1=-58.285714, x2=0.000000, y2=233.142857
#Error in <TCanvas::RangeAxis>: illegal axis coordinates range: xmin=0.000000, ymin=0.000000, xmax=0.000000, ymax=204.000000
#DozerGraph  : DEBUG    point i=0 x=0 y=102 uerr=12 derr=12
#DozerGraph  : INFO     number of points 1
# FCN=0 FROM MIGRAD    STATUS=CONVERGED      24 CALLS          25 TOTAL
#                     EDM=0    STRATEGY= 1      ERR MATRIX NOT POS-DEF
#  EXT PARAMETER                APPROXIMATE        STEP         FIRST   
#  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
#   1  a            1.02000e+02   3.79663e+02   1.17186e-02   0.00000e+00
#   2  b            5.10000e+01   1.89831e+02   5.85934e-03   0.00000e+00
#DozerGraph  : INFO     number of points: 1
#Info in <TCanvas::Print>: png file rndInt.png has been created
#DozerGraph  : INFO     image rndInt.png has been created
#True
#>>> plotter.histogram("rndInt", True, "histogram.png")                      # [4] and histogram as well
#DozerStyle  : DEBUG    setting ROOT TStyle to Dozer
#DozerGraph  : DEBUG    bins 4
# FCN=0 FROM MIGRAD    STATUS=CONVERGED     123 CALLS         124 TOTAL
#                     EDM=0    STRATEGY= 1  ERROR MATRIX UNCERTAINTY 100.0 per cent
#  EXT PARAMETER                APPROXIMATE        STEP         FIRST   
#  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
#   1  N            1.00000e+00   1.00000e+00   4.88281e-04   0.00000e+00
#   2  #mu          1.02000e+02   1.41421e+00   0.00000e+00   0.00000e+00
#   3  #sigma       0.00000e+00   3.00000e-09   0.00000e+00   0.00000e+00
#Info in <TCanvas::Print>: png file histogram.png has been created
#DozerGraph  : INFO     image histogram.png has been created
#True
#</pre></b>
#</div></div></div>
#  
#  
# </ul>
#
class DozerRecordSet( DozerObject.DozerObject ):

    ## DozerChannel name 
    __channelName = None

    ## DozerChannel UUID
    __channelUUID = None

    ## begin DozerIOV 
    __since = None

    ## end DozerIOV
    __until = None
 
    ## points in time dictionary 
    __pointsInTime = { }

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param package a package name
    # @param framework test framework name
    # @param nightly a nighlty name
    # @param install install name
    # @param cmtconfig a CMTCONFIG value
    # @param project an Atlas project name
    # @param jobId job id
    # @param since cool time stamp
    # @param until cool time stamp
    # @param log DozerLogger instance
    def __init__( self, 
                  package, 
                  framework, 
                  nightly, 
                  install, 
                  cmtconfig, 
                  project, 
                  jobId,  
                  since, 
                  until,
                  log=None ):
        
        super( DozerRecordSet, self).__init__( log )

        self.__package = package
        self.__framework = framework
        self.__nightly = nightly
        self.__install = install
        self.__cmtconfig = cmtconfig
        self.__project = project
        self.__jobId = jobId

        if ( type(since) not in (LongType, IntType) ):
            raise DozerError("error constructing DozerRecordSet instance, parameter since should be integer or long")
        else:
            self.__since = DozerIOV( long( since ) )
            
        if ( type(until) not in (LongType, IntType) ):
            raise DozerError("error constructing DozerRecordSet instance, parameter until should be integer or long")
        else:
            self.__until = DozerIOV( long( until ) )
            

        self.info("constructing DozerRecordSet since='%s' until='%s'" % ( self.__since.AsISO(), 
                                                                          self.__until.AsISO() ) )
         
        self.__channel = None
        self.__dataNames = {}

        self.__style = "Default"
        self.__stylist = DozerStyle( self.__style )

        self.__graph = None 

        self.info( str(self) )

    ## DozerChannel name getter
    # @param self "Me, myself and Irene"
    def channel( self ):
        return self.__channel 

    ## package name getter
    # @param self "Me, myself and Irene"
    def package( self ):
        return self.__package

    ## test framework name getter
    # @param self "Me, myself and Irene"
    def framework( self ):
        return self.__framework

    ## nightly name getter
    # @param self "Me, myself and Irene"
    def nightly( self ):
        return self.__nightly

    ## install name getter
    # @param self "Me, myself and Irene"
    def install( self ):
        return self.__install

    ## CMTCONFIG getter
    # @param self "Me, myself and Irene"
    def cmtconfig( self ):
        return self.__cmtconfig

    ## project name getter
    # @param self "Me, myself and Irene"
    def project( self ):
        return self.__project

    ## job Id getter
    # @param self "Me, myself and Irene"    
    def jobId( self ):
        return self.__jobId

    ## DozerData names getter
    # @param self "Me, myself and Irene"
    # @return a list with DozerData names
    def dataNames( self ):
        return self.__dataNames.keys()


    ## += pointInTime operator 
    # @param self "Me, myself and Irene"
    # @param pointInTime tuple with two cool timestamps + DozerChannel instance
    def __iadd__( self, pointInTime ):

        self.debug("add point in time")

        if ( ( type(pointInTime) is  TupleType ) and
             ( len(pointInTime) == 3 ) ):

            since = until = channel = None

            if ( type(pointInTime[0]) is LongType ): 
                since = DozerIOV( pointInTime[0] )
                if ( self.__since > since ):
                    self.debug("extending 'since' timestamp to %s" % since.AsISO() )
                    self.__since = since

            if ( type(pointInTime[1]) is LongType ):
                until = DozerIOV( pointInTime[1] )
                if ( self.__until < until ):
                    self.debug("extending 'until' timestamp to %s" % until.AsISO() )
                    self.__until = until
                
            if ( type(pointInTime[2]) is DozerChannel ):
                channel = pointInTime[2] 
                
            if ( None not in ( since, until, channel ) ):
                if ( self.__channel == None ): self.__channel = channel.name()
                

                self.debug("adding point in time ['%s', '%s') => channel name='%s'" % ( since.AsISO(), 
                                                                                        until.AsISO(), 
                                                                                        channel.name() ) )
                
                for data in channel:
                    if ( data not in self.__dataNames.keys() ):
                        self.__dataNames[data] = channel[data].unit() 
           

                if ( ( since.AsCool(), until.AsCool() ) not in self.__pointsInTime.keys() ):
                    self.__pointsInTime[ (since.AsCool(), until.AsCool() ) ] = channel 
                else:
                    del self.__pointsInTime[ ( since.AsCool(), until.AsCool() ) ]
                    self.__pointsInTime[ (since.AsCool(), until.AsCool() ) ] = channel 
        else:
            self.error("wrong argument to += operator for DozerRecordSet, should be a tuple ( (long), (long), (DozerChannel) )")

        return self

    ## str( ) operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        out = "DozerRecordSet since=%s until=%s package=%s channel=%s " % ( self.__since.AsISO(), 
                                                                            self.__until.AsISO(),
                                                                            self.__package,
                                                                            self.__channel )
                                                                            
        out += "framework=%s nightly=%s install=%s cmtconfig=%s project=%s jobId=%s" % ( self.__framework,
                                                                                         self.__nightly,
                                                                                         self.__install,
                                                                                         self.__cmtconfig,
                                                                                         self.__project,
                                                                                         self.__jobId )
        return out

    ## since cool stamp gatter
    # @param self "Me, myself and Irene"
    # @return DozerIOV instance
    def since( self ):
        return self.__since 

    ## until cool stamp getter
    # @param self "Me, myself and Irene"
    # @return DozerIOV instance
    def until( self ):
        return self.__until 

    ## keys getter
    # @param self "Me, myself and Irene"
    def keys( self ):
        return sorted(self.__pointsInTime.keys() )
    
    ## item getter 
    # @param self "Me, myself and Irene"
    # @param key  tuple ( long, long )
    def __getitem__( self, key ):
        if ( key in self.__pointsInTime.keys() ):
            return self.__pointsInTime[key]

    ## iterator
    # @param self "Me, myself and Irene"
    def __iter__( self ):
        return self.__pointsInTime.__iter__()

    ## in operator
    # @param self "Me, myself and Irene"
    # @param key a key to check
    def __contains__( self, key ):
        if ( key in self.__pointsInTime ): return True
        return False

    ## has_key() 
    # @param self "Me, myself and Irene"
    # @param key a key to check
    def has_key( self, key ):
        if ( self.__pointsInTime.has_key( key ) ) : return True
        return False

    ## data items getter 
    # @param self "Me, myself and Irene"
    def items( self ):
        return self.__pointsInTime.items()

    ## iteritems getter
    # @param self "Me, myself and Irene"
    def iteritems( self ):
        return self.__pointsInTime.iteritems()

    ## iterkeys getter
    # @param self "Me, myself and Irene"
    def iterkeys( self ):
        return self.__pointsInTime.iterkeys() 

    ## number of points in time getter
    # @param self "Me, myself and Irene"
    def __len__( self ):
        return len( self.__pointsInTime.keys()  )

    ## transforms pointsInTime to DozerGraph format
    # @param self "Me, myself and Irene"
    def __transform( self ):
        forPlot = [ ]
        for key, value in self.__pointsInTime.iteritems():
            since, until = key
            forPlot.append( ( since, until, value ) )
        return forPlot 

    ## DozerGraph instance getter
    # @param self "Me, myself and Irene"
    def getDozerGraph( self ):
        try:
            forPlot = self.__transform() 
            return DozerGraph( self.__package, 
                               self.__framework,
                               self.__nightly,
                               self.__install,
                               self.__cmtconfig,
                               self.__project,
                               self.__jobId, 
                               forPlot  )
        except:
            self.epanic("unknown excption when making DozerGraph instance")
            return None

        
    ## XML DOM document builder
    # @param self "Me, myself and Irene"
    # @brief XML DOM document builder
    def __DOMXML( self ):
        self.doc = xml.dom.minidom.Document()
        recordSetNode = self.doc.createElement("recordset")
        
        recordSetNode.setAttribute( "since", self.__since.AsISO() )
        recordSetNode.setAttribute( "until", self.__until.AsISO() )
        recordSetNode.setAttribute( "framework", self.__framework )
        recordSetNode.setAttribute( "package", self.__package )
        recordSetNode.setAttribute( "nightly", self.__nightly )
        recordSetNode.setAttribute( "install", self.__install )
        recordSetNode.setAttribute( "cmtconfig", self.__cmtconfig )
        recordSetNode.setAttribute( "project", self.__project )
        recordSetNode.setAttribute( "jobId", self.__jobId )
        recordSetNode.setAttribute( "channel", self.__channel )
        
        for key, value in self.__pointsInTime.iteritems():
            recordNode = self.doc.createElement( "record" )
            since, until = key
            recordNode.setAttribute( "since", DozerIOV(since).AsISO()  )
            recordNode.setAttribute( "until", DozerIOV(until).AsISO()  )
            recordNode.appendChild ( value.xml().documentElement )
            recordSetNode.appendChild( recordNode ) 
        self.doc.appendChild( recordSetNode )
        return self.doc
        

    ## convert record set to comma separated value string
    # @param self "Me, myself and Irene"
    # @return string with CSV file content
    def AsCSV( self ):
        out = ""
        for key, channel in sorted( self.__pointsInTime.iteritems() ):
            if ( out == "" ):
                out = "# %s\n" % str(self) 
                out += "since,until"
                for data in channel.keys():
                    out += "," + data
                    if ( channel[data].unit() != "" ):
                        out += " [" + channel[data].unit() + "]"
                out += "\n"
            since, until = key
            out += "%s,%s" % ( since, until )
            for data in channel:
                value = channel[data].value()
                if ( value ==  None ):
                    value + ""
                out += "," + str(value)
            out += "\n"
        return out
        
    ## convert to DOM XML document
    # @param self "Me, myself and Irene"
    # @return DOM XML document instance
    def AsXML( self ):
        return self.__DOMXML() 

    ## convert record set to HTML table
    # @param self "Me, myself and Irene"
    # @param dataName DozerData to dump (default None means all)
    # @param tableCSS css class for table
    # @return HTML fragment with table
    def AsHTMLTable( self, dataName = None, tableCSS = None ):
        out = "<table"
        if ( tableCSS != None ):
            out += " class='" + str(tableCSS) + "'"
        out += ">"
        if ( not dataName ):
            out += "\n<caption>Payload for channel '%s' (%s/%s/%s/%s/%s).</caption>" % ( self.channel(), 
                                                                                         self.nightly(), 
                                                                                         self.install(), 
                                                                                         self.cmtconfig(),
                                                                                         self.project(),
                                                                                         self.jobId() )
        else:
            out += "\n<caption>Payload for data '%s' (%s/%s/%s/%s/%s/%s).</caption>" % ( str(dataName), 
                                                                                         self.nightly(), 
                                                                                         self.install(), 
                                                                                         self.cmtconfig(),
                                                                                         self.project(),
                                                                                         self.jobId(),
                                                                                         self.channel(),)
        # head row
        out += "\n<tr>"
        out += "<th>date</th>" # <th>until</th>"
        if ( dataName ):
            out += "<th>" 
            if ( dataName in self.__dataNames ):
                out += dataName 
                if ( self.__dataNames[dataName] != "" ):
                    out += " [" +  self.__dataNames[dataName] + "]"
            out += "</th>"
        else:
            for data in sorted(self.__dataNames):
                self.debug("data = " + data )
                if ( ( ";error" not in data ) and
                     ( ";downerror" not in data ) and 
                     ( ";uperror" not in data ) ):
                    unit = ""
                    if ( self.__dataNames[data] != "" ):
                        unit = "[%s]" % self.__dataNames[data]
                    out += "<th> %s %s </th>" % (data, unit )

        out += "</tr>"
        # data points rows
        for stamps, chan in sorted(self.__pointsInTime.iteritems(), reverse = True):
            since, until = DozerIOV( stamps[0] ).AsISO().split()[0], DozerIOV( stamps[1] ).AsISO().split()[0]
 
            out += "\n<tr>"
            out += "<td>" + since + "</td>"  # "<td>" + until + "</td>"
            if ( dataName ):
                val = "&mdash;"
                err = ""
                if ( dataName in chan and 
                     str(chan[dataName].value() ) != "" ):
                    val = str(chan[dataName].value()) 
                    if ( ( dataName + ";error" in chan ) and
                         ( chan[dataName + ";error"].value() != "" ) ):
                        err = " &plusmn; " + str(chan[dataName + ";error"].value() )
                    uerr = ""
                    ## up asymm error
                    if ( ( dataName + ";uperror" in chan ) and
                         ( chan[dataName + ";uperror"].value() != "" ) ):
                        uerr = " + " + str(chan[dataName + ";uperror"].value() )
                    derr = ""
                    if ( ( dataName + ";downerror" in chan ) and
                         ( chan[dataName + ";downerror"].value() != "" ) ):
                        derr = " - " + str(chan[dataName + ";downerror"].value() )
                    if ( (uerr + derr) != "" ):
                        err = uerr + derr
                out += "<td>%s</td>" % ( val + err )
            else:
                for data in sorted( self.__dataNames ):
                    if ( ( ";error" not in data ) and
                         ( ";downerror" not in data ) and 
                         ( ";uperror" not in data ) ):
                        val = "&mdash;"
                        err = ""

                        if ( data in chan and 
                             str(chan[data].value() ) != "" ):
                            val = str(chan[data].value() )
                        
                            err = ""
                            ## symm error
                            if ( ( data + ";error" in chan ) and
                                 ( chan[data + ";error"].value() != "" ) ):
                                err = " &plusmn; " + str(chan[data + ";error"].value() )
                            uerr = ""
                            ## up asymm error
                            if ( ( data + ";uperror" in chan ) and
                                 ( chan[data + ";uperror"].value() != "" ) ):
                                uerr = " + " + str(chan[data + ";uperror"].value() )
                            derr = ""
                            if ( ( data + ";downerror" in chan ) and
                                 ( chan[data + ";downerror"].value() != "" ) ):
                                derr = " - " + str(chan[data + ";downerror"].value() )
                            if ( (uerr + derr) != "" ):
                                err = uerr + derr
                       
                        out += "<td>%s</td>" % ( val + err )

            out += "</tr>"
        out += "\n</table>"
        return out
      
##
# @class test_DozerRecordSet
# @author Krzysztof Daniel Ciba
# @brief Test case for DozerRecordSet class.
#  
class test_DozerRecordSet( unittest.TestCase ):

    ## setup test suite
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.package = "Dummy"
        self.framework = "RTT"
        self.nightly = "dev"
        self.install = "build"
        self.cmtconfig = "i686-slc4-gcc34-opt"
        self.project = "AtlasProduction"
        self.jobId = "job_1"

        globals()["test_DozerRecordSet_logger"] = DozerLogger("DozerRecordSet_unittest.log", toConsole=True)

        self.__log =  globals()["test_DozerRecordSet_logger"]

        self.since = DozerIOV.fromRelease("rel_0").AsCool()
        self.until = DozerIOV.fromRelease("rel_6").AsCool()

        self.payload = DozerRecordSet( self.package,
                                       self.framework,
                                       self.nightly,
                                       self.install,
                                       self.cmtconfig,
                                       self.project,
                                       self.jobId,
                                       self.since, 
                                       self.until )

        channel = DozerChannel("test channel", "test channel description")
        channel += DozerData("IntDataRTT", "Int", "counts")
        channel += DozerData("IntDataRTT;error", "Int", "counts")
        channel += DozerData("FloatDataRTT", "Float", "some unit")
        channel += DozerData("FloatDataRTT;uperror", "Float", "some unit")
        channel += DozerData("FloatDataRTT;downerror", "Float", "some unit")

        pointsInTime = { }

        today = DozerIOV()

        for i in range(30):
            since = today.lastNDays(i-1)
            until = today.lastNDays(i)

            ichan = DozerChannel.fromXMLDoc( channel.xml() )
            if ( i != 10 ):
                ichan.get("IntDataRTT").setValue( i )
                ichan.get("IntDataRTT;error").setValue( 5 )

            if ( i % 3 != 0 ):
                ichan.get("FloatDataRTT").setValue( i )
                ichan.get("FloatDataRTT;uperror").setValue( i/2.0 ) 
                ichan.get("FloatDataRTT;downerror").setValue( i/4.0 ) 

            pointsInTime[ ( since, until) ] = ichan

        for k in sorted( pointsInTime.keys() ) :
            since, until = k
            val = pointsInTime[k].get("IntDataRTT").value()
            self.__log.info("since %s until %s value %s" % ( str(since), str(until) , str(val) ) ) 
            self.payload += ( since, until, pointsInTime[k] ) 


    ## __init__()
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):

        try:
            self.payload = DozerRecordSet( self.package,
                                           self.framework,
                                           self.nightly,
                                           self.install,
                                           self.cmtconfig,
                                           self.project,
                                           self.jobId,
                                           self.since, 
                                           self.until )            
        except:
            pass
        self.assertEqual( isinstance( self.payload, DozerRecordSet), True )

    ## += operator
    # @param self "Me, myself and Irene"
    def test_02_add( self ):        
        channel = DozerChannel("test channel", "test channel description")
        channel += DozerData("IntDataRTT", "Int", "counts")
        channel += DozerData("IntDataRTT;error", "Int", "counts")
        channel += DozerData("FloatDataRTT", "Float", "some unit")
        channel += DozerData("FloatDataRTT;uperror", "Float", "some unit")
        channel += DozerData("FloatDataRTT;downerror", "Float", "some unit")

        print self.payload.since()
        print self.payload.until()

        pointsInTime = { }

        today = DozerIOV()

        for i in range(30):
            since = today.lastNDays(i-1)
            until = today.lastNDays(i)

            ichan = DozerChannel.fromXMLDoc( channel.xml() )

            ichan.get("IntDataRTT").setValue( i )
            ichan.get("IntDataRTT;error").setValue( 1 )

            pointsInTime[ ( since, until) ] = ichan

        for k in sorted( pointsInTime.keys() ) :
            since, until = k
            val = pointsInTime[k].get("IntDataRTT").value()
            self.__log.info("since %s until %s value %s" % ( str(since), str(until), str(val) ) )
            self.payload += ( since, until, pointsInTime[k] ) 
    

        self.assertEqual( isinstance( self.payload, DozerRecordSet), True )
        
        self.assertEqual( len(self.payload.dataNames()), 5)

    ## dictionary interface
    # @param self "Me, myself and Irene"
    def test_03_dict( self ):
      
        for since, until in self.payload.keys():
            value = self.payload[ (since, until) ]["IntDataRTT"].value() 
            print "since=%s until=%s value=%s" % ( DozerIOV( since ).AsISO(), DozerIOV( until ).AsISO(), str(value) )
            
    
    ## plotter 
    # @param self "Me, myself and Irene"
    def test_04_plots( self ):
        grapher = self.payload.getDozerGraph()        
        self.assertEqual( isinstance( grapher, DozerGraph) , True )
        
        grapher.trend("IntDataRTT", True, "DozerRecordSet_test_04_trend_IntDataRTT.png")
        grapher.trend("FloatDataRTT", True, "DozerRecordSet_test_04_trend_FloatDataRTT.png")

        grapher.histogram("IntDataRTT", True, "DozerRecordSet_test_04_hist_IntDataRTT.png" )
        grapher.histogram("FloatDataRTT", True , "DozerRecordSet_test_04_hist_FloatDataRTT.png")
        
    def test_05_converters( self ):
        print self.payload.AsCSV() 
        print self.payload.AsXML().toprettyxml(" ", "\n")
        print self.payload.AsHTMLTable() 

        print self.payload.AsHTMLTable( "FloatDataRTT", "Dozer") 

        

## test suite execution
if __name__ == "__main__":
    
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerRecordSet)      
    unittest.TextTestRunner(verbosity=3).run(suite)
