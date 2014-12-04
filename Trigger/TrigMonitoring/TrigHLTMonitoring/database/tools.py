# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def is_non_zero_file(fpath):  
    import os
    #return True if os.path.isfile(fpath) and os.path.getsize(fpath) > 0 else False
    #return True if os.path.isfile(fpath) and os.stat(fpath).st_size > 0 else False
    if os.path.exists(fpath):
        fi=open(fpath)
        if not fi.read(3):  #avoid reading entire file.
            print "Dictionary file is empty!"
            fi.close()
            return False
        elif fi.readline().isspace() or fi.readline()=="" or fi.readline()==" " or fi.readline()=="\n" or fi.readline()=="\0":
             print "Dictionary file contains only empty spaces!"
             fi.close()
             return False
        else:
             fi.close()
             return True
    else:
        print "Dictionary file does not exist!"
        return False

def getMaxMCK(dbconnection, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()

      c.execute("SELECT max(mt_id) FROM master_table")

      for x in c.fetchall():
          #print x[0]
          max_id = x[0]

      conn.close()
      return max_id 


def getMaxTypeId(dbconnection, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()

      c.execute("SELECT max(tt_id) FROM type_table")

      for x in c.fetchall():
          #print x[0]
          max_id = x[0]

      conn.close()
      return max_id 

def getMaxConfigId(dbconnection, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()

      c.execute("SELECT max(sc_id) FROM sig_config")

      for x in c.fetchall():
          #print x[0]                                                                                                 
          max_id = x[0]

      conn.close()
      return max_id

 
def getActiveKeyMCK(dbconnection, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()

      # get first active key from COOL
      # look at the table of correspondence COOL SMK <--> MCK
      # return MCK
      # for now let's return the last MCK in the table
      # all before was not correct
      # we take an active key MCK as the last entry in the table activeMCK_table

      #c.execute("SELECT max(mt_id) FROM master_table")
      c.execute("SELECT max(activeMCK_id) FROM activeMCK_table") 

      for x in c.fetchall():
          #print x[0]
           max_active_MCK_ID = x[0]

      c.execute("SELECT activeMCK FROM activeMCK_table WHERE activeMCK_id=:my_MCK_id",my_MCK_id=max_active_MCK_ID)
      for x in c.fetchall():
         #print x[0]
         active_MCK=x[0]    
     
      conn.close()
      return active_MCK

def getConfigForMCK(dbconnection, MCK, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()


      RunType=[]
      ConfigId=[]
      ConfigName=[]
      ConfigRecostep=[]
      ConfigCode=[]
      ConfigCodeHash=[]
      ConfigScript=[]
      ConfigScriptHash=[]


      # we have to get CLOBS separately, when fetchall they get spoiled... known problem with cx_Oracle
      c.execute("SELECT sc_data FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=MCK)

      for value, in c:
        #print "Doing some manipulation with value", value.read()
        ConfigCode.append(eval(str(value)))

      #print "here 2"

      c.execute("SELECT sc_script FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=MCK)

      for script_value, in c:
        ConfigScript.append(str(script_value))


      c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=MCK)


      for x in c.fetchall():
         #print "Printing...", x
         RunType.append(x[6])
         ConfigId.append(x[9])
         ConfigName.append(x[10])
         ConfigRecostep.append(x[11])
         ConfigCodeHash.append(x[14])
         ConfigScriptHash.append(x[15])

      conn.close()
      return RunType,ConfigId,ConfigName,ConfigRecostep,ConfigCode,ConfigScript,ConfigCodeHash,ConfigScriptHash


def getConfigCodeForMCK(dbconnection, MCK, runtype, slice_name, reco_step, *vargs, **kwargs):
    runType=[]
    configId=[]     
    configName=[]
    configRecostep=[]
    configCode=[]
    configScript=[]
    configCodeHash=[]
    configScriptHash=[]

    runType,configId,configName,configRecostep,configCode,configScript,configCodeHash,configScriptHash=getConfigForMCK(dbconnection, MCK)      
    for index,conf_name in enumerate(configName):

       if runType[index]==runtype and conf_name==slice_name and configRecostep[index]==reco_step:
          return configId[index], configCode[index], configScript[index],configCodeHash[index],configScriptHash[index]
       #else:
       #   print "No such configuration in the active key configuration!"  

def getTypeIDForMCK(dbconnection, MCK, RunType, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()

      TypeID=[]
      runType=[]

      c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=MCK)


      for x in c.fetchall():
         #print "Printing...", x
         TypeID.append(x[5])
         runType.append(x[6])
 
      conn.close()

      for i,type in enumerate(runType):
           if type==RunType:
              return TypeID[i]


def getAvailableTypeIDsForGivenConfig(dbconnection, config_id, run_type, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()
     
      TypeID=[]
      myTypeID=[]
      runType=[]

      c.execute("SELECT * FROM type_table, type2sig, sig_config WHERE type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and sig_config.sc_id=:my_config_id", my_config_id=config_id)
      
      for x in c.fetchall():
         #print "Printing...", x
         TypeID.append(x[0])
         runType.append(x[1])
      
      for i,type in enumerate(runType):
           if type==run_type:
              myTypeID.append(TypeID[i])
       
      if len(myTypeID)>0:
          return myTypeID
      else:
          return 0
     


def getConfigIdForMCK(dbconnection, MCK, runtype, slice_name, reco_step, *vargs, **kwargs):
    runType=[]
    configId=[]     
    configName=[]
    configRecostep=[]
    configCode=[]
    configScript=[]
    configCodeHash=[]
    configScriptHash=[]

    runType,configId,configName,configRecostep,configCode,configScript,configCodeHash,configScriptHash=getConfigForMCK(dbconnection, MCK)      
    for index,conf_name in enumerate(configName):
       if runType[index]==runtype and conf_name==slice_name and configRecostep[index]==reco_step:
          return configId[index]
       #else:
       #   print "No such configuration in the active key configuration!"  

       

def equalConfigs(config1,config2,*vargs, **kwargs):
     # 1 is dictionary; 2 is database
     CHANGED_CONFIG=0 # 1 if at least 1 parameter changed, or if at least 1 new parameter for a certain tool was introduced (i.e. changed in comparison to the standard value), or if a new tool was introduced in dictionary which is not in the database
     CHANGED_TRIGGERS=0 # 1 if monitoring triggers have changed
     CHANGED_COMBINED=0 # 1 if any of ToolConfig or MonitoringTriggers has changed 
     
     if config1==None or config2==None:
       if config1==config2:
           return 1
       else:
           return 0
     else:
       from AthenaCommon.AppMgr import ToolSvc
       exec "from %s import %s" % (config1["PkgName"],config1["CreatorName"])
       exec "%s()" % (config1["CreatorName"])
       #print config1["PkgName"],config1["CreatorName"] 


       toolname_list2=[]
       for (toolname2, toolconfig2) in config2['Config']:
         toolname_list2.append(toolname2)

       for (toolname1, toolconfig1) in config1['Config']:
          if toolname1 not in toolname_list2:
             print "New tool in dictionary was introduced!"
             CHANGED_CONFIG=1
             break
          else:
             for (toolname2, toolconfig2) in config2['Config']:
                if toolname2==toolname1:
                   tool=getattr(ToolSvc,toolname1)
                   for (confattr1, confvalue1) in toolconfig1.items():
                       if confattr1 not in toolconfig2:
                           print "New parameter for some tool was introduced!"
                           # we should check if such parameter exists for a certain tool
                           # if it exists then we should compare to the value of an attribute (if it is an attribute of a tool) or we should compare its values to default values (if it is not an attribute of a tool)
                           if hasattr(tool,confattr1):
                              if confvalue1==getattr(tool,confattr1):
                                 CHANGED_CONFIG=0  
                                 # here we put "None" in the config code of the DB if all values are like in the release or like default values (see isDictionaryLikeInRelease and readDictionaries) 
                                 continue
                              else:
                                 CHANGED_CONFIG=1
                                 break  
                           elif tool.getDefaultProperty(confattr1):
                               if confvalue1==tool.getDefaultProperty(confattr1):
                                 CHANGED_CONFIG=0
                                 # here we should put "None" in the config code of the DB if all values are like in the release or like default values (see isDictionaryLikeInRelease and readDictionaries)   
                                 continue
                               else:
                                 CHANGED_CONFIG=1
                                 break
                           else:
                             print "Newly introduced parameter for some tool does not exist! Although this situation should not occur because it was checked in validateDictionary(dictionary) already.." 
                             CHANGED_CONFIG=1
                             break
                       else:
                           if toolconfig2[confattr1]==confvalue1:
                              CHANGED_CONFIG=0
                              continue
                           else:
                              # 1 if the configuration in 'Config' part has changed
                              CHANGED_CONFIG=1
                              break
                if CHANGED_CONFIG!=0:
                    break
             if CHANGED_CONFIG!=0:
                break



       # get monitoring triggers from config1 and config2 and sort them
       monitoring_triggers1 = sorted(config1['MonitoringTriggers'])
       monitoring_triggers2 = sorted(config2['MonitoringTriggers'])
     
       #print monitoring_triggers1
       #print monitoring_triggers2

       if monitoring_triggers1==monitoring_triggers2:
         CHANGED_TRIGGERS=0
       else:
         CHANGED_TRIGGERS=1

       CHANGED_COMBINED = CHANGED_CONFIG or CHANGED_TRIGGERS
       #print "CHANGED_CONFIG ", CHANGED_CONFIG
       #print "CHANGED_TRIGGERS ", CHANGED_TRIGGERS
       return not(CHANGED_COMBINED)
       

def searchSimilarConfigsInDB(dbconnection, slice_name, reco_step, config_code, config_code_hash, *vargs, **kwargs):
    # we search similar configs first according to hash code of config code (we don't take into account script comparison yet)
    # then for these configs we compare slice_name, reco_step
    # then for left config array we check all parameters by hand
    # and for the left config array we return config ID of the last entry in the DB ( can happen that there are several identical entries in the DB, but, in principle, should not be the case!!! )
    if dbconnection=='oracle':
       import cx_Oracle
       import sys
       conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
       c = conn.cursor()

       #c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id", my_MCK=MCK)
       c.execute("SELECT * FROM sig_config WHERE sig_config.data_hash=:my_data_hash", my_data_hash=config_code_hash)


       equal_hash_configs=[]
       equal_hash_Ids=[]


       for x in c.fetchall():
          #print "Printing...", x
          if x[1]==slice_name and x[2]==reco_step:
             equal_hash_configs.append(eval(str(x[3]))) 
             equal_hash_Ids.append(x[0])
        
       #print equal_hash_configs      
       #print equal_hash_Ids

       # now we want to compare config parameters to be sure that they are equal. Although hasgh codes are the same it can happen that configs are not the same, probability of such a situation is very low..

       equal_configs=[]
       equal_Ids=[]

       if len(equal_hash_configs)!=0:
          for i,db_config in enumerate(equal_hash_configs):
             if equalConfigs(config_code,db_config):        
                equal_configs.append(db_config)
                equal_Ids.append(equal_hash_Ids[i])

       conn.close()

       #get config with the last Id 
       if len(equal_configs)!=0:
          return equal_Ids[-1]
       else:
          return 0       


def printConfig(dbconnection, MCK, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()

      if MCK=='ACTIVE_KEY':
         current_MCK=getActiveKeyMCK('oracle')
      elif MCK=='LAST_MCK':
         current_MCK=getMaxMCK('oracle')
      else:
         current_MCK=MCK

      MasterKey=[]
      MTconnect1=[]
      MTconnect2=[]
      TypeId=[]
      RunType=[]
      t2s_connect1=[]
      t2s_connect2=[]
      ConfigId=[]
      ConfigName=[]
      ConfigRecostep=[]
      ConfigCode=[]
      ConfigCodeHash=[]
      ConfigScript=[]
      ConfigScriptHash=[]

      c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=current_MCK)


      for x in c.fetchall():
         #print "Printing...", x
         MasterKey.append(x[0])
         MTconnect1.append(x[3])
         MTconnect2.append(x[4])
         TypeId.append(x[5])
         RunType.append(x[6])
         t2s_connect1.append(x[7])
         t2s_connect2.append(x[8])
         ConfigId.append(x[9])
         ConfigName.append(x[10])
         ConfigRecostep.append(x[11])
         ConfigCodeHash.append(x[14])
         ConfigScriptHash.append(x[15])

      slice_list = ['TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','IDTRK','IDJPSI','CALORAW','CALOESD']

      print "MCK: ", MasterKey[0]
      print " Physics: ", TypeId[RunType.index('Physics')]
      for ii in slice_list:
        for i, item in enumerate(ConfigId):
          if RunType[i]=='Physics':
            if ConfigName[i]==ii: 
              print ('{0:6} {1:10} {2:10}'.format(item,ii,ConfigRecostep[i]))

      print " Standby: ", TypeId[RunType.index('Standby')]
      for ii in slice_list:
       for i, item in enumerate(ConfigId):
         if RunType[i]=='Standby':
            if ConfigName[i]==ii: 
              print ('{0:6} {1:10} {2:10}'.format(item,ii,ConfigRecostep[i]))

      print " Cosmics: ", TypeId[RunType.index('Cosmics')]
      for ii in slice_list:
        for i, item in enumerate(ConfigId):
          if RunType[i]=='Cosmics':
            if ConfigName[i]==ii: 
              print ('{0:6} {1:10} {2:10}'.format(item,ii,ConfigRecostep[i]))

      conn.close()
      return None

def printAllConfig(dbconnection, MCK, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()

      if MCK=='ACTIVE_KEY':
         current_MCK=getActiveKeyMCK('oracle')
      elif MCK=='LAST_MCK':
         current_MCK=getMaxMCK('oracle')
      else:
         current_MCK=MCK

      MasterKey=[]
      MTconnect1=[]
      MTconnect2=[]
      TypeId=[]
      RunType=[]
      t2s_connect1=[]
      t2s_connect2=[]
      ConfigId=[]
      ConfigName=[]
      ConfigRecostep=[]
      ConfigCode=[]
      ConfigCodeHash=[]
      ConfigScript=[]
      ConfigScriptHash=[]

      #maxMCK=getMaxMCK('oracle')


      # we have to get CLOBS separately, when fetchall they get spoiled... known problem with cx_Oracle
      c.execute("SELECT sc_data FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=current_MCK)

      for value, in c:
        #print "Doing some manipulation with value", value.read()
        ConfigCode.append(eval(str(value)))

      c.execute("SELECT sc_script FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=current_MCK)

      for script_value, in c:
        ConfigScript.append(str(script_value))


      c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=current_MCK)


      for x in c.fetchall():
         #print "Printing...", x
         MasterKey.append(x[0])
         MTconnect1.append(x[3])
         MTconnect2.append(x[4])
         TypeId.append(x[5])
         RunType.append(x[6])
         t2s_connect1.append(x[7])
         t2s_connect2.append(x[8])
         ConfigId.append(x[9])
         ConfigName.append(x[10])
         ConfigRecostep.append(x[11])
         ConfigCodeHash.append(x[14])
         ConfigScriptHash.append(x[15])


      slice_list = ['TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','IDTRK','IDJPSI','CALORAW','CALOESD']

      print "MCK: ", MasterKey[0]
      print "============"
      print "Physics: ", TypeId[RunType.index('Physics')]
      print "############"

      for ii in slice_list:
        for i, item in enumerate(ConfigId):
          if RunType[i]=='Physics':
            if ConfigName[i]==ii:
              print "===> ",ConfigId[i], " ", ConfigName[i], " ", ConfigRecostep[i], " ", ConfigCode[i], " ", ConfigScript[i]

      print "Standby: ", TypeId[RunType.index('Standby')]
      print "############"

      for ii in slice_list:
        for i, item in enumerate(ConfigId):
          if RunType[i]=='Standby':
            if ConfigName[i]==ii:
              print "===> ",ConfigId[i], " ", ConfigName[i], " ", ConfigRecostep[i], " ", ConfigCode[i], " ", ConfigScript[i]

      print "Cosmics: ", TypeId[RunType.index('Cosmics')]
      print "############"

      for ii in slice_list:
        for i, item in enumerate(ConfigId):
          if RunType[i]=='Cosmics':
            if ConfigName[i]==ii:
              print "===> ",ConfigId[i], " ", ConfigName[i], " ", ConfigRecostep[i], " ", ConfigCode[i], " ", ConfigScript[i]
              
      conn.close()
      return None


def printDB(dbconnection, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()


      MasterKey=[]
      MTconnect1=[]
      MTconnect2=[]
      TypeId=[]
      RunType=[]
      t2s_connect1=[]
      t2s_connect2=[]
      ConfigId=[]
      ConfigName=[]
      ConfigRecostep=[]
      ConfigCode=[]
      ConfigCodeHash=[]
      ConfigScript=[]
      ConfigScriptHash=[]

      c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt2type.type_id1=type_table.tt_id and mt2type.master_id=mt_id and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id")

      for x in c.fetchall():
         #print "Printing...", x
         MasterKey.append(x[0])
         TypeId.append(x[5])
         RunType.append(x[6])
         ConfigId.append(x[9])
         ConfigName.append(x[10])
         ConfigRecostep.append(x[11])
 
      MCK_list=[]
      TypeId_list=[]

       # values in MasterKey list are repeating
       #we want to produce a list of unique values
      for i,value in enumerate(MasterKey):
         if value not in MCK_list:
           MCK_list.append(value)


      slice_list = ['TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','IDTRK','IDJPSI','CALORAW','CALOESD']


      MCK_list=[]

      # values in MasterKey list are repeating                                                                    
      #we want to produce a list of unique values                                                                 

      for i,value in enumerate(MasterKey):
         if value not in MCK_list:
           MCK_list.append(value)

      active_MCK=getActiveKeyMCK('oracle')

      physicsList=[]
      standbyList=[]
      cosmicsList=[]

       # get sorted lists of types
      for mck in sorted(MCK_list):
           for i,item in enumerate(TypeId):
               if MasterKey[i]==mck: 
                  if RunType[i]=='Physics':
                     physicsList.append(item)
                     break


      for mck in sorted(MCK_list):
           for i,item in enumerate(TypeId):
               if MasterKey[i]==mck: 
                  if RunType[i]=='Standby':
                     standbyList.append(item)
                     break

      for mck in sorted(MCK_list):
           for i,item in enumerate(TypeId):
               if MasterKey[i]==mck: 
                  if RunType[i]=='Cosmics':
                     cosmicsList.append(item)
                     break

      #print physicsList
      #print standbyList
      #print cosmicsList

      for mck_index,mck in enumerate(sorted(MCK_list)):
        if mck==active_MCK:
           print "ACTIVE MCK: ",mck
           print "==============="
        else:
           print "MCK: ",mck
           print "======="
        print "Physics: ",physicsList[mck_index]
        for ii in slice_list:
          for i,item in enumerate(ConfigId):
            if MasterKey[i]==mck:
              if TypeId[i]==physicsList[mck_index]:
                if ConfigName[i]==ii:
                     print ('{0:6} {1:10} {2:10}'.format(item,ConfigName[i],ConfigRecostep[i]))

        print "Standby: ",standbyList[mck_index]
        for ii in slice_list:
          for i,item in enumerate(ConfigId):
            if MasterKey[i]==mck:
              if TypeId[i]==standbyList[mck_index]:
                if ConfigName[i]==ii:
                     print ('{0:6} {1:10} {2:10}'.format(item,ConfigName[i],ConfigRecostep[i]))

        print "Cosmics: ",cosmicsList[mck_index]
        for ii in slice_list:
          for i,item in enumerate(ConfigId):
            if MasterKey[i]==mck:
              if TypeId[i]==cosmicsList[mck_index]:
                if ConfigName[i]==ii:
                     print ('{0:6} {1:10} {2:10}'.format(item,ConfigName[i],ConfigRecostep[i]))
        
      conn.close()
      return None


def printMCKandTypes(dbconnection, *vargs, **kwargs):
  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()


      MasterKey=[]
      MTconnect1=[]
      MTconnect2=[]
      TypeId=[]
      RunType=[]
      t2s_connect1=[]
      t2s_connect2=[]
      ConfigId=[]
      ConfigName=[]
      ConfigRecostep=[]
      ConfigCode=[]
      ConfigCodeHash=[]
      ConfigScript=[]
      ConfigScriptHash=[]

      c.execute("SELECT * FROM master_table, mt2type, type_table WHERE mt2type.type_id1=type_table.tt_id and mt2type.master_id=mt_id")


      for x in c.fetchall():
         #print "Printing...", x
         MasterKey.append(x[0])
         TypeId.append(x[5])
         RunType.append(x[6])

      MCK_list=[]

      # values in MasterKey list are repeating
      #we want to produce a list of unique values

      for i,value in enumerate(MasterKey):
         if value not in MCK_list:
           MCK_list.append(value)

      active_MCK=getActiveKeyMCK('oracle')

      for mck in sorted(MCK_list):
        if mck==active_MCK:
           print "ACTIVE MCK: ",mck
        else:
           print "MCK: ",mck
        for i,item in enumerate(MasterKey):
           if item==mck:
              print "Type ID: ",TypeId[i], " Run Type: ", RunType[i]
 
      conn.close()
      return 0

def validateDictionary(dictionary, slice_name, slice_match_pattern, *vargs, **kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    import re
    # here we need to compare PkgName and CreatorName to patterns to be sure we take the right ones
    if re.search(slice_match_pattern,dictionary["PkgName"],re.I) and re.search(slice_match_pattern,dictionary["CreatorName"],re.I):
       pass
    else:
       print "PkgName or CreatorName are wrong! Look at ", dictionary["PkgName"], " ", dictionary["CreatorName"]
       return 0
      
    # because for Calo tools there are couple of flags defined in the release we have to consider CALO case separately to be able to call all tools for CALO for RAW and ESD steps:

    from AthenaMonitoring.DQMonFlags import DQMonFlags

    if slice_name=='CALORAW':
      DQMonFlags.monManEnvironment = 'tier0Raw'
    elif slice_name=='CALOESD':   
      DQMonFlags.monManEnvironment = 'tier0ESD'
    else:
      DQMonFlags.monManEnvironment = 'tier0ESD'

    exec "from %s import %s" % (dictionary["PkgName"],dictionary["CreatorName"])
    exec "%s()" % (dictionary["CreatorName"])
    #print dictionary["PkgName"],dictionary["CreatorName"]
    for (toolname, toolconfig) in dictionary["Config"]:
        #print "toolname ", toolname
        #exec "hasattr(ToolSvc,%s)" % (toolname)
        #print "HERE! ", hasattr(ToolSvc,toolname)

        # here we need to compare tool names to patterns to be sure we take the right ones
        if re.search(slice_match_pattern,toolname,re.I):
           pass
        else:
           print "Tool name is wrong! Look at ", dictionary["CreatorName"]
           return 0
        if hasattr(ToolSvc,toolname):
           tool=getattr(ToolSvc,toolname)
           for confattr,confvalue in toolconfig.items(): 
             if hasattr(tool,confattr):
                continue
             elif tool.getDefaultProperty(confattr):
                continue
             else:
                print "There is no parameter ", confattr, " in tool ", toolname, " Check the name of the tool parameter!"
                return 0
        else:
           print "There is no tool ", toolname, " in ToolSvc. Check the name of the tool!"
           return 0  

    return 1


def isDictionaryLikeInRelease(dictionary, *vargs, **kwargs):
    # here we check if parameters in dictionary have values like in release or default values, if yes we return 1, else 0
    # we suppose that dictionary is already validated and all tool names and parameter names are correct and dictionary is not empty 
    from AthenaCommon.AppMgr import ToolSvc
    exec "from %s import %s" % (dictionary["PkgName"],dictionary["CreatorName"])
    exec "%s()" % (dictionary["CreatorName"])
    #print dictionary["PkgName"],dictionary["CreatorName"]
    for (toolname, toolconfig) in dictionary["Config"]:
        if hasattr(ToolSvc,toolname):
           tool=getattr(ToolSvc,toolname)
           #if toolname=="HLTTauMon":
           #print tool
           for confattr,confvalue in toolconfig.items(): 
             if hasattr(tool,confattr):
                #print "dictionary value ", toolname, confattr, confvalue  
                #print "release value", getattr(tool,confattr)
                if getattr(tool,confattr)==confvalue:
                  continue
                else:
                  return 0
             elif tool.getDefaultProperty(confattr):
                #print "dictionary value ", toolname, confattr, confvalue
                #print "default value", tool.getDefaultProperty(confattr)
                if tool.getDefaultProperty(confattr)==confvalue:
                  continue
                else:
                  return 0   
#             else:
#                print "There is no parameter ", confattr, " in tool ", toolname, " Check the name of the tool parameter!"
#                return 0
#        else:
#           print "There is no tool ", toolname, " in ToolSvc. Check the name of the tool!"
#           return 0  

    return 1

 

def searchSimilarTypeIdInDB(dbconnection, run_type, configIdList, *vargs, **kwargs):
    if dbconnection=='oracle':
       import cx_Oracle
       import sys
       conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
       c = conn.cursor()
       #print "configIdList", configIdList

       in_clause = ', '.join([':id%d' % x for x in xrange(len(configIdList))])

       sql = 'SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE sig_config.sc_id in (%s) and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt_id=mt2type.master_id' % in_clause

       c.execute(sql,configIdList)

       run_type_id_array=[]
       slice_name_array=[]
       config_id_array=[]
       run_type_array=[]

       run_type_id_array_fortype=[]
       slice_name_array_fortype=[]
       config_id_array_fortype=[]
       run_type_array_fortype=[]


       for x in c.fetchall():
          #print "Printing...", x
           if len(x)>0:
             run_type_array.append(x[6])
             run_type_id_array.append(x[5])
             slice_name_array.append(x[10])
             config_id_array.append(x[9])
           else:
             return 0

       for i, rt in enumerate(run_type_array):
           if rt==run_type:
              run_type_array_fortype.append(rt)
              slice_name_array_fortype.append(slice_name_array[i])
              config_id_array_fortype.append(config_id_array[i])
              run_type_id_array_fortype.append(run_type_id_array[i])


       #print "run_type_id_array ", run_type, " ", run_type_id_array_fortype
       #print "run_type_array ", run_type, " ", run_type_array_fortype
       #print "slice_name_array_fortype ", slice_name_array_fortype
       #print "config_id_array ", config_id_array_fortype

       conn.close()
       tmp_list=[]

       for i,value in enumerate(run_type_id_array_fortype):
         if value not in tmp_list:
           tmp_list.append(value)

       #print "tmp_list ", tmp_list

       # we got the list of all type IDs contained in run_type_id_array
       
       slice_list = ['TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','IDTRK','IDJPSI','CALORAW','CALOESD']

       array_of_my_slice_array=[]


       for i in tmp_list:
       # fill array of slice names for these type IDs
          my_slice_array=[]
          for ii, v1 in enumerate(run_type_id_array_fortype):
              if v1==i:
                 my_slice_array.append(slice_name_array_fortype[ii])
          array_of_my_slice_array.append(my_slice_array)
          #print " my_slice_array ", my_slice_array

       #print "slice list ", slice_list
       for i, arr in enumerate(array_of_my_slice_array):
         #print "arr.. ", arr
         if sorted(slice_list)==sorted(arr):
             #print "found similar type ID ", tmp_list[i]
             return tmp_list[i]

       #print "no similar type.."
       return 0

def getMCKforTypeID(dbconnection, run_type_id, *vargs, **kwargs):
    if dbconnection=='oracle':
       import cx_Oracle
       import sys
       conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
       c = conn.cursor()

       c.execute("SELECT * FROM master_table, mt2type, type_table WHERE tt_id=:my_run_type_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=mt_id", my_run_type_id=run_type_id)

       MCK_list=[]
       for x in c.fetchall():
          MCK_list.append(x[0])

       conn.close()
       #print MCK_list

       if len(MCK_list)>0:
         return MCK_list
       else:
         return 0                                                       


def makeActive(dbconnection, MCK, *vargs, **kwargs):
    if dbconnection=='oracle':
       import cx_Oracle
       import sys
       conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
       c = conn.cursor()

 
       # check that a certain MCK exists
       c.execute("SELECT mt_id FROM master_table")

       all_MCK = []
       for x in c.fetchall():
          #print x[0]
          all_MCK.append(x[0])
          #max_id = x[0]
       
       #print all_MCK

       # here we should check if such MCK number exists or if MCK number is max MCK number + 1 (we just cannot read here noncommitted numbers from the database. If new MCK number came, makeActive was called before we committed database with new MCK number..) 
       if MCK in all_MCK or MCK==max(all_MCK)+1:
          print "Such MCK really exists!"
       else:
          print "Check MCK number! It does not exist!"
          return 0

       # now we want to check if this MCK is already active. If yes then we don't create new entry in the table
       current_active_MCK=getActiveKeyMCK('oracle')
       if MCK==current_active_MCK:
          print "This MCK is already active!"
          return 0

       # get the last ID from the activeMCK_table:
       c.execute("SELECT max(activeMCK_id) FROM activeMCK_table")

       for x in c.fetchall():
           #print x[0]                                                                                                          
           maxActiveID = x[0]

       nextActiveID=maxActiveID+1
       c.execute("INSERT INTO activeMCK_table VALUES ("+str(nextActiveID)+", 'activeMCK', "+str(MCK)+")")       
       print "New Active MCK is written into activeMCK_table: ID = ", nextActiveID, ", MCK = ",MCK 
       conn.commit()
       conn.close()

       return 0

def printActiveMCKTable(dbconnection, *vargs, **kwargs):
    if dbconnection=='oracle':
       import cx_Oracle
       import sys
       conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
       c = conn.cursor()

       c.execute("SELECT * FROM activeMCK_table")

       activeMCK_id=[]
       sorted_activeMCK_id=[]
       activeMCK=[] 
       for x in c.fetchall():
           #print x 
           activeMCK_id.append(x[0])
           activeMCK.append(x[2])
       

       sorted_activeMCK_id=sorted(activeMCK_id) 

       for i,value in enumerate(sorted_activeMCK_id):
           for ii,val in enumerate(activeMCK_id):
              if val==value:
                print "MCK ID: ",val, " MCK: ", activeMCK[ii]
              else:
                continue

       return 0
