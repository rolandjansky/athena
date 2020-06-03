###########################################################################
#    
#    <nozicka@mail.desy.de>
#
###########################################################################

import string

class SetupLoader(object) :
  _properties = ('Source','dbType','dbHost','dbUser','dbPasswd','dbName','SMKey','Level','RunNumber','OutputLevel')
  
  ## Initialize the SetupLoader service
  def __init__(self, *args, **kwargs) :
    from AthenaCommon.Logging import logging
    self.__dict__['_log'] = logging.getLogger('HLTSetupLoader')
    self._log.setLevel(3)
    ## @var setupSource 
    #  Source of the TrigConfiguration
    #  supported values: DBLookup ALIAS, Connection string, db
    self.__dict__['Source'] = 'TRIGGERDB'

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

    ## @var SMKey
    #  Super Master Key 
    self.SMKey = -1
    
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
      
  ## Set the Logger
  def setlog(self):
    if self.__dict__.has_key('OutputLevel'):
      self._log.setLevel(self.OutputLevel)
    else:
      self._log.setLevel(3)

    self._log.name = 'HLTSetupLoader '
  
  ## Set attributes of the class
  #  Allowed attributes: Source,dbType,dbHost,dbUser,dbPasswd,dbName,SMKey,Level,RunNumber,OutputLevel 
  #  @param name Name of the attribute
  #  @param value Value to be set
  def __setattr__(self, name, value) :
    #self.setlog()
    self._log.verbose("__setattr__(%s,%s)" % (name,value))
    if name in self._properties:
      self._log.debug('Set property: %s = %s' % (name, value))
      if name=="Source":
        value = self._setSource(value)
        
      self.__dict__[name] = value
      if name=='OutputLevel':
        self._log.setLevel(value)
    else:
      self._log.warning('Unknown property: %s = %s  - property not set' % (name, value))
  
  ## Convert the class to string for pretty print
  #  @param self Pointer to the class
  #  @return String
  def __str__(self):
    self.setlog()
    output = "-"*80 + "\n"
    string = "%s.%s" % (self.__module__,'HLTSetupLoader')
    output += "|" + " "*(39 - len(string)/2) + string +" "*(39 - len(string)/2) + "|\n"
    output += "-"*80 + "\n"
    
    for k,v in self.__dict__.items() :
      if k == 'dbPasswd' :
        output += "\t%s = %s\n" % (k,"*"*len(v))
      elif k== '_log':
        pass
      else :
        output += "\t%s = %s\n" % (k,v)
        
    return output
      
  ## Check the consistency of the DB source 
  #  @param self Object pointer
  #  @return True if parameters are OK
  def _consistencyCheck(self):
    self.setlog()
    if self.Source[:-4] != '.xml' :
      return _consistencyDBCheck()
    else:
      return _consistencyFileCheck()
    
    return False
  
  ## Check the consistency of the DB source 
  #  @param self Object pointer
  def _consistencyDBCheck(self):
    self.setlog()
    # Check existence of the superMaster key
    if not self.__dict__.has_key['SMKey'] :
      return False
    if self.SMKey <= 0 :
      return False
    
    # Check if the source of the configuration exists
    if not (self.__dict__.has_key('Source') and self.Source):
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
        self._log.warning('No trigger setup source')
        # Try to fix it - however looks doing nothing it does more
        self.Source = self.Source
      else :
        self._log.warning('setupSource previously not set')
        self.setupSource = 'db'

    # Check the remaining DB parameters
    if self.setupSource == 'db' :
      if not (self.__dict__.has_key('dbType') and self.dbType) :
        self._log.fatal('No DB Type specified')
        return False
      
      if not (self.__dict__.has_key('dbHost') and self.dbHost) :
        self._log.fatal('No DB Host or sqlite file specified')
        return False
      
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbUser') and self.dbUser) :
        self._log.fatal('No DB User specified')
        return False
      
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbPasswd') and self.dbPasswd) :
        self._log.fatal('No DB Password specified')
        return False
      
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbName') and self.dbName) :
        if self.dbType=='oracle' :
          self.dbName = self.dbUser
        else :
          self._log.fatal('No DB Name specified')
          return False
    else : # Should not appear - but just in case user gets here 
      if not (self.__dict__.has_key('dbType') and self.dbType) : self._setSource(self.setupSource)
      if not (self.__dict__.has_key('dbHost') and self.dbHost) : self._setSource(self.setupSource)
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbUser') and self.dbUser) : self._setSource(self.setupSource)
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbPasswd') and self.dbPasswd) : self._setSource(self.setupSource)
      if self.dbType!='sqlite' and not (self.__dict__.has_key('dbName') and self.dbName) : self._setSource(self.setupSource)
    
    return True

  def _consistencyFileCheck(self):
    self.setlog()
    if self.Source[:-4] == ".xml" :
      return self._consistencyXMLCheck()
    return True
  
  ## Check the consistensy of the XML source 
  #  @param self Object pointer
  def _consistencyXMLCheck(self):
    self.setlog()
    return True
  
  ## Process the connnection string 
  #  @param connectionString Connection string for details see TrigConfigSvc.TrigConfigSvcUtils.py
  def _setSource(self, connectionString) :
    output = connectionString
    #self.setlog()
    self._log.verbose("_setSetupSource(%s)" % (connectionString))
    
    if connectionString != 'db' :
      from TrigConfOffline.HLTConfOffline import interpretConnectionString
      connectionParameters = interpretConnectionString(connectionString)
      if connectionParameters.has_key('techno') :self.dbType = connectionParameters['techno']
      if connectionParameters.has_key('server'): self.dbHost = connectionParameters['server']
      if connectionParameters.has_key('schema'): self.dbName = connectionParameters['schema']
      if connectionParameters.has_key('user'): self.dbUser = connectionParameters['user']
      if connectionParameters.has_key('passwd'): self.dbPasswd = connectionParameters['passwd']
      if self.dbType == 'sqlite' and connectionParameters.has_key('filename'): self.dbHost = connectionParameters['filename']
    return connectionString
  
  ## Load algorithms
  #  @return HLTAlgorithm list
  def load(self) :
    self.setlog()
    hlt_setup = None
    if self.Source[:-4] !='.xml':
      hlt_setup = self._loadfromDB()
    else :
      hlt_setup = self._loadfromXML()
    
    return hlt_setup
    
  def _loadfromXML(self) :
    self.setlog()
    hlt_setup = []
    return hlt_setup
  
  ## Load algorithms from DB
  #  @return HLTAlgorithm list
  def _loadfromDB(self) :
    self.setlog()
    cursor = self.connectDB()
    hmt_id = self.getHLTMasterKeys(cursor)
    # top_algs = self.getTopAlgs(cursor, hmt_id)
    
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    
    # All component IDs
    componentIDs = []
    
    # L2 TE component IDs
    componentL2TEIDs= []
    if self.Level.upper() in ('L2','BOTH','OL'):
      # Get the L2 TE components
      level = 'L2'
      componentL2TEIDs=self.getHLTTEComponentIDs(cursor, level, hmt_id)
    
    # EF TE component IDs
    componentEFTEIDs= []
    if self.Level.upper() in ('EF','BOTH','OL'):
      # Get the EF TE components
      level = 'EF'
      componentEFTEIDs=self.getHLTTEComponentIDs(cursor, level, hmt_id)

    # Get the HLT_SETUP ID's
    hst_l2_id = hst_ef_id = -1
    if self.Level.upper() in ('L2','BOTH','OL'): hst_l2_id = self.getHLTL2SetupID(cursor, hmt_id)
    if self.Level.upper() in ('EF','BOTH','OL'): hst_ef_id = self.getHLTEFSetupID(cursor, hmt_id)
          
    # Get the upper layer components related to the L2 setup
    componentL2IDs = []
    if self.Level.upper() in ('L2','BOTH','OL'):
      componentL2IDs = self.getHLTSetupComponentIDs(cursor, hst_l2_id)
      for component_id in componentL2IDs :
        if component_id not in componentIDs : componentIDs.append(component_id)
    
    # Get the upper layer components related to the EF setup
    componentEFIDs = []
    if self.Level.upper() in ('EF','BOTH','OL'):
      if hst_ef_id == hst_l2_id and self.Level.upper()!='EF':
        componentEFIDs = componentL2IDs
      else :
        componentEFIDs = self.getHLTSetupComponentIDs(cursor, hst_ef_id)
        for component_id in componentEFIDs :
          if component_id not in componentIDs : componentIDs.append(component_id)
    
    componentIDs.sort()
    algorithms = []
    import TrigConfOffline.HLTAlgorithm as HLTAlgorithm
    trigSteer_L2 = None
    trigSteer_EF = None
    for compID in componentIDs:
      alg=HLTAlgorithm.HLTAlgorithm(dbId=compID,OutputLevel=self.OutputLevel)
      alg.loadFromDB(cursor,self.dbName)
      if compID in componentL2IDs :
        alg.setLevel('L2')
      elif compID in componentEFIDs :
        alg.setLevel('EF')
      algorithms.append(alg)
      # Search through the algorithms for the main steering alg
      for flatAlg in alg.flattenAlg() :
        if flatAlg.getName() == 'HLT::TrigSteer':
          if flatAlg.getAlias()[-2:] == 'L2':
            trigSteer_L2 = flatAlg
            self._log.debug('L2 TrigSteer Algorithm found: %s' % (trigSteer_L2))
          elif flatAlg.getAlias()[-2:] == 'EF':
            trigSteer_EF = flatAlg
            self._log.debug('EF TrigSteer Algorithm found: %s' % (trigSteer_EF))
    
    for compID in componentL2TEIDs:
      alg=HLTAlgorithm.HLTAlgorithm(dbId=compID,OutputLevel=self.OutputLevel)
      alg.loadFromDB(cursor,self.dbName)
      alg.setLevel('L2')
      if trigSteer_L2 :
        trigSteer_L2.appendChild(alg)
      else :
        algorithms.append(alg)
    
    for compID in componentEFTEIDs:
      alg=HLTAlgorithm.HLTAlgorithm(dbId=compID,OutputLevel=self.OutputLevel)
      alg.loadFromDB(cursor,self.dbName)
      alg.setLevel('EF')
      if trigSteer_EF :
        trigSteer_EF.appendChild(alg)
      else :
        algorithms.append(alg)
      
    cursor.close()
    return algorithms
  
  ## Connect to the database
  #  @return Database cursor
  def connectDB(self):
    self.setlog()
    import TrigConfOffline.HLTSQLUtils as HLTSQLUtils
    cursor = HLTSQLUtils.get_cursor(self.dbHost, self.dbUser, self.dbName, self.dbType, False, self.dbPasswd)
    return cursor
    
  ## Get the HLT_MASTER_TABLE ID
  #  @param cursor Database cursos
  #  @return int HLT_MASTER_TABLE ID - <0 not found
  def getHLTMasterKeys(self, cursor) :
    self.setlog()
    table_name = 'SUPER_MASTER_TABLE'
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      table_name = "%s.%s" % (self.dbName, table_name)
      
    # Get the HMT_ID
    query =  ""
    query += " SELECT DISTINCT %s.SMT_HLT_MASTER_TABLE_ID " % (table_name)
    query += " FROM %s " % (table_name)
    query += " WHERE %s.SMT_ID = %s " % (table_name, self.SMKey)
    
    self._log.debug(query)
    cursor.execute( query )
    result = cursor.fetchall()
    
    hmt_id = -1
    if len(result) == 1 :
      if len(result[0]) ==1 :
        hmt_id = result[0][0]
      else :
        self._log.error('No HLT Master Key found check the parameters: \n%s' % (self))
        self._log.debug(query)
        #raise HLTSetupLoaderError('No HLT Master Key found: query: %s \nCheck the parameters: \n%s' % (query,self))
    elif len(result) > 1:
        self._log.error('Multiple HLT Master Keys found check the parameters: \n%s' % (self))
        self._log.debug(query)
        #raise HLTSetupLoaderError('Multiple HLT Master Keys found: Corrupted DB: \n%s' % (self))
    else :
        self._log.error('No HLT Master Key found check the parameters: \n%s' % (self))
        self._log.debug(query)
        #raise HLTSetupLoaderError('No HLT Master Key found: query: %s \nCheck the parameters: \n%s' % (query,self))
    
    self._log.verbose("HLT Master table ID:%d" % (hmt_id))
    return hmt_id
  
  ## Get the L2 HLT_SETUP ID
  #  @param cursor Database cursos
  #  @param hmt_id HLT_MASTER_TABLE ID - default -2
  #  @return int HLT_SETUP ID - < 0 not found
  def getHLTL2SetupID(self, cursor, hmt_id=-2):
    hst_id = -1
    self.setlog()
    if hmt_id == -2: hmt_id=self.getHLTMasterKeys(cursor)
    if hmt_id<=0 : return hst_id
    
    table_name = 'HLT_MASTER_TABLE'
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      table_name = "%s.%s" % (self.dbName, table_name)
    query = ""
    query += " SELECT DISTINCT %s.HMT_L2_SETUP_ID " % (table_name)
    query += " FROM %s " % (table_name)
    if self.dbType == 'sqlite':
      query += " WHERE %s.HMT_ID=%d" % (table_name, hmt_id)
    else :
      query += " WHERE %s.HMT_ID=:hmt_id" % (table_name)
    
    self._log.verbose('%s, hmt_id=%d' % (query,hmt_id))
    if self.dbType == 'sqlite':
      cursor.execute(query)
    else :
      cursor.execute(query, hmt_id=hmt_id)
    result = cursor.fetchall()
    if len(result) == 1 :
      if len(result[0]) ==1 :
        hst_id = result[0][0]
      else :
        self._log.error('No HLT L2 Setup ID check the parameters: HMT_ID=%d \n%s' % (hmt_id,self))
        self._log.debug('%s, hmt_id=%d' % (query,hmt_id))
    elif len(result) > 1:
        self._log.error('Multiple HLT L2 Setup ID found check the parameters: HMT_ID=%d \n%s' % (hmt_id,self))
        self._log.debug('%s, hmt_id=%d' % (query,hmt_id))
    else :
        self._log.error('No HLT L2 Setup ID found check the parameters: HMT_ID=%d \n%s' % (hmt_id,self))
        self._log.debug(query)
    
    # Test the level matching
    level = self.getHLTSetupTriggerLevel(cursor, hst_id)
    if level != 'L2' :
      if level == 'OL' and self.Level.upper() not in ('BOTH', 'OL') :
        self._log.warning('L2 HLT Setup trigger level mismatch - trigger level %s required level: %s' % (level,self.Level.upper()))
      elif level != 'OL':
        self._log.warning('Unknown trigger level %s (required level %s) in HLT setup ID %d' % (level, self.Level.upper(), hst_id) )
    return hst_id
  
  ## Get the EF HLT_SETUP ID
  #  @param cursor Database cursos
  #  @param hmt_id HLT_MASTER_TABLE ID - default -2
  #  @return int HLT_SETUP ID - < 0 not found
  def getHLTEFSetupID(self, cursor, hmt_id=-2):
    hst_id = -1
    self.setlog()
    if hmt_id == -2: hmt_id=self.getHLTMasterKeys(cursor)
    if hmt_id<=0 : return hst_id
    
    table_name = 'HLT_MASTER_TABLE'
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      table_name = "%s.%s" % (self.dbName, table_name)
    query = ""
    query += " SELECT DISTINCT %s.HMT_EF_SETUP_ID " % (table_name)
    query += " FROM %s " % (table_name)
    if self.dbType == 'sqlite' :
      query += " WHERE %s.HMT_ID=%d" % (table_name, hmt_id)
    else:
      query += " WHERE %s.HMT_ID=:hmt_id" % (table_name)
      
    self._log.verbose('%s, hmt_id=%d' % (query,hmt_id))
    if self.dbType == 'sqlite' :
      cursor.execute(query)
    else :
      cursor.execute(query, hmt_id=hmt_id)
    result = cursor.fetchall()
    if len(result) == 1 :
      if len(result[0]) ==1 :
        hst_id = result[0][0]
      else :
        self._log.error('No HLT EF Setup ID check the parameters: HMT_ID=%d \n%s' % (hmt_id,self))
        self._log.debug('%s, hmt_id=%d' % (query,hmt_id))
    elif len(result) > 1:
        self._log.error('Multiple HLT EF Setup ID found check the parameters: HMT_ID=%d \n%s' % (hmt_id,self))
        self._log.debug('%s, hmt_id=%d' % (query,hmt_id))
    else :
        self._log.error('No HLT EF Setup ID found check the parameters: HMT_ID=%d \n%s' % (hmt_id,self))
        self._log.debug(query)
    
    # Test the level matching
    level = self.getHLTSetupTriggerLevel(cursor, hst_id)
    if level != 'EF' :
      if level == 'OL' and self.Level.upper() not in ('BOTH', 'OL') :
        self._log.warning('EF HLT Setup trigger level mismatch - trigger level %s required level: %s' % (level,self.Level.upper()))
      elif level != 'OL':
        self._log.warning('Unknown trigger level %s (required level %s) in HLT setup ID %d' % (level, self.Level.upper(), hst_id) )
        
    return hst_id
  
  ## Get the HLT_SETUP trigger level
  #  @param cursor DB connection cursor
  #  @param hst_id HLT_SETUP ID
  #  @return String - in HLT_SETUP.HST_L2_OR_EF
  def getHLTSetupTriggerLevel(self, cursor, hst_id) :
    level = ''
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    
    # Get the upper layer components related to the EF setup
    query  = ' SELECT DISTINCT %sHLT_SETUP.HST_L2_OR_EF ' % (prepend)
    query += ' FROM %sHLT_SETUP ' % (prepend)
    if self.dbType == 'sqlite':
      query += ' WHERE %sHLT_SETUP.HST_ID=%d ' % (prepend, hst_id)
    else:
      query += ' WHERE %sHLT_SETUP.HST_ID=:hst_id ' % (prepend)
    
    self._log.verbose('%s, hst_id=%d' % (query, hst_id))
    if self.dbType == 'sqlite':
      cursor.execute(query)
    else:
      cursor.execute(query, hst_id=hst_id)
    result = cursor.fetchall()
    if len(result) == 1 :
      if len(result[0]) ==1 :
        level = str(result[0][0]).upper()
      else :
        self._log.error('No HLT Setup Level found check the parameters: HST_ID=%d \n%s' % (hst_id,self))
        self._log.debug('%s, hst_id=%d' % (query,hst_id))
    elif len(result) > 1:
        self._log.error('Multiple HLT Setup Levels found check the parameters: HST_ID=%d \n%s' % (hst_id,self))
        self._log.debug('%s, hst_id=%d' % (query,hst_id))
    else :
        self._log.error('No HLT EF Setup Level found check the parameters: HMT_ID=%d \n%s' % (hmt_id,self))
        self._log.debug(query)
    
    return level
  
  ## Get the HLT_SETUP parent components ID's
  #  @param cursor DB connection cursor
  #  @param hst_id HLT_SETUP ID
  #  @return List of the component ID's
  def getHLTSetupComponentIDs(self, cursor, hst_id):
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    
    algIDs = []
    # Get the upper layer components related to the EF setup
    query  = ' SELECT DISTINCT %sHLT_COMPONENT.HCP_ID ' % (prepend)
    query += ' FROM %sHLT_SETUP ' % (prepend)
    query += ' JOIN %sHLT_ST_TO_CP ON (%sHLT_ST_TO_CP.HST2CP_SETUP_ID = %sHLT_SETUP.HST_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_COMPONENT ON (%sHLT_COMPONENT.HCP_ID = %sHLT_ST_TO_CP.HST2CP_COMPONENT_ID) ' % (prepend, prepend, prepend)
    if self.dbType == 'sqlite':
      query += ' WHERE %sHLT_SETUP.HST_ID=%d ' % (prepend, hst_id)
    else:
      query += ' WHERE %sHLT_SETUP.HST_ID=:hst_id ' % (prepend)
      
    self._log.verbose('%s, hst_id=%d' % (query, hst_id))
    if self.dbType == 'sqlite':
      cursor.execute(query)
    else:
      cursor.execute(query, hst_id=hst_id)
    result = cursor.fetchall()
    for column in result:
      algIDs.append(column[0])
    
    return algIDs
  
  ## Get the HLT_TRIGGER_MENU parent components ID's
  #  @param cursor DB connection cursor
  #  @param hmt_id HLT_MASTER_TABLE ID
  #  @param level Trigger Level - EF/L2
  #  @return List of the HLT_COMPONENTs ID's
  def getHLTTEComponentIDs(self, cursor, level, hmt_id=-2):
    componentIDs = []
    self.setlog()
    if hmt_id==-2: hmt_id=self.getHLTMasterKeys(cursor)
    if hmt_id<=0 : return componentIDs
    
    prepend=''
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      prepend = '%s.' % (self.dbName)
    
    # Get the upper layer components related to the trigger elements for EF
    query  = ' SELECT DISTINCT %sHLT_COMPONENT.HCP_ID ' % (prepend)
    query += ' FROM %sHLT_MASTER_TABLE ' % (prepend)
    query += ' JOIN %sHLT_TRIGGER_MENU ON (%sHLT_TRIGGER_MENU.HTM_ID = %sHLT_MASTER_TABLE.HMT_TRIGGER_MENU_ID) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_TM_TO_TC ON ( %sHLT_TM_TO_TC.HTM2TC_TRIGGER_MENU_ID = %sHLT_TRIGGER_MENU.HTM_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_TRIGGER_CHAIN ON ( %sHLT_TM_TO_TC.HTM2TC_TRIGGER_CHAIN_ID = %sHLT_TRIGGER_CHAIN.HTC_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_TC_TO_TS ON ( %sHLT_TC_TO_TS.HTC2TS_TRIGGER_CHAIN_ID = %sHLT_TRIGGER_CHAIN.HTC_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_TRIGGER_SIGNATURE ON ( %sHLT_TC_TO_TS.HTC2TS_TRIGGER_SIGNATURE_ID = %sHLT_TRIGGER_SIGNATURE.HTS_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_TS_TO_TE ON ( %sHLT_TS_TO_TE.HTS2TE_TRIGGER_SIGNATURE_ID = %sHLT_TRIGGER_SIGNATURE.HTS_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_TRIGGER_ELEMENT ON ( %sHLT_TS_TO_TE.HTS2TE_TRIGGER_ELEMENT_ID = %sHLT_TRIGGER_ELEMENT.HTE_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_TE_TO_CP ON ( %sHLT_TE_TO_CP.HTE2CP_TRIGGER_ELEMENT_ID = %sHLT_TRIGGER_ELEMENT.HTE_ID ) ' % (prepend, prepend, prepend)
    query += ' JOIN %sHLT_COMPONENT ON (  %sHLT_COMPONENT.HCP_ID = %sHLT_TE_TO_CP.HTE2CP_COMPONENT_ID) ' % (prepend, prepend, prepend)
    if self.dbType == 'sqlite' :
      query += ' WHERE %sHLT_MASTER_TABLE.HMT_ID=%d ' % (prepend, hmt_id)
    else :
      query += ' WHERE %sHLT_MASTER_TABLE.HMT_ID=:hmt_id ' % (prepend)
    query += ' AND %sHLT_TRIGGER_CHAIN.HTC_L2_OR_EF=\'%s\' ' % (prepend, level)
    
    self._log.verbose('%s, hmt_id=%d' % (query, hmt_id))
    if self.dbType == 'sqlite' :
      cursor.execute(query)
    else :
      cursor.execute(query, hmt_id=hmt_id)
    result = cursor.fetchall()
    for column in result:
      componentIDs.append(column[0])
    
    return componentIDs
  
  ## Get the Top Algorithms from ApplicationMgr.TopAlg parameter
  #  @param cursor DB connection cursor
  #  @param hmt_id HLT_MASTER_TABLE ID
  #  @return List of the TopAlg names
  def getTopAlgs(self, cursor, hmt_id) :
    self.setlog()
    view_name = "HLT_MK_TL_TO_PA_VIEW"
    if self.dbType == 'oracle' and (self.dbUser != self.dbName) :
      view_name = "%s.%s" % (self.dbName, view_name)
    # Get the TopAlgorithms
  
    db_levels = (self.Level)
    if db_levels == ('BOTH'):
      db_levels = ('L2', 'EF')
    top_algs = None
    
    top_algs = {}
        
    for l in db_levels :
      query=""
      query += " SELECT %s.HPA_VALUE " % (view_name)
      query += " FROM %s " % (view_name)
      query += " WHERE %s.HMT_ID IN (%s) " % (view_name, hmt_id)
      query += " AND %s.HST_L2EF =\'%s\' " % (view_name, l)
      query += " AND %s.HCP_NAME =\'ApplicationMgr\' " % (view_name)
      query += " AND %s.HCP_ALIAS =\'ApplicationMgr\' " % (view_name)
      query += " AND %s.HPA_NAME =\'TopAlg\' " % (view_name)
      
      self._log.debug(query)
      cursor.execute( query )
      result = cursor.fetchall()
      
      if len(result) > 0 and len(result[0]) == 1:
        top_algs[l] = eval(result[0][0])
      else :
        # Try Offline setup, send warning
        self._log.warning('No Top Algorithms were found for level %s try OL ' % (l))
        query=""
        query += " SELECT %s.HPA_VALUE " % (view_name)
        query += " FROM %s " % (view_name)
        query += " WHERE %s.HMT_ID IN (%s) " % (view_name, hmt_id)
        query += " AND %s.HST_L2EF =\'%s\' " % (view_name, 'OL')
        query += " AND %s.HCP_NAME =\'ApplicationMgr\' " % (view_name)
        query += " AND %s.HCP_ALIAS =\'ApplicationMgr\' " % (view_name)
        query += " AND %s.HPA_NAME =\'TopAlg\' " % (view_name)
      
        self._log.debug(query)
        cursor.execute( query )
        result = cursor.fetchall()
        
        if len(result) > 0 and len(result[0]) == 1:
          top_algs[l] = eval(result[0][0])
        else:
          self._log.error('No Top Algorithms were found for level OL')
        #raise HLTSetupLoaderError('No Top Algorithms were found')
      
    return top_algs
    
  def findTopAlg(self, top_algs, level, algorithm) :
    self.setlog()
    topalg = None
  
    if len(top_algs[level]) == 1:
      topalg = top_algs[level][0]
    elif len(top_algs[level]) > 1:
      # Check whether the algorithm isn't top alg itself
      fullalgname = "%s/%s" % (algorithm.getName(),algorithm.getAlias())
      if fullalgname in top_algs[level] : return fullalgname
      #look for the HLT::TrigSteer Top Alg
      hltsteer_topalg = []
      for n in top_algs[level]:
        if n.split('/')[0] == 'HLT::TrigSteer' and n.split('/')[1][-2:].upper() == level:
          if n not in hltsteer_topalg:
            hltsteer_topalg.append(n)
      
      if len(hltsteer_topalg) != 1:
        if len(hltsteer_topalg) == 0:
          self._log.warning('No HLT::TrigSteer top algorithm found')
        else :
          self._log.warning('Multiple HLT::TrigSteer top algorithms found: %s' % (hltsteer_topalg))
        return None
      
      # Test the algorithm for presence of the properties doTiming, AthenaMonTools
      if algorithm.getProperties() :
        if algorithm.properties.has_key('doTiming') and algorithm.properties.has_key('AthenaMonTools') :
          topalg = hltsteer_topalg[0]
    return topalg





































