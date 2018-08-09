###########################################################################
#                                          
#    <nozicka@mail.desy.de>                                                             
#
#
###########################################################################
doNotSetPropertiesCfgName = ['EventSelector']

class HLTConfOfflineError(Exception):
  pass

## Main Class of @package
#  Basic usage: 
#  from TrigConfOffline.HLTConfOffline import HLTConfOffline
#  hltConfOffline = HLTConfOffline()
#  hltConfOffline.SMKey = 73
#  htConfOffline.load()
class HLTConfOffline(object) :
  ## @var List of supported attributes 
  #  
  _properties = ('dbType','dbHost','dbUser','dbPasswd','dbName',
  'setupSource','rulesSource', 'OutputLevel',
  'SMKey','HLTPrescaleKey','LVL1PrescaleKey','LVL1BunchGroupKey','Level','RunNumber')
  
  ## Initialize the HLTConOffline service
  def __init__(self, *args, **kwargs) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTConfOffline')
    self._log.setLevel(3)
    ## @var setupSource 
    #  Source of the TrigConfiguration
    #  supported values: DBLookup ALIAS, Connection string, db
    self.__dict__['setupSource'] = 'TRIGGERDB'

    ## @var dbType
    #  Database type 
    #  Supported values: oracle, mysql, sqlite
    self.dbType = ''
    ## @var dbHost
    #  Database server
    self.dbHost = ''
    ## @var dbUser
    # Database login
    self.dbUser = ''
    ## @var dbPasswd 
    # Database user password
    self.dbPasswd = ''
    ## @var dbName 
    #  Database schema name
    self.dbName = ''

    ## @var rulesSource Source of rules
    #  Supported values: xml file , db
    self.rulesSource = 'offline_setup_rules.xml'

    ## @var SMKey
    #  Super Master Key 
    self.SMKey = -1
    ## @var HLTPrescaleKey
    #  HLT Prescale Key
    self.HLTPrescaleKey = -1
    ## @var LVL1PrescaleKey 
    #  LVL1 Prescale Key
    self.LVL1PrescaleKey = -1
    self.LVL1BunchGroupKey = -1
    
    ## @var Level Trigger Level
    #  Supported values BOTH, L2, EF, OL
    self.Level = 'BOTH'
    ## @var RunNumber
    #  Run number resp. lumi block
    self.RunNumber = None
    
    ## @var OutputLevel
    #  Logger OutputLevel
    self.OutputLevel = 3

    for k,v in kwargs.items() :
      self.__setattr__(k,v)
      
  ## Set logger
  def setlog(self) :
    if self.__dict__.has_key('OutputLevel'):
      self._log.setLevel(self.OutputLevel)
    else:
      self._log.setLevel(3)
    self._log.name = 'HLTConfOffline '
  
  ## Set attributes of the class.
  #  Special treatment for RunNumber, setupSource and OutputLevel
  def __setattr__(self, name, value) :
    self.setlog()
    #_properties = ('dbType','dbHost','dbUser','dbPaswd','dbName','setupSource','rulesSource','SMKey','HLTPre','LVL1Pre','Level','RunNumber')
    if name in self._properties:
      self._log.debug('Set property: %s = %s' % (name, value))
      if name=='RunNumber':
        if isinstance(value,int): value='%d/0' % (value)
      
      if name=='setupSource':
        value =  self._setSetupSource(value)
      
      if name=='OutputLevel':
        self._log.setLevel(value)
      # After all the exceptions set the value
      self.__dict__[name] = value
    else:
      self._log.warning('Unknown property: %s = %s  - property not set' % (name, value))
  
  ## Convert to string for printout
  def __str__(self):
    self.setlog()
    output = "-"*80 + "\n"
    string = "%s.%s" % (self.__module__,'HLTConfOffline')
    output += "|" + " "*(39 - len(string)/2) + string +" "*(39 - len(string)/2) + "|\n"
    output += "-"*80 + "\n"
    
    for k,v in self.__dict__.items() :
      if k[0] == '_' :
        # Skip the private properties
        continue
      elif k == 'dbPasswd' :
        output += "\t%s = %s\n" % (k,"*"*len(v))
      elif k in self._properties  :
        output += "\t%s = %s\n" % (k,v)
        
    return output
  
  ## Process the connnection string 
  #  @param connectionString Connection string for details see TrigConfigSvc.TrigConfigSvcUtils.py
  def _setSetupSource(self, connectionString) :
    output = connectionString
    #self.setlog()
    self._log.verbose("_setSetupSource(%s)" % (connectionString))
    
    if connectionString != 'db' :
      connectionParameters = interpretConnectionString(connectionString)
      if connectionParameters.has_key('techno') :self.dbType = connectionParameters['techno']
      if connectionParameters.has_key('server'): self.dbHost = connectionParameters['server']
      if connectionParameters.has_key('schema'): self.dbName = connectionParameters['schema']
      if connectionParameters.has_key('user'): self.dbUser = connectionParameters['user']
      if connectionParameters.has_key('passwd'): self.dbPasswd = connectionParameters['passwd']
      if self.dbType == 'sqlite' and connectionParameters.has_key('filename'): self.dbHost = connectionParameters['filename']
    return connectionString
        
  ## Check the consistency of the data members
  def _consistencyCheck(self) :
    self.setlog()
    self._log.verbose('Consistency check')
    if not (self.__dict__.has_key('setupSource') and self.setupSource):
      # Test the existence of neccesairy parameters:
      dbParComplete = True
      if dbParComplete and (not (self.__dict__.has_key('dbType') and self.dbType)) : dbParComplete = False
      if dbParComplete and (not (self.__dict__.has_key('dbHost') and self.dbHost)) : dbParComplete = False
      
      # If there is a type of DB sqlite already enough parameters are present
      if not (dbParComplete and self.dbType=='sqlite') :
        if dbParComplete and not (self.__dict__.has_key('dbUser') and self.dbUser) : dbParComplete = False
        if dbParComplete and not (self.__dict__.has_key('dbPasswd') and self.dbPasswd) : dbParComplete = False
        if dbParComplete and self.dbType!='oracle' :
          if dbParComplete and not (self.__dict__.has_key('dbName') and self.dbName) : dbParComplete = False
      
      if not dbParComplete:
        self._log.fatal('No trigger setup source')
        raise HLTConfOfflineError('No trigger setup source')
      else :
        self._log.warning('setupSource previously not set')
        self.setupSource = 'db'
    
    # If rules source doesn't exist set default
    if not self.__dict__.has_key('rulesSource') :
      self.rulesSource='offline_setup_rules.xml'
      #self._log.fatal('No rules source')
      #raise HLTConfOfflineError('No rules source')
    
    # Check the manual setup of the DB parameters - try to be idiot resistant
    if self.setupSource == 'db' :
      if not (self.__dict__.has_key('dbType') and self.dbType) :
        self._log.fatal('No DB Type specified')
        raise HLTConfOfflineError('Inconsistent DB parameters')
      
      if not (self.__dict__.has_key('dbHost') and self.dbHost) :
        self._log.fatal('No DB Host or sqlite file specified')
        raise HLTConfOfflineError('Inconsistent DB parameters')
      
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbUser') and self.dbUser) :
        self._log.fatal('No DB User specified')
        raise HLTConfOfflineError('Inconsistent DB parameters')
      
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbPasswd') and self.dbPasswd) :
        self._log.fatal('No DB Password specified')
        raise HLTConfOfflineError('Inconsistent DB parameters')
      
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbName') and self.dbName) :
        if self.dbType=='oracle' :
          self.dbName = self.dbUser
        else :
          self._log.fatal('No DB Name specified')
          raise HLTConfOfflineError('Inconsistent DB parameters')
    else : # Should not appear - but just in case user gets here 
      if not (self.__dict__.has_key('dbType') and self.dbType) : self._setSetupSource(self.setupSource)
      if not (self.__dict__.has_key('dbHost') and self.dbHost) : self._setSetupSource(self.setupSource)
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbUser') and self.dbUser) : self._setSetupSource(self.setupSource)
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbPasswd') and self.dbPasswd) : self._setSetupSource(self.setupSource)
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbName') and self.dbName) : self._setSetupSource(self.setupSource)
    
    # Check the Run number and get the Trigger Keys
    if self.__dict__.has_key('RunNumber') and self.RunNumber:
      self._getDBKeysFromCool()
    
    # Check the Trigger Keys
    if not self.__dict__.has_key('SMKey') or not self.SMKey or self.SMKey <= 0 :
      self._log.fatal('Wrong SMKey %s' % (self.SMKey))
      raise HLTConfOfflineError('Wrong SMKey %s' % (self.SMKey))
    
    if not self._checkLVL1PrescaleKey() :
      self._log.fatal('Wrong LVL1 Prescale Key %s' % (self.LVL1PrescaleKey))
      #raise HLTConfOfflineError('Inconsistent LVL1PrescaleKey')
      
    if not self._checkHLTPrescaleKey() :
      self._log.fatal('Wrong HLT Prescale Key %s' % (self.HLTPrescaleKey))
      #raise HLTConfOfflineError('Inconsistent HLTPrescaleKey')
  
  ## Get Trigger Keys from the COOL 
  #  @param self Pointer to the object
  #  Use the run number and lumi block information
  def _getDBKeysFromCool(self) :
    self.setlog()
    self._log.debug('Get DB Keys from Cool: Run/Lumi Block %s' % self.RunNumber)
    runNumber=long(self.RunNumber.split('/')[0])
    lumiBlock=0
    if len(self.RunNumber.split('/')) > 1: lumiBlock=long(self.RunNumber.split('/')[1])
    
    self._log.verbose('Run: %d (%s)' % (runNumber, type(runNumber)))
    self._log.verbose('Lumi Block: %d (%s)' % (lumiBlock, type(lumiBlock)))
    
    from CoolConvUtilities import AtlCoolTool
    dbconn='COOLONL_TRIGGER/COMP200'
    self._log.verbose('DB connection %s' % dbconn)
    cooltool = AtlCoolTool.AtlCoolTool(dbconn)
    if(str(cooltool)=="Not connected"):
      self._log.ERROR('COOL not connected')
      return False
    
    self._log.verbose(cooltool)
    
    from PyCool import cool
    limmin = cool.ValidityKeyMin
    limmax = cool.ValidityKeyMax
    limmin=(runNumber << 32)+lumiBlock
    limmax=limmin+1
    self._log.verbose('IOV: %d - %d' % (limmin, limmax))

    folder = cooltool.db.getFolder( "/TRIGGER/HLT/HltConfigKeys" )
    chansel=cool.ChannelSelection(0)
    objs = folder.browseObjects( limmin,limmax,chansel)

    configKeys = {}
    while objs.hasNext():
      obj=objs.next()
      runNr   = obj.since()>>32
      if runNr>1000000: continue
      lumiBl = obj.since() & 0xFFFFFFFF
      #runNr='%d/%d' % (runNr,lumiBl)
      payload=obj.payload()
      smk     = payload['MasterConfigurationKey']
      hltpsk  = payload['HltPrescaleConfigurationKey']
      confsrc = payload['ConfigSource'].split(',')
      release = 'not known'
      if len(confsrc)>1:
        release = confsrc[1]
      configKeys[runNr] = { "REL"     : release,
                            "SMK"     : smk,
                            "HLTPSK"  : hltpsk,
                            "LVL1PSK" : []}
    
    folder = cooltool.db.getFolder( "/TRIGGER/LVL1/Lvl1ConfigKey" )
    objs = folder.browseObjects(limmin,limmax,chansel)

    while objs.hasNext():
      obj=objs.next()
      runNr  = obj.since()>>32
      if runNr>1000000: continue
      lumiBl = obj.since() & 0xFFFFFFFF
      #runNr='%d/%d' % (runNr,lumiBl)
      payload=obj.payload()
      l1psk = payload['Lvl1PrescaleConfigurationKey']
      if configKeys.has_key(runNr):
        configKeys[runNr]["LVL1PSK"] += [l1psk]
      else:
        configKeys[runNr] = { "REL"     : "-",
                              "SMK"     : '-',
                              "HLTPSK"  : '-',
                              "LVL1PSK" : [l1psk]}
    
    self._log.debug('Found Keys: %s' % (configKeys))
    if configKeys.has_key(runNumber) :
      self.SMKey = configKeys[runNumber]['SMK']
      self.HLTPrescaleKey = configKeys[runNumber]['HLTPSK']
      if len(configKeys[runNumber]['LVL1PSK']) > 1 :
        self._log.warning('Multiple L1 prescale keys found (%s) - assigning first' % (configKeys[runNumber]['LVL1PSK']))
      self.LVL1PrescaleKey = configKeys[runNumber]['LVL1PSK'][0]
    else :
      self._log.error('No trigger keys in COOL')
    
    return True
  
  ## Check the consistency of LVL1 Prescale and Super Master keys
  def _checkLVL1PrescaleKey(self) :
    #self.setlog()
    if self.setupSource.lower().endswith('.xml'):
      return True
    cursor = self.connectDB()
    prescaleKeys = self.getLVL1PrescaleKeys(cursor)
    self._log.debug("SMKey=%d - available LVL1 prescale keys: %s" % (self.SMKey, prescaleKeys))
    cursor.close()
    check = False
    
    if self.__dict__.has_key('LVL1PrescaleKey'):
      if self.LVL1PrescaleKey and self.LVL1PrescaleKey > 0:
        if self.LVL1PrescaleKey not in prescaleKeys :
          self._log.error('Wrong LVL1PrescaleKey %d: available keys for SMKey=%d : %s)' % (self.LVL1PrescaleKey, self.SMKey, prescaleKeys))
          #raise HLTConfOfflineError('Wrong LVL1PrescaleKey: available keys for SMKey=%d : %s)' % (self.SMKey, prescaleKeys))
        else :
          check = True 
    
    # No LVL1 Prescale Key was set
    if not check:
      if len(prescaleKeys) == 1:
        self.LVL1PrescaleKey = prescaleKeys[0]
        self._log.info('LVL1PrescaleKey set to %d' % (self.LVL1PrescaleKey))
        check = True
      else:
        self._log.error('No LVL1PrescaleKey set: available keys for SMKey=%d : %s)' % (self.SMKey, prescaleKeys))
        #raise HLTConfOfflineError('No LVL1PrescaleKey set: available keys for SMKey=%d : %s)' % (self.SMKey, prescaleKeys))
      
    return check
  
  ## Check the consistency of HLT Prescale and Super Master keys
  def _checkHLTPrescaleKey(self) :
    #self.setlog()
    if self.setupSource.lower().endswith('.xml'):
      return True
    cursor = self.connectDB()
    prescaleKeys = self.getHLTPrescaleKeys(cursor)
    self._log.debug("SMKey=%d - available HLT prescale keys: %s" % (self.SMKey, prescaleKeys))
    check = False
    
    if self.__dict__.has_key('HLTPrescaleKey'):
      if self.HLTPrescaleKey and self.HLTPrescaleKey > 0:
        if self.HLTPrescaleKey not in prescaleKeys :
          raise HLTConfOfflineError('Wrong HLTPrescaleKey: available keys for SMKey=%d : %s)' % (self.SMKey, prescaleKeys))
        else :
         check = True 
    
    # No HLT Prescale Key was set
    if not check:
      if len(prescaleKeys) == 1:
        self.HLTPrescaleKey = prescaleKeys[0]
        check = True
      else:
        raise HLTConfOfflineError('No HLTPrescaleKey set: available keys for SMKey=%d : %s)' % (self.SMKey, prescaleKeys))
      
    return check
  
  ## Load the Trigger Configuration setup
  def loadSetup(self) :
    self.setlog()
    import time
    start = time.time()
    
    import TrigConfOffline.HLTSetupLoader as HLTSetupLoader
    hltsetuploader = HLTSetupLoader.SetupLoader(Source=self.setupSource,
                                                dbType = self.dbType,
                                                dbHost = self.dbHost,
                                                dbUser = self.dbUser,
                                                dbName = self.dbName,
                                                dbPasswd = self.dbPasswd,
                                                SMKey = self.SMKey,
                                                Level = self.Level,
                                                OutputLevel = self.OutputLevel)
    
    self._log.info('\n%s' % (hltsetuploader))
    
    startAlg = time.time()
    algorithms = hltsetuploader.load()
    
    self._log.info('%d algorithms loaded in %ds' % (len(algorithms),time.time()-startAlg))
    
    # Check the prescale keys
    self._checkHLTPrescaleKey()
    self._checkLVL1PrescaleKey()
    
    for alg in algorithms :
      #Exception for the L1/HLTConfigSvc - set the connection to the same DB
      if (alg.getName(), alg.getAlias()) in (('TrigConf::HLTConfigSvc','HLTConfigSvc'),('TrigConf::LVL1ConfigSvc','LVL1ConfigSvc')):
      #if (alg_name, alg_alias) == ('TrigConf::HLTConfigSvc','HLTConfigSvc')
        self._log.info('Modify %s' % (alg))
        cfg_source = self.setupSource
        
        alg.setProperty('ConfigSource', self.dbType)
        alg.setProperty('DBUser',self.dbUser)
        alg.setProperty('DBServer',self.dbHost)
        alg.setProperty('DBAccount',self.dbName)
        alg.setProperty('DBSMKey',self.SMKey)
        
        if (alg.getName(), alg.getAlias()) == ('TrigConf::HLTConfigSvc','HLTConfigSvc'):
          alg.setProperty('DBPassword',self.dbPasswd)
          alg.setProperty('DBTable',self.dbName)
          alg.setProperty('XMLMenuFile','')
          if self.HLTPrescaleKey:
            alg.setProperty('DBHLTPSKey',self.HLTPrescaleKey)
      
        if (alg.getName(), alg.getAlias()) == ('TrigConf::LVL1ConfigSvc','LVL1ConfigSvc'):
          alg.setProperty('DBPass',self.dbPasswd)
          alg.setProperty('XMLFile','')
          if self.LVL1PrescaleKey:
            alg.setProperty('DBLVL1PSKey',self.LVL1PrescaleKey)
        
        self._log.info('Modified %s' % (alg))
        
      # Exception for the IOVDbSvc - disentangle the ConvertHLTSetup_txt2xml.py formating
      #if (alg.getName(), alg.getAlias()) == ('IOVDbSvc','IOVDbSvc') :
        #self._log.info('Modify %s' % (alg))
        #max_folder = alg.getProperties()['dbConnection']
        #folders = alg.getProperties()['Folders']
        #alg.setProperty('Folders' ,[])
        #for f in folders :
          #if '<db>' not in f [:5] :
            #f = '<db>%s</db>%s' % (max_folder,f)
          #alg.getProperties()['Folders'].append(f)
        #self._log.info('Modified %s' % (alg))
    
    # Print out verbose message
    if self.OutputLevel <= 1:
      self._log.verbose('Algorithms before application of rules:')
      for i in xrange(len(algorithms)):
        self._log.verbose('\n%d\t%s' % (i, algorithms[i]))
    
    # Load and apply the rules
    if self.rulesSource not in ('none','') :
      startRules = time.time()
      import TrigConfOffline.HLTOfflineRuleLoader as HLTOfflineRuleLoader
      ruleloader = None
      if self.rulesSource == "db":
        ruleloader = HLTOfflineRuleLoader.OfflineRuleLoader(Source=self.rulesSource,
                                                            dbType = self.dbType,
                                                            dbHost = self.dbHost,
                                                            dbUser = self.dbUser,
                                                            dbName = self.dbName,
                                                            dbPasswd = self.dbPasswd,
                                                            SMKey = self.SMKey,
                                                            OutputLevel=self.OutputLevel )
      elif self.rulesSource:
        ruleloader = HLTOfflineRuleLoader.OfflineRuleLoader(Source=self.rulesSource,OutputLevel=self.OutputLevel)
      
      if ruleloader :
        self._log.debug("\n%s" % (ruleloader))
        rules = ruleloader.load()
        
        self._log.info('Rules loaded in %ds' % (time.time()-startRules))
        startRules = time.time()
        nmod = 0
        
        for r in rules:
          nmod += r.apply(algorithms, self.OutputLevel)
        
        self._log.info('Offline to Online setup conversion in %ds' % (time.time()-startRules) )
        #for r in rules:
          #if r._nactive == 0 :
            #self._log.debug('Rule %s was not applied' % (r))
            
        if self.OutputLevel <= 1:
          self._log.verbose('Algorithms after application of rules:')
          for i in xrange(len(algorithms)):
            self._log.verbose('\n%d\t%s' % (i, algorithms[i]))
            
      else:
        self._log.warning('No offline rules were loaded - continue with unmodified algorithms')
    
    stop = time.time()
    self._log.info('HLT setup loaded in %ds' % (stop-start))
    
    return algorithms
  
  ## Merge algorithms with the same alias
  #  @param algs List of algorithms
  def mergeAlg(self, algs) :
    self.setlog()
    nmod = 0
    if isinstance(algs, list) :
      # Loop over parent algorithms - merge them if needed
      i = 0
      while i < len(algs):
        firstLog = True
        # Loop over remaining parent algorithms
        ii = i+1
        while ii < len(algs) :
          # Fix the unknowns if possible
          if (algs[ii].getAlias() == algs[i].getAlias()) and (algs[i].getName().lower() in ('unknown','') or algs[ii].getName().lower() in ('unknown','')) :
            if algs[i].getName().lower() in ('unknown','') and algs[ii].getName().lower() not in ('unknown','') :
              self._log.debug('Fix the algorithm %s name %s to %s' % (algs[i].getAlias(),algs[i].getName(), algs[ii].getName()))
              algs[i].setName(algs[ii].getName())
              
            elif algs[ii].getName().lower() in ('unknown','') and algs[i].getName().lower() not in ('unknown','') :
              self._log.debug('Fix the algorithm %s name %s to %s' % (algs[ii].getAlias(),algs[ii].getName(), algs[i].getName()))
              algs[ii].setName(algs[i].getName())
            
          if algs[ii].getName() == algs[i].getName() and algs[ii].getAlias() == algs[i].getAlias() :
            success = False
            if firstLog :
              firstLog=False
              messg = "\n____________________________\n"
              messg += "Algorithm before application:\n"
              messg += "----------------------------\n"
              messg += '%s' % (algs[i])
              self._log.debug(messg)
              
            # If the algorithms are successfully merged
            success = algs[i].mergeAlgorithm(algs[ii])
          
            # Delete succesfully merged algorithm or increase the counter
            if not success :
              # Problems in merging of the algorithms
              self._log.warning("Merging of the algorithms unsuccessfull - see previous error messages")
              
            # Increase the statistics counter
            nmod += 1
            # Print log message for debugging
            messg = "\n____________________________\n"
            messg += "Algorithm will be deleted:\n"
            messg += "----------------------------\n"
            messg += '%s' % (algs[ii])
            self._log.debug(messg)
            
            # Use the delete method in order to delete possible references
            algs[ii].delete()
            del algs[ii]
            
          else:
            # Increase counter if not matching
            ii += 1
        
        # End of loop over remaining parent algorithms
        if not firstLog :
          # Increase the statistics counter
          nmod += 1
          # Print the log message for debugging
          messg = "\n____________________________\n"
          messg += "Algorithm after application:\n"
          messg += "----------------------------\n"
          messg += '%s' % (algs[i])
          self._log.debug(messg)
          
        i += 1
      # End of Loop over parent algorithms
      
      # Merge the children - use recursive method
      for parentAlg in algs :
        if parentAlg.hasChildren() :
          nmod += self.mergeAlg(parentAlg.getChildren())
          #self.mergeAlg(parentAlg.getChildren())
          
    return nmod
  
  ## Fix the child parent relation based on the alias of an algorithm
  #  @param algorithms List of HLTAlgorithm objects
  def fixChildParentRelations(self, algorithms):
    self.setlog()
    if isinstance(algorithms, list) :
      # Suppose the algorithms does not have parents
      # Sorting of the algorithms accordingly to the alias assure the consequence of the parent and child
      algorithms.sort(self.hltAlgAliases_cmp)
      i = 0
      while i < len(algorithms):
        alias = algorithms[i].getAlias()
        # Remove the ToolSvc. from alias
        if alias[:8] == 'ToolSvc.' :
          alias = alias[8:]
          
        # Check the alias
        if '.' in alias :
          self._log.warning('Attempt to fix child algorithm %s/%s' % (algorithms[i].getName(), algorithms[i].getAlias()))
          childAlias = algorithms[i].getAlias().split('.')[-1]
          parentAlias = algorithms[i].getAlias().rstrip(childAlias)
          parentAlias = parentAlias.rstrip('.')
          
          parentFound = False
          flattenAlgs = []
          for ii in range(len(algorithms)) :
            if ii != i :
              flattenAlgs.extend(algorithms[ii].flattenAlg())
          
          for alg in flattenAlgs :
            if alg.getAlias() == parentAlias :
              self._log.debug("Parent found: %s/%s - algorithm will be appended as child" % (alg.getName(), alg.getAlias()))
              parentFound = True
              alg.appendChild(algorithms[i])
          
          # If the algorithm was assigned as a child alg to some parent - delete it
          if parentFound :
            self._log.debug("Appended algorithm %s/%s will be deleted from top parents list" % (algorithms[i].getName(), algorithms[i].getAlias()))
            del algorithms[i]
          else :
            self._log.warning("No parent algorithm found for %s/%s" % (algorithms[i].getName(), algorithms[i].getAlias()))
            i = i+1
            
        else :
          i = i+1
      
      # Fix the children within the parents
      for parent in algorithms : parent.fixChildren()
  
  ## Create a TrigConfiguration setup
  #  @param algorithms List of HLTAlgorithms which will be converted to the Configurable
  def createSetup(self,algorithms):
    self.setlog()
    from AthenaCommon.Configurable import (ConfigurableAlgorithm,
                                      ConfigurableService,
                                      ConfigurableAlgTool,
                                      ConfigurableAuditor)
    # load configurable db and create the ToolSvc
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    
    algs = []
    nservices = 0
    ntools    = 0
    nauditors = 0
    nnocfg = 0
    cfgs = []
    applicationMgr = None
    
    algorithms.sort(compareSvcToolsAuditorsAlg)
    #algorithms.sort(compareAlgToolsSvcAuditor)
    
    algcfg_pair = []
    # Create the configurables
    for alg in algorithms :
      # Exclude ApplicationMgr - set it as last
      if (alg.getAlias(), alg.getName()) == ('ApplicationMgr', 'ApplicationMgr') :
        applicationMgr = alg
        algcfg_pair.append([alg,cfg])
      else :
        cfg = alg.getConfigurable()
        algcfg_pair.append([alg,cfg])
        self._log.debug('Algorithm:\n%s' % (alg))
        self._log.debug('Configurable:\n%s' % (cfg))
        if cfg :
          cfgs.append(cfg)
          if isinstance(cfg, ConfigurableAlgorithm): 
            algs.append({'cfg':cfg, 'topalg':alg.getTopAlg()})
          elif isinstance(cfg, ConfigurableService):
            ServiceMgr += cfg
            nservices += 1
          elif isinstance(cfg, ConfigurableAlgTool):
            ToolSvc    += cfg
            ntools += 1
          elif isinstance(cfg, ConfigurableAuditor):
            ServiceMgr.AuditorSvc += cfg
            nauditors += 1
          else:
            self._log.error('Not nested and not algorithm: %s' % (alg))
        else :
          self._log.error('No configurable assigned for algorithm: %s' % (alg))
          nnocfg += 1
    
    # Set the Configurable properties again
    for alg,cfg in algcfg_pair :
      if cfg: alg.setConfigurableProperties(cfg)
      
    # Assign the algorithms
    algs.sort(self.alg_compare)
    unbound_algs = []
    #for a in algs:
      #alg_full_name = a['cfg'].getFullJobOptName()
      #if alg_full_name in applicationMgr.getProperties()['TopAlg']:
        #topSequence += a['cfg']
        #self._log.debug('Top algorithm %s set' %(alg_full_name))
      #elif not a['topalg'] : # In case there is no TopAlg 
        #unbound_algs.append(a['cfg'])
      #else:
        #cfg = a['cfg']
        #stmt = 'topSequence.%s += cfg' % (a['topalg'].split('/')[-1])
        #self._log.verbose('Execute %s' % (stmt))
        #exec(stmt)
        #self._log.debug('Algorithm %s added to TopAlg %s' %(alg_full_name,a['topalg']))
    
    for a in unbound_algs :
      self._log.warning("Unbound algorithm: %s " % (a.getFullJobOptName()))
    
    # Set the ApplicationMgr
    # Configure the ApplicationMgr
    if applicationMgr.getProperties().has_key('EvtSel'):
      oldEvtSel=theApp.getProperties()['EvtSel']
      evtSel = applicationMgr.getProperties()['EvtSel']
      if not (oldEvtSel == '' or oldEvtSel == '<no value>'):
        self._log.debug('EventSelector defined: %s - keep the value (DB value %s)' % (oldEvtSel, evtSel))
        applicationMgr.getProperties()['EvtSel'] = oldEvtSel
        
    for n,v in applicationMgr.getProperties().items():
      stmt = 'theApp.%s = %s'
      dflt_value = None
      try :
        dflt_value = theApp.__slots__[n]
      except :
        pass
      if v == dflt_value:
          continue
      if isinstance(dflt_value,str): stmt = 'theApp.%s = "%s"'
      stmt = stmt % (n,v)
      if n=='CreateSvc' :
        for vv in v:
          self._log.debug('theApp.CreateSvc += [\'%s\']' % (vv))
          theApp.CreateSvc += [vv]
        self._log.verbose('Value: theApp.CreateSvc=%s' % (theApp.CreateSvc))
      elif n=='ExtSvc' :
        oldExtSvc=theApp.ExtSvc
        theApp.ExtSvc = self.handleExtSvc(oldExtSvc, v)
        self._log.verbose('Value: theApp.ExtSvc=%s' % (theApp.ExtSvc))
      elif n=='TopAlg' :
        self._log.debug('Set: theApp.TopAlg = %s' % (v))
        setTopAlg(v,self._log)
        self._log.verbose('Value: theApp.TopAlg=%s' % (theApp.TopAlg))
      else :
        self._log.verbose('Execute %s' % (stmt))
        try:
          exec(stmt)
          self._log.verbose('Value theApp.%s set: %s' % (n, theApp.getProperties()[n]))
        except Exception,err:
          self._log.error('Can not set theApp.%s=%s :%s' % (n,v,err.message))
    
    
    self._log.info("Configurables in db: %d" % (len(algorithms)))
    self._log.info("Algorithms set:      %d" % (len(algs)))
    self._log.info("Services set:        %d" % (nservices))
    self._log.info("Tools set:           %d" % (ntools))
    self._log.info("Auditors set:        %d" % (nauditors))
    # The 1 is for the ApplicationMgr
    not_set = len(algorithms) - len(algs) - nservices - ntools - nauditors - 1
    
    if not_set != 0:
      self._log.warning("Failed to restore %d configurables" % (not_set))
  
  ## Special setting of the ApplicationMgr.ExtSvc property
  #  @param existingExtSvc pointer to the existing property
  #  @param addExtSvc values which has to be add to the existing property
  #  @return returns new value 
  def handleExtSvc(self, existingExtSvc, addExtSvc) :
    self.setlog()
    extSvcDict = self.extSvcListToDict(existingExtSvc)
    extSvcDict = self.extSvcListToDict(addExtSvc, extSvcDict)
    
    output = []
    for k,v in extSvcDict.items() :
      if v != '' :
        output.append('%s/%s' %(v,k))
      else:
        output.append(k)
    return output
  
  ## Convert ApplicationMgr.ExtSvc to dictionairy
  #  @param extSvcList Input ApplicationMgr.ExtSvc
  #  @param existingDict Existing dictionairy
  def extSvcListToDict(self, extSvcList, existingDict={}):
    self.setlog()
    extSvcDict = existingDict
    # Process the existing ExtSvc to dictionary
    for k in extSvcList:
      svc_name=k
      svc_type=''
      if len(k.split('/'))==2 :
        svc_type = k.split('/')[0]
        svc_name = k.split('/')[1]
        
      saved_type = extSvcDict.get(svc_name, svc_type)
      if saved_type != svc_type :
        self._log.warning('Different job types for name: %s - existing: %s required: %s - existing will be kept' % (svc_name, saved_type, svc_type))
      else:
        extSvcDict[svc_name] = svc_type
    return extSvcDict
  
  def alg_compare(self, a1, a2) :
    if a1['topalg'] == a1['cfg'].getFullJobOptName() :
      return -1
    
    if a2['topalg'] == a2['cfg'].getFullJobOptName() :
      return 1
    
    return cmp(a1['cfg'].getName(), a2['cfg'].getName())
  
  def hltAlgAliases_cmp(self, a1, a2) :
    if a1.getAlias()[:8] == 'ToolSvc.' and a2.getAlias()[:8] != 'ToolSvc.' :
      return -1
    
    if a2.getAlias()[:8] == 'ToolSvc.' and a1.getAlias()[:8] != 'ToolSvc.' :
      return 1
    
    return cmp(a1.getAlias(), a2.getAlias())

  def connectDB(self):
    self.setlog()
    import TrigConfOffline.HLTSQLUtils as HLTSQLUtils
    cursor = HLTSQLUtils.get_cursor(self.dbHost, self.dbUser, self.dbName, self.dbType, False, self.dbPasswd)
    return cursor
  
  ## Get the HLT Prescale Keys
  #  @param cursor DB cursor
  def getHLTPrescaleKeys(self, cursor) :
    self.setlog()
    prepend = ""
    if self.dbType == "oracle" and self.dbName != self.dbUser :
      prepend = "%s." % (self.dbName)
    
    query  = " SELECT %sHLT_PRESCALE_SET.HPS_ID " % (prepend)
    query += " FROM %sSUPER_MASTER_TABLE " % (prepend)
    query += " JOIN %sHLT_MASTER_TABLE ON (%sSUPER_MASTER_TABLE.SMT_HLT_MASTER_TABLE_ID = %sHLT_MASTER_TABLE.HMT_ID) " % (prepend, prepend, prepend)
    query += " JOIN %sHLT_TM_TO_PS ON (%sHLT_MASTER_TABLE.HMT_TRIGGER_MENU_ID = %sHLT_TM_TO_PS.HTM2PS_TRIGGER_MENU_ID) " % (prepend, prepend, prepend)
    query += " JOIN %sHLT_PRESCALE_SET ON (%sHLT_PRESCALE_SET.HPS_ID = %sHLT_TM_TO_PS.HTM2PS_PRESCALE_SET_ID) " % (prepend, prepend, prepend)
    query += " WHERE %sSUPER_MASTER_TABLE.SMT_ID=%s " % (prepend, self.SMKey)
    
    self._log.debug(query)
    cursor.execute( query )
    result = cursor.fetchall()
    if len(result) > 0 and len(result[0]) > 0:
      prescaleKeys = []
      for t in result :
        prescaleKeys.append(t[0])
      return prescaleKeys
    else:
      return None
  
  ## Get the LVL1 Prescale Keys
  #  @param cursor DB cursor
  def getLVL1PrescaleKeys(self, cursor) :
    self.setlog()
    prepend = ""
    if self.dbType == "oracle" and self.dbName != self.dbUser :
      prepend = "%s." % (self.dbName)
    
    query  = " SELECT %sL1_PRESCALE_SET.L1PS_ID " % (prepend)
    query += " FROM %sSUPER_MASTER_TABLE " % (prepend)
    query += " JOIN %sL1_MASTER_TABLE ON (%sSUPER_MASTER_TABLE.SMT_L1_MASTER_TABLE_ID = %sL1_MASTER_TABLE.L1MT_ID) " % (prepend, prepend, prepend)
    query += " JOIN %sL1_TM_TO_PS ON (%sL1_MASTER_TABLE.L1MT_TRIGGER_MENU_ID = %sL1_TM_TO_PS.L1TM2PS_TRIGGER_MENU_ID) " % (prepend, prepend, prepend)
    query += " JOIN %sL1_PRESCALE_SET ON (%sL1_PRESCALE_SET.L1PS_ID = %sL1_TM_TO_PS.L1TM2PS_PRESCALE_SET_ID) " % (prepend, prepend, prepend)
    query += " WHERE %sSUPER_MASTER_TABLE.SMT_ID=%s " % (prepend, self.SMKey)
    
    self._log.debug(query)
    cursor.execute( query )
    result = cursor.fetchall()
    if len(result) > 0 and len(result[0]) > 0:
      prescaleKeys = []
      for t in result :
        prescaleKeys.append(t[0])
      return prescaleKeys
    else:
      return None
  
  ## Set the DB connection parameters using the dblookup alias
  #  @param dblookupString Alias in the dblookup file
  def setDbConnectionFromAuthFile(self, dblookupString='TRIGGERDB') :
    #self.setlog()
    # Read Trigger DB Parameters from dblookup and db authentication files
    import os
    dblookup_file_name='dblookup.xml'
    authentication_file_name='authentication.xml'
    # Find the DBLookup file path:
    dblookup_file=dblookup_file_name
    authentication_file=authentication_file_name
    
    from AthenaCommon.Utils.unixtools import FindFile
    # look for it in the local and then all XML directories
    dblookup_file = FindFile(dblookup_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
    authentication_file = FindFile(authentication_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
    
    # Not found in user space - look in the AtlasAuthentication package
    if not dblookup_file:
      dblookup_file = 'AtlasAuthentication/'+dblookup_file_name
      dblookup_file = FindFile(dblookup_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
    
    if not authentication_file:
      authentication_file = 'AtlasAuthentication/'+authentication_file_name
      authentication_file = FindFile(authentication_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
    
    # Look for CORAL_DBLOOKUP_PATH and CORAL_AUTH_PATH
    if not dblookup_file:
      coral_dblookup_path = os.environ.get('CORAL_DBLOOKUP_PATH')
      if not coral_dblookup_path:
        self._log.error('no CORAL_DBLOOKUP_PATH environment variable found')
        return False
      dblookup_file = coral_dblookup_path + '/dblookup.xml'
    
    if not authentication_file:
      coral_auth_path = os.environ.get('CORAL_AUTH_PATH')
      if not coral_auth_path:
        self._log.error('no CORAL_AUTH_PATH  environment variable found')
        return False
      authentication_file = coral_auth_path + '/authentication.xml'
    
    if authentication_file and dblookup_file :
      self._log.verbose('dblookup file: %s' % (dblookup_file))
      self._log.verbose('authentication file: %s' % (authentication_file))
    
    if self.__dict__.has_key('setupSource') and self.setupSource :
      dblookupString=self.setupSource
    
    trigger_connection_list = None
    import xml.dom.minidom
    # read dblookup file
    doc = xml.dom.minidom.parse(dblookup_file)
    doc = doc.documentElement
    trigger_dblookup_list = []
    trigger_connection_list = []
    for logsvc in doc.getElementsByTagName('logicalservice') :
      if dblookupString in logsvc.getAttribute('name') :
        for svc in logsvc.getElementsByTagName('service') :
          connectionString = str(svc.getAttribute('name'))
          self._log.verbose('Connection string: %s' % (connectionString))
          connection = {}
          connection['dbType'] = connectionString.split(':')[0]
          connection['dbUser'] = None
          connection['dbPasswd'] = None
          trigger_dblookup_list.append(connectionString)
          if svc.hasAttribute('authentication') :
            if connection['dbType'] != 'sqlite_file' :
              connection['dbServer'] = connectionString.split('://')[1].split('/')[0]
              connection['dbName'] = connectionString.split('://')[1].split('/')[1]
          if connection['dbType'] == 'sqlite_file':
            connection['dbType'] = 'sqlite'
            connection['dbServer'] = connectionString.split(':')[1]
            connection['dbName'] = connectionString.split(':')[1]
          
          trigger_connection_list.append(connection)
    
    if len(trigger_dblookup_list) <= 0:
      self._log.error('no logicalservice %s found in file %s' % (dblookupString,dblookup_file))
      return False
    
    # read authentication file
    if len(trigger_dblookup_list) > 0:
      doc = xml.dom.minidom.parse(authentication_file)
      doc = doc.documentElement
      
      for conn in doc.getElementsByTagName('connection') :
        if conn.getAttribute('name') in trigger_dblookup_list:
          name = str(conn.getAttribute('name'))
          ind = trigger_dblookup_list.index(name)
          connection = {}
          connection['dbType'] = name.split(':')[0]
          connection['dbServer'] = name.split('://')[1].split('/')[0]
          connection['dbName'] = name.split('://')[1].split('/')[1]
          connection['dbUser'] = None
          connection['dbPasswd'] = None
          for par in conn.getElementsByTagName('parameter'):
            if par.getAttribute('name').lower() == 'user':
              connection['dbUser'] = str(par.getAttribute('value'))
            elif par.getAttribute('name').lower() == 'password':
              connection['dbPasswd'] = str(par.getAttribute('value'))
          
          if trigger_connection_list[ind]['dbUser'] :
            trigger_connection_list.append(connection)
          else:
            trigger_connection_list[ind] = connection
    
    conn = False
    if len(trigger_connection_list) == 0:
      self._log.error('missing %s connection parameters check files:\n\t %s \n\t%s' % (dblookupString, dblookup_file, authentication_file))
      return False
    
    if self.OutputLevel <= 2:
      mesg = 'Available connections:'
      for connection in trigger_connection_list :
        mesg += '\n\t%s' % (connection)
        
      self._log.debug(mesg)
    
    for connection in trigger_connection_list :
      self.dbType = connection['dbType']
      if connection['dbType'] in ('oracle','mysql') :
        self.dbHost = connection['dbServer']
        self.dbUser = connection['dbUser']
        self.dbPasswd = connection['dbPasswd']
        self.dbName = connection['dbName']
      
      try:
        cursor = None
        cursor = self.connectDB()
        if cursor :
          cursor.close()
          return True
        else:
          self._log.debug('Can\'t connect to the DB %s' % (connection))
      except Exception, exc:
        self._log.debug(exc.message)
        pass

    
    self._log.error('No valid %s connection found - check files:\n\t %s \n\t%s' % (dblookupString, dblookup_file, authentication_file))
        
    return False
      
  ## Load and create the Trigger Configuration setup
  def load(self):
    self.setlog()
    self.configure()
    import time, sys
    start = time.time()
    
    # Load the algorithms
    algorithms = self.loadSetup()
    
    # Merge loaded algorithms
    nmod = self.mergeAlg(algorithms)
    self._log.debug('%d modified algorithms during merge' % (nmod))
    
    # There may be some child algorithms
    self.fixChildParentRelations(algorithms)
    
    # Create a job setup
    self.createSetup(algorithms)
    
    stop = time.time()
    self._log.info('Full configuration loaded in %ds' % (stop-start))
  
  ## Configure self
  def configure(self) :
    self.setlog()
    self._consistencyCheck()
    self._checkHLTPrescaleKey()
    self._checkLVL1PrescaleKey()
    
  def initialize(self):
    self.setlog()
    self.load()
    
  def run(self):
    self.setlog()
    self.load()
      
  def finalize(self):
    pass

## Erase Configurable
#  @param cfg Configurable which has to be entirely deleted
#  @param logger Logger
def eraseConfigurable(cfg, logger=None):
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.eraseConfigurable')
  from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
  # erase existing Configurable
  cfg_parent = cfg.getParent() # It is a string with the configurable parent name if empty - no parent
  cfg_children = cfg.getAllChildren()
  name = cfg.getName()
  from AthenaCommon.Configurable import (ConfigurableAlgorithm,
                                          ConfigurableService,
                                          ConfigurableAlgTool,
                                          ConfigurableAuditor)
  if isinstance(cfg, ConfigurableAlgorithm):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    logger.info('remove algorithm: %s' % (name))
    stmt = 'del topSequence.%s' % (name)
    logger.verbose(stmt)
    exec(stmt)
  elif isinstance(cfg, ConfigurableService):
    logger.info('remove service: %s' % (name))
    stmt = 'del ServiceMgr.%s' % (name)
    logger.verbose(stmt)
    exec(stmt)
  elif isinstance(cfg, ConfigurableAlgTool): 
    logger.info('remove public tool: %s' % (name))
    stmt = 'del ServiceMgr.ToolSvc.%s' % (name)
    logger.verbose(stmt)
    exec(stmt)
  elif isinstance(cfg, ConfigurableAuditor):
    logger.info('remove auditor service: %s' % (name))
    stmt = 'del ServiceMgr.AuditorSvc.%s' % (name)
    logger.verbose(stmt)
    exec(stmt)
  else:
    logger.error('Unknown type of configurable: %s' % (cfg.getFullName()))
  
  del cfg
  # Just in case
  from AthenaCommon.Configurable import Configurable
  try:
    del Configurable.allConfigurables[name]
  except :
    pass

## Documentation for function removeAllCfgChildren
#  @param cfg Input configurable
#  @param logger Logger default value None message HLTConfOffline.removeAllCfgChildren
#
#  Remove all the children from the configurable including the PrivateTools and PrivateToolArrays
def removeAllCfgChildren(cfg, logger=None):
  # Set logger if non existing
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.removeAllCfgChildren')
    #import logging
    #logger=logging.getLogger('HLTConfOffline.removeAllCfgChildren')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
  
  logger.verbose("Configurable before child removal:\n%s" % (cfg))
  privToolCand = {}
  # Check properties for PrivateTool
  for k,v in cfg.getProperties().items() :
    # Test the type of the property
    cfgPropertyType = ('%s' % (type(v))).replace('<class ','').replace('<type ','').replace("'",'')
    cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
    
    # Skip PublicTools and Services
    if cfgPropertyType in ( 'GaudiKernel.GaudiHandles.ServiceHandle',
                            'GaudiKernel.GaudiHandles.ServiceHandleArray',
                            'GaudiKernel.GaudiHandles.PublicToolHandle',
                            'GaudiKernel.GaudiHandles.PublicToolHandleArray') : continue
    
    # Search the PrivateTooHandleArray
    if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandleArray' :
      logger.verbose('Clear PrivateToolHandleArray %s.%s = %s' % (cfg.getFullJobOptName(),k,v))
      stmt = 'cfg.%s = []' % (k)
      logger.verbose('Execute: %s' % (stmt))
      exec stmt
      continue
    
    # Search the PrivateTooHandle
    if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandle' :
      logger.verbose('Set the PrivateToolHandle to None %s.%s = %s' % (cfg.getFullJobOptName(),k,v))
      stmt = 'cfg.%s = None' % (k)
      logger.verbose('Execute: %s' % (stmt))
      exec stmt
      continue
    
    # Skip the built-in-type values
    if isinstance(v,(bool,int,long,float,str,unicode,tuple,list,dict)) : continue
    
    privTFullJobOptName = None
    try :
      privTFullJobOptName = v.getFullJobOptName()
    except :
      pass
    if privTFullJobOptName :
      logger.verbose('PrivateToolHandle candidate %s.%s = %s (type: %s)' % (cfg.getFullJobOptName(),k,privTFullJobOptName, type(v)))
      privToolCand[privTFullJobOptName] = k
      
  # Erase the children - start with the last item
  inds = range(len(cfg.getAllChildren()))
  inds.reverse()
  for i in inds :
    cFullJobOptName = cfg.getAllChildren()[i].getFullJobOptName()
    logger.verbose('Delete child %s' % (cFullJobOptName))
    del cfg.getAllChildren()[i]
    if cFullJobOptName in privToolCand.keys() :
      logger.verbose('Set the PrivateToolHandle to None %s.%s = %s' % (cfg.getFullJobOptName(),privToolCand[cFullJobOptName],None))
      stmt = 'cfg.%s = None' % (privToolCand[cFullJobOptName])
      logger.verbose('Execute: %s' % (stmt))
      exec stmt
      
## Documentation for function removeCfgChild
#  @param cfg Input configurable
#  @param childName Name of the child to be removed
#  @param logger Logger default value None message HLTConfOffline.removeAllCfgChildren
#
#  Remove the child from the configurable including the PrivateTools and PrivateToolArrays
def removeCfgChild(cfg, childName, logger=None):
  # Set logger if non existing
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.removeAllCfgChildren')
    #import logging
    #logger=logging.getLogger('HLTConfOffline.removeCfgChild')
    #logger.setLevel(logging.INFO)
    #ch = logging.StreamHandler()
    #ch.setLevel(logging.INFO)
    #formatter = logging.Formatter("%(name)s - %(levelname)s\t%(message)s")
    #ch.setFormatter(formatter)
    #logger.addHandler(ch)
  
  
  logger.verbose('Remove child %s from %s' % (childName, cfg.getFullJobOptName()))
  privToolCand = {}
  # Check properties for PrivateTool or PrivateToolHandleArray
  for k,v in cfg.getProperties().items() :
    # Test the type of the property
    cfgPropertyType = ('%s' % (type(v))).replace('<class ','').replace('<type ','').replace("'",'')
    cfgPropertyType = cfgPropertyType[:len(cfgPropertyType)-1]
    
    # Skip PublicTools and Services
    if cfgPropertyType in ( 'GaudiKernel.GaudiHandles.ServiceHandle',
                            'GaudiKernel.GaudiHandles.ServiceHandleArray',
                            'GaudiKernel.GaudiHandles.PublicToolHandle',
                            'GaudiKernel.GaudiHandles.PublicToolHandleArray') : continue
    
    # Remove the child from the PrivateToolHandleArray
    if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandleArray' :
      for item in v :
        privToolJobOptName = item
        try :
          privToolJobOptName = item.getName()
        except:
          pass
        if privToolJobOptName and  isinstance(privToolJobOptName,str) and privToolJobOptName == childName :
          # Format the privToolJobOptName if needed
          if len(privToolJobOptName.split('/')) >= 2 :
            privToolJobOptName = '/'.join([privToolJobOptName.split('/')[0],privToolJobOptName.split('/')[-1].split('.')[-1]])
          stmt = 'del cfg.%s[\'%s\']' % (k,privToolJobOptName)
          try :
            logger.verbose('Execute: %s' % (stmt))
            exec stmt
          except Exception, exc:
            logger.debug('Exception occured: %s' % (exc.message))
          continue
    
    # Remove the child from PrivateToolHandle
    if cfgPropertyType == 'GaudiKernel.GaudiHandles.PrivateToolHandle' :
      privToolFullJobOptName = v
      try :
        privToolFullJobOptName = v.getFullJobOptName()
      except:
        pass
      if privToolFullJobOptName and  isinstance(privToolFullJobOptName,str) and privToolFullJobOptName.split('/')[-1].split('.')[-1] == childName :
        stmt = 'cfg.%s = None' % (k)
        logger.verbose('Execute: %s' % (stmt))
        exec stmt
        continue
    
    # Skip the built-in-type values
    if isinstance(v,(bool,int,long,float,str,unicode,tuple,list,dict)) : continue
    
    # Check the unsupported types
    privTFullJobOptName = v
    try :
      privTFullJobOptName = v.getFullJobOptName()
    except :
      pass
    if privTFullJobOptName and  isinstance(privTFullJobOptName,str) and privTFullJobOptName.split('/')[-1].split('.')[-1] == childName :
      logger.debug('PrivateToolHandle candidate %s.%s = %s' % (cfg.getFullJobOptName(),k,privTFullJobOptName))
      privToolCand[privTFullJobOptName] = k
  
  # Remove the child - start with the last item
  inds = range(len(cfg.getAllChildren()))
  inds.reverse()
  for i in inds :
    if cfg.getAllChildren()[i].getName() == childName :
      cFullJobOptName = cfg.getAllChildren()[i].getFullJobOptName()
      logger.debug('Delete child %s' % (cFullJobOptName))
      del cfg.getAllChildren()[i]
      if cFullJobOptName in privToolCand.keys() :
        logger.debug('Set the PrivateToolHandle to None %s.%s' % (cfg.getFullJobOptName(),privToolCand[cFullJobOptName]))
        stmt = 'cfg.%s = None' % (privToolCand[cFullJobOptName])
        logger.verbose('Execute: %s' % (stmt))
        exec stmt

## Documentation for function getUserPasswdFromAuth
#  @param connectionName string obtained from the dblookup.xml file
#  @return dictionairy with items user,passwd or empty dictionairy
#  Extract the DB connection parameters
def getUserPasswdFromAuth(connectionName, logger=None) :
  # Set logger if non existing
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.getUserPasswdFromAuth')
  # output dictionairy
  output = {}
  authentication_file='authentication.xml'
  import os
  from AthenaCommon.Utils.unixtools import FindFile
  # look for it in the local and then all XML directories
  authentication_file = FindFile(authentication_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
  
  # Not found in user space - look in the AtlasAuthentication package
  if not authentication_file:
    authentication_file = 'AtlasAuthentication/authentication.xml'
    authentication_file = FindFile(authentication_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
  
  # Look for CORAL_DBLOOKUP_PATH and CORAL_AUTH_PATH
  if not authentication_file:
    coral_auth_path = os.environ.get('CORAL_AUTH_PATH')
    if not coral_auth_path:
      logger.error('no CORAL_AUTH_PATH  environment variable found')
      return output
    authentication_file = coral_auth_path + '/authentication.xml'
  
  import xml.dom.minidom
  doc = xml.dom.minidom.parse(authentication_file)
  doc = doc.documentElement
  
  for conn in doc.getElementsByTagName('connection') :
    if conn.getAttribute('name')==connectionName:
      for par in conn.getElementsByTagName('parameter'):
        if par.getAttribute('name').lower() == 'user':
          output['user'] = str(par.getAttribute('value'))
        elif par.getAttribute('name').lower() == 'password':
          output['passwd'] = str(par.getAttribute('value'))
  
  return output

## Get the conection strings from the dblookup file
#  @param alias Logical service attibute - name
#  @return list of connection strings
def getConnectionsFromDBLookup(alias, logger=None) :
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.getConnectionsFromDBLookup')
  
  output = []
  dblookup_file='dblookup.xml'
  import os
  from AthenaCommon.Utils.unixtools import FindFile
  # look for it in the local and then all XML directories
  dblookup_file = FindFile(dblookup_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
  
  # Not found in user space - look in the AtlasAuthentication package
  if not dblookup_file:
    dblookup_file = 'AtlasAuthentication/dblookup.xml'
    dblookup_file = FindFile(dblookup_file,['.']+os.environ[ 'XMLPATH' ].split(os.pathsep),os.R_OK )
  
  # Look for CORAL_DBLOOKUP_PATH and CORAL_AUTH_PATH
  if not dblookup_file:
    coral_dblookup_path = os.environ.get('CORAL_DBLOOKUP_PATH')
    if not coral_dblookup_path:
      logger.error('no CORAL_DBLOOKUP_PATH  environment variable found')
      return output
    dblookup_file = coral_dblookup_path + '/dblookup.xml'
  
  import xml.dom.minidom
  doc = xml.dom.minidom.parse(dblookup_file)
  doc = doc.documentElement
  
  for logsvc in doc.getElementsByTagName('logicalservice') :
    if alias==logsvc.getAttribute('name') :
      for svc in logsvc.getElementsByTagName('service') :
        connectionString = str(svc.getAttribute('name'))
        output.append(connectionString)
          
  return output
## Function for sorting the algorithms - priority: Services, Tools, Auditors, Algorithms
def compareSvcToolsAuditorsAlg(alg1, alg2) :
  name1 = alg1.getName()
  name2 = alg2.getName()
  alias1 = alg1.getAlias()
  alias2 = alg2.getAlias()
  
  # Services
  if (name1.endswith('Svc') or alias1.endswith('Svc')) and not (name2.endswith('Svc') or alias2.endswith('Svc')) :
    return -1
  
  if (name2.endswith('Svc') or alias2.endswith('Svc')) and not (name1.endswith('Svc') or alias1.endswith('Svc')) :
    return 1
  
  # Tools
  if alias1.startswith('ToolSvc') and not alias2.startswith('ToolSvc') :
    return -1
  
  if alias2.startswith('ToolSvc') and not alias1.startswith('ToolSvc') :
    return 1
  
  return cmp(alias1, alias2)

## Function for sorting the algorithms - priority: Algorithms, Tools, Services
def compareAlgToolsSvcAuditor(alg1, alg2) :
  name1 = alg1.getName()
  name2 = alg2.getName()
  alias1 = alg1.getAlias()
  alias2 = alg2.getAlias()
  
  a1Svc = name1.endswith('Svc') or alias1.endswith('Svc')
  a2Svc = name2.endswith('Svc') or alias2.endswith('Svc')
  a1Tool = alias1.startswith('ToolSvc')
  a2Tool = alias2.startswith('ToolSvc')
  a1ToolSvc = name1 == 'ToolSvc'
  a2ToolSvc = name2 == 'ToolSvc'
  a1Auditor = name1.endswith('Auditor') or alias1.endswith('Auditor')
  a2Auditor = name2.endswith('Auditor') or alias2.endswith('Auditor')
  a1Alg = not (a1Svc or a1Tool or a1Tool or a1ToolSvc)
  a2Alg = not (a2Svc or a2Tool or a2Tool or a2ToolSvc)
  
  # Algorithms
  if a1Alg and not a2Alg :  return -1
  if a2Alg and not a1Alg :  return  1
  if a1Alg and a2Alg :      return cmp(alias1, alias2)
  
  # ToolSvc
  if a1ToolSvc and not a2ToolSvc : return -1
  if a2ToolSvc and not a1ToolSvc : return 1
  if a1ToolSvc and a2ToolSvc : return cmp(alias1, alias2)
  
  # Tools
  if a1Tool and not a2Tool : return -1
  if a2Tool and not a1Tool : return 1
  if a1Tool and a2Tool : return cmp(alias1, alias2)
  
  # Services
  if a1Svc and not a2Svc : return -1
  if a2Svc and not a1Svc : return 1
  if a1Svc and a2Svc : return cmp(alias1, alias2)
  
  # Auditors
  if a1Auditor and not a2Auditor : return -1
  if a2Auditor and not a1Auditor : return 1
  if a1Auditor and a2Auditor : return cmp(alias1, alias2)
  
  # Unencoutered
  return cmp(alias1, alias2)

## Interprets the connection string
#  @param connectionString connection string
#  connection string needs to be of the following format (this is also the order of checking)
#  <ALIAS>                              -- any string without a colon ':' will be checked for in the dblookup.xml file
#  type:<detail>                        -- no dblookup will be used, type has to be oracle, mysql, or sqlite_file
#  sqlite_file:filename.db              -- an sqlite file, no authentication needed, will be opened in read-only mode
#  oracle://ATLR/ATLAS_CONF_TRIGGER_V2  -- a service description without user and password, requires lookup in authentication.xml
#  oracle://ATLR/ATLAS_CONF_TRIGGER_V2;username=ATLAS_CONF_TRIGGER_V2_R;password=<...>  -- a service description with user and password
#  @return dictionairy - items techno, server, schema, user, passwd, filename
def interpretConnectionString(connectionString, logger=None) :
  # Set logger if non existing
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.interpretConnectionString')
  # output dictionairy
  connectionParameters={}
  dbtype=''
  
  if ':' in connectionString : # Connection string in format type:<detail>
    dbtype = connectionString.split(':')[0].lower()
    if dbtype in ('sqlite','oracle','mysql') :
      connectionParameters['techno'] = dbtype
    else:
      logger.error('Unsupported type of DB %s - check the connection string: %s' % (dbtype,connectionString))
      return connectionParameters
    # Remaining information
    # Process sqlite type connection string
    if dbtype == 'sqlite' : # Remaining information is a filename
      filename = ':'.join(connectionString.split(':')[1:])
      from AthenaCommon.Utils.unixtools import FindFile
      # Get full file path
      filename = FindFile(filename,['.'],os.R_OK )
      if filename:
        connectionParameters['filename'] = filename
      else :
        connectionParameters['filename'] = connInformation
        logger.error('sqlite file %s not found' % (connectionParameters['filename']))
      # enough information saved - nothing to process
      return connectionParameters
    # Here the type of DB is either oracle or mysql
    # Extract the server resp. schema information from begining of the string
    connInformation = ':'.join(connectionString.split(':')[1:]).lstrip('/')
    serverDbSchema=connInformation.split(';')[0]
    connectionParameters['server'] = serverDbSchema.split('/')[0]
    if '/' in serverDbSchema :
      connectionParameters['schema'] = serverDbSchema.split('/')[1].split(';')[0]
    if serverDbSchema.count('/') == 1 :
      connInformation = connInformation.replace(serverDbSchema,'',1).strip(';')
    elif serverDbSchema.count('/') == 2 :
      connInformation = connInformation.replace('/'.join(serverDbSchema.split('/')[:1]),'',1).lstrip('/')
    # If other information provided process it
    for item in connInformation.split(';'):
      if '=' in item :
        name = item.split('=')[0].lower()
        value = item.split('=')[1]
        if name in ('schema','name') :
          connectionParameters['schema'] = value
        elif name.startswith('user') :
          connectionParameters['user'] = value
        elif name in ('passwd', 'password') :
          connectionParameters['passwd'] = value
            
    # Password needed and not specified - use the authentication file
    if dbtype !='sqlite' and not connectionParameters.has_key('passwd'):
      authConnPar = getUserPasswdFromAuth(connectionString)
      if authConnPar.has_key('user') : connectionParameters['user'] = authConnPar['user']
      if authConnPar.has_key('passwd') : connectionParameters['passwd'] = authConnPar['passwd']
  else : # aliases and passwords to be searched in the dblokup file
    connStrings=getConnectionsFromDBLookup(connectionString)
    import TrigConfOffline.HLTSQLUtils as HLTSQLUtils
    # Process all the connection strings found - return 1st valid connection
    for conStr in connStrings :
      # Recursive call to process the dblookup connection string
      connectionParameters = interpretConnectionString(conStr)
      # Try the connection on 1st successfull connection return the parameters and stop searching
      cursor = None
      if connectionParameters['techno'] in ('oracle','mysql') :
        cursor = HLTSQLUtils.get_cursor(connectionParameters['server'], connectionParameters['user'],
                                        connectionParameters['schema'], connectionParameters['techno'],
                                        False, connectionParameters['passwd'])
      elif connectionParameters['techno'] in ('sqlite') :
        cursor = HLTSQLUtils.get_cursor(connectionParameters['filename'], None, None, connectionParameters['techno'], False, None)
      # If connection is opened - stop
      if cursor :
        cursor.close()
        break
    
  return connectionParameters

## Set the top algorithms
#  @param TopAlg top algorithm list as stated in ApplicationMgr.TopAlg
#  @param logger Logger
def setTopAlg(TopAlg, logger=None) :
  # Set logger if non existing
  if logger == None :
    from AthenaCommon.Logging import logging
    logger = logging.getLogger('HLTConfOffline.setTopAlg')
  
  logger.verbose('Top algorithms to be set: (%s) %s' %(type(TopAlg),TopAlg))
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  from AthenaCommon.Configurable import Configurable
  for item in TopAlg :
    if len(item.split('/')) not in (1,2) :
      logger.error('Unsupported form of TopAlg item %s - will be ignored' % (item))
      continue
    logger.verbose("Get the top algorithm %s" % (item))
    topAlgAlias = item.split('/')[-1]
    topAlgType = item.split('/')[0]
    topAlgCfg = None
    # Test if the configurable exists
    if Configurable.allConfigurables.has_key(topAlgAlias) :
      topAlgCfg = Configurable.allConfigurables[topAlgAlias]
      logger.verbose("Top algorithm %s exists %s/%s" % (topAlgAlias, topAlgCfg.getType(), topAlgCfg.getName()))
      # Test the type if stated
      if len(item.split('/'))==2 and topAlgType != topAlgCfg.getType():
        logger.warning('Imcompatible type of existing configurable %s/%s - required %s' % (topAlgCfg.getType(), topAlgCfg.getName(),item))
        topAlgCfg = None
        
    # The requested top algorithm doesn't exist for some reason create new configurable
    if not topAlgCfg :
      logger.verbose("Top algorithm %s does not exist create new" % (item))
      from TrigConfOffline.HLTAlgorithm import HLTAlgorithm
      topAlg = HLTAlgorithm(name=topAlgType, alias=topAlgAlias)
      topAlgCfg = topAlg.getConfigurable()
    
    if topAlgCfg:
      logger.debug("Append top algorithm %s/%s to alg sequence" % (topAlgCfg.getType(), topAlgCfg.getName()))
      topSequence += topAlgCfg
    else :
      logger.error('Can not get top algorithm %s' % (item))
      
      
    
