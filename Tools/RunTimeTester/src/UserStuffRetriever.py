# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CVSPackage             import CVSPackage
from ShellCommand           import ShellCommand
from formatCollection       import formatCollection
from RTTpath                import RTTpath
from pathToProjectCMTDir    import pathToProjectCMTDir
from pathToProjectCMTDir    import allProjectCMTDirs
# import CMTtools

import os, sys, glob, string


# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
smallLogger = logging.getLogger('rtt.standalone')
# -------------------------------------------------------------------------

from MethodTracer        import MetaTracer

# __metaclass__ = MetaTracer
    
"""
UserStuffRetriever. Finds the configuration files for packages which define
them in a CMT macro.

A substring of the macro name is used to find macros with that name.
By convention, the macro name must start with <package>_

If the paths object passed in the constructor has no package name set, the
entire release is queried via AtlasRelease for the macros.

If the package _is_ given in the paths object, UserRetriever only asks that package.
This assumes that the user has set some CMT stuff outside the RTT.

"""

class UserStuffRetrieverBase:
    def __init__(self, argBag):
        logger.debug('%s.__init__ starts' % self.__class__.__name__)
        
        # self.cmtHomeDir      = argBag.cmtHomeDir
        # self.cmtProjDir      = argBag.cmtProjDir
        self.hardCoded       = argBag.hardCoded
        self.topProject      = argBag.topProject
        self.substring       = argBag.confFileSuffix
        self.projectCMTDirs  = argBag.projectCMTDirs
        self.cmds            = argBag.cmds
        self.init            = False
        self.packageStuff    = {}
        self.packages        = {}

    # def homeRequirements(self):       return self.cmtHomeDir
    # def projectRequirements(self):    return self.cmtProjDir
        
    def readMacros(self, project):

        macros = self.macrosFromCMT(project)
        # macros = self.macrosFromFile()

        # print 'macros'
        # print macros
        

        # the macros that interest us have one '=' sign
        allMacros = {}
        print 'no of macros', len(macros)
        # print macros
        for m in macros:

            # if m.find('RunTimeTester')>-1: print m

            split = m.split('=')
            if len(split) != 2: continue

            # if split[0].find('RunTimeTester')>-1: print split
            
            allMacros[split[0].strip()] = split[1].strip()

        # print
        # print 'self.allMacros'
        # print formatCollection(self.allMacros)
        # print 'self.allMacros end'
        # print

        
        return allMacros
    
    def packageInfo(self, package):

        self.userStuff()

        return self.packages.get(package,
                                CVSPackage(package, '', '', '', '', '', '',
                                           '', '',
                                           'Insufficient info from  CMT'))
    
    def userStuff(self):
        " sets up a dictionary packageName: CVSPackage returns list packages"

        if not self.init:
            self.makePackages()
            self.init = True

        return self.packages.values()

    def getPackageInfo(self, allMacros):

        terms = ['_project_release', '_offset', '_cmtPath', '_project',
                 '_TestConfiguration']

        # build a dictionary with
        # packageName : dict
        # dict is a dictionary of term: macroValue
        for macroName in allMacros.keys():
            if macroName.find(self.substring)>=0:
                packageName = macroName[:macroName.rfind(self.substring)]
                self.packageStuff[packageName] = {}

        # save macros which seem to be related to the package.
        for macroName in allMacros.keys():
            for packageName in self.packageStuff.keys():
                if macroName.startswith(packageName):
                    key = macroName[len(packageName):]
                    val = allMacros[macroName][1:-1]
                    self.packageStuff[packageName][key]=val


        # handle the case where the package part of the macro is uppercase
        terms = ['VERSION']
        for macroName in allMacros.keys():
            for t in terms:
                if macroName.find(t)>0:
                    packageName = macroName[:macroName.rfind(t)]
                    for p in self.packageStuff.keys():
                        # add to dictionary only if the packagename is already
                        # present
                        if p.upper() == packageName:
                            #strip the surrounding quotes
                            val = allMacros[macroName][1:-1]
                            self.packageStuff[p][t] = val
                                                               

        # make dictionary of CVSpackages
        m  = 'About to examine packages for sufficient information from CMT.\n'
        m += 'Currently have %d packages\n' % len(self.packageStuff.keys())
        logger.debug(m)
        packages = {}
        for package in self.packageStuff.keys():
            dict = self.packageStuff[package]
            print 'packageStuff dict',dict
            #needs = ['_cmtpath', '_TestConfiguration', '_offset',
            #         'VERSION', '_root']
            needs = ['_cmtpath', '_TestConfiguration',
                     'VERSION', '_root']
            notPresent = [n for n in needs if n not in dict.keys()]
            if notPresent:
                m  = 'Could not find package information '
                m += '%s for package %s: ' % (str(notPresent), package)
                m += 'not handling this package'
                m += 'package dictionary:\n%s\nend' % formatCollection(dict)
                
                logger.warning(m)
                print m
                continue

            configFilePath = RTTpath(
                dict['_root'],
                'cmt',
                dict['_TestConfiguration']
                )

            pathToConfigFile = str(configFilePath)
            errMsg = '' # will be passed onto the CVSPackage
            if not configFilePath.exists():
                errMsg = 'Non-existent config file %s' % pathToConfigFile
                logger.error(errMsg)
             

            # packageOffset     = dict['_offset']
            packageOffset     = 'notUsed'
            packageVersion    = dict['VERSION']
            packageCmtPath    = dict['_cmtpath']
            packageRoot       = dict['_root']

            packageSharePath = self.makePackageSharePath(dict) # allow subclasses to alter for eg local running 
            # packageSharePath  = RTTpath(packageCmtPath,
            #                            self.hardCoded.installAreaDirName(),
            #                            self.hardCoded.shareAreaDirName()
            #                            )
            if not packageSharePath.exists():
                msg = 'Non-existant share path %s\n ' % str(packageSharePath)
                msg += 'for package %s' % package
                logger.error(msg)
                continue
                
            print '**************************************'
            print 'package name............', package
            print 'packageOffset ',   packageOffset
            print 'packageCmtPath ',  packageCmtPath
            print 'pathToConfigFile', pathToConfigFile
            
            packages[package] = CVSPackage(package,
                                           pathToConfigFile,
                                           packageOffset,
                                           packageVersion,
                                           packageCmtPath,
                                           packageRoot,
                                           str(packageSharePath),
                                           # self.cmtHomeDir,
                                           # self.cmtProjDir,
                                           errMsg)
            
        plist = packages.keys()
        plist.sort()
        m += '%s found %d packages\n%s\n' % (self.__class__.__name__,
                                             len(plist),
                                             str(plist))

        
        logger.info(m)

        return packages

    def makePackageSharePath(self, dict):
        return RTTpath(dict['_cmtpath'],
                       self.hardCoded.installAreaDirName(),
                       self.hardCoded.shareAreaDirName())

    def macrosFromCMT(self, project):
        raise NotImplementedError
    
    def makePackages(self):
        raise NotImplementedError

#===================================================================

class UserStuffRetrieverLocal(UserStuffRetrieverBase):
    def __init__(self, paths):
        UserStuffRetrieverBase.__init__(self, paths)

    def macrosFromCMT(self, project):
        self.cmds.extend(['cmt broadcast cmt -quiet show macros'])
        
        logger.debug('CMT query cmd: %s' % str(self.cmds))
        print 'CMT query cmd: %s' % str(self.cmds)
        sc = ShellCommand(self.cmds, logger)
        macros = sc.getReply()
        
        return macros

    def makePackages(self):
        macros = self.readMacros('all')
        self.packages = self.getPackageInfo(macros) # list of CVSPackage objects

    def makePackageSharePath(self, dict):
        return RTTpath(dict['_root'], self.hardCoded.shareAreaDirName())
#===================================================================        

class UserStuffRetrieverNonLocal(UserStuffRetrieverBase):
    def __init__(self, paths):
        UserStuffRetrieverBase.__init__(self, paths)

    def cmdsToSourceProjects(self, project):
        if project == 'all':
            cmtdirs = allProjectCMTDirs(self.projectCMTDirs)
        else:
            cmtdirs = [pathToProjectCMTDir(project, self.projectCMTDirs)]
            
        
        cwd = os.getcwd()

        cmd = []
        for dir in cmtdirs:
            cmd.append('cd %s' % dir)
            cmd.append('cmt -quiet show macros')
        cmd.append('cd %s' % cwd)

        logger.info('Command fragment to source all projects:\n%s' % str(cmd))
        return cmd
        
    def macrosFromCMT(self, project):
        self.cmds.extend(self.cmdsToSourceProjects(project))

        logger.debug('macrosFromCMT - issuing command %s', str(self.cmds))
        sc = ShellCommand(self.cmds, logger)
        return sc.getReply()

    def makePackages(self):
        macros = self.readMacros('all')
        self.packages = self.getPackageInfo(macros) # list of CVSPackage objects

        # overwrite the package if there is a patch package in the top package
        macros = self.readMacros(self.topProject)
        self.packages.update(self.getPackageInfo(macros))
        
#===================================================================




if __name__ == '__main__':
    pass
