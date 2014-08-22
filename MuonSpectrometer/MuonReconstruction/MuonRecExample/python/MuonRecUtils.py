# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from GaudiKernel.GaudiHandles import \
     GaudiHandle,GaudiHandleArray,\
     PublicToolHandle,PublicToolHandleArray,\
     PrivateToolHandle,PrivateToolHandleArray,\
     ServiceHandle

from AthenaCommon.JobProperties import JobProperty,jobproperties


import os,sys,copy,re,commands

# for backwards compat of clients. TO BE REMOVED !!!
from AthenaCommon.ConfiguredFactory import getProperty

# for backwards compat of clients. TO BE REMOVED !!!
from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber

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
##        print "%r: adding new class member %s" % (newclass,d)
        setattr(newclass,d,{})
        # fill in the missing defaults from bases
        for b in bases:
            try:
                newd = getattr(newclass,d)
                for n,v in getattr(b,d).items():
                    if n not in newd:
##                        print "%r.%s: adding default %s=%r from %r" % (newclass,d,n,v,b)
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
class ConfiguredBase(object):
    __slots__ = ()
    __metaclass__ = ConfiguredBaseMeta


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
        print "User default properties of class %s" % cls.__name__
        for n,v in cls._userDefaults.items():
            print "  %s = %r" % (n,v)


        


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
        except ImportError,AttributeError:
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
    # print out message
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
    print "Detector flags:"
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



#
# Copied from PyJobTransformsCore/python/trfutil.py
# until these tools are available in AtlasCore
class StringNumberList:
    """List of strings only differing by a number they contain, where the list of numbers is coded in a special syntax.
    <prefix>[<list>]<postfix> where '[' and ']' are litteral characters and <list> gives a recipe for a list of
    numbers to be generated (with <n> and <m> integers): 
    <n>,<m> (enumeration) or <n>-<m> (range including m) or any combination of those.
    A python list of strings is generated where the [<list>] is replaced by the actual integers that the list represents.
    The [<list>] part can also be omitted, in which case a list of one string is generated (as given).
    The width of the integers is determined by the integer with the most number of digits in the [<list>], where leading 0's
    are included in the count. The integers in the expanded filenames have leading 0's padded where needed."""

    openBracket = '['
    closeBracket = ']'

    def __init__(self, codedString=None):
        self.set( codedString )

        
    def set(self, codedString):
        self.__codedString = codedString
        self.__numbers = None
        self.__prefix = None
        self.__suffix = None
        self.__numberList = None
        self.__digits = 0
        # return self for easy chaining of commands
        return self


    def getPrefix(self,openBracket=-1):
        """Get everything up to (but not including) the [.
        If [ is not found, return the full string. In case of error, raise exception.
        The <openBracket> argument (optional) is there for optimisation and gives
        the position of the '[' in the coded string (or -1 if unknown)."""
        if self.__prefix is not None: return self.__prefix
        valIn = self.__codedString
        if valIn is None: raise RuntimeError("No input string set")
        if openBracket != -1:
            assert valIn[openBracket] == StringNumberList.openBracket
            bopen = openBracket
        else:
            bopen = valIn.find(StringNumberList.openBracket)
            if bopen == -1: self.__prefix = valIn
        if bopen > 0:
            self.__prefix = valIn[:bopen]
        else:
            self.__prefix = valIn

        return self.__prefix


    def getSuffix(self,closeBracket=-1):
        """Get everything after the ].
        If ] is not found, return emptry string. In case of error, return None"""
        if self.__suffix is not None: return self.__suffix
        valIn = self.__codedString
        if valIn is None: raise RuntimeError("No input string set")
        if closeBracket != -1:
            assert valIn[closeBracket] == StringNumberList.closeBracket
            bclose = closeBracket
        else:
            bclose = valIn.find(StringNumberList.closeBracket)
            if bclose == -1: self.__suffix = ""
        if bclose < len(valIn) - 1:
            self.__suffix = valIn[bclose+1:]
        else:
            self.__suffix = ""
        return self.__suffix
    

    def getNumbers(self,openBracket=-1,closeBracket=-1):
        """Get the part in between [ and ], including the [].
        If [] part is not found, return empty string. In case of error, raise exception"""
        if self.__numbers is not None: return self.__numbers
        valIn = self.__codedString
        if valIn is None: raise RuntimeError("Input string not set")
        if openBracket != -1:
            assert valIn[openBracket] == StringNumberList.openBracket
            bopen = openBracket
        else:
            bopen = valIn.find(StringNumberList.openBracket)
        if closeBracket != -1:
            assert valIn[closeBracket] == StringNumberList.closeBracket
            bclose = closeBracket
        else:
            bclose = valIn.find(StringNumberList.closeBracket,bopen + 1)

        if   bopen == -1 and bclose == -1:
            # no brackets at all, so no numbers given
            self.__numbers = ""
        elif bopen == -1 and bclose != -1:
            # close bracket without open bracket
            raise RuntimeError("Missing %r in %r" % (StringNumberList.openBracket,valIn))
        elif bopen != -1 and bclose == -1:
            # open bracket without close bracket
            raise RuntimeError("Missing %r in %r" % (StringNumberList.closeBracket,valIn))
        elif bclose == bopen + 1:
            self.__numbers = ""
        else:
            self.__numbers = valIn[bopen:bclose+1]

        return self.__numbers

                
    def getNumberList(self,openBracket=-1,closeBracket=-1):
        """Return a tuple of size 2, containing the list of integers in the first field and
        the number of digits in the second field. The list of integers is the result of
        the decoding of the numbers coded in the [] part of the input string. The number
        of digits is the maximum number of digits used in the numbers in the [] part,
        where leading 0's are included in the count.
        If no '[]' part is found, return an tuple with an empty list. In case of error,
        raises a RuntimeError exception"""
        if self.__numberList is not None: return (self.__numberList,self.__digits)
        nums = self.getNumbers(openBracket,closeBracket)
        if nums is "": return (list(),0)
        numList = [ ]
        bclose = len(nums)
        posB = 1
        digits = 0
        while posB <= bclose:
            #always start with an digit
            posE = posB
            while posE < bclose and nums[posE].isdigit(): posE += 1
            # require at leaste one digit
            if posE == posB: raise RuntimeError("Syntax error: require at least one digit per field in %r" % nums)
            # convert to integer
            digits = max(digits,posE - posB)
            iNum = int(nums[posB:posE])
            charE = nums[posE]
            if charE == StringNumberList.closeBracket: # last number
                numList.append( iNum )
                break
            elif charE == ',':  # single number
                numList.append( iNum )
                posB = posE + 1
                continue
            elif charE == '-':  # next comes end of range
                posB = posE + 1
                posE = posB
                while posE < bclose and nums[posE].isdigit(): posE += 1
                # require at leaste one digit
                if posE == posB: raise RuntimeError("Syntax error: require at least one digit per field in %r" % nums)
                # convert to integer    
                digits = max(digits,posE - posB)
                iStop = int(nums[posB:posE])
                if iStop < iNum: raise RuntimeError("Syntax error: %r is not a number" % nums[posB:posE] )
                numList += range(iNum,iStop+1)
                charE = nums[posE]
                if charE == ',':
                    posB = posE + 1
                    continue
                elif charE == StringNumberList.closeBracket:
                    break
                else:
                    raise RuntimeError("Syntax error: expected \',\' or \'%s\' at position %d. Found \'%s\' instead in %r" %\
                                       (StringNumberList.closeBracket,posE,charE,nums) )
            else: # spurious character
                raise RuntimeError("Syntax error: unexptected character %r at position %d in %r" % (charE,posE,nums) )

        self.__numberList = numList
        self.__digits = digits

        return (self.__numberList,self.__digits)


    def convertStringList(self,codedString):
        return self.set(codedString).getStringList()


    def getStringList(self):
        """Convert coded string <valIn> into an expanded list. If <valIn> contains a syntax error an exception is thrown"""
        openBracket = self.__codedString.find(StringNumberList.openBracket)
        closeBracket = self.__codedString.find(StringNumberList.closeBracket,openBracket + 1)
        numList,digits = self.getNumberList(openBracket,closeBracket)
        if len(numList) == 0: return [ self.__codedString ]

        prefix = self.getPrefix(openBracket)
        suffix = self.getSuffix(closeBracket)
        return [ '%s%0*d%s' % (prefix,digits,i,suffix) for i in numList ]
  
    
# Make sure that the castor environment variables STAGE_HOST and STAGE_SVCCLASS
# are set to the required values. If the test fails, a RuntimeError exception is raised.
def assertCastorStager(required_STAGE_HOST, required_STAGE_SVCCLASS):
    import os
    STAGE_HOST = os.environ.get('STAGE_HOST')
    STAGE_SVCCLASS = os.environ.get('STAGE_SVCCLASS')
    err = ""
    if STAGE_HOST is None:
        err += " STAGE_HOST is not set."
    elif STAGE_HOST != required_STAGE_HOST:
        err += " STAGE_HOST=%r is incorrect." % STAGE_HOST

    if STAGE_SVCCLASS is None:
        err += " STAGE_SVCCLASS is not set."
    elif STAGE_SVCCLASS != required_STAGE_SVCCLASS:
        err += " STAGE_SVCCLASS=%r is incorrect." % STAGE_SVCCLASS

    err = err.strip()
    if err:
        raise RuntimeError("Problem with environment variable(s): %s Correct values are (zsh syntax):\n"
                           "export STAGE_HOST=%s\n"
                           "export STAGE_SVCCLASS=%s\n"
                           "Please set these environment variables and run job again" % (err, required_STAGE_HOST, required_STAGE_SVCCLASS))
    print "STAGE_HOST=%s" % STAGE_HOST
    print "STAGE_SVCCLASS=%s" % STAGE_SVCCLASS




#
# Copied from PyJobTransformsCore/python/trfutil.py
# until these tools are available in AtlasCore
class FileList:
    """Comma separeted list of filenames. Each filename in the list can be coded
    with the following syntax:
    <prefix>[<list>]<postfix> where '[' and ']' are litteral characters and <list>
    gives a recipe for a list of numbers to be generated (with <n> and <m> integers): 
    <n>,<m> (enumeration) or <n>-<m> (range including m) or a combination of those.
    A list of filenames is generated where the [<list>] is replaced by the actual
    integers that the list represents.
    The [<list>] part can also be omitted, in which case a list of one filename
    is generated (as given)."""

    listSep = ','
    wildCardRE = re.compile(re.escape('[]*?'))

    @staticmethod
    def expandStringToList(valIn,maxFiles=-1):
        """Convert a string with filenames <valIn> into python list.
        In case of a syntax error in the string, raise exception
        <maxFiles> is the maximum number of files kept in the list. If -1, keep all files (default)"""
        # split the comma separated list (outside brackets)
        sepList = []
        n = len(valIn)
        i = 0
        start = 0
        while i < n:
            c = valIn[i]
            if c == StringNumberList.openBracket:
                i = valIn.find( StringNumberList.closeBracket, i + 1 )
                if i == -1: raise RuntimeError("Missing %r in valIn" % StringNumberList.closeBracket)
            elif c == FileList.listSep:
                if i == n - 1 or i - start < 1: raise RuntimeError( "Unexpected %r at position %d in %s" % (FileList.listSep,i,valIn) )
                sepList.append( valIn[start:i] )
                start = i + 1

            i += 1
        # add the last entry in the list
        if start < n: sepList.append( valIn[start:] )
        all = []
        for val in sepList:
            all += StringNumberList(val).getStringList()

        if maxFiles >= 0 and len(all) > maxFiles:
            all = all[:maxFiles]

        return all

    @staticmethod
    def expandList(listIn,maxFiles=-1):
        """Expand all entries in (python) list. Can also be a string with ',' separated entries.
        <maxFiles> is the maximum number of files kept in the list. If -1, keep all files (default)"""
        all=[]
        if type(listIn) == str:
            all = FileList.expandStringToList( listIn, maxFiles )
        elif type(listIn) in (list,tuple):
            all = []
            for s in listIn:
                if maxFiles >= 0 and len(all) >= maxFiles:
                    break
                all += FileList.expandStringToList(s,maxFiles)
        else:
            raise RuntimeError("Can not expand list of type %r" % type(listIn)) 

        if maxFiles >= 0 and len(all) > maxFiles:
            all = all[:maxFiles]

        return all

    @staticmethod
    def expandAndReadDirs(listIn,maxFiles=-1):
        """Expand all entries in (python) list. Can also be a string with ',' separated entries.
        It will expand wildcards and read files in directories."""
        allIn = FileList.expandList(listIn)
        allFiles = []
        for f in allIn:
            ft = FileList.fileType(f)
            if ft == 'd' or FileList.hasWildCard(f):
                allFiles += FileList.readDirectory(f)
            elif ft == 'f': # regular file
                allFiles.append(f)
            elif ft == 'e': # error
                logMuon.warning("Skipping file/dir (can not read): %r", f)
            else:
                logMuon.warning("Skipping file (unknown type=%r): %r",ft,f)

            if maxFiles >= 0 and len(allFiles) >= maxFiles:
                break

        if maxFiles >= 0 and len(allFiles) > maxFiles:
            allFiles = allFiles[:maxFiles]

        return allFiles


    @staticmethod
    def castorPrefix(filename):
        prefixes = [ "rfio:", "root://castoratlas/" ]
        STAGE_HOST = os.environ.get("STAGE_HOST","castoratlas")
        if STAGE_HOST != "castoratlas":
            prefixes += [ "root://%s/" % STAGE_HOST ]
        for prefix in prefixes:
            if filename.startswith(prefix): return prefix

        return ""


    @staticmethod
    def isFileOnCastor(filename):
        return filename.startswith("/castor/") or FileList.castorPrefix(filename)

    @staticmethod
    def nsls(dir,opts=''):
        prefixToRemove = FileList.castorPrefix(dir)
        dir=dir[len(prefixToRemove):]
        cmd = 'nsls '+opts+' '+dir
##        print cmd
        status,output = commands.getstatusoutput( cmd )
        if status != 0:
            raise IOError(output)
        # turn multiline string into list with one line per entry
        output = output.split(os.linesep)
        # add back prefix to filename (last column)
        prefix=prefixToRemove+dir
        if not prefix.endswith(os.sep): prefix += os.sep
        fullList=[]
        for f in output:
            items = f.split()
            if not items: continue
            items[-1] = prefix+items[-1]
            fullList.append( ' '.join(items) )

        return fullList


    @staticmethod
    def readDirectory(dirIn):
        """Return a list of regular files in directory <dirIn>. If the filename part contains a wildcard, it will be expanded."""
        logMuon.info("Reading list of files from directory %s",dirIn)
        all = []
        if FileList.isFileOnCastor(dirIn):
            all = FileList.nsls(dirIn)
        elif FileList.hasWildCard(dirIn): # regular files
            dirname,basename = os.path.split(dirIn)
            if FileList.hasWildCard(dirname):
                raise RuntimeError("Wildcards in directories not supported (filenames only)")
            from PyUtils import path
            try:
                all = path.path(dirname).files(basename)
            except:
                logMuon.warning("Could not read files %r from directory %s", basename,dirname)
                return []
        else:
            from PyUtils import path
            try:
                all = path.path(dirIn).files()
            except:
                logMuon.warning("Could not read files from directory %s", dirIn)
                return []
                

        # only return regular files (not directories)
        return [ str(f) for f in all if not FileList.isDirectory(f) ]

    @staticmethod
    def fileType(pathIn):
        """Return string indicating type of file:
        'f': regular file
        'd': directory
        'e': error (not readable)
        Works for local files, rfio:, /castor, root:
        """

        if pathIn.startswith("rfio:") or pathIn.startswith("root:") or pathIn.startswith("/castor/"):
            output = FileList.nsls(pathIn, '-ldL')
            statbits = output[0].split()[0]
            typebit = statbits[0]
            if typebit in '-m': # regular file or migrated to tape
                return 'f'
            elif typebit == 'd': # directory
                return 'd'
            elif typebit == 'D': # logically deleted file
                return 'e'
            elif typebit == 'l': # symbolic link (should not happen with -L option to nsls)
                return FileList.fileType(os.path.realpath(pathIn)) # UNTESTED! May fail!
            else:
                logMuon.error("Don't know what to do with nsls filetype %r of file %r", typebit, pathIn)
                return 'e'
        else:
            if not os.path.exists(pathIn):
                return 'e'
            elif os.path.isfile(pathIn):
                return 'f'
            elif os.path.isdir(pathIn):
                return 'd'
            elif os.path.islink(pathIn):
                return FileList.fileType(os.path.realpath(pathIn))

        # if it gets here, something is wrong!
        return 'e'


    @staticmethod
    def isDirectory(pathIn):
        """file system independent way of determining if pathIn is a directory.
        Works for local files, rfio:, /castor"""
        return FileList.fileType(pathIn) == 'd'


    @staticmethod
    def hasWildCard(pathIn):
        return FileList.wildCardRE.search(pathIn) is not None


class DataInfo(object):

    def __init__(self,listOfFiles,type,detdescr='AUTO',condTag='',maxFiles=-1,STAGE_SVCCLASS=None,STAGE_HOST=None,**kwargs):
        self.fileList = listOfFiles
        self.type = type
        self.detDescr = detdescr
        self.condTag = condTag
        self.maxFiles = maxFiles # -1 = all files
        self.STAGE_SVCCLASS = STAGE_SVCCLASS
        self.STAGE_HOST = STAGE_HOST

    def _getFileList(self):
        # activate castor stager if set
        if self.STAGE_SVCCLASS:
            os.environ['STAGE_SVCCLASS'] = self.STAGE_SVCCLASS
            logMuon.info("Setting STAGE_SVCCLASS=%s",os.environ['STAGE_SVCCLASS'])

        if self.STAGE_HOST:
            os.environ['STAGE_HOST'] = self._STAGE_HOST
            logMuon.info("Setting STAGE_HOST=%s",os.environ['STAGE_HOST'])

        if self._expandedFiles is None:
            self._expandedFiles = FileList.expandAndReadDirs(self._rawFiles,self.maxFiles)

        if not self._expandedFiles and self._rawFiles:
            raise RuntimeError("No expanded input files resulted from %s raw input files of %r" % (self.maxFiles,self._rawFiles) )
            
        return self._expandedFiles

    def _setFileList(self,fileList):
        self._rawFiles = fileList
        self._expandedFiles = None

    def _getMaxFiles(self):
        return self._maxFiles

    def _setMaxFiles(self,value):
        if not hasattr(self,'_maxFiles') or self._maxFiles != value:
            self._maxFiles = value
            self._expandedFiles = None
            
    

    ## Expand fileList on demand only, and only once
    fileList = property(_getFileList,_setFileList,None)

    ## Make sure fileList is re-expanded at next demand if maxFiles changes
    maxFiles = property(_getMaxFiles,_setMaxFiles,None)
