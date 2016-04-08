#!/bin/env python
##
# @file DozerReader.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief implementation of standalone CoolDozer db reader


import sys, os
sys.rtt = True
import optparse
import re
import datetime 
from CoolDozer import *
import uuid
import unittest

##
# @class dozerReader::DozerReaderOptParser
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com) 
# @brief command line option parser for dozerReader
class DozerReaderOptParser( object ):
    
    ## c'tor
    # @param self "Me, myself and Irene"
    def __init__( self ):
        
        ## validator
        self.valid = DozerValid()

        version = "%prog $Revision: 308299 $ by Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)"
        usage = "%prog [opts]"
        ## optparse.OptionParser instance
        self.__parser = optparse.OptionParser( usage=usage, version=version )

        self.__parser.add_option( "-b",
                                  action="store_true",
                                  dest="batch",
                                  help="batch mode for PyROOT [always on]")

        self.__parser.add_option( "-p",
                                  "--package",
                                  type="string",
                                  action="callback",
                                  callback=self.check_package,
                                  dest="package",
                                  help="package name (REQUIRED)" )
       
        self.__parser.add_option( "-l",
                                  "--logname",
                                  type="string",
                                  action="store",
                                  dest="logname",
                                  help="log name [default dozerReader_PACKAGE.log]" )
 
        self.__parser.add_option( "-q",
                                  "--quiet",
                                  action="store_false",
                                  dest="console",
                                  help="don't print log messages to console [default %default]" )

        self.__parser.add_option( "--verbose",
                                  action="store_true",
                                  dest="console",
                                  help="print log messages to stdout [default %default]" )

        self.__parser.add_option("--framework", 
                                 type="choice",
                                 dest = "framework",
                                 choices = self.valid.frameworks().keys(),
                                 help="test framework [default %default]")

        self.__parser.add_option( "--branch",
                                  action="callback",
                                  callback=self.check_branch, 
                                  type="string",
                                  dest="branch",
                                  help="branch name [default %default]")

        self.__parser.add_option( "--install",
                                  action="callback",
                                  callback=self.check_install, 
                                  type="string",
                                  dest="install",
                                  help="install type [default %default]")
        
        self.__parser.add_option( "--cmtconfig",
                                  action="callback",
                                  callback=self.check_cmtconfig,
                                  type="string",
                                  dest="cmtconfig",
                                  help="cmtconfig value [default %default]")

        self.__parser.add_option( "--project",
                                  action="callback",
                                  callback=self.check_project,
                                  type="string",
                                  dest="project",
                                  help="project name [default %default]")

        self.__parser.add_option( "--jobid",
                                  type="string",
                                  action="store",
                                  dest="jobid",
                                  help="job id name" )

        self.__parser.add_option( "--since", 
                                  type="string", 
                                  action="callback",
                                  callback=self.check_date,
                                  dest="since",
                                  help="since date in ISO format (YYYY-MM-DD)")

        self.__parser.add_option( "--until", 
                                  type="string", 
                                  action="callback",
                                  callback=self.check_date,
                                  dest="until",
                                  help="until date in ISO format (YYYY-MM-DD)")

        self.__parser.add_option( "--lastndays", 
                                  type="int", 
                                  action="store",
                                  dest="lastNDays",
                                  help="last N days of monitoring [default 30]")

        self.__parser.add_option( "--dump-xml", 
                                  action="store_true",
                                  dest = "dump_xml",
                                  help = "dump payloads to xml files [default %default]" )

        self.__parser.add_option( "--dump-csv", 
                                  action="store_true",
                                  dest = "dump_csv",
                                  help = "dump payloads to csv files [default %default]" )

        self.__parser.add_option( "--output",
                                  action="store",
                                  type="string",
                                  dest="output",
                                  help="output file name [default %default]")

        self.__parser.add_option( "--css", 
                                  type="choice",
                                  choices=[ "Dozer", "Dolphin", "Orca",  "Seagull",  "Seal" ], 
                                  dest="css",
                                  help="html page style sheet [default %default]")

        
        self.__parser.add_option( "--plots",
                                  action="store_true",
                                  dest="plots",
                                  help="trigger trends and histograms generation (same as --plot-trend and --plot-histos together) [default %default]" )

        self.__parser.add_option( "--plot-trends",
                                  action="store_true",
                                  dest="trends",
                                  help="trigger trend plots generation [default %default]")

        self.__parser.add_option( "--plot-histos",
                                  action="store_true",
                                  dest="histos",
                                  help="trigger histogram plots generation [default %default]")

        self.__parser.add_option( "--fit",
                                  action="store_true",
                                  dest="fit",
                                  help="trigger plots fitting (same as --fit-trends and --fit-histos together) [default %default]")

        self.__parser.add_option( "--fit-trends",
                                  action="store_true",
                                  dest="fit_trends",
                                  help="trigger trend plots fitting [default %default]")

        self.__parser.add_option( "--fit-histos",
                                  action="store_true",
                                  dest="fit_histos",
                                  help="trigger histogram plots fitting [default %default]")
        
        self.__parser.set_defaults( console = False,
                                    framework = "RTT",
                                    cmtconfig="*", 
                                    branch = "*", 
                                    project = "*", 
                                    install = "build",
                                    jobid = "*",
                                    output = "dozerReader.html",
                                    css = "Dozer",
                                    plots = False, 
                                    trends = False,
                                    histos = False, 
                                    fit = False, 
                                    fit_trends = False, 
                                    fit_histos = False,
                                    dump_xml = False,
                                    dump_csv = False )

    ## parse command line aruments
    # @param self "Me, myself and Irene"
    # @param argv list of options, default = sys.argv[1:]
    # @return tuple with parsed args and opts 
    def parse( self, argv = sys.argv[1:] ):
        self.opts, self.args = self.__parser.parse_args( argv )

        if not self.opts.package:
            self.__parser.error("--package option ir required")
        if ( not self.opts.since and self.opts.until ):
            self.__parser.error("option --until present but --since is missing")
        if ( self.opts.since and not self.opts.until ):
            self.__parser.error("option --since present but --until is missing")
        if ( self.opts.since and self.opts.until ):
            since = DozerIOV.fromISO( self.opts.since + " 00:00:00" )
            until = DozerIOV.fromISO( self.opts.until + " 00:00:00" )
            if ( since > until ):
                self.__parser.error("wrong parameters for --since and --until, until date(=" +
                                    self.opts.until + ") is earlier than since date=(" + self.opts.since + ")!")
            if ( self.opts.lastNDays ):
                self.__parser.error("cannot use --since, --until and --lastndays options at the same time!")
        elif ( not self.opts.lastNDays ):
            self.update( "lastNDays", 30 )
        if ( self.opts.lastNDays ):
            until = DozerIOV()
            since = DozerIOV( until.lastNDays( 30 ) )
            self.update( "since", since.AsISO().split()[0] )
            self.update( "until", until.AsISO().split()[0] ) 
        if ( self.opts.plots ):
            self.update( "trends", True )
            self.update( "histos", True )
        if ( True in ( self.opts.fit, self.opts.fit_trends, self.opts.fit_histos ) ):
            if ( True not in ( self.opts.plots, self.opts.trends, self.opts.histos ) ):
                self.__parser.error("cannot use --fit, --fit-trends and --fit-histos, if plots (--plots, --plot-trends or --plot-histos) are nor enabled")
        
        if ( self.opts.fit_trends and not self.opts.trends ):
            self.__parser.error("cannot use --fit-trends if trends plotting (--plots or --plot-trends) is not enabled")
        if ( self.opts.fit_histos and not self.opts.histos ):
            self.__parser.error("cannot use --fit-histos if histos plotting (--plots or --plot-histos) is not enabled")
        if ( self.opts.fit ):
            self.update( "fit_trends", True)
            self.update( "fit_histos", True)

        return ( self.opts, self.args )

    ## callback function for --package option
    # @param self "Me, myself and Irene"
    # @param option
    # @param opt_str
    # @param value 
    # @param parser
    def check_package(self, option, opt_str, value, parser):
        if ( parser.values.logname == None ):
            parser.values.logname = "dozerReader_" + value + ".log"
        setattr( parser.values, option.dest, value )

    ## callback function for --project option
    # @param self "Me, myself and Irene"
    # @param option
    # @param opt_str
    # @param value 
    # @param parser
    def check_project( self, option, opt_str, value, parser):
        if ( value not in self.valid.projects() ):
            msg = "Invalid %s option value '%s'\n\nIt should be one of:\n - " % ( option, value )
            msg += "\n - ".join( sorted(self.valid.projects().values() ) )
            msg += "\n"
            raise optparse.OptionValueError( msg )
        setattr( parser.values, option.dest, value )

    ## callback function for --install option
    # @param self "Me, myself and Irene"
    # @param option
    # @param opt_str
    # @param value 
    # @param parser
    def check_install( self, option, opt_str, value, parser):
        if ( value not in self.valid.installs() ):
            msg = "Invalid %s option value '%s'\n\nIt should be one of:\n - " % ( option, value )
            msg += "\n - ".join( sorted(self.valid.installs().values()) )
            msg += "\n"
            raise optparse.OptionValueError( msg )
        setattr( parser.values, option.dest, value )

    ## callback function for --cmtconfig option
    # @param self "Me, myself and Irene"
    # @param option
    # @param opt_str
    # @param value 
    # @param parser
    def check_cmtconfig( self, option, opt_str, value, parser):
        if ( value not in self.valid.cmtconfigs() ):
            msg = "Invalid %s option value '%s'\n\nIt should be one of:\n - " % ( option, value )
            msg += "\n - ".join( sorted( self.valid.cmtconfigs().values()) )
            msg += "\n"
            raise optparse.OptionValueError( msg )
        setattr( parser.values, option.dest, value )

    ## callback function for --branch option
    # @param self "Me, myself and Irene"
    # @param option
    # @param opt_str
    # @param value 
    # @param parser
    def check_branch( self, option, opt_str, value, parser):
        found = False
        for nightly, reNightly in self.valid.nightlies().iteritems():
            if ( nightly != "*" ):
                if ( reNightly.match(value) ): found = True
        if ( not found ):
            msg = "Invalid %s option value '%s'\n\nIt should be one of:\n - " % ( option, value )
            msg += "\n - ".join( sorted( self.valid.nightlies().keys()) )
            msg += "\n"
            raise optparse.OptionValueError( msg )
        setattr( parser.values, option.dest, value )

    ## callback function for --since and --until options
    # @param self "Me, myself and Irene"
    # @param option
    # @param opt_str
    # @param value 
    # @param parser
    def check_date( self, option, opt_str, value, parser ):
        info = ( opt_str, value )
        if ( ( "-" not in value ) or 
             ( value.count("-") != 2 ) ): 
            raise optparse.OptionValueError("%s option value (=%s) error, not in ISO date format (YYYY-MM-DD)!" % info ) 
        
        year, month, day = value.split("-")

        try:
            year = int(year)
        except ValueError, value:
            raise optparse.OptionValueError("%s option value (=%s) error, year isn't an integer!" % info )
            
        try:
            month = int(month)
        except ValueError, value:
            raise optparse.OptionValueError("%s option value (=%s) error, month isn't an integer!" % info )
    
        try:
            day = int(day)
        except ValueError, value:
            raise optparse.OptionValueError("%s option value (=%s) error, day isn't an integer!" % info )
        
        try:
            from datetime import datetime
            datetime( year, month, day, 0, 0, 0  )
        except ValueError, val:
            raise optparse.OptionValueError("%s option value (=%s) error, %s" % ( opt_str, value, val ) )
            
        setattr( parser.values, option.dest, value )
       
    ## update parsed options 
    # @param self "Me, myself and Irene"
    # @param key option name
    # @param what new value
    def update( self, key, what ):
        self.opts._update_loose( { key : what } )
        
        
##
# @class dozerReader::dozerReader
# @extends python::DozerApp::DozerApp
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief stand-alone reader CoolDozer app
class dozerReader( DozerApp ):

    ## c'tor
    # @param self "Me, myself and Irene"
    def __init__( self ):
        self.parser = DozerReaderOptParser()
        self.args, self.opts = self.parser.parse()

        self.outDir = os.path.split( os.path.abspath( self.args.output ) )[0]
        if ( not os.path.exists( self.outDir ) ):
            try:
                os.makedirs( self.outDir )
            except OSError:
                print "cannot create output directory " + self.outDir + ", exiting!"
                sys.exit(1)
        
        super( dozerReader, self ).__init__( self.outDir + os.sep + self.args.logname, self.args.console )
        self.setLevel( 20 )
        # say hello
        self.info( DozerVersion() )
        # what are the args and opts?
        self.print_args( )
        # configure yourself
        self.configure( )
        # and run 
        self.run( )
 
    ## dump args to logger
    # @param self "Me, myself and Irene"  
    # @callgraph
    def print_args( self ):
        self.info("will list and check CLI options...")
        self.info("[01] package name      = " + self.args.package )
        self.info("[02] test framework    = " + self.args.framework )
        self.info("[03] configuration:" ) 
        self.info(" - branch              = " +  self.args.branch )
        self.info(" - install             = " +  self.args.install )
        self.info(" - cmtconfig           = " +  self.args.cmtconfig )
        self.info(" - project             = " +  self.args.project )
        self.info(" - jobid               = " +  self.args.jobid )
        self.info("[04] time period:" )
        self.info(" - since               = " +  self.args.since )
        self.info(" - until               = " +  self.args.until )
        self.info("[05] dump:")
        flag = "on" if self.args.dump_xml else "off"
        self.info(" - xml                 = " + flag )
        flag = "on" if self.args.dump_csv else "off"
        self.info(" - csv                 = " + flag )
        self.info("[06] plotting:")
        flag = "on" if self.args.trends else "off"
        self.info(" - trends              = " + flag )
        flag = "on" if self.args.fit_trends else "off"
        self.info(" - trends fitting      = " + flag )
        flag = "on" if self.args.histos else "off"        
        self.info(" - histogramms         = " + flag )
        flag = "on" if self.args.fit_histos else "off"
        self.info(" - histogramms fitting = " + flag )
        self.info("[07] output:" )
        self.info(" - output name         = " + self.args.output )
        self.info(" - log name            = " + self.args.logname )
        flag = "on" if self.args.console else "off"
        self.info(" - log to stdout       = " + flag )
        
    ## make path pattern for search
    # @param self "Me, myself and Irene"
    def configure( self ):
        self.pathPattern = "/".join( [ "",  # empty for leading /
                                       self.args.package,
                                       self.args.branch,
                                       self.args.install,
                                       self.args.cmtconfig,
                                       self.args.project,
                                       self.args.jobid ] )
                                    
        self.debug( "path pattern        = " + self.pathPattern )
        self.rePathPattern = self.pathPattern.replace( "*", "[a-zA-Z0-9\-_\.]+")
        self.debug( "regexp path pattern = " + self.rePathPattern )
        self.rePathPattern = re.compile( self.rePathPattern )

        self.pageTitle = "CoolDozer &mdash; dozerReader &mdash; " + self.args.package 

        if ( self.args.trends or self.args.histos ):
            self.imagesDir = self.outDir + os.sep + "images"
            if ( not os.path.exists( self.imagesDir ) ):
                try:
                    self.info( "will create directory 'images' for storing plots...")
                    os.mkdir( self.imagesDir )
                except OSError:
                    self.epanic("cannot create output directory for plots " + self.imagesDir + ", exiting!")
                    sys.exit(1)
           
        if ( self.args.dump_xml ):
            self.xmlDir = self.outDir + os.sep + "xml"
            if ( not os.path.exists( self.xmlDir ) ):
                try:
                    self.info( "will create directory 'xml' for storing xml dump files...")
                    os.mkdir( self.xmlDir )
                except OSError:
                    self.epanic("cannot create output directory for xml files " + self.xmlDir + ", exiting!")
                    sys.exit(1)
        
        if ( self.args.dump_csv ):
            self.csvDir = self.outDir + os.sep + "csv"
            if ( not os.path.exists( self.csvDir ) ):
                try:
                    self.info( "will create directory 'csv' for storing csv dump files...")
                    os.mkdir( self.csvDir )
                except OSError:
                    self.epanic("cannot create output directory for csv files " + self.csvDir + ", exiting!")
                    sys.exit(1)
     
        self.info( "will try to open '" + self.args.output + "' for writing..." )
        try:
            self.htmlFile = open( os.path.abspath( self.args.output ) , "w+")
        except:
            self.epanic( "cannot open " + self.args.output + " file for writing" )
            sys.exit(1)

    ## scan db internal structure looking for FolderSets and Folders
    # @param self "Me, myself and Irene"
    # @param where string with path 
    def scanDB( self, where ):
        self.debug("will scan directory '" + where + "'..." )
        pwd = self.db.pwd()
        self.db.cd( where )
        what, folders =  self.db.ls()
        self.db.cd( pwd )
        if ( what == "folders" ):
            for folder in folders:
                self.scanDB( folder )
        else:
            self.allpaths[where] = folders
            return
                
    ## write text to html file 
    # @param self "Me, myself and Irene"
    # @param text html string 
    def html( self, text, tag = None ):
        if ( tag ):
            text = "<%s>%s</%s>" % ( str(tag), str(text), str(tag) )
        self.htmlFile.write( str(text) )
        pass


    def head( self ):
        out = "<html><head><title>"+self.pageTitle+"</title>" 
        out +=  "<link type='text/css' href='https://atlasrtt.cern.ch/dozer/api/latest/css/"+self.args.css+".css' rel='stylesheet'>"
        out += "</head>"
        return out

    ## run, Forest, run!
    # @param self "Me, myself and Irene"
    def run( self ):
        
        self.db = DozerDB( self.args.package, self.args.framework, dry=False, db="PROD" )
        
        if ( not self.db.openDB() ):
            self.error("cannot open db for reading, wrong package name?, exiting!")
            raise DozerError( "cannot open db for reading, wrong package name?" )
        else:
            ## all paths for one package
            self.allpaths = { }
            ## matched paths
            self.paths = { }
            # scanning
            self.scanDB( "/" + self.args.package )
            ## matching
            for path in sorted(self.allpaths.keys()):
                self.debug( "path " + path )
                # match found!
                if self.rePathPattern.match( path ):
                    # copy channels
                    self.paths[path] = self.allpaths[path]
            
            self.html( "<html>" )
            self.html( self.head() )
            self.html( "<body>" )
            self.since = DozerIOV.fromISO ( self.args.since + " 00:00:00")
            self.until = DozerIOV.fromISO ( self.args.until + " 00:00:00")
            
            self.html( "<div class='top'><a name='top'></a>\n" )
            self.html( "<h2> dozerReader output for package " + self.args.package + "</h2>")
            self.html( "<h3> since " + self.since.AsISO() + " until " + self.until.AsISO() + "</h3>")
            
            self.html("<a href='"+self.args.logname+"' target='_new'>log file &gt;&gt;</a>", "h3" )
    

            self.html("<ul>")
            for path in sorted( self.paths.keys() ):
                self.html( "<li>" + path + "<ul>" )
                for chan in self.paths[path].keys():
                    if ( chan != "cooldozer metadata" ):
                        self.html( "<a href='#"+path+"/"+chan+"'>"+chan+"</a>", "li" )
                self.html("</ul></li>")
            self.html("</ul>")
            self.html("</div>")
            ## read db
            for path in sorted( self.paths.keys() ):
               
                self.info( "will read all channels in path = " + path )
                for chan in self.paths[path].keys():
                    if ( chan != "cooldozer metadata" ):
                        self.html("<div class='channel'><a name='"+path+"/"+chan+"'></a>")
                        self.html("Records from " + path+"/"+chan, "h3" )
                        recordSet = self.db.get( path + "/" + chan, self.since, self.until )
                        if ( recordSet ):

                            dumpName = str(path[1:] + "/" + chan).replace("/", "__").replace(" ", "_")

                            if ( self.args.dump_xml ):
                                try:
                                    dumpXMLDoc = recordSet.AsXML()
                                    dumpXMLFile = self.xmlDir + os.sep + dumpName + ".xml" 
                                    self.info("will dump recordset to file " + dumpXMLFile )
                                    dumpXMLFile = open( dumpXMLFile, "w+")
                                    dumpXMLFile.write( dumpXMLDoc.toprettyxml(" ", "\n") )
                                    dumpXMLFile.close() 
                                    self.html("<center><a href='xml/"+ dumpName +".xml'>As XML &gt;&gt;</a></center><br>")

                                except:
                                    self.epanic("something goes wrong when dumping channel " + 
                                                path + "/" + chan + " to xml file, exiting!" )
                                    sys.exit(1)
                                    
                            if ( self.args.dump_csv ):
                                try:
                                    dumpCSVDoc = recordSet.AsCSV()
                                    dumpCSVFile = self.csvDir + os.sep + dumpName + ".txt" 
                                    self.info("will dump recordset to file " + dumpCSVFile )
                                    dumpCSVFile = open( dumpCSVFile, "w+")
                                    dumpCSVFile.write( dumpCSVDoc )
                                    dumpCSVFile.close() 
                                    self.html("<center><a href='csv/"+ dumpName +".txt'>As CSV &gt;&gt;</a></center><br>" )
                                except:
                                    self.epanic("something goes wrong when dumping channel " + 
                                                path + "/" + chan + " to cvs file, exiting!" )
                                    sys.exit(1)
                            
                            # dump as html table
                            self.info("will dump recordset to html table...")
                            self.html( recordSet.AsHTMLTable(), "center" )
                        
                            
                            if ( self.args.trends or self.args.histos ):
                                plotter = recordSet.getDozerGraph() 
                                for pdata in sorted( plotter.plotables() ):
                                    if ( ( ";error" not in pdata ) and 
                                         ( ";uperror" not in pdata ) and
                                         ( ";downerror" not in pdata) ):
                                        if ( self.args.trends ):
                                            trendName = self.imagesDir + "/" + "trend__" + dumpName + "__" + pdata + ".png" 
                                            self.info("will plot trend " + trendName )
                                            trend = plotter.trend( pdata, self.args.fit_trends, trendName, self.args.css  )
                                            self.html("<div class='image'>")
                                            self.html("<div>Trend plot for <b>"+pdata+"</b></div><br>")
                                            if ( trend ):
                                                self.html("<center><img src='images/" + "trend__" + dumpName + "__" + pdata + ".png" +"'><br></center>")
                                            else:
                                                self.html("<span style='color: red'>Plotting failed!</span>", "center")
                                            self.html("</div>")
                                            
                                        if ( self.args.histos ):
                                            histName = self.imagesDir + "/" + "histo__" + dumpName + "__" + pdata + ".png" 
                                            self.info("will plot histogram " + histName )
                                            histogram = plotter.histogram( pdata, self.args.fit_trends, histName, self.args.css  )
                                            self.html("<div class='image'>")
                                            self.html("<div>Histogram plot for <b>"+pdata+"</b></div><br>")
                                            if ( histogram ):
                                                self.html("<center><img src='images/" + "histo__" + dumpName + "__" + pdata + ".png" +"'><br></center>")
                                            else:
                                                self.html("<span style='color: red'>Plotting failed!</span>", "center")
                                            self.html("</div>")
                        else:
                            self.html("<span style='color: red'>Payload is empty!<span>", "center")
                            self.error("payload not read!")
                        self.html("<br><center><a href='#top'>^TOP^</a></center><br>")
                        self.html("</div>")

            self.html("<div class='bottom'> Generated on <i>" + str(datetime.datetime.now()).split(".")[0] + 
                      "</i> by dozerReader &copy; <a href='mailto:Krzysztof.Ciba@NOSPAMgmail.com'>Krzysztof Daniel Ciba</a></div>" )

            self.html("</body></html>")
            self.info("All done. Bye!")

## start processing 
if __name__ == "__main__":
    dozerReader()
    
