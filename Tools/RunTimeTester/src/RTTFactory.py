# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
PS 15/12/08
RTTFactory is part a of a major refactorisation of the RTT,
and is currently work in progress.

We are moving object construction out of the main RTT classes
(Tester, TestRun) and into the factory in order to

- make the code dependencies clearer. Diminish the bag objects
that carries much info to many places.

- clarify the action structure of Tester et al

- allow for shelving of objects to allow quick design turn around
"""

from Tester                    import Tester
from PhasePrinter              import PhasePrinter
from RTTpath                   import RTTpath
from CMTProjectDirCalculator   import CMTProjectDirCalculator
from RTTSException             import RTTCodingError
from RTTSException             import RTTInputError
from createDirectoryDictionary import findSeg
from exc2string2               import exc2string2

import os
import shelve
import sys

from RTTConstructionFactoryHelpers import makeArgDict
from RTTConstructionFactoryHelpers import ArgBag
from RTTConstructionFactoryHelpers import makeArgBag

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------


class RTTFactory:
    def __init__(self, RTTSrcDir, RTTLibDir, topLevelRTTconfFileLocation, db, logDir):
        
        pp = PhasePrinter()

        pp.printIt('getTimes')
        from getTimes import getTimes
        self.startTime1, self.startTime2 = getTimes()

        self.RTTLibDir = RTTLibDir
        self.RTTSrcDir = RTTSrcDir
                    
        
        pp.printIt('validateInputParams')
        
        from validateInputParams import validateInputParams
        validateInputParams(logger,  self.RTTSrcDir, self.RTTLibDir, topLevelRTTconfFileLocation)
            
        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('makeRTTDOM')
        
        from makeRTTDOM import makeRTTDOM
        self.summaryDocument = makeRTTDOM('RTTSummary')
        

        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('readTopLevelConfigFile')

        rttConfig = db.get('rttConfig', None)
        if not rttConfig:
            from  readTopLevelConfigFile import readTopLevelConfigFile
            try:
                rttConfig  = readTopLevelConfigFile(topLevelRTTconfFileLocation, logger)
            except RTTCodingError, e:
                print 'RTTCoding Error while reading top level configuration file'
                print str(e)
            except RTTInputError, e:
                print 'RTTInput Error while reading top level configuration file'
                print str(e)

            db['rttConfig'] = rttConfig        

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('make noPackagePathsDict')
        noPackagePathsDict = {}
        noPackagePathsDict['RTTSrcDir'] = self.RTTSrcDir
        noPackagePathsDict['RTTLibDir'] = self.RTTLibDir
        fillWith = [
            'cmtConfig',
            'build',
            'site',
            'resultsBasePath',
            'resultsBaseURL',
            'disableMailTo',
            'sandboxRunning',
            'deleteNonKeepFiles',
            'devRun',
            'dCubeCfgFile',
            'bigFilesArea',
            'bigFilesSize',
            'pathToVetoFile',
            'chainStoreBasePath',
            'isNightly',
            'rttPilotJobs',
            'atnPilotJobs',
            'mode',
            'rttProvidesConfFiles',
            'localConfFiles',
            'installedConfFiles',
            'versionDir',
            # 'rttRun',
            # 'tctRun',
            # 'fctRun',
            'localPackages',
            'dataSetCatalog',
            'release',
            'releaseType',
            'runType',
            'workBasePath',
            'localRTTRun',
            'originalBranch',
            'branch',
            'otherProject',
            'distArea',
            'topProject',
            'confFileSuffix',
            'confFiles',
            'packagesAccept',
            'packagesVeto',
            'jobGroupConfig',
            'devRun',
            'archivers']

        for k in fillWith: noPackagePathsDict[k] = rttConfig.get(k)

        if rttConfig.get('localConfFiles'):
            noPackagePathsDict['userHomeCmtDir']    = rttConfig.get('userHomeCmtDir')
            noPackagePathsDict['cmtHomeDir']        = rttConfig.get('userHomeCmtDir')
            noPackagePathsDict['testProjectCmtDir'] = rttConfig.get('testProjectCmtDir')
            noPackagePathsDict['localRTTRun']    = True
        else:
            noPackagePathsDict['cmtHomeDir']     = os.path.join(noPackagePathsDict['workBasePath'], 'cmtHome')
            noPackagePathsDict['cmtProjDir']     = os.path.join(noPackagePathsDict['workBasePath'], 'cmtProject')
            noPackagePathsDict['localRTTRun']    = False

        self.noPackagePaths = ArgBag(noPackagePathsDict)

        pathsToCheck = [
            'RTTSrcDir',
            'RTTLibDir',
            'resultsBasePath',
            'dCubeCfgFile',
            'installArea',
            'bigFilesArea',
            'pathToVetoFile',
            'chainStoreBasePath',
            'dataSetCatalog', 
            'userHomeCmtDir',
            'cmtHomeDir',
            'cmtProjDir',
            'workBasePath',
            'distArea'
            ]
        
        pathsToCheck = [self.noPackagePaths.__dict__.get(p, None) for p in pathsToCheck]
        if self.noPackagePaths.rttProvidesConfFiles:
            for cf in self.noPackagePaths.confFiles.values():
                pathsToCheck.append(cf['installArea'])
                pathsToCheck.append(cf['shareArea'])
                pathsToCheck.append(cf['confFilePath'])
                pathsToCheck.append(cf['cmtPath'])

        pathsToCheck = [p for p in pathsToCheck if p]
        badPaths = [p for p in pathsToCheck if not os.path.exists(p)]

        if badPaths:
            m =  'WARNING: bad paths\n'            
            for p in badPaths: m+= '   %s\n' % p
            logger.debug(m)
            
        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('pathSegment')
        try:
            self.noPackagePaths.pathSegment = findSeg(self.noPackagePaths.topProject)
        except:
            m = 'Could not get "segment" for topProject %s, aborting' % self.noPackagePaths.topProject
            logger.error(m)
            raise RTTCodingError(m)

        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('HardCoded')
        from Factory_HardCoded import HardCodedFactory
        self.hardCoded      = HardCodedFactory(logger).create(self.noPackagePaths.releaseType, self.noPackagePaths.runType)

        # --------------------------------------------------------------------------------------------------------        
        
        pp.printIt('Legals')
        from Factory_Legals import LegalsFactory

        self.legals    = LegalsFactory(logger).create(self.noPackagePaths.releaseType,
                                                      self.noPackagePaths.originalBranch,
                                                      self.hardCoded)
        
        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('SiteSpecifics')
        from Factory_SiteSpecifics   import SiteSpecificsFactory
        siteSpecificsFact            = SiteSpecificsFactory(logger)
        self.siteSpecificsObj        = siteSpecificsFact.create(rttConfig.get('site'))

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('PathNameBuilder')
        # to be injected into paths. was sitting in NoPackagePaths, but is not used there
        from Factory_PathNameBuilder    import PathNameBuilderFactory
        factory = PathNameBuilderFactory(logger)

        labels = ['release', 'cmtConfig', 'originalBranch', 'isNightly', 'topProject', 'otherProject', 'distArea']
        argBag = makeArgBag(labels, self.noPackagePaths)
        self.pnb   = factory.create(self.noPackagePaths.releaseType, argBag, self.hardCoded, self.legals, self.siteSpecificsObj)

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('make StampObject')
        from StampObject import StampObject
        stampFile = str(self.pnb.pathToNICOSLatestCopiedLink())
        self.stampObject = StampObject(stampFile)
        
        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('find nightly release')
        if rttConfig.findNightly:
            from findNightlyRelease import findNightlyRelease
            release = findNightlyRelease(self.stampObject, logger)
            if not release: sys.exit(1) # exit, no nightly flag came

            self.noPackagePaths.release = release
            self.pnb.release = release

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('CMTConfigurer')
        from Factory_CMTConfigurer import CMTConfigurerFactory
        factory = CMTConfigurerFactory(logger)

        factoryArgBag = makeArgBag(['runType', 'localRTTRun'], self.noPackagePaths)
        argDict       = makeArgDict(['cmtHomeDir', 'release', 'branch', 'topProject', 'distArea',
                                     'cmtProjDir', 'userHomeCmtDir'], self.noPackagePaths)

        argDict['hardCoded'] = self.hardCoded

        cmtConfigurer = factory.create(factoryArgBag, argDict)

        self.noPackagePaths.cmtVersion = cmtConfigurer.cmtVersion

        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('configure CMT dirs')
        from configureCMTDirs import configureCMTDirs
        configureCMTDirs(self.noPackagePaths.localRTTRun, cmtConfigurer, logger)

        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('make CMTProjectDirCalculator')

        self.projectCMTDirs = db.get('projectCMTDirs', None)
        if not self.projectCMTDirs:
            cmtProjectDirCalculator = CMTProjectDirCalculator(self.noPackagePaths.topProject,
                                                              self.noPackagePaths.originalBranch,
                                                              self.noPackagePaths.release,
                                                              self.noPackagePaths.otherProject,
                                                              self.noPackagePaths.isNightly,
                                                              self.noPackagePaths.cmtConfig,
                                                              self.noPackagePaths.cmtHomeDir,
                                                              logger)
            
            self.projectCMTDirs = cmtProjectDirCalculator.projectCMTDirs
            db['projectCMTDirs'] = self.projectCMTDirs


        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('make results and work directory dictionaries')
        from createDirectoryDictionary import createDirectoryDictionary
        argBag = makeArgBag(['release', 'isNightly', 'originalBranch', 'runType', 'cmtConfig',
                             'topProject', 'otherProject'], self.noPackagePaths)
        argBag.base = self.noPackagePaths.workBasePath
        try:
            self.noPackagePaths.workDirs = createDirectoryDictionary(argBag)
        except Exception, e:
            m  = 'Error calling createDirectoryDictionary: arguments:\n%s\n' % str(e)
            m += 'Exception:\n%s\n' % str(argBag.__dict__)
            m += 'Traceback:\n%s' % exc2string()
            logger.error(m)
            raise RTTCodingError(m)

        argBag.base = self.noPackagePaths.resultsBasePath
        try:
            self.noPackagePaths.resultsDirs = createDirectoryDictionary(argBag)
        except Exception, e:
            m  = 'Error calling createDirectoryDictionary: arguments:\n%s\n' % str(e)
            m += 'Exception:\n%s\n' % str(argBag.__dict__)
            m += 'Traceback:\n%s' % exc2string()
            logger.error(m)
            raise RTTCodingError(m)

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('make closeDownKeyPath')
        
        self.noPackagePaths.RTTSummary     = os.path.join(self.noPackagePaths.resultsDirs['packages'], 'RTTSummary.xml')
        self.noPackagePaths.closeDownFile  = os.path.join(self.noPackagePaths.workDirs['base'], 'closeDownFile.xml')

        if not self.noPackagePaths.isNightly: self.noPackagePaths.branch = 'Numbered' 
        limitedNightlies = [self.legals.nightlies.get(n, n) for n in self.hardCoded.limitedNightlies]
        self.noPackagePaths.limitedNightlyVeto   = not self.noPackagePaths.release in limitedNightlies

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('Outputting PID of this python process')
        from Tools import mkdirIfNew

        basepath = os.path.join(self.noPackagePaths.resultsBasePath,'rtt.prod.pids')
        mkdirIfNew(basepath)
        if not os.path.exists(basepath):
            logger.error('Unable to create inexistant %s' % basepath)
        else:
            my_pid = str(os.getpid())
            pidfile = os.path.join(basepath,my_pid)
            if os.path.exists(pidfile):
                logger.warning('%s: already exists! Doing nothing.' % pidfile)
            else:
                runFrag = self.noPackagePaths.resultsDirs['packages']
                os.system('echo %s %s > %s' % (self.startTime2, runFrag, pidfile))
        
        # --------------------------------------------------------------------------------------------------------
        
        pp.printIt('store NoPackagePaths')            
        db['noPackagePathsArgBag'] = self.noPackagePaths
        

if __name__ == '__main__':
    pass 
    # RTTSrcDir                   = '/Users/peter/RTTdevelopment/Refactor/Directories/src'
    # RTTLibDir                   = '/Users/peter/RTTdevelopment/Refactor/Directories/share' 
    # topLevelRTTconfFileLocation = '/Users/peter/RTTdevelopment/Refactor/Directories/cfg/cfg.xml'
    # logDir                      = '/Users/peter/RTTdevelopment/Refactor/Directories/logs'

    # factory = RTTFactory(RTTSrcDir, RTTLibDir, topLevelRTTconfFileLocation, logDir)
