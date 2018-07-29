###########################################################################
#    
#    <nozicka@mail.desy.de>
#
#    Package TrigConfOffline
#    Python class HLTAlgorithm
#
###########################################################################

import string
import weakref

class HLTAlgorithm (object) :
  def __init__(self, *args, **kwargs) :
    # init logger
    self.__initLogger__()
    # Assign private attributes
    self.__dict__['_children'] = {}
    self.OutputLevel=3
    self.dbId=0

    #self._setdefault()
    for k,v in kwargs.items() :
      self.__setattr__(k,v)
  
  def __initLogger__(self) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTAlgorithm')
    self._log.setLevel(3)
    #import logging
    #self.__dict__['_log']=logging.getLogger('HLTAlgorithm')
    #self._log.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #self._log.addHandler(ch)
  
  def __setattr__(self, name, value):
    _attributes=('name','alias','alg_type','py_name','py_package','OutputLevel','dbId','topAlg','parent','level')
    if name in _attributes:
      self._log.verbose('Set %s = %s' % (name, value))
      # Exception for setting the alias - possible renaming of references
      if name == 'alias' :
        self.setAlias(value)
      else :
        self.__dict__[name] = value
        
      # Setting the OutputLevel changes the _log 
      if name=='OutputLevel':
        self._log.setLevel((value-1)*10)
        
    else:
      self._log.warning('Unsupported attribute %s' % (name))
  
  def __str__(self):
    output=''
    prepend =''
    output += self.algToStr(prepend)
    return output
  
  def setlog(self):
    if self.__dict__.has_key('OutputLevel'):
      self._log.setLevel(self.OutputLevel)
    origmessage = self._log.name
    message = 'HLTAlgorithm '
    if self.__dict__.has_key('name'):
      message += '%s/' % (self.name)
    
    if self.__dict__.has_key('alias'):
      message += '%s' % (self.alias)
    # If the logger message changed
    if origmessage != message:
      self._log.name = message
      
  ## Delete HLTAlgorithm and references to it
  def delete(self) :
    self.setlog()
    # Delete itself and the reference from the parent
    # Use with caution
    if self.hasParent() :
      self.getParent().removeChild(self.alias)
    del self
  
  ## Convert the HLTAlgorithm for pretty print to string
  #  @param self The object pointer
  #  @param prepend The string to be prepended to the output
  #  @param printChildren Flag to print algorithm children too
  #  @return string
  def algToStr(self,prepend,printChildren=True):
    self.setlog()
    name = ''
    if self.__dict__.has_key('name'):
      name=self.name
    alias = ''
    if self.__dict__.has_key('alias'):
      alias=self.alias
    alg_type = 'Algorithm'
    if self.__dict__.has_key('alg_type'):
      alg_type=self.alg_type
    py_name = ''
    if self.__dict__.has_key('py_name'):
      py_name=self.py_name
    py_package = ''
    if self.__dict__.has_key('py_package'):
      py_package=self.py_package
    dbId = 0
    if self.__dict__.has_key('dbId'):
      dbId=self.dbId
      
    output = '\n%s/*****%s %s/%s (Python: import %s from %s) DB ID: %d' % (prepend,alg_type,name,alias,py_name,py_package,dbId)
    if self.__dict__.has_key('properties'):
      for n,v in self.properties.items() :
        output += '\n%s|-%s\t\t%s' % (prepend,n,v)

    if printChildren :
      prepend += '|'
      if self.__dict__.has_key('_children'):
        for child in self.getChildren() :
          output += child.algToStr(prepend)
          
    output += '\n%s\------------------------------------------------------' % (prepend)
    return output
        
  ## Set algorithm property
  #  @param self Object pointer
  #  @param name  Property name
  #  @param value Property value
  def setProperty(self, name, value):
    self.setlog()
    if not self.__dict__.has_key('properties'):
      self.__dict__['properties'] = {}
    
    self._log.verbose('%s/%s.%s - value type: %s value %s' % (self.getName(), self.getAlias(), name, type(value), value))
    
    # Test the split parameter type
    split_param_flag = False
    if '__IPC__' in name :
      try :
        index = eval(name[name.find('__IPC__')+len('__IPC__'):].lstrip('0'))
        if isinstance(index,int): split_param_flag=True
      except :
        pass
    
    # Try to evaluate the value
    if isinstance(value,str) and not split_param_flag :
      try :
        value = eval(value)
        self._log.verbose('%s/%s.%s - value type: %s value %s' % (self.getName(), self.getAlias(), name, type(value), value))
      except :
        pass
      
    self.properties[name] = value
    self._log.debug('Property %s = %s' % (name, value))
  
  ## Append child algorithm if exists - merge algorithms
  #  @param self Object pointer
  #  @param alg HLTAlgorithm child algorithm to be appended
  def appendChild(self, alg) :
    self.setlog()
    success = True
    # Test whether attribute _children exists
    if not self.__dict__.has_key('_children'):
      self.__dict__['_children'] = {}
    
    # Test if the child algorithm is present
    if self._children.has_key(alg.alias):
      self._log.debug('Existing child %s/%s - algorithms will be merged' % (self._children[alg.alias].getName(),alg.getAlias()))
      # Check the compatibility
      if alg.name == self._children[alg.alias].getName() :
        success = self._children[alg.alias].mergeAlgorithm(alg)
      else :
        self._log.error("Can not append child %s/%s - incompatible - existing name: %s" % (alg.name, alg.alias, self._children[alg.alias].getName()))
        success = False
    else:
      self.__dict__['_children'][alg.alias] = alg
      self.__dict__['_children'][alg.alias].setParent(self)
      
    return success
  
  ## Set algorithm child - existing child will be deleted
  #  @param self Object pointer
  #  @param alg HLTAlgorithm child algorithm to be set
  def setChild(self, alg):
    self.setlog()
    if not self.__dict__.has_key('_children'):
      self.__dict__['_children'] = {}
    # Test whether to replace an existing child
    if self._children.has_key(alg.alias) :
      self._log.debug('Child %s/%s exists - will be deleted' % (self.__dict__['_children'][alg.alias].name, alg.alias))
      self.__dict__['_children'][alg.alias].delete()
    # Add the child
    self.__dict__['_children'][alg.alias] = alg
    self.__dict__['_children'][alg.alias].setParent(self)
    
  ## Get algorithm child - existing child will be deleted
  #  @param self Object pointer
  #  @param alias Child alias
  #  @return return HLTAlgorithm child or None
  def getChild(self, alias) :
    self.setlog()
    if self.hasChildren() and self._children.has_key(alias) :
      return self._children[alias]
    else:
      return None
  
  ## Remove algorithm child - existing child will be deleted
  #  @param self Object pointer
  #  @param alias Child alias
  #  @return return True if successful
  def removeChild(self, alias) :
    self.setlog()
    success = True
    if self.hasChildren() and self._children.has_key(alias) :
      del self._children[alias]
    else:
      success = False
    return success 
  
  def getChildren(self):
    self.setlog()
    if not self.__dict__.has_key('_children'):
      return []
    return self._children.values()

  def hasChildren(self) :
    self.setlog()
    if not self.__dict__.has_key('_children'):
      return False
    
    if len(self._children) <= 0 :
      return False
    return True

  def getName(self) :
    self.setlog()
    if not self.__dict__.has_key('name'):
      return None
    return self.name
  
  def setName(self, name):
    self.setlog()
    # Change own name
    if self.__dict__.has_key('name'):
      self._log.debug('Previous name %s change to %s' % (self.name, name))
    self.name = name
    return True
    
  def getAlias(self):
    self.setlog()
    if not self.__dict__.has_key('alias'):
      return None
    return self.alias
  
  def setAlias(self, alias):
    self.setlog()
    # Process all children
    for child in self.getChildren() :
      childAlias = child.alias.replace(self.alias,alias,1)
      child.setAlias(childAlias)
      
    # Change own alias
    if self.__dict__.has_key('alias') and self.alias != alias :
      self._log.debug('Previous alias %s change to %s' % (self.alias, alias))
      # Change the reference in _children of the parent algorithm
      if self.hasParent():
        self.getParent().removeChild(self.alias)
        self.__dict__['alias'] = alias
        self.getParent().setChild(self)
        
    self.__dict__['alias'] = alias
  
  def getType(self):
    self.setlog()
    if not self.__dict__.has_key('alg_type'):
      return None
    return self.alg_type
  
  def getPy_package(self):
    self.setlog()
    if not self.__dict__.has_key('py_package'):
      return None
    return self.py_package
  
  def getPy_name(self):
    self.setlog()
    if not self.__dict__.has_key('py_name'):
      return None
    return self.py_name
  
  def getTopAlg(self):
    self.setlog()
    if not self.__dict__.has_key('topAlg'):
      return None
    return self.topAlg
  
  # Merge self with another algorithm
  def mergeAlgorithm(self,alg, mergeProperties=[]) :
    self.setlog()
    self._log.debug('mergeAlgorithm %s/%s' % (alg.name, alg.alias))
    success = True
    # Merge properties of the algorithms
    if not self.__dict__.has_key('properties'):
      self.properties=alg.getProperties()
    else :
      success = self.mergeProperties(alg,mergeProperties=[])
    
    # Merge children of the algorithms
    for child in alg.getChildren() :
      if not self.appendChild(child) :  success = False
    
    return success
  
  # Merge properties with another algorithm - return True if successfull
  def mergeProperties(self, alg, mergeProperties=[]) :
    self.setlog()
    success = True
    problemMess = "merge algorithms %s/%s and %s/%s" % (self.getName(), self.getAlias(), alg.getName(), alg.getAlias())
    # Loop over properties
    for k,v in alg.getProperties().items() :
      properties = self.getProperties()
      if k in properties :
        if isinstance(v,list) and k in mergeProperties :
          for item in v :
            if item not in properties[k] :
              self.properties[k].append(item)
              
        #if v != properties[k] and isinstance(v,list):
          ## Merge the lists
          #if k not in mergeProperties:
            #self._log.warning("%s : \n\tDifferent values of property %s: %s vs. %s - lists will be merged." % (problemMess, k, properties[k],v))
          ## Loop over items in the list
          #for item in v :
            #try:
              #ind = properties[k].index(item)
            #except:
              #self.properties[k].append(item)
        
        if v != properties[k] :
          self._log.warning("%s: \n\tDifferent values of property %s:  %s != %s" % (problemMess, k, self.properties[k], v))
          # Problem of merging the value
          algLevel = alg.getLevel()
          level = self.getLevel()
          if not level : level = 'BOTH'
          if not algLevel : algLevel = 'BOTH'
          # Compare levels - priority: 1. BOTH, 2. EF, 3. L2 - same levels - error
          if level != algLevel :
            changeValue = False 
            if algLevel == 'BOTH' :
              changeValue = True
            elif level == 'BOTH' :
              changeValue = False
            elif algLevel == 'EF' :
              changeValue = True
            elif level == 'EF' :
              changeValue = False
            elif algLevel == 'L2' :
              changeValue = True
            elif level == 'L2' :
              changeValue = False
            else :
              self._log.error("Can't %s: \n\tUnknown trigger levels %s, %s" % (problemMess, level, algLevel))
              success = False
              
            if changeValue :
              self._log.debug("Algorithm %s/%s higher priority trigger level %s vs. %s" % (alg.getName(), alg.getAlias(), algLevel, level))
              self.setProperty(k,v)
          else:
            self._log.error("Can't %s: \n\tSame trigger level: %s" % (problemMess, level))
            success = False
      else :
        # Property doesn't exist just set the property
        self.setProperty(k,v)
    return success
  
  def getProperties(self):
    self.setlog()
    if not self.__dict__.has_key('properties'):
      return {}
    return self.properties
  
  def joinProperties(self):
    self.setlog()
    #print 'joinProperties'
    if self.__dict__.has_key('properties'):
      propTojoin = {}
      propertyNames=self.properties.keys()
      propertyNames.sort()
      for propertyName in propertyNames :
        nameFormatCheck=False
        if '__IPC__' in propertyName :
          newPropertyName = propertyName[0:propertyName.find('__IPC__')]
          try :
            index = eval(propertyName[propertyName.find('__IPC__')+len('__IPC__'):].lstrip('0'))
            if isinstance(index,int) :
              nameFormatCheck=True
          except Exception, exc:
            self._log.debug('Property %s : no split format - %s' % (propertyName, exc.message))
            nameFormatCheck=False
            
          if not nameFormatCheck:
            continue
          
          value = self.properties[propertyName]
          del self.properties[propertyName]
          
          self._log.debug('Property %s will be joined to %s' % (propertyName, newPropertyName))
          if self.properties.has_key(newPropertyName):
            self.setProperty(newPropertyName, self.properties[newPropertyName]+value)
          else :
            self.setProperty(newPropertyName, value)
  
  def setParent(self, alg):
    self.setlog()
    self.parent = weakref.ref(alg)()
  
  def getParent(self):
    self.setlog()
    if not self.__dict__.has_key('parent'):
      return None
    return self.parent
  
  def hasParent(self):
    self.setlog()
    output = False
    if self.__dict__.has_key('parent'):
      output = True
    return output
    
  def setLevel(self, level):
    self.setlog()
    if level.upper() in ('L2','EF','BOTH','OL') :
      self.level = level.upper()
      for child in self.getChildren() :
        child.level = level
    else :
      self._log.error('Unsupported trigger level %s' % (level))
        
  def getLevel(self):
    self.setlog()
    output = None
    if self.__dict__.has_key('level') :
      output = self.level
    return output
  
  def formatPropertiesForCfgInit(self, cfg_class=None) :
    properties = []
    for n,v in self.getProperties().items():
      if cfg_class :
        try:
          dflt_value = cfg_class.__slots__[n]
        except KeyError,err:
          self._log.warning("Configurable [%s] has no such property [%s] !!"%( cfg_class.__name__, n))
          continue
      p = '%s = %s'
      if isinstance(v,str) or v == None:
        p = '%s = "%s"'
      properties += [ p % (n,v) ]
    
    propertiesStr = ', '.join(properties)
    return propertiesStr

  def setConfigurableProperties(self, cfg):
    # cfg suppose to be Configurable
    # Properties has to be a dictionary
    output = True
    cfgProperties = cfg.getProperties()
    for property_name, property_value in self.getProperties().items():
      if property_name in cfgProperties.keys() :
        # if not setCfgProperty(cfg,property_name,property_value, self._log) :
        if not self.setConfigurableProperty(cfg,property_name) :
          output = False
          #self._log.warning('Original property: type: %s value: %s' % (type(cfg.getProperties()[property_name]),cfg.getProperties()[property_name]))
          self._log.warning('Property %s.%s not set values: %s vs. %s' % (cfg.getFullJobOptName(), property_name, cfg.getProperties()[property_name], property_value))
      else :
        self._log.warning('%s has no property name: %s' % (cfg.getFullJobOptName(), property_name))
        output = False
    return output
  
  ## Set a single property of the configurable
  #  @param self pointer to the object
  #  @param cfg input configurable
  #  @param property_name name of the property to be set
  def setConfigurableProperty(self, cfg, property_name) :
    output = False
    # Test the existence of the property
    if not cfg.getProperties().has_key(property_name) : return False
    
    # Test the cfg property type
    cfgProperty = cfg.getProperties()[property_name]
    cfgPropertyType = None
    cfgPropertyType = ('%s' % (type(cfgProperty))).replace('<class ','').replace('<type ','').replace("'",'')
    cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
    
    from GaudiKernel.GaudiHandles import PrivateToolHandle, PrivateToolHandleArray
    from AthenaCommon.Configurable import Configurable
    # PrivateToolHandleArray property
    if isinstance(cfgProperty, PrivateToolHandleArray) :
      output = self.setCfgProperty_PrivToolArray(cfg, property_name)
      
    # PrivateToolHandle property
    if not output and isinstance(cfgProperty, (Configurable, PrivateToolHandle)) :
      output = self.setCfgProperty_PrivTool(cfg, property_name)
      
    # All the other types of properties
    if not output :
      return setCfgProperty(cfg,property_name,self.getProperties()[property_name], self._log)
    
    return output
    
  ## Set PrivateToolHandleArray type property of the configurable
  #  @param self pointer to the object
  #  @param cfg input configurable
  #  @param property_name name of the property to be set
  def setCfgProperty_PrivToolArray(self, cfg, property_name) :
    output = True
    # Test the existence of the property
    if not self.getProperties().has_key(property_name): return False
    property_value = self.getProperties()[property_name]
    # Test the type of the property
    if isinstance(property_value, list):
      if not self.hasChildren() : # Set the property in the usual way
        return setCfgProperty(cfg,property_name,property_value, self._log)
      
      self._log.verbose("setCfgProperty_PrivToolArray %s.%s = %s" % (cfg.getFullJobOptName(),property_name,property_value))
      origValue = cfg.getProperties()[property_name]
      self._log.verbose("setCfgProperty_PrivToolArray - existing value: %s" % (origValue.toStringProperty()))
      
      for item in property_value :
        self._log.verbose("setCfgProperty_PrivToolArray: set item %s" % (item))
        childCfg = None
        childAlias = item.split("/")[1].split('.')[-1]
        childName = item.split("/")[0]
        child = self.getChild('.'.join([self.getAlias(),childAlias]))
        if child and child.getName() == childName:
          # Try to get the childcfg
          try :
            childCfg = origValue[childAlias]
          except Exception, exc:
            self._log.verbose("setCfgProperty_PrivToolArray - Exception occured: %s" % (exc.message))
            self._log.verbose("setCfgProperty_PrivToolArray - item %s not existing in current value" % (childAlias))
            childCfg = None
          
          # Check the childCfg
          if childCfg :
            from AthenaCommon.Configurable import Configurable
            if isinstance(childCfg, Configurable) and (childCfg.getName(), childCfg.getType()) == (childAlias, childName):
              self._log.debug('setCfgProperty_PrivToolArray - item %s exists' % (childCfg.getFullJobOptName()))
              child.setConfigurableProperties(childCfg)
              child.setConfigurableChildren(childCfg)
              continue
            elif not isinstance(childCfg, Configurable):
              self._log.debug('setCfgProperty_PrivToolArray - item %s is not Configurable (type %s) - remove' % (item, type(childCfg)))
              # Not a configurable instance - delete only an item from Array
              stmt = 'del cfg.%s[\'%s\']' % (property_name,childAlias)
              try :
                self._log.verbose('Execute: %s' % (stmt))
                exec stmt
              except Exception, exc:
                self._log.debug('setCfgProperty_PrivToolArray - Exception occured: %s' % (exc.message))
              childCfg=None
            elif isinstance(childCfg, Configurable) and not (childCfg.getName(), childCfg.getType()) == (childAlias, childName):
              self._log.debug("setCfgProperty_PrivToolArray - Incompatible type of the child %s - required %s/%s" % (childCfg.getFullJobOptName(), childName, childAlias))
              from TrigConfOffline.HLTConfOffline import removeCfgChild
              removeCfgChild(cfg, childCfg.getName())
              childCfg = None
            else :
              childCfg = None
          
          if not childCfg : # Get the child cfg configurable and append it to the array - do not set the properties
            # Check for existing children - if a child exist - delete it - avoid doubling of the children
            for c in cfg.getAllChildren() :
              if c.getName() == childAlias :
                self._log.debug("setCfgProperty_PrivToolArray - child %s exist within %s - delete" % (c.getFullJobOptName(), cfg.getFullJobOptName()))
                from TrigConfOffline.HLTConfOffline import removeCfgChild
                removeCfgChild(cfg, childAlias)
                break
            # Get the child configurable
            if childCfg == None : childCfg = child.getCfgPyClassWithProperties()
            if childCfg == None : childCfg = child.getCfgPyClass()
            if childCfg == None : childCfg = child.getCfgPyFunc()
            if childCfg == None : childCfg = child.getCfgCppWithProperties()
            if childCfg == None : childCfg = child.getCfgCpp()
            
            if childCfg :
              stmt = "cfg.%s += [childCfg]" % (property_name)
              try :
                self._log.verbose("setCfgProperty_PrivToolArray - Execute: %s" % (stmt))
                exec stmt
                childCfg = cfg.getProperties()[property_name][childCfg.getName()]
              except Exception, exc:
                self._log.debug('setCfgProperty_PrivToolArray - Exception occured: %s' % (exc.message))
              
              # Set the properties and children
              child.setConfigurableProperties(childCfg)
              child.setConfigurableChildren(childCfg)
              continue
        
        # If the child doesn't exist or it has incompatible type or it was not assigned use the string interpretation
        if not childCfg :
          stmt = "cfg.%s += [\'%s\']" % (property_name, item)
          try :
            self._log.verbose("setCfgProperty_PrivToolArray - Execute: %s" % (stmt))
            exec stmt
          except Exception, exc:
            self._log.debug('setCfgProperty_PrivToolArray - Exception occured: %s' % (exc.message))
              
    self._log.verbose("setCfgProperty_PrivToolArray end - %s.%s = %s" % (cfg.getFullJobOptName(),property_name,cfg.getProperties()[property_name].toStringProperty()))
    return compareCfgProperty(cfg, property_name, property_value, self._log)
  
  ## Sets the property of the PrivateToolHandle type
  #  @param self object pointer
  #  @param cfg Configurable
  #  @param property_name Name of the property
  def setCfgProperty_PrivTool(self, cfg, property_name) :
    # Test the existence of the property in the HLTAlgorithms
    if not self.getProperties().has_key(property_name):
      self._log.error("setCfgProperty_PrivTool:\t algorithm %s/%s has no such property " % (self.getName(), self.getAlias(), property_name))
      return False
    
    # Value to be set
    property_value = self.getProperties()[property_name]
    childName = property_value.split("/")[0]
    childAlias = property_value.split("/")[-1].split('.')[-1]
    # Get and check corresponding HLTAlgorithm child
    child = self.getChild('.'.join([self.getAlias(), childAlias]))
    if child and child.getName() !=  childName : child = None
    
    # Original value of the property
    origValue = cfg.getProperties()[property_name]
    
    self._log.verbose("setCfgProperty_PrivTool - %s.%s = %s" % (cfg.getFullJobOptName(), property_name, property_value))
    #self._log.verbose("Mirek: setCfgProperty_PrivTool - Original value: %s.%s = %s" % (cfg.getFullJobOptName(), property_name, origValue))
    #self._log.verbose("Mirek: setCfgProperty_PrivTool - Input Configurable:\n%s" % (cfg))
    
    # Check the original value
    childCfg = None
    from AthenaCommon.Configurable import Configurable
    if isinstance(origValue,Configurable) and (childName, childAlias) == (origValue.getType(), origValue.getName()):
      self._log.verbose("setCfgProperty_PrivTool - PrivateTool %s.%s exist %s" % (cfg.getFullJobOptName(), property_name, origValue.getFullJobOptName()))
      childCfg = origValue
      if child :
        child.setConfigurableProperties(childCfg)
        child.setConfigurableChildren(childCfg)
      return compareCfgProperty(cfg, property_name, property_value)
    elif isinstance(origValue,Configurable) and child :
      self._log.debug('setCfgProperty_PrivTool - property %s is not a Configurable (type %s) - remove' % (propertye_name, type(origValue)))
      # Not a configurable instance set to None
      stmt = 'cfg.%s = None' % (property_name)
      try :
        self._log.verbose('Execute: %s' % (stmt))
        exec stmt
      except Exception, exc:
        self._log.debug('setCfgProperty_PrivTool - Exception occured: %s' % (exc.message))
      childCfg=None
    elif isinstance(origValue, Configurable) and not (origValue.getName(), origValue.getType()) == (childAlias, childName):
      self._log.debug("setCfgProperty_PrivTool - Incompatible type of the child %s - required %s/%s" % (origValue.getFullJobOptName(), childName, childAlias))
      from TrigConfOffline.HLTConfOffline import removeCfgChild
      removeCfgChild(cfg, origValue.getName())
      childCfg = None
    
    # On this place childCfg is None
    if child:
      #self._log.verbose("Mirek: setCfgProperty_PrivTool - child algorithm\n %s" % (child))
      for c in cfg.getAllChildren() :
        if c.getName() == childAlias :
          self._log.debug("setCfgProperty_PrivToolArray - child %s exist within %s - delete" % (c.getFullJobOptName(), cfg.getFullJobOptName()))
          from TrigConfOffline.HLTConfOffline import removeCfgChild
          removeCfgChild(cfg, childAlias)
          break
        
      # Get the child configurable
      if childCfg == None : childCfg = child.getCfgPyClassWithProperties()
      if childCfg == None : childCfg = child.getCfgPyClass()
      if childCfg == None : childCfg = child.getCfgPyFunc()
      if childCfg == None : childCfg = child.getCfgCppWithProperties()
      if childCfg == None : childCfg = child.getCfgCpp()
      
      if childCfg :
        #self._log.verbose("Mirek: setCfgProperty_PrivTool - append Configurable:\n %s" % (childCfg))
        stmt = "cfg.%s = childCfg" % (property_name)
        try :
          self._log.verbose("setCfgProperty_PrivTool - Execute: %s" % (stmt))
          exec stmt
          childCfg = cfg.getProperties()[property_name][childCfg.getName()]
        except Exception, exc:
          self._log.debug('setCfgProperty_PrivTool - Exception occured: %s' % (exc.message))
        # Set the properties and children
        if isinstance(childCfg, Configurable) :
          child.setConfigurableProperties(childCfg)
          child.setConfigurableChildren(childCfg)
          output = compareCfgProperty(cfg, property_name, property_value)
    else :
      return setCfgProperty(cfg,property_name,property_value, self._log)
    
    self._log.verbose("setCfgProperty_PrivTool end - %s.%s = %s" % (cfg.getFullJobOptName(), property_name, cfg.getProperties()[property_name]))
    #self._log.verbose("Mirek: setCfgProperty_PrivTool - Output Configurable:\n%s" % (cfg))
    return output
  
  def setCfgPrivToolPyWithProperties(self, cfg,property_name) :
    self.setlog()
    childCfg = None
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return None
    
    # Try to import the package and the class
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.debug('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return None
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg.%s = %s(name = '%s', %s)" % (property_name, self.py_name, alias, self.formatPropertiesForCfgInit())
    
    # Try the statement
    try :
      self._log.debug('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
      childCfg = cfg.getProperties()[property_name]
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    return childCfg
  
  def setCfgPrivToolPy(self, cfg,property_name) :
    self.setlog()
    childCfg = None
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    
    # Try to import the package and the class
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.debug('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return None
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg.%s = %s(name = '%s')" % (property_name, self.py_name, alias)
    
    # Try the statement
    try :
      self._log.debug('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
      childCfg = cfg.getProperties()[property_name]
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    return childCfg
  
  def setCfgPrivToolPyFunc(self, cfg,property_name) :
    self.setlog()
    childCfg = None
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    
    # Try to import the package and the class
    import_stmt = "from %s import %s" % (self.py_package, self.py_name)
    try:
      self._log.debug('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return False
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg.%s = %s(name=\"%s\")" % (property_name, self.py_name, alias)
    
    # Try the statement
    try :
      self._log.debug('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
      childCfg = cfg.getProperties()[property_name]
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    return childCfg
  
  def setCfgPrivToolCppWithProperties(self, cfg,property_name) :
    childCfg = None
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return None
    
    # Get Configurable Class if exists
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(self.getName(), assumeCxxClass=True)
    
    if not cfg_class :
      self._log.debug('Configurable class: %s not found' % (self.getName()))
      return None
    
    alias = self.getAlias().split('.')[-1]
    from AthenaCommon import CfgMgr
    # Executable statement
    stmt = 'cfg.%(property)s = CfgMgr.%(cfg_class)s(name="%(cfg_name)s", %(properties)s)'
    stmt = stmt % {'property':property_name, 'cfg_class':cfg_class.__name__, 'cfg_name':alias, 'properties':self.formatPropertiesForCfgInit(cfg_class)}
    
    try :
      self._log.debug('Execute %s' % (stmt))
      exec(stmt)
      childCfg = cfg.getProperties()[property_name]
    except Exception, exc:
      self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
      return None
    
    return childCfg
  
  def setCfgPrivToolCpp(self, cfg,property_name) :
    childCfg = None
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return None
    
    # Get Configurable Class if exists
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(self.getName(), assumeCxxClass=True)
    
    if not cfg_class :
      self._log.debug('Configurable class: %s not found' % (self.getName()))
      return None
    
    alias = self.getAlias().split('.')[-1]
    from AthenaCommon import CfgMgr
    # Executable statement
    stmt = 'cfg.%(property)s = CfgMgr.%(cfg_class)s(name="%(cfg_name)s")'
    stmt = stmt % {'property':property_name, 'cfg_class':cfg_class.__name__, 'cfg_name':alias}
    
    try :
      self._log.debug('Execute %s' % (stmt))
      exec(stmt)
      childCfg = cfg.getProperties()[property_name]
    except Exception, exc:
      self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
      return None
    
    return childCfg
    
  def getCfgPyClassWithProperties(self) :
    self.setlog()
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return None
    
    # Try to import the package and the class
    import_success=False
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.debug('Import configurable: %s' % (import_stmt))
      exec import_stmt
      import_success=True
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return None
    
    # Assign the python class to cfg
    cfg = None
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg = %s(name = '%s', %s)" % (self.py_name, alias, self.formatPropertiesForCfgInit())
      
    # Try execute the statement
    try :
      self._log.debug('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    # If everything is fine the configurable exists here
    return cfg
  
  def getCfgPyClass(self) :
    self.setlog()
    
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    
    # Try to import the package and the class
    import_success=False
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.debug('Import configurable: %s' % (import_stmt))
      exec import_stmt
      import_success=True
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return None
    
    #Assign the python class to cfg
    cfg = None
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg = %s(name = '%s')" % (self.py_name, alias)
    
    # Try ro assign the configurable
    try :
      self._log.debug('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    # If everything is fine the configurable exists here
    return cfg
  
  def getCfgPyFunc(self) :
    self.setlog()
    
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    
    # Try to import the package and the function
    import_stmt = "from %s import %s" % (self.py_package, self.py_name)
    try:
      self._log.debug('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    #Assign the python class to cfg
    cfg = None
    alias = self.getAlias().split('.')[-1]
    
    assign_stmt = "cfg = %s(name=\"%s\")" % (self.py_name,alias)
    # Try the statement
    try :
      self._log.debug('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    # If everything is fine the configurable exists here
    return cfg
  
  def getCfgCppWithProperties(self) :
    # Output variable
    cfg = None
    # Test the properties
    if len(self.getProperties()) <= 0 : return None
    # Format name and type of the configurable
    name = self.getAlias()
    cfg_name = self.getAlias().split('.')[-1]
    cfg_type = self.getName()
    
    if name.count('.') > 0:
      cfg_type = cfg_type.split('.')[-1]
      
    # Get the cfg_class
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(cfg_type, assumeCxxClass=True)
    if not cfg_class :
      self._log.debug('Configurable class: %s not found' % (cfg_type))
      # The base C++ class doesn't exist no way to get the configurable
      return None
    
    # Create the executable statement
    stmt = 'cfg = CfgMgr.%(cfg_class)s("%(cfg_name)s", %(properties)s)'
    stmt = stmt % {'cfg_class' : cfg_class.__name__, 'cfg_name'  : cfg_name,
                   'properties' : self.formatPropertiesForCfgInit(cfg_class)}
    
    # Try to create the Configurable
    from AthenaCommon import CfgMgr
    try:
      self._log.debug('Execute %s' % (stmt))
      exec(stmt)
    
    except NameError, exc: # Configurable is most probably a child of some algorithm
      self._log.debug('Configurable: %s/%s not found %s' % (cfg_type, cfg_name, exc.message))
      return None
    
    except TypeError, exc: # Configurable exist under the same name, but different type (derived Configurable?)
      mesg = 'attempt to redefine type of "%s"' % cfg_name
      if exc.message[:len(mesg)] != mesg: return None
      
      self._log.debug(exc.message)
      # Get the old configurable name
      cfg_types = exc.message[len(mesg):].strip().strip('(').strip(')').replace(',','')
      cfg_types = cfg_types.split()
      del cfg_types[2]
      del cfg_types[0]
      existing_cfg_type = cfg_types[cfg_types.index(cfg_class.__name__)-1]
      self._log.debug('Try to get configurable %s/%s' % (existing_cfg_type,cfg_name))
      
      # Get the existing configurable
      from AthenaCommon.Configurable import Configurable
      existing_cfg = Configurable.allConfigurables[cfg_name]
      
      # Exceptions for some configurable names, do not set the properties
      from TrigConfOffline.HLTConfOffline import doNotSetPropertiesCfgName
      if cfg_name in doNotSetPropertiesCfgName :
        return existing_cfg
      
      # Set the properties
      # If there is at least 1 property which can't be set - delete existing configurable and set this one
      propertiesSet = self.setConfigurableProperties(existing_cfg)
      if not propertiesSet:
        self._log.warning('Delete existing configurable %s/%s', (existing_cfg_type,cfg_name))
        from TrigConfOffline.HLTConfOffline import eraseConfigurable
        eraseConfigurable(existing_cfg, self._log)
        del existing_cfg
        # Call itself recursively
        cfg = self.getConfigurable()
      else :
        return existing_cfg
        
    except Exception, exc: # Unknown exception
      self._log.debug(exc.message)
      return None
        
    return cfg
      
  ## Get the confugrable based on C++ information only
  #  @param self Object pointer
  def getCfgCpp(self) :
    # Output variable
    cfg = None
    # Format name and type of the configurable
    name = self.getAlias()
    cfg_name = self.getAlias().split('.')[-1]
    cfg_type = self.getName()
    
    if name.count('.') > 0:
      cfg_type = cfg_type.split('.')[-1]
     
    # Get the cfg_class
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(cfg_type, assumeCxxClass=True)
    if not cfg_class :
      self._log.debug('Configurable class: %s not found' % (cfg_type))
      # The base C++ class doesn't exist no way to get the configurable
      return None
    
    # Create the executable statement
    stmt = 'cfg = CfgMgr.%(cfg_class)s("%(cfg_name)s")'
    stmt = stmt % {'cfg_class' : cfg_class.__name__, 'cfg_name' : cfg_name}
    
    # Try to create the Configurable
    from AthenaCommon import CfgMgr
    try:
      self._log.debug('Execute %s' % (stmt))
      exec(stmt)
    
    except NameError, exc: # Configurable is most probably a child of some algorithm
      self._log.debug('Configurable: %s/%s not found %s' % (cfg_type, cfg_name, exc.message))
      return None
    
    except TypeError, exc: # Configurable exist under the same name, but different type (derived Configurable?)
      mesg = 'attempt to redefine type of "%s"' % cfg_name
      if exc.message[:len(mesg)] != mesg: return None
      
      self._log.debug(exc.message)
      # Get the old configurable name
      cfg_types = exc.message[len(mesg):].strip().strip('(').strip(')').replace(',','')
      cfg_types = cfg_types.split()
      del cfg_types[2]
      del cfg_types[0]
      existing_cfg_type = cfg_types[cfg_types.index(cfg_class.__name__)-1]
      self._log.debug('Try to get configurable %s/%s' % (existing_cfg_type,cfg_name))
      
      # Get the existing configurable
      from AthenaCommon.Configurable import Configurable
      existing_cfg = Configurable.allConfigurables[cfg_name]
      
      # Exceptions for some configurable names, do not set the properties
      from TrigConfOffline.HLTConfOffline import doNotSetPropertiesCfgName
      if cfg_name in doNotSetPropertiesCfgName :
        return existing_cfg
      
      # Set the properties
      # If there is at least 1 property which can't be set - delete existing configurable and set this one
      propertiesSet = self.setConfigurableProperties(existing_cfg)
      if not propertiesSet:
        self._log.warning('Delete existing configurable %s/%s', (existing_cfg_type,cfg_name))
        from TrigConfOffline.HLTConfOffline import eraseConfigurable
        eraseConfigurable(existing_cfg, self._log)
        del existing_cfg
        # Call itself recursively
        cfg = self.getConfigurable()
      else :
        return existing_cfg
        
    except Exception, exc: # Unknown exception
      self._log.debug(exc.message)
      return None
        
    return cfg
  
  ## Documentation fo getConfigurable method
  #  @param self pointer to the object
  #  Accordignly to the information in the class try to create a configurable
  #  @return configurable if unsuccessfull None
  def getConfigurable(self):
    self.setlog()
    cfg = None
    # Look if the cfg doesn't exist yet
    from AthenaCommon.Configurable import Configurable
    if Configurable.allConfigurables.has_key(self.getAlias().replace('ToolSvc.','',1)) :
      cfg = Configurable.allConfigurables[self.getAlias().replace('ToolSvc.','',1)]
      self._log.debug("Configurable %s already exists" % (cfg.getFullJobOptName()))
      # Compare the name of the algorithm
      if cfg.getType() != self.getName():
        self._log.warning("Different type of existing configurable %s requested: %s" % (cfg.getType(), self.getName()))
        from TrigConfOffline.HLTConfOffline import doNotSetPropertiesCfgName
        if self.getAlias().split('.')[-1] not in doNotSetPropertiesCfgName :
          from TrigConfOffline.HLTConfOffline import eraseConfigurable
          eraseConfigurable(cfg)
          cfg = None
      
    # 1. try to get cfg as a python class with properties
    if cfg == None : cfg = self.getCfgPyClassWithProperties()
    # 2. try to get cfg as a python class without properties
    if cfg == None : cfg = self.getCfgPyClass()
    # 3. try to get cfg as a python function
    if cfg == None : cfg = self.getCfgPyFunc()
    # 4. try to get cfg as a C++ class with properties
    if cfg == None : cfg = self.getCfgCppWithProperties()
    # 5. try to get cfg as a C++ class without properties
    if cfg == None : cfg = self.getCfgCpp()
    
    # If the Configurable doesn't exists here - there is no known way how to get it
    if cfg == None :
      self._log.error("Can not create a configurable for: %s" % (self))
      return None
    
    # Before setting the Properties - fix the children
    if len(cfg.getAllChildren()) > 0 : self.fixCfgChildCompatibility(cfg)
    
    # Configurable exists - set the properties
    self.setConfigurableProperties(cfg)
    
    # Set the children
    self.setConfigurableChildren(cfg)
    
    # Append to ToolSvc
    #if self.getAlias().startswith('ToolSvc.') :
      #from AthenaCommon.AppMgr import ToolSvc
      #ToolSvc += cfg
          
    return cfg
  
  ## Fix the children algorithms within the HLTAlgorithm
  #  @param self Object pointer
  #  Check the children aliases compatibility with the parent name
  #  Try to fix child parent relations
  #  Called recursively for the children too
  def fixChildren(self) :
    ## Fixing of children within algorithm ##
    self.setlog()
    alias = self.getAlias()
    if self.hasChildren() :
      i = 0
      while i < len(self._children) :
        child = self.getChildren()[i]
        childAlias = child.getAlias()
        aliases = childAlias.replace('%s.' % (alias),'',1).split('.')
        # All fine the child alias is correct
        if len(aliases) == 1 :
          i += 1
          continue
        
        # Check whether fixable
        if childAlias.find('%s.' % (alias)) != 0 :
          self._log.warning('Child alias %s does not match parent alias - can not be fixed' % (childAlias))
          i += 1
          continue
        
        self._log.warning('Child alias %s does not match parent alias - another child algorithm should be a parent' % (childAlias))
        parentAlias = '%s.%s' % (alias, aliases[0])
        parent = self.getChild(parentAlias)
        if parent :
          self._log.info('Parent algorithm %s/%s found - move %s/%s' % (parent.getName(), parent.getAlias(), child.getName(), childAlias))
          parent.appendChild(child)
          self.removeChild(childAlias)
        else :
          self._log.warning('Parent algorithm for %s not found' % (childAlias))
          i += 1
    
      # Call the function recursively
      for child in self.getChildren() : child.fixChildren()
  
  ## Check the compatibility of cfg children and HLTAlgorithm
  #  @param self pointer to the object
  #  @param cfg configurable
  #  @return True if compatible
  #  Check existing children from configurable with the HLTAlgorithm children
  #  Compatibility is checked based on the name and type resp. existence of the property referring to a child
  #  At 1st incompatible child returns False
  def checkCfgChildCompatibility(self, cfg):
    output = True
    # Dictionairy with children
    algChildrenDict = {}
    for child in self.getChildren() :
      algChildrenDict[child.getAlias().split('.')[-1]] = child.getName()
    
    # Check the children of the configurable
    for child in cfg.getAllChildren() :
      if child.getName() in algChildrenDict.keys():
        if child.getType() != algChildrenDict[child.getName()] :
          self._log.warning("Conflicting type of child algorithm %s found - required type %s" % (child.getFullJobOptName(), algChildrenDict[child.getName()]))
          return False
      else :
        privateToolMatch = False
        cfgChildName = child.getName()
        cfgChildType = child.getType()
        # Look through HLTAlgorithm properties
        for k,v in self.getProperties().items() :
          # PrivateToolHandle
          if isinstance(v,(str,unicode)) and (v.split('/')[0], v.split('/')[-1]) == (cfgChildType, cfgChildName) : 
            privateToolMatch = True
            break
          # PrivateToolHandle Array
          if isinstance(v,list) and (cfgChildName in v or '/'.join([cfgChildType, cfgChildName]) in v) :
            privateToolMatch = True
            break
        
        if not privateToolMatch :
          self._log.warning("Unwanted child algorithm %s found" % (child.getFullJobOptName()))
          return False
    
    return output
  
  ## Fix the compatibility of cfg children and HLTAlgorithm
  #  @param self pointer to the object
  #  @param cfg configurable
  #  
  #  Remove the existing children from configurable uncompatible with the HLTAlgorithm children
  #  Compatibility is checked based on the name and type resp. existence of the property refering to child
  def fixCfgChildCompatibility(self, cfg):
    algChildrenDict = {}
    for child in self.getChildren() :
      algChildrenDict[child.getAlias().split('.')[-1]] = child.getName()
    
    from TrigConfOffline.HLTConfOffline import removeCfgChild
    for child in cfg.getAllChildren() :
      if child.getName() in algChildrenDict.keys():
        if child.getType() != algChildrenDict[child.getName()] :
          self._log.debug("Remove child algorithm %s conflicting type - required type %s" % (child.getFullJobOptName(), algChildrenDict[child.getName()]))
          removeCfgChild(cfg,child.getName())
      else :
        privateToolMatch = False
        cfgChildName = child.getName()
        cfgChildType = child.getType()
        # Look through HLTAlgorithm properties
        for k,v in self.getProperties().items() :
          # PrivateToolHandle
          if isinstance(v,(str,unicode)) and (v.split('/')[0], v.split('/')[-1]) == (cfgChildType, cfgChildName) : 
            privateToolMatch = True
            break
          # PrivateToolHandle Array
          if isinstance(v,list) and (cfgChildName in v or '/'.join([cfgChildType, cfgChildName]) in v) :
            privateToolMatch = True
            break
        
        if not privateToolMatch :
          self._log.debug("Remove unwanted child algorithm %s" % (child.getFullJobOptName()))
          removeCfgChild(cfg,child.getName())
  
  ## Documentation for the method setConfigurableChildren
  #  @param self The Object pointer
  #  @param cfg input configurable to append the children of the object as private tools
  def setConfigurableChildren(self, cfg) :
    self.setlog()
    nchildren = 0
    
    # Fix the children
    if not self.checkCfgChildCompatibility(cfg) and self.getName() not in ('ToolSvc','ServiceManager') : 
      self.fixCfgChildCompatibility(cfg)
      
    # Test if any children exists
    if not self.hasChildren() :
      return nchildren
    
    self._log.verbose('Set children of %s/%s to %s' % (self.getName(), self.getAlias(), cfg))
    # Loop over children and set them
    for child in self.getChildren() :
      childCfg = child.setConfigurableChild(cfg, nchildren)
      
      if childCfg :
        self._log.debug('Child %s appended to parent %s' % (childCfg.getFullJobOptName(),cfg.getFullJobOptName()))
        self._log.verbose('Child Algorithm vs. Configurable:\n %s \nvs\n %s' % (child, childCfg))
        nchildren += nchildren
        
    return nchildren
  
  ## Documentation for the method setConfigurableChild
  #  @param self The Object pointer
  #  @param cfg input configurable to append self as private tool
  #  @return Child configurable pointer
  def setConfigurableChild(self, cfg, nchildren=0) :
    self.setlog()
    
    childCfg = None
    # Look for the existing children
    for c in cfg.getAllChildren() :
      if c.getName() == self.getAlias().split('.')[-1] :
        if c.getType() == self.getName() :
          childCfg = c
          break
        else :
          self._log.debug("Conflicting type of child algorithm %s found - required type %s" % (c.getFullJobOptName(), self.getName()))
          from TrigConfOffline.HLTConfOffline import removeCfgChild
          # Remove the conflicting child
          removeCfgChild(cfg, c.getName())
    
    if childCfg == None : childCfg = self.setCfgChildPyWithProperties(cfg)
    if childCfg == None : childCfg = self.setCfgChildPy(cfg)
    if childCfg == None : childCfg = self.setCfgChildPyFunc(cfg)
    if childCfg == None : childCfg = self.setCfgChildCppWithProperties(cfg)
    if childCfg == None : childCfg = self.setCfgChildCpp(cfg)
    
    if not childCfg : 
      self._log.error("%s/%s could not be assign as child of %s" % (self.getName(),self.getAlias(),cfg.getFullJobOptName()))
      return childCfg
    
    # Check for multiple children
    nchild = 0
    childAlias = self.getAlias().split('.')[-1]
    for c in cfg.getAllChildren() :
      if c.getName() == childAlias :
        nchild += 1
    
    # If multiple children exists - delete them
    if nchild > 1 :
      self._log.warning('Multiple %d children %s/%s found' % (nchildren, self.getName(), self.getAlias()))
      # Deleting the child should remove only one
      if cfg.getProperties().has_key(childAlias) :
        self._log.warning('Can not delete a child %s/%s identical property exist' % (self.getName(), self.getAlias()))
        stmt = 'childCfg = cfg.%s' % (childAlias)
        self._log.verbose('Execute: %s' % (stmt))
        exec (stmt)
      else :
        stmt = 'del cfg.%s' % (childAlias)
        self._log.verbose('Execute: %s' % (stmt))
        exec (stmt)
      # To be certain assign right childCfg
      for c in cfg.getAllChildren() :
        if c.getName() == childAlias :
          childCfg = c
          break
      
    # Fix the childCfg children if needed
    if not self.checkCfgChildCompatibility(childCfg) : self.fixCfgChildCompatibility(childCfg)
    
    # Set properties of the child before assigning the children
    self.setConfigurableProperties(childCfg)
    
    # Set the child configurable children if needed
    if self.hasChildren(): nchildren += self.setConfigurableChildren(childCfg)
    
    return childCfg
  
  def loadFromDB(self, cursor, dbName=''):
    self.setlog()
    if self.dbId > 0:
      # Test if db type is an oracle
      oracleFlag ='%s' % (cursor.description[0][1])
      oracleFlag = oracleFlag.lower()
      oracleFlag = 'oracle' in oracleFlag
      
      sqliteFlag = '%s' % (cursor)
      sqliteFlag = sqliteFlag.lower()
      sqliteFlag = 'sqlite' in sqliteFlag
      
      prepend=''
      if dbName :
        prepend = '%s.' % (dbName)
        
      query  = ' SELECT %sHLT_COMPONENT.HCP_NAME, ' % (prepend)
      query += ' %sHLT_COMPONENT.HCP_ALIAS, ' % (prepend)
      query += ' %sHLT_COMPONENT.HCP_TYPE, ' % (prepend)
      query += ' %sHLT_COMPONENT.HCP_PY_NAME, ' % (prepend)
      query += ' %sHLT_COMPONENT.HCP_PY_PACKAGE ' % (prepend)
      query += ' FROM %sHLT_COMPONENT ' % (prepend)
      if sqliteFlag :
        query += ' WHERE %sHLT_COMPONENT.HCP_ID=%d ' % (prepend, self.dbId)
      else :
        query += ' WHERE %sHLT_COMPONENT.HCP_ID=:hcp_id ' % (prepend)
      
      self._log.verbose('%s, hcp_id=%d' % (query,self.dbId))
      if sqliteFlag :
        cursor.execute(query)
      else :
        cursor.execute(query, hcp_id=self.dbId)
        
      result = cursor.fetchall()
      for column in result:
        values = []
        # Format DB output for oracle
        for val in column :
          #if oracleFlag and val == '~' :
          if val == '~' :
            values.append('')
          else :
            values.append(val)
        
        self.name       = str(values[0])
        self.alias      = str(values[1])
        self.alg_type   = str(values[2])
        self.py_name   = str(values[3])
        self.py_package = str(values[4])
      
      nProperties = self.loadPropertiesFromDB(cursor, dbName)
      self.joinProperties()
      nChildren = self.loadChildrenFromDB(cursor, dbName)
      self._log.debug('Succesfull load of: \n%s' % (self))
    else:
      self._log.warning('Invalid component dbId= %d' % (self.dbId))
  
  def loadPropertiesFromDB(self, cursor, dbName=''):
    self.setlog()
    nProperties = 0
    if self.dbId > 0:
      # Test if db type is an oracle
      oracleFlag ='%s' % (cursor.description[0][1])
      oracleFlag = oracleFlag.lower()
      oracleFlag = 'oracle' in oracleFlag
      
      sqliteFlag = '%s' % (cursor)
      sqliteFlag = sqliteFlag.lower()
      sqliteFlag = 'sqlite' in sqliteFlag

      prepend=''
      if dbName :
        prepend = '%s.' % (dbName)
      
      query  = ' SELECT %sHLT_PARAMETER.HPA_NAME, ' % (prepend)
      query += ' %sHLT_PARAMETER.HPA_VALUE ' % (prepend)
      query += ' FROM %sHLT_CP_TO_PA ' % (prepend)
      query += ' JOIN %sHLT_PARAMETER ON  (%sHLT_CP_TO_PA.HCP2PA_PARAMETER_ID = %sHLT_PARAMETER.HPA_ID) ' % (prepend, prepend, prepend)
      if sqliteFlag :
        query += ' WHERE %sHLT_CP_TO_PA.HCP2PA_COMPONENT_ID=%d ' % (prepend, self.dbId)
      else :
        query += ' WHERE %sHLT_CP_TO_PA.HCP2PA_COMPONENT_ID=:hcp_id ' % (prepend)
      query += ' ORDER BY %sHLT_PARAMETER.HPA_NAME ' % (prepend)
      
      self._log.verbose('%s, hcp_id=%d' % (query,self.dbId))
      if sqliteFlag :
        cursor.execute(query)
      else:
        cursor.execute(query, hcp_id=self.dbId)
      result = cursor.fetchall()
      nProperties=len(result)
      for column in result:
        name = str(column[0])
        value = str(column[1])
        
        setProperty=True
        # Exception for properties with values "" or ''
        if value =='""' or value =='\'\'' :
          setProperty=False
        
        # Oracle may not contain empty value thus we set it in the DB to ~
        #if oracleFlag and value=='~':
        if value=='~':
          value=''
        
        if setProperty :
          self.setProperty(name, value)
    
    else:
      self._log.warning('Invalid component dbId= %d' % (self.dbId))
      
    return nProperties
    
  def loadChildrenFromDB(self, cursor, dbName=''):
    self.setlog()
    nChildren=0
    if self.dbId > 0:
      prepend=''
      if dbName :
        prepend = '%s.' % (dbName)
      
      sqliteFlag = '%s' % (cursor)
      sqliteFlag = sqliteFlag.lower()
      sqliteFlag = 'sqlite' in sqliteFlag
      
      query  = ' SELECT %sHLT_CP_TO_CP.HCP2CP_CHILD_COMP_ID ' % (prepend)
      query += ' FROM %sHLT_CP_TO_CP ' % (prepend)
      if sqliteFlag :
        query += ' WHERE %sHLT_CP_TO_CP.HCP2CP_PARENT_COMP_ID=%d ' % (prepend, self.dbId)
      else :
        query += ' WHERE %sHLT_CP_TO_CP.HCP2CP_PARENT_COMP_ID=:hcp_id ' % (prepend)
      
      self._log.verbose('%s, hcp_id=%d' % (query,self.dbId))
      if sqliteFlag :
        cursor.execute(query)
      else :
        cursor.execute(query, hcp_id=self.dbId)
      result = cursor.fetchall()
      nChildren = len(result)
      for column in result:
        childAlgorithm=HLTAlgorithm(dbId=column[0],OutputLevel=self.OutputLevel)
        childAlgorithm.loadFromDB(cursor, dbName)
        self.setChild(childAlgorithm)
        
    else:
      self._log.warning('Invalid component dbId= %d' % (self.dbId))
    return nChildren

  def loadFromXMLNode(self, node):
    self.setlog()
    alias=""
    if node.hasAttribute('alias') :
      self.alias = str(node.getAttribute('alias'))
    else:
      self._log.error('Component has no attribute alias %s' % (node.toxml()))
      return False
    name=alias
    if node.hasAttribute('name') :
      self.name = str(node.getAttribute('name'))
      
    typeList = ('TopAlg','Algorithm','SteerAlg','Service','PublicTool','PrivateTool','Auditor','Type')
    self.alg_type = typeList[7]
    if node.hasAttribute('aud') :
      if int(node.getAttribute('aud')) > 0 : self.alg_type = typeList[6]
    if node.hasAttribute('privT') :
      if int(node.getAttribute('privT')) > 0 : self.alg_type = typeList[5]
    if node.hasAttribute('pubT') :
      if int(node.getAttribute('pubT')) > 0 : self.alg_type = typeList[4]
    if node.hasAttribute('svc') :
      if int(node.getAttribute('svc')) > 0 : self.alg_type = typeList[3]
    if node.hasAttribute('steeralg') :
      if int(node.getAttribute('steeralg')) > 0 : self.alg_type = typeList[2]
    if node.hasAttribute('alg') :
      if int(node.getAttribute('alg')) > 0 : self.alg_type = typeList[1]
    if node.hasAttribute('topalg') :
      if int(node.getAttribute('topalg')) > 0 : self.alg_type = typeList[0]
      
    if node.hasChildNodes() :
      doc_pars = node.getElementsByTagName('parameter')
      if len(doc_pars) > 0:
        for p in doc_pars :
          self.loadParameterfromXML(p)
      
      doc_children = node.getElementsByTagName('component')
      if len(doc_children) > 0:
        for child in doc_children :
          child = HLTAlgorithm(OutputLevel=self.OutputLevel)
          child.loadFromXML(child)
          self.setChild(child)
    return True
        
  def loadParameterfromXML(self, node):
    self.setlog()
    if node.hasAttribute('name') :
      name = str(node.getAttribute('name'))
    else:
      self._log.error('Parameter has no attribute name %s' % (node.toxml()))
      return False
    
    if node.hasAttribute('value') :
      value = str(node.getAttribute('value'))
      try:
        value = eval(value)
      except :
        # Nothing happened just pass the value as string
        pass
    
    self.setProperty(name,value)
    return True
    
  def deepcopy(self, copyChildren=False) :
    self.setlog()
    alg = HLTAlgorithm()
    for k,v in self.__dict__.items() :
      if k=='_children':
        if copyChildren :
          for child in self.getChildren() :
            alg.setChild(child)
      elif k=='properties' :
        for kprop, vprop in self.properties.items():
          alg.setProperty(kprop,vprop)
      elif k=='_log':
        continue
      else :
        alg.__setattr__(k,v)
    return alg
      
  def flattenAlg(self):
    self.setlog()
    alglist = []
    alglist.append(self)
    for ch in self.getChildren() :
      alglist.extend(ch.flattenAlg())
    return alglist
  
  def convertToOld(self):
    self.setlog()
    alg={}
    alg['alias']=self.getAlias()
    alg['name']=self.getName()
    alg['properties']={}
    for k,v in self.getProperties().items() :
      alg['properties'][k]=v
    
    if self.__dict__.has_key('topAlg') :
      alg['topalg'] = self.topAlg

    return alg
    
  def setCfgChildPyWithProperties(self, parentCfg):
    self.setlog()
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return None
    
    # Test whether the parent configurable doesn't already have a child with the same name
    childCfg = None
    attributes = {"alias": self.getAlias().split('.')[-1],
                  "name": self.getName().split('.')[-1],
                  "py_name": self.py_name,
                  "py_package": self.py_package}
    childAttributes = {"alias": None, "name": None, "py_name": None, "py_package": None}
    for cCfg in parentCfg.getAllChildren() :
      py_class = str(cCfg.__class__)[6:].strip('\' >').split('.')[-1]
      py_package = str(cCfg.__module__)
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1],
                        "py_name": py_class,
                        "py_package": py_package}
                        
      if cCfgAttributes['alias'] == attributes['alias']:
        childCfg = cCfg
        childAttributes = cCfgAttributes
        if cCfgAttributes == attributes : # 100% match - can't be better - return the configurable
          return childCfg
          
    # Child exists but it does not agree with the setting - delete it
    if childCfg :
      from TrigConfOffline.HLTConfOffline import eraseConfigurable
      eraseConfigurable(childCfg, self._log)
      del childCfg
      if parentCfg.getProperties().has_key(childAttributes['alias']) :
        # self._log.warning('Can not delete a child %s/%s identical property exist' % (child.getName(), child.getAlias()))
        stmt = 'parentCfg.%s = None' % (childAttributes['alias'])
        self._log.verbose('Execute: %s' % (stmt))
        exec (stmt)
    
    # Try to import the package and the class
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.verbose('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return None
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "parentCfg += %s(name = '%s', %s)" % (self.py_name, alias, self.formatPropertiesForCfgInit())
    if parentCfg.getProperties().has_key(alias) :
      cfgPropertyType = ('%s' % (type(cfg.getProperties()[childAlias]))).replace('<class ','').replace('<type ','').replace("'",'')
      cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
      if cfgPropertyType in ('GaudiKernel.GaudiHandles.ServiceHandle',
      'GaudiKernel.GaudiHandles.PublicToolHandle','GaudiKernel.GaudiHandles.PrivateToolHandle') :
        assign_stmt = "parentCfg.%s = %s(name = '%s', %s)" % (alias, self.py_name, alias, self.formatPropertiesForCfgInit())
    # Try the statement
    try :
      self._log.verbose('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    # Search the children of the parent for the child
    cfg = None
    for cCfg in parentCfg.getAllChildren() :
      py_class = str(cCfg.__class__)[6:].strip('\' >').split('.')[-1]
      py_package = str(cCfg.__module__)
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1],
                        "py_name": py_class,
                        "py_package": py_package}
      if cCfgAttributes == attributes:
        cfg = cCfg
    
    return cfg


  def setCfgChildPy(self, parentCfg):
    self.setlog()
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    
    # Test whether the parent configurable doesn't already have a child with the same name
    childCfg = None
    attributes = {"alias": self.getAlias().split('.')[-1],
                  "name": self.getName().split('.')[-1],
                  "py_name": self.py_name,
                  "py_package": self.py_package}
    childAttributes = {"alias": None, "name": None, "py_name": None, "py_package": None}
    for cCfg in parentCfg.getAllChildren() :
      py_class = str(cCfg.__class__)[6:].strip('\' >').split('.')[-1]
      py_package = str(cCfg.__module__)
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1],
                        "py_name": py_class,
                        "py_package": py_package}
                        
      if cCfgAttributes['alias'] == attributes['alias']:
        childCfg = cCfg
        childAttributes = cCfgAttributes
        if cCfgAttributes == attributes : # 100% match - can't be better - return the configurable
          return childCfg
          
    # Child exists but it does not agree with the setting - delete it
    if childCfg :
      from TrigConfOffline.HLTConfOffline import eraseConfigurable
      eraseConfigurable(childCfg, self._log)
      del childCfg
      if parentCfg.getProperties().has_key(childAttributes['alias']) :
        # self._log.warning('Can not delete a child %s/%s identical property exist' % (child.getName(), child.getAlias()))
        stmt = 'parentCfg.%s = None' % (childAttributes['alias'])
        self._log.verbose('Execute: %s' % (stmt))
        exec (stmt)
    
    # Try to import the package and the class
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.verbose('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return None
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "parentCfg += %s(name = '%s')" % (self.py_name, alias)
    if parentCfg.getProperties().has_key(alias) :
      cfgPropertyType = ('%s' % (type(cfg.getProperties()[childAlias]))).replace('<class ','').replace('<type ','').replace("'",'')
      cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
      if cfgPropertyType in ('GaudiKernel.GaudiHandles.ServiceHandle',
      'GaudiKernel.GaudiHandles.PublicToolHandle','GaudiKernel.GaudiHandles.PrivateToolHandle') :
        assign_stmt = "parentCfg.%s = %s(name = '%s')" % (alias, self.py_name, alias)
    # Try the statement
    try :
      self._log.verbose('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    # Search the children of the parent for the child
    cfg = None
    for cCfg in parentCfg.getAllChildren() :
      py_class = str(cCfg.__class__)[6:].strip('\' >').split('.')[-1]
      py_package = str(cCfg.__module__)
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1],
                        "py_name": py_class,
                        "py_package": py_package}
      if cCfgAttributes == attributes:
        cfg = cCfg
    
    return cfg

  def setCfgChildPyFunc(self, parentCfg):
    self.setlog()
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return None
    
    # Test whether the parent configurable doesn't already have a child with the same name
    childCfg = None
    attributes = {"alias": self.getAlias().split('.')[-1],
                  "name": self.getName().split('.')[-1],
                  "py_name": self.py_name,
                  "py_package": self.py_package}
    childAttributes = {"alias": None, "name": None, "py_name": None, "py_package": None}
    for cCfg in parentCfg.getAllChildren() :
      py_class = str(cCfg.__class__)[6:].strip('\' >').split('.')[-1]
      py_package = str(cCfg.__module__)
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1],
                        "py_name": py_class,
                        "py_package": py_package}
                        
      if cCfgAttributes['alias'] == attributes['alias']:
        childCfg = cCfg
        childAttributes = cCfgAttributes
        if cCfgAttributes == attributes : # 100% match - can't be better - return the configurable
          return childCfg
          
    # Child exists but it does not agree with the setting - delete it
    if childCfg :
      from TrigConfOffline.HLTConfOffline import eraseConfigurable
      eraseConfigurable(childCfg, self._log)
      del childCfg
      if parentCfg.getProperties().has_key(childAttributes['alias']) :
        # self._log.warning('Can not delete a child %s/%s identical property exist' % (child.getName(), child.getAlias()))
        stmt = 'parentCfg.%s = None' % (childAttributes['alias'])
        self._log.verbose('Execute: %s' % (stmt))
        exec (stmt)
    
    # Try to import the package and the class
    import_stmt = "from %s import %s" % (self.py_package, self.py_name)
    try:
      self._log.verbose('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return None
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "parentCfg += %s(name=\"%s\")" % (self.py_name,alias)
    if parentCfg.getProperties().has_key(alias) :
      cfgPropertyType = ('%s' % (type(parentCfg.getProperties()[alias]))).replace('<class ','').replace('<type ','').replace("'",'')
      cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
      if cfgPropertyType in ('GaudiKernel.GaudiHandles.ServiceHandle',
      'GaudiKernel.GaudiHandles.PublicToolHandle','GaudiKernel.GaudiHandles.PrivateToolHandle') :
        assign_stmt = "parentCfg.%s = %s(name=\"%s\")" % (alias, self.py_name, alias)
    # Try the statement
    try :
      self._log.verbose('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return None
    
    # Search the children of the parent for the child
    cfg = None
    for cCfg in parentCfg.getAllChildren() :
      py_class = str(cCfg.__class__)[6:].strip('\' >').split('.')[-1]
      py_package = str(cCfg.__module__)
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1],
                        "py_name": py_class,
                        "py_package": py_package}
      if cCfgAttributes == attributes:
        cfg = cCfg
    
    return cfg

  def setCfgChildCppWithProperties(self, parentCfg):
    self.setlog()
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return None
    
    # Test whether the parent configurable doesn't already have a child with the same name
    childCfg = None
    attributes = {"alias": self.getAlias().split(',')[-1], "name": self.getName().split(',')[-1]}
    childAttributes = {"alias": None, "name": None}
    for cCfg in parentCfg.getAllChildren() :
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1], "name": cCfg.getType().split('.')[-1]}
                        
      if cCfgAttributes['alias'] == attributes['alias']:
        childCfg = cCfg
        childAttributes = cCfgAttributes
        if cCfgAttributes == attributes : # 100% match - can't be better - return the configurable
          return childCfg
    
    # Child exists - type of the child differs - test if it is not a derived configurable
    if childCfg :
      # Try to set the properties - if succesfull return the childCfg
      propertiesSet = self.setConfigurableProperties(childCfg)
      if not propertiesSet:
        self._log.warning('Delete existing configurable %s/%s', (existing_cfg_type,cfg_name))
        from TrigConfOffline.HLTConfOffline import eraseConfigurable
        eraseConfigurable(childCfg, self._log)
        del childCfg
        if parentCfg.getProperties().has_key(childAttributes['alias']) :
          stmt = 'parentCfg.%s = None' % (childAttributes['alias'])
          self._log.verbose('Execute: %s' % (stmt))
        exec (stmt)
      else:
        return childCfg
    
    # Get Configurable Class if exists
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(self.getName(), assumeCxxClass=True)
    
    if not cfg_class :
      self._log.warning('Configurable class: %s not found' % (self.getName()))
      return None
    
    alias = self.getAlias().split('.')[-1]
    from AthenaCommon import CfgMgr
    # Executable statement
    stmt = 'parentCfg += CfgMgr.%(cfg_class)s(name="%(cfg_name)s", %(properties)s)'
    stmt = stmt % {'cfg_class' : cfg_class.__name__, 'cfg_name'  : alias, 'properties' : self.formatPropertiesForCfgInit(cfg_class)}
      
    if parentCfg.getProperties().has_key(alias) :
      cfgPropertyType = ('%s' % (type(parentCfg.getProperties()[alias]))).replace('<class ','').replace('<type ','').replace("'",'')
      cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
      
      if cfgPropertyType in ('GaudiKernel.GaudiHandles.ServiceHandle',
      'GaudiKernel.GaudiHandles.PublicToolHandle','GaudiKernel.GaudiHandles.PrivateToolHandle') :
        
        stmt = 'parentCfg .%(property_name)s = CfgMgr.%(cfg_class)s(name="%(cfg_name)s", %(properties)s)'
        stmt = stmt % {'property_name' : alias, 'cfg_class'  : cfg_class.__name__, 'cfg_name'  : alias,
          'properties' : self.formatPropertiesForCfgInit(cfg_class)}

    try :
      self._log.verbose('Execute %s' % (stmt))
      exec(stmt)
    except TypeError, exc:
      # TypeError: attempt to redefine type of "Time" (was: TrigTimeHistTool, new: TrigTimeHistToolConfig)
      if exc.message.startswith('attempt to redefine type of "%s"' % (alias)):
        from AthenaCommon.Configurable import Configurable
        existingCfg = Configurable.allConfigurables[alias]
        # Check the type of the existing configurable - if OK use the python information
        if existingCfg.getType() == self.getName() :
          self.py_name = str(existingCfg.__class__)[6:].strip('\' >').split('.')[-1]
          self.py_package = str(existingCfg.__module__)
          childCfg = None
          if childCfg == None : childCfg = self.setCfgChildPyWithProperties(parentCfg)
          if childCfg == None : childCfg = self.setCfgChildPy(parentCfg)
          if childCfg == None : childCfg = self.setCfgChildPyFunc(parentCfg)
          if childCfg : return childCfg
        else:
          self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
    except Exception, exc:
      self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
    
    # Search the children of the parent for the child
    cfg = None
    for cCfg in parentCfg.getAllChildren() :
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1]}
      if cCfgAttributes == attributes:
        cfg = cCfg
    
    return cfg

  def setCfgChildCpp(self, parentCfg):
    self.setlog()
    
    # Test whether the parent configurable doesn't already have a child with the same name
    childCfg = None
    attributes = {"alias": self.getAlias().split('.')[-1], "name": self.getName().split('.')[-1]}
    childAttributes = {"alias": None, "name": None}
    for cCfg in parentCfg.getAllChildren() :
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1], "name": cCfg.getType().split('.')[-1]}
                        
      if cCfgAttributes['alias'] == attributes['alias']:
        childCfg = cCfg
        childAttributes = cCfgAttributes
        if cCfgAttributes == attributes : # 100% match - can't be better - return the configurable
          return childCfg
    
    # Child exists - type of the child differs - test if it is not a derived configurable
    if childCfg :
      # Try to set the properties - if succesfull return the childCfg
      propertiesSet = self.setConfigurableProperties(childCfg)
      if not propertiesSet:
        self._log.warning('Delete existing configurable %s/%s', (existing_cfg_type,cfg_name))
        from TrigConfOffline.HLTConfOffline import eraseConfigurable
        eraseConfigurable(childCfg, self._log)
        del childCfg
        if parentCfg.getProperties().has_key(childAttributes['alias']) :
          stmt = 'parentCfg.%s = None' % (childAttributes['alias'])
          self._log.verbose('Execute: %s' % (stmt))
        exec (stmt)
      else:
        return childCfg
    
    # Get Configurable Class if exists
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(self.getName(), assumeCxxClass=True)
    
    if not cfg_class :
      self._log.warning('Configurable class: %s not found' % (self.getName()))
      return None
    
    alias = self.getAlias().split('.')[-1]
    from AthenaCommon import CfgMgr
    # Executable statement
    stmt = 'parentCfg += CfgMgr.%(cfg_class)s(name="%(cfg_name)s")'
    stmt = stmt % {'cfg_class' : cfg_class.__name__, 'cfg_name'  : alias}
      
    if parentCfg.getProperties().has_key(alias) :
      cfgPropertyType = ('%s' % (type(parentCfg.getProperties()[alias]))).replace('<class ','').replace('<type ','').replace("'",'')
      cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
      
      if cfgPropertyType in ('GaudiKernel.GaudiHandles.ServiceHandle',
      'GaudiKernel.GaudiHandles.PublicToolHandle','GaudiKernel.GaudiHandles.PrivateToolHandle') :
        
        stmt = 'parentCfg .%(property_name)s = CfgMgr.%(cfg_class)s(name="%(cfg_name)s")'
        stmt = stmt % {'property_name' : alias, 'cfg_class'  : cfg_class.__name__, 'cfg_name'  : alias}

    try :
      self._log.verbose('Execute %s' % (stmt))
      exec(stmt)
    except TypeError, exc:
      # TypeError: attempt to redefine type of "Time" (was: TrigTimeHistTool, new: TrigTimeHistToolConfig)
      if exc.message.startswith('attempt to redefine type of "%s"' % (alias)):
        from AthenaCommon.Configurable import Configurable
        existingCfg = Configurable.allConfigurables[alias]
        # Check the type of the existing configurable - if OK use the python information
        if existingCfg.getType() == self.getName() :
          self.py_name = str(existingCfg.__class__)[6:].strip('\' >').split('.')[-1]
          self.py_package = str(existingCfg.__module__)
          childCfg = None
          if childCfg == None : childCfg = self.setCfgChildPyWithProperties(parentCfg)
          if childCfg == None : childCfg = self.setCfgChildPy(parentCfg)
          if childCfg == None : childCfg = self.setCfgChildPyFunc(parentCfg)
          if childCfg : return childCfg
        else:
          self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
    except Exception, exc:
      self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
    
    # Search the children of the parent for the child
    cfg = None
    for cCfg in parentCfg.getAllChildren() :
      cCfgAttributes = {"alias": cCfg.getName().split('.')[-1],
                        "name": cCfg.getType().split('.')[-1]}
      if cCfgAttributes == attributes:
        cfg = cCfg
    
    return cfg
  
  # Sets the child of the cfg by appending it to the PrivateToolHandleArray
  # Returns True in case of success
  def appendToPrivToolArray(self, cfg, property) :
    alias = self.getAlias().split('.')[-1]
    childCfg = None
    # Test if the child doesn't exists yet
    attributes = {"alias": alias,
                  "name": self.getName(),
                  "py_name": self.py_name,
                  "py_package": self.py_package}
    childAttributes = {"alias": None, "name": None, "py_name": None, "py_package": None}
    for child in cfg.getAllChildren() :
      py_class = str(child.__class__)[6:].strip('\' >').split('.')[-1]
      py_package = str(child.__module__)
      cAttributes = {"alias": child.getName().split('.')[-1],
                        "name": child.getType().split('.')[-1],
                        "py_name": py_class,
                        "py_package": py_package}
                        
      if cAttributes['alias'] == attributes['alias']:
        childCfg = child
        childAttributes = cAttributes
        if childAttributes == attributes : # 100% match - can't be better - return the configurable
          break
    
    # Child exists but it does not agree with the setting - delete it
    if childCfg  and childAttributes != attributes:
      from TrigConfOffline.HLTConfOffline import eraseConfigurable
      eraseConfigurable(childCfg, self._log)
      del childCfg

    # Append child to the PrivateToolArray
    if childCfg :
      matchingCfg = False
      # Check if the child is not already in the PrivateToolHandleArray
      for child in cfg.getProperties()[property] :
        cJobOptName = None
        try:
          cJobOptName = child.getFullJobOptName()
        except:
          pass
        if cJobOptName == childCfg.getFullJobOptName() : 
            matchingCfg = True
            break
      if not matchingCfg :
        stmt = "cfg.%s += [childCfg]" % (property)
        try:
          exec stmt
        except:   # Sometimes an exception occurs because same child 
          pass
        
    if childCfg :
      success = self.setConfigurableProperties(childCfg)
      return childCfg
    
    # The childCfg is not existing yet - thus needs to be appended
    success = False
    if not success : success = self.appendToPrivToolArrayPyWithProperties(cfg,property)
    if not success : success = self.appendToPrivToolArrayPy(cfg,property)
    if not success : success = self.appendToPrivToolArrayPyFunc(cfg,property)
    if not success : success = self.appendToPrivToolArrayCppWithProperties(cfg,property)
    if not success : success = self.appendToPrivToolArrayCpp(cfg,property)
    
    # Test if the child was assigned
    for child in cfg.getProperties()[property] :
      if child.getName() == alias :
        childCfg = child
        break
      
    # Set the properties of the child
    if childCfg : self.setConfigurableProperties(childCfg)
    
    return childCfg
  
  # Python Class with properties style
  def appendToPrivToolArrayPyWithProperties(self, cfg, property) :
    output = False
    self.setlog()
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return False
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return False
    
    # Try to import the package and the class
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.verbose('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return False
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg.%s += [%s(name = '%s', %s)]" % (property, self.py_name, alias, self.formatPropertiesForCfgInit())
    
    # Try the statement
    try :
      self._log.verbose('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
      output = True
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return False
    
    return output

  # Python Class style
  def appendToPrivToolArrayPy(self, cfg, property) :
    output = False
    self.setlog()
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return False
    
    # Try to import the package and the class
    import_stmt = "from %s.%s import %s" % (self.py_package, self.py_name, self.py_name)
    try:
      self._log.verbose('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return False
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg.%s += [%s(name = '%s')]" % (property, self.py_name, alias)
    
    # Try the statement
    try :
      self._log.verbose('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
      output = True
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return False
    
    return output
  
  # Python Func style
  def appendToPrivToolArrayPyFunc(self, cfg, property) :
    output = False
    self.setlog()
    # 1st try if there is a python package and python class and they have some value
    if not (self.__dict__.has_key('py_name') and 
        self.__dict__.has_key('py_package') and 
        self.py_name != '' and 
        self.py_package != ''):
      return False
    
    # Try to import the package and the class
    import_stmt = "from %s import %s" % (self.py_package, self.py_name)
    try:
      self._log.verbose('Import configurable: %s' % (import_stmt))
      exec import_stmt
    except Exception, exc:
      self._log.debug('Exception occured: %s' % (exc.message))
      return False
    
    # Assign the python class to cfg
    alias = self.getAlias().split('.')[-1]
    assign_stmt = "cfg.%s += [%s(name=\"%s\")]" % (property, self.py_name, alias)
    
    # Try the statement
    try :
      self._log.verbose('Assign configurable: %s' % (assign_stmt))
      exec assign_stmt
      output = True
    except Exception, exc:
      self._log.debug('%s' % (exc.message))
      return False
    
    return output
  
  # Cpp with properties style
  def appendToPrivToolArrayCppWithProperties(self, cfg, property) :
    self.setlog()
    output = False
    # Test if the properties exists
    if len(self.getProperties()) <= 0 :
      return False
    
    # Get Configurable Class if exists
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(self.getName(), assumeCxxClass=True)
    
    if not cfg_class :
      self._log.warning('Configurable class: %s not found' % (self.getName()))
      return False
    
    alias = self.getAlias().split('.')[-1]
    from AthenaCommon import CfgMgr
    # Executable statement
    stmt = 'cfg.%(property)s += [CfgMgr.%(cfg_class)s(name="%(cfg_name)s", %(properties)s)]'
    stmt = stmt % {'property':property, 'cfg_class':cfg_class.__name__, 'cfg_name':alias, 'properties':self.formatPropertiesForCfgInit(cfg_class)}
    
    try :
      self._log.verbose('Execute %s' % (stmt))
      exec(stmt)
      output = True
    except Exception, exc:
      self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
    return output
  
  # Cpp style
  def appendToPrivToolArrayCpp(self, cfg, property) :
    self.setlog()
    output = False
    
    # Get Configurable Class if exists
    from AthenaCommon.ConfigurableDb import getConfigurable
    cfg_class = getConfigurable(self.getName(), assumeCxxClass=True)
    
    if not cfg_class :
      self._log.warning('Configurable class: %s not found' % (self.getName()))
      return False
    
    alias = self.getAlias().split('.')[-1]
    from AthenaCommon import CfgMgr
    # Executable statement
    stmt = 'cfg.%(property)s += [CfgMgr.%(cfg_class)s(name="%(cfg_name)s")]'
    stmt = stmt % {'property':property, 'cfg_class':cfg_class.__name__, 'cfg_name':alias}
    
    try :
      self._log.verbose('Execute %s' % (stmt))
      exec(stmt)
      output = True
    except Exception, exc:
      self._log.debug('Child  %s/%s not set: %s' % (self.getName(), self.getAlias(), exc.message))
    
    return output

# Functions which doesn't need to be class method - but used in the class
def setCfgProperty(cfg, property_name, property_value, logger=None):
  ## Try to set the configurable property - if successfull return True
  # Return value
  output = False
  # Set logger if needed
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.setCfgProperty')
    #import logging
    #logger=logging.getLogger('compareCfgProperty')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
  
  #logger.verbose("Set: %s.%s = %s" % (cfg.getFullJobOptName(), property_name, property_value))
  
  # Test original value for a type etc.
  origValue = None
  if property_name in cfg.getProperties().keys() :
    origValue = cfg.getProperties()[property_name]
  
  if compareCfgProperty(cfg, property_name, property_value) :
    #logger.verbose("No need to set the property - original and new value are equal")
    return True
  
  # 1st attempt - try the brute force
  if not output : output = setCfgPropertyNoPreFormat(cfg, property_name, property_value, logger)
  # 2nd - try the string format
  if not output : output = setCfgPropertyStrFormat(cfg, property_name, property_value, logger)
  # 3rd - try the list format
  if not output : output = setCfgPropertyListFormat(cfg, property_name, property_value, logger)
  # 4th - try the Gaudi format
  if not output : output = setCfgPropertyGaudiHandleFormat(cfg, property_name, property_value, logger)

  return output

# Set the property without preformating the value
def setCfgPropertyNoPreFormat(cfg, property_name, property_value, logger=None):
  ## Brute force set of the property
  # Set logger if needed
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.setCfgPropertyNoPreFormat')
    #import logging
    #logger=logging.getLogger('compareCfgProperty')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
  
  #logger.verbose("Set: %s.%s = %s" % (cfg.getFullJobOptName(), property_name, property_value))
  
  stmt = "cfg.%s = %s" % (property_name,property_value)
  # Format the setting of type string
  if isinstance(property_value, str) :
    stmt = "cfg.%s = '%s'" % (property_name,property_value)
  
  try :
    logger.verbose(stmt)
    exec(stmt)
  except Exception, exc:
    logger.debug('%s.%s = %s can not be set: %s' % (cfg.getFullName(), property_name, property_value,  exc.message))
    return False
  
  return compareCfgProperty(cfg, property_name, property_value)

# Set the property preformating the value to string
def setCfgPropertyStrFormat(cfg, property_name, property_value, logger=None):
  ## Brute force set of the property
  # Set logger if needed
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.setCfgPropertyStrFormat')
    #import logging
    #logger=logging.getLogger('compareCfgProperty')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
  
  logger.verbose("Set: %s.%s = \'%s\'" % (cfg.getFullJobOptName(), property_name, str(property_value)))
  
  stmt = "cfg.%s = '%s'" % (property_name,str(property_value))
  try :
    logger.verbose(stmt)
    exec(stmt)
  except Exception, exc:
    logger.debug('%s.%s = \'%s\' can not be set: %s' % (cfg.getFullName(), property_name, property_value,  exc.message))
    return False
  
  return compareCfgProperty(cfg, property_name, property_value)

# Set the property preformating the value for List type
def setCfgPropertyListFormat(cfg, property_name, property_value, logger=None):
  ## Brute force set of the property
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.setCfgPropertyListFormat')
    #import logging
    #logger=logging.getLogger('compareCfgProperty')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
  
  logger.verbose("Set: %s.%s = \'%s\'" % (cfg.getFullJobOptName(), property_name, str(property_value)))
  
  origValue = None
  if property_name in cfg.getProperties().keys():
    origValue = cfg.getProperties()[property_name]
  
  # Check the type of the original value:
  if not isinstance(origValue,list): return False
  
  # Erase the original list:
  stmt = "cfg.%s = %s" % (property_name,[])
  try:
    logger.verbose("Erase original list: %s" %(stmt))
    exec(stmt)
  except Exception, exc:
    logger.debug('Can not erase original list %s.%s: %s' % (cfg.getFullName(), property_name, exc.message))
    return False
  
  # Try to set value at once:
  stmt = "cfg.%s = %s" % (property_name,property_value)
  try :
    logger.verbose(stmt)
    exec(stmt)
    # Compare the value - if equal return True
    if compareCfgProperty(cfg, property_name, property_value) : return True
  except Exception, exc:
    logger.debug('%s.%s = %s can not be set: %s' % (cfg.getFullName(), property_name, property_value,  exc.message))
  
  # Value can not be set at once - will sequentially
  # 1st erase the mess left after previous actions:
  stmt = "cfg.%s = %s" % (property_name,[])
  logger.verbose("Erase the list: %s" %(stmt))
  exec(stmt)
  
  # Go item by item and append it:
  for item in property_value :
    stmt = "cfg.%s += [%s]" % (property_name, item)
    if isinstance(item, str) :
      stmt = "cfg.%s += [\'%s\']" % (property_name, item)
    
    try :
      logger.verbose(stmt)
      exec(stmt)
    except Exception, exc:
      logger.debug('%s.%s = %s can not be set: %s' % (cfg.getFullName(), property_name, property_value,  exc.message))
      return False
     
  # Compare the value and return the result of the comparison
  return compareCfgProperty(cfg, property_name, property_value)

# Set the property preformating the value to string
def setCfgPropertyGaudiHandleFormat(cfg, property_name, property_value, logger=None):
  ## Brute force set of the property
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.setCfgPropertyGaudiHandleFormat')
    #import logging
    #logger=logging.getLogger('compareCfgProperty')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
  
  logger.verbose("Set: %s.%s = %s" % (cfg.getFullJobOptName(), property_name, property_value))
  
  cfgPropertyType = None
  if property_name in cfg.getProperties().keys() :
    cfgPropertyType = ('%s' % (type(cfg.getProperties()[property_name]))).replace('<class ','').replace('<type ','').replace("'",'')
    cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
  
  if cfgPropertyType not in ( 'GaudiKernel.GaudiHandles.ServiceHandleArray',
                              'GaudiKernel.GaudiHandles.ServiceHandle',
                              'GaudiKernel.GaudiHandles.PrivateToolHandleArray',
                              'GaudiKernel.GaudiHandles.PrivateToolHandle',
                              'GaudiKernel.GaudiHandles.PublicToolHandleArray',
                              'GaudiKernel.GaudiHandles.PublicToolHandle') :
    return False
  
  # Import needed Gaudi classes
  from GaudiKernel.GaudiHandles import PublicToolHandle,PublicToolHandleArray,PrivateToolHandle,PrivateToolHandleArray,ServiceHandle,ServiceHandleArray
  
  # Compose the executable statement
  stmt = ''
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.PublicToolHandle' :
    stmt = "cfg.%s = PublicToolHandle('%s')" % (property_name,property_value)
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.PublicToolHandleArray' :
    stmt = "cfg.%s = PublicToolHandleArray(%s)" % (property_name,property_value)
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandle' :
    stmt = "cfg.%s = PrivateToolHandle('%s')" % (property_name,property_value)
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandleArray' :
    stmt = "cfg.%s = PrivateToolHandleArray(%s)" % (property_name,property_value)
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.ServiceHandle' :
    stmt = "cfg.%s = ServiceHandle('%s')" % (property_name,property_value)
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.ServiceHandleArray' :
    stmt = "cfg.%s = ServiceHandleArray('%s')" % (property_name,property_value)
  
  try :
    logger.verbose(stmt)
    exec(stmt)
  except Exception, exc:
    logger.debug('%s.%s = %s can not be set: %s' % (cfg.getFullName(), property_name, property_value,  exc.message))
    return False
  
  return compareCfgProperty(cfg, property_name, property_value)
  
def compareCfgProperty(cfg, property_name, property_value, logger=None):
  ## compare cfg property with the value
  
  # Set logger if needed
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.compareCfgProperty')
    #import logging
    #logger=logging.getLogger('compareCfgProperty')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
    

  
  # Get the original value if exists
  origValue = None
  if property_name in cfg.getProperties().keys() :
    origValue = cfg.getProperties()[property_name]
  else :
    logger.debug("Property: %s.%s doesn't exist" % (cfg.getFullJobOptName(), property_name))
    return False
  

  #logger.verbose("Compare: %s.%s: %s vs. %s" % (cfg.getFullJobOptName(), property_name, origValue , property_value))
  
  # Normal comparison
  if origValue == property_value : return True
  # Empty values comparison
  if origValue == '<no value>' and (property_value=='' or property_value==None) : return True
  
  # Special type properties comparison
  cfgPropertyType = ('%s' % (type(origValue))).replace('<class ','').replace('<type ','').replace("'",'')
  cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
  
  # Reformat the value of the cfg property if GaudiHandleArray
  if cfgPropertyType in ( 'GaudiKernel.GaudiHandles.ServiceHandleArray',
                          'GaudiKernel.GaudiHandles.PublicToolHandleArray') :
    origValue = str(origValue).split()[1:]
    if str(origValue) == str(property_value) : return True
  
  # Compare the PrivateToolHandle property
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandle':
    try:
      origValue = '/'.join([origValue.getType(), origValue.getName()])
    except :
      pass
    if str(origValue) == str(property_value) : return True
  
  # Compare the PrivateToolHandleArray property
  if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandleArray':
    valueToCompare = origValue.toStringProperty()
    try :
      valueToCompare = eval(valueToCompare)
    except :
      pass
    #for item in origValue :
      #try :
        #valueToCompare.append('/'.join([item.getType(),item.getName()]))
      #except :
        #valueToCompare.append(item)
    valueToCompare.sort()
    property_value.sort()
    #logger.verbose("Mirek: compareCfgProperty %s vs. %s" % (property_value, valueToCompare))
    origValue = valueToCompare
    if str(origValue) == str(property_value) : return True
  
  # On this place the cfg value and property_value differs
  # Only a debugging think
  if type({}) == type(origValue) and type({}) == type(property_value) and str(origValue) != str(property_value):
    logMessage = ''
    for k,v in origValue.items() :
      if property_value.has_key(k) :
        if property_value[k] != v :
          logMessage += "\tValue difference key %s: %s vs. %s\n" % (k, v , property_value[k])
      else :
        logMessage += "\nKey %s not in the property_value\n" % (k)
    
    for k,v in property_value.items() :
      if not origValue.has_key(k) :
        logMessage += "\nKey %s not in the cfg property\n" % (k)
    
    if logMessage : logger.debug(logMessage)
      
  # Just in case
  if str(origValue) == str(property_value) : return True
  
  return False


## Compare 2 HLT algorithms and print out the differences
#  @param alg1 Algorithm 1
#  @param alg2 Algorithm 2
#  @param pattern Bit patttern what should be compared
#  @param prepend Prepend string for print out
#  @return string - formatted for pretty print out
def diffHLTAlgorithms(alg1, alg2, pattern=0xffff, prepend='') :
  if prepend=='' : prepend = '\t'
  output = ""
  # Compare Alias
  if  ((0x1 << 0) & pattern) : 
    if(alg1.getAlias()!=alg2.getAlias()) :
      output += '%s| Alias differs:\t%s vs. %s\n' % (prepend, alg1.getAlias(), alg2.getAlias())
  
  # Compare Name
  if  ((0x1 << 1) & pattern) : 
    if(alg1.getName()!=alg2.getName()) :
      output += '%s| Name differs:\t%s vs. %s\n' % (prepend, alg1.getName(), alg2.getName())
  
  # Compare Python Name
  if  ((0x1 << 2) & pattern) : 
    if(alg1.getPy_name()!=alg2.getPy_name()) :
      output += '%s| Python name differs:\t%s vs. %s\n' % (prepend, alg1.getPy_name(), alg2.getPy_name())
  
  # Compare Python Package
  if  ((0x1 << 3) & pattern) : 
    if(alg1.getPy_package()!=alg2.getPy_package()) :
      output += '%s| Python package differs:\t%s vs. %s\n' % (prepend, alg1.getPy_package(), alg2.getPy_package())
  
  # Compare Python Package
  if  ((0x1 << 4) & pattern) : 
    if(alg1.getType()!=alg2.getType()) :
      output += '%s| Type differs:\t%s vs. %s\n' % (prepend, alg1.getType(), alg2.getType())
      
  # Compare Properties
  if ((0x1 << 8) & pattern) and alg1.getProperties() != alg2.getProperties():
    output += '%s| Properties differ:\n' % (prepend)
    prop1 = alg1.getProperties()
    prop2 = alg2.getProperties()
            
    # Find missing properties alg2
    for key in prop1.keys() :
      if key not in prop2.keys() :
        output += '%s| 2: %s%s/%s: has no property %s\n' % (prepend, prepend, alg2.getName(), alg2.getAlias(), key)
    
    # Find missing properties alg1
    for key in prop2.keys() :
      if key not in prop1.keys() :
        output += '%s| 1: %s%s/%s: has no property %s\n' % (prepend, prepend, alg1.getName(), alg1.getAlias(), key)

    # Compare values
    for key in prop1.keys():
      if key != 'OutputLevel' and key in prop2.keys() and prop1[key] != prop2[key] :
        output += '%s| %s%s: %s vs. %s\n' % (prepend, prepend, key, prop1[key], prop2[key])
    
  # Compare child algorithms
  if ((0x1 << 12) & pattern):
    # Get missing children of alg2
    for child in alg1.getChildren() :
      if not alg2.getChild(child.getAlias()) :
        output += "%s| 2: %s/%s has no child %s/%s\n" % (prepend,alg2.getName(), alg2.getAlias(), child.getName(), child.getAlias())
    
    # Get missing children of alg1
    for child in alg2.getChildren() :
      if not alg1.getChild(child.getAlias()) :
        output += "%s| 1: %s/%s has no child %s/%s\n" % (prepend,alg1.getName(), alg1.getAlias(), child.getName(), child.getAlias())
    
    # Compare the children:
    for child1 in alg1.getChildren() :
      child2 = alg2.getChild(child1.getAlias())
      if child2 :
        output += diffHLTAlgorithms(child1, child2, pattern, prepend+prepend[0])
  
  if output!='' : 
    head = '%s/%s 1: %s/%s vs. 2: %s/%s %s\n' % (prepend,'='*30, alg1.getName(), alg1.getAlias(), alg2.getName(), alg2.getAlias(), '='*30)
    end = '%s\%s 1: %s/%s vs. 2: %s/%s %s\n' % (prepend,'='*30, alg1.getName(), alg1.getAlias(), alg2.getName(), alg2.getAlias(), '='*30)
    output = head + output + end
    
  return output





















