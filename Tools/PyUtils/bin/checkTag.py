#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    checkTag.py
# @purpose: Check the differences between 2 tags in CVS
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    July 2006
#
# @example:
#
# python checkTag.py MyTag-00-00-01
#

__version__ = "$Revision: 1.1 $"

from optparse import OptionParser

import sys
import os
import sets
import commands

import logging
class CheckTag:
    LoggerName = "CheckTag"
    pass

from HTMLParser import HTMLParser
class LxrParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.files = []
        pass
    def handle_starttag(self, tag, attrs):
        #print "===> %s" % tag, " --- ", attrs
        if len(attrs) == 2:
            name0, value0 = attrs[0]
            name1, value1 = attrs[1]
            if name0 == "class" and value0 == "find-file" and \
               name1 == "href":
                self.files.append(value1)
                pass
            pass
        pass
    def handle_endtag(self, tag):
        #print "<=== %s" % tag
        pass
    def getFiles(self):
        return self.files
    pass

import urllib
class Lxr(object):
    AtlasServer  = "http://alxr.usatlas.bnl.gov/lxr/search?"
    Repositories = [ 'current', 'gaudi16' ]
    
    def __init__( self, lxrServerUrl = AtlasServer ):
        object.__init__(self)
        self.lxrUrl = lxrServerUrl
        return

    def query( self,
               filePattern = None,
               queryPattern = None,
               repository   = None ):
        log = logging.getLogger("Lxr");
        query = {}
        if repository != None and \
           repository in Lxr.Repositories :
            query['v'] = repository
            pass
        query['filestring'] = filePattern
        query['string']     = queryPattern

        query = urllib.urlencode( query )
        url = urllib.urlopen( "".join( [self.lxrUrl, query] ) )
        log.debug( "URL: %s" % url.geturl() )
        query = url.read()
        return query
    
    pass #Lxr

class CvsOpts:
    ContextDiff = "-c"
    UniDiff     = "-u"
    pass #CvsOpts

class Cvs( object ):

    def __init__( self,
                  CVSROOT=":kserver:atlas-sw.cern.ch:/atlascvs",
                  diffFormat = CvsOpts.UniDiff ):
        self.cvsroot = CVSROOT
        self.cvs = commands.getoutput( "which cvs" )
        self.diffFormat = diffFormat
        return

    def __exec( self, cmdLine ):
        #print "===>",cmdLine
        sc,out = commands.getstatusoutput( "%s %s" % (self.cvs, cmdLine) )
        if sc != 0:
            log = logging.getLogger("PyCvs")
            if sc == 256 and \
               out.count( "cvs rdiff: failed to read diff file header" ) > 0:
                log.warning( "cvs rdiff returned exit-code 256 !!" )
                log.warning( "Maybe because a binary was put into CVS..." )
                log.warning( "Command which failed: [cvs %s]" % cmdLine )
                return out
            else:
                log.error( "CVS command failed !!" )
                log.error( out )
                raise RuntimeError,"Exit code [%i] !!" % sc
        return out
    
    def rdiff( self,
               tag1 = None,
               tag2 = None,
               modules = None ):
        if tag1 == None or tag2 == None or modules == None:
            raise RuntimeError, "Wrong arguments for Cvs.rdiff !!"
        
        return self.__exec( "rdiff %s -r %s -r %s offline/%s" % \
                            ( self.diffFormat,
                              tag1,
                              tag2,
                              modules ) )
    pass # Cvs class

class TagCollector( object ):
    """
    Interface to TagCollector to retrieve tags and the like
    """
    class MsgStrings:
        NoPackage = "This package doesn't exist. "
        pass
    
    def __init__( self ):

        object.__init__(self)
        sc,bin = commands.getstatusoutput( "which get_tag" )
        if sc != 0:
            raise RuntimeError, "Could not fetch 'get_tag': %s" % bin
        
        self.bin = bin
        self.log = logging.getLogger("TagCollector")
        return

    def getTag( self, pkgFullName, release = "" ):
        sc, out = commands.getstatusoutput( "%s %s %s" % ( self.bin,
                                                           pkgFullName,
                                                           release ) )
        if sc != 0:
            raise RuntimeError, "Prbl running 'get_tag': %s" % out

        import re
        pattern = re.compile( r'(?P<ProjectTag>.*?);(?P<ProjectName>.*?);(?P<PkgName>.*?);(?P<PkgTag>.*?)')

        out = [ l for l in out.splitlines() if re.match(pattern, l.strip()) ]
        if TagCollector.MsgStrings.NoPackage in out:
            errMsg = "get_tag could not find [%s]" % pkgFullName
            if release != "":
                errMsg = "".join( [errMsg, " in release [%s]" % release] )
                pass
            self.log.error( errMsg )
            raise RuntimeError, errMsg
                
        if len(out) > 1:
            self.log.warning( "[%s] has more than 1 entry in getTag !!" %
                              pkgFullName )
            self.log.warning( [ "project: %s-%s" % ( l.split(";")[1],
                                                     l.split(";")[0] )
                                for l in out ] )
            pass
        out = re.match( pattern, out[0] )
        projectTag  = out.group( 'ProjectTag' )
        projectName = out.group( 'ProjectName' )
        pkgName     = out.group( 'PkgName' )
        pkgTag      = out.group( 'PkgTag' )

        if pkgName[0] == "/":
            pkgName = pkgName[1:]
            pass
        tag = {
            'project' : { 'name' : projectName,
                          'tag'  : projectTag },
            'pkg'     : { 'name' : pkgName,
                          'tag'  : pkgTag }
            }
        return tag

    def getProject( self, pkgFullName, release = "" ):
        project = self.getTag( pkgFullName, release )['project']
        return "%s-%s" % ( project['name'], project['tag'] )
                           
    pass # TagCollector

class CmtWrapper(object):
    """
    A python wrapper around CMT
    """

    def __init__(self):
        object.__init__(self)
        self.bin = commands.getoutput("which cmt.exe")
        return

    def checkOut(self, pkgFullName, pkgVersion = None ):
        args = pkgFullName
        cmd  = "%s co %s" % ( self.bin, pkgFullName )
        if pkgVersion != None:
            cmd = "%s co -r %s %s" % ( self.bin, pkgVersion, pkgFullName )
            pass
        
        sc,out = commands.getstatusoutput( "%s" % cmd )
        if sc != 0:
            log = logging.getLogger(CheckTag.LoggerName)
            log.warning( "Problem doing 'cmt co' !" )
            log.warning( "Failed to issue [%s]" % cmd )
            log.warning( out )
            pass
        return
    
class TagChecker(object):
    """
    A class to check that a tag someone is requesting on tag-approve is
    satisfying some criteria.
    """

    def __init__( self,
                  pkgTag,
                  pkgFullName = None,
                  refPkgTag   = None,
                  refRelease  = None,
                  checkTagsConsistency = True,
                  doCheckOut = False ):
        """
         - pkgTag is the tag one wants to check for sanity : eg MyPkg-00-00-00
         - pkgFullName is the optional full name of the pkg: Reco/Foo/Bar/MyPkg
        """
        object.__init__(self)

        # fetch logger object
        self.log = logging.getLogger(CheckTag.LoggerName)
        
        self.pkgTag      = pkgTag
        self.pkgFullName = pkgFullName

        self.refPkgTag   = refPkgTag
        self.refRelease  = refRelease

        # to check consistency of tags when both refPkgTag ans refRelease
        # have been configured by the user
        self.checkConsistency = checkTagsConsistency

        # to check-out a package from CVS (for further tests, as compilation)
        self.doCheckOut = doCheckOut
        
        # List of extensions we want to flag as a 'header file'
        self.headersExts = [ '.h', '.hpp', '.hh', '.icc', '.hcc' ]
        
        # We need an access point to the Atlas CVS repository to fetch
        # modifications between a pair of tags
        self.cvs = Cvs()

        # We need an access point to LXR to get the full name of a package
        # if it hasn't been given (hence the 'None')
        self.lxr = None

        # We need an access point to TagCollector to fetch the current
        # tag of a package within a given Release
        self.tc  = TagCollector()

        # We need an access point to CMT in case a package has to be checkout
        # from CVS to be checked for compilation
        self.cmt = CmtWrapper()

        # The list of files which have been modified between the 2 tags
        # we are checking
        self.modifiedFiles = []

        # A dictionary containing our findings wrt the analysis of the
        # package tag we have been configured to check
        self.statusReport = {}
        
        return

    def setPkgTag( self, pkgTag, refRelease = None ):
        """
        reinitialize method in case we want to re-use this TagChecker object
        with a different package tag and (optionaly) with a different
        referent release.
        """
        self.pkgTag      = pkgTag
        self.pkgFullName = None
        self.refPkgTag   = None
        if refRelease != None:
            self.refRelease = refRelease
            pass
        
        self.modifiedFiles = []
        self.statusReport = {}
        return
    
    def check(self):
        """
        The main method to check the sanity of a package tag.
        """
        self.log.info( "#"*80 )
        self.log.info( "...Checking tag set..." )
        
        if self.pkgFullName == None:
            self.retrievePkgFullName()
            pass

        self.retrieveRefPkgTag()
        if self.pkgTag == self.refPkgTag:
            self.printStatusReport()
            self.log.info( "==> Same tags: nothing to compare." )
            return
        
        ## Interogates CVS for modifications between
        ## a pair of tags and retrieves the modified files
        self.fetchModsList()

        ## if there are header files which have been modified
        ## then we fetch the list of clients of this package so
        ## their status wrt the tag being analysed will be also checked
        if self.tagTouchsHeaderFile():
            self.buildClientList()
            pass

        ## Fetchs the package from CVS, using CMT
        if self.doCheckOut:
            self.fetchPkgFromCvs()
        
        ## Now we test that the package at least compiles
        ## as well as its clients (if a header file has been modified)
        self.checkCompilation()

        ## Finally we report our findings
        self.printStatusReport()
        
        return

    def retrievePkgFullName( self ):
        if self.lxr == None:
            self.lxr = Lxr( Lxr.AtlasServer )
            pass

        # pkgTag is like "MyPackage-00-00-00"
        pkgName = self.pkgTag.split("-")[0]

        query = self.lxr.query( filePattern  = "/%s/CVS/Entries" % pkgName,
                                queryPattern = "%s" % pkgName,
                                repository   = "current" )

        
        lxr = LxrParser()
        lxr.feed( query )
        lxrFiles = lxr.getFiles()
        del lxr
        if len(lxrFiles) == 0:
            self.log.warning( "Didn't find any CVS/Entries for [%s]" %
                              pkgName )
            self.log.warning( "Check for a typo in the package tag [%s]" %
                              self.pkgTag )
            self.log.warning( " or for a LXR limitation..." )
            raise RuntimeError, "Can't find the package full name !"
    
        #lxrFiles are like "/lxr/source/atlas/SomePath/MyPackage/CVS/Entries#001"
        lxrFiles = [ f.split("/CVS/Entries")[0]\
                     .split("/lxr/source/atlas/")[1] \
                     for f in lxrFiles ]
        lxrFiles = [f for f in sets.Set(lxrFiles) \
                    if f.count("/%s/%s" % (pkgName,pkgName) ) <= 0]

        if len(lxrFiles) > 0:
            self.pkgFullName = lxrFiles[0]
            pass
    
        if os.path.basename(self.pkgFullName) != pkgName:
            self.log.warning( "INCONSISTENCY:" )
            self.log.warning( "pkgFullName= %s" % self.pkgFullName )
            self.log.warning( "pkgName=     %s" % pkgName )
            pass
        if len(lxrFiles) > 1:
            self.log.warning( "Found more than one CVS/Entries for [%s]" %
                              pkgName )
            for f in lxrFiles:
                self.log.warning( "\t%s" % f )
                pass
            pass
        self.log.info( "Package (full) name: %s" % self.pkgFullName )
        return

    def retrieveRefPkgTag( self ):

        if self.refRelease == None and self.refPkgTag != None:
            # nothing to do
            return

        if self.refRelease == None and self.refPkgTag == None:
            self.log.error( "You have to provide a reference tag !!" )
            self.log.error( " - either through an explicit one," )
            self.log.error( " - or via a reference release" )
            raise RuntimeError, "Invalid State"

        # TODO: check validity of self.refRelease eg: "2.0.2" not "FooBooYou"
        if self.refRelease != None and self.refPkgTag == None:
            self.refPkgTag = self.tc.getTag( self.pkgFullName,
                                             self.refRelease )['pkg']['tag']
            return

        if self.refRelease != None and self.refPkgTag != None:
            refPkgTag = None
            if self.checkConsistency:
                refPkgTag = self.tc.getTag( self.pkgFullName,
                                            self.refRelease )['pkg']['tag']
                pass
            else:
                self.log.warning( "You have setup both :" )
                self.log.warning( " - an explicit reference tag [%s]" %
                                  self.refPkgTag )
                self.log.warning( " - a release reference tag [project-%s]" %
                                  self.refRelease )
                self.log.warning( "...but no consistency is enforced..." )
                return
            
            if refPkgTag != self.refPkgTag and \
               refPkgTag != None:
                self.log.warning( "Inconsistency in reference tags:" )
                self.log.warning( " from release  tag: %s (project-%s)" %
                                  ( refPkgTag, self.refRelease ) )
                self.log.warning( " from explicit tag: %s" % self.refPkgTag )
                self.log.warning( "==> explicit tag WINS !!" )
                pass
            pass
        return

    def fetchModsList( self ):
        self.log.info( "Checking for modified files [%s VS %s]" %
                       ( self.pkgTag,
                         self.refPkgTag ) )

        out = self.cvs.rdiff( tag1 = self.refPkgTag,
                              tag2 = self.pkgTag,
                              modules = self.pkgFullName )
        self.modifiedFiles = []
        lineHdr = "Index: "
        for l in out.splitlines():
            if len(l) > len(lineHdr) and l[:len(lineHdr)] == lineHdr:
                fileName = l.split("Index: offline/")
                if len(fileName) != 2 :
                    self.log.error( "Could not get modified file name:" )
                    self.log.error( l )
                    continue
                fileName = fileName[1]
                self.log.info( " - %s" % fileName )
                self.modifiedFiles.append(fileName)
                pass
            pass
        return

    def isHeaderFile( self, f ):
        """Little helper to decide if a file is considered as a header file"""
        return os.path.splitext(f)[1] in self.headersExts
    
    def tagTouchsHeaderFile( self ):
        return len( [ f for f in self.modifiedFiles \
                      if self.isHeaderFile(f) ] ) > 0

    def buildClientList( self ):
        return

    def fetchPkgFromCvs( self ):
        self.log.info( "Retrieving [%s/%s] from CVS..." % 
                       ( self.pkgFullName, self.pkgTag ) )
        self.cmt.checkOut( pkgFullName = self.pkgFullName,
                           pkgVersion  = self.pkgTag )

        return

    def checkCompilation( self ):
        return

    def printStatusReport( self ):
        self.log.info( "#"*80 )
        if self.refRelease != None:
            self.log.info( "## Ref Project     : %s" %
                           self.tc.getProject( self.pkgFullName,
                                               self.refRelease ) )
        self.log.info( "## Ref release     : %s" % self.refRelease )
        self.log.info( "## Ref package tag : %s" % self.refPkgTag  )
        self.log.info( "## Package name    : %s" % self.pkgFullName )
        self.log.info( "## Package tag     : %s" % self.pkgTag )
        self.log.info( "##" )
        self.log.info( "## Header file(s) affected: %s" %
                       self.tagTouchsHeaderFile() or "No" )
        for f in [ f for f in self.modifiedFiles if self.isHeaderFile(f) ] :
            self.log.info( "##  - %s" % f )
            pass
        self.log.info( "#"*80 )
        return

    def printHeadersDiff( self, fd = sys.stdout ):
        for f in [ f for f in self.modifiedFiles if self.isHeaderFile(f) ] :
            diff = self.cvs.rdiff( tag1    = self.refPkgTag,
                                   tag2    = self.pkgTag,
                                   modules = f )
            print >>fd, diff
            pass
        return
    
    def printDiff( self, fd = sys.stdout ):
        for f in self.modifiedFiles:
            diff = self.cvs.rdiff( tag1    = self.refPkgTag,
                                   tag2    = self.pkgTag,
                                   modules = f )
            print >>fd, diff
            pass
        return
    
    pass # TagChecker

def _installLogger( lvl        = "INFO",
                    loggerName = CheckTag.LoggerName ):
    # define a Handler which writes DEBUG messages or higher to the sys.stderr
    logger = logging.StreamHandler()
    logger.setLevel(logging.DEBUG)
    # set a format which is simpler for console use
    formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
    # tell the handler to use this format
    logger.setFormatter(formatter)
    # add the handler to the root logger
    logging.getLogger('').addHandler(logger)

    log = logging.getLogger(loggerName)
    exec ( """
try:
 log.setLevel( logging.%s )
except AttributeError,err:
 log.warning( 'Invalid logging.Level [%s]' )
 log.warning( 'setting to [INFO]' )
 log.setLevel( logging.INFO )
""" % (lvl, lvl) )
    return

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] MyPkg-00-00-00")
    parser.add_option( "-p",
                       "--pkg-tag",
                       dest = "pkgTag",
                       help = "The pkg tag one wants to check (eg: MyPkg-00-00-01)" )
    parser.add_option( "-P",
                       "--pkg-full-name",
                       dest = "pkgFullName",
                       help = "The full name of the package one wants to check (eg: Deconstruction/MyContainer/MyPkg)" )
    parser.add_option( "-r",
                       "--ref-tag",
                       dest = "refPkgTag",
                       help = "The reference pkg tag (eg: MyPkg-00-00-01)" )
    parser.add_option( "-R",
                       "--ref-rel",
                       dest    = "refRelease",
                       default = "13.0.0",
                       help    = "The reference release (eg: 13.0.0)" )
    parser.add_option( "--show-headers-diff",
                       action  = "store_true",
                       dest    = "showHeadersDiff",
                       default = False,
                       help    = "Switch to dump or not the diff for header files" )
    
    parser.add_option( "--show-diff",
                       action  = "store_true",
                       dest    = "showDiff",
                       default = False,
                       help    = "Switch to dump or not the diff for all files" )
    
    parser.add_option( "--co",
                       action  = "store_true",
                       dest    = "checkOut",
                       default = False,
                       help    = "Switch to check-out or not a package from CVS" )
    
    parser.add_option( "-l",
                       "--loglevel",
                       type    = "string",
                       dest    = "logLevel",
                       default = "INFO",
#                       choices = [ "DEBUG", "INFO", "WARNING", "ERROR" ],
                       help    = "Logging message level [DEBUG, INFO, WARNING, ERROR]"
                       )
    
    (options, args) = parser.parse_args()

    if len(args) > 0 and args[0][0] != "-":
        options.pkgTag = args[0]
        pass

    if options.pkgTag == None:
        str(parser.print_help() or "")
        sys.exit(1)

    ## install the user loglevel
    _installLogger( loggerName = CheckTag.LoggerName,
                    lvl        = options.logLevel )
    log = logging.getLogger( CheckTag.LoggerName )
    
    log.info( "...Checking tags..." )

    log.info( "...checking tag [%s]..." % options.pkgTag )

    tagChecker = TagChecker( pkgTag      = options.pkgTag,
                             pkgFullName = options.pkgFullName,
                             refPkgTag   = options.refPkgTag,
                             refRelease  = options.refRelease )
    tagChecker.check()

    if options.showDiff:
        tagChecker.printDiff()
        tagChecker.printStatusReport()

    if options.showHeadersDiff:
        tagChecker.printHeadersDiff()
        tagChecker.printStatusReport()

    if options.checkOut:
        cmt = CmtWrapper()
        log.info( "Retrieving [%s/%s] from CVS..." %
                  ( tagChecker.pkgFullName, tagChecker.pkgTag ) )
        cmt.checkOut( pkgFullName = tagChecker.pkgFullName,
                      pkgVersion  = tagChecker.pkgTag )
        pass
    
    pass
