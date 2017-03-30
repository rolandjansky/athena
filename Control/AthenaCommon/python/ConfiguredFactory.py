# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__author__ = 'Martin Woudstra <martin.woudstra@cern.ch>'

import string,copy,os,weakref

from GaudiKernel.GaudiHandles import \
     GaudiHandle,GaudiHandleArray,\
     PublicToolHandle,PublicToolHandleArray,\
     PrivateToolHandle,PrivateToolHandleArray,\
     ServiceHandle,ServiceHandleArray

from Configurable import Configurable
import ConfigurableDb
from AppMgr import ToolSvc,ServiceMgr
from Logging import logging


# Classes-to-be-raised-as-errors
# NB probably should go in another file somewhere in AthenaCommon

class ConfigurationError(RuntimeError):
    pass

class ConfigurableInstanceNotFoundError(ConfigurationError):
    pass

class ConfigurableClassNotFoundError(ConfigurationError):
    pass

class ConfigurableTypeError(ConfigurationError):
    pass


#
# Functions for Configurables. These should ideally be member functions of class Configurable
#
def getProperty(configurable,property):
    """Helper function to get a property of a configurable: the set value, user set default or C++ default."""
    try:
        # set value
        return getattr(configurable,property)
    except AttributeError:
        retVal = configurable.getDefaultProperty(property)
        if retVal is None: raise
        return retVal

def getPropertyNoAutoRetrieve(configurable,property):
    """Helper function to get a property of a configurable: the set value, user set default or C++ default."""
    if hasPropertyBeenSet(configurable,property):
        return getattr(configurable,property)
    else:
        retVal = configurable.getDefaultProperty(property)
        if retVal is None: raise AttributeError("%r does not have attribute %s" % (configurable.__class__, property) )
        return retVal


def getPropertyType(configurable,property):
    try:
        return type(configurable._properties[property].default)
    except KeyError:
        raise AttributeError("Configurable %r does not have property %s" % (configurable.__class__.__name__,property) ) 
    

def hasPropertyBeenSet(configurable,property):
    try:
        proxy = configurable._properties[property]
    except KeyError:
        raise AttributeError("Configurable %r does not have property %s" % (configurable.__class__.__name__,property) ) 

    return configurable in proxy.history


#
# Handy function to get type and name from different kinds of objects: string,Configurable,GaudiHandle
#
def getTypeAndName(objectWithTypeAndName):
    """Return tuple (\"<type>\",\"<name>\") extracted from <objectWithTypeAndName> string or Configurable or GaudiHandle.
    <objectWithTypeAndName> format if string: \"<type>/<name>\". Short format: \"<name>\" assumes name == type and returns (\"<name>\",\"<name>\" """
    if type(objectWithTypeAndName) == str:
        parts = objectWithTypeAndName.split('/')
        typePart = parts[0]
        if len(parts) >= 2:
            namePart = parts[1]
        else:
            namePart = typePart
    else:
        typePart = objectWithTypeAndName.getType()
        namePart = objectWithTypeAndName.getName()

    return (typePart,namePart)
        

#
# Handy function to get the full name from different kinds of objects: string,Configurable,GaudiHandle
#
def getFullName(objectWithTypeAndName):
    if type(objectWithTypeAndName) == str:
        return objectWithTypeAndName
    else:
        return objectWithTypeAndName.getFullName()


# helper function: add element to list only if it is not yet present. Should probably go elsewhere
def addToListIfNotIn(theList,elementsToAdd):
    if not isinstance( elementsToAdd, (list,tuple) ): # also covers GaudiHandleArrays (since they derive from list)
        elementsToAdd = [ elementsToAdd ]
    for e in elementsToAdd:
        if e not in theList: theList.append(e)


## @brief A helper class that contains all neccessary information to create a configurable instance.
#
# Arguments to the constructor:
# @arg \c configuredMaker is a string of the form \"package.module.maker\", where the "maker" is either
# a class derived from auto-generated configurable, a factory function (creating a configurable instance),
# or the name of a C++ class. A configurable instance is also supported temporarily
# @arg \c configurableArgs are the (optional) named arguments (name=value) which will be passed on to the configuredMaker
# function or class __init__
class ConfiguredPlaceHolder(object):
    def __init__(self,configuredMaker,**configurableArgs):
        self.configurableArgs = configurableArgs
        self._originalMaker = configuredMaker
        self._setMaker( configuredMaker )


    def _setMaker(self,configuredMaker):
        # set optional member based on type of configurableFactory
        if type(configuredMaker) == str:
            self._configuredMakerName = configuredMaker
        elif isinstance(configuredMaker,Configurable):
            self._configurableInstance  = weakref.ref(configuredMaker)
            self._configurableClass     = configuredMaker.__class__
        elif type(configuredMaker).__name__ == "function":
            self._configurableFunction = configuredMaker
        elif issubclass(configuredMaker,Configurable):
            self._configurableClass     = configuredMaker
        else:
            raise ConfigurationError("%r is not a Configurable class, a string (class name), a factory function or a configurable instance",
                                     configuredMaker)
        

    def _getObject(self,fullName):
        parts = fullName.split('.')
        objName = parts[-1]
        if len(parts) == 1:
            # assume it's a name of a Configurable class
            conf = ConfigurableDb.getConfigurable(objName)
            if conf is None:
                raise ConfigurableClassNotFoundError("Configurable for class %s not found" % (objName) )
            return conf

        else:
            # assume it's the name of an object in a module
            mods = parts[:-1]
            modName = '.'.join(mods)
            # load the top-level module
            mod = __import__(modName,globals(),locals(),[objName])
            return getattr(mod,objName)
            

    def _getConfigurableMaker(self):
        # first try class
        try:
            return self._configurableClass
        except AttributeError:
            pass

        # then try factory function
        try:
            return self._configurableFunction
        except AttributeError:
            pass

        # create one of the two
        try:
            name = self._configuredMakerName
        except AttributeError:
            pass
        else:
            self._setMaker( self._getObject(name) )
            # now try again with updated maker
            return self._getConfigurableMaker()
            
        raise ConfigurationError("Could not determine configuredMaker for %r" % (self._originalMaker) )


    # read-only access
    configuredMaker = property( _getConfigurableMaker )


    def getMakerType(self):
        if type(self._originalMaker) == str:
            return "%r" % (self._originalMaker)  # for printout
        elif isinstance(self._originalMaker,Configurable):
            return "<instance \'%s.%s\'>" % (self._originalMaker.__module__,self._originalMaker.__class__.__name__)  # for printout
        elif type(self._originalMaker).__name__ == "function":
            return "<function \'%s.%s\'>" % (self._originalMaker.__module__,self._originalMaker.__name__)  # for printout
        elif issubclass(self._originalMaker,Configurable):
            return "<class \'%s.%s\'>" % (self._originalMaker.__module__,self._originalMaker.__name__)  # for printout

        return "%r" % self._originalMaker


    def shortDescription(self,name):
        typ = None
        indent = "  "
        theInstance = self.getInstance()
        if theInstance is not None:
            typ = theInstance.getType()
            indent = "* "
        else:
            try:
                typ = self._configurableClass.getType()
            except AttributeError:
                try:
                    makerName = self._configuredMakerName
                    if '.' not in makerName: typ = makerName
                except AttributeError:
                    pass

        mess = "%s%r  maker=%s" % (indent, name, self.getMakerType())
        if typ is not None: mess += "  type=%s" % (typ,)

        return mess


    def createInstance(self,name,argsOverride=None):
        theMaker = self._getConfigurableMaker()

        args = dict(self.configurableArgs)
        if argsOverride: args.update(argsOverride)
        # make an instance of the class
        conf = theMaker(name,**args)

        # store for later quick access
        self._configurableInstance = weakref.ref(conf)

        return conf
        

    def hasInstance(self):
        return self.getInstance() is not None


    def getInstance(self):
        try:
            return self._configurableInstance() # weakref: may return None is object was deleted
        except AttributeError:
            return None


    ## @brief make a clone of this PlaceHolder, with a new name and possibly updated arguments
    def clone(self,**kwargs):
        args = dict(self.configurableArgs) # make a copy
        args.update(kwargs)
        return ConfiguredPlaceHolder(self._originalMaker, **args)
        

# end of class ConfiguredPlaceHolder

#
# Helper class to keep track of the full hierarchy of failed properties
#
class PropertyStack(object):
    def __init__(self):
        self._names = [] # list of strings to get unique full property hierarchy name
        self._props = [] # list of strings

    def push(self,parent,propName):
        if not self._names:
            # top one: start with instance name
            self._names.append( parent.getJobOptName() )
        # all: use property name
        self._names.append( propName )

        fullPropName = "%s(%r).%s" % (parent.getType(), parent.getJobOptName(), propName)
        self._props.append(fullPropName)
##        print "Adding to property stack %i: %s" % (self.depth(),fullPropName)


    def pop(self):
        self._names.pop()
        fullPropName = self._props.pop()
##        print "Removing from property stack %i: %s" % (self.depth()+1,fullPropName)

    def depth(self):
        return len(self._props)

    def fullHierarchyName(self):
        return '.'.join(self._names)

    def toString(self,indent=""):
        header =  "%s%s property stack details (%i deep, top first):" % (indent,self.fullHierarchyName(),len(self._props))
        lines = []
        # print in reverse order (deepest first)
        for p in self._props:
            indent+=" "
            lines.append( indent+p )
        sep = " ="+os.linesep
        return header + os.linesep + sep.join(lines)


## @brief track some created instances and keep track of the properties they were assigned to
class ConfigurableInstanceTracker(object):
    def __init__(self,fullName):
        self._confName = fullName
        self._properties = {} # list of all PropertyStacks where this same instance was assigned to

    def addProperty(self,propStack):
        fullName = propStack.fullHierarchyName()
        if fullName not in self._properties:
            self._properties[fullName] = copy.deepcopy(propStack)

    def getType(self):
        return getTypeAndName(self._confName)[0]

    def __str__(self):
        lines = []
        lines.append( "%r as value of the following %i properties:" % \
                      (self._confName, len(self._properties)) )
        for p in self._properties.values():
            lines.append( p.toString("    ") )

        return os.linesep.join(lines)



## @brief A gathering place where fully configured configurables can be retrieved by name.
#
# This is an intermediate solution between the ToolSvc of the old days that accepted anything without
# any checks, including tools that have not been explicitly added, and the current ToolSvc which only
# allows access to the tools that have been explicitly added.
# The ConfiguredFactory keeps a list of recipes to create tools or services by instance name.
# The tools/services are only created and added to ToolSvc and/or ServiceMgr (where applicable) when
# someone is requesting the tool via one of the get*() functions or when properties of a requested
# tool are resolved (i.e. recursively configured).
# This helps to keep the ToolSvc/ServiceMgr clean by having only the tools/services that are needed.
#
# One can declare a 'recipe' for tools and services by adding:
# - the instance maker, the instance name and its properties to set
# The maker is given in the form of a string ("module.maker") which points to a Configurable derived class,
# a Configurable factory function or a Configurable instance.
# Clients can then request a fully configured tool (public or private, or even a service) by its name,
# just as with ToolSvc, although the interface is different: factory.getPublicTool("toolName").
#
# The configurable is not instantiated immediately when added, but the information is stored in the dedicated
# class ConfiguredPlaceHolder, which will instantiate the configurable on demand.
# The main benefit is for the clients of the tools. When a client (algorithm, service, tool) needs a tool
# (or service), it can just ask the factory to create the configured tool (by name), without having to know
# the details of how to configure it, because the configuration has been setup by the expert of the tool.
# The client does not need to know the name of the configurable class, and there is a single place where
# to find it. The client only needs to know the instance name (which should be documented!).
#
# When a tool is being created, all its ToolHandle and ServiceHandle properties are being resolved, meaning
# that the tools/services they refer to are being configured by the ConfiguredFactory. What happens if
# the tool/service correspondig to the Handle properties is not found in the ConfiguredFactory depends
# on the settings of the Factory.
# Example:<br>
# <code>
# confFactory.addTool( "SlaveToolPackage.SlaveToolModule.SlaveToolMaker, "SlaveTool", beObedient=True )<br>
# confFactory.addTool( "MasterToolPackage.MasterToolModule.MasterToolMaker", "TheMaster", doTheWorkTool = "SlaveTool" )<br>
# </code>
# where doTheWorkTool is a ToolHandle property of class MasterTool.
#
# Other features:
# - clone a configurable: make a copy of a configured configurable with a different name.
#   This is useful if one wants to make a small change w.r.t. an already configured tool.
#   There are functions line addToolClone() and getPublicToolClone()
#
class ConfiguredFactory(object):

    ## @arg @c typesToExcludeDefaults : list of strings with name of configurable type that should not be resolved
    def __init__(self,
                 configureSetDependencies=True,
                 configureDefaultDependencies=True,
                 configureDependenciesAlsoIfDefaultInstance=False,
                 requireAllDependencies=False,
                 typesToExcludeIfDefaultValue=[],
                 namesToExcludeIfDefaultValue=[],
                 fullNamesToExcludeIfDefaultValue=[],
                 propertiesToExcludeIfDefaultValue=[],
                 typesToSkipIfNotAvailable=[],
                 namesToSkipIfNotAvailable=[],
                 fullNamesToSkipIfNotAvailable=[],
                 tryDefaultTool=False,
                 tryDefaultService=False,
                 tryDefaultAlgorithm=False ):
        
        self._availableConfigurables = {} # dict with ConfiguredPlaceHolders
        self._configSetDependencies      = configureSetDependencies
        self._configDefaultDependencies  = configureDefaultDependencies
        self._configDependenciesAlsoIfDefaultInstance = configureDependenciesAlsoIfDefaultInstance
        self._requireAllDependencies = requireAllDependencies
        self._tryDefaultTool = tryDefaultTool
        self._tryDefaultService = tryDefaultService
        self._tryDefaultAlgorithm = tryDefaultAlgorithm
        self._typesToExcludeDefaults        = copy.copy(typesToExcludeIfDefaultValue)
        self._namesToExcludeDefaults        = copy.copy(namesToExcludeIfDefaultValue)
        self._fullNamesToExcludeDefaults    = copy.copy(fullNamesToExcludeIfDefaultValue)
        self._propertiesToExcludeDefaults   = copy.copy(propertiesToExcludeIfDefaultValue)
        self._typesToSkipIfNotAvailable     = copy.copy(typesToSkipIfNotAvailable)
        self._namesToSkipIfNotAvailable     = copy.copy(namesToSkipIfNotAvailable)
        self._fullNamesToSkipIfNotAvailable = copy.copy(fullNamesToSkipIfNotAvailable)
        self._typesOnlyToSkip = []
        
        self._hasReadDB = False

        # some members for printout
        # list of default configurable instances that have been created
        self._createdDefaultInstances = {}
        self._defaultTypeNotFound= {}
        # list of configurable types/names that have been excluded from recursive config
        self._excludedDependencies = []
        # list of configurable members whose instantiation failure was ignored
        # One done for C++ defaults, and only if requireAllDependencies=False (otherwise exception is thrown)
        self._ignoredDependencyErrors = []
        # list of properties where the default has not been explicitly set
        self._excludedDefaultProperties = {}
        # list of default instances that we not created because they had only the type
        self._excludedOnlyTypes = []

        
    def logger(self):
        return logging.getLogger(self.__class__.__name__)

    def setLogLevel(self,level):
        self.logger().setLevel(level)

    ## Add types to exclude if the value if the C++ default
    # Argument <excludeList> is list of names (strings) of configurable types (C++ class names)
    # that should not be resolved. A single type name (a string) is also supported.
    def addTypesToExcludeIfDefaultValue(self,excludeList):
        addToListIfNotIn(self._typesToExcludeDefaults, excludeList)

    ## Add instance names to exclude if the value if the C++ default
    # Argument <excludeList> is list of names (strings) of configurable instances
    # that should not be resolved. A single instance name (a string) is also supported.
    def addNamesToExcludeIfDefaultValue(self,excludeList):
        addToListIfNotIn(self._namesToExcludeDefaults, excludeList)
        
    # Add to list of full names ("type/name") to exclude if the value if the C++ default
    # This is mainly to exclude defaults which point to an interface type: fullname = "ISomeInterface" (should be fixed in C++ !!!)
    def addFullNamesToExcludeIfDefaultValue(self,excludeList):
        addToListIfNotIn(self._fullNamesToExcludeDefaults, excludeList)

    # Add to names of properties not to resolve if their value is the C++ default
    def addPropertiesToExcludeIfDefault(self,excludeProps):
        addToListIfNotIn(self._propertiesToExcludeDefaults, excludeProps)

    # Add types to ignore if they are not found in the database
    def addTypesToSkipIfNotAvailable(self,skipList):
        addToListIfNotIn(self._typesToSkipIfNotAvailable,skipList)
                
    # Add names to ignore if they are not found in the database
    def addNamesToSkipIfNotAvailable(self,skipList):
        addToListIfNotIn(self._namesToSkipIfNotAvailable,skipList)
                
    # Add full names to ignore if they are not found in the database
    def addFullNamesToSkipIfNotAvailable(self,skipList):
        addToListIfNotIn(self._fullNamesToSkipIfNotAvailable,skipList)

    def addTypesOnlyToSkip(self,typeList):
        addToListIfNotIn(self._typesOnlyToSkip,typeList)
                
    def _isExcludedDefaultValue(self,propValue):
        propType,propName = getTypeAndName(propValue)
        fullName = getFullName(propValue)
        if propType in self._typesToExcludeDefaults or \
               propName in self._namesToExcludeDefaults or \
               fullName in self._fullNamesToExcludeDefaults:
            # keep list for printout at the end
            excluded = "%s(%r)" % (propType,propName)
            addToListIfNotIn(self._excludedDependencies, excluded)
                
            return True

        return False


    def _isExcludedDefaultProperty(self,propName):
        if propName in self._propertiesToExcludeDefaults:
            n = self._excludedDefaultProperties.setdefault(propName,0)
            n += 1
            self._excludedDefaultProperties[propName] = n
            return True

        return False


    def _skipIfNotAvailable(self,propValue):
        propType,propName = getTypeAndName(propValue)
        fullName = getFullName(propValue)
        if propType in self._typesToSkipIfNotAvailable or \
               propName in self._namesToSkipIfNotAvailable or \
               fullName in self._fullNamesToSkipIfNotAvailable:
            # keep list for printout at the end
            excluded = "%s(%r)" % (propType,propName)
            addToListIfNotIn(self._excludedDependencies, excluded)
                
            return True

        return False


    # this is really to counteract a design mistake of (C++) GaudiHandles:
    # the default type/name is the tool/service/alg type of the Handle
    def _excludeIfOnlyType(self,objectWithTypeAndName):
        if type(objectWithTypeAndName) == str:
            parts = objectWithTypeAndName.split('/')
            if len(parts) >= 2: return False
            # now it has only 1 part, either type or name
            if '::' in objectWithTypeAndName or objectWithTypeAndName in self._typesOnlyToSkip:
                addToListIfNotIn(self._excludedOnlyTypes, objectWithTypeAndName)
                return True

            # in case of doubt
            return False

        else:
            return self._excludeIfOnlyType(objectWithTypeAndName.getFullName())


    ## @arg @c parent : the parent (@c ConfiguredPlaceHolder) of the property
    def _resolveConfigurable(self,parent,propertyName,propertyValue,getterFunc,checkType,indent,propStack):
        if propertyValue is None: return None
        parentType=parent.getType()
        parentName=parent.getName()
        isPropDefault = not hasPropertyBeenSet(parent,propertyName)

        propertyType = getPropertyType(parent,propertyName)
        isPropertyArray = issubclass(propertyType,GaudiHandleArray)
        isValueArray = type(propertyValue) in (list,tuple) or isinstance(propertyValue,GaudiHandleArray)

        resolvingString = indent+"%s "
        # for debug printout
        if self.logger().isEnabledFor(logging.VERBOSE):
            if ( isPropDefault ):
                resolvingString += "default "
            else:
                resolvingString += "set "
            resolvingString += "%s(%r).%s = %r" % (parentType,parentName,propertyName,propertyValue)

        if propStack is not None: propStack.push(parent,propertyName)

        try:
            if isValueArray:
                self.logger().verbose(resolvingString, "Resolving")
                newPropertyValue = [] # propertyType()
                for v in propertyValue:
                    conf = self._resolveConfigurable(parent,propertyName,v,getterFunc,checkType,indent,propStack)
                    if conf:
                        newPropertyValue.append( conf )
                    else: # keep existing one
                        newPropertyValue.append( v )

            else: # not an array
                if isPropDefault and self._isExcludedDefaultValue(propertyValue):
                    self.logger().verbose(resolvingString, "Keeping existing")
                    return propertyValue
                if not isPropertyArray: self.logger().verbose(resolvingString, "Resolving")

                if isinstance(propertyValue,Configurable):
                    # do not change already assigned configurables
                    self.logger().verbose(resolvingString, "Keeping existing")
                    return propertyValue
                elif isinstance(propertyValue,GaudiHandle) or type(propertyValue) == str:
                    doCheck=checkType
                    # tryDefaultConfigurable None instead of False to always retrieve it for the sake of dependencies
                    tryDefault=self._configDependenciesAlsoIfDefaultInstance or None
                else:
                    raise TypeError("%r is not a Configurable, Handle, HandleArray, string or list" % propertyValue)

                nextIndent=indent+" "
                newPropertyValue = getterFunc(propertyValue, tryDefaultConfigurable=tryDefault, checkType=doCheck, allowNone=True,
                                              indent=nextIndent,propStack=propStack)

                # for printout at the end
                if propStack is not None:
                    valueType,valueName = getTypeAndName(propertyValue)
                    defaultInstance = self._createdDefaultInstances.get(valueName)
                    if defaultInstance:
                        defaultInstance.addProperty(propStack)
                    typeNotFound = self._defaultTypeNotFound.get(valueName)
                    if typeNotFound:
                        typeNotFound.addProperty(propStack)


            # for debug printout
            if self.logger().isEnabledFor(logging.VERBOSE) and \
                   newPropertyValue is not None and (not isPropertyArray or isValueArray):
                resolvedString  = indent+"Resolved  "
                if ( isPropDefault ):
                    resolvedString  += "default "
                else:
                    resolvedString  += "set "
                # add array name by hand to see the resolving (PublicToolHandleArray will only show tool names)
                if isPropertyArray and isValueArray:
                    newValueRepr = "%s(%r)" % (propertyType.__name__, newPropertyValue)
                else:
                    newValueRepr = "%r" % newPropertyValue
                resolvedString  += "%s(%r).%s = %s" % (parentType,parentName,propertyName,newValueRepr)
                self.logger().verbose(resolvedString)

            if propStack is not None: propStack.pop()

            # return the new value (or old one)
            if newPropertyValue is None:
                # Not available, just keep the old one
                return propertyValue
            else:
                return newPropertyValue

        except ConfigurableInstanceNotFoundError:
            if propStack is not None: propStack.pop()
            if self._requireAllDependencies:
                raise

            # Not available, just keep the old one
            return propertyValue

        except ConfigurationError,err:
            if propStack is not None: propStack.pop()
            if self._requireAllDependencies or not isPropDefault:
                raise

            ignored = "%s(%r).%s = %r\n    Error message: %s" % (parentType,parentName,propertyName,propertyValue,err)
            if ignored not in self._ignoredDependencyErrors:
                self._ignoredDependencyErrors.append(ignored)
            self.logger().debug( "Ignoring C++ default config error: %s", ignored)
            # Do not include ignored values in return list
            return None
        

        # I should never get here
        raise RuntimeError("Logical Bug in Code. This line should not have been executed")





    ## @arg @c parent : the parent (@c ConfiguredPlaceHolder) of the property
    def _resolveProperty(self,parent,propertyName,propertyValue,checkType=False,indent="",propStack=None):
        """For Tool/ServiceHandles: turn name of tool/service into configurable instances.
        For any other, return None so it does not get re-assigned"""
        if propertyValue is None: return None # valid also for Handles. Nothing special to do.
        cls = parent.__class__
        try:
            cxxdefault = cls._properties[propertyName].default
        except KeyError:
            raise AttributeError("%s does not have property %s" % (cls.getType(),propertyName))

        if isinstance(cxxdefault,(PublicToolHandle,PublicToolHandleArray)):
            confGetter = self.getPublicTool
        elif isinstance(cxxdefault,(PrivateToolHandle,PrivateToolHandleArray)):
            confGetter = self.getPrivateTool
        elif isinstance(cxxdefault,(ServiceHandle,ServiceHandleArray)):
            confGetter = self.getService
        else: # any other type of property left untouched
            return propertyValue

        return self._resolveConfigurable(parent,propertyName,propertyValue,confGetter,checkType,indent,propStack)


    ## @arg @c parent : the parent (@c ConfiguredPlaceHolder) of the property
    def _resolveProperties(self,parent,props,indent="",propStack=None):
        newProps = {}
        for name,value in props.items():
            if value != Configurable.propertyNoValue:
                newProps[name] = self._resolveProperty( parent, name, value, indent, propStack )

        return newProps


    def _resolveAllProperties(self,conf,indent="",propStack=None):

        confName = conf.getName()
        defaultProps = conf.getDefaultProperties()
        for name,value in defaultProps.items():
            # skip non-configurables, since they don't need to be resolved
            propertyType = getPropertyType(conf,name)
            if not issubclass(propertyType,(GaudiHandle,GaudiHandleArray,Configurable)):
                continue

            isPropSet = hasPropertyBeenSet(conf,name)
            if isPropSet:
                if not self._configSetDependencies: continue
            else:
                if not self._configDefaultDependencies: continue
                if self._isExcludedDefaultProperty( name ): continue

            # to avoid auto-retrieve of private AlgTools, only call getattr() if already set elsewhere
            if isPropSet:
                value = getattr(conf,name)
            else:
                value = defaultProps[name]

            resolvedValue = self._resolveProperty(conf,name,value,checkType=isPropSet,indent=indent,propStack=propStack)
            if resolvedValue is None or resolvedValue == value:
                self.logger().verbose("%sProperty %s.%s=%r left unchanged", indent, conf.getFullName(),name, value)
                continue

            try:
                setattr(conf,name,resolvedValue)
            except Exception,err:
                raise ConfigurationError("ERROR in setting %s(%r).%s = %r\n  Exception raised: %s" %
                                         (conf.getType(),conf.getName(),name,value,err) )

        
    def numberOfInstances(self):
        n = 0
        for c in self._availableConfigurables.values():
            if c.hasInstance(): n += 1

        return n


    def numberOfConfigurables(self):
        return len(self._availableConfigurables)
    

    def checkProperties(self, configInstance, propsToCheck):
        # check that any newly requested properties are consistent with existing tool
        problem = ""
        oldValue = None
        problemName = None
        newValue = None
        for n,v in propsToCheck.items():
            try:
                oldProp = getPropertyNoAutoRetrieve(configInstance,n)
            except AttributeError:
                continue
            
            if hasattr(oldProp,'getName') and hasattr(oldProp,'getType'):
                newType,newName = getTypeAndName(v)
                oldType,oldName = getTypeAndName(oldProp)
                if oldName != newName or (newType != newName and oldType != newType):
                    oldValue = oldProp
                    problemName = n
                    newValue = v
                    break
            else:
                if oldProp != v:
                    oldValue = oldProp
                    problemName = n
                    newValue = v
                    break

        if problemName is not None:
            problem = "Existing property value %s(%r).%s = %r inconsistent with required property value %r" % \
                      (configInstance.getType(),configInstance.getName(),problemName,oldValue,newValue)

        return problem


    def _addConfigured(self, configuredMaker, instanceName, **configurableArgs):
        if instanceName in self._availableConfigurables:
            raise RuntimeError("Configurable with name %r already declared in factory" % (instanceName) )

        conf = ConfiguredPlaceHolder( configuredMaker, **configurableArgs )
        self._availableConfigurables[instanceName] = conf
        self.logger().verbose("Added configurable maker %s", conf.shortDescription(instanceName))


    def _addConfiguredClone(self, originalName, cloneName, **kwargs):
        if originalName == cloneName:
            raise RuntimeError("Can not make clone %r with same name as original" % (cloneName) )

        orig = self._availableConfigurables.get(originalName)
        if orig is None:
            raise RuntimeError("Can not make clone %r of non-existing original %r" % (cloneName,originalName))

        if cloneName in self._availableConfigurables:
            raise RuntimeError("Configurable with name %r already declared in factory" % (cloneName) )

        conf = orig.clone(**kwargs)
        self._availableConfigurables[cloneName] = conf
        self.logger().verbose("Added configurable maker clone %s from original %r", conf.shortDescription(cloneName), originalName)


    def checkConfigurableType(self,conf,requiredType):
        """Check that the configurable <conf> has the correct type (conf.getType()). C++ and equivalent python type names can be mixed."""
        typeRequested = string.translate( requiredType, ConfigurableDb._transtable )
        typeFound     = string.translate( conf.getType(), ConfigurableDb._transtable )
        if typeFound != typeRequested:
            raise TypeError("Requested configurable %s(%r) already exists with different type: %s" % (typeRequested,conf.getName(),typeFound))
        

    def getConfigured(self, instanceName, tryDefaultConfigurable=None, checkType=True, indent="", propStack = None):
        if not instanceName: return None
        if not self._hasReadDB: self.read()
        # first look in available instances
        confType,confName = getTypeAndName(instanceName)
        if not confName: return None
        self.logger().verbose("%sTrying to instantiate %s(%r)",indent,confType,confName)
        nextIndent=indent+" "
        if propStack is None: propStack = PropertyStack()
        conf = None
        # look in predefined configurables
        maker = self._availableConfigurables.get(confName)
        if maker is not None:
            conf = maker.getInstance()
            if conf is not None:
                self.logger().debug("%sUsing already configured %s(%r)",indent,conf.getType(),conf.getName())            
            else: # create it
                conf = maker.createInstance(confName)
                self.logger().verbose("%sInstantiated %s(%r). Starting to resolve dependencies",indent,conf.getType(),conf.getName())
                self._resolveAllProperties(conf,nextIndent,propStack)
                if checkType and confType != confName:
                    # explicit type given, check that existing type is as requested
                    self.checkConfigurableType(conf,confType)

                self.logger().debug("%sInstantiated configured %s(%r)",indent,conf.getType(),conf.getName())

        else:
            # look in default configurables (if requested)
            if tryDefaultConfigurable or tryDefaultConfigurable is None:
                # do not make defaults if only the type is given
                if self._excludeIfOnlyType(instanceName):
                    self.logger().verbose("%sNot instantiated because %r is only a type",indent,instanceName)
                    return None
                # skip explicitly excludes ones
                if self._skipIfNotAvailable(instanceName):
                    self.logger().verbose("%sNot instantiated because %r is not in database",indent,instanceName)
                    return None

                configurableClass = ConfigurableDb.getConfigurable(confType)
                if configurableClass is None:
                    self._defaultTypeNotFound.setdefault(confName,ConfigurableInstanceTracker(getFullName(instanceName)))
                    if tryDefaultConfigurable:
                        raise ConfigurableClassNotFoundError("Could not find configurable class for %s" % instanceName)
                    elif tryDefaultConfigurable is None:
                        self.logger().verbose("%sNot instantiated because configurable class %r does not exist",indent,confType)
                        return None

                else:
                    self.logger().verbose("%sGoing to create default configurable instance %s(%r)",indent,confType,confName)
                    conf = configurableClass(confName)
                    # this is the reason we make it even if tryDefaultConfigurable is not True
                    self._resolveAllProperties(conf,nextIndent,propStack)

                    if tryDefaultConfigurable:
                        self._createdDefaultInstances.setdefault(confName,ConfigurableInstanceTracker(getFullName(conf)))
                        self.logger().debug("%sInstantiated default %s(%r)",indent,conf.getType(),conf.getName())
                    else:
                        self.logger().verbose("%sGoing to delete default configurable instance %s(%r)",indent,conf.getType(),conf.getName())
                        return None # this will automatically delete the just created configurable (if no other references exist)
                    

        if conf is None:
            self.logger().verbose("%sCould not find configurable instance %r", indent, instanceName)
            raise ConfigurableInstanceNotFoundError("Could not find configurable instance %r" % (instanceName) )

        return conf


    def getConfiguredClone(self, cloneName, originalOrName, **kwargs):
        if not cloneName or not originalOrName: return None
        if not self._hasReadDB: self.read()
        # check if original is available (to catch mistakes like swapping cloneName and originalName)
        if type(originalOrName) == str and originalOrName not in self._availableConfigurables:
            raise ConfigurationError("Can not make clone %r of non-existing configurable %r" % (cloneName,originalOrName) )

        conf = None
        # first see if cloneName is already present in list of available configurables
        maker = self._availableConfigurables.get(cloneName)
        if maker is not None:
            conf = maker.getInstance()
            if conf is None:
                conf = maker.createInstance(cloneName)
                self._resolveAllProperties(conf)
                self.logger().info("Instantiated configurable %s(%r), requested as clone of %r", conf.__class__.__name__, conf.name(), originalOrName)
            else:
                self.logger().debug("Using existing configurable %s(%r), requested as clone of %r", conf.__class__.__name__, conf.name(), originalOrName)
                
            problem = self.checkProperties(conf,kwargs)
            if problem:
                raise ConfigurationError("Existing clone requested with incompatible properties: "+problem)


        if conf is None:
            # if we get here, clone needs to me made
            if type(originalOrName) == str:
                originalName = originalOrName # for printout later
                self._addConfiguredClone(originalName,cloneName,**kwargs)
                conf = self.getConfigured(cloneName,tryDefaultConfigurable=False,checkType=True)
                self.logger().info("Instantiated configurable %s(%r) as clone of %r", conf.__class__.__name__, conf.name(), originalName)

            elif isinstance(originalOrName,Configurable):
                originalName = originalOrName.name() # for printout later
                conf = originalOrName.clone(cloneName)
                for n,v in kwargs.items():
                    setattr(conf,n,v)
                self._resolveAllProperties(conf)
                # add to list for later retrieval
                self._availableConfigurables[cloneName] = ConfiguredPlaceHolder(conf)
                self.logger().info("Instantiated direct resolved clone %s(%r) from original %r", conf.__class__.__name__, conf.name(), originalName)


        if conf is None:
            # complete failure
            raise RuntimeError("Could not find configurable %r as source for clone %r" % (originalOrName,cloneName) )

        return conf
        

    #
    # Functions for AlgTools
    #
    def addTool(self, toolMaker, name, **toolArgs):
        """Add a recipe to make a tool"""
        self._addConfigured( toolMaker, name, **toolArgs )


    def addToolClone(self, originalName, cloneName, **kwargs):
        """Add a clone of an existing tool-recipe"""
        self._addConfiguredClone(originalName, cloneName, **kwargs)


    def checkTool(self,tool):
        """Check that the tool configurable instance is indeed an AlgTool.
        Raises ConfigurableTypeError exception is case of error, and returns tool in case of success"""
        if tool.getGaudiType() != 'AlgTool':
            raise ConfigurableTypeError("%r is not an AlgTool, but a %s" % (tool,tool.getGaudiType()))


    def _getTool(self, name, tryDefaultConfigurable, checkType, indent, propStack ):
        """Get private or public tool."""
        if not name: return None
        if tryDefaultConfigurable is None:
            tryDefault = self._tryDefaultTool or None
        else:
            tryDefault = tryDefaultConfigurable

        tool = self.getConfigured( name, tryDefault, checkType, indent, propStack )
        if tool is None: return None
        self.checkTool( tool )
        return tool


    def getPublicTool(self, name, tryDefaultConfigurable=None, checkType=False, allowNone=False, indent="", propStack=None):
        """Get public tool"""
        if not name:
            if allowNone: return None
            else: raise ConfigurationError("Requested Public Tool with empty name")

        global ToolSvc
        confType,confName = getTypeAndName(name)
        if not confName: return None
        # first try to get it from ToolSvc
        try:
            theTool = getattr(ToolSvc,confName)
            if confType != confName and theTool.getType() != confType:
                if checkType:
                    raise ConfigurableTypeError("Existing public tool %r has type %s while type %s is requested" % \
                                                (confName, theTool.getType(), confType) )
                else:
                    self.logger().info("Requested type %s of tool named %r overwritten by already defined type %s",
                                       confType,confName,theTool.getType())
            return theTool
        except AttributeError:
            # make it and add to ToolSvc
            theTool = self._getTool(name, tryDefaultConfigurable, checkType, indent, propStack)
            if theTool is None:
                if allowNone: return None
                else: raise ConfigurationError("Public Tool %r not found" % (name,) )
                    
            # some configurable makers already add the tool to ToolSvc
            if not hasattr(ToolSvc,theTool.getName()):
                ToolSvc += theTool
            return theTool


    def getPrivateTool(self, name, tryDefaultConfigurable = None, checkType = True, allowNone=False, indent = "", propStack=None, **toolProps):
        """Get private tool. Returns a (deep) copy of the tool, so really private"""
        if not name:
            if allowNone: return None
            else: raise ConfigurationError("Requested Private Tool with empty name")
            
        tool = self._getTool(name, tryDefaultConfigurable, checkType, indent, propStack)
        if tool is None:
            if allowNone: return None
            else: raise ConfigurationError("Private Tool %r not found" % (name,) )

        # start with a copy
        tool = copy.deepcopy( tool )
        # overwrite with user properties
        extraProps = self._resolveProperties( tool, toolProps, indent, propStack )
        for n,v in extraProps.items():
            setattr(tool,n,v)
        return tool


    def getPublicToolClone(self, cloneName, originalOrName, **toolProps):
        """Get a public clone of a pre-defined (public or private) tool"""
        if not cloneName or not originalOrName: return None
        global ToolSvc
        # check if clone is same as original
        origType,origName = getTypeAndName(originalOrName)
        if not origName: return None
        if origName == cloneName:
            self.logger().warning("Clone name same as original name. Returning unmodified original AlgTool named %r",cloneName)
            oldTool = self.getPublicTool(cloneName)
            problem = self.checkProperties(oldTool,toolProps)
            if problem:
                raise ConfigurationError("Existing AlgTool (clone=original) requested with incompatible properties: "+problem)

            return oldTool
        
        # try to get existing clone from ToolSvc
        try:
            oldTool = getattr(ToolSvc,cloneName)
            problem = self.checkProperties(oldTool,toolProps)
            if problem:
                raise ConfigurationError("Existing clone requested with incompatible properties: "+problem)
                
            return oldTool

        except AttributeError:
            theTool = self.getConfiguredClone( cloneName, origName, **toolProps )
            if theTool is None: return None
            self.checkTool(theTool)
            # some configurable makers already add the tool to ToolSvc
            if not hasattr(ToolSvc,theTool.getName()):
                ToolSvc += theTool
            return theTool


    def getPrivateToolClone(self, cloneName, originalOrName, **toolProps):
        """Get private tool. Returns a (deep) copy of the tool, so really private"""
        if not cloneName or not originalOrName: return None
        theClone = self.getConfiguredClone( cloneName, originalOrName, **toolProps )
        if theClone is None: return None
        self.checkTool( theClone )
        return copy.deepcopy( theClone )


    #
    # Functions for Services
    #
    def checkService(self,theSvc):
        """Make sure that theSvc configurable instance is indeed a Service.
        Raises ConfigurableTypeError exception is case of error, and returns theSvc in case of success"""
        if theSvc.getGaudiType() != 'Service':
            raise ConfigurableTypeError("%r is not a Service, but a %s" % (theSvc,theSvc.getGaudiType()))

    def addService( self, serviceMaker, name, **svcArgs ):
        self._addConfigured( serviceMaker, name, **svcArgs )

    def addServiceClone(self, originalName, cloneName, **kwargs):
        """Add a clone of a possible service to the list of possible services."""
        self._addConfiguredClone(originalName, cloneName, **kwargs)

    def getService( self, name, tryDefaultConfigurable=None, checkType=True, allowNone=False, indent="", propStack=None ):
        if not name:
            if allowNone: return None
            else: raise ConfigurationError("Requested Service with empty name")

        confType,confName = getTypeAndName(name)
        if not confName:
            if allowNone: return None
            else: raise ConfigurationError("Requested Service with empty instance name: %r" % (name,) )


        global ServiceMgr
        # first try to get it from ServiceMgr
        try:
            return getattr(ServiceMgr,confName)
        except AttributeError:
            # then try to create it
            if tryDefaultConfigurable is None:
                tryDefault = self._tryDefaultService or None
            else:
                tryDefault = tryDefaultConfigurable
            # make it and add to ServiceMgr
            theSvc = self.getConfigured( name, tryDefault, checkType, indent, propStack )
            if theSvc is None:
                if allowNone: return None
                else: raise ConfigurationError("Service %r not found" % (name,) )

            self.checkService(theSvc)
            ServiceMgr += theSvc
            return theSvc


    def getServiceClone( self, cloneName, originalOrName, **kwargs ):
        if not cloneName or not originalOrName: return None
        global ServiceMgr

        # check if clone is same as original
        origType,origName = getTypeAndName(originalOrName)
        if origName == cloneName:
            self.logger().warning("Clone name same as original name. Returning unmodified original Service named %r",cloneName)
            oldSvc = self.getService(cloneName)
            problem = self.checkProperties(oldSvc,kwargs)
            if problem:
                raise RuntimeError("Existing Service (clone=original) requested with incompatible properties: "+problem)

            return oldSvc
        

        # first try to get clone from ServiceMgr
        try:
            oldSvc = getattr(ServiceMgr,cloneName)
            problem = self.checkProperties(oldSvc,kwargs)
            if problem:
                raise ConfigurationError("Existing clone requested with incompatible properties: "+problem)
                
            return oldSvc

        except AttributeError:
            # then create clone from existing definition
            theSvc = self.getConfiguredClone( cloneName, origName, **kwargs )
            if theSvc is None: return None
            self.checkService(theSvc)
            # some configurable makers already add the service to ServiceMgr
            if not hasattr(ServiceMgr,theSvc.getName()):
                ServiceMgr += theSvc
            return theSvc


    def checkAlgorithm(self,theAlg):
        """Make sure that theAlg configurable instance is indeed an Algorithm.
        Raises ConfigurableTypeError exception is case of error, and returns theAlg in case of success"""
        if theAlg.getGaudiType() != 'Algorithm':
            raise ConfigurableTypeError("%r is not an Algorithm, but a %s" % (theAlg,theAlg.getGaudiType()))


    def addAlgorithm(self, algMaker, algName, **algProps ):
        self._addConfigured( algMaker, algName, **algProps )


    def addAlgorithmClone(self, originalName, cloneName, **kwargs):
        """Add a clone of a possible Algorithm to the list of possible algorithms"""
        self._addConfiguredClone(originalName, cloneName, **kwargs)


    def getAlgorithm( self, name, tryDefaultConfigurable=None, checkType=True, allowNone=False, indent="", propStack=None ):
        if not name:
            if allowNone: return None
            else: raise ConfigurationError("Requested Algorithm with empty name")

        if tryDefaultConfigurable is None:
            tryDefault = self._tryDefaultAlgorithm or None
        else:
            tryDefault = tryDefaultConfigurable

        theAlg = self.getConfigured( name, tryDefault, checkType, indent, propStack )
        if theAlg is None:
            if allowNone: return None
            else: raise ConfigurationError("Algorithm %r not found" % (name,) )

        self.checkAlgorithm(theAlg)        
        return theAlg


    def getAlgorithmClone( self, cloneName, originalOrName, **kwargs ):
        if not cloneName or not originalOrName: return None

        # check if clone is same as original
        origType,origName = getTypeAndName(originalOrName)
        if origName == cloneName:
            self.logger().warning("Clone name same as original name. Returning unmodified original Algorithm named %r",cloneName)
            oldAlg = self.getAlgorithm(cloneName)
            problem = self.checkProperties(oldAlg,kwargs)
            if problem:
                raise ConfigurationError("Existing Algorithm (clone=original) requested with incompatible properties: "+problem)

            return oldAlg
        
        # make it from original one
        theAlg = self.getConfiguredClone(cloneName, origName, **kwargs)
        if theAlg is None: return None
        self.checkAlgorithm(theAlg)
        return theAlg


    # if called at the end of the job, it will show some statistics
    def printStats(self):
        log = self.logger()
        nConfig = self.numberOfConfigurables()
        nInstances = self.numberOfInstances()
        sepLine = 80*"-";
        lines = [ "%d configurables were declared, %d instantiated (indicated by *):" % (nConfig, nInstances) ]
        # print in alphabetic order by name
        names = self._availableConfigurables.keys()
        names.sort(key = lambda x : x)
        for n in names:
            c = self._availableConfigurables[n]
            lines.append( c.shortDescription(n) )
        lines.append( "end of list of declared configurables" )
#        lines.append( sepLine )
        log.info( os.linesep.join(lines) )


        # print list of created defaults, in alphabetic order by type
        defaultInstances = self._createdDefaultInstances.values()
        defaultInstances.sort(key = lambda x : x.getType())
        nDefault = len(defaultInstances)
        if nDefault:
            lines = [ "%d default configurables were created:" % (nDefault) ]
            for c in defaultInstances:
                lines.append( "* %s" % (c) )
            lines.append("end of list of created default configurables")
            log.info( os.linesep.join(lines) )


        # print list of default configurables where the type was not found
        defaultTypesNotFound = self._defaultTypeNotFound.values()
        defaultTypesNotFound.sort(key = lambda x : x.getType())
        nTypesNotFound = len(defaultTypesNotFound)
        if nTypesNotFound:
            lines= [ "%d types not found for default configurable:" % (nTypesNotFound) ]
            for c in defaultTypesNotFound:
                lines.append( "  %s" % (c) )
            log.info( os.linesep.join(lines) )


        if self._excludedDependencies:
            lines = ["Defaults of the following %i configurables were not explicitly configured:" % \
                     len(self._excludedDependencies) ]
            for c in self._excludedDependencies:
                lines.append( "  %s" % c )
            log.info( os.linesep.join(lines) )

        if self._excludedDefaultProperties:
            lines = [ "%i properties with the following names and default values were not explicitly configured:" % \
                      len(self._excludedDefaultProperties) ]
            for c,count in self._excludedDefaultProperties.items():
                lines.append("  %s (%d times)" % (c,count) )
            log.info( os.linesep.join(lines) )

        if self._excludedOnlyTypes:
            lines = [ "%i default instances were not made because only the type was given:" % \
                      len(self._excludedOnlyTypes) ]
            for c in self._excludedOnlyTypes:
                lines.append("  %s" % c)
            log.info( os.linesep.join(lines) )                
        
        if self._ignoredDependencyErrors:
            lines = [ "%i errors in configuring C++ defaults were ignored:" % \
                      len(self._ignoredDependencyErrors) ]
            for c in self._ignoredDependencyErrors:
                lines.append("  %s" % c)
            log.info( os.linesep.join(lines) )
            

        del log
        

    def read(self):
        """Read definitions from Database modules"""
        import sys, os, os.path, time
       
        self.logger().debug( "importing configDb modules..." )
        nFiles = 0
        startTime = time.time()

        paths = [p for p in sys.path if p.endswith ('/python')]
        if not paths: paths = sys.path

        for path in paths:
            if not os.path.exists( path ):
                continue
            if not os.path.isdir (path): continue
         
            self.logger().verbose( "searching in %s..." % path )
         
            packages = os.listdir(path)
            for p in packages:
                dbFile = p+"ConfigDb.py"
                localfile = os.path.join(p,dbFile)
                globalfile = os.path.join(path,localfile)
                if not os.path.isfile(globalfile): continue
                    
                # turn filename syntax into module syntax: remove extension and replace / with . (dot)
                confDbModule = os.path.splitext(localfile)[0].replace(os.sep,'.')
                self.logger().debug( "importing %s..." % confDbModule )
                try:
                    mod = __import__( confDbModule, globals(), locals(), [dbFile] )
                except Exception, err:
                    self.logger().warning( "Error importing module %s !", confDbModule )
                    self.logger().warning( "Reason: %s", err )
                else:
                    nFiles += 1

        stopTime = time.time()
        self.logger().info( "imported %i confDb modules in %.2f seconds" % (nFiles,stopTime-startTime) )
        self._hasReadDB = True
        

    def instantiateAll(self):
        """Instantiate all declared tools,services and algorithms. Mainly for testing the full database."""
        for name in self._availableConfigurables.keys():
            self.getConfigured(name,tryDefaultConfigurable=False,checkType=False)

# end of class ConfiguredFactory
