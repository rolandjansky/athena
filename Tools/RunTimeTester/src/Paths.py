# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from formatCollection             import formatCollection
from Tools                        import mkdirIfNew
from extractDCubeBaseDir          import extractDCubeBaseDir
from RTTSException                import RTTCodingError
from RTTSException                import RTTInputError
import os
# -------------------------------------------------------------------------

class Paths:
    def __init__(self, noPackagePathsArgBag,
                 cvsPackage, searchPaths,
                 eCreator, tCreator, logger):

        self.dCubeCfgFile    = noPackagePathsArgBag.pop('dCubeCfgFile')
        self.pathNameBuilder = noPackagePathsArgBag.pop('pathNameBuilder')

        self.workDirs        = noPackagePathsArgBag.pop('workDirs')
        self.resultsDirs     = noPackagePathsArgBag.pop('resultsDirs')

        self.__dict__.update(noPackagePathsArgBag.__dict__)
        self.logger = logger

        # cvsPackage is of type CVSPackage
        
        # Examples:
        # ---------
        # package.name = JiveXML
        # package.pathToXMLfile = /some/base/path/JiveXML_TestConfiguration.xml
        # package.fullName = graphics/JiveXML

        self.packageXMLfile       = cvsPackage.packageXMLfile # instance of PackageXMLFile.py
        self.package              = cvsPackage.name
        self.confFile             = cvsPackage.packageXMLfile.path
        self.fullPackageName      = cvsPackage.getFullName()
        self.packageTag           = cvsPackage.getVersion()
        self.local                = cvsPackage.local
        self.cmtPath              = str(cvsPackage.getPackageCmtPath())
        self.cmtDir               = str(self.pathNameBuilder.cmtDir(self.cmtPath))
        self.installArea          = self.pathNameBuilder.getInstallArea(cvsPackage)
        self.shareArea            = self.pathNameBuilder.getShareArea(cvsPackage)
        self.containerPackage     = self.fullPackageName.split(self.cmtPath)[1].strip()[1:]
        self.searchPaths          = searchPaths
        try:
            self.mailTo           = self.packageXMLfile['rtt/mailto[0]/text()']
        except:
            self.mailTo = ''
            

        self.dCubeRefBaseDir      = os.path.join(extractDCubeBaseDir(self.dCubeCfgFile),
                                                 'offline',
                                                 self.containerPackage)

        self.elementCreator       = eCreator
        self.textNodeCreator      = tCreator
    

        # handle the package information
        self.releasePath = self.checkCriticalPath(self.pathNameBuilder.getReleasePath(cvsPackage))
        self.shareArea   = self.checkCriticalPath(self.pathNameBuilder.getShareArea(cvsPackage))
        self.installArea = self.checkCriticalPath(self.pathNameBuilder.getInstallArea(cvsPackage))

        self.workDirs['package'] = os.path.join(self.workDirs['packages'], self.package)
        self.resultsDirs['package'] = os.path.join(self.resultsDirs['packages'], self.package)
    # def dumpHeader(self):
    #    self.logger.info('=============Paths.dump================')
        
    #------------------------------------------------------------------------

    def checkCriticalPath(self, path):
        
        if not path.exists():
            m = 'Critical path: %s does not exist. Aborting run  ' % str(path)
            self.logger.error(m)
            raise RTTInputError(m)

        return str(path)
        
    #------------------------------------------------------------------------

    def checkPath(self, path):
        if not path.exists():
            m = 'Path: %s does not exist. Path creation ' % str(path)
            self.logger.error(m)
            try:
                mkdirIfNew(path)
                m += 'succeeded'
            except:
                
                m += 'failed'

            self.logger.error(m)
        # for now, convert back to the old string format so we dont
        # have to ptopogate RTTpath objects to other modules

        return  str(path)


        
       

