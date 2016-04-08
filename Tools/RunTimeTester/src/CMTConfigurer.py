# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tools   import rmdirMkdir
from RTTpath import RTTpath
import os, string

# -------------------------------------------------------------------------

from MethodTracer        import MetaTracer

# __metaclass__ = MetaTracer
    
"""
CMTConfigurer. Configures the CMT directories. Makes them first, if necessary
"""

def massageCmd(cmd):
    # remove excess white space, make separator = ';' 
    tokens = cmd.split()
    cmd = string.join(tokens)
    tokens = cmd.split('; ')
    cmd = string.join(tokens,';')
    return cmd

def pathUpToReleaseArea(branch, isNightly, distArea, hardCoded, logger):    
    return os.path.join(str(distArea), hardCoded.nightlyDirIfAny(branch,isNightly))

def getCMTversionFromReleaseFile(release, branch, isNightly, topProject, distArea, hardCoded, logger):
    pathToFile   = RTTpath(pathUpToReleaseArea(branch,isNightly, distArea, hardCoded, logger),
                           topProject,
                           release,
                           topProject+'Release/cmt/setup.sh')
    searchString = '/afs/cern.ch/sw/contrib/CMT/'
    version = 'v1r18p20060606' # default
    logger.debug('Looking for CMT version, setup.sh file: %s' % str(pathToFile))

    if pathToFile.exists():
        try:
            logger.debug('Looking for CMT version, setup.sh file %s found' % str(pathToFile))
            
            ifile = open(str(pathToFile),'r')
            lines = ifile.readlines()
            ifile.close
            
            for l in lines:
                if l.find(searchString)!=-1:                        
                        version = l[l.rfind('/')+1:l.rfind(';')]
                        logger.debug('Using release CMT version: %s' % version)
        except Exception, e:
            logger.warning('Problem grepping setup.sh file for CMT version')
            logger.warning('Using default CMT version: %s' % version)
    else:
        logger.warning('%s does not exist.' % str(pathToFile))
        logger.warning('Using default CMT version: %s' % version)

    return version

class CMTConfigurerBase:

    def __init__(self, argDict, logger):
        logger.debug('%s.__init__ starts'% self.__class__.__name__)

        # must haves...
        self.release    = argDict.pop('release')
        self.branch     = argDict.pop('branch')
        self.topProject = argDict.pop('topProject')
        self.distArea   = argDict.pop('distArea')
        self.hardCoded  = argDict.pop('hardCoded')

        self.__dict__.update(argDict)
        self.logger                 = logger
        self.cmtVersion             = self.getCMTversion()
        # paramsForCMT.cmtVersion     = self.cmtVersion
        
    def getCMTversion(self):
        isNightly  = True
        if self.release[0] in string.digits: isNightly = False
        return getCMTversionFromReleaseFile(self.release,
                                            self.branch,
                                            isNightly,
                                            self.topProject,
                                            self.distArea,
                                            self.hardCoded,
                                            self.logger)

    def makeHomeCMTDir(self): pass # override for sub classes


class CMTConfigurerBuild(CMTConfigurerBase):
    def __init__(self, argDict, logger):

        cmtProjDir      = argDict.pop('cmtProjDir')
        self.cmtHomeDir = argDict.pop('cmtHomeDir')
        CMTConfigurerBase.__init__(self, argDict, logger)

        ### NOTE: ATLAS_DIST_AREA is not the dist area as we know it i.e. the base in
        ### which you find releases, rather it is the path to the AtlasLogin package.
        ### It is a constant, hence hardcoded.
        self.hReq  = 'set CMTSITE CERN\n'
        self.hReq += 'set SITEROOT /afs/cern.ch\n'
        self.hReq += 'macro ATLAS_DIST_AREA $(SITEROOT)/atlas/software/dist\n'
        self.hReq += 'macro ATLAS_TEST_AREA %s\n' %  cmtProjDir
        self.hReq += 'use AtlasLogin AtlasLogin-* $(ATLAS_DIST_AREA)\n'

    def makeHomeCMTDir(self):
        dir = str(self.cmtHomeDir)
        rmdirMkdir(dir)
        ofile = open(os.path.join(dir,'requirements'), 'w')
        ofile.write(self.hReq)
        ofile.close()        
    
    def configure(self):
        msg = 'Configure CMT for release %s\n' % self.release
        self.logger.debug(msg)

        r  = """cd %s;
        source /afs/cern.ch/sw/contrib/CMT/%s/mgr/setup.sh>& /dev/null;
        cmt config >& /dev/null;
        """ % (self.cmtHomeDir,
               self.cmtVersion)

        r = massageCmd(r)
        self.logger.debug('configure command %s' % r)
        return r
    
# ===============================================================

class CMTConfigurerKit(CMTConfigurerBase):
    def __init__(self, argDict, logger):
        CMTConfigurerBase.__init__(self, argDict, logger)

    def configure(self):
        msg = 'Setting up CMT for kit, release %s\n' % self.release
        self.logger.debug(msg)
        setup  = str(self.distArea)
        setup += '/cmtsite/setup.sh '
        return setup

# ===============================================================

class CMTConfigurerLocal(CMTConfigurerBase):
    'User does all, nothing to be done.'    
    def __init__(self, argDict, logger):
        self.userHomeCmtDir = argDict['userHomeCmtDir']
        CMTConfigurerBase.__init__(self,  argDict, logger)       
        
    def configure(self): pass
        
    def getCMTversion(self): return self.userHomeCmtDir
