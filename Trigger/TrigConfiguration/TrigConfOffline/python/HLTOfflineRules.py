###########################################################################
#    
#    <nozicka@mail.desy.de>
#
# Description:
#     Set of rules needed for the transformation from online to offline setup
#     Every Rule has an apply method with an input with dictionary or array of HLT Algorithms
#
#     Copy Rule:
#          Data: source algorithm with maximum 1 parameter, target algorithm with maximum 1 parameter
#          The value of source parameter is copied to the target
#
#     Replace Rule:
#          Data: online algorithms, offline algorithms
#          Online algorithms are replaced by the offline algorithms
#
#     Merge Rule:
#          Data: algorithm with one or more properties
#          Online properties of an online algorithm will be merged
#
#     Rename Rule:
#          Data: 1 online algorithm (max. 1 parameter), 1 offline algorithm
#          Online algorithm resp. its parameter will be renamed to offline
#
#     Modify Rule:
#          Data: 1 online algorithm with 1 parameter, 1 offline algorithm with 1 offline parameter
#          Value of the online parameter will be replaced by the offline value
#          Value must be of type: list
#
#     Sort Rule:
#          Data: 1 algorithm, sort_order
#          Values (must be of type: list) will be put at the start/end dependend on the sort_order of the list
#
###########################################################################

import TrigConfOffline.HLTAlgorithm as HLTAlgorithm
import re, os

class HLTOfflineRuleConsistencyError(Exception):
  """
  Exception thrown by the rules in case of inconsistency
  """
  pass

## CopyRule class
#  The copy rules applies to the list of HLTAlgorithms
class CopyRule(object):
  ## The constructor of the class. 
  #  @param self The object pointer
  #  @param kwargs arguments 
  def __init__(self, **kwargs):
    self.__initLogger__()
    ## @var sourceAlg 
    #  Online algorithm with maximum 1 property it's value will be copied to the offline algorithm
    self.__dict__['sourceAlg'] = None
    ## @var targetAlg
    #  Offline algorithm with maximum 1 property will be set to value of the online algorithm property
    self.__dict__['targetAlg'] = None
    ## @var nactive 
    #  Counter of algorithms modified by the rule
    self.__dict__['nactive'] = 0
    
    if kwargs.has_key('online') and isinstance(kwargs['online'],HLTRuleAlgorithm):
      self.sourceAlg = kwargs['online']
      
    if kwargs.has_key('offline') and isinstance(kwargs['offline'],HLTRuleAlgorithm):
      self.targetAlg = kwargs['offline']
  
  ## Conversion of the rule for pretty print
  def __str__(self):
    """ Convert the Copy Rule to string for print out."""
    output = "Copy Rule:"
    algStr = "%s" % (self.sourceAlg)
    output += "\n\tSource Algorithm:"
    output += "\t\t" + algStr.replace("\n","\n\t\t")    
    output += "\n\tTarget Algorithm:" 
    algStr = "%s" % (self.targetAlg)
    output += "\t\t" + algStr.replace("\n","\n\t\t")
    return output
  
  ## Initialize logger of the class
  def __initLogger__(self) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTOfflineRules.CopyRule')
    if self.__dict__.has_key('OutputLevel') :
      self._log.setLevel(self.OutputLevel)
    else :
      self.__dict__['OutputLevel'] = 3
    
  ## Set attribute of the class
  def __setattr__(self, name, value) :
    self._log.verbose('set attribute %s=%s' % (name, value))
    if name not in self.__dict__.keys():
      self.warning('Can not set %s=%s - unsupported attribute' % (name, value))
      return
      
    # Setting of the source/target algorithms - check the type 
    if name in ('sourceAlg', 'targetAlg') :
      if isinstance(value, HLTRuleAlgorithm) :
        self.__dict__[name] = value
      else:
        self._log.error('Unsupported type %s of the %s' % (type(value),name))

    # Set the OutputLevel and the logger level
    if name=='OutputLevel' :
      self.__dict__[name]= value
      self._log.setLevel(value)
    
  ## Apply Method
  #  @param self The object pointer
  #  @param algs Input list of HLTAlgorithms
  #  @return Number of modified algorithms
  def apply(self, algs, OutputLevel=1):
    """ Apply the Copy Rule on the list of algorithms:
      Find matching online and offline algorithms in the list - algs
      Apply the rule if matching algorithms found and return the number of modified algorithms. 
    """
    messg = '\n******************************************\n'
    messg += '||         Copy Rule: Start               ||\n'
    messg += '******************************************\n'
    self._log.verbose(messg)
    self._log.verbose(self)
    messgEnd  = '\n******************************************\n'
    messgEnd += '||         Copy Rule: End                 ||\n'
    messgEnd += '******************************************\n'
    nmod = 0
    # Test type of algs
    if not isinstance(algs, list) :
      self._log.warning('Unsupported input type %s - rule not applied' % (type(algs)))
      self._log.verbose(messgEnd)
      return 0
    
    # Keep the found value in this variable
    value = None
    nvalues = 0
    # Search the input algorithm list for the source algorithm value
    for parentAlg in algs :
      for alg in parentAlg.flattenAlg() :
        if self.sourceAlg.match(alg) : # Match the online algorithm
          for propName, propValue in alg.getProperties().items() :
            if self.sourceAlg.properties[0].match(propName,propValue) : 
              if nvalues<=0:
                value = propValue
                nvalues += 1
              elif value!=propValue:
                self._log.error('Multiple inconsistent properties in input algorithm list - rule can not be applied')
                self._log.debug('Existing value %s vs. %s/%s.%s= %s' % (value, alg.name, alg.alias, propName, propValue))
                self._log.verbose(messgEnd)
                return 0
    
    # Test if a source value exists
    if nvalues<=0:
      self._log.verbose("No source found %s" % (self.sourceAlg))
      self._log.verbose(messgEnd)
      return 0
    
    # Search the input algorithm list for the target algorithm value
    for parentAlg in algs :
      for alg in parentAlg.flattenAlg() :
        if self.targetAlg.match(alg) : # Match the offline algorithm
          for propName, propValue in alg.getProperties().items() :
            if self.targetAlg.properties[0].match(propName,propValue) :
              self._log.verbose('Original value: %s/%s.%s= %s' % (alg.name, alg.alias, propName, alg.getProperties()[propName]))
              alg.setProperty(propName, value)
              self._log.verbose('Modified value: %s/%s.%s= %s' % (alg.name, alg.alias, propName, alg.getProperties()[propName]))
              nmod += 1
              self.nactive += 1
    
    self._log.verbose(messgEnd)
    return nmod

class ReplaceRule(object):
  """Replace Rule Class:
    Replace the online algorithms by the offline algorithms
  """
  ## List of online algorithms
  _onlineAlgs =  []
  ## List of offline algorithms
  _offlineAlgs = []
  ## Counter of algorithms modified by the rule
  _nactive = 0
  
  ## The constructor of the class. 
  #  @param self The object pointer
  #  @param args arguments 
  def __init__(self,*args):
    if len(args) ==1 :
      if isinstance(args[0],dict):
        if args[0].has_key('online') :
          self._onlineAlgs = args[0]['online']
        else :
          raise HLTOfflineRuleConsistencyError('Replace Rule: No online component found')
        
        if args[0].has_key('offline') :
          self._offlineAlgs = args[0]['offline']
        else :
          raise HLTOfflineRuleConsistencyError('Replace Rule: No offline component found')
        
  def __str__(self):
    """ Convert the Replace Rule to string for print out."""
    output = "Replace Rule:\n"
    output += "\tOnline Algorithms:\n"
    for a in  self._onlineAlgs :
      output += "\t\t%s/%s\n" % (a['name'], a['alias'])
      if a.has_key('properties') :
        for p,v in a['properties'].items() :
          output += "\t\t\t%s = \'%s\'\n" % (p,v)
    output+= "\tOffline Algorithms:\n"
    for a in  self._offlineAlgs :
      output += "\t\t%s/%s\n" % (a['name'], a['alias'])
      if a.has_key('properties') :
        for p,v in a['properties'].items() :
          output += "\t\t\t%s = \'%s\'\n" % (p,v)
    return output
    
  ## Append an offline algorithm to the algorithm list
  #  @param self The object pointer
  #  @param offlineAlg Offline algorithm
  #  @param algs Algorithm list
  #  @param logger Logger
  def _appendOfflineAlg(self, offlineAlg, algs, logger) :
    nmod = 0 # Number of modified algs
    inputAlgJobOptNames = getAlgJobOptNames(algs)
    joboptname = "%s/%s" % (offlineAlg['name'],offlineAlg['alias'])
    if joboptname in inputAlgJobOptNames :
      ind = 0
      while ind < len(inputAlgJobOptNames) :
        try :
          ind += inputAlgJobOptNames[ind:].index(joboptName)
          mesg =  "\n____________________________\n"
          mesg += "Algorithm before application: %d\n" % (ind)
          mesg += "----------------------------\n"
          mesg += alg2str(algs[ind])
          logger.debug(mesg)
          for offpropname in offlineAlg['properties'] :
            algs[ind]['properties'][offpropname] = offlineAlg['properties'][offpropname]
            
          mesg =  "\n____________________________\n"
          mesg += "Algorithm after application: %d\n" % (ind)
          mesg += "----------------------------\n"
          mesg += alg2str(algs[ind])
          logger.debug(mesg)
          nmod += 1
          ind +=1
        except :
          ind = len(inputAlgJobOptNames)
    else:
      mesg =  "\n____________________________\n"
      mesg += "Append Offline Algorithm:   \n"
      mesg += "----------------------------\n"
      mesg += alg2str(offlineAlg)
      logger.debug(mesg)
      algs.append(offlineAlg)
      nmod += 1
    return nmod

  ## Apply the rule to the list of algorithms
  #  @param self The object pointer
  #  @param algs List of algorithms
  #  @param OutputLevel Logger OutputLevel
  #  @return Number of modified algorithms
  def apply(self, algs, OutputLevel=3) :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTOfflineRule.Replace')
    logger.setLevel(OutputLevel)
    # Counter of modified algorithms
    nmod = 0
    nmodprop = 0
    
    if isinstance(algs, list) :
      messg =  '\n==========================================\n'
      messg += '||      Replace Rule: Start             ||\n'
      messg += '==========================================\n'
      logger.verbose(messg)
      logger.debug(self)
      
      # Execute the recursive application of the rule
      nmod = self.applyRecursive(algs,logger)
    
      messg = '\n==========================================\n'
      messg += '||      Replace Rule: %d End               ||\n' % (nmod)
      messg += '==========================================\n'
      logger.verbose(messg)
      
    self._nactive += nmod
    return nmod
  
  ## Apply the rule recursively to the list of algorithms
  #  @param self The object pointer
  #  @param algs List of algorithms
  #  @param logger Logger
  #  @return Number of modified algorithms
  def applyRecursive(self,algs,logger) :
    # Counter of modified algorithms
    nmod = 0
    # Counter of modified properties
    nmodprop = 0
    
    if isinstance(algs, list) and len(algs)>0:
      nmatchingOnlineAlgs = 0
      for onlineAlg in self._onlineAlgs :
        for alg in algs :
          matchingAlg = matchString(onlineAlg['name'], alg.name) and matchString(onlineAlg['alias'], alg.alias)
          if matchingAlg and onlineAlg.has_key('py_name'):
            matchingAlg = matchString(onlineAlg['py_name'], alg.getPy_name())
          if matchingAlg and onlineAlg.has_key('py_package'):
            matchingAlg = matchString(onlineAlg['py_package'], alg.getPy_package())
          if matchingAlg:
            nmatchingOnlineAlgs += 1
            # Break the loop over parent algorithms - no need to continue the counter has been increased
            break
          
      # Apply only if all online algorithms have been found in the list
      if nmatchingOnlineAlgs == len(self._onlineAlgs) and nmatchingOnlineAlgs > 0 :
        # Set the parent algorithm if exists - the input algorithms must have either no parent or a common parent algorithm
        parentAlgorithm = None
        if algs[0].hasParent() :
          parentAlgorithm = algs[0].getParent()
          
        # Found matching offline Algorithms - the not found will be attached to the list of algorithms
        matchingOfflineAlgs = {}
        # Process the parent algorithms first
        # Loop over parent algorithms for selection of matching algs
        i = 0
        while i < len(algs) :
          # Flag that the algorithm was modified
          algModified = False
          algDeleted = False
          # Select matching online parent algorithms:
          for onlineAlg in self._onlineAlgs :
            matchingAlg = matchString(onlineAlg['name'], algs[i].name) and matchString(onlineAlg['alias'], algs[i].alias)
            if matchingAlg and onlineAlg.has_key('py_name'):
              matchingAlg = matchString(onlineAlg['py_name'], algs[i].getPy_name())
            if matchingAlg and onlineAlg.has_key('py_package'):
              matchingAlg = matchString(onlineAlg['py_package'], algs[i].getPy_package())
            if matchingAlg:
              logger.verbose("Matching online Algorithm found:\n%s" % (algs[i]))
              firstLog = True
              # Search for matching offline algorithm
              matchOffline = None
              for offlineAlg in self._offlineAlgs :
                if matchString(offlineAlg['alias'], algs[i].getAlias()) :
                  # Match the rest of the algorithm items
                  matchingAlg = matchString(offlineAlg['name'], algs[i].name)
                  if matchingAlg and offlineAlg.has_key('py_name'):
                    matchingAlg = matchString(offlineAlg['py_name'], algs[i].getPy_name())
                  if matchingAlg and offlineAlg.has_key('py_package'):
                    matchingAlg = matchString(offlineAlg['py_package'], algs[i].getPy_package())
                  if matchingAlg:
                    matchOffline = offlineAlg
                  elif matchOffline == None :
                    matchOffline = offlineAlg
              
              # If no matching algorithm found - delete the online algorithm
              if matchOffline == None :
                messg = "\n____________________________\n"
                messg += "Algorithm will be deleted: %d\n" % (i)
                messg += "----------------------------\n"
                messg += '%s' % (algs[i])
                logger.debug(messg)
                algs[i].delete()
                algDeleted = True
                del algs[i]
                nmod += 1
                continue
              else : # At least the alias of the algorithm matches
                # If the alias matches only - change the name of the algorithm
                matchingAlg = matchString(matchOffline['name'], algs[i].name)
                if matchingAlg and matchOffline.has_key('py_name'):
                  matchingAlg = matchString(matchOffline['py_name'], algs[i].getPy_name())
                if matchingAlg and matchOffline.has_key('py_package'):
                  matchingAlg = matchString(matchOffline['py_package'], algs[i].getPy_package())
                if not matchingAlg:
                  if not algModified :
                    messg = "\n____________________________\n"
                    messg += "Algorithm before application:\n"
                    messg += "----------------------------\n"
                    messg += '%s' % (algs[i])
                    logger.debug(messg)
                  # This operation may be dangerous if the name of the offline algorithm contains wildcards
                  #logger.debug('Algorithm %s/%s - set name: %s' % (algs[i].getAlias(), algs[i].getName(), matchOffline['name']))
                  algs[i].setName(matchOffline['name'])
                  if matchOffline.has_key('py_name') : algs[i].py_name = matchOffline['py_name']
                  if matchOffline.has_key('py_package') : algs[i].py_package = matchOffline['py_package']
                  algModified = True
                
                # Append an algorithm to the used offline algorithms
                dictName = '%s/%s' % (matchOffline['name'],matchOffline['alias'])
                if not matchingOfflineAlgs.has_key(dictName) :
                  matchingOfflineAlgs[dictName] = matchOffline
                
                # Handle the properties
                onlineProperties = {} 
                if onlineAlg.has_key('properties') :  onlineProperties = onlineAlg['properties']
                offlineProperties = {}
                if matchOffline.has_key('properties') :  offlineProperties = matchOffline['properties']
                
                for k in onlineProperties.keys() :
                  # Property mentioned in online and offline and in alg - set it
                  if offlineProperties.has_key(k) and algs[i].getProperties().has_key(k) :
                    if not algModified :
                      messg = "\n____________________________\n"
                      messg += "Algorithm before application:\n"
                      messg += "----------------------------\n"
                      messg += '%s' % (algs[i])
                      logger.debug(messg)
                    algs[i].setProperty(k,offlineProperties[k])
                    algModified = True
                    nmodprop += 1
                    
                  # Property mentioned in online does not exists in offline - delete it
                  if not offlineProperties.has_key(k) and algs[i].getProperties().has_key(k) :
                    if not algModified :
                      messg = "\n____________________________\n"
                      messg += "Algorithm before application:\n"
                      messg += "----------------------------\n"
                      messg += '%s' % (algs[i])
                      logger.debug(messg)
                    del algs[i].properties[k]
                    algModified = True
                    nmodprop += 1
                    
                for k,v in offlineProperties.items() :
                  # Property mentioned in offline only - set it
                  if not onlineProperties.has_key(k) :
                    if not algModified :
                      messg = "\n____________________________\n"
                      messg += "Algorithm before application:\n"
                      messg += "----------------------------\n"
                      messg += '%s' % (algs[i])
                      logger.debug(messg)
                    algs[i].setProperty(k,v)
                    algModified = True
                    nmodprop += 1
                
                # Print the debug message
                # Get to the next iteration if the online algorithm was modified
                if algModified :
                  messg = "\n____________________________\n"
                  messg += "Algorithm after application:\n"
                  messg += "----------------------------\n"
                  messg += '%s' % (algs[i])
                  logger.debug(messg)
                  nmod += 1
                  #if not algDeleted: i += 1
                  continue
          # End of loop over Rule online algorithms
          if algDeleted : continue
          # Select matching offline parent algorithms:
          for offlineAlg in self._offlineAlgs :
            if matchString(offlineAlg['name'], algs[i].name) and matchString(offlineAlg['alias'], algs[i].alias):
              dictName = '%s/%s' % (offlineAlg['name'],offlineAlg['alias'])
              if not matchingOfflineAlgs.has_key(dictName) :
                matchingOfflineAlgs[dictName] = offlineAlg
                
              # Handle the properties
              offlineProperties = {}
              if offlineAlg.has_key('properties') : offlineProperties = offlineAlg['properties']
              
              for k,v in offlineProperties.items() :
                if not algModified :
                  messg = "\n____________________________\n"
                  messg += "Algorithm before application:\n"
                  messg += "----------------------------\n"
                  messg += '%s' % (algs[i])
                  logger.debug(messg)
                algs[i].setProperty(k,v)
                algModified = True
                nmodprop += 1
                
              # Print the debug message
              if algModified :
                messg = "\n____________________________\n"
                messg += "Algorithm after application:\n"
                messg += "----------------------------\n"
                messg += '%s' % (algs[i])
                logger.debug(messg)
                nmod += 1
                
          if not algDeleted: i += 1
        # End of loop over parent algorithms for selection of matching algs
      
        # Process the offline algorithms which were not used 
        for offlineAlg in self._offlineAlgs :
          dictName = '%s/%s' % (offlineAlg['name'],offlineAlg['alias'])
          # Unused offline algorithm - it has to be appended - either to the list or to the parent algorithm
          if not matchingOfflineAlgs.has_key(dictName) :
            offlineHLTAlg = HLTAlgorithm.HLTAlgorithm(name=offlineAlg['name'],alias=offlineAlg['alias'])
            for k,v in offlineAlg.items() :
              if k != 'properties' :
                offlineHLTAlg.__setattr__(k,v)
              else:
                for kk,vv in v.items() :
                  offlineHLTAlg.setProperty(kk,vv)
            
            # Append the algorithm
            if parentAlgorithm == None :
              algs.append(offlineHLTAlg)
            else :
              parentAlgorithm.appendChild(offlineHLTAlg)
            
            nmod += 1
            
            # Printout a message
            messg = "\n____________________________\n"
            messg += "New offline Algorithm created:\n"
            messg += "----------------------------\n"
            messg += '%s' % (offlineHLTAlg)
            logger.debug(messg)
          
      # Process the children algorithms within the parent algorithms
      for parentAlg in algs :
        if parentAlg.hasChildren() : # recursive call of the apply function
          nmod += self.applyRecursive(parentAlg.getChildren(),logger)
    
    return nmod

## MergeRule class
class MergeRule(object):
  ## The constructor of the class. 
  #  @param self The object pointer
  #  @param kwargs arguments 
  def __init__(self, **kwargs):
    self.__initLogger__()
    ## @var sourceAlg 
    #  Online algorithm with maximum 1 property it's value will be copied to the offline algorithm
    self.__dict__['sourceAlg'] = None
    ## @var nactive 
    #  Counter of algorithms modified by the rule
    self.__dict__['nactive'] = 0
    
    if kwargs.has_key('online') and isinstance(kwargs['online'],HLTRuleAlgorithm):
      self.sourceAlg = kwargs['online']
  
  ## Conversion of the rule for pretty print
  def __str__(self):
    """ Convert the Copy Rule to string for print out."""
    output = "Merge Rule:"
    algStr = "%s" % (self.sourceAlg)
    output += "\n\tSource Algorithm:\n %s"
    output += "\t\t" + algStr.replace("\n","\n\t\t")
    return output
  
  ## Initialize logger of the class
  def __initLogger__(self) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTOfflineRules.MergeRule')
    if self.__dict__.has_key('OutputLevel') :
      self._log.setLevel(self.OutputLevel)
    else :
      self.__dict__['OutputLevel'] = 3
    
  ## Set attribute of the class
  def __setattr__(self, name, value) :
    self._log.verbose('set attribute %s=%s' % (name, value))
    if name not in self.__dict__.keys():
      self.warning('Can not set %s=%s - unsupported attribute' % (name, value))
      return
      
    # Setting of the source/target algorithms - check the type 
    if name in ('sourceAlg') :
      if isinstance(value, HLTRuleAlgorithm) :
        self.__dict__[name] = value
      else:
        self._log.error('Unsupported type %s of the %s' % (type(value),name))

    # Set the OutputLevel and the logger level
    if name=='OutputLevel' :
      self.__dict__[name]= value
      self._log.setLevel(value)
    
  ## Apply Method
  #  @param self The object pointer
  #  @param algs Input list of HLTAlgorithms
  #  @return Number of modified algorithms
  def apply(self, algs, OutputLevel=1):
    """ Apply the Merge Rule on the list of algorithms:
      Find matching online algorithms in the list - algs
      Apply the rule if matching algorithms found and return the number of modified algorithms.
    """
    messg  = '\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
    messg += '||       Merge Rule: Start              ||\n'
    messg += '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
    self._log.verbose(messg)
    self._log.verbose(self)
    messgEnd  = '\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
    messgEnd += '||       Merge Rule: End                 ||\n'
    messgEnd += '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
    nmod = 0
    # Test type of algs
    if not isinstance(algs, list) :
      self._log.warning('Unsupported input type %s - rule not applied' % (type(algs)))
      self._log.verbose(messgEnd)
      return 0
    
    # Find and assemble matching properties
    properties = {}
    matchingAlgs = []
    for parentAlg in algs :
      for alg in parentAlg.flattenAlg():
        if self.sourceAlg.match(alg):
          matchingAlgs.append(alg)
          for propName, value in alg.getProperties().items() :
            for propRule in self.sourceAlg.properties :
              if propRule.match(propName,value):
                if properties.has_key(propName):
                  properties[propName] = self.mergeItems(properties[propName], value)
                else:
                  properties[propName] = value
    
    nmod=0
    for alg in matchingAlgs :
      messg = "\n____________________________\n"
      messg += "Algorithm before application:\n"
      messg += "----------------------------\n"
      messg += "%s" % (alg)
      self._log.debug(messg)
      # Set the properties
      for name, value in properties.items():
        alg.setProperty(name,value)
      
      nmod += 1
      messg = "\n____________________________\n"
      messg += "Algorithm after application:\n"
      messg += "----------------------------\n"
      messg += "%s" % (alg)
      self._log.debug(messg)
   
    self.nactive += nmod
    self._log.verbose(messgEnd)
    return nmod
    
  ## Merge Values
  #  @param self The object pointer
  #  @param value1
  #  @param value2
  #  @return Merged values
  def mergeItems(self, value1, value2):
    """ Merge 2 values
        Value1 type:  Value2 type:  Result type:
          list          other         list
          other         list          list
          str           str           str
          other         other         list
        If type one of the input values is of list type it checks the present of the items and do not repeat them 
    """
    output=None
    if isinstance(value1, list) and isinstance(value2, list):
      output = []
      for item in value1 : 
        if item not in output :
          output.append(item)
      for v2 in value2 :
        if not v2 in output : output.append(v2)
        
    elif isinstance(value1, list) and not isinstance(value2, list):
      output = []
      for item in value1 : 
        if item not in output :
          output.append(item)
      if value2 not in value1 : output.append(value2)
    
    elif not isinstance(value1, list) and isinstance(value2, list):
      output = []
      for item in value2 :
        if item not in output :
          output.append(item)
      if value1 not in value2 : output.append(value1)
    
    elif isinstance(value1, str) and isinstance(value2, str):
      output = value1+value2
    
    else :
      output = []
      output.append(value1)
      if value2 not in output :
        output.append(value2)

    return output

class MergeRuleOld(object):
  _alg = {}
  _nactive = 0
  def __init__(self,*args):
    if len(args) ==1 :
      if isinstance(args[0],dict):
        if args[0].has_key('component') :
          self._alg = args[0]['component']
        else :
          self._alg = args[0]
          
  def __str__(self):
    output = "Merge Rule:\n"
    output += "\tAlgorithm:\n"
    output += "\t\t%s/%s\n" % (self._alg['name'], self._alg['alias'])
    if self._alg.has_key('properties') :
      for p,v in self._alg['properties'].items() :
        output += "\t\t\t%s = \'%s\'\n" % (p,v)
    return output
  
  def apply(self, algs, OutputLevel=3):
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTOfflineRule.Merge')
    logger.setLevel(OutputLevel)
    nmod = 0
    if isinstance(algs, list) :
      messg = '\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
      messg += '||       Merge Rule: Start              ||\n'
      messg += '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
      logger.verbose(messg)
      logger.debug(self)
      
      # Prepare the container for the merged properties
      for k in self._alg['properties'].keys() :
        self._alg['properties'][k] = []
        
      # Flatten the algorithm list
      flattenAlgs = []
      for alg in algs :
        flattenAlgs.extend(alg.flattenAlg())
      
      # Container for algorithms which has to be modified - speeds up the treatment
      modifyAlgs = []
      for alg in flattenAlgs :
        matchingAlg = matchString(self._alg['name'], alg.name) and matchString(self._alg['alias'], alg.alias)
        if matchingAlg and self._alg.has_key('py_name'):
          matchingAlg = matchString(self._alg['py_name'], alg.getPy_name())
        if matchingAlg and self._alg.has_key('py_package'):
          matchingAlg = matchString(self._alg['py_package'], alg.getPy_package())
        if matchingAlg :
          modifyAlgs.append(alg)
          for property, value in self._alg['properties'].items() :
            logger.debug('Property: %s\tValue: %s' % (property,value))
            if alg.getProperties().has_key(property) :
              logger.debug('Algorithm Property: %s\tValue: %s\t Type of value:%s' % (property,alg.getProperties()[property], type(alg.getProperties()[property])))
              for item in alg.getProperties()[property] :
                logger.verbose('Item: %s' % (item))
                if item not in value : 
                  value.append(item)
                  logger.debug('Item %s appended: %s' % (item, value))
              # Accumulate the items in the rule alg.property
              self._alg['properties'][property] = value
      
      # Loop over algorithms which has to be modified
      for alg in modifyAlgs :
        messg = "\n____________________________\n"
        messg += "Algorithm before application:\n"
        messg += "----------------------------\n"
        messg += '%s' % (alg)
        logger.debug(messg)
        for property, value in self._alg['properties'].items() :
          alg.setProperty(property, value)
        nmod += 1
        messg = "\n____________________________\n"
        messg += "Algorithm after application:\n"
        messg += "----------------------------\n"
        messg += '%s' % (alg)
        logger.debug(messg)
      #nmod += self.applyRecursive(algs,logger)
      
      # Empty the containers for the merged properties
      for k in self._alg['properties'].keys() :
        self._alg['properties'][k] = []
        
      messg = '\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
      messg += '||       Merge Rule: End                ||\n'
      messg += '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n'
      logger.verbose(messg)
    
    self._nactive += nmod
    return nmod

## RenameRule class
#  The Rename rule applies to the list of algorithms
#  Algorithm matching the online algorithm is renamed to offline
class RenameRule(object):
  ## The constructor of the class. 
  #  @param self The object pointer
  #  @param kwargs arguments 
  def __init__(self, **kwargs):
    self.__initLogger__()
    ## @var sourceAlg 
    #  Online algorithm with maximum 1 property it's value will be copied to the offline algorithm
    self.__dict__['sourceAlg'] = None
    ## @var targetAlg
    #  Offline algorithm with maximum 1 property will be set to value of the online algorithm property
    self.__dict__['targetAlg'] = None
    ## @var nactive 
    #  Counter of algorithms modified by the rule
    self.__dict__['nactive'] = 0
    
    if kwargs.has_key('online') and isinstance(kwargs['online'],HLTRuleAlgorithm):
      self.sourceAlg = kwargs['online']
      
    if kwargs.has_key('offline') and isinstance(kwargs['offline'],HLTRuleAlgorithm):
      self.targetAlg = kwargs['offline']
  
  ## Initialize logger of the class
  def __initLogger__(self) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTOfflineRules.RenameRule')
    if self.__dict__.has_key('OutputLevel') :
      self._log.setLevel(self.OutputLevel)
    else :
      self.__dict__['OutputLevel'] = 3
  
  ## Set attribute of the class
  def __setattr__(self, name, value) :
    self._log.verbose('set attribute %s=%s' % (name, value))
    if name not in self.__dict__.keys():
      self.warning('Can not set %s=%s - unsupported attribute' % (name, value))
      return
      
    # Setting of the source/target algorithms - check the type 
    if name in ('sourceAlg', 'targetAlg') :
      if isinstance(value, HLTRuleAlgorithm) :
        self.__dict__[name] = value
      else:
        self._log.error('Unsupported type %s of the %s' % (type(value),name))

    # Set the OutputLevel and the logger level
    if name=='OutputLevel' :
      self.__dict__[name]= value
      self._log.setLevel(value)
  
  ## Convert the class for pretty print
  def __str__(self):
    output = "Rename Rule:\n"
    output += "\tOnline Algorithm:"
    algStr = '%s' % (self.sourceAlg)
    output += "\n\t\t" + algStr.replace("\n","\n\t\t") + "\n"
    output += "\tOffline Algorithm:"
    algStr = '%s' % (self.targetAlg)
    output += "\n\t\t" + algStr.replace("\n","\n\t\t")
    return output
  
  ## Check self consistency of the rule
  def consistencycheck(self) :
    nonline_par = len(self.sourceAlg.properties)
    noffline_par = len(self.targetAlg.properties)
    if nonline_par > 1 or noffline_par > 1 or nonline_par != noffline_par:
      self._log.error("Incosistent number of properties in online or offline algorithm")
      return False
    return True

  ## Apply the rule to the HLTAlgorithm list
  #  @param self The object pointer
  #  @param algs - list of HLTAlgorithms
  #  @parame OutputLevel - obsolete
  def apply(self, algs, OutputLevel=3):
    messg = '\n******************************************\n'
    messg += '||       Rename Rule: Start             ||\n'
    messg += '******************************************\n'
    self._log.verbose(messg)
    self._log.verbose(self)
    nmod = 0
    if not self.consistencycheck(): return 0
    # test type of algs
    if isinstance(algs, list) :
      # Search the input algorithm list for the source algorithm value
      for parentAlg in algs :
        for alg in parentAlg.flattenAlg() :
          if self.sourceAlg.match(alg) :
            # Print the message
            messg = "\n____________________________\n"
            messg += "Algorithm before application:\n"
            messg += "----------------------------\n"
            messg += "%s" % (alg)
            self._log.debug(messg)
             # Change the algorithm
            for item in self.targetAlg.getApplicableItems() :
              if item != 'properties' :
                alg.__setattr__(item,self.targetAlg.getAttribute(item))
              elif len (self.targetAlg.properties) > 0:
                alg.setProperty(self.targetAlg.properties[0].getName(),self.targetAlg.properties[0].getvalue())
                
            nmod += 1
            # Print the message
            messg = "\n____________________________\n"
            messg += "Algorithm after application:\n"
            messg += "----------------------------\n"
            messg += "%s" % (alg)
            self._log.debug(messg)
            
    messg = '\n******************************************\n'
    messg += '||       Rename Rule: End               ||\n'
    messg += '******************************************\n'
    self._log.verbose(messg)
    self.nactive += nmod
    return nmod

class RenameRuleOld(object):
  _onlineAlg = {}
  _offlineAlg = {}
  _nactive = 0
  def __init__(self,*args):
    if len(args) ==1 :
      if isinstance(args[0],dict):
        if args[0].has_key('online') :
          self._onlineAlg = args[0]['online']
        else :
          raise HLTOfflineRuleConsistencyError('Rename Rule: No online component found')
        
        if args[0].has_key('offline') :
          self._offlineAlg = args[0]['offline']
        else :
          raise HLTOfflineRuleConsistencyError('Rename Rule: No offline component found')
        
    self._consistencycheck()
    
  def __str__(self):
    output = "Rename Rule:\n"
    output += "\tOnline Algorithm:\n"
    output += "\t\t%s/%s\n" % (self._onlineAlg['name'], self._onlineAlg['alias'])
    if self._onlineAlg.has_key('properties') :
      for p,v in self._onlineAlg['properties'].items() :
        output += "\t\t\t%s = \'%s\'\n" % (p,v)
    output+= "\tOffline Algorithm:\n"
    output += "\t\t%s/%s\n" % (self.targetAlg['name'], self.targetAlg['alias'])
    if self.targetAlg.has_key('properties') :
      for p,v in self.targetAlg['properties'].items() :
        output += "\t\t\t%s = \'%s\'\n" % (p,v)
    return output
  
  def _consistencycheck(self) :
    nonline_par = 0
    noffline_par = 0
    if self.sourceAlg.has_key('properties') :
      nonline_par = len(self._onlineAlg['properties'])
    if self._offlineAlg.has_key('properties') :
      noffline_par = len(self._offlineAlg['properties'])
    
    if nonline_par > 1 or noffline_par > 1 or nonline_par != noffline_par:
      raise HLTOfflineRuleConsistencyError('Rename Rule: Inconsistent number of parameters: \n %s' % (self))

  def apply(self, algs, OutputLevel=3):
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTOfflineRule.Rename')
    logger.setLevel(OutputLevel)
    messg = '\n******************************************\n'
    messg += '||       Rename Rule: Start             ||\n'
    messg += '******************************************\n'
    logger.verbose(messg)
    logger.debug(self)
    nmod = 0
    # test type of algs
    if isinstance(algs, list) :
      sourceParentName  = self._onlineAlg['name'].split('.')[0]
      sourceParentAlias = self._onlineAlg['alias'].split('.')[0]
      # Search the input algorithm list for the source algorithm value
      for parentAlg in algs :
        #algParentAlias = parentAlg.alias.split('.')[0]
        #if matchString(sourceParentAlias, algParentAlias) :
        for alg in parentAlg.flattenAlg() :
          matchingAlg = matchString(self._onlineAlg['name'], alg.name) and matchString(self._onlineAlg['alias'], alg.alias)
          if matchingAlg and self._onlineAlg.has_key('py_name'):
            matchingAlg = matchString(self._onlineAlg['py_name'], alg.getPy_name())
          if matchingAlg and self._onlineAlg.has_key('py_package'):
            matchingAlg = matchString(self._onlineAlg['py_package'], alg.getPy_package())
          if matchingAlg :
            # Print the message
            messg = "\n____________________________\n"
            messg += "Algorithm before application:\n"
            messg += "----------------------------\n"
            messg += "%s" % (alg)
            logger.debug(messg)
            # Change the algorithm
            for k,v in self._offlineAlg.items() :
              if k != 'properties' :
                alg.__setattr__(k,v)
            nmod += 1
            # Print the message
            messg = "\n____________________________\n"
            messg += "Algorithm after application:\n"
            messg += "----------------------------\n"
            messg += "%s" % (alg)
            logger.debug(messg)
        
    messg = '\n******************************************\n'
    messg += '||       Rename Rule: End               ||\n'
    messg += '******************************************\n'
    logger.verbose(messg)
    self._nactive += nmod
    return nmod


## ModifyRule class
#  The modify rules applies to the list of HLTAlgorithms
class ModifyRule(object):
  ## The constructor of the class. 
  #  @param self The object pointer
  #  @param kwargs arguments 
  def __init__(self, **kwargs):
    self.__initLogger__()
    ## @var sourceAlg 
    #  Online algorithm with maximum 1 property it's value will be copied to the offline algorithm
    self.__dict__['sourceAlg'] = None
    ## @var targetAlg
    #  Offline algorithm with maximum 1 property will be set to value of the online algorithm property
    self.__dict__['targetAlg'] = None
    ## @var nactive 
    #  Counter of algorithms modified by the rule
    self.__dict__['nactive'] = 0
    
    if kwargs.has_key('online') and isinstance(kwargs['online'],HLTRuleAlgorithm):
      self.sourceAlg = kwargs['online']
      
    if kwargs.has_key('offline') and isinstance(kwargs['offline'],HLTRuleAlgorithm):
      self.targetAlg = kwargs['offline']
  
  ## Conversion of the rule for pretty print
  def __str__(self):
    """ Convert the Copy Rule to string for print out."""
    output = "Modify Rule:"
    output += "\n\tSource Algorithm:"
    algStr = "%s" % (self.sourceAlg)
    output += "\t\t" + algStr.replace("\n","\n\t\t")    
    output += "\n\tTarget Algorithm:" 
    algStr = "%s" % (self.targetAlg)
    output += "\t\t" + algStr.replace("\n","\n\t\t")
    return output
  
  ## Initialize logger of the class
  def __initLogger__(self) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTOfflineRules.ModifyRule')
    if self.__dict__.has_key('OutputLevel') :
      self._log.setLevel(self.OutputLevel)
    else :
      self.__dict__['OutputLevel'] = 3
    
  ## Set attribute of the class
  def __setattr__(self, name, value) :
    self._log.verbose('set attribute %s=%s' % (name, value))
    if name not in self.__dict__.keys():
      self.warning('Can not set %s=%s - unsupported attribute' % (name, value))
      return
      
    # Setting of the source/target algorithms - check the type 
    if name in ('sourceAlg', 'targetAlg') :
      if isinstance(value, HLTRuleAlgorithm) :
        self.__dict__[name] = value
      else:
        self._log.error('Unsupported type %s of the %s' % (type(value),name))

    # Set the OutputLevel and the logger level
    if name=='OutputLevel' :
      self.__dict__[name]= value
      self._log.setLevel(value)
    
  ## Apply Method
  #  @param self The object pointer
  #  @param algs Input list of HLTAlgorithms
  #  @return Number of modified algorithms
  def apply(self, algs, OutputLevel=1):
    """ Apply the Copy Rule on the list of algorithms:
      Find matching online and offline algorithms in the list - algs
      Apply the rule if matching algorithms found and return the number of modified algorithms. 
    """
    messg = '\n******************************************\n'
    messg += '||         Modify Rule: Start            ||\n'
    messg += '******************************************\n'
    self._log.verbose(messg)
    self._log.verbose(self)
    messgEnd  = '\n******************************************\n'
    messgEnd += '||         Modify Rule: End              ||\n'
    messgEnd += '******************************************\n'
    nmod = 0
    # Test type of algs
    if not isinstance(algs, list) :
      self._log.warning('Unsupported input type %s - rule not applied' % (type(algs)))
      self._log.verbose(messgEnd)
      return 0
    
    # Search the input algorithm list for the source algorithm value
    for parentAlg in algs :
      for alg in parentAlg.flattenAlg() :
        if self.sourceAlg.match(alg, 0x1ff) : # Match the online algorithm - match only property name not a value
          for propName, propValue in alg.getProperties().items() :
            propToSet = {}
            # Loop over source properties to match and remove the items
            for srcAlgProp in self.sourceAlg.properties :
              if srcAlgProp.match(propName,propValue, 0x1) :
                # Test if all items are present in the algorithm property
                matchFlag = True
                copyPropValue = propValue[:]
                for item in srcAlgProp.getValue():
                  if item not in propValue:
                    matchFlag = False
                    break
                  else : # remove item from the value
                    i=0
                    while i < len(copyPropValue) :
                      if copyPropValue[i]==item :
                        del copyPropValue[i]
                      else :
                        i += 1
                        
                if matchFlag :
                  propToSet[propName] = copyPropValue
            
            # Loop Over target properties to set the items
            for trgAlgProp in self.targetAlg.properties:
              if trgAlgProp.match(propName,propValue, 0x1) and propToSet.has_key(propName):
                for item in trgAlgProp.getValue(): propToSet[propName].append(item)
            
            # Set the properties of the algorithm
            for k,v in propToSet.items() :
              # Print the message
              messg = "\n____________________________\n"
              messg += "Algorithm before application:\n"
              messg += "----------------------------\n"
              messg += "%s" % (alg)
              self._log.debug(messg)
              alg.setProperty(k,v)
              nmod += 1
              messg = "\n____________________________\n"
              messg += "Algorithm after application:\n"
              messg += "----------------------------\n"
              messg += "%s" % (alg)
              self._log.debug(messg)
    
    # End of apply
    self._log.verbose(messg)
    self.nactive += nmod
    return nmod
                
class ModifyRuleOld(object):
  _onlineAlg = {}
  _offlineAlg = {}
  _nactive = 0
  def __init__(self,*args):
    if len(args) ==1 :
      if isinstance(args[0],dict):
        if args[0].has_key('online') :
          self._onlineAlg = args[0]['online']
        else :
          raise HLTOfflineRuleConsistencyError('Modify Rule: No online component found')
        
        if args[0].has_key('offline') :
          self._offlineAlg = args[0]['offline']
        else :
          raise HLTOfflineRuleConsistencyError('Modify Rule: No offline component found')
        
  def __str__(self):
    output = "Modify Rule:\n"
    output += "\tOnline Algorithm:\n"
    output += "\t\t%s/%s\n" % (self._onlineAlg['name'], self._onlineAlg['alias'])
    if self._onlineAlg.has_key('properties') :
      for p,v in self._onlineAlg['properties'].items() :
        output += "\t\t\t%s = \'%s\'\n" % (p,v)
    output+= "\tOffline Algorithm:\n"
    output += "\t\t%s/%s\n" % (self._offlineAlg['name'], self._offlineAlg['alias'])
    if self._offlineAlg.has_key('properties') :
      for p,v in self._offlineAlg['properties'].items() :
        output += "\t\t\t%s = \'%s\'\n" % (p,v)
    return output

  def apply(self, algs, OutputLevel=3):
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTOfflineRule.Modify')
    logger.setLevel(OutputLevel)
    messg = '\n##########################################\n'
    messg += '||       Modify Rule: Start             ||\n'
    messg += '##########################################\n'
    logger.verbose(messg)
    logger.debug(self)
    nmod = 0
    if isinstance(algs, list) :
      sourceParentName  = self._onlineAlg['name'].split('.')[0]
      sourceParentAlias = self._onlineAlg['alias'].split('.')[0]
      # Search the input algorithm list for the source algorithm value
      for parentAlg in algs :
        #algParentAlias = parentAlg.alias.split('.')[0]
        #if matchString(sourceParentAlias, algParentAlias) :
        for alg in parentAlg.flattenAlg() :
          matchingAlg = matchString(self._onlineAlg['name'], alg.name) and matchString(self._onlineAlg['alias'], alg.alias)
          if matchingAlg and self._onlineAlg.has_key('py_name'):
            matchingAlg = matchString(self._onlineAlg['py_name'], alg.getPy_name())
          if matchingAlg and self._onlineAlg.has_key('py_package'):
            matchingAlg = matchString(self._onlineAlg['py_package'], alg.getPy_package())
          if matchingAlg :
            for k in  self._onlineAlg['properties'].keys():
              if k in alg.getProperties().keys():
                onlinevalues = self._onlineAlg['properties'][k]
                propvalue = alg.properties[k]
                nonlinefound = 0
                for onlineel in onlinevalues :
                  if onlineel in propvalue :
                    nonlinefound += 1
                
                # If all items in the rule found in algorithm property the algorithm will be changed
                if nonlinefound == len(self._onlineAlg['properties'][k]) :
                  messg = "\n____________________________\n"
                  messg += "Algorithm before application:\n"
                  messg += "----------------------------\n"
                  messg += "\t %s/%s.%s = %s" % (alg.name, alg.alias, k, alg.properties[k])
                  logger.debug(messg)
                  ind = 0
                  if nonlinefound != 0 :
                    # Delete the online items
                    for onlineel in onlinevalues :
                      while ind < len(alg.properties[k]) :
                        try:
                          ind = alg.properties[k].index(onlineel)
                          del alg.properties[k][ind]
                        except :
                          ind = len(alg.properties[k])
                  # Extend algorithm value with the offline items
                  for offlineeel in self._offlineAlg['properties'][k] :
                    if offlineeel not in alg.properties[k] :
                      alg.properties[k].append(offlineeel)
                  messg = "\n____________________________\n"
                  messg += "Algorithm after application:\n"
                  messg += "----------------------------\n"
                  messg += "\t %s/%s.%s = %s" % (alg.name, alg.alias, k, alg.properties[k])
                  logger.debug(messg)
                  nmod += 1
        
    messg = '\n##########################################\n'
    messg += '||       Modify Rule: %d End              ||\n' % (nmod)
    messg += '##########################################\n'
    logger.verbose(messg)
    self._nactive += nmod
    return nmod

class SortRule(object):
  _alg = {}
  _sortOrder = 'start'
  _nactive = 0
  def __init__(self,*args):
    if len(args) ==1 :
      if isinstance(args[0],dict):
        if args[0].has_key('sortOrder') :
          self._sortOrder = args[0]['sortOrder'].lower()
        if args[0].has_key('component') :
          self._alg = args[0]['component']
        else :
          raise HLTOfflineRuleConsistencyError('Sort Rule: No component found: %s' % (args[0]))
        
  def __str__(self):
    output = "Sort Rule:\n"
    output +="\tSort Order: %s" % (self._sortOrder)
    output += "\tAlgorithm:\n"
    output += "\t\t%s/%s\n" % (self._alg['name'], self._alg['alias'])
    if self._alg.has_key('properties') :
      for p,v in self._alg['properties'].items() :
        output += "\t\t\t%s = \'%s\'\n" % (p,v)
    return output

  def apply(self, algs, OutputLevel=3):
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTOfflineRule.Sort')
    logger.setLevel(OutputLevel)
    messg = '\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n'
    messg += '||       Sort Rule: Start               ||\n'
    messg += '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n'
    logger.verbose(messg)
    logger.debug(self)
    nmod = 0
    if isinstance(algs, list) :
      sourceParentName  = self._alg['name'].split('.')[0]
      sourceParentAlias = self._alg['alias'].split('.')[0]
      # Search the input algorithm list for the source algorithm value
      for parentAlg in algs :
        #algParentAlias = parentAlg.alias.split('.')[0]
        #if matchString(sourceParentAlias, algParentAlias) :
        for alg in parentAlg.flattenAlg() :
          matchingAlg = matchString(self._alg['name'], alg.name) and matchString(self._alg['alias'], alg.alias)
          if matchingAlg and self._alg.has_key('py_name'):
            matchingAlg = matchString(self._alg['py_name'], alg.getPy_name())
          if matchingAlg and self._alg.has_key('py_package'):
            matchingAlg = matchString(self._alg['py_package'], alg.getPy_package())
          if matchingAlg :
            algModified=False
            # Loop over properties of the sort algorithm
            for k in  self._alg['properties'].keys():
              if k in alg.getProperties().keys():
                # Deepcopy the algorithm property
                propvalue = []
                for algv in alg.getProperties()[k] :
                  propvalue.append(algv)
                    
                nfound = 0
                matchvalues = []
                # Loop over items of the Sort algorithm
                for onlineel in self._alg['properties'][k] :
                  ind = 0
                  lmatch = len(matchvalues)
                  while ind < len(propvalue) :
                    try :
                      # Find sort Algorithm item
                      ind = propvalue.index(onlineel)
                      # If found delete it from the list
                      matchvalues.append(propvalue.pop(ind))
                    except:
                      # If not found set the ind to stop the loop
                      ind = len(propvalue)
                  # set the count of found values
                  if lmatch < len(matchvalues) :
                    nfound += 1
                
                if len(matchvalues) != len(self._alg['properties'][k]) and nfound == len(self._alg['properties'][k]) :
                  # Mismatch of items in algorithm and values listed in sort rule - more items in the alg property
                  logger.warning("%s can't be applied inconsistent properties: %s" % (self._alg['properties'][k], alg.properties[k]))
                elif nfound == len(self._alg['properties'][k]):
                  # Match - found properties
                  value = alg.properties[k]
                  # Prepend the items
                  if self._sortOrder in ['start', 'begin'] :
                    value = self._alg['properties'][k]
                    value.extend(propvalue)
                  # Append the items
                  if self._sortOrder in ['end'] :
                    value = propvalue
                    value.extend(self._alg['properties'][k])
                  
                  if not algModified :
                    messg = "\n____________________________\n"
                    messg += "Algorithm before application:\n"
                    messg += "----------------------------\n"
                    messg += '%s' % (alg)
                    logger.debug(messg)
                  
                  # Set the algorithm.property value
                  logger.verbose("Previous value of property: %s/%s.%s=%s" % (alg.name, alg.alias, k, alg.getProperties()[k]))
                  alg.setProperty(k,value)
                  algModified=True
                  logger.verbose("Set value of property: %s/%s.%s=%s" % (alg.name, alg.alias, k,  alg.getProperties()[k]))
            
            if algModified :
              messg = "\n____________________________\n"
              messg += "Algorithm after application:\n"
              messg += "----------------------------\n"
              messg += '%s' % (alg)
              logger.debug(messg)

    messg = '\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n'
    messg += '||       Sort Rule: End                 ||\n'
    messg += '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n'
    logger.verbose(messg)
    self._nactive += nmod
    return nmod

## Container class for the algorithms used in rules
class HLTRuleAlgorithm(object):
  import re
        
  ## Class Constructor
  def __init__(self,**kwargs):
    self.__initLogger__()
    ## @var name 
    # Name (type) of the HLTComponent (algorithm)
    self.__dict__['name'] = re.compile(".*")
    ## @ var alias
    #  Alias (name) of the HLTComponent (algorithm) 
    self.__dict__['alias'] = re.compile(".*")
    ## @var py_name
    #  Python class of the HLTComponent (algorithm) 
    #self.__dict__['py_name'] = re.compile(".*")
    self.__dict__['py_name'] = None
    ## @var py_package
    #  Python module of the HLTComponent (algorithm) 
    #self.__dict__['py_package'] = re.compile(".*")
    self.__dict__['py_package'] = None
    ## @var alg_type
    #  Type of the HLTComponent (algorithm) - usual values TopAlg, Service, PublicTool, PrivateTool, Auditor, SteerAlg 
    #self.__dict__['alg_type'] = re.compile(".*")
    self.__dict__['alg_type'] = None
    ## @var properties
    #  List of Properties
    self.__dict__['properties']=[]
    
    # Set the attributes
    for k,v in kwargs.items() :
      self.__setattr__(k,v)
    
  ## Initialize the logger
  def __initLogger__(self) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTRuleAlgorithm')
    ## @var OutputLevel
    #  Logger level
    self.__dict__['OutputLevel'] = 3
    self._log.setLevel(self.OutputLevel)
    
  ## Set attributes of the class
  #  @param self The object pointer
  #  @param name The name of the attribute
  #  @param value The value of the attribute to be set
  def __setattr__(self, name, value) :
    # Set only the allowed attributes
    if name in self.__dict__.keys() and name not in ('._log','OutputLevel','properties'):
      if not isinstance(value,(str,unicode)):
        self._log.error('Unsupported type %s of attribute %s - must be str' % (type(name),name))
      else:
        try :
          regex = re.compile(r"%s" % (value))
          if regex: 
            self.__dict__[name] = regex 
        except Exception, exc:
          self._log.debug('%s' % (exc.message))
          self.__dict__[name] = value
    elif name=='OutputLevel' :
      self.__dict__[name] = value
      self._log.setLevel(value)
    elif name=='properties' :
      if isinstance(value,list) :
        self.__dict__[name] = value
      else :
        self._log.error('Unsupported type %s of attribute properties' % (type(value)))
    else :
      self._log.error('Unsupported attribute name=\"%s\"' % (name))
      
  ## Convert to string for pretty print
  #  @return string
  def __str__(self):
    alg_type = self.getAttribute('alg_type')
    name = self.getAttribute('name')
    alias = self.getAttribute('alias')
    py_name = self.getAttribute('py_name')
    py_package = self.getAttribute('py_package')
    output = '%s:\n/***** -%s- %s/%s (Python: import %s from %s)' % ('HLTRuleAlgorithm',alg_type,name,alias,py_name,py_package)
    # Print the properties
    if len(self.properties) > 0:
      for prop in self.properties :
        output += '\n|-%s' % (prop)
          
    output += '\n\------------------------------------------------------'
    return output
  
  ## Get applicable items of the HLTRuleAlgorithm
  #  @return List of items which might be used for application of the rule
  def getApplicableItems(self) :
    output = []
    items=('name','alias','py_name','py_package','alg_type','properties')
    return [item for item in items if item in self.__dict__.keys() and self.__dict__[item]!=None]

  ## Get HLTRuleAlgorithm public attribute
  #  @param self Object pointer
  #  @param name Attribute name
  #  @return value of public attributes 
  def getAttribute(self, name):
    if name not in self.__dict__.keys():
      self._log.error('Attribute %s does not exists' % (name))
      self._log.verbose('List of supported attributes: %s' % (self.__dict__.keys()))
      return None
    
    if name in ('name','alias','py_name','py_package','alg_type') :     # Regular expression attributes
      value = None
      try: 
        value = self.__dict__[name].pattern
      except Exception, exc :
        value = self.__dict__[name]
      return value
    elif not name.startswith('_') : # Strictly private attributes
      return self.__dict__[name]
    
    return None
  
  ## Set property of the HLTRuleAlgorithm
  #  @param args  
  def setProperty(self, *args):
    if len(args) == 1:
      self._setProperty1(args[0])
    elif len(args) == 2:
      self._setProperty2(args[0], args[1])
    else :
      self._log.error("Invalid number of arguments %d - property not set" % (len(args)))
      
  ## Set property of the HLTRuleAlgorithm
  #  @param name Property name
  #  @param value Property value
  def _setProperty2(self, name, value):
    propertySet = False
    for i in range(len(self.properties)):
      if name == self.properties[i].getName(): 
        self.properties[i].value=value
        propertySet = True
    
    if not propertySet: self.properties.append(HLTRuleProperty(name=name,value=value))
      
  ## Set property of the HLTRuleAlgorithm
  #  @param prop HLTRuleProperty
  def _setProperty1(self, prop):
    name = prop.getName()
    value= prop.getValue()
    self.setProperty(name, value)
    
  ## Match the HLTAlgorithm to the HLTRuleAlgorithm
  #  @param self The object pointer
  #  @param hltAlgorithm HLTAlgorithm object
  #  @param pattern Mask for the matching entities
  #  @return True in case of matching attributes
  def match(self,hltAlgorithm, pattern=0xffff):

    output = True
    # Match the main attributes
    if output and ((0x1 << 0) & pattern) and self.__dict__.has_key('alias') and self.__dict__['alias'] != None:
      output = self.matchAttribute('alias',hltAlgorithm.getAlias())
    if output and ((0x1 << 1) & pattern) and self.__dict__.has_key('name') and self.__dict__['name'] != None:
      output = self.matchAttribute('name',hltAlgorithm.getName())
      self._log.verbose("Pattern:\t %s" % (self))
      self._log.verbose("HLTAlgorithm:\t %s" % (hltAlgorithm))

    if output and ((0x1 << 2) & pattern) and self.__dict__.has_key('py_name') and self.__dict__['py_name'] != None:
      output = self.matchAttribute('py_name',hltAlgorithm.getPy_name())
    if output and ((0x1 << 3) & pattern) and self.__dict__.has_key('py_package') and self.__dict__['py_package'] != None:
      output = self.matchAttribute('py_package',hltAlgorithm.getPy_package())
    if output and ((0x1 << 4) & pattern) and self.__dict__.has_key('alg_type') and self.__dict__['alg_type'] != None:
      output = self.matchAttribute('alg_type',hltAlgorithm.getType())
    
    if not output or ((pattern >> 8) & 0xff)==0: return output
    
    # Match the properties
    for prop in self.properties :
      propMatch = False
      for k,v in hltAlgorithm.getProperties().items() :
        if prop.match(k,v, (pattern>>8)) :
          propMatch = True
          break
      if not propMatch :
        self._log.debug('None matching property %s found' % (prop))
        return False
      
    return output
  
  ## Get matching group
  #  @param self the object pointer
  #  @param name name of the attribute to be matched
  #  @param value string value to be matched
  #  @return matching group
  def getMatchingGroup(self,name,value) :
    output = None
    if not self.matchAttribute(name,value):
      self._log.debug("Attribute %s: value \'%s\' does not match: \'%s\'" % (name, value, self.getAttribute(name)))
      return output
    
    # Matching is fine - get the group
    try :
      output=self.__dict__[name].match("%s" % (value)).group()
    except Exception, exc:
      self._log.verbose('getMatch(%s,%s)' % (exc.message,name,value))
      output=value
    
    return output
  
  ## Match single attribute
  #  @param self The object pointer
  #  @param name Name of the attribute
  #  @param value Value of the attribute
  #  @return True in case of matching attributes
  def matchAttribute(self, name, value) :
    match=False
    if name not in self.__dict__.keys():
      self._log.warning('Can not match %s - no such attribute' % (name))
      return False
    if name in ('_log','OutputLevel', 'properties') :
      self._log.warning('Can not compare %s - unsupported' % (name))
      return False
    
    try :
      match = self.__dict__[name].match("%s" % (value))
      if match: self._log.verbose('Attribute %s=%s matches regexp %s' % (name, value, self.__dict__[name].pattern))
    except Exception, exc:
      self._log.verbose('match - %s' % (exc.message))
      match = (self.__dict__[name] == value)
      if match: self._log.verbose('Attribute %s=%s matches %s' % (name, value, self.__dict__[name]))

    return match
  
## Container class for the properties used in the HLTRuleAlgorithms
class HLTRuleProperty(object):
  ## Class Constructor
  def __init__(self,**kwargs):
    self.__initLogger__()
    ## @var name 
    #  Name of the property
    self.__dict__['name'] = re.compile(".*")
    ## @var value
    #  Property value 
    self.__dict__['value'] = re.compile(".*")

    for k,v in kwargs.items() :
      self.__setattr__(k,v)
      
  ## Initialize the logger
  def __initLogger__(self) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTRuleProperty')
    self.__dict__['OutputLevel'] = 3
    self._log.setLevel(self.OutputLevel)
  
  ## Convert HLTRuleProperty to the string for pretty print
  def __str__(self) :
    out = ''
    try:
      out += '%s= ' % (self.name.pattern)
    except :
      out += '%s= ' % (self.name)
    
    try:
      out += '%s' % (self.value.pattern)
    except :
      out += '%s' % (self.value)
    
    return out
    
  ## Set attributes of the class
  #  @param self The object pointer
  #  @param name The name of the attribute
  #  @param value The value of the attribute to be set
  def __setattr__(self, name, value) :
    # Set only the allowed attributes
    if name in self.__dict__.keys() and name != '._log':
      if name=='name':
        if not isinstance(value,(str,unicode)):
          self._log.error('Unsupported type %s of attribute %s - must be str' % (type(name),name))
        else:
          try :
            regex = re.compile(r"%s" % (value))
            if regex: 
              self.__dict__[name] = regex 
          except Exception, exc:
            self._log.error('%s' % (exc.message))
      
      if name=='value':
        if not isinstance(value,(str,unicode)):
          self.__dict__[name] = value
        else:
          try :
            regex = re.compile(r"%s" % (value))
            if regex: 
              self.__dict__[name] = regex 
          except Exception, exc:
            self._log.debug('%s' % (exc.message))
            self.__dict__[name] = value
      
      if name=='OutputLevel': 
        self._log.setLevel(value)
        
    else:
      self._log.error('Unsupported attribute name=\"%s\"' % (name))
        
  ## Match the property name and value to the HLTRuleProperty
  #  @param self The object pointer
  #  @param name The name of the proeprty
  #  @param value The value of the property
  #  @param pattern Bit mask what should be compared
  #  @return True if matching False if not
  def match(self, name, value, pattern=0xff) :
    if not isinstance(name,(str,unicode)):
      self._log.error('Unsupported type %s of the name %s' % (type(name), name))
      return False
    
    output = True
    # Compare name
    if output and ((0x1 << 0) & pattern) : output=self.matchAttribute("name", name)
    
    # Compare value
    if output and ((0x1 << 1) & pattern) : output=self.matchAttribute("value", value)
    
    return  output
  
  ## Get the property name
  #  @param self Object pointer
  #  @return name attribute
  def getName(self) :
    name = None
    try :
      name = self.name.pattern
    except :
      name = self.name
    return name
    
  ## Get the property value
  #  @param self Object pointer
  #  @return value attribute
  def getValue(self) :
    value = None
    try :
      value = self.value.pattern
    except :
      value = self.value
    return value
  
  ## Get public attribute
  #  @param self Object pointer
  #  @param name Attribute name
  #  @return value of public attributes 
  def getAttribute(self, name):
    if name not in self.__dict__.keys():
      self._log.error('Attribute %s does not exists' % (name))
      self._log.verbose('List of supported attributes: %s' % (self.__dict__.keys()))
      return None
    
    if name in ('name','value') :     # Regular expression attributes
      value = None
      try:
        value = self.__dict__[name].pattern
      except Exception, exc :
        value = self.__dict__[name]
      return value
    elif not name.startswith('_') : # Strictly private attributes
      return self.__dict__[name]
    
    return None
  
  ## Match single attribute
  #  @param self The object pointer
  #  @param name Name of the attribute
  #  @param value Value of the attribute
  #  @return True in case of matching attributes
  def matchAttribute(self, name, value) :
    match=False
    if name not in self.__dict__.keys():
      self._log.warning('Can not match %s - no such attribute' % (name))
      return False
    if name in ('_log','OutputLevel') :
      self._log.warning('Can not compare %s - unsupported' % (name))
      return False
    
    try :
      match = self.__dict__[name].match("%s" % (value))
      if match: self._log.verbose('Attribute %s=%s matches regexp %s' % (name, value, self.__dict__[name].pattern))
    except Exception, exc:
      self._log.verbose('match - %s' % (exc.message))
      match = (self.__dict__[name] == value)
      if match: self._log.verbose('Attribute %s=%s matches %s' % (name, value, self.__dict__[name]))

    return match
    
  ## Get matching group
  #  @param self the object pointer
  #  @param name name of the attribute to be matched
  #  @param value string value to be matched
  #  @return matching group
  def getMatchingGroup(self,name,value) :
    output = None
    if not self.matchAttribute(name,value):
      self._log.debug("Attribute %s: value \'%s\' does not match: \'%s\'" % (name, value, self.getAttribute(name)))
      return output
    
    # Matching is fine - get the group
    try :
      output=self.__dict__[name].match("%s" % (value)).group()
    except Exception, exc:
      self._log.verbose('getMatch(%s,%s)' % (exc.message,name,value))
      output=value
    
    return output
  
def getAlgJobOptNames(algs) :
  output = []
  for a in algs :
    output.append('%s/%s' % (a['name'],a['alias']))
    
  return output

def matchString(strPattern, strInput):
  # Function to match wildcards string pattern with some input string
  # Test the simple cases: no wildcard present or anything matches
  if not '*' in strPattern :
    return strInput == strPattern
  elif strPattern == '*' :
    return True
  
  # First input is a pattern which may contain wildcards - second is a string which has to be compared
  patterns = strPattern.split('*')
  
  output = True
  indexes = []
  lastindex = 0
  for part in patterns :
    ind = strInput.find(part,lastindex)
    if not ind==-1 :
      lastindex = ind+1
    else:
      return False
  
  if len(patterns[-1])>0 and not (lastindex-1+len(patterns[-1]) == len(strInput)) :
      output = False
  return output
  

def alg2str(alg) :
  output = "%s/%s" % (alg['name'], alg['alias'])
  
  if alg.has_key('topalg') :
    output += "\t TopAlg = %s" % (alg['topalg'])
  
  output +='\n'
  
  if alg.has_key('properties') :
    for p,v in alg['properties'].items() :
      output += "\t%s = \'%s\'\n" % (p,v)
  return output
  
