#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    getTagDiff.py
# @purpose: Get the list of tag differences between 2 releases (CERN centric)
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    September 2006
#
# @example:
#
# getTagDiff rel_3,12.0.X 12.0.3
#

__version__ = "$Revision: 1.4 $"

from optparse import OptionParser

import sys
import os
import commands

import logging
class GetTagDiff:
    LoggerName = "GetTagDiff"
    ReleaseRoot = "/afs/cern.ch/atlas/software/builds"
    Projects = [
#        "AtlasPoint1",
        "AtlasProduction",
        "AtlasOffline",
        "AtlasAnalysis",
        "AtlasTrigger",
        "AtlasReconstruction",
        "AtlasSimulation",
        "AtlasEvent",
        "AtlasConditions",
        "DetCommon",
        "AtlasCore"
        ]
    pass

try:
    import cPickle as pickle
except ImportError:
    import pickle
    pass

class CmtStrings:
    CMTPATH        = 'CMTPATH'
    CMTDIR         = 'cmt'
    CMTVERSIONFILE = 'version.cmt'
    CMTREQFILE     = 'requirements'
    CMTPROJFILE    = 'project.cmt'
    pass

class CmtPkg:

    def __init__( self,
                  pkgName    = 'Example',
                  pkgVersion = 'Example-00-00-00',
                  pkgPath    = '' ):
        self.name    = pkgName
        self.version = pkgVersion
        self.path    = pkgPath

        return
    def __repr__( self ):
       s = [ "Package: %s" % self.name,
             "Version: %s" % self.version,
             "Path:    %s" % self.path ]
       return os.linesep.join(s)
    
    pass

def listCmtDirs( path ):

    log = logging.getLogger( GetTagDiff.LoggerName )
    
    cmtDirs = []
    
    # fill list of CMT directories
    try:
        ls = os.listdir(path)
    except OSError, what:
        log.error( "OSError: %s" % what )
        return cmtDirs

    subDirs = []
    for n in ls:
        fullName = os.path.join( path, n )

        # hack to optimize searching 
        if fullName.count( "i686-" )        <= 0 and \
           fullName.count( "o..pacman..o" ) <= 0 and \
           fullName.count( "InstallArea"  ) <= 0 and \
           os.path.splitext( fullName )[1] != '.so' :
            isDir = os.path.isdir(fullName)
        else:
            isDir = False
            pass
        
        isCmtDir =  os.path.split(fullName)[1] == CmtStrings.CMTDIR

        if isDir :
            if isCmtDir:
                log.debug( "\t==> found %s" % fullName )
                cmtDirs.append(fullName)
                isDir = True
                pass
            else:
                subDirs.append( fullName )

        pass
    
    # now take care of the subdirs:
    for subDir in subDirs:
        subDirName = os.path.split(subDir)[1]
        if subDirName.count( "i686-") <= 0 and \
           subDirName.count( "o..pacman..o" ) <= 0 and \
           subDirName != 'CVS' and \
           os.path.splitext( subDirName )[1] != '.so' :
            cmtDirs.extend( listCmtDirs( subDir ) )
        pass
    
    return cmtDirs

def createCmtPkg( cmtDir, printWrongPkgNames = True ):
    """
    the cmtDir is assumed to be of the form Xyz/cmt
    One has also to handle the case with or without version-directory
    """
    log = logging.getLogger(GetTagDiff.LoggerName)
    
    pkgName = None
    try:
        # the CMTVERSIONFILE should provide the name of the package
        # so we extract it from this file
        fileName = os.path.join( cmtDir, CmtStrings.CMTVERSIONFILE )
        if os.path.exists(fileName):
            versionFile = open( fileName, "r" )
            pkgName = versionFile.readlines()[0].strip()
            # a valid pkgName is of the form PkgName-??-??-??[-??]
            pkgName = pkgName.split("-")[0]
            versionFile.close()
            del versionFile
        else:
            # the CMTREQFILE should (also) provide the name of the package
            # so we extract it from this file
            fileName = os.path.join( cmtDir, CmtStrings.CMTREQFILE )
            reqFile = open( fileName, "r" )
            for line in reqFile.readlines():
                line = line.strip()
                if len(line) > 0  and \
                       line[0] != "#" and \
                       line.count("package ") > 0:
                    pkgName = line.splitlines()[0]\
                              .split("package ")[1]\
                              .replace("\r","")\
                              .split("#")[0]\
                              .strip()
                    break
                pass
            reqFile.close()
            del reqFile
    except IOError:
        ## No CMTREQFILE in this directory
        ## ==> not a CMT package then ?
        ## check if there is any CMT project file instead
        if not os.path.exists( os.path.join(cmtDir, CmtStrings.CMTPROJFILE) ):
            log.warning( "[%s] does NOT contain any '%s' nor '%s' file !!" % \
                         ( cmtDir,
                           CmtStrings.CMTREQFILE,
                           CmtStrings.CMTPROJFILE ) )
        return None

    if pkgName == None:
        log.warning( "No 'package Foo' line in %s of %s" % \
                     ( CmtStrings.CMTREQFILE, cmtDir ) )
        return None
    
    log.debug( "\t\t==> Analysing [%s]" % cmtDir )
    
    # first we try the no-version-directory case as it is the ATLAS
    # default now.
    if CmtStrings.CMTVERSIONFILE in os.listdir(cmtDir):
        version = open( os.path.join( cmtDir, CmtStrings.CMTVERSIONFILE ),
                        'r' )\
                        .readline()
        version = version.splitlines()[0].strip()
        pkgDir = os.path.split(cmtDir)[0].strip()
        pkgPath = os.path.split(pkgDir)[0].strip()
        pass

    # Now we are in the case where:
    # /somePath/MyPkg/MyPkg-00-00-00/cmt
    # or
    # /somePath/MyPkg/v1r2p3/cmt
    else:
        baseDir = os.path.split(cmtDir)[0].strip()
        pkgDir, version = os.path.split(baseDir)
        pkgPath = os.path.split(pkgDir)[0].strip()

        pass

    log.debug( "\t\t\t- name    = %s" % pkgName )
    log.debug( "\t\t\t- version = %s" % version )
    log.debug( "\t\t\t- path    = %s" % pkgPath )

    if pkgName.count(os.sep) > 0 and printWrongPkgNames :
       log.warning( "About to create a funny CMT package !" )
       log.warning( "'PkgName' contains '%s'. Please fix it!" % os.sep )
       log.warning( "\t- name    = %s" % pkgName )
       log.warning( "\t- version = %s" % version )
       log.warning( "\t- path    = %s" % pkgPath )
       # Ok, so, I fix it - but user is warned...
       pkgName = os.path.basename(pkgName)
       pass
    
    #version = '*'
    return CmtPkg( pkgName, version, pkgPath )

def scan( scanDir = os.curdir,
          printWrongPkgNames = True,
          suppressList = ["WorkArea"] ):
    """Search for CMT packages in the given directory and walk down the
    directory tree.
    Return the list of found CMT packages.
    """
    log = logging.getLogger( GetTagDiff.LoggerName )
    log.debug( "Scanning [%s]" % scanDir )
    
    # return value
    cmtPackages = []
    
    # retrieve all cmt-ised directories in the scan directory
    scanDir = os.path.abspath( scanDir )

    cmtDirs = []
    try:
        cmtDirs = listCmtDirs(scanDir)
    except KeyboardInterrupt:
        log.warning( "Scanning has been STOPPED ! (by you)" )
        pass
    
    for cmtDir in cmtDirs:
        cmtPkg = createCmtPkg(cmtDir, printWrongPkgNames)
        if cmtPkg != None and \
           cmtPkg.name not in suppressList:
            cmtPackages.append( cmtPkg )
        pass
    
    return cmtPackages


def getRelease( releaseId = "12.0.3", releaseRoot = GetTagDiff.ReleaseRoot ):
    # fetch logger object
    log = logging.getLogger( GetTagDiff.LoggerName)

    id = releaseId.split(",")
    #print "## id [%s] length: %i" % (str(id), len(id))
    if releaseId.count("rel_") > 0 :
        if releaseId.lower().count("bugfix")  > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "bugfix" )
        elif releaseId.lower().count("cos")   > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "cos" )
        elif releaseId.lower().count("devmig0")>0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "devmig0" )
        elif releaseId.lower().count("devval")> 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "devval" )
        elif releaseId.lower().count("dev")   > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "dev" )
        elif releaseId.lower().count("lcg")   > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "lcg" )
        elif releaseId.lower().count("lst")   > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "lst" )
        elif releaseId.lower().count("mig0")  > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "mig0" )
        elif releaseId.lower().count("mig1")  > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "mig1" )
        elif releaseId.lower().count("trials")> 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "trials" )
        elif releaseId.lower().count("val")   > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "val" )
        elif releaseId.lower().count("pcache")   > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "pcache" )
        elif releaseId.count(".0.X")  > 0 :
            releaseRoot = os.path.join( releaseRoot, "nightlies", "bugfix" )
        elif releaseId.count(".X.0") > 0:
            releaseRoot = os.path.join( releaseRoot, "nightlies", "dev" )
        else:
            log.warning( "Unknown release id [%s]" % releaseId )
        for i in id:
            if i.count("rel_") > 0:
                releaseId = i
                break
    else:
        if len(id) == 1:
            releaseId = id[0]
        else:
            log.warning( "Wrong release id [%s]" % releaseId )
        pass
    log.debug( "## releaseRoot: %s" % releaseRoot )
    log.debug( "## releaseId  : %s" % releaseId   )
    
    return releaseRoot,releaseId

class Release(object):

    def __init__(self, id, projs = GetTagDiff.Projects ):
        object.__init__(self)
        self.rawId  = self.__getRawId(id)
        self.topDir, self.id = getRelease(id)
        self.projs  = projs

        self.printWrongPkgNames = False
        
        # fetch logger object
        self.log = logging.getLogger(GetTagDiff.LoggerName)

        self.cmtPkgs = None
        self.cachedCmtPkgs = False
        return

    def __getRawId(self, id):
        # 'normalize' the rawId : always 12.0.X-rel_? (not rel_?-12.0.X)
        ids = id.split(",")
        ids.sort()
        id = ",".join( ids )
        return id
    
    def setProjects(self, projs):
        self.projs = projs
        return

    def getProjectDirs(self):
        def getProjectVersion(projName, projVersion):
            version = projVersion
            if projVersion.count("rel_") <= 0:
                if projName in GetTagDiff.Projects[2:]:
                    version = projVersion[1:]
                    pass
                pass
            return version
        
        return [
            os.path.join( self.topDir,
                          p,
                          getProjectVersion(p, self.id)) for p in self.projs
            ]

    def getCmtDirs(self):
        cmtDirs = []
        for d in self.getProjectDirs():
            self.log.info( "## scanning [%s]..." % \
                           os.path.basename( os.path.dirname(d) ) )
            cmtDirs += listCmtDirs(d)
            pass
        return cmtDirs

    def buildCmtPkgsList(self):
        self.log.info( "Building list of CMT pkgs [%s]..." % \
                       self.rawId )
        self.cachedCmtPkgs = False
        self.cmtPkgs = {}
        cmtPkgs = []
        cmtDirs = self.getProjectDirs()
        for cmtDir in cmtDirs:
            self.log.info( "...Scanning [%s]..." % cmtDir )
            cmtPkgs += scan(cmtDir, self.printWrongPkgNames)
            pass
        for cmtPkg in cmtPkgs:
            # don't bother with container pkg
            if self.isContainerPkg(cmtPkg):
                continue
            
            if self.cmtPkgs.has_key(cmtPkg.name):
                old = self.cmtPkgs[cmtPkg.name]
                self.log.warning( "About to replace this pkg:" )
                self.log.warning( " - name:    %s" % old.name )
                self.log.warning( " - version: %s" % old.version )
                self.log.warning( " - path:    %s" % old.path )
                new = cmtPkg
                self.log.warning( "with this pkg:" )
                self.log.warning( " - name:    %s" % new.name )
                self.log.warning( " - version: %s" % new.version )
                self.log.warning( " - path:    %s" % new.path )
                pass
            self.cmtPkgs[cmtPkg.name] = cmtPkg
        self.log.info( "Building list of CMT pkgs [%s]... [OK]" % \
                       self.rawId )
        self.saveCmtPkgs()
        return

    def getCmtPkgs(self):
        if not self.cachedCmtPkgs:
            self.buildCmtPkgsList()
            pass
        return self.cmtPkgs

    def getPkgFullName(self, cmtPkg):
        projId  = self.id
        if projId.count("rel_") <= 0 :
            projId = projId[1:]
            pass
        pkgName = cmtPkg.name
        if cmtPkg.path.endswith(projId):
            return pkgName
        pkgFullName = cmtPkg.path.split(projId+os.sep)[1]
        pkgFullName = os.path.join( pkgFullName, pkgName )
        return pkgFullName
    
    def isContainerPkg(self, cmtPkg):
        # this method assumes that a tag-name for a container pkg is of
        # the form:
        # ContainerName-AtlasProject-00-00-00[-00]
        isContainer = False
        version = cmtPkg.version
        for p in self.projs:
            if version.count( "-%s-" % p ) > 0:
                isContainer = True
                self.log.debug("Flagged Container pkg [%s] (%s)" %
                               ( self.getPkgFullName(cmtPkg),
                                 cmtPkg.path ) )
                break
        return isContainer
    
    def saveCmtPkgs(self, fileName = None):
##         if fileName == None:
##             fileName
        fileName = os.path.join( os.path.expandvars( os.environ['HOME'] ),
                                 ".pane",
                                 "cmtTags-%s.db" % self.rawId.replace(",",
                                                                      "-") )
        if not os.path.exists( os.path.dirname(fileName) ):
            os.makedirs( os.path.dirname(fileName) )
            pass

        self.log.debug( "Saving CMT pkgs list to [%s]..." % fileName )
        self.cachedCmtPkgs = True
        f = open( fileName, "wb" )
        pickle.dump(self.getCmtPkgs(), f)
        f.close()
        self.log.info( "Saving CMT pkgs list to [%s]... [OK]" % fileName )
        
        return
    
    def loadCmtPkgs(self, fileName = None):
        if fileName == None:
            fileName = os.path.join( os.path.expandvars( os.environ['HOME'] ),
                                     ".pane",
                                     "cmtTags-%s.db" % \
                                     self.rawId.replace(",", "-")
                                     )
        if not os.path.exists( fileName ):
            self.log.error( "No such file [%s]" % fileName )
            return -1
        
        # we don't want to read a tag-db which is deprecated
        # hence we read the last modified time of SomeProject/cmt/project.cmt
        projFile = os.path.join( self.getProjectDirs()[0],
                                 "cmt",
                                 "project.cmt" )
        if os.path.getmtime( fileName ) <= \
           os.path.getmtime( projFile ):
            self.log.warning( "[%s] is older than [%s]" %
                              ( fileName, projFile ) )
            return -1
        
        self.log.info( "Loading CMT pkgs list from [%s]..." % fileName )

        f = open( fileName, "r" )
        self.cmtPkgs = pickle.load(f)
        f.close()
        
        return len(self.cmtPkgs.keys())

    pass # Release

class ReleaseCmp(object):

    def __init__(self, ref, rel, rescan = False):
        self.ref    = ref
        self.rel    = rel
        self.rescan = rescan
        self.log    = logging.getLogger(GetTagDiff.LoggerName)
        self.fmt    = "%-45s : %-25s -> %-25s"
        return

    def cmp(self, ref = None, rel = None):
        if ref == None:
            ref = self.ref
            pass
        if rel == None:
            rel = self.rel
            pass

        self.log.info( "Comparing [%s VS %s]" % (ref.rawId, rel.rawId) )

        if self.rescan:
            self.log.info( "Rescan has been requested: doing it..." )
            ref.buildCmtPkgsList()
            rel.buildCmtPkgsList()
            pass
        
        if ref.loadCmtPkgs() <= 0:
            self.log.warning( "==> will rebuild cache file..." )
            ref.buildCmtPkgsList()
        if rel.loadCmtPkgs() <= 0:
            self.log.warning( "==> will rebuild cache file..." )
            rel.buildCmtPkgsList()

        diff = {}

        # first compare the list of packages registered in the reference
        for pkgName in ref.cmtPkgs.keys():

            # check if it is a container package
            if ref.isContainerPkg(ref.cmtPkgs[pkgName]):
                continue

            pkgFullName = ref.getPkgFullName(ref.cmtPkgs[pkgName])
            
            if not rel.cmtPkgs.has_key(pkgName):
                diff[pkgFullName] = { 'ref' : ref.cmtPkgs[pkgName],
                                      'rel' : CmtPkg( "None",
                                                      "None-00-00-00",
                                                      "-" ) }
                pass
            else:
                refVersion = ref.cmtPkgs[pkgName].version
                version    = rel.cmtPkgs[pkgName].version
                if refVersion != version:
                    diff[pkgFullName] = { 'ref' : ref.cmtPkgs[pkgName],
                                          'rel' : rel.cmtPkgs[pkgName] }
                pass
            pass

        # then compare the list of packages registered in the release
        for pkgName in rel.cmtPkgs.keys():

            # check if it is a container package
            if rel.isContainerPkg(rel.cmtPkgs[pkgName]):
                continue

            pkgFullName = rel.getPkgFullName(rel.cmtPkgs[pkgName])

            if not ref.cmtPkgs.has_key(pkgName):
                diff[pkgFullName] = { 'ref' : CmtPkg( "None",
                                                      "None-00-00-00",
                                                      "-" ),
                                      'rel' : rel.cmtPkgs[pkgName] }
                pass
            else:
                refVersion = ref.cmtPkgs[pkgName].version
                version    = rel.cmtPkgs[pkgName].version
                if refVersion != version:
                    diff[pkgFullName] = { 'ref' : ref.cmtPkgs[pkgName],
                                          'rel' : rel.cmtPkgs[pkgName] }
                pass
            pass

        self.diff = diff
        self.saveReport()
        return diff

    def printReport(self):
        self.log.info( "#" * 80 )
        self.log.info( "Found [%i] tags which are different !" %
                       len(self.diff.keys()) )
        self.log.info( self.fmt % ( "          PKGNAME",
                                    " PKG-REF",
                                    " PKG-REL" ) )
        self.log.info( "-" * 80 )
        for k in self.diff.keys():
            pkgFullName = k
            pkgName = os.path.basename(pkgFullName)
            self.log.info( self.fmt %
                           ( pkgFullName,
                             self.diff[k]['ref'].version.replace(pkgName,""),
                             self.diff[k]['rel'].version.replace(pkgName,"") )
                           )
            pass
        self.log.info( "-" * 80 )
        return

    def saveReport(self, fileName=None):
        refId = self.ref.rawId.replace(",","-")
        relId = self.rel.rawId.replace(",","-")
        if fileName == None:
            fileName = os.path.join( os.getcwd(),
                                     "tag-diff-%s-vs-%s.out" % \
                                     ( refId, relId ) )
            pass
        out = open( fileName, "w" )
        lines = []
        lines += [
            "-" * 80,
            "## Found [%i] tags which are different !" % len(self.diff.keys()),
            self.fmt % ( "##       PKGNAME",
                         " PKG-REF (%s)" % refId,
                         " PKG-REL (%s)" % relId ),
            "-" * 80
            ]
        for k in self.diff.keys():
            pkgName = k
            lines += [
                self.fmt %
                ( pkgName,
                  self.diff[k]['ref'].version.replace(pkgName,""),
                  self.diff[k]['rel'].version.replace(pkgName,"") )
                ]
            pass
        lines += [ "-" * 80 ]
        for line in lines:
            out.writelines( line + os.linesep )
            pass
        out.close()
        return
        
def _installLogger( lvl        = "INFO",
                    loggerName = GetTagDiff.LoggerName ):
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

    parser = OptionParser( usage = "usage: %prog [-R|--ref] 12.0.X,rel_3 [-r|--rel] 12.0.3")
    parser.add_option( "-R",
                       "--ref",
                       dest = "ref",
                       help = "The description string of the reference release (eg: 12.0.X,rel_3)" )

    parser.add_option( "-r",
                       "--rel",
                       dest = "rel",
                       help = "The description string of the to-be-compared release (eg: 12.0.X,rel_3 or 12.0.3)" )

    
    parser.add_option( "-f",
                       "--force-rescan",
                       action  = "store_true",
                       dest    = "rescan",
                       default = False,
                       help    = "Switch to rescan the whole release or look for tags from a previous db-file" )

    
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
        options.ref = args[0]
        pass

    if len(args) > 1 and args[1][0] != "-":
        options.rel = args[1]
        pass

    if options.ref == None or options.rel == None:
        parser.print_help()
        sys.exit(1)
        pass
    
    ## install the user loglevel
    _installLogger( loggerName = GetTagDiff.LoggerName,
                    lvl        = options.logLevel )
    log = logging.getLogger( GetTagDiff.LoggerName )

    log.info( "#" * 80 )
    log.info( "## Welcome to getTagDiff" )
    log.info( "#" * 80 )
    log.info( "##" )

    relRef = Release(options.ref)
    rel    = Release(options.rel)
    log.info( "## Comparing [%s] VS [%s]" % ( relRef.rawId, rel.rawId ) )

    relCmp = ReleaseCmp( relRef, rel, options.rescan )
    relCmp.cmp()
    relCmp.printReport()
    
    log.info( "...Bye..." )
