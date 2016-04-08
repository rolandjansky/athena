# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Evaluate            import Evaluate, getText
from xml.dom.minidom     import parse
from validateXMLFile     import Validator
from Tools               import mkdirIfNew
from Tools2              import fixSortBranches
from formatCollection    import formatCollection
from Factory_Legals      import LegalsFactory
from Factory_HardCoded   import HardCodedFactory
from Archiver            import ArchiverFactory, ArchivePathSelectorFactory, ArchivingCondition
from Tools2              import isValidReleaseNumber
from Tools               import machineNameToIPaddress
from RTTSException       import RTTInputError
from Factory_IOUtils     import IOUtilsFactory

import os.path

import logging 
logger = logging.getLogger('rtt')

# -------------------------------------------------------------------------

def handleConfFiles(topElement, config):
    """ make a

dictionary confiles:
key: package val:dict

dictionary dict:
key: conFilePath val: absolute path for package test configuration file
key: cmtDir      val: absolute path to the package cmt directory 
"""
    confFiles  = {}
    packageElements = Evaluate('packageConfFile', topElement)
    for element in packageElements:
        package = Evaluate('package/text()', element)
        
        dict = {}
        dict['confFilePath'] = Evaluate('pathToFile/text()', element)
        dict['cmtPath']      = Evaluate('cmtpath/text()', element)
        dict['version']      = Evaluate('version/text()', element)
        dict['installArea']  = Evaluate('installArea/text()', element)
        dict['shareArea']    = Evaluate('shareArea/text()', element)
        confFiles[package] = dict
    config['confFiles'] = confFiles

#----------------------------------------------------------------------

def handleChainStoreBasePath(topElement, config):
    area = Evaluate('chainStoreBasePath', topElement)
    if area:
        config['chainStoreBasePath'] = Evaluate('text()', area[0])

#----------------------------------------------------------------------

def handleConfFileSrc(topElement, config):

    # place a dictionary {leaf package: absolute path to conf file}
    # of rtt provided configuration files in the conf dictioanry 
    packagesFromRTT = Evaluate('confFileSrc/rttProvides', topElement)
    if packagesFromRTT:
        config['rttProvidesConfFiles'] = True
        handleConfFiles(packagesFromRTT[0], config)

    localPackageTags = Evaluate('confFileSrc/localConfFiles', topElement)

    # flag whether the config file declares local packages (ie checked out
    # packahes, and make a list of packages so declared> Both flag and list
    # are placed in the conf dictionary.
    config['localConfFiles']     = False
    if localPackageTags:
        config['localConfFiles'] = True

        config['userHomeCmtDir'] = Evaluate(
            'confFileSrc/localConfFiles/userHomeCmtDir/text()', topElement)
        config['testProjectCmtDir'] = Evaluate(
            'confFileSrc/localConfFiles/testProjectCmtDir/text()', topElement)

    # flag whether the release packages should be accessed.
    installed = Evaluate('confFileSrc/installedConfFiles', topElement)
    if installed: config['installedConfFiles'] = True
            
#----------------------------------------------------------------------

def handleSize(sizeEls):
    def check(sizeEls):
        if not sizeEls: return ''
        size = sizeEls[0]
        try:
            size = float(Evaluate('text()', size))
        except:
            m = '%s: illegal value for a size in bytes.' % size
            raise RTTInputError(m)
        else:
            if not size > 0.0:
                m = 'Must provide a file size to match > 0.0 bytes! (you gave %s)' % str(size)
                raise RTTInputError(m)
            else:
                return size
            
    if not sizeEls: return {}
    size_el = sizeEls[0]
    size_gt = check(Evaluate('gt', size_el))
    size_lt = check(Evaluate('lt', size_el))
    if (size_gt and size_lt) and (size_gt <= size_lt):
        m = 'Cannot simultaneously match files larger than X bytes _and_ less than Y bytes where X > Y.'
        raise RTTInputError(m)        
            
    dict = {}
    if size_lt and not size_gt:
        dict = {'lt':size_lt}
    elif size_gt and not size_lt:
        dict = {'gt':size_gt}
    else:
        dict = {'lt':size_lt, 'gt':size_gt}
    return dict

def handleName(nameEls):
    def check(nameEls):
        if not nameEls: return []
        return [Evaluate('text()', nameEl) for nameEl in nameEls]
        
    if not nameEls: return {}
    name_el = nameEls[0]
    dict = {}
    dict['is_']    = check(Evaluate('is', name_el))
    dict['isnot_'] = check(Evaluate('isnot', name_el))    
    return dict

def handleParentDir(parentdirEls):
    def check(nameEls):
        if not nameEls: return []
        return [Evaluate('text()', nameEl) for nameEl in nameEls]
    
    if not parentdirEls: return {}
    parentdirEl = parentdirEls[0]
    dict = {}
    dict['is_']    = check(Evaluate('is', parentdirEl))
    dict['isnot_'] = check(Evaluate('isnot', parentdirEl))    
    return dict    
    
def handleArchive(topElement, config):
    # Example:
    #    <archive>
    #       <type>castor</type>
    #       <base>/castor/cern.ch/user/j/joebloggs/archive_area</base>
    #       <nvers>7</nvers>
    #       <condition>
    #          <size><gt>100000</gt></size>
    #          <name><is>*.root</is><isnot>root.*</isnot></name>   
    #       </condition>
    #    </archive>
    # 
    from createDirectoryDictionary import findSeg
    archives    = Evaluate('archive', topElement)
    archiveList = []
    
    for archive in archives:
        numb  = Evaluate('nvers/text()', archive)
        type_ = Evaluate('type/text()', archive)
        dest  = Evaluate('base/text()', archive)
        try:
            numb = int(numb)
        except:
            msg = 'Please give an integer for the number of required archives. You gave: <nvers>%s</nvers>' % numb
            raise RTTInputError(msg)
        if numb < 1:
            msg = 'Require an archive to have >=1 versions (<nvers>), you gave %d' % numb
            raise RTTInputError(msg)

        utils = IOUtilsFactory(logger).create(type_)
        if not utils.exists(dest): # complicated statement because we may need to check on Castor
            msg = 'Archive <dest> %s: inexistant' % dest
            raise RTTInputError(msg)

        conditions = Evaluate('condition', archive)
        condsList = []
        for c in conditions:
            cList = []

            sizeDict = handleSize(Evaluate('size', c))
            nameDict = handleName(Evaluate('name', c))
            parentdirDict = handleParentDir(Evaluate('parentdir', c))
            
            if sizeDict.keys(): cList.append(ArchivingCondition.Size(sizeDict))
            if nameDict.keys(): cList.append(ArchivingCondition.Name(nameDict))
            if parentdirDict.keys(): cList.append(ArchivingCondition.ParentDir(parentdirDict))
                                
            # Now finally append the list of conditions to the master list
            condsList.append(ArchivingCondition(cList))
    
        selector = ArchivePathSelectorFactory(logger).create(type_, dest, numb)
        
        prefix = config['release']
        if prefix.startswith('rel_'): prefix='nightly'
        run_frag = os.path.join(prefix,                     
                                config['branch'],
                                'build',
                                config['cmtConfig'], 
                                findSeg(config['topProject']))

        # Select which archive we should use
        # Archives have path like <base_archive_path>/<branch>/<archiveIndex>/<run_frag>/
        # where, for example:
        #     branch=15.2.X.Y,
        #     archiveIndex=N (where 0<=N<size of archive (7 days for ex.))
        #     run_frag=nightly/dev/build/i686-slc4-gcc34-opt/offline
        # RTT when it chooses an archive, dumps an empty file called time_stamp below
        # <run_frag> dir. Next run, by comparing time_stamps, the RTT can select the oldest archive to re-use.
        archivepath = selector.select_archive(run_frag)
        archiveList.append(ArchiverFactory(logger).create(type_, archivepath.split(run_frag)[0].strip(),
                                                          run_frag, condsList))    
    config['archivers'] =  archiveList

#----------------------------------------------------------------------

def handleBigFiles(topElement, config):
    bigFilesElement = Evaluate('bigFiles', topElement)
    config['bigFilesArea'] = ''
    config['bigFilesSize'] = 0
    
    if not bigFilesElement: return
    config['bigFilesArea'] = Evaluate('bigFilesArea/text()', bigFilesElement[0])
    config['bigFilesSize'] = int(Evaluate('bigFilesSize/text()', bigFilesElement[0])) # size in MB

#----------------------------------------------------------------------

def handleAcceptPackage(topElement, config):
    acceptElements = Evaluate('packageAccept', topElement)
    acceptPackages = []
    [acceptPackages.append(getText(element)) for element in acceptElements] 
    config['packagesAccept'] = acceptPackages

#----------------------------------------------------------------------

def handleVetoPackage(topElement, config):
    vetoElements = Evaluate('packageVeto', topElement)
    vetoPackages = []
    [vetoPackages.append(getText(element)) for element in vetoElements] 
    config['packagesVeto'] = vetoPackages

#----------------------------------------------------------------------

def handleLibToolsLoc(topElement,config):
    libTools = Evaluate('libToolsLoc', topElement)
    if libTools: config['libToolsLoc'] = getText(libTools[0])

#----------------------------------------------------------------------

def handleMaxTime(topElement, config):
    
    hours=mins=secs=0

    texthours   = Evaluate('maxTime/hours',   topElement)
    if texthours:
        texthours   = getText(texthours[0])
        try:
            hours = int(texthours)
        except:
            logger.error('error in hours spec '+texthours)


    if hours < 0:
        msg  = 'time spec out of range (hours) = '+ str(hours)
        logger.error(msg)
        hours = 0
        
    textmins   = Evaluate('maxTime/minutes',   topElement)
    if textmins:
        textmins   = getText(textmins[0])
        try:
            mins = int(textmins)
        except:
            logger.error('error in mins spec '+textmins)


    if mins < 0 or mins >59:
        msg  = 'time spec out of range (mins) = '+ str(mins)
        logger.error(msg)
        mins = 0
        
    total = hours*3600+mins*60
    if total > 0: config['maxTime'] = total
    return

#----------------------------------------------------------------------

def handleTopProject(topElement, config):
    config['topProject'] = Evaluate('topProject/text()', topElement)
    
#----------------------------------------------------------------------

def handleOtherProject(topElement, config):
    # DTD guarantees presence
    config['otherProject'] = Evaluate('otherProject/text()', topElement)

#----------------------------------------------------------------------

def handleRunType(topElement, config):
    # DTD guarantees either release or kit
    config['runType'] = 'build'
    elements = Evaluate('runType/kitRun', topElement)
    if elements:
        config['runType'] = 'kit'
        installAreaElements = Evaluate('runType/kitRun/installArea',topElement)
        
#----------------------------------------------------------------------

def handleReleaseType(topElement, config):
    releaseTypeElements = Evaluate('releaseType', topElement)
    if releaseTypeElements: # DTD ensures 0 or 1 such elements
        config['releaseType'] = getText(releaseTypeElements[0])
    else:
        config['releaseType'] = 'project' # default
        
                                        
#----------------------------------------------------------------------

def handlePilotJobFlag(topElement, config):

    config['rttPilotJobs'] = False # default
    config['atnPilotJobs'] = False # default

    # DTD ensures 0 or 1 such elements
    if Evaluate('rttPilotJobs', topElement): config['rttPilotJobs'] = True
    if Evaluate('atnPilotJobs', topElement): config['atnPilotJobs'] = True
                                        
#----------------------------------------------------------------------

def handleConcurrentFlag(topElement, config):
    config['concurrent'] = False # default

    # DTD ensures 0 or 1 such elements
    if Evaluate('concurrent', topElement):
        config['concurrent'] = True
        config['wpcServer']  = machineNameToIPaddress(Evaluate('concurrent/wpcServer/text()',topElement))

#----------------------------------------------------------------------

def handleBooleanFlag(flagname, topElement, config):
    config[flagname] = False
    if Evaluate(flagname, topElement): config[flagname] = True

#----------------------------------------------------------------------

def handleRTTRunFlag(topElement, config):
    handleBooleanFlag('rttRun', topElement, config)

def handleTCTRunFlag(topElement, config):
    handleBooleanFlag('tctRun', topElement, config)

def handleFCTRunFlag(topElement, config):
    handleBooleanFlag('fctRun', topElement, config)

#----------------------------------------------------------------------

def handleDisableMailToFlag(topElement, config):
    handleBooleanFlag('disableMailTo', topElement, config)
                                        
#----------------------------------------------------------------------

def handleDeleteNonKeepFiles(topElement, config):
    handleBooleanFlag('deleteNonKeepFiles', topElement, config)

#----------------------------------------------------------------------

def handleDevRunFlag(topElement, config):
    handleBooleanFlag('devRun', topElement, config)

#----------------------------------------------------------------------

def handleSandboxRunning(topElement, config):
    handleBooleanFlag('sandboxRunning', topElement, config)

#----------------------------------------------------------------------

def ensureInequalityWorkResBase(workBase,resBase):
    msg = ''
    #if workBase == resBase:
    #   msg  = 'You have given the same path in <workBasePath> and '
    #   msg += '<resultsBasePath>. This is forbidden! Exiting...\n'

    return msg

#----------------------------------------------------------------------

def checkPaths(config):
    
    pathsToMake = ['workBasePath',
                   'resultsBasePath'
                   ]

    [mkdirIfNew(config[p]) for p in pathsToMake]

    pathsThatMustExistAndBeAbs = ['workBasePath',
                                  'resultsBasePath',
                                  'jobGroupConfig',
                                  'dataSetCatalog',
                                  'libToolsLoc',
                                  'dCubeCfgFile',
                                  'pathToVetoFile']
    


    badPaths = []
    badPaths.extend([config[xmlTagName]
                     for xmlTagName in pathsThatMustExistAndBeAbs
                     if checkPathsCondition(config,xmlTagName)])
    
    badPaths.extend([aPath for aPath in config.get('confiles', {}).values()
    if checkPathsCondition2(aPath)])    

    msg = ''
    if badPaths: msg = 'Bad paths: %s' % str(badPaths)
    return msg

#----------------------------------------------------------------------

def checkPathsCondition(config,key):    
    conditionMet = key in config
    conditionMet = conditionMet and not isDefault(key,config[key])
    conditionMet = conditionMet and not (os.path.exists(config[key]) and os.path.isabs(config[key]))
    return conditionMet

#----------------------------------------------------------------------

def checkPathsCondition2(aPath):
    conditionMet = not (os.path.exists(aPath) and os.path.isabs(aPath))
    return conditionMet

#----------------------------------------------------------------------

def checkPathsCondition3(config,key):
    bad = False
    if not  key in config: return bad

    isRel         = not os.path.isabs(config[key])
    # doesntExist    = not os.path.exists(config[key])
    # bad = isRel or doesntExist
    bad = isRel
    return bad

#----------------------------------------------------------------------

def loadSimplePathsIntoConfig(documentElement,config):
    simplePaths = [
        'mode', 'release', 'cmtConfig', 'workBasePath', 'resultsBasePath',
        'site', 'dataSetCatalog', 'branch',
        'jobGroupConfig', 'versionDir','distArea','libToolsLoc','confFileSuffix',
        'resultsBaseURL', 'devRun', 'dCubeCfgFile', 'pathToVetoFile']
    
    # DTD guarantees only a single element
    for path in simplePaths:
        elements = Evaluate(path, documentElement)
        if elements:
            element=elements[0]
            config[path]  = getText(element)

#----------------------------------------------------------------------

defaults = {'packagesAccept'       : [],
            'packagesVeto'         : [],
            'maxTime'              : 60*60*20,
            'versionDir'           : None,
            'rttProvidesConfFiles' : False,
            'localConfFiles'       : False,
            'installedConfFiles'   : False,
            'confFileSuffix'       : '_TestConfiguration',
            'libToolsLoc'          : None,
            'dCubeCfgFile'         : '',
            'otherProject'         : '',
            'pathToVetoFile'       : ''
            }

# -------------------------------------------------------------------

def addDefaultsToConfig(config):
    for item in defaults.items():
        key          = item[0]
        defaultValue = item[1]
        config.setdefault(key,defaultValue)

# -------------------------------------------------------------------

def isDefault(key,val):
    itIs = False
    try:
        if defaults[key] == val:
            itIs = True
    except:
        itIs = False
    return itIs

# -------------------------------------------------------------------

def rttConfiguration(fn):

    Validator(fn).validate() # throws exception if invalid
        
    documentElement = parse(fn).documentElement    

    config = {}
    loadSimplePathsIntoConfig(documentElement, config)
    
    handleRunType(documentElement, config)
    handleTopProject(documentElement, config)
    handleOtherProject(documentElement, config)
    handleConfFileSrc(documentElement, config)
    handleAcceptPackage(documentElement, config)
    handleVetoPackage(documentElement, config)
    handleMaxTime(documentElement, config)
    handleLibToolsLoc(documentElement,config)
    handleReleaseType(documentElement, config)
    handlePilotJobFlag(documentElement, config)
    handleConcurrentFlag(documentElement, config)
    handleDisableMailToFlag(documentElement, config)
    handleDevRunFlag(documentElement, config) # if present, sets a smaller sleep time in TestRun
    handleDeleteNonKeepFiles(documentElement, config)
    handleChainStoreBasePath(documentElement, config)
    handleArchive(documentElement, config)
    # handleRTTRunFlag(documentElement, config)
    # handleTCTRunFlag(documentElement, config)
    # handleFCTRunFlag(documentElement, config)

    # fill in blanks where necessry
    addDefaultsToConfig(config)

    logger.debug('Checked legality of top level configuration file:\n %s' % config)    
    return config

class RTTConfiguration:
    def __init__(self, configFile):
        self.config = rttConfiguration(configFile)
        self.legals = LegalsFactory(logger).create(
            self.config['releaseType'],
            self.config['branch'],
            HardCodedFactory(logger).create(self.config['releaseType'],
                                            self.config['runType'])
            
            )
        # make a copy of the branch as self.branch will be overwritten
        # when handling numbered releases
        self.originalBranch    = self.config['branch']
        self.isNightly         = False
        # the user may request that the release is to be discovered
        # make a copy of the release here. This copy will be overwritten
        # if the release is discovered
        self.release         = self.config['release']
        self.findNightly     = False
        self.valid           = True
        self.invalidText     = ''


        self.isConfigFileLegal()
        
        if not self.valid:
            print '\n\n*******************************************'
            print '     ILLEGAL TOP LEVEL CONFIGURATION FILE!'
            print '*******************************************\n\n'
            logger.critical(self.invalidText)
            raise RTTInputError(self.invalidText)

        self.processConfigData()

        
    def isConfigFileLegal(self):
        canTest = self.legals.validKeys()
        willTest = [k for k in canTest if k in self.config.keys()]
            
        valids = [(k,
                   self.config[k],
                   self.legals.isLegal(k, self.config[k])) for k in willTest]

        m = ''
        for v in valids:
            if not v[2]:
                m += 'Illegal value (%s) for variable "%s".\n' % (v[1], v[0])
                m += 'Legal values: \n'
                for l in self.legals.vals(v[0]):
                    m += '\t%s\n' % l
                m += '******************************************************\n\n'
        if m:
            self.valid = False
            self.invalidText += m


        # need a test for webBranchVeto
        
            
        msg = ensureInequalityWorkResBase(self.config['workBasePath'],
                                          self.config['resultsBasePath'])
        if msg:
            self.valid = False
            self.invalidText += msg+'\n'

        msg = checkPaths(self.config)

        if msg:
            self.valid = False
            self.invalidText += msg+'\n'
    
                
    def processConfigData(self):
        release          = self.config['release']
        
        # adjust the config file logical name for nightly
        # releases to the name used as part of a path
        if self.legals.isNightlyRelease(release):
            self.release = self.legals.convertLogicalNightly(release)
            self.isNightly = True
        # set flag to find current nighlty release (user puts the word
        # 'nightly' in the config file
        if release in self.legals.vals('nightlyWord'):
            self.findNightly = True
            self.isNightly   = True

        
        numbered = isValidReleaseNumber(release)

        if self.isNightly == numbered:
            self.valid = False
            msg  = 'Release %s is illegal.\n' % release
            msg += 'Legal nightlies: %s \n' % str(self.legals.vals('nightlies'))
            self.invalidText = msg
        
  
    def discoverRelease(self): return self.findNightly
    def setRelease(self, release): self.release = release
    def get(self, key, defaultReturn=None):
        keys = ['release', 'isNightly', 'originalBranch']
        if key in keys: return self.__dict__[key]
        if self.config.has_key(key): return self.config[key]
        return defaultReturn
    def text(self): return self.invalidText
    def confDict(self): return self.config
    def configIsValid(self): return self.valid
    def __str__(self):
        s = ''
        for k in self.__dict__.keys():
            if k != 'config':
                s += '%s      %s\n' % (k, str(self.__dict__[k]))
        s += '----\nconfig: \n%s\n' %formatCollection(self.config)
        return s
#----------------------------------------------------------------------

if __name__ == '__main__':
    #    import  getopt, sys
    #    from findRTThome import findRTThome
    #
    #    try:
    #        opts = getopt.getopt(sys.argv[1:],'c:')
    #    except getopt.GetoptError:
    #        print 'bad options'
    #        sys.exit(2)
    #
    #    cfg = None
    #    for opt in opts[0]:
    #        print opt
    #        if opt[0] == '-c': cfg = opt[1]
    #
    #
    #    (base, cfg, src) = findRTThome(cfg)
    #
    #    if not os.path.exists(cfg):
    #        print 'could not find cfg file',cfg
    #
    #    logger.info('config file: '+cfg)
    #    
    #    logger.info(formatCollection(rttConfiguration(cfg)))
    #
    b = ['Numbered','N.X.0','10.0.X']
    print b
    b =  fixSortBranches(b)
    print b
