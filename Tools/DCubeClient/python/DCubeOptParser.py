#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DCubeClient/python/DCubeOptParser.py
# @author Krzysztof Danile Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief implementation of DCubeOptParser

import sys
import optparse
from DCubeUtils import DCubeObject, DCubeLogger, DCubeSingleton, DCubeVersion
from DCubeValid import DCubeValid
import unittest


##
# @class DCubeOptParser
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief sys.argv parser for DCube
class DCubeOptParser( object ):

    ## forcing to be a singleton
    __metaclass__ = DCubeSingleton
 
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param opts options to parse
    # @param args args to parse 
    def __init__( self  ):


        self.valid = DCubeValid()

        today = "today"
        try:
            from datetime import date
            today = date.today().isoformat()
        except: 
            pass

        usage = "dcube.py [options] [args] [MONITORED.root]"

        self.__version = DCubeVersion()
        self.__par = optparse.OptionParser( usage=usage, version=self.__version.version() )

        self.__par.add_option( "-r",
                               "--reference", 
                               action="store", 
                               type="string", 
                               dest="reference",
                               metavar="REFERENCE",
                               help="reference root file" )

        self.__par.add_option( "-x", 
                               "--xml",
                               action="store",
                               dest="output",
                               metavar="OUTPUTXML",
                               help="XML output file" )

        self.__par.add_option( "-c", 
                               "--config",
                               type="string",
                               dest="config",
                               metavar="CONFIGXML",
                               help="XML configuration file" )

        self.__par.add_option( "-l", 
                               "--logname",
                               type="string",
                               dest="log",
                               metavar="LOGNAME",
                               help="log file [default %default]" )

        self.__par.add_option( "-t",
                               "--tests",
                               action="callback",
                               callback=self.tests_callback,
                               dest="tests",
                               help="statistics tests to perform: KS(default),chi2,bbb,meany(default),all")

        self.__par.add_option( "-f", 
                               "--pfail",
                               action="callback",
                               type="float",
                               callback=self.check_pfail_callback,
                               dest="pfail",
                               help="p-value limit for test failure [default %default]" )

        self.__par.add_option( "-w", 
                               "--pwarn",
                               type="float", 
                               action="callback",
                               callback=self.check_pwarn_callback,
                               dest="pwarn",
                               help="p-value limit for test warning [default %default]" )

        self.__par.add_option( "-d", 
                               "--date", 
                               action="callback",
                               type="string",
                               callback=self.isodate_callback,
                               dest="isodate",
                               metavar="YYYY-MM-DD",
                               help="date of run in ISO format [default %default]")

        self.__par.add_option( "-s", 
                               "--server",
                               type="string",
                               dest="server",
                               metavar="DCUBESERVER",
                               help="path to DCubeServer")

        self.__par.add_option( "--branch",
                               type="string",
                               action="callback",
                               callback=self.branch_callback,
                               dest="branch",
                               help="branch name [default %default]" )

        self.__par.add_option( "--cmtconfig",
                               type="string",
                               action="callback",
                               callback=self.cmtconfig_callback,
                               dest="cmtconfig",
                               help="CMTCONFIG name [default %default]" )
        
        self.__par.add_option( "--install",
                               type="string",
                               action="callback",
                               callback=self.install_callback,
                               dest="install",
                               help="install name [default %default]" )

        self.__par.add_option( "--project",
                               type="string",
                               action="callback",
                               callback=self.project_callback,
                               dest="project",
                               help="project name [default %default]" )

        self.__par.add_option( "--jobId",
                               type="string",
                               dest="jobId",
                               help="job identification name [default %default]")

        self.__par.add_option( "-g",
                               "--generate",
                               action="store_true",
                               dest="generate",
                               help="trigger for configuration file generation [default off]")
        
        self.__par.add_option( "-p", 
                               "--plots",
                               action="store_true",
                               dest="makeplots",
                               help="trigger for histograms plots generation [default off]")


        self.__par.add_option( "-b",
                               action="store_true",
                               dest="batch",
                               help="batch mode for PyROOT [always on]")

    
        self.__par.set_defaults( pwarn=0.95,
                                 pfail=0.75, 
                                 config="dcube_config.xml",
                                 output="",
                                 log="./dcube.log",
                                 tests=["KS", "meany"],
                                 isodate=today,
                                 server="",
                                 branch="*",
                                 cmtconfig="*",
                                 install="*",
                                 project="*",
                                 jobId="*",
                                 generate=False,
                                 makeplots=False,
                                 root2null=False)



    ## prints out help
    # @param self "Me, myself and Irene"
    def help( self ):
        self.__par.print_help()
        print self.valid

    ## prints out usage
    # @param self "Me, myself and Irene"
    def usage( self ):
        self.__par.print_usage()

    ## prints out version
    # @param self "Me, myself and Irene"
    def version( self ):
        self.__par.print_version()

    ## parses arguments
    # @param self "Me, myself and Irene"
    # @param args list of arguments to parse (default sys.argv[1:])
    # @return tuple ( argumenst, optparse.Values )
    def parse( self, args = sys.argv[1:] ):

        args, opts = self.__par.parse_args( args )
        
        #if ( args.generate and not args.reference ):
        #    print "can't generate DCube config, reference root file not set (use -r, --reference)"
        #    self.usage()
        #    sys.exit(0)
        #if ( ( args.config and not args.generate ) and len(opts) < 2 ):
        #    self.usage()
        #    sys.exit(0)
        
        return ( args, opts )
        


    ##
    # @param self "Me, myself and Irene"
    # @param option option name
    # @param opt_str option long name
    # @param value returned value
    # @param parser OptParser instance
    def check_generation_callback( self, option , opt_str, value, parser ):

        print parser.values.reference
        if ( not parser.values.reference ):
            pass

    ## check pwarn value
    # @param self "Me, myself and Irene"
    # @param option option name
    # @param opt_str option long name
    # @param value returned value
    # @param parser OptParser instance
    def check_pwarn_callback( self, option, opt_str, value, parser ):

        if ( value > 1.0 or value < 0.0 ):
            raise optparse.OptionValueError("%s value %f out of range (0, 1)" % (opt_str, value ) )
        
        if ( ( "-f", "--pfail" ) not in parser.rargs ):
            if ( value <= parser.values.pfail ):
                raise optparse.OptionValueError("value for %s <= p-value failure (%f <= %f)" % ( opt_str, 
                                                                                                 value,
                                                                                                 parser.values.pfail ) )

        setattr( parser.values, option.dest, value )
 
    ## callback for -f and -pfail options
    # @param self "Me, myself and Irene"
    # @param option option name
    # @param opt_str option long name
    # @param value returned value
    # @param parser OptParser instance
    def check_pfail_callback( self, option, opt_str, value, parser ):
      
        if ( value > 1.0 or value < 0.0 ):
            raise optparse.OptionValueError("%s value %f out of range (0, 1)" % (opt_str, value ) )

        if ( ( "-w", "--pwarn" ) not in parser.rargs ):
            if ( value >= parser.values.pwarn ):
                raise optparse.OptionValueError("value for %s >= p-value warning (%f >= %f)" % ( opt_str,
                                                                                                 value, 
                                                                                                 parser.values.pwarn ) )

        setattr( parser.values, option.dest, value )

    ## callback for "-d" and "--date" options
    # @param self "Me, myself and Irene"
    # @param option option name
    # @param opt_str option long name
    # @param value returned value
    # @param parser OptParser instance 
    def isodate_callback( self, option, opt_str, value, parser ):
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

    ## callback for parsing "-t" and "--tests" options
    # @param self "Me, myself and Irene"
    # @param option option name
    # @param opt_str option long name
    # @param value option value
    # @param parser OptParser instance
    def tests_callback( self, option, opt_str, value, parser ):
        assert value is None
        done = 0
        value = []
        rargs = parser.rargs
        while rargs:
            arg = rargs[0]
            if ( (arg[:2] == "--" and len(arg) > 2) or
                 (arg[:1] == "-" and len(arg) > 1 and arg[1] != "-") ):
                break
            else:
                if ( arg in ("KS", "chi2", "bbb", "all", "meany" ) ):
                    value.append(arg)
                    del rargs[0]
                else:
                    raise optparse.OptionValueError("unknown statistics test value '%s'" % arg ) 

        if ( "all" in value ): value = [ "KS", "bbb", "chi2", "meany" ]
        assert len(value) != 0
        setattr( parser.values, option.dest, value )


    ## callback for parsing --branch option
    # @param self "Me, myself and Irene"
    # @param option option string
    # @param opt_str long option name
    # @param value option value
    # @param parser OptParser instance
    def branch_callback(  self, option, opt_str, value, parser ):
        if ( "*" not in value ):
            found = False
            for k, v in self.valid.branches().iteritems():
                if ( v.match( value ) ): 
                    found = True
            if ( not found ):
                print self.valid.listBranches()
                raise optparse.OptionValueError("invalid %s option value '%s'" % (option, value ) ) 
        setattr( parser.values, option.dest, value )
        

    ## callback for parsing --install option
    # @param self "Me, myself and Irene"
    # @param option option string
    # @param opt_str long option name
    # @param value option value
    # @param parser OptParser instance
    def install_callback(  self, option, opt_str, value, parser ):    
        if ( "*" not in value ):
            if ( value not in self.valid.installs() ):
                print self.valid.listInstalls()
                raise optparse.OptionValueError("invalid %s option value '%s'" % (option, value) ) 
        setattr( parser.values, option.dest, value )
        

    ## callback for parsing --cmtconfig option
    # @param self "Me, myself and Irene"
    # @param option option string
    # @param opt_str long option name
    # @param value option value
    # @param parser OptParser instance
    def cmtconfig_callback(  self, option, opt_str, value, parser ):    
        if ( "*" not in value ):
            if ( value not in self.valid.cmtconfigs() ):
                print self.valid.listCmtconfigs()
                raise optparse.OptionValueError("invalid %s option value '%s'" % (option, value) ) 
        setattr( parser.values, option.dest, value )
        
    ## callback for parsing --project option
    # @param self "Me, myself and Irene"
    # @param option option string
    # @param opt_str long option name
    # @param value option value
    # @param parser OptParser instance
    def project_callback(  self, option, opt_str, value, parser ):    
        if ( "*" not in value ):
            if ( value not in self.valid.projects() ):
                print self.valid.listProjects()
                raise optparse.OptionValueError("invalid %s option value '%s'" % (option, value)  ) 
        setattr( parser.values, option.dest, value )
   
        

##
# @class test_DCubeOptParse
# @author Krzysztof Daniel Ciba (Krzsyztof.Ciba@NOSPAMgmail.com)
# @brief test case for DCubeOptParser
class test_DCubeOptParser( unittest.TestCase ):

    ## test suite setup
    # @param self "Me, myself and Irene"
    def setUp( self ):

        self.logger = DCubeLogger( "test_DCubeOptParser.log", logName="test_DCubeParser", toConsole=True )

        opts = [ ]
        args = [ __file__, "-h" ]

        self.opt = DCubeOptParser( )

        self.log = self.logger.getLogger(self)
        
        self.info = self.logger.info
       
     
    ## c'tor
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):
        try:
            self.opt = DCubeOptParser() 
        except:
            pass

        self.assertEqual( isinstance( self.opt, DCubeOptParser), True )

    ## usage(), version(), help()
    # @param self "Me, myself and Irene"
    def test_02_printouts( self ):
        self.opt.version()
        self.opt.usage()
        self.opt.help()

    ## parse()
    # @param self "Me, myself and Irene"
    def test_03_parse( self ):
     

        self.info( "sys.argv[1:] = %s" % str( sys.argv[1:] ) ) 
        args, opts = self.opt.parse()
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
     
        sysExitHolder = sys.exit
        def sysExit( value ):
            print "sys.exit called with value = %s" % str(value)
        sys.exit = sysExit
        
        args = [ __file__, "-h"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )

        args = [ __file__, "--version"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )


        args = [ __file__, "-g"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.generate = %s " % args.generate )


        args = [ __file__, "-c", "configuration.xml"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.config = %s " % args.config )

        args = [ __file__, "-x", "output.xml"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.output = %s " % args.output )

        args = [ __file__, "-r", "reference.xml"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.reference = %s " % args.reference )

        args = [ __file__, "-d", "2008-12-24"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.isodate = %s " % args.isodate )

        args = [ __file__, "-t", "KS", "bbb", "chi2"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.tests = %s " % str(args.tests) )

        args = [ __file__, "-t", "all"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.tests = %s " % str(args.tests) )

        args = [ __file__, "-t", "KS", "bbb"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.tests = %s " % str(args.tests) )

        args = [ __file__, "--date", "2008-02-31"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )

        args = [ __file__, "-w", "0.85"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.pwarn = %s " % str(args.pwarn) )

        args = [ __file__, "-f", "0.65"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.pfail = %s " % str(args.pfail) )

        args = [ __file__, "-f", "-1"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.pfail = %s " % str(args.pfail) )

        args = [ __file__, "-f", "2"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.pfail = %s " % str(args.pfail) )

        args = [ __file__, "-w", "-1"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.pwarn = %s " % str(args.pwarn) )

        args = [ __file__, "-w", "2"]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.pwarn = %s " % str(args.pwarn) )

        args = [ __file__, "-f", "0.65" , "-w", "0.85" ]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
        self.info( "args (after) =%s" % str(args) )
        self.info( "opts (after) =%s" % str(opts) )
        self.info( "args.pfail = %s " % str(args.pfail) )
        self.info( "args.pwarn = %s " % str(args.pwarn) )

        args = [ __file__, "-w", "0.65" , "-f", "0.85" ]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )
     
        args = [ __file__, "-f", "0.75" , "-w", "0.65" ]
        self.info( "args (before) =%s" % str(args) )
        args, opts = self.opt.parse( args )


    def test_04_callback( self ):
        args = [ __file__, "--branch", "blah" ]
        args, opts = self.opt.parse( args )
        
        args = [ __file__, "--branch", "14.2X.Y" ]
        args, opts = self.opt.parse( args )
        self.info( "args.branch = %s" % str(args.branch) )


        args = [ __file__, "--install", "blah" ]
        args, opts = self.opt.parse( args )
        
        args = [ __file__, "--install", "build" ]
        args, opts = self.opt.parse( args )
        self.info( "args.install = %s" % str(args.install) )
        
        args = [ __file__, "--cmtconfig", "blah" ]
        args, opts = self.opt.parse( args )
        
        args = [ __file__, "--cmtconfig", "i686-slc4-gcc34-opt" ]
        args, opts = self.opt.parse( args )
        self.info( "args.cmtconfig = %s" % str(args.cmtconfig) )


        args = [ __file__, "--project", "blah" ]
        args, opts = self.opt.parse( args )
        
        args = [ __file__, "--project", "AtlasProduction" ]
        args, opts = self.opt.parse( args )
        self.info( "args.cmtconfig = %s" % str(args.cmtconfig) )


## test suite execution
if __name__ == "__main__":

    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DCubeOptParser)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    
