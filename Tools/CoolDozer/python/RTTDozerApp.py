#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file RTTDozerApp.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date October 12th, 2009 
# @brief base class for RTT CoolDozer application

import sys
sys.argv.append("-b")

sys.rtt = "blah"

import logging
import datetime
from CoolDozer import *

## 
# @class python::RTTDozerApp::RTTDozerApp
# @extends python::DozerApp::DozerApp
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief base class for simplified long term monitoring in Atlas RTT
#
# @see https://twiki.cern.ch/twiki/bin/view/Main/RTTCoolDozerApp 
#
class RTTDozerApp( DozerApp ):

    ## last n day to read
    defaultLastNDays = 30

    ## flag to fit 
    defaultFit = True 

    ## flag for histo creation
    defaultHistos = False
    
    ## default will use prod
    dryRun = False

    ## default CSS + plot style 
    defaultStyle = "Dozer"

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param argDict argument dictionary from RTT side
    def __init__( self, argDict = {} ):

        self.name = self.__class__.__name__        
        super( RTTDozerApp, self ).__init__( self.name + ".log", True )
        DozerLogger.setLevel( logging.DEBUG )
        self.info( "*** Welcome to " + self.name + " ***" )
        self.info( str( DozerVersion() ) )

        self.configFlag = False 
        jobDscr   = argDict.get( "JobDescriptor", None )
        try:
            self.lastNDays = int(argDict.get( "LastNDays", self.defaultLastNDays ))
        except ValueError:
            self.epanic("cannot convert argument 'LastNDays' to integer, will use default (=%d)" % self.defaultLastNDays )
            self.lastNDays = self.defaultLastNDays 

    
        self.fitFlag   = argDict.get( "Fit", self.defaultFit )
        if ( self.fitFlag == "True" ): self.fitFlag = True
        else: self.fitFlag = False
    
        self.histoFlag = argDict.get( "Histos", self.defaultHistos )
        if ( self.histoFlag == "True" ): self.histoFlag = True
        else: self.histoFlag = False
        
        self.dryRun    = argDict.get( "DryRun", self.dryRun )
        if ( self.dryRun == "True" ): self.dryRun = True
        else: self.dryRun = False
        
        self.style     = argDict.get( "Style",  self.defaultStyle )

        if ( self.style not in [ "Dolphin", "Dozer", "Orca", "Seagull", "Seal" ] ):
            self.warn( "style " + self.style + " not known, will use default" )
            self.style = self.defaultStyle

        if ( jobDscr ):
            self.info( "[01]     DRY RUN = " + str( self.dryRun ) )
            self.info( "[02] LAST'N'DAYS = " + str( self.lastNDays ) )
            self.info( "[03]         FIT = " + str( self.fitFlag ) )
            self.info( "[04]      HISTOS = " + str( self.histoFlag ) )
            self.package    = jobDscr.package          # name of a package
            self.info( "[05]     PACKAGE = " + str( self.package ) )        
            self.runPath    = jobDscr.runPath             # output path
            self.info( "[06]    RUN PATH = " + str( self.runPath ) )
            self.branch     = jobDscr.paths.branch     # branch name
            self.info( "[07]      BRANCH = " + str( self.branch ) )
            self.install    = jobDscr.paths.runType    # run type 
            self.info( "[08]     INSTALL = " + str( self.install )  )
            self.cmtconfig  = jobDscr.paths.cmtConfig  # cmt config value
            self.info( "[09]   CMTCONFIG = " + str( self.cmtconfig ) )
            self.project    = jobDscr.paths.topProject            # project name
            self.info( "[10]     PROJECT = " + str( self.project ) )
            self.jobId      = jobDscr.userJobId  + "_" + self.name  # jobId
            self.info( "[11]   USERJOBID = " + str( self.jobId ) )
            self.logFile    = jobDscr.log # job log
            self.info( "[12]     JOB LOG = " + str( self.logFile ) )
            self.info( "[13] DOZER STYLE = " + str( self.style) )
            self.packageTag = jobDscr.paths.packageTag # svn tag
            self.info( "[14]     SVN TAG = " + str(self.packageTag) )
            self.release = jobDscr.paths.release # release 
            self.info( "[15]     RELEASE = " + str(self.release) )
            
            if ( None not in ( self.package, self.branch, self.install, self.cmtconfig, self.project, self.jobId ) ):
                self.configPath = "/".join( [ self.package, self.branch, self.install, self.cmtconfig, self.project, self.jobId ] ) 
                self.info("will use " + self.configPath + " for runtime configuration" )   
                self.dozerConfig = DozerConfig( "RTT" )
                self.configFlag = True
                self.info("*** DozerConfig created")
            else:
                self.warn( "missing parameter(s) in RTT job descriptor" )
                self.configFlag = False
        else:
            self.warn( "RTT job descriptor not found in argument dictionary, exitiing...")
            sys.exit(0)
        
    ## RTT run 
    # @param self "Me, myself and Irene"
    def run( self ):
        self.info( "*** entering run" )
        if ( self.configFlag ):
            self.debug( "*** adding user defined schema to config" )
            self.dozerConfig += self.schema()
            self.info( "*** constructing schema for runtime configuration: " + 
                       "/".join( [self.branch, self.install, self.cmtconfig, self.project, self.jobId ] ) )
            self.dozerSchema = self.dozerConfig.get( "/".join( [ self.branch, self.install, self.cmtconfig, self.project, self.jobId ] ) )
            self.info( "*** calling writer" )
            try:
                self.writer()
            except Exception, val:
                self.epanic("exception hit when running writer: " + str(val) )
            self.db = DozerDB( self.package, "RTT", self.dryRun, "PROD" )
        
            self.db.openDB()
            if ( self.db.isOpened() ):
                self.db +=  self.dozerConfig.get( "/".join( [self.branch, self.install, self.cmtconfig, self.project, self.jobId ] ) )
                since = None
                until = None
                if ( self.release.startswith("rel_") ):
                    since = DozerIOV().fromRelease( self.release )
                    until = DozerIOV( since.lastNDays(-1) )
                else:
                    since = DozerIOV().today()
                    until = DozerIOV().nextDay()
                #self.db += self.dozerConfig.get( "/".join( [ self.branch, self.install, self.cmtconfig, self.project, self.jobId ] ) )

                self.info( "since=" + str(since) )
                self.info( "until=" + str(until) )

                if ( None not in (since, until ) ):
                    self.db.addSchemaPayload( self.dozerSchema, since, until )
                else:
                    self.error( "adding a new record to db failed, one of 'since' or 'until' in None" )
                    return 1
                
            self.info( "*** calling reader" )      
            self.htmlFile = open( self.name + ".html", "w+")
            self.html( "<html><head><title>"+self.name+"</title>" )
            self.html( "<link type='text/css' href='https://atlasrtt.cern.ch/dozer/api/latest/css/"+self.style+".css' rel='stylesheet'>" )
            self.html( "</head>")


            self.html( "<body>" )
            try:
                self.reader()
            except Exception, val:
                self.epanic("exception hit when running reader: " + str(val) )
                

            self.html("<div class='bottom'> Generated on <i>" + str(datetime.datetime.now()).split(".")[0] + 
                      "</i> by " + self.name + " &copy; Krzysztof Daniel Ciba </div>" )

            self.html( "</body></html>\n")
            self.info( "html file " + self.name + ".html has been created")
            return 0
        else:
            self.error( "wrong configuration, will not work" )
            self.info( "*** exiting run")
            return 1
 
    ## db reader
    # @param self "Me, myself and Irene"
    def reader( self ):
       
        self.info("-> entering reader")

        self.html( "<div class='top'><a name='top'></a>\n" )
        self.html( "<h2>" + self.name + " &mdash; " + self.package + "</h2>")
        config = " &mdash; ".join( [ self.branch, self.install, self.cmtconfig, self.project , self.jobId ] )
        self.html( "<h2> <span><i>(" + config + ")</i></span></h2>" )
        self.html("<a href='" + self.name + ".log' target='_new'>[ log file ]</a>" )
        

        if ( self.db.isOpened() ):
            self.info("--> db is still opened, that's good, building channels list")
            self.html("<ul>\n")
            
            for channel in self.dozerSchema.getChannelsByName():
                self.html( "<li>channel: <a href='#"+channel+"'>" + channel + "</a>" )
                self.html("<ul>\n")
                for data in self.dozerSchema[channel]:
                    if ( ";error" not in data and
                         ";uperror" not in data and 
                         ";downerror" not in data ):
                        self.html("<li> data: <a href='#"+channel+"_"+data+"'>" + data + "</a></li>")
                self.html("</ul></li>\n")
            self.html("</ul></div>\n")

            for channel in self.dozerSchema.getChannelsByName():
                self.info("---> dumping html for channel " + channel )
                self.html("\n<div class='channel'><h2>\n")
                self.html( "CHANNEL: <a name='"+ channel+"'></a>"+channel+ "</h2>" )
                self.html( "<h3>DESCRIPTION: " + self.dozerSchema[channel].desc() + "</h3><br><br>")

                since = DozerIOV().lastNDays( self.lastNDays )
                until = DozerIOV().today()
                recordSet = self.db.get( "/" + self.configPath + "/" + channel, since, until )
                
                if ( not recordSet ):
                    self.panic( "cannot read recod set out of db, connection lost?")
                    self.html( "<span style='color:red;'>ERROR reading record set, connection lost?</span></div>")
                    continue

                self.info("----> channel's table")
                self.html("<table>\n")
                self.html("<caption> Data table for channel: <b>" + channel + "</b></caption>")
                self.html("<tr><th> Date </th>")
                for data in self.dozerSchema[channel].keys():
                    if ( ";error" not in data and
                         ";uperror" not in data and 
                         ";downerror" not in data ):
                        self.html( "<th>" + data )
                        if ( self.dozerSchema[channel][data].unit() ):
                            self.html(" [" +  self.dozerSchema[channel][data].unit() + "]" )
                        self.html( "</th>" )
                self.html( "</tr>" )

                keys = sorted( recordSet.keys(), reverse=True)
                for point in keys:
                    self.html( "\n<tr>")
                    self.html( "<td>" + DozerIOV(point[0]).AsISO().split()[0] + "</td>" )                
                    for data in self.dozerSchema[channel].keys():
                        if ( ";error" not in data and
                             ";uperror" not in data and 
                             ";downerror" not in data ):

                            val = str( recordSet[point][data].value() )
                            err = ""
                            if ( data + ";error" in self.dozerSchema[channel].keys() ):
                                 err = " &plusmn; " + str(recordSet[point][str(data) + ";error"].value() )
                            else:
                                if ( data + ";uperror" in self.dozerSchema[channel].keys() ):
                                    err += "+" + str( recordSet[point][str(data) + ";uperror"].value() ) 
                                if ( data + ";downerror" in self.dozerSchema[channel].keys() ):
                                    err += "-" + str( recordSet[point][str(data) + ";downerror"].value() ) 
                            if ( val not in ( "", None) ):
                                self.html( "<td>" + val + err + "</td>" )
                            else:
                                self.html( "<td>&mdash;</td>" )
                    self.html("</tr>\n")
                self.html("</table><br><br>\n")

                for data in self.dozerSchema[channel].keys():
                    if ( ";error" not in data and
                         ";uperror" not in data and 
                         ";downerror" not in data ):
                        self.info("----> dumping html for data=" + data )
                        self.html( "\n<div class='data'>" +
                                   "<a href='#top'>[ top ]</a><h3>DATA: <a name='"+channel+"_"+data+"'></a> "+data+"</h3><br>\n"  )
                        
                        if ( recordSet ):
                            gr = recordSet.getDozerGraph()
                            if ( gr.isPlotable( data) ):
                                self.info("----> plotting data=" + data )
                                png = "_".join( ["trend", channel, data, self.name] ) + ".png"
                                try:
                                    plottingTrend = gr.trend( data, self.fitFlag,  png, self.style )
                                    if ( plottingTrend ):
                                        self.html("<div class='image'>")
                                        self.html("<div>Trend plot for <b>"+data+"</b></div><br>")
                                        self.html( "<img src='"+png+"'></div>")
                                    else:
                                        self.html("<div class='image'>")
                                        self.html("<div>Trend plot for <b>"+data+"</b><br><span style='color:red;'>Plotting failed!</span></div><br></div>")
                                    if ( self.histoFlag ):
                                        png =  "_".join( ["histo", channel, data, self.name] ) + ".png"
                                        plottingHisto = gr.histogram( data, self.fitFlag, png, self.style )
                                        if ( plottingHisto ):
                                            self.html("<div class='image'>")
                                            self.html("<div>Histogram plot for <b>" + data + "</b></div><br>")
                                            self.html( "<img src='"+png+"'></div>")
                                        else:
                                            self.html("<div class='image'>")
                                            self.html("<div>Histogram plot for <b>" + data + "</b><br><span style='color:red;'>Plotting failed!</span></div><br></div>")
                                except:
                                    self.epanic( "plotting of " + data + " failed!" )
                                    self.html("<div style='color:red;'>Plotting failed!</div>")
                                
                                    
                        self.info("-----> table for data=" + data)
                        self.html("<table>")
                        self.html("<caption> Data table for <b>" + data + "</b></caption>")
                        self.html("<tr><th> Date </th><th>" + data )
                        if (  self.dozerSchema[channel][data].unit()  ):
                            self.html( " [" +  self.dozerSchema[channel][data].unit() + "]" )
                        self.html( "</th></tr>" )
                        dkeys = sorted( recordSet.keys(), reverse=True )
                        for point in dkeys:
                            val = str( recordSet[point][data].value() )
                            err = ""
                            if ( data + ";error" in self.dozerSchema[channel].keys() ):
                                err = " &plusmn; " + str(recordSet[point][str(data) + ";error"].value() )
                            else:
                                if ( data + ";uperror" in self.dozerSchema[channel].keys() ):
                                    err += "+" + str( recordSet[point][str(data) + ";uperror"].value() ) 
                                if ( data + ";downerror" in self.dozerSchema[channel].keys() ):
                                    err += "-" + str( recordSet[point][str(data) + ";downerror"].value() ) 
                           
                            since = DozerIOV(point[0]).AsISO().split()[0]
                           
                            self.info("date=" + since + " val=" + val )
                            self.html( "<tr>")
                            self.html( "<td>" + since + "</td>" )
                            if ( val not in ( "", None ) ):
                                self.html( "<td>" + val + err + "</td>" )
                            else:
                                self.html("<td>&mdash;</td>")
                            self.html("</tr>\n")
                        self.html("</table><br>\n")
                        self.html("</div>")

                self.html("<br><a href='#top'>[ top ]</a></div>")
            

        else:
            self.panic("--> db is closed, crap!")
            self.html( "Cannot open db..." )
        self.info("-> exiting reader")
        
    ## html page creation
    # @param self "Me, myself and Irene"
    # @param msg HTML string 
    def html( self, msg ):
        self.htmlFile.write( msg )
