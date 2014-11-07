# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


__all__ = [ 'ConfiguredMuonRec', 'GetMuonRec', 'CompositeMuonRec' ]

import os

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.ConfigurableDb import getConfigurable
from AthenaCommon.Logging import logging

from MuonRecUtils import RecConfigInfo,logMuon,logMuonResil
from AthenaCommon.ConfiguredFactory import getProperty

# default package used if no package is specified in configTag of GetMuonRec()
defaultPackage='MuonRecExample'

topSequence = AlgSequence()

def addDictToList(totalKeys, dataKeys):
    for key,value in dataKeys.items():
        keyList = totalKeys.setdefault(key,[])
        if type(value) != list: value = [ value ]
        for v in value:
            if v not in keyList: keyList.append(v)




class PropertyHandle:
    """Helper class to set and get properties of configurables"""
    def __init__(self,configurable,property):
        self.configurable = configurable
        self.property = property
        
    def set(self,value,log=None):
        try:
            retVal = setattr(self.configurable,self.property,value)
            if log:
                log.verbose("Setting property %s = %r", self.propertyName(), value) 
            return retVal
        except AttributeError:
            # change type of exception to avoid misleading error message
            raise RuntimeError("Configurable \'%s\' does not have property \'%s\'" % (self.configurable.__class__.__name__, self.property) )

    def get(self):
        try:
            return getProperty(self.configurable,self.property)
        except AttributeError:
            # change type of exception to avoid misleading error message
            raise RuntimeError("Configurable \'%s\' does not have property \'%s\'" % (self.configurable.__class__.__name__, self.property) )

    def name(self):
        return self.configurable.getName()

    def propertyName(self):
        return self.name() + '.' + self.property


class ConfiguredMuonRec(object):
    """Base class from which all muon reco configuration classes need to derive"""

    def __init__(self,name,**kwargs):
        """<name> is used to access components of CompositeMuonRec"""
        self._name = name
        # enabled determines if algorithms are added to topSequence or not
        self._enabled = kwargs.get("enabled", True)
        self._algs = []
        self._publicTools = []
        # store parent Config (in CompositeMuonRec) if given (or None otherwise)
        self._parent = kwargs.get('parent')
        # make it possible to share dataKeys
        self._inputKeys = kwargs.get( 'inputKeys', {} ) # dictionary with names and values of input + output data keys
        self._outputKeys = kwargs.get( 'outputKeys', {} ) # dictionary with names and values of input + output data keys
        self._registeredInputKeys = {} # key=dataKey, value=list of configurable property handlers
        self._registeredOutputKeys = {} # key=dataKey, value=list of configurable property handlers
        # get personal logger
        self._log = logging.getLogger(self.fullName())
        
    def __str__(self,options='header,dataKeys,publicTools,algorithms,footer'):
        # support <options> as list, or as string with comma separated list
        if type(options) != list:
            options = options.split(',')
        lines = []
        if 'header' in options:
            lines.append( self._header() )
        if 'dataKeys' in options:
            dataKeys = self.dataKeys()
            if len(dataKeys) > 0:
                lines.append('## Storegate Data Keys: ##')
                keys = dataKeys.keys()
                inKeys = self.registeredInputKeys()
                outKeys = self.registeredOutputKeys()
                keys.sort() # show alphabetically
                for key in keys:
                    lines.append( '%s=%r' % (key,dataKeys[key]) )
                    props = outKeys.get(key)
                    if props:
                        propStr = ', '.join([p.propertyName() for p in props])
                        lines.append('  Output: ' + propStr)
                    props = inKeys.get(key)
                    if props:
                        propStr = ', '.join([p.propertyName() for p in props])
                        lines.append('   Input: ' + propStr)
        if 'publicTools' in options and len(self.publicTools()) > 0:
            lines.append( '## Public Tools: ##' )
            for t in self.publicTools():
                lines.append( t.getFullJobOptName() )
        if 'algorithms' in options and len(self.algs()) > 0:
            lines.append( '## Algorithms: ##' )
            for a in self.algs():
                lines.append( a.getFullJobOptName() )
        if 'footer' in options:
            lines.append( self._footer() )
        return os.linesep.join(lines)


    def _header(self):
        return '<<<<<< Start of Reco Config: %s (class %s.%s) >>>>>>' % \
               (self.fullName(), self.__class__.__module__, self.__class__.__name__)

    def _footer(self):
        return '<<<<<<   End of Reco Config: %s (class %s.%s) >>>>>>' % \
               (self.fullName(), self.__class__.__module__, self.__class__.__name__)

    def name(self):
        return self._name
    
    def fullName(self):
        name = self.name()
        if self._parent:
            name = self._parent.fullName() + '.' + name
        return name

    def empty(self):
        """A config is empty if it has no algorithms and no public tools"""
        return len(self.algs()) == 0 and len(self.publicTools()) == 0

    def algs(self):
        """Return (copy of) list of algorithms defined by this unit"""
        return self._algs[:]

    def alg(self,name):
        """Return algorithm by name. Raises KeyError exception if not found."""
        for a in self.algs():
            if a.getName() == name:
                return a
        raise KeyError('%s does not have algorithm %s' % (self.fullName(),name))
            
    def hasAlg(self,name):
        """Check if algorithm <name> is part of this config"""
        try:
            self.alg(name)
        except KeyError:
            return False
        return True

    def addAlg(self,alg):
        """Add ConfigurableAlgorithm instance for the topSequence, and store
        in local dictionary, with algorithm name as key.
        If <alg> is a string, it is assumed to be the C++ class
        name, and the corresponding (default) configurable is
        retrieved and added. The configurable is returned.
        If <alg> is None, then None is returned."""
        if alg is None: return None
        global topSequence
        if type(alg) == str:
            alg = getConfigurable( alg )()
        # check that it is indeed Algorithm
        gaudiType = alg.getGaudiType()
        if gaudiType != 'Algorithm':        
            raise TypeError( '%r is not an Algorithm, but an %s' % (alg,gaudiType) )
        # add to local dictionary
        if not alg in self._algs:
            self._algs.append( alg )
            # add to own dict for direct access: self.AlgName
            self.__dict__[ alg.getName() ] = alg
        return alg

    def removeAlg(self,algName):
        global topSequence
        try:
            self._algs.remove( self.__dict__[algName] )
        except KeyError:
            pass
        try:
            del self.__dict__[ algName ]
        except KeyError:
            pass
        try:
            delattr(topSequence,algName)
        except AttributeError:
            pass

    def addPublicTool(self,tool):
        """Add ConfigurableAlgTool instance to ToolSvc, and store
        in local dictionary, with tool name as key.
        If <tool> is a string, it is assumed to be the C++ class
        name, and the corresponding (default) configurable is
        retrieved and added. The configurable is returned."""
        global ToolSvc
        if type(tool) == str:
            tool = getConfigurable( tool )()
        gaudiType = tool.getGaudiType()
        if gaudiType != 'AlgTool':        
            raise TypeError( '%r is not an AlgTool, but an %s' % (tool,gaudiType) )
        ToolSvc += tool
        self._publicTools.append( tool )
        self.__dict__[ tool.getName() ] = tool
        return tool

    def publicTools(self):
        """Return (copy of) list of public tools defined by this unit"""
        return self._publicTools[:]

    def publicTool(self,name):
        """Return public tool by name. Raises KeyError exception if not found."""
        for t in self.publicTools():
            if t.getName() == name:
                return t
        raise KeyError('%s does not have public tool %s' % (self.fullName(),name))        

    def hasPublicTool(self,name):
        try:
            self.publicTool(name)
        except KeyError:
            return False
        return True

    def registerOutputKey(self,key,configurable,property):
        """Register a data output key to be read from/write to the configurable property.
        If the key is already present, its current value will be kept and transferred to the configurable."""
        handle = PropertyHandle(configurable,property)
        # take value from existing key
        value = self._outputKeys.get(key)
        # else take it from configurable
        if value is None: value = handle.get()
        # add it to the list
        value = self.addOutputKey(key,value)
        # update the value (in case it changed)
        handle.set(value)
        # store ref to read/write value later on
        propList = self._registeredOutputKeys.setdefault(key,list())
        propList.append( handle )
        return value

    def registeredOutputKeys(self):
        return dict(self._registeredOutputKeys)

    def addOutputKey(self,key,value,propagate=True):
        """Add a output key directly to the list of output Keys. To be used if value can not be
        read from a configurable (via registerOutputKey()), to overwrite an existing key or to 'hardcode' a key"""
        self._outputKeys[key] = value
        if propagate: self._propagateDataKey(key,value)
        return value

    def outputKeys(self):
        """List of output keys"""
        return dict(self._outputKeys) # return a copy

    def registerInputKey(self,key,configurable,property):
        """Register a data input key to be read from/written to the configurable property.
        If the key is already present, its current value will be kept and transferred to the configurable."""
        handle =  PropertyHandle(configurable,property)
        # existing value has priority
        value = self._inputKeys.get(key)
        # else get value from configurable
        if value is None: value = handle.get()
        # add to list (value may be taken from outputKeys, which has priority)
        value = self.addInputKey(key,value)
        # value may have been changed (taken from existing output one), so update it
        handle.set(value)
        # store ref to read/write value later on
        propList = self._registeredInputKeys.setdefault(key,list())
        propList.append( handle )
        return value
        
    def registeredInputKeys(self):
        return dict(self._registeredInputKeys)

    def addInputKey(self,key,value=None,propagate=True):
        """Add a input key directly to the list of input Keys. To be used if value can not be
        read from a configurable (via registerInputKey()), or to overwrite a value.
        If the key already exists as an outputKey, the outputKey value will be used instead of the given value.
        The actually used and stored value is returned."""
        # output key values have priority over input key values
        try:
            outputKey = self._outputKeys[key]
        except KeyError:
            if value is None:
                raise RuntimeError("InputKey %r does not exist yet as OutputKey and no value given" % key)
        else:
            value = outputKey
        self._inputKeys[key] = value
        if propagate: self._propagateDataKey(key,value)
        return value

    def inputKeys(self):
        """List of input keys"""
        return dict(self._inputKeys) # return copy

    def dataKeys(self):
        """Return (copy of) dictionary with data key values indexed by key names."""
        keys = dict(self.inputKeys())
        # output keys have priority
        keys.update(self.outputKeys())
        return keys

    def outputKey(self,name):
        """Return value of output key"""
        return self_.outputKeys[name]

    def hasOutputKey(self,name):
        return name in self._outputKeys

    def inputKey(self,name):
        """Return value of output key"""
        return self_.inputKeys[name]

    def hasInputKey(self,name):
        return name in self._inputKeys

    def dataKey(self,name):
        """Return value of dataKey with given <name>"""
        return self.dataKeys()[name]

    def hasDataKey(self,name):
        return name in self.dataKeys()

    def addDataKeys(self,keys):
        """Add a dictionary of keys to the data keys (both input and output)"""
        for k,v in keys.items():
            self.addDataKey(k,v)

    def addDataKey(self,key,value,propagate=True):
        """Add (overwrite) a single data key (used for both input and output)"""
        self.addInputKey(key,value,False)
        self.addOutputKey(key,value,False)
        if propagate: self._propagateDataKey(key,value)

    def _propagateDataKey(self,key,value):
        """Propagate the date keys to the registered properties"""
        # propagate input keys
        self._log.debug("Propagating key %s=%r",key,value)
        inputList = self._registeredInputKeys.get(key)
        if inputList:
            for p in inputList:
                p.set(value,self._log)
        # propagate output keys
        outputList = self._registeredOutputKeys.get(key)
        if outputList:
            for p in outputList:
                p.set(value,self._log)

    def propagateDataKeys(self,keys=None):
        """Propagate the date keys to the registered properties"""
        if keys is None: keys = self.dataKeys()
        for k,v in keys.items():
            self._propagateDataKey(k,v)

    def configure(self,keys=None):
        """Do post-init configuration"""
        global topSequence
        if self._enabled:
            for alg in self._algs:
                if not hasattr(topSequence,alg.getName()):
                    topSequence += alg
        # all internal keys are already up-to-date, and don't need propagation
        # So only propagate external keys
        if keys is not None: self.propagateDataKeys(keys)

    def unConfigure(self):
        """Undo the configuration (as far as possible): remove algorithms from topSequence"""
        self.disable()

    def disable(self):
        """Remove all algorithms from topSequence"""
        self._enabled = False
        for a in self._algs:
            name = a.getName()
            try:
                delattr(topSequence,name)
            except AttributeError:
                # already removed
                pass
        
    def enable(self):
        """Add all algorithms to topSequence, if not already in.
        Only needs to be called if it was disabled in the first place, but want to use anyway.
        BEWARE: as this adds to the end of topSequence, total order in topSequence may not be correct!!!"""
        self._enabled = True
        for alg in self._algs:
            if not hasattr(topSequence,alg.getName()):
                topSequence += alg

    def isEnabled(self):
        return self._enabled
        
    def setProperty(self,name,value,components=['algs','tools']):
        """For all indicated types of <components> in this config that have property <name>,
        set the property to <value>.
        <components> is either a list fo strings or a string with comma separated types.
        Recognised component types:
        'algs'  : the (top) algorithms
        'tools' : the public tools"""
        if type(components) == str: components = components.split(',')
        if 'algs' in components:
            for a in self.algs():
                try:
                    setattr(a,name,value)
                except AttributeError:
                    pass
        if 'tools' in components:
            for t in self.publicTools():
                try:
                    setattr(t,name,value)
                except AttributeError:
                    pass

    def setOutputLevel(self,value,components=['algs','tools']):
        """Set the output level of all components to <value>."""
        self.setProperty("OutputLevel",value,components)



class CompositeMuonRec(ConfiguredMuonRec):
    """A sequence of ConfiguredMuonRec's"""
    def __init__(self, name, confList, applyResilience=False, shareDataKeys=True, **kwargs):
        if shareDataKeys:
            # share all dataKeys with children
            kwargs['outputKeys'] = kwargs.get('outputKeys',{})
            kwargs['inputKeys' ] = kwargs.get('inputKeys' ,{})
        ConfiguredMuonRec.__init__(self, name, **kwargs)
        self._shareDataKeys = shareDataKeys
        # topConfigs to ensure proper order of added algorithms
        self._topConfigs = [ ConfiguredMuonRec("first",**kwargs) ]
        self._applyResilience = applyResilience
        self._subConfigs = []
        self._subConfigsByName = {}
        self._subConfigsByClassName = {}
        if type(confList) == str:
            confList = confList.split('+') # allow sequence syntax: "CONF1+CONF2+CONF3"
        for t in confList:
            self.addConfig(t.strip(),False,**kwargs)

    def __str__(self,options='header,dataKeys,publicTools,algorithms,footer'):
        # support <options> as list, or as string with comma separated list
        if type(options) != list:
            options = options.split(',')
        mainOptions = [ o for o in options if o not in ['publicTools', 'algorithms','footer'] ]
        lines = [ ConfiguredMuonRec.__str__(self,mainOptions) ]
        # don't repeat printing of datakeys if shared
        if self._shareDataKeys:
            subOptions = [ o for o in options if o not in ['dataKeys'] ]
        else:
            subOptions = options[:]
        for c in self.allConfigs():
            if c.isEnabled():
                lines.append( c.__str__(subOptions) )
        # print footer
        if 'footer' in options:
            lines.append( self._footer() )
        return os.linesep.join(lines)

    def __getattr__(self,name):
        """Add support for getting names from sub-config directly"""
        for c in self._topConfigs + self._subConfigs:
            try:
                return getattr(c,name)
            except AttributeError:
                pass # try the next one
        # not found
        raise AttributeError('%s does not have attribute %s' % (self.__class__.__name__,name))

    def addConfig(self,conf,doConfigure=True,**kwargs):
        """Add a sub-configuration. <conf> can either be a ConfiguredMuonRec instance,
        or a string (a configuration tag)"""
        if conf is None: return None
        # set self as parent
        kwargs['parent'] = self
        if self._shareDataKeys:
            kwargs['outputKeys'] = self._outputKeys
            kwargs['inputKeys'] = self._inputKeys
        if type(conf) == str:
            conf = GetMuonRec(conf, doConfigure, self._applyResilience, **kwargs)
            if conf is None: return None
        elif not isinstance(conf,ConfiguredMuonRec):
            raise TypeError("%r is not a ConfiguredMuonRec" % conf)
        if conf in self._subConfigs:
            self._log.warning("Configuration %s already has config %s. Not adding a second time.",
                              self.__class__.__name__, conf.fullName())
            return conf
        oldConf = self._subConfigsByName.get( conf.name() )
        if oldConf is not None:
            message = "Configuration %s (%s) already has a config named %s (%s). Attempt to add (%s)" % \
                      (self.fullName(), self.__class__.__name__,
                       conf.name(), oldConf.__class__.__name__, conf.__class__.__name__ )
            if self._applyResilience:
                logMuonResil.error(message + ": Not added")
                return oldConf
            else:
                raise RuntimeError(message)
        # OK, add it
        self._subConfigs.append(conf)
        self._subConfigsByName[conf.name()] = conf
        self._subConfigsByClassName[conf.__class__.__name__] = conf
        self.__dict__[conf.name()] = conf # allow access: self.<confName>
        # add next topConfig
        topConf = ConfiguredMuonRec("after_" + conf.name(),**kwargs)
        self._topConfigs.append( topConf )
        self.__dict__[topConf.name()] = topConf
        return conf

    def removeConfig(self,name):
        try:
            c = self.getConfig(name)
        except KeyError:
            pass # nothing to be done
        else:
            # remove it
            c.unConfigure()
            idx = self._subConfigs.index(c)
            self._subConfigs.remove(c)
            del self._subConfigsByName[c.name()]
            del self.__dict__[c.name()]
            # remove the corresponding topConfig
            del self._topConfigs[idx+1]

    def configure(self,keys=None):
        if keys is None: keys = self.dataKeys()
        for c in self.allConfigs():
            c.configure(keys)

    def unConfigure(self):
        for c in self.allConfigs():
            c.unConfigure()

    def _currentTopConfig(self):
        return self._topConfigs[-1]

    def addAlg(self,alg):
        """Add algorithm after last added sub-config, or to pre-list
        if no sub-config is available. This is to ensure the correct
        order of algorithms"""
        self._currentTopConfig().addAlg(alg)

    def algs(self):
        """Return all algorithms of sub-configs and this top config"""
        all = []
        for c in self.allConfigs():
            all += c.algs()
        return all
    
    def publicTools(self):
        all = []
        for c in self.allConfigs():
            all += c.publicTools()
        return all

    def addPublicTool(self,tool):
        self._currentTopConfig().addPublicTool(tool)

    def _propagateDataKey(self,key,value):
        for c in self.allConfigs():
            c._propagateDataKey(key,value)

    def propagateDataKeys(self,keys=None):
        """Propagate the date keys to the registered properties"""
        if keys is None: keys = self.dataKeys()
        ConfigurableMuonRec.propagateDataKeys(self,keys)
        for c in self.allConfigs():
            c.propagateDataKeys(keys)

    def inputKeys(self):
        """List of input keys"""
        allKeys = {}
        if not self._shareDataKeys:
            for c in self.allConfigs():
                allKeys.update(c.inputKeys())
        # high level keys have priority
        allKeys.update( self._inputKeys )
        return allKeys

    def outputKeys(self):
        """List of output keys"""
        allKeys = {}
        if not self._shareDataKeys:
            for c in self.allConfigs():
                allKeys.update(c.outputKeys())
        # high level keys have priority
        allKeys.update( self._outputKeys )
        return allKeys

    def registerInputKey(self,key,configurable,property):
        value = self._currentTopConfig().registerInputKey(key,configurable,property)
        self._propagateDataKey(key,value)

    def registeredInputKeys(self):
        allKeys = {}
        for c in self.allConfigs():
            addDictToList(allKeys, c.registeredInputKeys())
        return allKeys

    def registerOutputKey(self,key,configurable,property):
        value = self._currentTopConfig().registerOutputKey(key,configurable,property)
        self._propagateDataKey(key,value)
    
    def registeredOutputKeys(self):
        allKeys = {}
        for c in self.allConfigs():
            addDictToList(allKeys, c.registeredOutputKeys())
        return allKeys

    def getConfig(self,name):
        """Get the sub-config with <name> equal to its name or class name. Throws KeyError exception if not found."""
        try:
            return self._subConfigsByName[name]
        except KeyError:
            try:
                return self._subConfigsByClassName[name]
            except KeyError:
                raise KeyError("%s does not have sub-config %r" % (self.name(),name))

    def hasConfig(self,name):
        """Return if sub-configuration <name> is present and enabled"""
        try:
            c = self.getConfig(name)
        except KeyError:
            return False
        else:
            return c.isEnabled()

    def configs(self):
        """A copy of the list of sub configurations"""
        return self._subConfigs[:]

    def allConfigs(self):
        """A copy of the list of top (non-empty) + sub configurations, in the correct order"""
        all = []
        topConfs = self._topConfigs
        subConfs = self._subConfigs
        if not topConfs[0].empty():
            all.append( topConfs[0] )
        for i in range(len(subConfs)):
            all.append(subConfs[i])
            top = topConfs[i+1]
            if not top.empty(): all.append(top)
        return all

    def configNames(self):
        names = []
        for c in self._subConfigs:
            names.append(c.name())
        return names



class ParallelMuonRec(CompositeMuonRec):
    """A configuration of muons reconstructions that could run in parallel:
    completely independent separate reco's"""
    def __init__(self, name, confList, applyResilience=False, **kwargs):
        CompositeMuonRec.__init__(self, name, confList, applyResilience, False, **kwargs)

    def inputKeys(self):
        """Dictionary of input keys with all values of all parallel recos."""
        allKeys = {}
        for c in self.allConfigs():
            addDictToList(allKeys,c.inputKeys())
        addDictToList(allKeys,self._inputKeys)
        return allKeys

    def registeredInputKeys(self):
        return {}

    def outputKeys(self):
        """Dictionary of output keys with all values of all parallel recos."""
        allKeys = {}
        for c in self.allConfigs():
            addDictToList(allKeys,c.outputKeys())
        addDictToList(allKeys,self._outputKeys)
        return allKeys

    def registeredOutputKeys(self):
        return {}

    def dataKeys(self):
        allKeys = {}
        for c in self.allConfigs():
            addDictToList(allKeys,c.dataKeys())
        # overwrite with my local data keys
        addDictToList(allKeys,self.localDataKeys())
        return allKeys

    def localDataKeys(self):
        keys = dict(self._inputKeys)
        keys.update(self._outputKeys)
        return keys

    def propagateDataKeys(self,keys=None):
        """Propagate the data keys to the registered properties"""
        if keys is None: keys = self.localKeys()
        for c in self.allConfigs():
            c.propagateDataKeys(keys)

    def configure(self,keys=None):
        for c in self.allConfigs():
            try:
                c.configure(keys)
            except:
                if self._applyResilience:
                   import traceback
                   print traceback.format_exc().rstrip()
                   logMuonResil.error("Configuration of %s FAILED. Switching off", c.fullName())
                else:
                    raise


# the dictionary containing created instances
_muonRecInstances = {}

def GetMuonRec(configTag,doConfigure=True,applyResilience=False, **kwargs):
    """Main end-user entry point to get a specific Muon reconstruction configuration.
    <configTag> is a string containing dot-separated fields:
    \"[package].[module].<class>\"
    <class>             : the name of the configuration class
    [module] (optional) : the name of the python module (inside package)
                 default: equal to <class>
    [package] (optional): the name of the package that contains the python module
                 default: given by defaultPackage member of this module
    If <applyResilience>=True, then any python exception in the configuration
    is caught, an ERROR message is printed, and None is returned.
    If <doConfigure>=True, then configure() is called after creation
    <kwargs> is an optional list of named arguments (arg1=value1,arg2=value2,etc),
    which will be passed on to the constructor of the configuration class.
    When called the first time for a particular configuration, an instance
    of the class is created and returned. When called more than once, the
    same instance is returned ('singleton')."""
    #
    # do some checks and set defaults
    #
    if configTag is None: return None
    global defaultPackage
    configInfo = RecConfigInfo(configTag,defaultPackage)
    if not configInfo:
        if applyResilience:
            return None
        else:
            raise RuntimeError('MuonRec configuration \"%s\" does not have format \"[package].<module>.[class]\"', configTag)
    #
    # get the instance
    #
    try:
        # existing instance
        return _muonRecInstances[configInfo.fullTag]
    except KeyError:
        # new instance
        className = configInfo.className
        modName = configInfo.fullModuleName
        mods = modName.split('.')
        if applyResilience:
            oldTopSequence = topSequence.getChildren() # is a copy
        try: # block for resilience
            # load the top-level module
            mod = __import__(modName)
            # get down to the lowest-level module
            try:
                for m in mods[1:]: mod = getattr(mod,m)
            except AttributeError:
                logMuon.error('Problem loading Muon Reconstruction configuration: "%s"', configTag)
                raise ImportError('module %s does not have submodule %s' % (mod.__name__,m))
            # get the class
            try:
                theClass = getattr(mod,className)
            except AttributeError:
                logMuon.error('Problem loading Muon Reconstruction configuration: "%s"', configTag)
                raise AttributeError('module %s does not contain class %s' % (modName,className))
            # instantiate and configure
            theInstance = theClass(**kwargs)
            if doConfigure: theInstance.configure()
            # store for later reference ('singleton')
            _muonRecInstances[configInfo.fullTag] = theInstance
            return theInstance
        except:
            if applyResilience:
                import traceback
                print traceback.format_exc().rstrip()
                logMuonResil.error("Configuration of \"%s\" FAILED. Switching off.", configTag)
                try:
                    theInstance.unConfigure()
                except UnboundLocalError:
                    pass
                # restore topSequence
                newTopSequence = topSequence.getChildren() # is a copy
                added = [ c for c in newTopSequence if c not in oldTopSequence ]
                if added:
                    logMuonResil.info("Removing TopAlgs: %s", ', '.join([s.getFullName() for s in added]))
                    topSequence.remove( added )
                return None
            else:
                raise


# backwards compat
GetConfiguredMuonRec = GetMuonRec
