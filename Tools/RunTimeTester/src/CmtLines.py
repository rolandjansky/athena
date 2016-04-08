# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, os.path
from RTTpath        import RTTpath
from CMTtools       import decodeCMTCONFIG

from RTTSException import RTTCodingError

class CmtLinesBase:
    def __init__(self, logger, argBag, useRunTime=False):

        cmtConfig = argBag.pop('cmtConfig')
        self.__dict__.update(argBag.__dict__)

        self.useRunTime = useRunTime # True=AtlasXXXRunTime dir, False=AtlasXXXRelease dir

        cmtInfo       = decodeCMTCONFIG(cmtConfig)
        self.build    = cmtInfo['build']
        self.nbits    = cmtInfo['nbits']
        self.compiler = cmtInfo['compiler']
        
    def cmtLinesScript(self):
        """
        Returns lines of script needed to set up cmt if this is being
        done from scratch(eg for nightly running)
        Returns an empty string otherwise
        (for running from checked-out code, where it is
        assumed the user has performed the cmt operations.
        """

        # is the package in the local package list declared in the top level
        # RTT config file? Uncaught exception thrown if there is a problem 

        self.checkConditions()
        #lines = self.lines()
        #lines.append('source setup.sh') # needed to setup athena.py
        #return lines
        return self.lines()

    def checkConditions(self):
        """
        check we have made the project build cmt requirements files
        for now I am going to hardwire a lot of the code. This is less than
        ideal as we are assuming what is done in other parts of the code
        (UserStuffRetriever)
        Need to make an USR in order to refind the cmt requirements
        files...
        """
        hExists = os.path.exists(self.cmtHomeDir)
        msg     = ''
        
        if not hExists:
            msg += 'cmtLinesProject no file:  self.cmtHomeDir  = %s\n' % self.cmtHomeDir
            self.logger.error(msg)
            raise RTTCodingError(msg)
                
# -------------------------------------------------------------------------

class CmtLinesBuild(CmtLinesBase):
    def __init__(self, logger, argBag, useRunTime=False):
        CmtLinesBase.__init__(self, logger, argBag, useRunTime)

    def lines(self):
        topProjectPath = str(self.projectCMTDirs[self.topProject][{True:'RunTime',False:'Release'}[self.useRunTime]])
        lines = [
            'unset PYTHONPATH',
            'cd %s' % self.cmtHomeDir,
            'source setup.sh -tag=%s' % self.tags()
            ]
        
        return lines

    def hardCodedFlags(self):
        flags = {('14.1.0.Y', 'AtlasHLT'): 'noTest,noPath,setup,bugfix',
                 ('14.0.10.Y', 'AtlasHLT'): 'noTest,noPath,setup,bugfix'
                 }

        return flags.get((self.originalBranch, self.otherProject), 'noTest,setup')

    def tags(self):
        tags = '%s,%s,%s,%s,%s,%s,%s,%s'  % (self.topProject,
                                             self.otherProject,
                                             self.release,
                                             self.branch,
                                             self.build,
                                             self.nbits,
                                             self.compiler,
                                             self.hardCodedFlags())
        return tags

# -------------------------------------------------------------------------

class CmtLinesBuildHLT(CmtLinesBuild):
    def __init__(self, logger, argBag, useRunTime=False):
        CmtLinesBuild.__init__(self, logger, argBag, useRunTime)

    def hardCodedFlags(self):
        flags = {'14.0.10.Y': 'noTest,noPath,setup,bugfix'}
        return flags.get(self.originalBranch, 'noTest,setup')

    def tags(self):
        tags = '%s,%s,%s,%s,%s,%s,%s,%s' % (
            self.topProject,
            self.release,
            self.otherProject,
            self.branch,
            self.build,
            self.nbits,
            self.compiler,
            self.hardCodedFlags())
        
        return tags
        
# -------------------------------------------------------------------------
        
class CmtLinesLocal(CmtLinesBase):
    def __init__(self, logger, argBag, useRunTime=False):
        CmtLinesBase.__init__(self, logger, argBag, useRunTime)
        self.testProjectCmtDir = argBag.testProjectCmtDir # test project dir
    def checkConditions(self):
        """ do nothing: the existence of the cmt dirs is checked in
        the argBag.__init__"""
        pass

    def lines(self):
        lines = [
            'unset PYTHONPATH',
            'cd %s' % self.userHomeCmtDir,
            'source setup.sh -tag=%s' % self.tags(),
            'cd %s' % str(self.testProjectCmtDir),
            # 'cmt broadcast'
            ]

        return lines

    def tags(self):
        tags = '%s,%s,%s,%s,%s,%s,%s,oneTest,setup' % (self.topProject,
                                                       self.otherProject,
                                                       self.release,
                                                       self.branch,
                                                       self.build,
                                                       self.nbits,
                                                       self.compiler)
        return tags

