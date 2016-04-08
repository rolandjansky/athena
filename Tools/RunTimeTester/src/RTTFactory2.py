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

from Tester                        import Tester
from RTTCloseDownKeyMaker          import RTTCloseDownKeyMaker
from PhasePrinter                  import PhasePrinter
from exc2string2                   import exc2string2
from ATNSummaryRetriever           import ATNSummaryRetriever
from RTTSException                 import RTTCodingError
from RTTSException                 import RTTInputError
from Tools                         import nowSecs

from RTTConstructionFactoryHelpers import makeArgDict
from RTTConstructionFactoryHelpers import ArgBag
from RTTConstructionFactoryHelpers import makeArgBag

import os
import shelve
import time

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------


class RTTFactory2:
    def __init__(self, factory, logDir, db):
        noPackagePaths   = factory.noPackagePaths
        legals           = factory.legals
        hardCoded        = factory.hardCoded
        siteSpecificsObj = factory.siteSpecificsObj
        pnb              = factory.pnb
        stampObject      = factory.stampObject
        RTTSrcDir        = factory.RTTSrcDir
        RTTLibDir        = factory.RTTLibDir
        startTime1       = factory.startTime1
        startTime2       = factory.startTime2
        summaryDocument  = factory.summaryDocument
        projectCMTDirs   = factory.projectCMTDirs
        
        # db = shelve.open('RTTFactory.dbm', 'c')
        
        pp = PhasePrinter()

        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('make BIGIO')
        
        # watch out - has global! was also being set self.bigFileIO in tester, but no more!
        from BigFileIO import createBigFileIO
        self.bigFileIO = createBigFileIO(noPackagePaths.site,
                                         noPackagePaths.bigFilesArea,
                                         noPackagePaths.workDirs,
                                         noPackagePaths.isNightly)
        

        # --------------------------------------------------------------------------------------------------------
                                      
        pp.printIt('make NICOS Dates')
        nicosDate          = 'n/a'
        nicosDateFormatted = 'n/a'

        from NICOS import getReleaseDate
        try:
            nDate = getReleaseDate(pnb)
        except:
            logger.warning('Invalid NICOS date')
        else:
            if nDate.isValid():
                nicosDate          = str(nDate.absTime())
                nicosDateFormatted = str(nDate)

        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('make CMT lines commands')
        from Factory_CmtLines import CmtLinesFactory
        argBagFactory = makeArgBag(['runType','localRTTRun', 'topProject'], noPackagePaths)

        labels = ['release', 'branch', 'originalBranch', 'topProject', 'otherProject', 'cmtHomeDir', 'cmtConfig',
                  'userHomeCmtDir', 'testProjectCmtDir']
        argBag                = makeArgBag(labels, noPackagePaths)
        argBag.projectCMTDirs = projectCMTDirs
        argBag.logger         = logger
        
        cmtLinesFactory = CmtLinesFactory(logger).create(argBagFactory, argBag)
        cmtLinesCmds = cmtLinesFactory.lines()

        # --------------------------------------------------------------------------------------------------------                   
        pp.printIt('make UserStuffRetriever')
        # --------------------------------------------------------------------------------------------------------                   
        usr = None
        if noPackagePaths.installedConfFiles or noPackagePaths.localConfFiles:
            from Factory_UserStuffRetriever import UserStuffRetrieverFactory
            fact       = UserStuffRetrieverFactory(logger)

            factoryArgBag = makeArgBag(['localRTTRun', 'runType', 'release', 'branch'] , noPackagePaths)

            labels = ['cmtConfig', 'topProject', 'otherProject', 'confFileSuffix']
            argBag                = makeArgBag(labels, noPackagePaths)
            argBag.hardCoded      = hardCoded
            argBag.cmds           = cmtLinesCmds[:] # copy, don't just reference. USR appends to this which messes things up otherwise.
            argBag.projectCMTDirs = projectCMTDirs
            
            usr   = fact.create(factoryArgBag, argBag)

        # --------------------------------------------------------------------------------------------------------                   
        pp.printIt('make GetUserData')
        # --------------------------------------------------------------------------------------------------------                   

        from GetUserConfigData          import GetUserConfigData
        labels = ['installedConfFiles', 'localConfFiles', 'rttProvidesConfFiles', 'confFileSuffix', 'confFiles',
                  'packagesAccept', 'packagesVeto', 'cmtHomeDir', 'cmtProjDir']
        argBag        = makeArgBag(labels, noPackagePaths)
        argBag.usr    = usr
        argBag.logger = logger
        argBag.pnb    = pnb
        
        getUserConfigData = GetUserConfigData(argBag)

        # --------------------------------------------------------------------------------------------------------
                               
        pp.printIt('make cvsPackages')
        
        cvsPackages = getUserConfigData.userConfigData()
        
        from cullPackages import cullPackages
        cvsPackages = cullPackages(cvsPackages, logger)
        if len(cvsPackages) == 0:
            m = 'No packages left after RTTFactory::cullPackages(), abandoning RTT run'
            logger.error(m)
            raise RTTInputError(m)
        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('make Kits')
        
        # createKits sets up a Singleton
        from createKits import createKits
        jobGroupKits = createKits(cvsPackages, noPackagePaths.jobGroupConfig, logger)
        # print str(jobGroupKits)
        # assert(False)

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('make joSearchPaths')
        
        from findJobOptionsSearchPath import findJobOptionsSearchPath
        joSearchPaths = db.get('joSearchPaths', None)

        argBag = makeArgBag(['isNightly', 'release'], noPackagePaths)

        if not joSearchPaths:
            try:
                joSearchPaths = findJobOptionsSearchPath(cmtLinesCmds, argBag, logger)
            except Exception, e:
                print e
            else:
                db['joSearchPaths'] = joSearchPaths
        
        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('make Paths')
        from Paths import Paths

        packagePaths = db.get('packagePaths', None)
            
        if not packagePaths:

            argLabels = [
                'RTTSrcDir',
                'RTTLibDir',
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
                'installArea',
                'bigFilesArea',
                'bigFilesSize',
                'pathToVetoFile',
                'chainStoreBasePath',
                'isNightly',
                'rttPilotJobs',
                'atnPilotJobs',
                'mode',
                'dataSetCatalog',
                'rttProvidesConfFiles',
                'localConfFiles',
                'installedConfFiles',
                'versionDir',
                'cmtHomeDir',
                'cmtProjDir',
                'localPackages',
                'workBasePath',
                'release',
                'runType',
                'topProject',
                'otherProject',
                'workDirs',
                'resultsDirs',
                'closeDownFile',
                'originalBranch',
                'branch',
                'devRun',
                'limitedNightlyVeto',
                'localRTTRun',
                'distArea',
                'pathSegment',
                'archivers']

            # make argumentBag an attribute to allow resuse when creating Paths objects
            argBag = makeArgBag(argLabels, noPackagePaths)
            argBag.siteSpecificsObj = siteSpecificsObj
            argBag.legals           = legals
            argBag.projectCMTDirs   = projectCMTDirs
            argBag.hardCoded        = hardCoded
            argBag.pathNameBuilder  = pnb
            argBag.logDir           = logDir

            packagePaths = [(p, Paths(argBag.duplicate(),
                                      p,
                                      joSearchPaths,
                                      summaryDocument.createElement,
                                      summaryDocument.createTextNode,
                                      logger)) for p in cvsPackages]

            for p in packagePaths: p[1].logger = None

        for p in packagePaths: p[1].logger = logger
        
        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('make tester XMLConverter')
        from TesterXMLConverter2 import TesterXMLConverter

        elementCreator  = summaryDocument.createElement
        textNodeCreator = summaryDocument.createTextNode

        labels =['resultsDirs', 'RTTSrcDir', 'RTTLibDir', 'topProject', 'otherProject',
                 'cmtConfig', 'originalBranch', 'site', 'release', 'releaseType', 'runType',
                 'RTTSummary', 'devRun']
        # --------------------------------------------------------------------------------------------------------

        pp.printIt('Tester Logger setup')


        from makeLocalLog                 import makeLocalLog
        testerLogger = makeLocalLog(
            logDir,
            'TesterError.log',
            'TesterWarning.log',
            'TesterInfo.log',
            'TesterDebug.log',
            'Tester'
            )
        

        argBag = makeArgBag(labels, noPackagePaths)
        argBag.legals             = legals
        argBag.logger             = testerLogger
        argBag.startTime1         = startTime1
        argBag.startTime2         = startTime2
        argBag.nicosDate          = nicosDate
        argBag.nicosDateFormatted = nicosDateFormatted
        argBag.node               = summaryDocument.documentElement

        testerXMLConverter = TesterXMLConverter(argBag, elementCreator, textNodeCreator)

        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('make closeDownKeyMaker')
        labels = ['closeDownFile', 'cmtConfig', 'originalBranch', 'release', 'releaseType', 'runType',
                  'topProject', 'otherProject', 'isNightly']
        argBag              = makeArgBag(labels, noPackagePaths)
        argBag.logger       = testerLogger
        argBag.testerStart  = startTime2

        self.closeDownKeyMaker = RTTCloseDownKeyMaker(argBag)

        # --------------------------------------------------------------------------------------------------------
        
        from DataSetCatalog import DataSetCatalog
        dataSetCatalog = DataSetCatalog(noPackagePaths.dataSetCatalog, testerLogger)
        
        # --------------------------------------------------------------------------------------------------------
                   
        pp.printIt('make Factories for TestRuns')

        from Factory_TestRunXMLConverter import TestRunXMLConverterFactory
        xf = TestRunXMLConverterFactory(logger)

        from Factory_JobGroupDirectoryMaker import JobGroupDirectoryMakerFactory
        jobGroupDirectoryMakerFactory = JobGroupDirectoryMakerFactory(logger)

        from Factory_Commander import CommanderFactory
        commander = CommanderFactory(noPackagePaths.mode, logger).create()

        from UnifiedConfigurationTestRun  import UnifiedConfigurationTestRun
        from FailureTestRun               import FailureTestRun
        from Factory_JobDescriptor        import DescriptorFactory
        from Factory_Minder               import MinderFactory
        from ProductionManagerVetoer      import ProductionManagerVetoer
        from Factory_JobsXMLReader        import JobsXMLReaderFactory
        from cullJobDescriptors           import cullJobDescriptors
        from PackageMailer                import PackageMailer
        from ChainStoreMaker              import ChainStoreMaker
        
        def makeTestRun((cvsPackage, paths)):


            pkgName = cvsPackage.name
            pp.printIt('make TestRun: %s' % pkgName)

            labels = ['originalBranch', 'cmtConfig', 'package', 'release', 'runType', 'topProject',  'otherProject']
            closeDownKey = makeArgBag(labels, paths)
            for l in labels: assert(hasattr(closeDownKey, l))

            closeDownKey.runStartTime = startTime2
            if paths.isNightly: closeDownKey.release = 'nightly'

            # make package directories
            directoriesToMake = [paths.workDirs['package'], paths.resultsDirs['package']]

            sleeptimes=[30,60,300]
            for sleeptime in sleeptimes:
                try:
                    [os.mkdir(d) for d in directoriesToMake if not os.path.exists(d)]
                    break
                except:
                    
                    time.sleep(sleeptime)

            # Log files: make directories and logger that routes to 4 files
            pLogDir = os.path.join(logDir, pkgName)
            if not os.path.exists(pLogDir): os.makedirs(pLogDir)


            # make the logger for this test run unique (stops cross talking betweeen testruns for the same package which run in different RTT instances
            # this occcurs while testing the RTT
            time.sleep(1)
            timedPackageName = pkgName+nowSecs() 
            trLogger = makeLocalLog(pLogDir, pkgName+'Error.log', pkgName+'Warning.log', pkgName+'Info.log', pkgName+'Debug.log',timedPackageName)

            # make a mailer pbject to handle emails to developers in case of problems
            mailer = PackageMailer(pkgName,
                                   paths.mailTo,
                                   paths.release,
                                   paths.runType,
                                   paths.originalBranch,
                                   paths.cmtConfig,
                                   paths.pathSegment,
                                   paths.packageTag,
                                   paths.resultsDirs,
                                   paths.resultsBaseURL,
                                   paths.confFile,
                                   paths.disableMailTo,
                                   trLogger)

            # make a Failure TestRun if there is already a known problem
            if not cvsPackage.runJobs:
                logger.warning('Package %s RTT operator veto\n' % pkgName)
                testRun =  FailureTestRun(pkgName, closeDownKey, startTime2, self.closeDownKeyMaker, trLogger, 'RTT operator veto', True)
                xmlConverter = xf.create(testRun,
                                         paths.resultsDirs['package'],
                                         pkgName,
                                         paths.packageTag,
                                         paths.containerPackage,
                                         testerXMLConverter,
                                         elementCreator,
                                         textNodeCreator,
                                         trLogger)
                testRun.setXMLConverter(xmlConverter)
                return testRun
            if not cvsPackage.noError():
                logger.warning('Package %s CVS package error: %s\n' % (pkgName, cvsPackage.errorMessage))
                testRun =  FailureTestRun(pkgName, closeDownKey, startTime2, self.closeDownKeyMaker, trLogger, cvsPackage.errorMessage, False)
                xmlConverter = xf.create(testRun,
                                         paths.resultsDirs['package'],
                                         pkgName,
                                         paths.packageTag,
                                                    paths.containerPackage,
                                         testerXMLConverter,
                                         elementCreator,
                                         textNodeCreator,
                                         trLogger)
                testRun.setXMLConverter(xmlConverter)
                return testRun

            
            labels = ['searchPaths', 'workDirs', 'resultsDirs',  # needed by descriptors
                      'dCubeCfgFile', 'installArea'              # needed by paths
                      ]
            
            argBag                      = makeArgBag(labels, paths)
            argBag.legals               = legals
            argBag.jobOptionsSearchPath = joSearchPaths
            argBag.pathNameBuilder      = pnb  # needed to instantiate paths
            smallPaths                  = Paths(argBag, cvsPackage, paths.searchPaths, elementCreator, textNodeCreator, trLogger)
            descFactory                 = DescriptorFactory(smallPaths, trLogger)

            labels = ['release', 'originalBranch', 'runType', 'cmtConfig', 'topProject', 'isNightly', 'package', 'otherProject']
            argBag                      = makeArgBag(labels, paths) 
            argBag.base                 = paths.chainStoreBasePath
            enableMake                  = True
            chainStoreMaker             = ChainStoreMaker(argBag, enableMake)
            
            # the xml files that contain the user information that species the jobs can come in various flavors
            # (RTT, ATN....) which are parsed by different parsers.
            readerFactory = JobsXMLReaderFactory(logger)
            xmlReader = readerFactory.create(paths.mode,
                                             cvsPackage.packageXMLfile['rtt/jobList[0]'],
                                             paths.rttPilotJobs,
                                             pkgName,
                                             dataSetCatalog,
                                             chainStoreMaker,
                                             descFactory)
            
            descs = xmlReader.getDescriptors()

            try:
                atnNode = cvsPackage.packageXMLfile['atn[0]']
            except:
                pass
            else:                
                xmlReader = readerFactory.create(paths.mode,
                                                 atnNode,
                                                 paths.rttPilotJobs,
                                                 pkgName,
                                                 dataSetCatalog,
                                                 chainStoreMaker,
                                                 descFactory)
                descs.extend(xmlReader.getDescriptors())

            logger.debug('No of descriptors: %d' % len(descs))
            descs = cullJobDescriptors(descs, paths.isNightly, paths.limitedNightlyVeto, paths.rttPilotJobs)
            logger.debug('No of descriptors after vetos: %d', len(descs))



            labels = [
                'workDirs',
                'cmtConfig',
                'RTTSrcDir',
                'RTTLibDir',
                'releasePath',
                'cmtDir',
                'package',
                'pathToProjectCMTDir',
                'topProject',
                'mode'
                ]
            argBag               = makeArgBag(labels, paths)
            argBag.cmtLines      = cmtLinesFactory.cmtLinesScript()
            argBag.siteSpecifics = siteSpecificsObj
            from Factory_ScriptWriter import ScriptWriterFactory
            scriptWriterFactory = ScriptWriterFactory(argBag, logger)
            
            # make an object to allow vetos on the new minders, pass this to a minder factory
            pathToVetoFile = paths.__dict__.get('pathToVetoFile', None)
            jobVetoer     = ProductionManagerVetoer(pathToVetoFile, trLogger)

            labels = ['shareArea',
                      'logDir',
                      'package',
                      'confFile',
                      'bigFilesSize',
                      'bigFilesArea',
                      'deleteNonKeepFiles',
                      'mode',
                      'resultsBasePath',
                      'elementCreator',
                      'textNodeCreator',
                      'release',
                      'workDirs',
                      'isNightly',
                      'site',
                      'dCubeCfgFile',
                      'installArea',
                      'containerPackage',
                      'runType',
                      'cmtPath',
                      'localRTTRun',
                      'distArea',
                      'topProject',
                      'branch',
                      'cmtConfig',
                      'archivers'
                      ]
            argBag                      = makeArgBag(labels, paths)
            argBag.commander            = commander
            argBag.scriptWriterFactory  = scriptWriterFactory
            argBag.RTTLibDir            = RTTLibDir
            argBag.RTTSrcDir            = RTTSrcDir
            argBag.cmtLinesCmds         = cmtLinesCmds

            minderFactory = MinderFactory(jobVetoer,
                                          scriptWriterFactory,
                                          jobGroupDirectoryMakerFactory,
                                          jobGroupKits,
                                          trLogger)

            argBag.minderFactory = minderFactory # used by container minders
            try:
                minders = [minderFactory.create(argBag, d) for d in descs]
            except Exception, e:
                msg =  'Error creating minders:\nException dump starts:\n %s\nTraceBack\n%s \nException dump ends' % (str(e), exc2string2())
                logger.error(msg)
                raise RTTCodingError(msg)


            logger.debug('Made %d minders' % len(minders))
            for m in minders: logger.debug(m.__class__.__name__)
            # we now give the minders to the TestRun, which looks after all the jobs for a package
            testRun =  UnifiedConfigurationTestRun(minders, pkgName, closeDownKey,
                                                   self.closeDownKeyMaker, paths.mode, paths.devRun,
                                                   mailer, trLogger, '')
            # xmlconverter is the summary callback object for TestRuns
            # Needs the tester xmlConverter to set up the call back
            xmlConverter = xf.create(testRun,
                                     paths.resultsDirs['package'],
                                     pkgName,
                                     paths.packageTag,
                                     paths.containerPackage,
                                     testerXMLConverter,
                                     elementCreator,
                                     textNodeCreator,
                                     trLogger)
            
            testRun.setXMLConverter(xmlConverter)
            return testRun

        testRuns = [makeTestRun(p) for p in packagePaths]
        msg = '%d test runs created' % len(testRuns)
        ftrs = [t for t in  testRuns if t.__class__.__name__ == 'FailureTestRun']
        msg +=  ' of which %d were FailureTestRuns' % len(ftrs)
        logger.debug(msg)
        

        # --------------------------------------------------------------------------------------------------------
                                      
        pp.printIt('make ATNretriever')

        atnSummary = None
        try:
            retriever = ATNSummaryRetriever(
                noPackagePaths.cmtConfig,
                noPackagePaths.originalBranch,
                noPackagePaths.release,
                logger)
        except:
            logger.info('Could not instantiate ATNSummaryRetriever %s ' % exc2string2())
        else:
            atnSummary = retriever.getDom()

        
        # --------------------------------------------------------------------------------------------------------
        pp.printIt('add ATNsummary')
        
        if atnSummary:
            deep = True
            importedSummary = summaryDocument.importNode(atnSummary.documentElement, deep)
            summaryDocument.documentElement.appendChild(importedSummary)
        else:
            logger.info('Unable to obtain ATN summary')

        # --------------------------------------------------------------------------------------------------------

        pp.printIt('make Tester')
        
        labels              = ['release', 'branch', 'releaseType', 'runType', 'versionDir', 'cmtConfig',
                               'originalBranch', 'site']
        argBag                 = makeArgBag(labels, noPackagePaths)
        argBag.stampObject     = stampObject
        argBag.legals          = legals
        argBag.summaryDocument = summaryDocument
        argBag.logger          = testerLogger
        argBag.xmlConverter    = testerXMLConverter 
        argBag.testRuns        = testRuns
        argBag.startTime2      = startTime2
        
        self.tester = Tester(argBag)

        # --------------------------------------------------------------------------------------------------------
                                   
        pp.printIt('Done creating objects')

        # db.close()


if __name__ == '__main__':
    pass
