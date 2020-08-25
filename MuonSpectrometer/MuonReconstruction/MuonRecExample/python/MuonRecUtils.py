# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from six import with_metaclass

from GaudiKernel.GaudiHandles import \
     GaudiHandle,GaudiHandleArray,\
     PublicToolHandle,PublicToolHandleArray,\
     PrivateToolHandle,PrivateToolHandleArray,\
     ServiceHandle

from AthenaCommon.JobProperties import JobProperty,jobproperties


import os,sys,copy,re,subprocess

# for backwards compat of clients. TO BE REMOVED !!!
from AthenaCommon.ConfiguredFactory import getProperty

# logger to use for normal output
logMuon = logging.getLogger("MuonRec")
# logger to use for resilience output
logMuonResil = logging.getLogger("Resilience.MuonRec")

from AthenaCommon.ConfigurableMeta import ConfigurableMeta
from AthenaCommon.Configurable import Configurable,ConfigurableAlgTool,ConfigurableService

class ExtraFlags(object):
    ## Set value of flag or add new flag (if not yet present). Return set value
    ## If first argument is a JobProperty, both the name and the value will be taken from it, and no second argument should be given.
    ## If the second argument is a JobProperty, it's value will be taken (with get_Value()).
    def setFlag(self,name,value=None):
        if isinstance(name,JobProperty):
            if value is not None:
                raise ValueError("ExtraFlags.setValue(): No second argument should be given if first argument is a JobProperty args=(%s,%r)" % (name._context_name,value) )
            return setattr(self, name.__name__, name.get_Value())

        elif isinstance(value,JobProperty):
            value = value.get_Value()

        return setattr(self,name,value)
            
    
    ## Set value of flag if not yet present. Return set value.
    ## If first argument is a JobProperty, both the name and the value will be taken from it, and no second argument should be given.
    ## If the second argument is a JobProperty, it's value will be taken (with get_Value()).
    def setFlagDefault(self,name,value=None):
        jp = name
        if isinstance(jp,JobProperty):
            if value is not None:
                raise ValueError("ExtraFlags.setDefault(): No second argument should be given if first argument is a JobProperty args=(%s,%r)" % (jp._context_name,value) )
            name  = jp.__name__
            try:
                return getattr(self,name)
            except AttributeError:
                return setattr(self,name,jp.get_Value())
                
        elif isinstance(value,JobProperty):
            value = value.get_Value()

        return setattr(self,name,value)

    




# Add easy setting of user defaults to Configurables
# NB ideally should be added to class Configurable
class ConfiguredBaseMeta(ConfigurableMeta):
    ## Technical note: It is important that the user defaults are stored
    # separately from the C++ defaults. This distinction is used in the
    # ConfigurableFactory: the C++ default tools/services can optionally
    # be instantiated (steered by configureAllDependencies arg to ConfigurableFactory __init__)
    def __new__( self, name, bases, dct ):
        # prepare for custom defaults

        newclass = type.__new__( self, name, bases, dct )
        if issubclass(newclass,Configurable):
            newclass = ConfigurableMeta.__new__( self, name, bases, dct )

        # add default class members
        d = '_userDefaults'
##        print("%r: adding new class member %s" % (newclass,d))
        setattr(newclass,d,{})
        # fill in the missing defaults from bases
        for b in bases:
            try:
                newd = getattr(newclass,d)
                for n,v in getattr(b,d).items():
                    if n not in newd:
##                        print("%r.%s: adding default %s=%r from %r" % (newclass,d,n,v,b))
                        newd[n] = v
            except AttributeError:
                pass

        return newclass



## @brief Add-in helper base class for configured configurable classes.
#
# Derived classes can set user default propoerties in a convenient way.
# Defaults can be set with setDefaultProperty() and setDefaultProperties().
# In the __init__ of the derived class, one should call applyDefaultProperties() *before*
# calling the __init__ of the base class
#
# Arguments to the constructor are almost the same as for any Configurable.
# @arg \c name the instance name
# @arg \c args the dictionary container of keyword arguments (rather than the named arguments one by one),
# which is being passed to the derived class __init__
#
# Example use:<br>
# <code>
# class DerivedConfigurable(SomeAutoGenConfigurable,ConfiguredBase):
#   def __init__(self,name,**kwargs):
#      self.applyUserDefaults(kwargs,name)
#      super(DerivedConfigurable,self).__init__(name,**kwargs)
#
# DerivedConfigurable.setDefaultProperties( prop1Name = prop1Value,
#                                           prop2Name = "prop2String",
#                                           toolHandle = "ConfigurableInstanceName" ) # in conjuction with ConfigurableFactory
# </code>
#
# NB This functionality should ideally be added directly to class Configurable
#
class ConfiguredBase (with_metaclass (ConfiguredBaseMeta, object)):
    __slots__ = ()


    def __init__(self,name,args):
        self.applyUserDefaults(args,name)


    ## Set user default (not C++ default). Includes type checking.
    @classmethod
    def setDefaultProperty(cls,propertyName,value):
        #  choose according to expected type
        try:
            prop = cls._properties[propertyName]
        except KeyError:
            raise AttributeError("%s does not have property %s" % (cls.__name__,propertyName))

        errorAllowedTypes = None
        cxxdefault = prop.default
        if type(cxxdefault) == type(value):
            cls._userDefaults[propertyName] = copy.copy(value)
        elif isinstance(cxxdefault,PublicToolHandle):
            if value is None:
                cls._userDefaults[propertyName] = PublicToolHandle("")
            elif type(value) == str:
                cls._userDefaults[propertyName] = PublicToolHandle(value)
            elif isinstance(value,ConfigurableAlgTool):
                cls._userDefaults[propertyName] = PublicToolHandle(value.getFullName())
            elif isinstance(value,PublicToolHandle):
                cls._userDefaults[propertyName] = copy.copy(value)
            else:
                errorAllowedTypes = "str,PublicToolHandle,ConfigurableAlgTool"
                
        elif isinstance(cxxdefault,PublicToolHandleArray):
            if value is None:
                cls._userDefaults[propertyName] = PublicToolHandleArray()
            elif type(value) == list:
                cls._userDefaults[propertyName] = PublicToolHandleArray(value)
            elif isinstance(value,PublicToolHandleArray):
                cls._userDefaults[propertyName] = copy.copy(value)
            else:
                errorAllowedTypes = "list,PublicToolHandleArray"
            
        elif isinstance(cxxdefault,PrivateToolHandle):
            if value is None:
                cls._userDefaults[propertyName] = PrivateToolHandle("")
            elif type(value) == str:
                cls._userDefaults[propertyName] = PrivateToolHandle(value)
            elif isinstance(value,ConfigurableAlgTool):
                cls._userDefaults[propertyName] = copy.deepcopy(value)
            elif isinstance(value,PrivateToolHandle):
                cls._userDefaults[propertyName] = copy.copy(value)
            else:
                errorAllowedTypes = "str,PrivateToolHandle,ConfigurableAlgTool"
            
        elif isinstance(cxxdefault,PrivateToolHandleArray):
            if value is None:
                cls._userDefaults[propertyName] = PrivateToolHandleArray()
            elif type(value) == list:
                cls._userDefaults[propertyName] = PrivateToolHandleArray(value)
            elif isinstance(value,PrivateToolHandleArray):
                cls._userDefaults[propertyName] = copy.copy(value)
            else:
                errorAllowedTypes = "list,PrivateToolHandleArray"
            
        elif isinstance(cxxdefault,ServiceHandle):
            if value is None:
                cls._userDefaults[propertyName] = ServiceHandle("")
            elif type(value) == str:
                cls._userDefaults[propertyName] = ServiceHandle(value)
            elif isinstance(value,ConfigurableService):
                cls._userDefaults[propertyName] = ServiceHandle(value.getFullName())
            elif isinstance(value,ServiceHandle):
                cls._userDefaults[propertyName] = copy.copy(value)
            else:
                errorAllowedTypes = "str,ServiceHandle,ConfigurableService"

        else:
            cls._userDefaults[propertyName] = value

        if errorAllowedTypes:
            raise TypeError( "Setting default property %s.%s with wrong type. Expected %s got %s" %
                             (cls.__name__,propertyName,errorAllowedTypes,type(value).__name__) )


    @classmethod
    def setDefaultProperties(cls,**kwargs):
        for name,value in kwargs.items():
            cls.setDefaultProperty(name,value)
        # for debug
        # cls.printDefaultProperties()


    @classmethod
    def getUserDefaults(cls):
        return copy.copy(cls._userDefaults)


    ## return user default for given propertyName. Returns None is no user default is set
    @classmethod
    def getUserDefault(cls,propertyName):
        return cls._userDefaults[propertyName]


    @classmethod
    def applyUserDefaults(cls,args,name=None):
        logger = None
        if name is not None: logger = logging.getLogger(name)
        if logger: logger.verbose("Applying user defaults to arguments:")
        for prop,default in cls._userDefaults.items():
            if prop not in args:
                args[prop] = default
                if logger: logger.verbose("%s = %r", prop, default)
            else:
                if logger: logger.verbose("not setting default %s = %r because value already set to %r", prop, default, args[prop])

                
    @classmethod
    def printUserDefaults(cls):
        print("User default properties of class %s" % cls.__name__)
        for n,v in cls._userDefaults.items():
            print("  %s = %r" % (n,v))


        


# end of class ConfiguredBase


#
# Some extra functionality for JobProperties
#

class AutoLoadContainer(object):
    def __init__(self,moduleName=None,containerName=None,parentContainer=jobproperties):
        self._parentContainer = parentContainer
        if moduleName and containerName:
            self._moduleName = moduleName
            self._containerName = containerName

    def setContainer(self,moduleName,containerName):
        self._moduleName = moduleName
        self._containerName = containerName

    def setParent(self,parent):
        self._parentContainer = parent

    def load(self,moduleName=None,containerName=None):
        """Load the JobProperty container in container module.
        If arguments not given (or None), then use stored members"""
        if not moduleName:
            moduleName = self._moduleName
        if not containerName:
            containerName = self._containerName
        # import the module. The module should itself
        # add its propertycontainer to toplevel jobproperties
        if moduleName not in sys.modules:
            logMuon.info("Auto-loading container %s from module %s", containerName, moduleName)
            # import the (top) module
            mod = __import__(moduleName)
            # get lowest-level module
            for m in moduleName.split('.')[1:]:
                mod = getattr(mod,m)
            # add container
            contClass = getattr(mod,containerName)
            if not hasattr(self._parentContainer,contClass.__name__):
                self._parentContainer.add_Container(contClass)
                logMuon.info("Adding container %s to %s",containerName,self._parentContainer.__name__)
            else:
                logMuon.debug("Container %s already in %s",containerName,self._parentContainer.__name__)

    def unload(self,moduleName=None,containerName=None):
        """Unload the JobProperty container, and the container module.
        If arguments not given (or None), then use stored members"""
        if not moduleName:
            moduleName = self._moduleName
        if not containerName:
            containerName = self._containerName
        try:
            mod = sys.modules[moduleName]
        except KeyError:
            # not there, nothing to be done
            pass
        else:
            # remove container
            contClass = getattr(mod,containerName)
            contInst = getattr(self._parentContainer,contClass.__name__,None)
            if contInst is not None:
                logMuon.info("Removing container %s from %s",containerName,self._parentContainer.__name__)
                self._parentContainer.del_Container(contClass)

# end of class AutoLoadContainer


class AutoLoadContainerJobProperty(JobProperty):
    """JobPropertyBaseClass to auto-load a JobPropertyContainer.
    The derived class must have a member:
    Loader = AutoLoadContainer(<args>)
    The default allowedTypes is bool."""

    allowedTypes=['bool']

    #   Loader = AutoLoadContainer()  # needed in derived class

    def _do_action(self):
        global logMuon
        try:
            self.Loader.load()
        except (ImportError,AttributeError):
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            if athenaCommonFlags.AllowIgnoreConfigError():
                logMuonResil.error("Error importing %s. Switching off %s", self.Loader._moduleName,self.__name__)
                self.set_Off()
            else: # do not allow config error
                raise

    def _undo_action(self):
        self.Loader.unload()


def fillJobPropertyContainer(container,modname):
    import sys
    mod = sys.modules[modname]
    propertiesList = []
    for n,p in vars(mod).items():
        if isinstance(p,type) and issubclass(p,JobProperty) and p.__module__ == modname and \
               'JobProperty' not in n:
            container.add_JobProperty(p)
            propertiesList.append(getattr(container,p.__name__))
    return propertiesList



## @brief JobProperty base class to make a summary boolean out of a number of booleans
#
# The list of JobProperties to control must be given in @c _properties (a list of refs).
# If this property is set to False than set_Off() is called on all controlled properties.
# If this property is set to True than set_On() is called on all controlled properties,
# which means they will the get value they have been assigned (can be True or False).
# The value of this property returns the logical OR of all the controlled values.
class SummaryJobProperty(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True
    _properties = [] # list of bool properties to control. To be set in derived class


    def __init__(self,context=''):
        super(SummaryJobProperty,self).__init__(context)
        self.__class__._log = logging.getLogger(self._context_name)
        

    def _update_action(self):
        if self.statusOn and self.StoredValue:
            for p in self._properties:
                if p is not self:
                    self._log.info("%s.set_On()", p.__name__)
                    p.set_On()
        else:
            for p in self._properties:
                if p is not self:
                    self._log.info("%s.set_Off()", p.__name__)
                    p.set_Off()


    def _do_action(self):
        self._update_action()

    def _undo_action(self):
        self._update_action()

    ## Return logical OR of controlled properties
    def get_Value(self):
        for p in self._properties:
            if p.get_Value(): return True
        return False



## Set the default value of JobProperty @c prop to @c value 
def setJobPropertyDefault(prop,value):
    global logMuon
    if value != prop.__class__.StoredValue:
        mess = "Changing default %s = %r" % ( prop.__name__, value )
        if prop.__class__.StoredValue is not prop.StoredValue:
            mess += " (current value: %r)" % prop.get_Value()
        logMuon.info(mess)
        prop.__class__.StoredValue = value


# Return full name of JobProperty, including parent containers,
# but excluding the top container "JobProperties"
def JobPropertyFullName(prop):
    fullname = prop._context_name
    # remove redundant prefix
    pre = 'JobProperties.'
    if fullname.startswith(pre): fullname = fullname[len(pre):]
    return fullname

def hasJobPropertyBeenSet(prop):
    return prop.StoredValue is not prop.__class__.StoredValue


class _ContestResult:
    __slots__ = ( 'winner', 'loser' )
    def __init__(self,winner,loser):
        self.winner = winner
        self.loser = loser
        

#
# Compare 2 competing JobProperties and decide who's value wins.
# Strategy:
# 1. locked property wins
# 2. set property wins
# 3. In case of draw, @c prop1 wins
def _whoWins( prop1, prop2, logger = logMuon ):
    # 1. locked property wins
    if prop1.is_locked() and not prop2.is_locked():
        winner = prop1
        loser = prop2
        reason = 'it is locked'
    elif not prop1.is_locked() and prop2.is_locked():
        winner = prop2
        loser = prop1
        reason = 'it is locked'
    # 2. set property wins
    elif hasJobPropertyBeenSet(prop1) and not hasJobPropertyBeenSet(prop2):
        winner = prop1
        loser = prop2
        reason = 'it has the value set by user'
    elif not hasJobPropertyBeenSet(prop1) and hasJobPropertyBeenSet(prop2):
        winner = prop2
        loser = prop1
        reason = 'it has the value set by user'
    # Draw: chose prop1
    else:
        winner = prop1
        loser  = prop2
        reason = 'it has precedence (both not locked and not set)'
    logger.debug('%s=%r vs %s=%r : %s=%r wins because %s',
                 JobPropertyFullName(prop1), prop1.get_Value(),
                 JobPropertyFullName(prop2), prop2.get_Value(),
                 JobPropertyFullName(winner),winner.get_Value(),
                 reason)
    # return the result
    return _ContestResult(winner,loser)

## synchronise @c toProp to @c fromProp (both JobProperties)
def syncFlags( toProp, fromProp, logger = logMuon, ignoreLock = False ):
    toValue = toProp.get_Value()
    fromValue = fromProp.get_Value()
    if toValue == fromValue: return # nothing to do
    if toProp.is_locked():
        if not ignoreLock:
            logger.warning('%s is locked. Not synchronising with %s', JobPropertyFullName(toProp), JobPropertyFullName(fromProp))
        else:
            logger.warning('Overriding lock in setting %s = %r (synchronising with %s)', JobPropertyFullName(toProp), fromValue, JobPropertyFullName(fromProp))
            override_lock_and_set_Value(toProp, fromValue)
    else:
        logger.info('Setting %s = %r (synchronising with %s)', JobPropertyFullName(toProp), fromValue, JobPropertyFullName(fromProp))
        toProp.set_Value( fromValue )

## synchronise flags, where the strongest wins.
# Strategy:
# 1. locked property wins
# 2. set property wins
# 3. In case of draw, @c prop1 wins
def syncWinningFlag( prop1, prop2, logger = logMuon, ignoreLock = False ):
    if prop1.get_Value() != prop2.get_Value():
        contest = _whoWins( prop1, prop2, logger )
        _syncFlags( contest.loser, contest.winner, logger, ignoreLock )




def override_lock_and_set_Value(prop,value):
    """Set value of JobProperty, ignoring lock"""
    oldLock = prop._locked
    prop._locked = False
    prop.set_Value(value)
    prop._locked = oldLock

def override_lock_and_set_On(prop):
    """call set_On() on JobProperty, ignoring lock"""
    oldLock = prop._locked
    prop._locked = False
    prop.set_On()
    prop._locked = oldLock

def override_lock_and_set_Off(prop):
    """call set_Off() on JobProperty, ignoring lock"""
    oldLock = prop._locked
    prop._locked = False
    prop.set_Off()
    prop._locked = oldLock


def dumpDetFlags(filename='config.txt'):
    """Append DetFlags printout to config file"""
    # add DetFlags
    from AthenaCommon.DetFlags import DetFlags
    import sys
    confFile = open(filename,'a')
    # redirect stdout to file
    sys.stdout, confFile = confFile, sys.stdout 
    print("Detector flags:")
    DetFlags.Print()
    # put back stdout to screen
    sys.stdout, confFile = confFile, sys.stdout 
    confFile.close()



class RecConfigInfo(object):
    """Small class to contain some info on the expanded reco tag"""
    def __init__(self,configTag,defaultPackage='MuonRecExample'):
        self.configTag   = configTag
        configParts = configTag.split('.')
        # fill in the missing pieces
        nParts = len(configParts)
        # class name is always the last part
        self.className = configParts[-1]
        defaultModule = self.className
        if nParts == 1:
            # default package, default module
            self.packageName = defaultPackage
            self.moduleName  = defaultModule
        elif nParts == 2:
            # default package, explicit module
            self.packageName = defaultPackage
            self.moduleName  = configParts[-2]
        else:
            # explicit package, explicit module
            self.packageName = configParts[0]
            self.moduleName  = '.'.join(configParts[1:-1])
        # full python module name
        self.fullModuleName = self.packageName + '.' + self.moduleName
        # make full configTag
        self.fullTag     = self.fullModuleName + '.' + self.className
        # make short configName
        shortParts = [ self.className ]
        if self.moduleName != defaultModule:
            shortParts.insert(0,self.moduleName)
        if self.packageName != defaultPackage:
            shortParts.insert(0,self.packageName)
        self.shortTag = '.'.join(shortParts)
        
# end of class RecConfigInfo



# ugly hack in case we run outside of athena (e.g. in diffConfig.py)
def uglyHackedInclude(jobOptions):
    global ToolSvc,ServiceMgr
    from AthenaCommon.Include import include
    import __main__
    if not hasattr(__main__,'include'):
        include.setShowIncludes(False)
        include.setClean(False)
        setattr(__main__,'include',include)
    if not hasattr(__main__,"ToolSvc"):
        setattr(__main__,"ToolSvc",ToolSvc)
    if not hasattr(__main__,"ServiceMgr"):
        setattr(__main__,"ServiceMgr",ServiceMgr)

    include(jobOptions)
