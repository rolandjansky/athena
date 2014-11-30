# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("readDictionary.py")
include("tools.py")
import hashlib
import sys

def upload(*vargs, **kwargs):

  # read dictionary with the following lists:
  ConfigNameDict=[]
  ConfigRunTypeDict=[]
  ConfigRecostepDict=[]
  ConfigCodeDict=[]
  ConfigCodeHashDict=[]
  ConfigScriptDict=[]
  ConfigScriptHashDict=[]
  ConfigNameMissingDict=[]
  ConfigTypeMissingDict=[]
  ConfigRecostepMissingDict=[]
  # here we get an array for different slices

  if readDictionary()==0:
    print "Something wrong with dictionaries. Check dictionaries!"
    #sys.exit("Something wrong with dictionaries. Check dictionaries!")
    return 0
  else:
    ConfigNameDict,ConfigRunTypeDict,ConfigRecostepDict,ConfigCodeDict,ConfigCodeHashDict,ConfigScriptDict,ConfigScriptHashDict=readDictionary()
  
  # for missing or empty dictionaries we assume that topOption will get monitoring config from release. Therefore we fill here "None" for these configs and scripts 
  
  # get active key MCK

  activeKeyMCK = getActiveKeyMCK('oracle')
  #print "active key MCK = ", activeKeyMCK
  
  # if OVERWRITE = 1 then we should create a new config entry in the DB or take the existing one but not the active one
  OVERWRITE=0
  # if EQUAL_CONFIG = 0 then config from dictionary is different than in the active key configuration
  EQUAL_CONFIG=[]
  index_list=[]
  dbConfigId_check=[]
  
  for index,conf_name in enumerate(ConfigNameDict):
      #print "#####################################"
      #print " dict slice, RunType ", conf_name, ConfigRunTypeDict[index]
      #print "config code from dict ", ConfigCodeDict[index]
      #print "dict hash ", ConfigCodeHashDict[index]

      # get active key conf and compare hash of config code:

      dbConfigId, dbConfigCode, dbConfigScript,dbConfigCodeHash,dbConfigScriptHash=getConfigCodeForMCK('oracle', activeKeyMCK, ConfigRunTypeDict[index], conf_name, ConfigRecostepDict[index])
  
      dbConfigId_check.append(dbConfigId)

      #print "name ", conf_name
      #print "code from DB ", dbConfigCode
      #print "dict hash ", ConfigCodeHashDict[index]
      #print "db hash ", dbConfigCodeHash

      if ConfigCodeHashDict[index]==dbConfigCodeHash:
         # check all parameters to be sure
         # print "equalConfigs = ", equalConfigs(ConfigCodeDict[index],dbConfigCode)
         if equalConfigs(ConfigCodeDict[index],dbConfigCode):
           EQUAL_CONFIG.append(1)
           index_list.append(index)           
           #OVERWRITE=0
         else:
           EQUAL_CONFIG.append(0)
           index_list.append(index)    
           OVERWRITE=1
      else:
         #print "This configuration from dictionary is new!"
         EQUAL_CONFIG.append(0) 
         index_list.append(index)
         OVERWRITE=1
         #break       

#  print "dbConfigId_check ", dbConfigId_check
  print "OVERWRITE ", OVERWRITE
#  print "slice ", ConfigNameDict
#  print "run type ", ConfigRunTypeDict
  print "EQUAL_CONFIG ", EQUAL_CONFIG
#  print "index list ", index_list

  active_MCK=0

# if OVERWRITE=1: write new configuration into the database: search for every config hash the entries in the DB for which there is the same hash, then compare them to be sure and connect to those configs; otherwise create new config IDs for new configs; for already equal configs take the existing configID connection 
# if OVERWRITE=0: leave DB like it is 

# in tools: search in DB for EQUAL_CONFIG = 0 configs with equal hash code, it can be several equal hash cosed, return array of them, then for every of them check if all parameters are equal. If all are equal take the last entry. If only one config found then take it. If none found then create a new config entry.
# create an array of sig_config IDs with rearranged numbers
# recreate connecting table type2sig
# create a new entry for MCK
# rewrite connecting table mt2type

  import cx_Oracle
  conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
  c = conn.cursor()
  
  checkPhysics=0
  checkStandby=0
  checkCosmics=0

  similar_config=[]

  physicsConfigID=[]
  cosmicsConfigID=[]
  standbyConfigID=[]
  
  CHECK_PHYSICS_LIST=[]
  CHECK_COSMICS_LIST=[]
  CHECK_STANDBY_LIST=[]

  physics_flags=[]
  cosmics_flags=[]
  standby_flags=[]  
  
  # flags for: if all EQUAL_CONFIG values for a certain type=1 with OVERWRITE=1 (that means that some type has to be rewritten) then we leave active type ID for this type

  physics_leave_active_type=0 
  cosmics_leave_active_type=0 
  standby_leave_active_type=0 


  if OVERWRITE==1:

   # now we need to check if we can take already existing type IDs for our configs
   # this should not happen if all flags of EQUAL_CONFIG=1 (this is not the case if OVERWRITE=1)
   # this should happen if for EQUAL_CONFIG=0 there is an equal config in the DB exists OR if EQUAL_CONFIG=1
   # introduce flag/function EQUAL_PHYSICS_TYPE, ... if there is a similar TYPE table in the DB. It returns the type_ID if exists and 0 if not. In the last case we create new entry for a certain type
   
    for i,flag in enumerate(EQUAL_CONFIG):
     if ConfigRunTypeDict[i]=='Physics':
        physics_flags.append(flag)
     elif ConfigRunTypeDict[i]=='Cosmics':
        cosmics_flags.append(flag)
     elif ConfigRunTypeDict[i]=='Standby':
        standby_flags.append(flag)

    if 0 in physics_flags:
      physics_leave_active_type=0
    else:
      physics_leave_active_type=1
    if 0 in cosmics_flags:
      cosmics_leave_active_type=0
    else:
      cosmics_leave_active_type=1
    if 0 in standby_flags:
      standby_leave_active_type=0
    else:
      standby_leave_active_type=1

    print "physics_leave_active_type = ", physics_leave_active_type
    print "cosmics_leave_active_type = ", cosmics_leave_active_type
    print "standby_leave_active_type = ", standby_leave_active_type

    print "physics_flags ", physics_flags
    print "cosmics_flags ", cosmics_flags
    print "standby_flags ", standby_flags

    existing_physics_type=0   
    existing_cosmics_type=0   
    existing_standby_type=0   

    if physics_leave_active_type:
        # here we should find out the ID of active Physics Type
        existing_physics_type = getTypeIDForMCK('oracle', activeKeyMCK, 'Physics')
    if standby_leave_active_type:
        # here we should find out the ID of active Standby Type
        existing_standby_type = getTypeIDForMCK('oracle', activeKeyMCK, 'Standby')
    if cosmics_leave_active_type:
        # here we should find out the ID of active Physics Type
        existing_cosmics_type = getTypeIDForMCK('oracle', activeKeyMCK, 'Cosmics')


    for i,flag in enumerate(EQUAL_CONFIG):
      similar_config.append(searchSimilarConfigsInDB('oracle', ConfigNameDict[i], ConfigRecostepDict[i], ConfigCodeDict[i], ConfigCodeHashDict[i]))

      if ConfigRunTypeDict[i]=='Physics':
        if ((flag==0 and similar_config[-1]!=0) or flag==1) and physics_leave_active_type==0:
          #CHECK_PHYSICS_LIST.append(1)
          if flag==0 and similar_config[-1] != 0:
            # now we need to find out which type IDs are connected to this config and check the size of this list of IDs (we check if it does not return 0)
            if getAvailableTypeIDsForGivenConfig('oracle', similar_config[-1], 'Physics'):
               physicsConfigID.append(similar_config[-1])
               CHECK_PHYSICS_LIST.append(1)
            else:
               CHECK_PHYSICS_LIST.append(0)
          elif flag==1:
            # we take number from active MCK
            # put config ID from active config 
            physicsConfigID.append(getConfigIdForActiveMCK('oracle', activeKeyMCK, 'Physics', ConfigNameDict[i], ConfigRecostepDict[i]))
            CHECK_PHYSICS_LIST.append(1)
        elif similar_config[-1]==0:
            CHECK_PHYSICS_LIST.append(0)

      if ConfigRunTypeDict[i]=='Standby':
        if ((flag==0 and similar_config[-1]!=0) or flag==1) and standby_leave_active_type==0:
          #CHECK_STANDBY_LIST.append(1)
          if flag==0 and similar_config[-1] != 0:
            # now we need to find out which type IDs are connected to this config and check the size of this list of IDs (we check if it does not return 0)
            if getAvailableTypeIDsForGivenConfig('oracle', similar_config[-1], 'Standby'):
               standbyConfigID.append(similar_config[-1])
               CHECK_STANDBY_LIST.append(1)
            else:
               CHECK_STANDBY_LIST.append(0)
          elif flag==1:
            # we take number from active MCK
            # put config ID from active config 
            standbyConfigID.append(getConfigIdForActiveMCK('oracle', activeKeyMCK, 'Standby', ConfigNameDict[i], ConfigRecostepDict[i]))
            CHECK_STANDBY_LIST.append(1)
        elif similar_config[-1]==0:
            CHECK_STANDBY_LIST.append(0)

      if ConfigRunTypeDict[i]=='Cosmics':
        if ((flag==0 and similar_config[-1]!=0) or flag==1) and cosmics_leave_active_type==0:
          #CHECK_COSMICS_LIST.append(1)
          if flag==0 and similar_config[-1] != 0:
            # now we need to find out which type IDs are connected to this config and check the size of this list of IDs (we check if it does not return 0)
            if getAvailableTypeIDsForGivenConfig('oracle', similar_config[-1], 'Cosmics'):
               cosmicsConfigID.append(similar_config[-1])
               CHECK_COSMICS_LIST.append(1)
            else:
               CHECK_COSMICS_LIST.append(0)
          elif flag==1:
            # we take number from active MCK
            # put config ID from active config 
            cosmicsConfigID.append(getConfigIdForActiveMCK('oracle', activeKeyMCK, 'Cosmics', ConfigNameDict[i], ConfigRecostepDict[i]))
            CHECK_COSMICS_LIST.append(1)
        elif similar_config[-1]==0:
            CHECK_COSMICS_LIST.append(0)

#######################################################
      # here we check if separates configs for a certain slice have the same values. If we will need later to create new entries in the DB we will have to write them with the same new ID..
      
    list_of_flag_list=[]
    list_of_similar_config_list=[]
    list_of_equal_config_list=[]
    list_of_hash_list=[]
    list_of_type_list=[]

    slice_list = ['TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','IDTRK','IDJPSI','CALORAW','CALOESD']
    for sl in slice_list:
      flag_list={}
      similar_config_list={}
      hash_list={}
      for i,flag in enumerate(EQUAL_CONFIG):
        if ConfigNameDict[i]==sl:
           if ConfigRunTypeDict[i]=='Physics':
              flag_list['Physics']=flag
              similar_config_list['Physics']=similar_config[i]
              hash_list['Physics']=ConfigCodeHashDict[i]
           elif ConfigRunTypeDict[i]=='Standby':
              flag_list['Standby']=flag
              similar_config_list['Standby']=similar_config[i]
              hash_list['Standby']=ConfigCodeHashDict[i]
           elif ConfigRunTypeDict[i]=='Cosmics':
              flag_list['Cosmics']=flag
              similar_config_list['Cosmics']=similar_config[i]
              hash_list['Cosmics']=ConfigCodeHashDict[i]

      list_of_type_list.append(flag_list.keys())
      list_of_flag_list.append(flag_list.values())
      list_of_similar_config_list.append(similar_config_list.values())
      list_of_hash_list.append(hash_list.values())

    # now search for equal configs from dictionaries 
    for ii in list_of_hash_list:
      equal_config_list={}
      if ii[0]==ii[1] and ii[0]!=ii[2]:
         equal_config_list=[1,1,0]
      elif ii[0]!=ii[1] and ii[0]==ii[2]:
         equal_config_list=[1,0,1]
      elif ii[0]!=ii[1] and ii[0]!=ii[2]:
         equal_config_list=[0,0,0]
      elif ii[0]==ii[1] and ii[0]==ii[2]:
         equal_config_list=[1,1,1]
      list_of_equal_config_list.append(equal_config_list)

    #print "list_of_type_list ",list_of_type_list
    #print "list_of_equal_config_list ", list_of_equal_config_list
    #print "list_of_flag_list", list_of_flag_list
    #print "list_of_similar_config_list", list_of_similar_config_list
    #print "list_of_hash_list", list_of_hash_list
    
    list_of_leave_same_config_id=[]
    for eqconfig_index,eqconfig_entry in enumerate(list_of_equal_config_list):
       leave_same_config_id=[]
       if 1 not in eqconfig_entry:
          for index,entry in enumerate(eqconfig_entry):
            #if list_of_flag_list[eqconfig_index][index]==0 and list_of_similar_config_list[eqconfig_index][index]==0:
            leave_same_config_id.append(0)
            #else:
            #   leave_same_config_id.append(0) 
       else:
          for index,entry in enumerate(eqconfig_entry):
            if entry==1 and list_of_flag_list[eqconfig_index][index]==0 and list_of_similar_config_list[eqconfig_index][index]==0:
               leave_same_config_id.append(1)
            else:
               leave_same_config_id.append(0)


       list_of_leave_same_config_id.append(leave_same_config_id)

     
    print "list_of_leave_same_config_id ", list_of_leave_same_config_id

    #we are going to create a list of config IDs...

    list_of_id_increment=[]
    list_of_repeated_id=[]
    for ii in list_of_leave_same_config_id:
      id_increment=[]
      repeated_id=[]
      if ii==[1,0,1]:
         id_increment=[0,0,1]
         repeated_id=[1,0,0]
      elif ii==[1,1,0]: 
         id_increment=[1,0,0]
         repeated_id=[0,1,0]
      elif ii==[0,1,1]:
         id_increment=[0,0,1]
         repeated_id=[0,1,0]
      elif ii==[1,1,1]:
         id_increment=[0,0,1]
         repeated_id=[1,1,0]
      else:
         id_increment=[0,0,0]
         repeated_id=[0,0,0]


      list_of_id_increment.append(id_increment)
      list_of_repeated_id.append(repeated_id)

    #print "list_of_id_increment before adding", list_of_id_increment
    #print "list_of_repeated_id ", list_of_repeated_id

      # now we should take into account increments for those configs which are not parts of equal dictionary configs (when entries in list_of_leave_same_config_id are 0 and flag = 0 and similar_configs=0)

  
    for ii,val in enumerate(list_of_leave_same_config_id):
        for ind,entry in enumerate(val):
          #print "entry ", entry
          #print "list_of_flag_list[ii][ind] ", list_of_flag_list[ii][ind]
          #print "list_of_similar_config_list[ii][ind] ",list_of_similar_config_list[ii][ind]
          if entry==0 and list_of_flag_list[ii][ind]==0 and list_of_similar_config_list[ii][ind]==0:            
              list_of_id_increment[ii][ind]=1

    #print "list_of_id_increment after adding", list_of_id_increment

    # now we need to sort list_of_id_increment  back according to  EQUAL_CONFIG

    list_of_id_increment_sorted=[]
    list_of_repeated_id_sorted=[]
    for i,flag in enumerate(EQUAL_CONFIG):    
       for ii,val in enumerate(list_of_id_increment):
           for tt,ttval in enumerate(val):
             if slice_list[ii]==ConfigNameDict[i] and list_of_type_list[ii][tt]==ConfigRunTypeDict[i]:
               list_of_id_increment_sorted.append(ttval)
       for ii,val in enumerate(list_of_repeated_id):
           for tt,ttval in enumerate(val):
             if slice_list[ii]==ConfigNameDict[i] and list_of_type_list[ii][tt]==ConfigRunTypeDict[i]:
               list_of_repeated_id_sorted.append(ttval)

    print "list_of_id_increment_sorted ",list_of_id_increment_sorted
    print "list_of_repeated_id_sorted ",list_of_repeated_id_sorted


###########################################################    

    print "CHECK_PHYSICS_LIST ", CHECK_PHYSICS_LIST
    print "CHECK_COSMICS_LIST ", CHECK_COSMICS_LIST
    print "CHECK_STANDBY_LIST ", CHECK_STANDBY_LIST

    if len(CHECK_PHYSICS_LIST)>0:
       if 0 in CHECK_PHYSICS_LIST:
         checkPhysics=0
       else:
         checkPhysics=1
    else:
       checkPhysics=0
    if len(CHECK_COSMICS_LIST)>0:        
       if 0 in CHECK_COSMICS_LIST:
         checkCosmics=0
       else:
         checkCosmics=1
    else:
       checkCosmics=0        
    if len(CHECK_STANDBY_LIST)>0:
       if 0 in CHECK_STANDBY_LIST:
         checkStandby=0
         #EQUAL_STANDBY_TYPE=0 
       else:
         checkStandby=1        
    else:
       checkStandby=0

    # now we want to check if there are similar whole types in DB available
   
    print "checkPhysics ",  checkPhysics
    print "checkCosmics ", checkCosmics
    print "checkStandby ", checkStandby

    #print "physicsConfigID ", physicsConfigID
    #print "cosmicsConfigID ", cosmicsConfigID
    #print "standbyConfigID ", standbyConfigID

    equal_cosmics_type=0
    equal_physics_type=0
    equal_standby_type=0

    if checkPhysics==1:
      #print "we are here!"
      existing_physics_type=searchSimilarTypeIdInDB('oracle', 'Physics',  physicsConfigID)
      if existing_physics_type:
        equal_physics_type=1
      else:
        equal_physics_type=0          

    if checkCosmics==1:
      #print "we are here!"
      existing_cosmics_type=searchSimilarTypeIdInDB('oracle', 'Cosmics',  cosmicsConfigID)
      if existing_cosmics_type:
         equal_cosmics_type=1
      else:
         equal_cosmics_type=0

    
    if checkStandby==1:
      #print "we are here!"
      existing_standby_type=searchSimilarTypeIdInDB('oracle', 'Standby',  standbyConfigID)
      if existing_standby_type:
         equal_standby_type=1
      else:
         equal_standby_type=0

    EQUAL_PHYSICS_TYPE=(physics_leave_active_type or equal_physics_type)
    EQUAL_STANDBY_TYPE=(standby_leave_active_type or equal_standby_type)
    EQUAL_COSMICS_TYPE=(cosmics_leave_active_type or equal_cosmics_type)
    # if 1 take existing_physics_type as a type ID

    print "EQUAL_PHYSICS_TYPE ", EQUAL_PHYSICS_TYPE
    print "EQUAL_STANDBY_TYPE ", EQUAL_STANDBY_TYPE
    print "EQUAL_COSMICS_TYPE ", EQUAL_COSMICS_TYPE

    physics_MCK_list = []
    cosmics_MCK_list = []
    standby_MCK_list = []

    getMCKforTypeID('oracle', existing_physics_type)

    take_existing_MCK=0
    existing_MCK_ID=0
    if EQUAL_PHYSICS_TYPE==1 and EQUAL_COSMICS_TYPE==1 and EQUAL_STANDBY_TYPE==1:
       
       physics_MCK_list=getMCKforTypeID('oracle', existing_physics_type)
       cosmics_MCK_list=getMCKforTypeID('oracle', existing_cosmics_type) 
       standby_MCK_list=getMCKforTypeID('oracle', existing_standby_type)
       print "physics_MCK_list ", physics_MCK_list
       print "cosmics_MCK_list ", cosmics_MCK_list
       print "standby_MCK_list ", standby_MCK_list

       # now we want to find if these 2 lists have the same MCK value (all 3 lists, not 2). Therefore we first find if 2 lists have the same value and then we search for this value in 3rd list
       duplicate_exists=0
       duplicates=[]
       for a1 in physics_MCK_list:
         if a1 in cosmics_MCK_list:
            duplicates.append(a1)
            duplicate_exists=1

       #print "duplicate_exists = ", duplicate_exists
       take_existing_MCK=0
       if duplicate_exists==1:
         for dup in duplicates:
           if dup in standby_MCK_list:
              take_existing_MCK=1
              existing_MCK_ID=dup



    if take_existing_MCK==1:
      #we make existing MCK as an active MCK. if there is such MCK available leave its number as an active combination (return MCK from upload)                
        active_MCK=existing_MCK_ID
        print "we take existing MCK number ", active_MCK
    else:
      # else create a new MCK

      new_MCK=getMaxMasterKey('oracle')+1
      active_MCK=new_MCK
      c.execute("INSERT INTO master_table VALUES ("+str(new_MCK)+", 'MCK', 'Monitoring Configuration Key')")

    if EQUAL_PHYSICS_TYPE==1 and take_existing_MCK==0:
       # rewrite conections 
       # otherwise (part down takes care of it)
       #print "HERE!!!! existing physics type..", existing_physics_type 
       c.execute("INSERT INTO mt2type VALUES ("+str(new_MCK)+","+str(existing_physics_type)+")")
    if EQUAL_COSMICS_TYPE==1 and take_existing_MCK==0: 
       #print "HERE!!!! existing cosmic type..", existing_cosmics_type 
       c.execute("INSERT INTO mt2type VALUES ("+str(new_MCK)+","+str(existing_cosmics_type)+")")
    if EQUAL_STANDBY_TYPE==1 and take_existing_MCK==0:
       #print "HERE!!!! existing standby type..", existing_standby_type 
       c.execute("INSERT INTO mt2type VALUES ("+str(new_MCK)+","+str(existing_standby_type)+")")
          

    #print "HERE!!!! physics type..", existing_physics_type 


    # find out which RunTypes have to be changed:
    maxTypeId=getMaxTypeId('oracle')

    # if these flags =1 then we should create new entries for these run types in the DB
    changePhysics=0
    changeStandby=0
    changeCosmics=0

    #for i,flag in enumerate(EQUAL_CONFIG):
      #if flag==0 and ConfigRunTypeDict[i]=='Physics' and EQUAL_PHYSICS_TYPE==0 and take_existing_MCK==0:
    if EQUAL_PHYSICS_TYPE==0 and take_existing_MCK==0:
       changePhysics=1
    if EQUAL_STANDBY_TYPE==0 and take_existing_MCK==0:
       changeStandby=1
    if EQUAL_COSMICS_TYPE==0 and take_existing_MCK==0:
       changeCosmics=1

    
    # assume that we have only unique entries in the config DB
    # if there are some entries with equal config for a certain type or if there are new entries for a certain type then we don't search for similar types in the DB
    # only if for a certain type there are all entries new and they exist in the DB we check if they belong to one existing type Id and we take it over. If this is the case for all types then we take over the whole MCK 
    # we fill flags checkPhysics, checkStandby, checkCosmics: if they are 1, we use them later..


    # if we need to create new RunType entries:
    # assign new Ids to type table entries
    if changePhysics==1:
       newPhysicsId=maxTypeId+1

    if changeStandby==1 and changePhysics==1: 
       newStandbyId=newPhysicsId+1
    elif changeStandby==1 and changePhysics==0:
       newStandbyId=maxTypeId+1

    #if changeCosmics==1 and changeStandby==1 and (changePhysics==0 or changePhysics==1):
    if changeCosmics==1 and changeStandby==1:
       newCosmicsId=newStandbyId+1
    elif changeCosmics==1 and changeStandby==0 and changePhysics==1:
       newCosmicsId=newPhysicsId+1
    elif changeCosmics==1 and changeStandby==0 and changePhysics==0:
       newCosmicsId=maxTypeId+1

    #print "new type Ids ", newPhysicsId, newStandbyId, newCosmicsId

    if changePhysics==1:
       c.execute("INSERT INTO type_table VALUES ("+str(newPhysicsId)+",'Physics')")
       c.execute("INSERT INTO mt2type VALUES ("+str(new_MCK)+","+str(newPhysicsId)+")")
    if changeStandby==1:
       c.execute("INSERT INTO type_table VALUES ("+str(newStandbyId)+",'Standby')")
       c.execute("INSERT INTO mt2type VALUES ("+str(new_MCK)+","+str(newStandbyId)+")")
    if changeCosmics==1:
       c.execute("INSERT INTO type_table VALUES ("+str(newCosmicsId)+",'Cosmics')")
       c.execute("INSERT INTO mt2type VALUES ("+str(new_MCK)+","+str(newCosmicsId)+")")

    sig_config_id = getMaxConfigId('oracle')
    #  now we produce a list of all config IDs
    list_of_config_id=[]
    for i in list_of_id_increment_sorted:
      if i==1:
        sig_config_id=sig_config_id+i
        list_of_config_id.append(sig_config_id)
      else:
        list_of_config_id.append(i)
  

    print "list of new config id before insertion of repetitions: ", list_of_config_id

    # loop over dictionaries 
    # here is the case when separate configs have to be changed
    # if changePhysics==0 or changeCosmics==0 or changeStandby==0
    # i.e. when we don't take at least one of the whole existing types from DB

    if changePhysics==1 or changeCosmics==1 or changeStandby==1:   
      for i,flag in enumerate(EQUAL_CONFIG):
        #similar_config=searchSimilarConfigsInDB('oracle', ConfigNameDict[i], ConfigRecostepDict[i], ConfigCodeDict[i], ConfigCodeHashDict[i])
        if flag==0 and similar_config[i]==0:
           # there are no similar configs in the db --> create new config entries
           # this case will go to sig_config table with the following IDs:
           config_str_=str(ConfigCodeDict[i])
           config_str=config_str_.replace("'",'"')
           #print "HERE!!! ", config_str
           script_str_ = str(ConfigScriptDict[i])
           script_str = script_str_.replace("'",'"')
           
           #sig_config_id=sig_config_id+1
           #print "here new config id ", sig_config_id
           if list_of_repeated_id_sorted[i]==0:           
             new_config_id=list_of_config_id[i]
             c.execute("INSERT INTO sig_config VALUES ("+str(new_config_id)+",'"+ConfigNameDict[i]+"','"+ConfigRecostepDict[i]+"','"+config_str+"','"+script_str+"','"+ConfigCodeHashDict[i]+"','"+ConfigScriptHashDict[i]+"')")
           elif list_of_repeated_id_sorted[i]==1 and list_of_repeated_id_sorted[i-1]==0 and list_of_id_increment_sorted==0: # if config is new but the same one as the previous one. Then we take previous id and don't create new entry, just reconnect..
             new_config_id=list_of_config_id[i-1]
           elif list_of_repeated_id_sorted[i]==1 and list_of_repeated_id_sorted[i-1]==1 and list_of_repeated_id_sorted[i-2]==0 and list_of_id_increment_sorted==0: # if config is new but the same one as the preprevious one (all 3 configs are the same). Then we take preprevious id and don't create new entry, just reconnect..
             new_config_id=list_of_config_id[i-2]

           if ConfigRunTypeDict[i]=='Physics' and changePhysics==1:
              c.execute("INSERT INTO type2sig VALUES ("+str(newPhysicsId)+","+str(new_config_id)+")")
           elif ConfigRunTypeDict[i]=='Standby' and changeStandby==1:
              c.execute("INSERT INTO type2sig VALUES ("+str(newStandbyId)+","+str(new_config_id)+")")
           elif ConfigRunTypeDict[i]=='Cosmics' and changeCosmics==1: 
              c.execute("INSERT INTO type2sig VALUES ("+str(newCosmicsId)+","+str(new_config_id)+")")
        elif flag==0 and similar_config[i]!=0:
           #that means that there are similar configs in the db, we need just create new connections if there are no whole Type tables similar to our new one existent. This can be only in the case if all configs in the dictionaries for a certain slice are already available in the DB
         
           if ConfigRunTypeDict[i]=='Physics' and changePhysics==1:
              c.execute("INSERT INTO type2sig VALUES ("+str(newPhysicsId)+","+str(similar_config[i])+")")
           elif ConfigRunTypeDict[i]=='Standby' and changeStandby==1:
              c.execute("INSERT INTO type2sig VALUES ("+str(newStandbyId)+","+str(similar_config[i])+")")
           elif ConfigRunTypeDict[i]=='Cosmics' and changeCosmics==1:
              c.execute("INSERT INTO type2sig VALUES ("+str(newCosmicsId)+","+str(similar_config[i])+")")
        elif flag==1:
           # these configurations didn't change in comparison to active key configurations
           # leave conf ID from active key ID
           # reconnect type table with sig_config table in the case if certain run type configuration is going to change 
           if changePhysics==1 and ConfigRunTypeDict[i]=='Physics':
              equal_config_id = getConfigIdForActiveMCK('oracle', activeKeyMCK, 'Physics', ConfigNameDict[i], ConfigRecostepDict[i])
              c.execute("INSERT INTO type2sig VALUES ("+str(newPhysicsId)+","+str(equal_config_id)+")")
           elif changeStandby==1 and ConfigRunTypeDict[i]=='Standby':
              equal_config_id = getConfigIdForActiveMCK('oracle', activeKeyMCK, 'Standby', ConfigNameDict[i], ConfigRecostepDict[i])
              c.execute("INSERT INTO type2sig VALUES ("+str(newStandbyId)+","+str(equal_config_id)+")")
           elif changeCosmics==1 and ConfigRunTypeDict[i]=='Cosmics':
              equal_config_id = getConfigIdForActiveMCK('oracle', activeKeyMCK, 'Cosmics', ConfigNameDict[i], ConfigRecostepDict[i])
              c.execute("INSERT INTO type2sig VALUES ("+str(newCosmicsId)+","+str(equal_config_id)+")")
  else:
     active_MCK=activeKeyMCK
     print "Configuration hasn't changed, we don't rewrite database entries"

  conn.commit()
  conn.close()
  
  return active_MCK

