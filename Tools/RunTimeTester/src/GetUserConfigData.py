# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class that takes in an top level RTT top configuration file,
and returns a list of tuples of form (configurationStyle, dict)
where config style is a string with values
['unifiedConfiguration | rttConfiguration]
and is a dictionary with key = package name, value = path to conf file
"""
#----- RTT modules -----

from Factory_UserStuffRetriever  import UserStuffRetrieverFactory
from formatCollection            import formatCollection
from CVSPackage                  import CVSPackage
from RTTpath                     import RTTpath
# -------------------------------------------------------------------------
import os.path, xml.dom.minidom, copy
# -------------------------------------------------------------------------

class GetUserConfigData:
    def __init__(self, argBag):

        self.usr                  = argBag.pop('usr')
        self.logger               = argBag.pop('logger')
        self.pnb                  = argBag.pop('pnb')

        self.installedConfFiles   = argBag.pop('installedConfFiles', None)
        self.localConfFiles       = argBag.pop('localConfFiles', None)
        self.rttProvidesConfFiles = argBag.pop('rttProvidesConfFiles', None)
        self.confFileSuffix       = argBag.pop('confFileSuffix', None)
        self.confFiles            = argBag.pop('confFiles', None)
        self.packagesAccept       = argBag.pop('packagesAccept', None)
        self.packagesVeto         = argBag.pop('packagesVeto', None)
        self.cmtHomeDir           = argBag.pop('cmtHomeDir', None)
        self.cmtProjDir           = argBag.pop('cmtProjDir', None)
 
        
        self.installedPackages    = []
        self.localPackages        = []
        self.rttProvidesPackages  = []

        self.packages        = []
        
        self.releaseConfig()
        self.rttProvidesConfig()
        self.packageDomination()

    def __str__(self):
        str  = 'GetUserConfigData - packages:\n'
        str += formatCollection([p.name for p in self.packages])
        return str 
        
    def userConfigData(self):
        return self.packages

    def packageInfo(self, packageName):

        # style = UnifiedConfiguration
        for p in self.packages:
            if p.name == packageName:
                m = 'Found package %s' % (packageName)
                self.logger.debug(m)
                return p

        m = 'GetUserDConfigData has no info about package %s ' % packageName
        self.logger.warning(m)
        return CVSPackage(packageName, '', '', '', '', '', '', '', '',
                          'No GetUserConfigData data for this package')
    
    def releaseConfig(self):
        # --------------------------------------------------
        # Get the user requested jobs:
        # handle packages with unified configuration files
        # --------------------------------------------------

        if not (self.installedConfFiles or self.localConfFiles ): return
    
        macroSubString    = self.confFileSuffix
        dicts             = {macroSubString:[]}
    
        # instlledPackages will contain a list of all CVSPackage objects
        # (installed, local, rttProvided)    allPackages       = [] 

        # -------------------------------------------------
        # ----- RELEASE PACKAGES --------------------------
        # -------------------------------------------------

        # get a list of CVSPackage objects
        self.installedPackages.extend(self.usr.userStuff())

    # -------------------------------------------------
    # ----- RTT PROVIDED PACKAGES ---------------------
    # -------------------------------------------------
    
    def rttProvidesConfig(self):        
        if not self.rttProvidesConfFiles: return

        topLevelPackageDict = self.confFiles


        # set the path given to the xml file
        # packagename is a cvs offset eg 'Simulation/Atlfast/TestAtlfast'
        # {packagename: {'confFilePath': xxxx,
        #                'cmtDir':      yyyy}
        #
        #    .
        #    .
        #    .}


        for packageOffset  in topLevelPackageDict.keys():
            innerDict        = topLevelPackageDict[packageOffset]
            pathToConfigFile = innerDict['confFilePath']
            #
            # cmtPath, version, root are package terms used by CMT
            # examples of values are documented in UserStuffRetriever
            #
            # the cmt mechanisms for determinging
            # packageCmtPath, packageCmtDir, packageCmtDir, packageRoot
            # are not currently working. The variable packageSharePath
            # is the only one currently used in CVSPackage.
            # kludge this to point to the user provided directory containing
            # the _TestConfiguration file
            #
            packageCmtPath   = innerDict['cmtPath']
            packageCmtDir    = str(self.pnb.cmtDir(packageCmtPath))
            packageVersion   = innerDict['version']
            packageRoot      = str(RTTpath(packageCmtPath, packageOffset))
            packageSharePath = innerDict['shareArea']
            # RTT provides files are not 'local'  as no CMT actions
            # have been done by hand
            local         = False 
            errMsg = ''
            if not os.path.exists(pathToConfigFile):
                errMsg = '"RTT provides" declares a non-existant configuration'
                errMsg += 'file %s' % (pathToConfigFile)                
                self.logger.warning(errMsg)


            self.rttProvidesPackages.append(
                CVSPackage(
                os.path.basename(packageOffset),
                pathToConfigFile,
                packageOffset,
                packageVersion,
                packageCmtPath,
                packageRoot,
                packageSharePath,
                # self.cmtHomeDir,
                # self.cmtProjDir,
                errMsg,
                local)
                )
                       
    def packageDomination(self):
        # add all RTT provided "packages" to the list
        packages = copy.copy(self.rttProvidesPackages)
        packagesNames = [package.name for package in packages]
        packages.extend([p for p in self.localPackages if p.name not in packagesNames])

        # add installled packages to the list
        packagesNames = [package.name for package in packages]
        packages.extend([p for p in self.installedPackages if p.name not in packagesNames])

        # keep configuration files only if they decalre RTT jobs   
        noRTT = [p.name for p in packages if not p.wantsRTT]
        msg = 'The following packages do not have RTT jobs %s' % str(noRTT)
        self.logger.debug(msg)
        packages = [p for p in packages if p.wantsRTT]
        
        # now handle user accept and veto requests
        # Do not remove any packages - just flag if jobs are not to be run
        # RunTimeTester is needed for reasons other than running jobs.
        
        accept    = self.packagesAccept
        if accept: # do nothing if the user does not ask for package selection
            [p.setNoRun() for p in packages if p.name not in accept]

        veto      = self.packagesVeto
        [p.setNoRun() for p in packages if p.name in veto]

        self.packages = packages


        
if __name__ == "__main__":
    pass

