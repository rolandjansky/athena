# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("readDatabase.py")
include("readDictionary.py")

# read dictionary

ConfigName=[]
ConfigRecostep=[]
MontoolConfigCode=[]
#MontoolConfigScript=[]

ConfigName,ConfigRecostep,MontoolConfigCode=readDictionary()

newIdList = []

for index,conf_name in enumerate(ConfigName):
    dbMaxConfigID,dbMontool_config_triggers,dbMontool_config_config,dbMontool_config_pkgname,dbMontool_config_creatorname,dbMontoolconfigScript=readDatabase('sqlite',1,'Physics',conf_name,ConfigRecostep[index])
    print "from dictionary: ", MontoolConfigCode[index]
    print "from DB: ", dbMontool_config_config

    # compare values of dictionary and DB!

    newID=0
    for (toolname, toolconfig) in MontoolConfigCode[index]:
        for (dbtoolname, dbtoolconfig) in dbMontool_config_config:
            if dbtoolname==toolname:
                for (confattr, confvalue) in toolconfig.items(): 
                    if confattr not in dbtoolconfig:
                        print "Wrong attribute name!"
                    else:
                        if dbtoolconfig[confattr]==confvalue:
                            continue
                        else:
                            # 1 if the configuration for a certain slice has changed 
                            newID=1
                            break
            if newID!=0:
                break
        if newID!=0:
            break

    #print  "new ID for ",conf_name,newID
    newIdList.append(newID)


# now we want to open database, get maxID, and write there new values with next IDs

import sqlite3 
conn = sqlite3.connect('test.db')                                                                                                                                                                    
c = conn.cursor()    

dbConfigIDs=[]
c.execute("SELECT * FROM sig_config")
for x in c.fetchall():
  #print "ID...", x[0]                                                                                                                
  dbConfigIDs.append(x[0])

oldMaxID=max(dbConfigIDs)
#print "max ID", oldMaxID

print newIdList
newDB_ID_List=[]
for i in newIdList:
  oldMaxID=newID*(oldMaxID+1)
  newDB_ID_List.append(oldMaxID)

print newDB_ID_List

newDB_ConfigID=[]
newDB_ConfigName=[]
newDB_ConfigRecostep=[]
newDB_MontoolConfigCode=[]

config_string_list=[]

for index,value in enumerate(newIdList):
    if value!=0:
        # rewrite for this slice the DB values with dictionary values
        newDB_ConfigID.append(newDB_ID_List[index])
        newDB_ConfigName.append(ConfigName[index])
        newDB_ConfigRecostep.append(ConfigRecostep[index])
        dbMaxConfigID,dbMontool_config_triggers,dbMontool_config_config,dbMontool_config_pkgname,dbMontool_config_creatorname,dbMontoolconfigScript=readDatabase('sqlite',1,'Physics',newDB_ConfigName[-1],newDB_ConfigRecostep[-1])
        #print "Triggers ", dbMontool_config_triggers
        for (toolname, toolconfig) in MontoolConfigCode[index]:
           #print "toolname", toolname 
           #print "Script ", dbMontoolconfigScript
           for (dbtoolname, dbtoolconfig) in dbMontool_config_config:            
            if dbtoolname==toolname:
               for (confattr, confvalue) in toolconfig.items():
                 if confattr not in dbtoolconfig:
                     print "Wrong attribute name!"
                 else:
                     dbtoolconfig[confattr]=confvalue
                             
        

        #print "new DB values: ", dbMontool_config_config  

        newDB_MontoolConfigCode.append(dbMontool_config_config)

        trig_str=str(dbMontool_config_triggers)
        trig_str=trig_str.replace("'",'"')
        config_str=str(newDB_MontoolConfigCode[-1])
        config_str=config_str.replace("'",'"')


        newDB_Config = """                                                                                                                                           
montool_config[\""""+ConfigName[index]+"""\"] = {}
montool_config[\""""+ConfigName[index]+"""\"]["MonitoringTriggers"] = """+trig_str+"""
montool_config[\""""+ConfigName[index]+"""\"]["Config"] = []
montool_config[\""""+ConfigName[index]+"""\"]["Config"] += """+config_str+"""
montool_config[\""""+ConfigName[index]+"""\"]["PkgName"]=\""""+str(dbMontool_config_pkgname)+"""\"
montool_config[\""""+ConfigName[index]+"""\"]["CreatorName"] =\""""+str(dbMontool_config_creatorname)+"""\"
        """      

        print "newDB_Config ",newDB_Config
        config_string_list.append(newDB_Config)

        c.execute("INSERT INTO sig_config VALUES ("+str(newDB_ID_List[index])+",'"+newDB_ConfigName[-1]+"','"+newDB_ConfigRecostep[-1]+"','"+newDB_Config+"','"+dbMontoolconfigScript+"')")
        c.execute("INSERT INTO type2sig VALUES (1,"+str(newDB_ID_List[index])+")")
        c.execute("INSERT INTO type2sig VALUES (2,"+str(newDB_ID_List[index])+")")
        c.execute("INSERT INTO type2sig VALUES (3,"+str(newDB_ID_List[index])+")")

conn.commit()
conn.close()

