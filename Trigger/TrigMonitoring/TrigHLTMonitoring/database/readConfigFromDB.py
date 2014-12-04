# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def readSliceConfigFromDB(dbconnection, MCK, runtype, slice_name, reco_step, *vargs, **kwargs):
  #this script returns config for a certain slice
  # MCK can be: any MCK number
  #             'ACTIVE_KEY'
  #             'LAST_MCK'      

  include("TrigHLTMonitoring/tools.py")

  if dbconnection=='oracle':
      import cx_Oracle
      import sys
      conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
      c = conn.cursor()


#      RunType=[]
#      ConfigId=[]
#      ConfigName=[]
#      ConfigRecostep=[]
#      ConfigCode=[]
#      ConfigCodeHash=[]
#      ConfigScript=[]
#      ConfigScriptHash=[]

      if MCK=='ACTIVE_KEY':
         current_MCK=getActiveKeyMCK('oracle')
      elif MCK=='LAST_MCK':
         current_MCK=getMaxMCK('oracle')
      else:
         current_MCK=MCK


      # we have to get CLOBS separately, when fetchall they get spoiled... known problem with cx_Oracle                                
      c.execute("SELECT sc_data FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK and type_table.tt_name=:my_runtype and sig_config.sc_name=:my_slice_name and sig_config.recostep=:my_reco_step", my_MCK=current_MCK,  my_runtype=runtype, my_slice_name=slice_name, my_reco_step=reco_step)

      for value, in c:
        #print "Doing some manipulation with value", value.read()                                                                      
        #ConfigCode.append(eval(str(value)))
        ConfigCode=eval(str(value))

      return ConfigCode


def readAllConfigFromDB(dbconnection, MCK, *vargs, **kwargs):
  #this script returns configs, run_type, slice_name, reco_step for all slices
  # MCK can be: any MCK number
  #             'ACTIVE_KEY'
  #             'LAST_MCK'      

  include("tools.py")

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

      if MCK=='ACTIVE_KEY':
         current_MCK=getActiveKeyMCK('oracle')
      elif MCK=='LAST_MCK':
         current_MCK=getMaxMCK('oracle')
      else:
         current_MCK=MCK


      # we have to get CLOBS separately, when fetchall they get spoiled... known problem with cx_Oracle                                

      c.execute("SELECT sc_data FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=current_MCK)

      for value, in c:
        #print "Doing some manipulation with value", value.read()                                                                      
        #ConfigCode.append(eval(str(value)))
        ConfigCode.append(eval(str(value)))

      c.execute("SELECT sc_script FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=current_MCK)


      for script_value, in c:
        #print "Doing some manipulation with value", value.read()                                                                      
        ConfigScript.append(str(script_value))


      c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=current_MCK)


      for x in c.fetchall():
         #print "Printing...", x                                                                                                       
         RunType.append(x[6])
         ConfigId.append(x[9])
         ConfigName.append(x[10])
         ConfigRecostep.append(x[11])
         #tkConfigCode.append(eval(str(x[12])))                                                                                        
         #print "here! ", type(x[12])                                                                                                  
         #ConfigCode.append(eval(str(value)))                                                                                          
         #ConfigScript.append(str(x[13]))                                                                                              
         ConfigCodeHash.append(x[14])
         ConfigScriptHash.append(x[15])


      return ConfigCode, RunType, ConfigName, ConfigRecostep









###################################################

#
#
#
#
#
#
#
#
#    runType=[]
#    configId=[]
#    configName=[]
#    configRecostep=[]
#    configCode=[]
#    configScript=[]
#    configCodeHash=[]
#    configScriptHash=[]
#
#      # we have to get CLOBS separately, when fetchall they get spoiled... known problem with cx_Oracle                                
#      c.execute("SELECT sc_data FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_conf\
#ig_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my\
#_MCK=MCK)
#
#      for value, in c:
#        #print "Doing some manipulation with value", value.read()                                                                      
#        ConfigCode.append(eval(str(value)))
#
#      c.execute("SELECT sc_script FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_co\
#nfig_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", \
#my_MCK=MCK)
#
#      for script_value, in c:
#        #print "Doing some manipulation with value", value.read()                                                                      
#        ConfigScript.append(str(script_value))
#
#
#      c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE mt_id=:my_MCK and type2sig.sig_config_id=\
#sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.type_id1=type_table.tt_id and mt2type.master_id=:my_MCK", my_MCK=M\
#CK)
#
#
#
#
#    runType,configId,configName,configRecostep,configCode,configScript,configCodeHash,configScriptHash=getConfigForMCK('oracle', MCK) \
#
#    for index,conf_name in enumerate(configName):
#       if runType[index]==runtype and conf_name==slice_name and configRecostep[index]==reco_step:
#          return configId[index], configCode[index], configScript[index],configCodeHash[index],configScriptHash[index]
#       #else:                                                                                                                          
#       #   print "No such configuration in the active key configuration!"         
#



#def readConfigFromDB(dbconnection, masterkey, runtype, slice_name, reco_step, *vargs, **kwargs):
##   if dbconnection=='sqlite':
##       import sqlite3
##       conn = sqlite3.connect('test.db')
#   if dbconnection=='oracle':
#       import sys
#       import cx_Oracle
#       conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
#       c = conn.cursor()
#       c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE type_table.tt_name=:my_runtype and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.master_id=:my_masterkey and mt2type.type_id1=type_table.tt_id and sig_config.sc_name=:my_slice_name and sig_config.recostep=:my_reco_step",my_runtype=runtype, my_masterkey=masterkey, my_slice_name=slice_name, my_reco_step=reco_step)
#       #c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE type_table.tt_name=? and type2sig.sig_config_id=sig_config.sc_id and type2sig.type_id2=type_table.tt_id and mt2type.master_id=? and mt2type.type_id1=type_table.tt_id and sig_config.sc_name=? and sig_config.recostep=?", (runtype, masterkey, slice_name, reco_step))
#       #c.execute("SELECT * FROM type2sig")
#       #print "la la ", runtype
#       #c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE type_table.tt_name=:lala",lala=runtype)
#       #c.execute("SELECT * FROM master_table, mt2type, type_table, type2sig, sig_config WHERE type_table.tt_name='Physics'")
#       #ff = c.fetchall()
#       #print "test ", ff
#
#       config_list_of_dict = []
#
#       config_list = c.fetchall()
#
#       if len(config_list)==0:
#         print "No entry for this slice!"
#         return 0
#       else:
#         for x in config_list:
#           print "Printing...", x
#           stringConfig=str(x[12]) # this is string, not dictionary 
#           dictConfig=eval(stringConfig)  # convert string to dictionary
#           # exec montoolconfigCode
#           print "All configuration info in the database for slice ", slice_name, " is the following:\n",dictConfig
#           #        print "length.. ", len(montool_config)
#           config_list_of_dict.append(dictConfig)
#         return config_list_of_dict 
#         
#       conn.close()
