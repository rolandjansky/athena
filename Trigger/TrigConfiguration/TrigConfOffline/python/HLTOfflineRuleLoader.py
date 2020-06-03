###########################################################################
#    
#    <nozicka@mail.desy.de>
#
#    Description:
#      Class to loads the rules from given source
#
###########################################################################

import string

## OfflineRuleLoader Class
#  The rules for modification of an online trigger setup to offline trigger setup 
class OfflineRuleLoader(object) :
  ## @var _properties List of attributes which may be set for class OfflineRuleLoader
  _properties = ('Source','dbType','dbHost','dbUser','dbPasswd','dbName','SMKey','RunNumber','OutputLevel')
  
  ## Exception to be raised if the xml rules file is inconsistent
  class XMLConsistencyError(Exception):
    pass
  
  ## Class constructor
  def __init__(self, *args, **kwargs) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTOfflineRuleLoader')
    
    ## @var Source
    #  rule source - set the default value to offline_setup_rules.xml 
    self.Source = "offline_setup_rules.xml"
    
    for k,v in kwargs.items() :
      self.__setattr__(k,v)
    
    self._log.verbose("\n%s" % (self))
    
  def setlog(self) :
    if self.__dict__.has_key('OutputLevel'):
      self._log.setLevel(self.OutputLevel)
    else:
      self._log.setLevel(3)
    self._log.name = 'HLTOfflineRuleLoader '
  
  def __setattr__(self, name, value) :
    self.setlog()
    # Find the absolut path to the file
    if name=='Source' and value.lower().endswith('.xml'):
      filepath=value
      # find the absolute file path
      import os
      from AthenaCommon.Utils.unixtools import FindFile
      # look for it in the local and then all XML directories
      filepath = FindFile(filepath,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
      
      # Not found in user space - look in the AtlasAuthentication package
      if not filepath:
        filepath = 'TrigConfOffline/'+value
        filepath = FindFile(filepath,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )

      if not filepath:
        self._log.warning("Rules source file %s not found" % (value))
      else :
        value=filepath
        self._log.debug("Rules source file found in %s" % (value))
      
    if name in self._properties:
      self._log.debug('Set property: %s = %s' % (name, value))
      self.__dict__[name] = value
      if name=='OutputLevel':
        self._log.setLevel(value)
    else:
      self._log.warning('Unknown property: %s = %s  - property not set' % (name, value))

  def __str__(self):
    self.setlog()
    output = "-"*80 + "\n"
    string = "%s.%s" % (self.__module__,'HLTOfflineRuleLoader')
    output += "|" + " "*(39 - len(string)/2) + string +" "*(39 - len(string)/2) + "|\n"
    output += "-"*80 + "\n"
    
    for k,v in self.__dict__.items() :
      if k == 'dbPasswd' :
        output += "\t%s = %s\n" % (k,"*"*len(v))
      elif k[0] != '_' :
        output += "\t%s = %s\n" % (k,v)

    return output
      
  def load(self) :
    self.setlog()
    #import TrigConfOffline.HLTOfflineRules
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    # Returns the list of Rules
    rules = []
    
    if not self.Source.lower().endswith('.xml'):
      rules = self._loadfromDB()
    else:
      rules = self._loadfromFile()
    
    self._log.info('Loaded %d rules from %s' % (len(rules), self.Source))
    return rules
  
  def _loadfromDB(self):
    self.setlog()
    self._log.debug('Load from DB')
    # DB connection utils
    cursor = self.connectDB()
    
    rules = []
    ruleSetKey = self.getRuleSetKey(cursor)
    if ruleSetKey > 0 :
      for ruleID in  self.getRuleIDs(ruleSetKey, cursor) :
        rule = self.loadRuleFromDb(ruleID, cursor)
        if not rule:
          continue
        elif isinstance(rule,list) :
          rules.extend(rule)
        else:
          rules.append(rule)
          
    self._log.debug('%d rules loaded' % (len(rules)))
    return rules
  
  def _loadfromFile(self):
    self.setlog()
    self._log.debug('Load from file')
    if self.Source.lower().endswith('.xml') :
      return self._loadfromXML()
    else:
      self._log.error('Unsupported source type: %s' % (self.Source))
      return []
    
  def _loadfromXML(self):
    self.setlog()
    import xml.dom.minidom
    rules = []
    doc = xml.dom.minidom.parse(self.Source)
    doc = doc.documentElement

    doc_children = doc.childNodes
    i = 0
    for ch in doc_children :
      if ch.nodeName in ['Copy','Modify','Replace','Rename','Sort','Merge']:
        i += 1
        rule = self.loadRulefromXML(ch)
        if not rule:
          continue
        elif isinstance(rule,list) :
          rules.extend(rule)
        else:
          rules.append(rule)
    
    return rules

  def connectDB(self):
    self.setlog()
    import TrigConfOffline.HLTSQLUtils as HLTSQLUtils
    cursor = HLTSQLUtils.get_cursor(self.dbHost, self.dbUser, self.dbName, self.dbType, False, self.dbPasswd)
    return cursor

  def loadParameterfromXML(self,node):
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    parameter = {}
    name = ""
    value = ""
    if node.hasAttribute('name') :
      name = str(node.getAttribute('name'))
    else:
      self._log.error('Parameter has no attribute name %s' % (node.toxml()))
      return None
    
    if node.hasAttribute('value') :
      value = str(node.getAttribute('value'))
      try:
        value = eval(value)
      except :
        # Nothing happened just pass the value as string
        pass
    
    parameter = {name:value}
    return parameter
  
  def loadComponentfromXML(self, node):
    self.setlog()
    component = {}
    
    alias=""
    if node.hasAttribute('alias') :
      alias = str(node.getAttribute('alias'))
    else:
      self._log.error('Component has no attribute alias %s' % (node.toxml()))
      return None
        
    name=alias
    if node.hasAttribute('name') :
      name = str(node.getAttribute('name'))
    
    component = {'alias': alias, 'name': name}
    # Set the py_package and py_name if exist
    if node.hasAttribute('py_name'): component['py_name'] = str(node.getAttribute('py_name'))
    if node.hasAttribute('py_package'): component['py_package'] = str(node.getAttribute('py_package'))
    
    # Set the type of the algorithm 
    if node.hasAttribute('topalg') and str(node.getAttribute('topalg'))=="1": component['type']='TopAlg'
    if node.hasAttribute('svc') and str(node.getAttribute('svc'))=="1": component['type']='Service'
    if node.hasAttribute('pubT') and str(node.getAttribute('pubT'))=="1": component['type']='PublicTool'
    if node.hasAttribute('privT') and str(node.getAttribute('privT'))=="1": component['type']='PrivateTool'
    if node.hasAttribute('aud') and str(node.getAttribute('aud'))=="1": component['type']='Auditor'
    
    if node.hasChildNodes() :
      doc_pars = node.getElementsByTagName('parameter')
      if len(doc_pars) > 0:
        component.setdefault('properties',{})
      for p in doc_pars :
        par = self.loadParameterfromXML(p)
        component['properties'].update(par)
    return component
      
  
  def loadRulefromXML(self, node) :
    self.setlog()
    rules = None
    name = node.nodeName
    if name == 'Sort':
      rules = []
      for ind in range(len(node.getElementsByTagName('component'))) :
        rule = self.loadSortRulefromXML(node, ind)
        if rule :
          rules.append(rule)
      return rules
    
    if name == 'Copy':
      return self.loadCopyRulefromXML(node)
    
    if name == 'Replace':
      return self.loadReplaceRulefromXML(node)
        
    if name == 'Rename':
      return self.loadRenameRulefromXML(node)
    
    if name == 'Modify':
      return self.loadModifyRulefromXML(node)
    
    if name == 'Merge':
      rules = []
      for ind in range(len(node.getElementsByTagName('component'))) :
        rule = self.loadMergeRulefromXML(node, ind)
        if rule:
          rules.append(rule)
      return rules
    
    return rules
  
  def loadSortRulefromXML(self, node, ind) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    rule = {}
    if not node.hasAttribute('position') :
      self._log.error('Sort Rule Node has no Attributes %s' % (node.toxml()))
      return None
    sortOrder = str(node.getAttribute('position'))
    comp_node = node.getElementsByTagName('component')[ind]
    return HLTOfflineRules.SortRule({'sortOrder':sortOrder,'component': self.loadComponentfromXML(comp_node)})
  
  ## Load Merge Rule from XML node
  #  @param self OfflineRuleLoader object pointer
  #  @param node XML node
  #  @return CopyRule object or None if not successfull
  def loadMergeRulefromXML(self, node, ind) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    rule = {}
    comp_node = node.getElementsByTagName('component')[ind]
    return HLTOfflineRules.MergeRule(online=loadHLTRuleAlgorithmFromXML(comp_node), OutputLevel=self.OutputLevel)

  ## Load Copy Rule from XML node
  #  @param self OfflineRuleLoader object pointer
  #  @param node XML node
  #  @return CopyRule object or None if not successfull
  def loadCopyRulefromXML(self, node) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    rule = {}
    # Check the number of online nodes - must be 1
    online_node = node.getElementsByTagName('online')
    if not len(online_node) == 1:
      self._log.error('Copy Rule inconsistent number of online items %s' % (node.toxml()))
      return None
    online_node = online_node[0]
    
    # Check the number of offline nodes - must be 1
    offline_node = node.getElementsByTagName('offline')
    if not len(offline_node) == 1:
      self._log.error("Copy Rule inconsistent number of offline items \n%s" % (node.toxml()))
      return None
    offline_node = offline_node[0]
    
    # Check for online components - must be 1
    online_component = None
    if online_node.hasChildNodes():
      comp_nodes = online_node.getElementsByTagName('component')
      if not len(comp_nodes) == 1:
        self._log.error("Copy Rule inconsistent number of online components \n%s" % (node.toxml()))
        return None
      
      # Check the number of online component parameters - must be 1
      par_nodes = comp_nodes[0].getElementsByTagName('parameter')
      if not len(par_nodes) == 1:
        self._log.error("Copy Rule inconsistent number of online components \n%s" % (node.toxml()))
        return None
      
      online_component = loadHLTRuleAlgorithmFromXML(comp_nodes[0])
      
    # Check for offline components - must be 1
    offline_component = None
    if offline_node.hasChildNodes():
      comp_nodes = offline_node.getElementsByTagName('component')
      if not len(comp_nodes) == 1:
        self._log.error("Copy Rule inconsistent number of offline components \n%s" % (node.toxml()))
        return None
      
      # Check the number of offline component parameters - must be 1
      par_nodes = comp_nodes[0].getElementsByTagName('parameter')
      if not len(par_nodes) == 1:
        self._log.error("Copy Rule inconsistent number of online components \n%s" % (node.toxml()))
        return None
      
      offline_component = loadHLTRuleAlgorithmFromXML(comp_nodes[0])
    
    return HLTOfflineRules.CopyRule(online=online_component, offline=offline_component, OutputLevel=self.OutputLevel)
  
  def loadModifyRulefromXML(self, node) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    rule = {}
    online_node = node.getElementsByTagName('online')
    if not len(online_node) == 1:
      self._log.error("Modify Rule inconsistent number of online items \n%s" % (node.toxml()))
      return None
    online_node = online_node[0]
    
    offline_node = node.getElementsByTagName('offline')
    if not len(offline_node) == 1:
      self._log.error("Modify Rule inconsistent number of offline items \n%s" % (node.toxml()))
      return None
    offline_node = offline_node[0]
    
    # Check for online components
    online_component = {}
    if online_node.hasChildNodes():
      comp_nodes = online_node.getElementsByTagName('component')
      if not len(comp_nodes) == 1:
        self._log.error("Modify Rule inconsistent number of online components \n%s" % (node.toxml()))
        return None
      online_component = loadHLTRuleAlgorithmFromXML(comp_nodes[0])
      # Just in case evaluate the properties if strings
      for prop in online_component.properties :
        if isinstance(prop.getValue(), str):
          propValue = eval(prop.getValue())
          prop.value = propValue
      
    # Check for online components
    offline_component = {}
    if offline_node.hasChildNodes():
      comp_nodes = offline_node.getElementsByTagName('component')
      if not len(comp_nodes) == 1:
        self._log.error("Modify Rule inconsistent number of offline components \n%s" % (node.toxml()))
        return None
      offline_component = loadHLTRuleAlgorithmFromXML(comp_nodes[0])
      # Just in case evaluate the properties if strings
      for prop in offline_component.properties :
        if isinstance(prop.getValue(), str):
          propValue = eval(prop.getValue())
          prop.value = propValue
      
    return HLTOfflineRules.ModifyRule(online=online_component, offline=offline_component, OutputLevel=self.OutputLevel)
  
  
  def loadReplaceRulefromXML(self, node) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    rule = {}
    
    online_node = node.getElementsByTagName('online')
    if not len(online_node) == 1:
      self._log.error("Replace Rule inconsistent number of online items \n%s" % (node.toxml()))
      return None
    online_node = online_node[0]
    
    offline_node = node.getElementsByTagName('offline')
    if not len(offline_node) == 1:
      slef._log.error("Replace Rule inconsistent number of offline items \n%s" % (node.toxml()))
      return None
    offline_node = offline_node[0]
    
    # Check for online components
    online_components = []
    if online_node.hasChildNodes():
      comp_nodes = online_node.getElementsByTagName('component')
      for c in comp_nodes :
        online_components.append(self.loadComponentfromXML(c))
      
    # Check for offline components
    offline_components = []
    if offline_node.hasChildNodes():
      comp_nodes = offline_node.getElementsByTagName('component')
      for c in comp_nodes :
        offline_components.append(self.loadComponentfromXML(c))
        
    return HLTOfflineRules.ReplaceRule({'online': online_components, 'offline':offline_components})
  
  ## Load Rename Rule from XML node
  #  @param self OfflineRuleLoader object pointer
  #  @param node XML node
  #  @return RenameRule object or None if not successfull
  def loadRenameRulefromXML(self, node) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    rule = {}
    online_node = node.getElementsByTagName('online')
    if not len(online_node) == 1:
      self._log.error("Rename Rule inconsistent number of online items \n%s" % (node.toxml()))
      return None
    online_node = online_node[0]
    
    offline_node = node.getElementsByTagName('offline')
    if not len(offline_node) == 1:
      self._log.error("Rename Rule inconsistent number of offline items \n%s" % (node.toxml()))
      return None
    offline_node = offline_node[0]
    
    # Check for online components
    online_component = {}
    if online_node.hasChildNodes():
      comp_nodes = online_node.getElementsByTagName('component')
      if not len(comp_nodes) == 1:
        self._log.error("Rename Rule inconsistent number of online components \n%s" % (node.toxml()))
        return None
      
      par_nodes = comp_nodes[0].getElementsByTagName('parameter')
      if len(par_nodes) > 1:
        self._log.error("Rename Rule inconsistent number of online parameters \n%s" % (node.toxml()))
        return None
      
      online_component = loadHLTRuleAlgorithmFromXML(comp_nodes[0])
      
    # Check for online components
    offline_component = {}
    if offline_node.hasChildNodes():
      comp_nodes = offline_node.getElementsByTagName('component')
      if not len(comp_nodes) == 1:
        self._log.error("Rename Rule inconsistent number of offline components \n%s" % (node.toxml()))
        return None
      
      par_nodes = comp_nodes[0].getElementsByTagName('parameter')
      if len(par_nodes) > 1:
        self._log.error("Rename Rule inconsistent number of offline parameters \n%s" % (node.toxml()))
        return None
      
      offline_component = loadHLTRuleAlgorithmFromXML(comp_nodes[0])
    
    return HLTOfflineRules.RenameRule(online=online_component, offline=offline_component, OutputLevel=self.OutputLevel)

  def getRuleSetKey(self, cursor) :
    self.setlog()
    output = -1
    
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    
    # Full query in order to ensure existing connections
    query  = ' SELECT DISTINCT %sHLT_RULE_SET.HRS_ID ' % (prepend)
    query += ' FROM %sSUPER_MASTER_TABLE ' % (prepend)
    query += ' JOIN %sHLT_SMT_TO_HRE ON (%sSUPER_MASTER_TABLE.SMT_ID = %sHLT_SMT_TO_HRE.SMT2RE_SUPER_MASTER_TABLE_ID) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_RELEASE ON (%sHLT_SMT_TO_HRE.SMT2RE_RELEASE_ID = %sHLT_RELEASE.HRE_ID) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_HRE_TO_HRS ON (%sHLT_RELEASE.HRE_ID = %sHLT_HRE_TO_HRS.HRE2RS_RELEASE_ID) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_RULE_SET ON (%sHLT_HRE_TO_HRS.HRE2RS_RULE_SET_ID = %sHLT_RULE_SET.HRS_ID) ' % (prepend, prepend, prepend)
    query += ' WHERE %sSUPER_MASTER_TABLE.SMT_ID=%d' % (prepend, self.SMKey)
    
    # Shortened query - to save time to get info from DB - trust the foreign keys etc.
    query  = ' SELECT DISTINCT %sHLT_HRE_TO_HRS.HRE2RS_RULE_SET_ID ' % (prepend)
    query += ' FROM %sHLT_SMT_TO_HRE ' % (prepend)
    query += ' JOIN %sHLT_HRE_TO_HRS ON (%sHLT_SMT_TO_HRE.SMT2RE_RELEASE_ID = %sHLT_HRE_TO_HRS.HRE2RS_RELEASE_ID) ' % (prepend, prepend, prepend)
    if self.dbType == 'sqlite' :
      query += ' WHERE %sHLT_SMT_TO_HRE.SMT2RE_SUPER_MASTER_TABLE_ID=%d' % (prepend,self.SMKey)
    else :
      query += ' WHERE %sHLT_SMT_TO_HRE.SMT2RE_SUPER_MASTER_TABLE_ID=:smkey' % (prepend)
    
    ruleSetIDs = []
    self._log.verbose('%s, smkey=%d' % (query, self.SMKey))
    if self.dbType == 'sqlite' :
      cursor.execute(query)
    else :
      cursor.execute(query, smkey=self.SMKey)

    result = cursor.fetchall()
    for column in result:
      ruleSetIDs.append(column[0])
    
    if(len(ruleSetIDs) == 1) :
      output = ruleSetIDs[0]
      
    return output
    
  def getRuleIDs(self, ruleSetID, cursor) :
    self.setlog()
    # Get rule ID's sorted accordingly to the rule_counter
    output = []
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    
    # Full query in order to ensure existing connections
    query  = ' SELECT DISTINCT %sHLT_RULE.HRU_ID, %sHLT_HRS_TO_HRU.HRS2RU_RULE_COUNTER ' % (prepend, prepend)
    query += ' FROM %sHLT_RULE_SET ' % (prepend)
    query += ' JOIN %sHLT_HRS_TO_HRU ON (%sHLT_RULE_SET.HRS_ID = %sHLT_HRS_TO_HRU.HRS2RU_RULE_SET_ID) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_RULE ON (%sHLT_HRS_TO_HRU.HRS2RU_RULE_ID = %sHLT_RULE.HRU_ID) ' % (prepend, prepend, prepend)
    query += ' WHERE %sHLT_RULE_SET.HRS_ID=%d' % (prepend, ruleSetID)
    query += ' ORDER %sHLT_HRS_TO_HRU.HRS2RU_RULE_COUNTER ' % (prepend)
    
    # Shortened query - to save time to get info from DB - trust the foreign keys etc.
    query  = ' SELECT DISTINCT %sHLT_HRS_TO_HRU.HRS2RU_RULE_ID, %sHLT_HRS_TO_HRU.HRS2RU_RULE_COUNTER ' % (prepend, prepend)
    query += ' FROM %sHLT_HRS_TO_HRU ' % (prepend)
    if self.dbType == 'sqlite' :
      query += ' WHERE %sHLT_HRS_TO_HRU.HRS2RU_RULE_SET_ID=%d' % (prepend,ruleSetID)
    else :
      query += ' WHERE %sHLT_HRS_TO_HRU.HRS2RU_RULE_SET_ID=:ruleset_id' % (prepend)
    query += ' ORDER BY %sHLT_HRS_TO_HRU.HRS2RU_RULE_COUNTER ' % (prepend)
    
    self._log.verbose('%s, ruleset_id=%d' % (query, ruleSetID))
    if self.dbType == 'sqlite' :
      cursor.execute(query)
    else:
      cursor.execute(query, ruleset_id=ruleSetID)
    result = cursor.fetchall()
    for column in result:
      output.append(column[0])
    
    return output
    
  def getComponentIDs(self, ruleID, cursor, componentType=-1) :
    self.setlog()
    self._log.verbose("getComponentIDs -- start\n Component type: %d" % (componentType))
    output = []
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    # Full query in order to ensure existing connections
    query  = ' SELECT DISTINCT %sHLT_RULE_COMPONENT.HRC_ID ' % (prepend)
    query += ' FROM %sHLT_RULE ' % (prepend)
    query += ' JOIN %sHLT_HRU_TO_HRC ON (%sHLT_RULE.HRU_ID = %sHLT_HRU_TO_HRC.HRU2RC_RULE_ID) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_COMPONENT ON (%sHLT_HRU_TO_HRC.HRU2RC_COMPONENT_ID = %sHLT_COMPONENT.HRC_ID) ' % (prepend, prepend, prepend)
    query += ' WHERE %sHLT_RULE.HRU_ID=%d' % (prepend, ruleID)
    if componentType >=0 :
        query += ' AND %sHLT_HRU_TO_HRC.HRU2RC_COMPONENT_TYPE=%d' % (prepend, componentType)
        
    # Shortened query - to save time to get info from DB - trust the foreign keys etc.
    query  = " SELECT DISTINCT %sHLT_HRU_TO_HRC.HRU2RC_COMPONENT_ID " % (prepend)
    query += " FROM %sHLT_HRU_TO_HRC " % (prepend)
    if self.dbType == 'sqlite' :
      query += " WHERE %sHLT_HRU_TO_HRC.HRU2RC_RULE_ID=%d " % (ruleID)
    else :
      query += " WHERE %sHLT_HRU_TO_HRC.HRU2RC_RULE_ID=:rule_id " % (prepend)
    if componentType >=0 :
      if self.dbType == 'sqlite' :
        query += ' AND %sHLT_HRU_TO_HRC.HRU2RC_COMPONENT_TYPE=%d' % (prepend,componentType)
      else :
        query += ' AND %sHLT_HRU_TO_HRC.HRU2RC_COMPONENT_TYPE=:component_type' % (prepend)
    
    if componentType >=0 :
      self._log.verbose('%s, rule_id=%d, component_type=%d' % (query,ruleID, componentType))
      if self.dbType == 'sqlite' :
        cursor.execute(query)
      else :
        cursor.execute(query, rule_id=ruleID, component_type=componentType)
    else :
      self._log.verbose('%s, rule_id=%d' % (query,ruleID))
      if self.dbType == 'sqlite' :
        cursor.execute(query)
      else :
        cursor.execute(query, rule_id=ruleID)
    result = cursor.fetchall()
    for column in result:
      output.append(column[0])
    return output
  
  def getOfflineComponentIDs(self, ruleID, cursor) :
    self.setlog()
    componentType=2
    return self.getComponentIDs(ruleID,cursor,componentType)
    
  def getOnlineComponentIDs(self, ruleID, cursor) :
    self.setlog()
    componentType=1
    return self.getComponentIDs(ruleID,cursor,componentType)
  
  def getParameterIDs(self, componentID, cursor) :
    self.setlog()
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    output = []
    # Full query in order to ensure existing connections
    query  = ' SELECT DISTINCT %sHLT_RULE_PARAMETER.HRP_ID ' % (prepend)
    query += ' FROM %sHLT_RULE_COMPONENT ' % (prepend)
    query += ' JOIN %sHLT_HRC_TO_HRP ON (%sHLT_RULE_COMPONENT.HRC_ID = %sHLT_HRC_TO_HRP.HRC2RP_COMPONENT_ID) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_PARAMETER ON (%sHLT_HRC_TO_HRP.HRC2RP_PARAMETER_ID = %sHLT_PARAMETER.HRP_ID) ' % (prepend, prepend, prepend)
    query += ' WHERE %sHLT_RULE_COMPONENT.HRC_ID=%d' % (prepend, componentID)
    
    # Shortened query - to save time to get info from DB - trust the foreign keys etc.
    query  = " SELECT DISTINCT %sHLT_HRC_TO_HRP.HRC2RP_PARAMETER_ID " % (prepend)
    query += " FROM %sHLT_HRC_TO_HRP " % (prepend)
    if self.dbType == 'sqlite' :
      query += " WHERE %sHLT_HRC_TO_HRP.HRC2RP_COMPONENT_ID=%d " % (prepend,componentID)
    else :
      query += " WHERE %sHLT_HRC_TO_HRP.HRC2RP_COMPONENT_ID=:component_id " % (prepend)
    
    self._log.verbose('%s, component_id=%d' % (query, componentID))
    if self.dbType == 'sqlite' :
      cursor.execute(query)
    else :
      cursor.execute(query, component_id=componentID)
    result = cursor.fetchall()
    for column in result:
      output.append(column[0])
    
    self._log.debug("%d parameter IDs found" % (len(output)))
    return output


  def loadParameterFromDb(self, parameterID, cursor) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    parameter = {}
    name = ""
    value = ""
    
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
      
    query  = " SELECT DISTINCT %sHLT_RULE_PARAMETER.HRP_NAME, %sHLT_RULE_PARAMETER.HRP_VALUE, %sHLT_RULE_PARAMETER.HRP_OP " % (prepend, prepend, prepend)
    query += " FROM %sHLT_RULE_PARAMETER " % (prepend)
    if self.dbType == 'sqlite':
      query += " WHERE %sHLT_RULE_PARAMETER.HRP_ID=%d " % (prepend,parameterID)
    else: 
      query += " WHERE %sHLT_RULE_PARAMETER.HRP_ID=:parameter_id " % (prepend)
    
    self._log.verbose('%s, parameter_id=%d' % (query, parameterID))
    if self.dbType == 'sqlite':
      cursor.execute(query)
    else:
      cursor.execute(query, parameter_id=parameterID)
    result = cursor.fetchall()
    for column in result:
      value = column[1]
      name = column[0]
      # Format the value for oracle DB empty string
      #if self.dbType == 'oracle' and '~' in (value, name):
      if '~' in (value, name):
        if value == '~' : value = ''
        if name == '~' : name = ''
      try : # Evaluate value if possible
        value = eval(value)
      except : # Nothing happened just pass the value as string
        pass
      parameter[name] = value
      
    if len(parameter) >1 :
      self._log.error("DB incosistency multiple parameters with ID %d found" % (componentID))
      
    self._log.debug('Parameter ID %d - %s loaded from DB' % (parameterID, parameter))
    
    return parameter
  
  
  def loadComponentFromDb(self, componentID, cursor) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    component = {}
    name = ""
    alias = ""
    
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
      
    query  = " SELECT DISTINCT %sHLT_RULE_COMPONENT.HRC_NAME, %sHLT_RULE_COMPONENT.HRC_ALIAS " % (prepend, prepend)
    query += " %sHLT_RULE_COMPONENT.HRC_TYPE, %sHLT_RULE_COMPONENT.HRC_PY_NAME, %sHLT_RULE_COMPONENT.HRC_PY_PACKAGE " % (prepend, prepend, prepend)
    query += " FROM %sHLT_RULE_COMPONENT " % (prepend)
    if self.dbType == 'sqlite' :
      query += " WHERE %sHLT_RULE_COMPONENT.HRC_ID=%d " % (prepend,componentID)
    else:
      query += " WHERE %sHLT_RULE_COMPONENT.HRC_ID=:comp_id " % (prepend)
    self._log.verbose('%s, comp_id=%d' % (query,componentID))
    if self.dbType == 'sqlite' :
      cursor.execute(query)
    else :
      cursor.execute(query, comp_id=componentID)
    result = cursor.fetchall()
    ncomponents = 0
    for column in result:
      name = column[0]
      alias = column[1]
      alg_type = column[2]
      py_name = column[3]
      py_package = column[4]
      # Correct oracle empty string
      #if self.dbType == 'oracle' and '~' in (name,alias,alg_type,py_name,py_package):
      if '~' in (name,alias,alg_type,py_name,py_package):
        if name=='~' : name=''
        if alias=='~' : alias=''
        if alg_type=='~' : name=''
        if py_name=='~' : py_name=''
        if py_package=='~' : py_package=''
      
      component = {'alias': alias, 'name': name}
      if alg_type !='': component['type']=alg_type
      if py_name !='': component['py_name']=py_name
      if py_package !='': component['py_package']=py_package
      
      ncomponents += 1
    
    self._log.debug("Component ID %d - %s/%s loaded from DB" % (componentID, name, alias))
    
    if ncomponents >1 :
      self._log.error("DB incosistency multiple components with ID %d found" % (componentID))
    
    # Get parameter ID's
    parameterIDs = self.getParameterIDs(componentID, cursor)
    
    if len(parameterIDs) > 0 :
      component.setdefault('properties',{})
      
    for parID in parameterIDs :
      parameter = self.loadParameterFromDb(parID, cursor)
      component['properties'].update(parameter)
      
    return component
  
  def loadRuleFromDb(self, ruleID, cursor) :
    self.setlog()
    import TrigConfOffline.HLTOfflineRules as HLTOfflineRules
    rules = None
    ruleName=""
    ruleType=0
    
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
      
    query  = ' SELECT DISTINCT %sHLT_RULE.HRU_NAME,  %sHLT_RULE.HRU_TYPE ' % (prepend, prepend)
    query += ' FROM %sHLT_RULE ' % (prepend)
    if self.dbType == 'sqlite' :
      query += ' WHERE %sHLT_RULE.HRU_ID=%d' % (prepend,ruleID)
    else :
      query += ' WHERE %sHLT_RULE.HRU_ID=:rule_id' % (prepend)
    self._log.verbose('%s, rule_id=%d' % (query,ruleID))
    
    if self.dbType == 'sqlite' :
      cursor.execute(query)
    else :
      cursor.execute(query,rule_id=ruleID)
    result = cursor.fetchall()
    nrules = 0
    for column in result:
      ruleName = column[0]
      ruleType = column[1]
      #if self.dbType == 'oracle' and '~' in (ruleName, ruleType) :
      if '~' in (ruleName, ruleType) :
        if ruleName=='~' : ruleName=''
        if ruleType=='~' : ruleType=''
      nrules += 1
    
    if nrules > 1 :
      self._log.error("DB incosistency multiple rules with ID %d found" % (ruleID))
      return rules
    if nrules == 0:
      return rules
    
    online_components  = []
    for componentID in self.getOnlineComponentIDs(ruleID, cursor) :
      online_components.append(self.loadComponentFromDb(componentID, cursor))
    offline_components  = []
    for componentID in self.getOfflineComponentIDs(ruleID, cursor) :
      offline_components.append(self.loadComponentFromDb(componentID, cursor))
    
    if   ruleType==1 : # Replace rule
      return HLTOfflineRules.ReplaceRule({'online': online_components, 'offline':offline_components})
    elif ruleType==2 : # Rename rule
      if len(online_components) == 1 and len(offline_components) == 1 :
        return HLTOfflineRules.RenameRule({'online': online_components[0], 'offline':offline_components[0]})
      else:
        self._log.error("Rule ID %d: Inconsistent number of online/offline components  %d/%d" % (ruleID,len(online_components),len(offline_components)))
    elif ruleType==3 : # Modify rule
      if len(online_components) == 1 and len(offline_components) == 1 :
        return HLTOfflineRules.ModifyRule({'online': online_components[0], 'offline':offline_components[0]})
      else :
        self._log.error("Rule ID %d: Inconsistent number of online/offline components  %d/%d" % (ruleID,len(online_components),len(offline_components)))
    elif ruleType==6 : # Copy rule
      if len(online_component) == 1 and len(offline_component) == 1 :
        return HLTOfflineRules.CopyRule({'online': online_components[0], 'offline':offline_components[0]})
      else :
        self._log.error("Rule ID %d: Inconsistent number of online/offline components  %d/%d" % (ruleID,len(online_components),len(offline_components)))
    elif ruleType==4 : # Merge rule
      rules = []
      # Split merge rule to single Merge rules
      for componentID in self.getComponentIDs(ruleID, cursor) :
        rules.append(HLTOfflineRules.MergeRule({'component': self.loadComponentFromDb(componentID,cursor)}))
      return rules
    elif ruleType==5 : # Sort rule
      rules = []
      for component in online_components :
        rules.append(HLTOfflineRules.SortRule({'sortOrder':'end','component': component}))
      for component in offline_components :
        rules.append(HLTOfflineRules.SortRule({'sortOrder':'start','component': component}))
      return rules
    else: # Unsupported rule type
      self._log.error("Rule ID %d: Unsupported type %d" % (ruleType))
    
    return rules
    
## Load HLTRuleAlgorithm from xml node
#  @param node XML component node
#  @param logger Message logger - default None - will be created
def loadHLTRuleAlgorithmFromXML(node, logger=None):
  # Set logger if non existing
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTOfflineRuleLoader.loadHLTRuleAlgorithmfromXML')
  
  from TrigConfOffline.HLTOfflineRules import HLTRuleAlgorithm
  hltRuleAlgorithm = HLTRuleAlgorithm(OutputLevel=logger.getEffectiveLevel())
  # Set the alias of the algorithm if exists
  if node.hasAttribute('alias') :
    hltRuleAlgorithm.alias = str(node.getAttribute('alias'))
  else:
    logger.warning('Component has no attribute alias %s' % (node.toxml()))

  # Set the name of the algorithm if exists
  if node.hasAttribute('name') :
    hltRuleAlgorithm.name  = str(node.getAttribute('name'))
  else :
    logger.warning('Component has no attribute alias %s' % (node.toxml()))
  
  # Set the py_package and py_name if exist
  if node.hasAttribute('py_name'):  hltRuleAlgorithm.py_name = str(node.getAttribute('py_name'))
  if node.hasAttribute('py_package'): hltRuleAlgorithm.py_package = str(node.getAttribute('py_package'))
  
  # Set the type of the algorithm  - Here I can treat regular expression or rather than the single types
  if node.hasAttribute('topalg') and str(node.getAttribute('topalg'))=="1": hltRuleAlgorithm.alg_type='TopAlg'
  if node.hasAttribute('svc') and str(node.getAttribute('svc'))=="1": hltRuleAlgorithm.alg_type='Service'
  if node.hasAttribute('pubT') and str(node.getAttribute('pubT'))=="1": hltRuleAlgorithm.alg_type='PublicTool'
  if node.hasAttribute('privT') and str(node.getAttribute('privT'))=="1": hltRuleAlgorithm.alg_type='PrivateTool'
  if node.hasAttribute('aud') and str(node.getAttribute('aud'))=="1": hltRuleAlgorithm.alg_type='Auditor'
  if node.hasAttribute('steeralg') and str(node.getAttribute('steeralg'))=="1": hltRuleAlgorithm.alg_type='SteerAlg'
  
  
  if node.hasChildNodes() :
    doc_pars = node.getElementsByTagName('parameter')
    for p in doc_pars :
      hltRuleAlgorithm.setProperty(loadHLTRulePropertyFromXML(p,logger))
  return hltRuleAlgorithm

## Load HLTRuleProperty from xml node
#  @param node XML component node
#  @param logger Message logger - default None - will be created
def loadHLTRulePropertyFromXML(node, logger=None):
  # Set logger if non existing
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTOfflineRuleLoader.loadHLTRulePropertyFromXML')
  
  from TrigConfOffline.HLTOfflineRules import HLTRuleProperty
  hltRuleProperty = HLTRuleProperty()
  
  if node.hasAttribute('name') :
    hltRuleProperty.name = str(node.getAttribute('name'))
  else:
    self._log.warning('Parameter has no attribute name %s' % (node.toxml()))
  
  if node.hasAttribute('value') :
    value = str(node.getAttribute('value'))
    try:
      value = eval(value)
    except :
      # Nothing happened just pass the value as string
      pass
    hltRuleProperty.value = value
  else:
    self._log.warning('Parameter has no attribute value %s' % (node.toxml()))

  return hltRuleProperty
